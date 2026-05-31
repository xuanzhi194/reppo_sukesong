/**
 * @file app_nvs_worker.c
 * @brief 与存储、BLE、单词学习相关的后台轮询（不阻塞主循环 / LVGL）
 */
#include "app_nvs_worker.h"
#include "app_globals.h"
#include "app_ui.h"
#include "bsp_ble.h"
#include "custom.h"
#include "nvs_flash.h"
#include "todo_storage.h"
#include "ws2812b.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

/** Todo 完成标记写回 NVS */
static void nvs_worker_handle_todo_save(void)
{
    if (!todo_back_label) {
        return;
    }
    printf("Saving done flags to NVS...\n");
    g_todo_list = ui_todo_list;
    _nvs_save_done();
    todo_back_label = 0;
}

/** 主界面重新加载任务列表标志（当前仅占位清零） */
static void nvs_worker_handle_main_reload(void)
{
    if (main_load_label) {
        main_load_label = 0;
    }
}

/** 归档已完成任务并触发 donelist 刷新 */
static void nvs_worker_handle_todo_archive(void)
{
    if (!todo_archive_label) {
        return;
    }
    todo_archive_label = 0;
    printf("Archiving completed tasks...\n");
    g_todo_list = ui_todo_list;
    todo_storage_archive_done_tasks();
    ui_todo_list = g_todo_list;
    done_sum = todo_storage_get_done_count();
    todo_sum = g_todo_list.count - done_sum;
    donelist_load_from_csv();
    donelist_load_label = 1;
}

/** 单词卡答对/答错触发的流水灯 */
static void nvs_worker_handle_led_hints(void)
{
    if (led_green_en) {
        led_green_en = 0;
        set_flow_light(34, 80, 32);
    }
    if (led_red_en) {
        led_red_en = 0;
        set_flow_light(80, 32, 33);
    }
}

/** BLE 开关与断开请求 */
static void nvs_worker_handle_ble_requests(void)
{
    if (ble_toggle_on_req) {
        ble_toggle_on_req = 0;
        bsp_ble_init();
        start_ble_flow_light();
    }
    if (ble_toggle_off_req) {
        ble_toggle_off_req = 0;
        stop_ble_flow_light();
        bsp_ble_deinit();
    }
    if (ble_disconnect_req) {
        ble_disconnect_req = 0;
        bsp_ble_disconnect();
    }
}

/** 单词卡 / 复习卡状态变更 → 在 LVGL 锁内刷新界面 */
static void nvs_worker_handle_wordcard_ui(void)
{
    if (!wordcard_change && !reviewcard_change) {
        return;
    }
    if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(50)) != pdTRUE) {
        return;
    }
    if (wordcard_change) {
        wordcard_change = 0;
        app_ui_wordcard_process_change();
    }
    if (reviewcard_change) {
        reviewcard_change = 0;
        app_ui_reviewcard_process_change();
    }
    xSemaphoreGive(lvgl_mutex);
}

void app_nvs_worker_task(void *param)
{
    (void)param;
    nvs_handle_t timing_handle;
    nvs_open("Timing", NVS_READWRITE, &timing_handle);

    while (1) {
        nvs_worker_handle_todo_save();
        nvs_worker_handle_main_reload();
        nvs_worker_handle_todo_archive();
        nvs_worker_handle_led_hints();
        nvs_worker_handle_ble_requests();
        nvs_worker_handle_wordcard_ui();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
