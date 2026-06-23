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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
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
