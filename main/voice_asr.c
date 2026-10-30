#include "voice_asr.h"
#include "voice_asr_config.h"
#include "cJSON.h"
#include "esp_crt_bundle.h"
#include "esp_heap_caps.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *TAG = "ASR";

#if VOICE_ASR_PROVIDER == VOICE_ASR_AISPEECH
static bool aispeech_proxy(const uint8_t *wav, size_t wav_len, char *out, size_t cap)
{
    if (!VOICE_ASR_PROXY_URL[0] || !VOICE_ASR_DEVICE_TOKEN[0]) {
        ESP_LOGW(TAG, "AISpeech proxy not configured; see voice_asr_local.example.h");
        return false;
    }
    bool ok = false;
    char auth[512];
    char *resp = NULL;
    cJSON *json = NULL;
    size_t sent = 0, received = 0;
    const size_t resp_cap = 8192;
    int auth_len = snprintf(auth, sizeof(auth), "Bearer %s", VOICE_ASR_DEVICE_TOKEN);
    if (auth_len < 0 || (size_t)auth_len >= sizeof(auth)) return false;
    esp_http_client_config_t cfg = {
        .url = VOICE_ASR_PROXY_URL,
        .method = HTTP_METHOD_POST,
        .timeout_ms = VOICE_ASR_PROXY_TIMEOUT_MS,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .buffer_size = 4096,
        .buffer_size_tx = 1024,
    };
    esp_http_client_handle_t client = esp_http_client_init(&cfg);
    if (!client) return false;
    resp = heap_caps_malloc(resp_cap, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!resp) goto done;
    esp_http_client_set_header(client, "Authorization", auth);
    esp_http_client_set_header(client, "Content-Type", "audio/wav");
    if (esp_http_client_open(client, (int)wav_len) != ESP_OK) goto done;
    while (sent < wav_len) {
        size_t chunk = wav_len - sent;
        if (chunk > 4096) chunk = 4096;
        int n = esp_http_client_write(client, (const char *)wav + sent, (int)chunk);
        if (n <= 0) goto done;
        sent += (size_t)n;
    }
    int64_t length = esp_http_client_fetch_headers(client);
    if (length < 0 || length >= (int64_t)resp_cap) goto done;
    int status = esp_http_client_get_status_code(client);
    if (status != 200) {
        /* Do not log URLs, authorization headers, or upstream response bodies. */
        ESP_LOGW(TAG, "AISpeech proxy HTTP %d; run scripts/test_aispeech_asr.py for details", status);
        goto done;
    }
    while (received < resp_cap - 1) {
        int n = esp_http_client_read(client, resp + received, (int)(resp_cap - 1 - received));
        if (n < 0) goto done;
        if (n == 0) break;
        received += (size_t)n;
    }
    if (received == resp_cap - 1 || !esp_http_client_is_complete_data_received(client)) goto done;
    resp[received] = '\0';
    json = cJSON_Parse(resp);
    const cJSON *text = cJSON_GetObjectItemCaseSensitive(json, "text");
    const cJSON *provider = cJSON_GetObjectItemCaseSensitive(json, "provider");
    if (!cJSON_IsString(text) || !text->valuestring[0] || !cJSON_IsString(provider) ||
        strcmp(provider->valuestring, "aispeech") != 0) goto done;
    size_t bytes = strlen(text->valuestring);
    if (bytes >= cap) goto done; /* Never cut in the middle of a UTF-8 character. */
    memcpy(out, text->valuestring, bytes + 1);
    ok = true;
done:
    cJSON_Delete(json);
    free(resp);
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
    return ok;
}
#endif

bool voice_asr_recognize(const uint8_t *wav, size_t wav_len, char *out, size_t cap,
                         voice_asr_backend_t qwen_backend)
{
    if (!out || !cap) return false;
    out[0] = '\0';
    if (!wav || !wav_len || !qwen_backend) return false;
#if VOICE_ASR_PROVIDER == VOICE_ASR_AISPEECH
    ESP_LOGI(TAG, "primary=aispeech fallback=%d", VOICE_ASR_ALLOW_QWEN_FALLBACK);
    if (aispeech_proxy(wav, wav_len, out, cap)) {
        ESP_LOGI(TAG, "success provider=aispeech");
        return true;
    }
    out[0] = '\0';
    ESP_LOGW(TAG, "AISpeech failed");
    if (!VOICE_ASR_ALLOW_QWEN_FALLBACK) return false;
    ESP_LOGW(TAG, "fallback provider=qwen (same recording)");
#else
    ESP_LOGI(TAG, "primary=qwen");
#endif
    bool ok = qwen_backend(wav, wav_len, out, cap);
    if (!ok || !out[0]) {
        out[0] = '\0';
        return false;
    }
    ESP_LOGI(TAG, "success provider=qwen");
    return true;
}
