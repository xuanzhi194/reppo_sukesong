#pragma once
#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TODO_MAX_ITEMS
#define TODO_MAX_ITEMS  100
#define TODO_NAME_LEN   128
typedef struct {
    char     name[TODO_NAME_LEN];
    uint16_t duration_min;
    uint8_t  done;
    char     finish_date[16];
} todo_item_t;
typedef struct {
    uint8_t     count;
    todo_item_t items[TODO_MAX_ITEMS];
} todo_list_t;
#endif

/** 全局 RAM 缓存，外部只读，写入通过下方 API */
extern todo_list_t g_todo_list;

/**
 * @brief 初始化：从 SPIFFS CSV 加载到 g_todo_list。
 *        在 init_spiffs() 之后调用一次。
 */
esp_err_t todo_storage_init(void);

/**
 * @brief 蓝牙同步新任务列表后调用：写入 CSV，并重置 NVS done 标志。
 *        调用前需先将新列表填入 g_todo_list。
 */
esp_err_t todo_storage_save_list(void);

/**
 * @brief 解析蓝牙 JSON 任务数组，清空并覆盖 /spiffs/todo.csv（name,duration_min,done）。
 * @param json 形如 [{"content":"..","durationStr":"60MIN","done":false}, ...]
 */
esp_err_t todo_storage_apply_ble_json(const char *json);

/** @brief 从 SPIFFS 重新加载 todo.csv + NVS done 到 g_todo_list */
esp_err_t todo_storage_reload_from_flash(void);

/**
 * @brief 将 g_todo_list 导出为微信小程序 JSON 数组格式。
 * @return ESP_OK 成功；ESP_ERR_INVALID_SIZE 缓冲区不足
 */
esp_err_t todo_storage_export_ble_json(char *out, size_t out_size);

/**
 * @brief READ 同步包：区分待办与已完成归档。
 * @param out JSON 形如 {"todo":[...],"taskDone":[...]}
 */
esp_err_t todo_storage_export_ble_read_bundle(char *out, size_t out_size);

/** @brief 返回已完成任务数 */
uint8_t todo_storage_get_done_count(void);

/** @brief 归档已完成任务并从列表中移除 */
esp_err_t todo_storage_archive_done_tasks(void);

//** @brief 设置任务完成状态，并保存到 NVS（不修改 CSV） */
esp_err_t _nvs_save_done(void);

/**
 * @brief 将 UI 侧 done 标记合并进 g_todo_list 并写 NVS（不改 CSV）。
 *        仅在 count/name 一致时合并，避免覆盖进行中的 BLE 列表替换。
 *        用于 todolist 内完成任务后、离开页面前也能被小程序 READ 到。
 */
esp_err_t todo_storage_apply_ui_done_flags(const todo_list_t *ui);

/** @brief 清洗任务名：去掉控制字符，并将 ,| 换行等替换为空格，降低 CSV/AI Prompt 注入风险 */
void todo_storage_sanitize_task_name(char *name, size_t cap);

/** @brief 获取 g_todo_list 互斥锁（跨 BLE / NVS / UI 同步时使用） */
bool todo_storage_lock(TickType_t timeout_ticks);

/** @brief 释放 g_todo_list 互斥锁 */
void todo_storage_unlock(void);

#ifdef __cplusplus
}
#endif
