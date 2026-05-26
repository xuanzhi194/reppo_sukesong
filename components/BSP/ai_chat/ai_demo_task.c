#include "ai_demo_task.h"
#include "ai_chat_secrets.h"

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ai_chat.h"
#include "wifi.h"
#include "esp_timer.h"
#include "esp_heap_caps.h"
#include "nvs_flash.h"
#include "nvs.h"

#define AI_NVS_NAMESPACE   "AIData"
#define AI_NVS_KEY_DAILY   "daily_res"   /* 每日总结 NVS key */
#define AI_NVS_KEY_WEEKLY  "week_res"    /* 每周总结 NVS key */

#define DAILY_MAX_TASKS    25   /* 每日总结最多取 N 条当天任务 */
#define WEEKLY_MAX_TASKS   80   /* 每周总结最多取 N 条近7天任务 */

/* 每日/每周全局 AI 结果，UI 可直接读取 */
ai_result_t g_ai_daily_result  = {0};
ai_result_t g_ai_weekly_result = {0};

/* AI 结果就绪标志，解析成功后置 1，UI 读取后由主循环清 0 */
volatile bool g_ai_daily_result_ready  = false;
volatile bool g_ai_weekly_result_ready = false;

/* 从 response 字符串解析 AI 结果 */
static void ai_result_parse(const char *response, ai_result_t *result)
{
    memset(result, 0, sizeof(ai_result_t));
    char line[300];
    const char *p = response;
    while (p && *p) {
        const char *end = strchr(p, '\n');
        size_t len = end ? (size_t)(end - p) : strlen(p);
        if (len >= sizeof(line)) len = sizeof(line) - 1;
        memcpy(line, p, len);
        /* 去掉行尾 \r 和空格 */
        while (len > 0 && (line[len-1] == '\r' || line[len-1] == ' ')) len--;
        line[len] = '\0';

        if (strncmp(line, "CAT1|", 5) == 0) {
            char *tok = strtok(line + 5, "|");
            if (tok) { strncpy(result->cat1_name, tok, sizeof(result->cat1_name)-1); tok = strtok(NULL, "|"); }
            if (tok) { result->cat1_percent  = atoi(tok); }
        } else if (strncmp(line, "CAT2|", 5) == 0) {
            char *tok = strtok(line + 5, "|");
            if (tok) { strncpy(result->cat2_name, tok, sizeof(result->cat2_name)-1); tok = strtok(NULL, "|"); }
            if (tok) { result->cat2_percent  = atoi(tok); }
        } else if (strncmp(line, "CAT3|", 5) == 0) {
            char *tok = strtok(line + 5, "|");
            if (tok) { strncpy(result->cat3_name, tok, sizeof(result->cat3_name)-1); tok = strtok(NULL, "|"); }
            if (tok) { result->cat3_percent  = atoi(tok); }
        } else if (strncmp(line, "SUMMARY|", 8) == 0) {
            strncpy(result->summary, line + 8, sizeof(result->summary)-1);
        } else if (strncmp(line, "QUOTE|", 6) == 0) {
            strncpy(result->quote, line + 6, sizeof(result->quote)-1);
        }
        p = end ? end + 1 : NULL;
    }
    result->valid = (result->cat1_name[0] != '\0');
}

/* 将 AI 结果保存到 NVS（按类型使用不同 key） */
static void ai_result_save(ai_summary_type_t type, const ai_result_t *result)
{
    const char *key = (type == AI_SUMMARY_DAILY) ? AI_NVS_KEY_DAILY : AI_NVS_KEY_WEEKLY;
    nvs_handle_t h;
    if (nvs_open(AI_NVS_NAMESPACE, NVS_READWRITE, &h) != ESP_OK) {
        printf("[AI Demo] NVS 打开失败，无法保存结果\r\n");
        return;
    }
    nvs_set_blob(h, key, result, sizeof(ai_result_t));
    nvs_commit(h);
    nvs_close(h);
    printf("[AI Demo] %s结果已写入 NVS (key=%s, 日期=%s)\r\n",
           type == AI_SUMMARY_DAILY ? "每日" : "每周", key, result->summary_date);
}

/* 通用 NVS 加载辅助 */
static bool ai_result_load_by_key(const char *key, ai_result_t *out, const char *label)
{
    nvs_handle_t h;
    if (nvs_open(AI_NVS_NAMESPACE, NVS_READONLY, &h) != ESP_OK) return false;
    size_t size = sizeof(ai_result_t);
    esp_err_t err = nvs_get_blob(h, key, out, &size);
    nvs_close(h);
    if (err == ESP_OK && out->valid) {
        printf("[AI Demo] %s已从 NVS 恢复 (日期:%s)\r\n", label, out->summary_date);
         printf("  %s%d%% | %s%d%% | %s%d%%\r\n",
             out->cat1_name, out->cat1_percent,
             out->cat2_name, out->cat2_percent,
             out->cat3_name, out->cat3_percent);
        return true;
    }
    printf("[AI Demo] %sNVS 中无有效数据\r\n", label);
    return false;
}

bool ai_daily_result_load(void)
{
    return ai_result_load_by_key(AI_NVS_KEY_DAILY, &g_ai_daily_result, "每日");
}

bool ai_weekly_result_load(void)
{
    return ai_result_load_by_key(AI_NVS_KEY_WEEKLY, &g_ai_weekly_result, "每周");
}

/*
 * 按日期范围过滤 CSV 任务行。
 * CSV 字段：任务名称,完成日期,用时分钟
 * date_from / date_to 格式 "YYYY-MM-DD"，可直接字典序比较。
 * 返回实际筛选到的任务条数（不含表头）。
 */
static int filter_csv_by_date(const char *csv_in, char *csv_out, size_t out_size,
                               const char *date_from, const char *date_to, int max_tasks)
{
    const char *p = csv_in;
    size_t written = 0;
    int task_count = 0;

    while (p && *p && task_count < max_tasks && written + 2 < out_size) {
        const char *line_end = strchr(p, '\n');
        size_t line_len = line_end ? (size_t)(line_end - p) : strlen(p);
        char line[160];
        if (line_len >= sizeof(line)) line_len = sizeof(line) - 1;
        memcpy(line, p, line_len);
        while (line_len > 0 && (line[line_len-1] == '\r' || line[line_len-1] == '\n')) line_len--;
        line[line_len] = '\0';

        if (line_len > 0) {
            /* 提取第2列日期（格式 YYYY-MM-DD） */
            const char *f1 = strchr(line, ',');
            if (f1) {
                const char *row_date = f1 + 1;
                if (strncmp(row_date, date_from, 10) >= 0 &&
                    strncmp(row_date, date_to,   10) <= 0) {
                    int n = snprintf(csv_out + written, out_size - written, "%s\n", line);
                    if (n > 0) written += (size_t)n;
                    task_count++;
                }
            }
        }
        p = line_end ? line_end + 1 : NULL;
    }
    if (written < out_size) csv_out[written] = '\0';
    return task_count;
}

typedef struct {
    bool *ntp_initialized;
    const char *api_key;
    ai_summary_type_t summary_type;
} ai_demo_args_t;

static void ai_demo_task(void *param)
{
    ai_demo_args_t *args = (ai_demo_args_t *)param;
    bool *ntp_initialized = args ? args->ntp_initialized : NULL;
    const char *api_key = args ? args->api_key : NULL;

    const size_t file_content_size = 8192;
    const size_t prompt_size = 10240;
    char *file_content = NULL;
    char *prompt = NULL;
    char *response = NULL;

    if (!api_key || api_key[0] == '\0') {
        printf("[AI Demo] 未配置认证信息,请检查 ai_chat_secrets.h\r\n");
        free(args);
        vTaskDelete(NULL);
        return;
    }

#if AI_CHAT_USE_PROXY
    ai_chat_init(AI_PROXY_URL, api_key, NULL);
#else
    ai_chat_init(NULL, api_key, NULL);
#endif

    file_content = (char *)heap_caps_malloc(file_content_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    prompt = (char *)heap_caps_malloc(prompt_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    response = (char *)heap_caps_malloc(AI_RESPONSE_BUF_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!file_content || !prompt || !response) {
        printf("[AI Demo] 内存分配失败: file=%p prompt=%p response=%p\r\n", file_content, prompt, response);
        free(file_content);
        free(prompt);
        free(response);
        free(args);
        vTaskDelete(NULL);
        return;
    }

    printf("\r\n[AI Demo] 判断 WiFi 连接...\r\n");
    if (!is_wifi_connected())
    {
        printf("[AI Demo] WiFi 未连接,退出ai功能...\r\n");
        free(file_content);
        free(prompt);
        free(response);
        free(args);
        vTaskDelete(NULL);
        return;
    }
    printf("[AI Demo] WiFi 已连接,等待 NTP 时间同步...\r\n");
    if (ntp_initialized && !(*ntp_initialized)) {
        initialize_sntp();
        *ntp_initialized = true;
        printf("[AI Demo] 已初始化 NTP,正在等待时间同步...\r\n");
    }

    bool time_synced = false;
    while(1){
        time_t now = 0;
        struct tm timeinfo = {0};
        time(&now);
        localtime_r(&now, &timeinfo);
        if (timeinfo.tm_year + 1900 >= 2025) {
            printf("[AI Demo] 时间已同步: %04d-%02d-%02d %02d:%02d:%02d\r\n",
                   timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
                   timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
            time_synced = true;
            break;
        } else {
            printf("[AI Demo] 当前时间未同步: %04d-%02d-%02d %02d:%02d:%02d\r\n",
                   timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
                   timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    if (!time_synced) {
        printf("[AI Demo] NTP 时间同步失败，无法进行 HTTPS 请求\r\n");
        free(file_content);
        free(prompt);
        free(response);
        free(args);
        vTaskDelete(NULL);
        return;
    }
    printf("[AI Demo] 开始请求 AI...\r\n");

    FILE *file = fopen("/spiffs/task_done.csv", "r");
    if (file == NULL) {
        printf("[AI Demo] 无法打开 /spiffs/task_done.csv\r\n");
        free(file_content);
        free(prompt);
        free(response);
        free(args);
        vTaskDelete(NULL);
        return;
    }
    int total = fread(file_content, 1, file_content_size - 1, file);
    fclose(file);
    file_content[total] = '\0';
    printf("[AI Demo] 原始 CSV %d 字节\r\n", total);

    /* -------- 按日期范围过滤任务 -------- */
    ai_summary_type_t stype = args->summary_type;
    int max_tasks = (stype == AI_SUMMARY_DAILY) ? DAILY_MAX_TASKS : WEEKLY_MAX_TASKS;

    time_t now_t = time(NULL);
    struct tm now_tm = {0};
    localtime_r(&now_t, &now_tm);
    char date_to_s[40], date_from_s[40];
    snprintf(date_to_s, sizeof(date_to_s), "%04d-%02d-%02d",
             now_tm.tm_year + 1900, now_tm.tm_mon + 1, now_tm.tm_mday);
    if (stype == AI_SUMMARY_DAILY) {
        strncpy(date_from_s, date_to_s, sizeof(date_from_s));
    } else {
        time_t week_ago = now_t - 7 * 24 * 3600;
        struct tm week_tm = {0};
        localtime_r(&week_ago, &week_tm);
        snprintf(date_from_s, sizeof(date_from_s), "%04d-%02d-%02d",
                 week_tm.tm_year + 1900, week_tm.tm_mon + 1, week_tm.tm_mday);
    }
    printf("[AI Demo] %s模式,日期范围 %s ~ %s,上限 %d 条\r\n",
           stype == AI_SUMMARY_DAILY ? "每日" : "每周", date_from_s, date_to_s, max_tasks);

    char *filtered = (char *)heap_caps_malloc(file_content_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    int task_count = 0;
    if (filtered) {
        task_count = filter_csv_by_date(file_content, filtered, file_content_size,
                                        date_from_s, date_to_s, max_tasks);
        if (task_count == 0) {
            /* 日期不匹配时（如测试数据），回退使用全部数据 */
            printf("[AI Demo] 日期范围内无任务，回退使用全部数据（最多 %d 条）\r\n", max_tasks);
            task_count = filter_csv_by_date(file_content, filtered, file_content_size,
                                            "0000-00-00", "9999-99-99", max_tasks);
        }
        free(file_content);
        file_content = NULL;
    } else {
        /* PSRAM 分配失败，直接使用原始数据 */
        printf("[AI Demo] filtered 分配失败，使用原始数据\r\n");
        filtered = file_content;
        file_content = NULL;
        task_count = max_tasks;
    }
    printf("[AI Demo] 最终使用 %d 条任务数据\r\n", task_count);

    const char *period_str = (stype == AI_SUMMARY_DAILY) ? "今日" : "近一周";
    const char *summary_guide = (stype == AI_SUMMARY_DAILY)
        ? "写一段今日复盘:必须点名至少1个上方CSV里的真实任务名或用时细节,语气像损友聊天,可幽默调侃拖延/摸鱼/突然高效,别空泛鸡汤,别哲学说教"
        : "写一段近一周复盘:必须结合上方多条真实任务名与用时做对比,语气生动有画面感,可幽默调侃这周节奏,点出亮点与槽点各一处,别空泛鸡汤";
    snprintf(prompt, prompt_size,
             "以下是用户%s完成的 %d 条 todolist 任务"
             "(CSV 格式，字段：任务名称、完成日期、用时分钟):\n\n"
             "%s\n\n"
             "请将任务分成3类,严格只输出下面6行(不输出任何额外文字/解释/Markdown):\n"
             "CAT1|<分类名称>|<占比整数%%>\n"
             "CAT2|<分类名称>|<占比整数%%>\n"
             "CAT3|<分类名称>|<占比整数%%>\n"
             "SUMMARY|<%s,45到55个中文字,标点仅用半角逗号或句号>\n"
             "QUOTE|<英文短句,轻松俏皮或自嘲口吻,与SUMMARY呼应,不超过12个英文单词>\n"
             "END\n"
             "规则：占比按任务数量计算,三个整数之和必须等于100;分类名称不超过4个汉字;只返回这6行;"
             "SUMMARY必须让读者看出你读过CSV里的具体任务",
             period_str, task_count, filtered, summary_guide);

    free(filtered);
    filtered = NULL;
    file_content = NULL;

    printf("[AI Demo] 正在发送请求给 AI,请稍候...\r\n");
    int64_t t_start = esp_timer_get_time();
    esp_err_t ret = ai_chat_send(prompt, response, AI_RESPONSE_BUF_SIZE);
    int64_t t_end = esp_timer_get_time();
    int elapsed_ms = (int)((t_end - t_start) / 1000);

    if (ret == ESP_OK) {
        printf("\r\n===== AI 分类结果 =====\r\n%s\r\n", response);
        printf("========================\r\n");
        printf("[AI Demo] 响应耗时: %d ms\r\n", elapsed_ms);
        /* 解析响应，记录日期，持久化到 NVS */
        ai_result_t *dest = (stype == AI_SUMMARY_DAILY) ? &g_ai_daily_result : &g_ai_weekly_result;
        ai_result_parse(response, dest);
        strncpy(dest->summary_date, date_from_s, sizeof(dest->summary_date) - 1);
        if (dest->valid) {
            ai_result_save(stype, dest);
            if (stype == AI_SUMMARY_DAILY)  g_ai_daily_result_ready  = true;
            else                            g_ai_weekly_result_ready = true;
        } else {
            printf("[AI Demo] 解析失败：响应未包含有效 CAT1/CAT2/CAT3 行\r\n");
        }
    } else {
        printf("[AI Demo] 请求失败 (%d ms): %s\r\n", elapsed_ms, response);
    }

    printf("[AI Demo] 任务完成。\r\n");
    free(prompt);
    free(response);
    free(args);
    vTaskDelete(NULL);
}

void start_ai_summary(ai_summary_type_t type, bool *ntp_initialized, const char *api_key)
{
    ai_demo_args_t *args = (ai_demo_args_t *)malloc(sizeof(ai_demo_args_t));
    if (!args) {
        printf("[AI Demo] 启动失败：参数内存分配失败\r\n");
        return;
    }
    args->ntp_initialized = ntp_initialized;
    args->api_key         = api_key;
    args->summary_type    = type;
    const char *task_name = (type == AI_SUMMARY_DAILY) ? "ai_daily" : "ai_weekly";
    printf("[AI Demo] 创建任务前 内部堆剩余: %d B\r\n", (int)heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    /* 注意：ai_demo_task 会调用 nvs_set_blob（flash 操作），
     * 执行 flash 写入时 cache 会被禁用，此时任务栈必须在内部 RAM。
     * 大块 buffer 已在任务内用 heap_caps_malloc(SPIRAM) 分配，
     * 栈本身只需 5120 字节即可。 */
    if (xTaskCreatePinnedToCore(ai_demo_task, task_name, 5120, args, 5, NULL, 1) != pdPASS) {
        printf("[AI Demo] 任务创建失败：内部堆不足\r\n");
        free(args);
    }
}
