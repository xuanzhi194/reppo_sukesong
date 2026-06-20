#include "ai_chat.h"
#include "esp_http_client.h"
#include "esp_crt_bundle.h"
#include "esp_heap_caps.h"
#include "cJSON.h"
#include "esp_log.h"
#include <string.h>
#include <stdlib.h>

static const char *TAG = "AI_CHAT";

/* esp_http_client + lwIP DNS/TLS 需要内部 DRAM；不足时提前失败，避免 LoadProhibited */
#define AI_HTTP_MIN_INTERNAL_HEAP  (16 * 1024)

static bool ai_chat_internal_heap_ok(char *response_buf, size_t buf_len)
{
    size_t free_internal = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    if (free_internal >= AI_HTTP_MIN_INTERNAL_HEAP) {
        return true;
    }
    ESP_LOGE(TAG, "Internal heap too low for HTTP: %u B (need %d B)",
             (unsigned)free_internal, AI_HTTP_MIN_INTERNAL_HEAP);
    snprintf(response_buf, buf_len,
             "Internal memory low (%u B), retry later", (unsigned)free_internal);
    return false;
}

/* 调试开关：1=跳过 HTTPS 证书校验（不安全，仅调试）；0=严格校验（默认） */
#ifndef AI_CHAT_INSECURE_SKIP_TLS_VERIFY
#define AI_CHAT_INSECURE_SKIP_TLS_VERIFY 0
#endif



static char s_api_url[192];
static char s_api_key[128];
static char s_model[64];
static bool s_use_proxy;

static void normalize_api_url(const char *in_url, char *out_url, size_t out_len)
{
    const char *default_url = AI_DEFAULT_URL;
    const char *url = (in_url && in_url[0]) ? in_url : default_url;
    const char *suffix_no_slash = "chat/completions";
    const char *suffix_with_slash = "/chat/completions";

    // If caller already provides full endpoint, use it as-is.
    if (strstr(url, "/chat/completions") != NULL) {
        strncpy(out_url, url, out_len - 1);
        out_url[out_len - 1] = '\0';
        return;
    }

    // Caller provided a base_url (e.g. https://api.deepseek.com or .../v1), append endpoint path.
    strncpy(out_url, url, out_len - 1);
    out_url[out_len - 1] = '\0';
    size_t n = strlen(out_url);

    if (n > 0 && out_url[n - 1] == '/') {
        strncat(out_url, suffix_no_slash, out_len - strlen(out_url) - 1);
    } else {
        strncat(out_url, suffix_with_slash, out_len - strlen(out_url) - 1);
    }
}

/* HTTP 事件回调：将响应体追加到缓冲区 */
typedef struct {
    char *buf;
    int   len;
    int   max_len;
    bool  truncated;
} http_resp_t;

static esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
    http_resp_t *resp = (http_resp_t *)evt->user_data;
    if (evt->event_id == HTTP_EVENT_ON_DATA && resp) {
        int avail = resp->max_len - resp->len - 1;
        int copy  = (evt->data_len < avail) ? evt->data_len : avail;
        if (evt->data_len > avail) {
            resp->truncated = true;
        }
        if (copy > 0) {
            memcpy(resp->buf + resp->len, evt->data, copy);
            resp->len += copy;
            resp->buf[resp->len] = '\0';
        }
    }
    return ESP_OK;
}

esp_err_t ai_chat_init(const char *api_url, const char *api_key, const char *model)
{
    s_use_proxy = (api_url && (strncmp(api_url, "http://", 7) == 0 ||
                               strncmp(api_url, "https://", 8) == 0 ||
                               strstr(api_url, "/v1/ai/summary") != NULL));
    if (s_use_proxy) {
        strncpy(s_api_url, api_url, sizeof(s_api_url) - 1);
        s_api_url[sizeof(s_api_url) - 1] = '\0';
    } else {
        normalize_api_url(api_url, s_api_url, sizeof(s_api_url));
    }
    strncpy(s_api_key, api_key  ? api_key  : "",               sizeof(s_api_key)  - 1);
    strncpy(s_model,   model    ? model    : AI_DEFAULT_MODEL,  sizeof(s_model)    - 1);
    s_api_key[sizeof(s_api_key) - 1] = '\0';
    s_model[sizeof(s_model) - 1]     = '\0';
    ESP_LOGI(TAG, "Initialized. proxy=%d URL=%s Model=%s", (int)s_use_proxy, s_api_url, s_model);
    return ESP_OK;
}

/* 从 chat/completions 响应 message 中提取可读文本 */
static bool extract_message_text(cJSON *message, char *out, size_t out_len)
{
    if (!message || !out || out_len == 0) {
        return false;
    }

    cJSON *content = cJSON_GetObjectItem(message, "content");
    if (cJSON_IsString(content) && content->valuestring[0] != '\0') {
        strncpy(out, content->valuestring, out_len - 1);
        out[out_len - 1] = '\0';
        return true;
    }

    /* deepseek-v4 thinking 模式可能把正文放在 reasoning_content，content 为空 */
    cJSON *reasoning = cJSON_GetObjectItem(message, "reasoning_content");
    if (cJSON_IsString(reasoning) && reasoning->valuestring[0] != '\0') {
        strncpy(out, reasoning->valuestring, out_len - 1);
        out[out_len - 1] = '\0';
        return true;
    }

    if (cJSON_IsString(content)) {
        out[0] = '\0';
        return false;
    }

    return false;
}

static esp_err_t ai_chat_send_proxy(const char *user_message, char *response_buf, size_t buf_len)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "prompt", user_message);
    cJSON_AddStringToObject(root, "model", s_model);

    char *req_body = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    if (!req_body) {
        return ESP_ERR_NO_MEM;
    }
    if (!ai_chat_internal_heap_ok(response_buf, buf_len)) {
        free(req_body);
        return ESP_ERR_NO_MEM;
    }

    char *http_buf = (char *)heap_caps_malloc(AI_RESPONSE_BUF_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!http_buf) {
        http_buf = (char *)malloc(AI_RESPONSE_BUF_SIZE);
    }
    if (!http_buf) {
        free(req_body);
        return ESP_ERR_NO_MEM;
    }
    memset(http_buf, 0, AI_RESPONSE_BUF_SIZE);

    http_resp_t resp = {
        .buf     = http_buf,
        .len     = 0,
        .max_len = AI_RESPONSE_BUF_SIZE,
        .truncated = false,
    };

    char auth_header[160];
    snprintf(auth_header, sizeof(auth_header), "Bearer %s", s_api_key);

    bool use_tls = (strncmp(s_api_url, "https://", 8) == 0);

    esp_http_client_config_t config = {
        .url            = s_api_url,
        .method         = HTTP_METHOD_POST,
        .transport_type = use_tls ? HTTP_TRANSPORT_OVER_SSL : HTTP_TRANSPORT_OVER_TCP,
        .event_handler  = http_event_handler,
        .user_data      = &resp,
        .timeout_ms     = AI_API_TIMEOUT_MS,
        .buffer_size    = 1024,
        .buffer_size_tx = 1024,
    };
    if (use_tls) {
        config.tls_version = ESP_HTTP_CLIENT_TLS_VER_TLS_1_2;
#if AI_CHAT_INSECURE_SKIP_TLS_VERIFY
        config.skip_cert_common_name_check = true;
#else
        config.crt_bundle_attach = esp_crt_bundle_attach;
#endif
    }

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_header(client, "Authorization", auth_header);
    esp_http_client_set_post_field(client, req_body, (int)strlen(req_body));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK && resp.truncated) {
        ESP_LOGE(TAG, "HTTP response truncated at %d bytes", resp.len);
        snprintf(response_buf, buf_len, "Response truncated (max %d B)", AI_RESPONSE_BUF_SIZE);
        err = ESP_ERR_INVALID_SIZE;
    }
    if (err == ESP_OK) {
        int status = esp_http_client_get_status_code(client);
        ESP_LOGI(TAG, "HTTP status=%d, body_len=%d", status, resp.len);
        if (status == 200) {
            cJSON *json = cJSON_Parse(http_buf);
            if (json) {
                cJSON *content = cJSON_GetObjectItem(json, "content");
                cJSON *error   = cJSON_GetObjectItem(json, "error");
                if (cJSON_IsString(content) && content->valuestring[0] != '\0') {
                    strncpy(response_buf, content->valuestring, buf_len - 1);
                    response_buf[buf_len - 1] = '\0';
                    err = ESP_OK;
                } else if (cJSON_IsString(error)) {
                    snprintf(response_buf, buf_len, "Proxy error: %s", error->valuestring);
                    err = ESP_FAIL;
                } else {
                    snprintf(response_buf, buf_len, "[Proxy parse error]");
                    err = ESP_FAIL;
                }
                cJSON_Delete(json);
            } else {
                snprintf(response_buf, buf_len, "[JSON parse failed]");
                err = ESP_FAIL;
            }
        } else {
            snprintf(response_buf, buf_len, "HTTP error %d:\n%.200s", status, http_buf);
            err = ESP_FAIL;
        }
    } else if (err != ESP_ERR_INVALID_SIZE) {
        ESP_LOGE(TAG, "HTTP perform failed: %s", esp_err_to_name(err));
        snprintf(response_buf, buf_len, "Network error:\n%s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    free(req_body);
    free(http_buf);
    return err;
}

esp_err_t ai_chat_send(const char *user_message, char *response_buf, size_t buf_len)
{
    if (!user_message || !response_buf || buf_len == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    if (s_use_proxy) {
        return ai_chat_send_proxy(user_message, response_buf, buf_len);
    }

    /* 构造请求 JSON body — 直连 DeepSeek */
    cJSON *root     = cJSON_CreateObject();
    cJSON *messages = cJSON_AddArrayToObject(root, "messages");
    cJSON *msg      = cJSON_CreateObject();
    cJSON_AddStringToObject(msg, "role",    "user");
    cJSON_AddStringToObject(msg, "content", user_message);
    cJSON_AddItemToArray(messages, msg);
    cJSON_AddStringToObject(root, "model",      s_model);
    cJSON_AddNumberToObject(root, "max_tokens", AI_DEFAULT_MAX_TOKENS);
    cJSON_AddFalseToObject(root, "stream");
    /* deepseek-v4 默认 thinking 会导致 content 为空，分类结果无法解析 */
    {
        cJSON *thinking = cJSON_AddObjectToObject(root, "thinking");
        cJSON_AddStringToObject(thinking, "type", "disabled");
    }

    char *req_body = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    if (!req_body) {
        return ESP_ERR_NO_MEM;
    }
    if (!ai_chat_internal_heap_ok(response_buf, buf_len)) {
        free(req_body);
        return ESP_ERR_NO_MEM;
    }

    /* 分配 HTTP 响应缓冲区 */
    char *http_buf = (char *)heap_caps_malloc(AI_RESPONSE_BUF_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!http_buf) {
        http_buf = (char *)malloc(AI_RESPONSE_BUF_SIZE);
    }
    if (!http_buf) {
        free(req_body);
        return ESP_ERR_NO_MEM;
    }
    memset(http_buf, 0, AI_RESPONSE_BUF_SIZE);

    http_resp_t resp = {
        .buf     = http_buf,
        .len     = 0,
        .max_len = AI_RESPONSE_BUF_SIZE,
        .truncated = false,
    };

    /* 构造 Authorization 头 */
    char auth_header[160];
    snprintf(auth_header, sizeof(auth_header), "Bearer %s", s_api_key);

    /* 配置 HTTP 客户端（默认严格校验；可切换为不安全调试模式） */
    esp_http_client_config_t config = {
        .url                = s_api_url,
        .method             = HTTP_METHOD_POST,
        .transport_type     = HTTP_TRANSPORT_OVER_SSL,
        .tls_version        = ESP_HTTP_CLIENT_TLS_VER_TLS_1_2,
#if AI_CHAT_INSECURE_SKIP_TLS_VERIFY
        .skip_cert_common_name_check = true,
#else
        .crt_bundle_attach  = esp_crt_bundle_attach,
#endif
        .event_handler      = http_event_handler,
        .user_data          = &resp,
        .timeout_ms         = AI_API_TIMEOUT_MS,
        .buffer_size        = 1024,
        .buffer_size_tx     = 1024,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
#if AI_CHAT_INSECURE_SKIP_TLS_VERIFY
    ESP_LOGW(TAG, "TLS verification is DISABLED for debug (AI_CHAT_INSECURE_SKIP_TLS_VERIFY=1)");
#endif
    esp_http_client_set_header(client, "Content-Type",  "application/json");
    esp_http_client_set_header(client, "Authorization", auth_header);
    esp_http_client_set_post_field(client, req_body, (int)strlen(req_body));

    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK && resp.truncated) {
        ESP_LOGE(TAG, "HTTP response truncated at %d bytes", resp.len);
        snprintf(response_buf, buf_len, "Response truncated (max %d B)", AI_RESPONSE_BUF_SIZE);
        err = ESP_ERR_INVALID_SIZE;
    }

    if (err == ESP_OK) {
        int status = esp_http_client_get_status_code(client);
        ESP_LOGI(TAG, "HTTP status=%d, body_len=%d", status, resp.len);

        if (status == 200) {
            /* 解析 JSON 响应，提取 choices[0].message.content */
            cJSON *json = cJSON_Parse(http_buf);
            if (json) {
                cJSON *choices = cJSON_GetObjectItem(json, "choices");
                if (cJSON_IsArray(choices)) {
                    cJSON *choice0  = cJSON_GetArrayItem(choices, 0);
                    cJSON *message  = choice0  ? cJSON_GetObjectItem(choice0,  "message") : NULL;
                    if (extract_message_text(message, response_buf, buf_len)) {
                        err = ESP_OK;
                    } else {
                        snprintf(response_buf, buf_len, "[Parse error: empty AI message]");
                        ESP_LOGW(TAG, "Empty content/reasoning in response (first 200): %.200s", http_buf);
                        err = ESP_FAIL;
                    }
                } else {
                    /* 尝试提取错误信息 */
                    cJSON *error = cJSON_GetObjectItem(json, "error");
                    cJSON *emsg  = error ? cJSON_GetObjectItem(error, "message") : NULL;
                    if (cJSON_IsString(emsg)) {
                        snprintf(response_buf, buf_len, "API error: %s", emsg->valuestring);
                    } else {
                        snprintf(response_buf, buf_len, "[Unexpected response format]");
                    }
                    err = ESP_FAIL;
                }
                cJSON_Delete(json);
            } else {
                snprintf(response_buf, buf_len, "[JSON parse failed]");
                err = ESP_FAIL;
            }
        } else {
            snprintf(response_buf, buf_len, "HTTP error %d:\n%.200s", status, http_buf);
            err = ESP_FAIL;
        }
    } else if (err != ESP_ERR_INVALID_SIZE) {
        ESP_LOGE(TAG, "HTTP perform failed: %s", esp_err_to_name(err));
        snprintf(response_buf, buf_len, "Network error:\n%s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    free(req_body);
    free(http_buf);
    return err;
}
