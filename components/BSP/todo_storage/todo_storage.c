/**
 * todo_storage.c
 *
 * 存储方案：CSV（只读任务名/时长） + NVS（存 done 标志）
 *   - CSV (/spiffs/todo.csv): 格式 name,duration_min，仅蓝牙同步时写一次
 *   - NVS (namespace "TodoDone"): done 标志数组，标记完成时立即更新
 */

#include "todo_storage.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <time.h>
#include "ds3231.h"
#include "esp_heap_caps.h"

#define TAG           "TODO_STORAGE"
#define TODO_CSV_PATH "/spiffs/todo.csv"
#define TASK_DONE_CSV_PATH "/spiffs/task_done.csv"
#define NVS_NS        "TodoDone"
#define NVS_KEY_DONE  "done_arr"

// ─── 全局 RAM 缓存 ─────────────────────────────
todo_list_t g_todo_list = {0};

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

esp_err_t todo_storage_init(void)
{
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

uint8_t todo_storage_get_done_count(void)
{
    uint8_t cnt = 0;
    for (uint8_t i = 0; i < g_todo_list.count; i++)
        if (g_todo_list.items[i].done) cnt++;
    return cnt;
}



esp_err_t todo_storage_archive_done_tasks(void)
{
    // 1. 从 ds3231 芯片检查并获取当前绝对日期
    ds3231_time_t ds_now;
    if (ds3231_get_time(I2C_NUM_0, &ds_now) != ESP_OK) {
        printf("[TODO Archive] Error reading from DS3231!\n");
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
