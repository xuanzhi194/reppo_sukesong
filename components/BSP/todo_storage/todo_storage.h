#pragma once
#include <stdint.h>
#include "esp_err.h"

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


/** @brief 返回已完成任务数 */
uint8_t todo_storage_get_done_count(void);

/** @brief 归档已完成任务并从列表中移除 */
esp_err_t todo_storage_archive_done_tasks(void);

//** @brief 设置任务完成状态，并保存到 NVS（不修改 CSV） */
esp_err_t _nvs_save_done(void);

#ifdef __cplusplus
}
#endif
