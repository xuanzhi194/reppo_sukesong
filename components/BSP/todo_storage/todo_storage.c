/**
 * todo_storage.c
 *
 * 存储方案：CSV（只读任务名/时长） + NVS（存 done 标志）
 *   - CSV (/spiffs/todo.csv): 格式 name,duration_min，仅蓝牙同步时写一次
 *   - NVS (namespace "TodoDone"): done 标志数组，标记完成时立即更新
 */

#include "todo_storage.h"
#include "cJSON.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <time.h>
#include <ctype.h>
#include "rx8130.h"
#include "esp_heap_caps.h"
#include "freertos/semphr.h"

#define TAG           "TODO_STORAGE"
#define TODO_CSV_PATH "/spiffs/todo.csv"
#define TASK_DONE_CSV_PATH "/spiffs/task_done.csv"
#define NVS_NS        "TodoDone"
#define NVS_KEY_DONE  "done_arr"

// ─── 全局 RAM 缓存 ─────────────────────────────
todo_list_t g_todo_list = {0};
static SemaphoreHandle_t s_todo_mutex;

void todo_storage_sanitize_task_name(char *name, size_t cap)
{
    if (name == NULL || cap == 0) {
        return;
    }

    size_t write = 0;
    for (size_t read = 0; name[read] != '\0' && read < cap; read++) {
        unsigned char c = (unsigned char)name[read];
        if (c < 0x20 || c == 0x7f) {
            continue;
        }
        if (c == ',' || c == '|' || c == '\r' || c == '\n' || c == '\t') {
            if (write > 0 && name[write - 1] != ' ') {
                name[write++] = ' ';
            }
            continue;
        }
        name[write++] = (char)c;
    }
    while (write > 0 && name[write - 1] == ' ') {
        write--;
    }
    name[write] = '\0';
}

bool todo_storage_lock(TickType_t timeout_ticks)
{
    if (s_todo_mutex == NULL) {
        return true;
    }
    return xSemaphoreTake(s_todo_mutex, timeout_ticks) == pdTRUE;
}

void todo_storage_unlock(void)
{
    if (s_todo_mutex != NULL) {
        xSemaphoreGive(s_todo_mutex);
    }
}

// ─── 私有：从 NVS 加载 done 标志 ───────────────
static void _nvs_load_done(void)
{
    nvs_handle_t h;
    if (nvs_open(NVS_NS, NVS_READONLY, &h) != ESP_OK) return;
    size_t sz = g_todo_list.count;
    uint8_t done_buf[TODO_MAX_ITEMS] = {0};
    if (nvs_get_blob(h, NVS_KEY_DONE, done_buf, &sz) == ESP_OK) {
        for (size_t i = 0; i < sz && i < g_todo_list.count; i++)
            g_todo_list.items[i].done = done_buf[i];
        ESP_LOGI(TAG, "Loaded done flags (%d entries) from NVS.", (int)sz);
    }
    nvs_close(h);
}

// ─── 私有：从 CSV 解析 name + duration_min ─────
static esp_err_t _read_csv(void)
{
    FILE *f = fopen(TODO_CSV_PATH, "r");
    if (f == NULL) {
        ESP_LOGW(TAG, "CSV not found: %s", TODO_CSV_PATH);
        return ESP_ERR_NOT_FOUND;
    }

    char line[128];
    uint8_t count = 0;
    memset(&g_todo_list, 0, sizeof(g_todo_list));

    while (fgets(line, sizeof(line), f) != NULL && count < TODO_MAX_ITEMS) {
        size_t len = strlen(line);
        while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r'))
            line[--len] = '\0';
        if (len == 0) continue;

        char *p_name = strtok(line, ",");
        char *p_dur  = strtok(NULL, ",");
        // 第三列 done 不读取，始终由 NVS 恢复
        if (!p_name || !p_dur) continue;

        strncpy(g_todo_list.items[count].name, p_name, TODO_NAME_LEN - 1);
        g_todo_list.items[count].name[TODO_NAME_LEN - 1] = '\0';
        g_todo_list.items[count].duration_min = (uint16_t)atoi(p_dur);
        g_todo_list.items[count].done = 0;  // done 由 NVS 单独恢复
        count++;
    }
    fclose(f);

    g_todo_list.count = count;
    ESP_LOGI(TAG, "Loaded %d tasks from CSV.", count);
    return ESP_OK;
}

// ─── 私有：将 name + duration + done 写入 CSV（仅蓝牙同步调用）──
static esp_err_t _write_csv(void)
{
    FILE *f = fopen(TODO_CSV_PATH, "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "Cannot open CSV for write: %s", TODO_CSV_PATH);
        return ESP_FAIL;
    }
    for (uint8_t i = 0; i < g_todo_list.count; i++) {
        fprintf(f, "%s,%u,%u\n",
                g_todo_list.items[i].name,
                (unsigned)g_todo_list.items[i].duration_min,
                (unsigned)g_todo_list.items[i].done);
    }
    fclose(f);
    ESP_LOGI(TAG, "Saved %d tasks to CSV.", g_todo_list.count);
    return ESP_OK;
}

static void _trim_task_done_csv(int max_lines)
{
    FILE *f = fopen(TASK_DONE_CSV_PATH, "r");
    if (!f) return;
    
    int lines = 0;
    char buf[200];
    while (fgets(buf, sizeof(buf), f)) {
        lines++;
    }
    
    if (lines <= max_lines) {
        fclose(f);
        return;
    }
    
    rewind(f);
    int skip = lines - max_lines;
    FILE *ftmp = fopen("/spiffs/task_done.tmp", "w");
    if (!ftmp) {
        ESP_LOGE(TAG, "Failed to open task_done.tmp");
        fclose(f);
        return;
    }
    
    int curr = 0;
    while (fgets(buf, sizeof(buf), f)) {
        if (curr >= skip) {
            fputs(buf, ftmp);
        }
        curr++;
    }
    fclose(f);
    fclose(ftmp);
    
    remove(TASK_DONE_CSV_PATH);
    rename("/spiffs/task_done.tmp", TASK_DONE_CSV_PATH);
    ESP_LOGI(TAG, "Trimmed task_done.csv from %d to %d lines.", lines, max_lines);
}

// ─── 公共 API ───────────────────────────────────

esp_err_t _nvs_save_done(void)
{
    nvs_handle_t h;
    esp_err_t err = nvs_open(NVS_NS, NVS_READWRITE, &h);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "nvs_open failed: %s", esp_err_to_name(err));
        return err;
    }
    uint8_t done_buf[TODO_MAX_ITEMS] = {0};
    for (uint8_t i = 0; i < g_todo_list.count; i++)
        done_buf[i] = g_todo_list.items[i].done;
    err = nvs_set_blob(h, NVS_KEY_DONE, done_buf, g_todo_list.count);
    if (err == ESP_OK) nvs_commit(h);
    nvs_close(h);
    return err;
}

esp_err_t todo_storage_apply_ui_done_flags(const todo_list_t *ui)
{
    if (ui == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!todo_storage_lock(pdMS_TO_TICKS(200))) {
        ESP_LOGW(TAG, "apply UI done flags: lock timeout");
        return ESP_ERR_TIMEOUT;
    }

    bool dirty_done = false;
    bool dirty_duration = false;
    if (ui->count == g_todo_list.count) {
        bool names_match = true;
        for (uint8_t i = 0; i < ui->count; i++) {
            if (strncmp(ui->items[i].name, g_todo_list.items[i].name,
                        TODO_NAME_LEN) != 0) {
                names_match = false;
                break;
            }
        }
        if (names_match) {
            for (uint8_t i = 0; i < ui->count; i++) {
                if (g_todo_list.items[i].done != ui->items[i].done) {
                    g_todo_list.items[i].done = ui->items[i].done;
                    dirty_done = true;
                }
                if (g_todo_list.items[i].duration_min != ui->items[i].duration_min) {
                    g_todo_list.items[i].duration_min = ui->items[i].duration_min;
                    dirty_duration = true;
                }
            }
        }
    }

    esp_err_t err = ESP_OK;
    if (dirty_duration) {
        err = _write_csv();
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "Synced UI duration to CSV (%u tasks)",
                     (unsigned)g_todo_list.count);
        }
    }
    if (dirty_done) {
        esp_err_t done_err = _nvs_save_done();
        if (done_err == ESP_OK) {
            ESP_LOGI(TAG, "Synced UI done flags to NVS (%u tasks)",
                     (unsigned)g_todo_list.count);
        }
        if (err == ESP_OK) {
            err = done_err;
        }
    }

    todo_storage_unlock();
    return err;
}

esp_err_t todo_storage_init(void)
{
    if (s_todo_mutex == NULL) {
        s_todo_mutex = xSemaphoreCreateMutex();
    }

    esp_err_t ret = _read_csv();
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Starting with empty task list.");
        memset(&g_todo_list, 0, sizeof(g_todo_list));
        return ESP_OK;
    }
    _nvs_load_done();  // 从 NVS 恢复完成标志
    ESP_LOGI(TAG, "Init done: %d tasks, %d done.",
             g_todo_list.count, todo_storage_get_done_count());
    return ESP_OK;
}

esp_err_t todo_storage_save_list(void)
{
    // 仅供蓝牙同步新任务列表时调用：写入 CSV，并清空 NVS done 标志
    esp_err_t ret = _write_csv();
    if (ret == ESP_OK) {
        for (uint8_t i = 0; i < g_todo_list.count; i++)
            g_todo_list.items[i].done = 0;
        _nvs_save_done();
    }
    return ret;
}

static uint16_t _parse_duration_min(const char *s)
{
    unsigned v = 0;
    if (s == NULL || s[0] == '\0') {
        return 0;
    }
    sscanf(s, "%u", &v);
    return (uint16_t)v;
}

esp_err_t todo_storage_apply_ble_json(const char *json)
{
    if (json == NULL || json[0] == '\0') {
        return ESP_ERR_INVALID_ARG;
    }

    if (!todo_storage_lock(pdMS_TO_TICKS(500))) {
        ESP_LOGE(TAG, "BLE JSON apply: todo lock timeout");
        return ESP_ERR_TIMEOUT;
    }

    cJSON *root = cJSON_Parse(json);
    if (root == NULL) {
        ESP_LOGE(TAG, "BLE JSON parse failed");
        todo_storage_unlock();
        return ESP_ERR_INVALID_ARG;
    }
    if (!cJSON_IsArray(root)) {
        ESP_LOGE(TAG, "BLE JSON root is not an array");
        cJSON_Delete(root);
        todo_storage_unlock();
        return ESP_ERR_INVALID_ARG;
    }

    memset(&g_todo_list, 0, sizeof(g_todo_list));

    const int n = cJSON_GetArraySize(root);
    for (int i = 0; i < n && g_todo_list.count < TODO_MAX_ITEMS; i++) {
        cJSON *obj = cJSON_GetArrayItem(root, i);
        cJSON *content = cJSON_GetObjectItem(obj, "content");
        cJSON *duration_str = cJSON_GetObjectItem(obj, "durationStr");
        cJSON *done_item = cJSON_GetObjectItem(obj, "done");

        if (!cJSON_IsObject(obj) || !cJSON_IsString(content)) {
            continue;
        }

        todo_item_t *dst = &g_todo_list.items[g_todo_list.count];
        strncpy(dst->name, content->valuestring, TODO_NAME_LEN - 1);
        dst->name[TODO_NAME_LEN - 1] = '\0';
        todo_storage_sanitize_task_name(dst->name, TODO_NAME_LEN);
        if (dst->name[0] == '\0') {
            continue;
        }
        dst->duration_min = cJSON_IsString(duration_str)
                                 ? _parse_duration_min(duration_str->valuestring)
                                 : 0;
        dst->done = cJSON_IsBool(done_item) ? (cJSON_IsTrue(done_item) ? 1 : 0) : 0;
        dst->finish_date[0] = '\0';
        g_todo_list.count++;
    }

    cJSON_Delete(root);

    esp_err_t ret = _write_csv();
    if (ret == ESP_OK) {
        _nvs_save_done();
        ESP_LOGI(TAG, "BLE sync: overwrote todo.csv with %u task(s)", g_todo_list.count);
    }
    todo_storage_unlock();
    return ret;
}

esp_err_t todo_storage_reload_from_flash(void)
{
    esp_err_t ret = _read_csv();
    if (ret == ESP_OK) {
        _nvs_load_done();
    }
    return ret;
}

static cJSON *todo_storage_build_todo_json_array(void)
{
    cJSON *arr = cJSON_CreateArray();
    if (arr == NULL) {
        return NULL;
    }

    for (uint8_t i = 0; i < g_todo_list.count; i++) {
        cJSON *obj = cJSON_CreateObject();
        char dur[20];
        if (obj == NULL) {
            cJSON_Delete(arr);
            return NULL;
        }
        snprintf(dur, sizeof(dur), "%uMIN", (unsigned)g_todo_list.items[i].duration_min);
        cJSON_AddStringToObject(obj, "content", g_todo_list.items[i].name);
        cJSON_AddStringToObject(obj, "durationStr", dur);
        cJSON_AddBoolToObject(obj, "done", g_todo_list.items[i].done != 0);
        cJSON_AddItemToArray(arr, obj);
    }
    return arr;
}

static cJSON *todo_storage_build_task_done_json_array(void)
{
    FILE *f = fopen(TASK_DONE_CSV_PATH, "r");
    cJSON *arr = cJSON_CreateArray();
    char line[200];

    if (arr == NULL) {
        if (f) {
            fclose(f);
        }
        return NULL;
    }
    if (f == NULL) {
        return arr;
    }

    while (fgets(line, sizeof(line), f)) {
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[--len] = '\0';
        }
        if (len == 0) {
            continue;
        }

        char *p_name = strtok(line, ",");
        char *p_date = strtok(NULL, ",");
        char *p_dur  = strtok(NULL, ",");
        if (p_name == NULL || p_date == NULL) {
            continue;
        }

        cJSON *obj = cJSON_CreateObject();
        char dur[20];
        if (obj == NULL) {
            cJSON_Delete(arr);
            fclose(f);
            return NULL;
        }
        snprintf(dur, sizeof(dur), "%uMIN", (unsigned)(p_dur ? atoi(p_dur) : 0));
        cJSON_AddStringToObject(obj, "content", p_name);
        cJSON_AddStringToObject(obj, "date", p_date);
        cJSON_AddStringToObject(obj, "durationStr", dur);
        cJSON_AddItemToArray(arr, obj);
    }
    fclose(f);
    return arr;
}

static esp_err_t todo_storage_print_json_to_buf(cJSON *root, char *out, size_t out_size)
{
    char *printed = cJSON_PrintUnformatted(root);
    if (printed == NULL) {
        return ESP_ERR_NO_MEM;
    }
    size_t need = strlen(printed) + 1;
    if (need > out_size) {
        cJSON_free(printed);
        return ESP_ERR_INVALID_SIZE;
    }
    memcpy(out, printed, need);
    cJSON_free(printed);
    return ESP_OK;
}

esp_err_t todo_storage_export_ble_json(char *out, size_t out_size)
{
    if (out == NULL || out_size < 3) {
        return ESP_ERR_INVALID_ARG;
    }

    cJSON *arr = todo_storage_build_todo_json_array();
    if (arr == NULL) {
        return ESP_ERR_NO_MEM;
    }
    esp_err_t ret = todo_storage_print_json_to_buf(arr, out, out_size);
    cJSON_Delete(arr);
    return ret;
}

esp_err_t todo_storage_export_ble_read_bundle(char *out, size_t out_size)
{
    cJSON *root;
    cJSON *todo_arr;
    cJSON *done_arr;
    esp_err_t ret;

    if (out == NULL || out_size < 16) {
        return ESP_ERR_INVALID_ARG;
    }

    if (todo_storage_reload_from_flash() != ESP_OK) {
        ESP_LOGW(TAG, "todo reload failed for BLE bundle");
    }

    root = cJSON_CreateObject();
    todo_arr = todo_storage_build_todo_json_array();
    done_arr = todo_storage_build_task_done_json_array();
    if (root == NULL || todo_arr == NULL || done_arr == NULL) {
        cJSON_Delete(root);
        cJSON_Delete(todo_arr);
        cJSON_Delete(done_arr);
        return ESP_ERR_NO_MEM;
    }

    cJSON_AddItemToObject(root, "todo", todo_arr);
    cJSON_AddItemToObject(root, "taskDone", done_arr);

    {
        unsigned done_count = (unsigned)cJSON_GetArraySize(done_arr);
        ret = todo_storage_print_json_to_buf(root, out, out_size);
        cJSON_Delete(root);
        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "BLE read bundle: todo=%u taskDone=%u",
                     (unsigned)g_todo_list.count, done_count);
        }
        return ret;
    }
}

uint8_t todo_storage_get_done_count(void)
{
    uint8_t cnt = 0;
    for (uint8_t i = 0; i < g_todo_list.count; i++)
        if (g_todo_list.items[i].done) cnt++;
    return cnt;
}



esp_err_t todo_storage_archive_done_tasks(void)
{
    // 1. 从 RX8130 芯片检查并获取当前绝对日期
    rx8130_time_t ds_now;
    if (rx8130_get_time(I2C_NUM_0, &ds_now) != ESP_OK) {
        printf("[TODO Archive] Error reading from RX8130!\n");
        return ESP_FAIL;
    }
    
    if (ds_now.year < 2026) {
        printf("[TODO Archive] Current clock year is %04d. Please synchronize time first! Archiving aborted.\n", ds_now.year);
        return ESP_FAIL;
    }

    char current_date[32];
    snprintf(current_date, sizeof(current_date), "%04d-%02d-%02d",
             (int)ds_now.year, (int)ds_now.month, (int)ds_now.date);

    FILE *f_done = fopen(TASK_DONE_CSV_PATH, "a");
    if (!f_done) {
        ESP_LOGE(TAG, "Failed to open %s for appending", TASK_DONE_CSV_PATH);
        return ESP_FAIL;
    }

    // 在堆上分配新列表，避免 ~14KB 的 todo_list_t 压垮任务栈
    todo_list_t *new_list = (todo_list_t *)heap_caps_malloc(sizeof(todo_list_t), MALLOC_CAP_8BIT);
    if (!new_list) {
        ESP_LOGE(TAG, "Failed to allocate new_list on heap");
        fclose(f_done);
        return ESP_FAIL;
    }
    memset(new_list, 0, sizeof(todo_list_t));
    int archive_cnt = 0;

    // 2. 遍历现有任务，分类处理
    for (uint8_t i = 0; i < g_todo_list.count; i++) {
        if (g_todo_list.items[i].done == 1) {
            // 已完成任务：附加绝对日期+时长并存入 task_done.csv
            strncpy(g_todo_list.items[i].finish_date, current_date, sizeof(g_todo_list.items[i].finish_date) - 1);
            todo_storage_sanitize_task_name(g_todo_list.items[i].name, TODO_NAME_LEN);
            fprintf(f_done, "%s,%s,%u\n",
                    g_todo_list.items[i].name,
                    g_todo_list.items[i].finish_date,
                    (unsigned)g_todo_list.items[i].duration_min);
            archive_cnt++;
        } else {
            // 未完成任务：转移到新列表中保留
            new_list->items[new_list->count] = g_todo_list.items[i];
            new_list->count++;
        }
    }
    fclose(f_done);

    if (archive_cnt > 0) {
        // 3. 覆盖写入新的未完成任务列表 (更新 RAM + SPIFFS + NVS)
        g_todo_list = *new_list;
        heap_caps_free(new_list);
        _write_csv();        // 更新 todo.csv（只剩下未完成的）
        _nvs_save_done();    // 更新 NVS，标记均为未完成(因为 done 全是 0 转移过来的)
        ESP_LOGI(TAG, "Archived %d done tasks. %d tasks remaining.", archive_cnt, g_todo_list.count);
        
        // 4. 限制 task_done 文件中的任务数量，最多保留最新的 100 条
        _trim_task_done_csv(100);
    } else {
        heap_caps_free(new_list);
        ESP_LOGI(TAG, "No done tasks to archive.");
    }

    return ESP_OK;
}
