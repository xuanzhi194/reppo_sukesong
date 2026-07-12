/**
 * @file app_nvs_worker.c
 * @brief 与存储、BLE、单词学习相关的后台轮询（不阻塞主循环 / LVGL）
 */
#include "app_nvs_worker.h"
#include "app_globals.h"
#include "bsp_ble.h"
#include "custom.h"
#include "nvs_flash.h"
#include "todo_storage.h"
#include "ws2812b.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <stdio.h>

/** Todo 完成标记写回 NVS */
static void nvs_worker_handle_todo_save(void)
{
    if (!todo_back_label) {
        return;
    }
    printf("Saving done flags to NVS...\n");
    if (todo_storage_lock(pdMS_TO_TICKS(200))) {
        g_todo_list = ui_todo_list;
        _nvs_save_done();
        todo_storage_unlock();
    } else {
        ESP_LOGW("NVS_WORKER", "todo save skipped: lock timeout");
        return;
    }
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
    if (!todo_storage_lock(pdMS_TO_TICKS(500))) {
        ESP_LOGW("NVS_WORKER", "todo archive skipped: lock timeout");
        return;
    }
    g_todo_list = ui_todo_list;
    todo_storage_archive_done_tasks();
    ui_todo_list = g_todo_list;
    todo_storage_unlock();
    done_sum = todo_storage_get_done_count();
    todo_sum = g_todo_list.count - done_sum;
    donelist_load_from_csv();
    donelist_load_label = 1;
}

/** 单词卡流水灯：异步播放，不阻塞 UI 切词 */
static void nvs_worker_handle_led_hints(void)
{
    if (led_green_en) {
        led_green_en = 0;
        start_flow_light_fast_async(34, 80, 32);
    }
    if (led_red_en) {
        led_red_en = 0;
        start_flow_light_fast_async(80, 32, 33);
    }
}

/** BLE 开关与断开请求 */
static void nvs_worker_handle_ble_requests(void)
{
    if (ble_toggle_on_req) {
        ble_toggle_on_req = 0;
        ble_toggle_off_req = 0;
        ble_enabled = true;
        ble_ui_refresh_req = true;
        bsp_ble_init();
        start_ble_flow_light();
    } else if (ble_toggle_off_req) {
        ble_toggle_off_req = 0;
        ble_toggle_on_req = 0;
        stop_ble_flow_light();
        bsp_ble_deinit();
        start_flow_light_fast_async(80, 32, 33);
    }
    if (ble_disconnect_req) {
        ble_disconnect_req = 0;
        bsp_ble_disconnect();
    }
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
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
