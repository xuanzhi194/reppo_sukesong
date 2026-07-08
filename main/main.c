/**
 * @file main.c
 * @brief 应用入口：初始化各子模块后进入主循环
 *
 * 模块划分：
 *   app_init.c         — 硬件 / 存储 / LVGL / 后台任务启动
 *   app_main_loop.c    — 主循环事件（AI、BLE、donelist 等）
 *   app_nvs_worker.c   — NVS、Todo、BLE、单词卡后台
 *   app_wifi_worker.c  — WiFi 扫描连接、NTP/RTC 时间
 *   app_ui.c           — 屏刷新与 LVGL 定时器回调
 *   app_led.c          — RGB 流水灯与蜂鸣
 *   app_globals.c      — 全局状态
 */
 //cyw 20260620 add
#include "app_init.h"
#include "app_main_loop.h"
#include "bsp_ble.h"
#include "reppo_demo_mode.h"
#include "demo_boot_gate.h"
#include "ws2812b.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "app_main";

void app_main(void)
{
#if REPPO_DEMO_BLE_BOOT_GATE
    ESP_LOGI(TAG, "Demo mode: waiting for BLE 0x30 or ENTER before normal boot");
    init_led_strip();
    set_led_bar_level(1, 10, 24, 49);

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    bsp_ble_init();
    demo_boot_gate_wait_ble_or_button();
    clear_led_bar();
#endif

    /* 最先初始化流水灯并立即点亮，缩短长按开机后的视觉反馈延迟 */
    init_led_strip();
    start_boot_led_blink();

    app_init_hardware();
    app_init_storage_and_data();
    app_init_lvgl_ui();
    app_init_start_workers();

    while (1) {
        app_main_loop_once();
        app_main_loop_lvgl_tick();
        vTaskDelay(pdMS_TO_TICKS(2));
    }
}
