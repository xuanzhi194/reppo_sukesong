/**
 * @file app_init.c
 * @brief app_main 启动阶段的分步初始化
 */
#include "app_init.h"
#include "app_globals.h"
#include "app_nvs_worker.h"
#include "app_ui.h"
#include "app_wifi_worker.h"
#include "ai_demo_task.h"
#include "bat_percent.h"
#include "buzzer.h"
#include "custom.h"
#include "rx8130.h"
#include "ec06.h"
#include "events_init.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lv_tick.h"
#include "nvs_flash.h"
#include "todo_storage.h"
#include "wifi.h"
#include "wifi_cred_storage.h"
#include "wificfg_bridge.h"
#include "volume_bridge.h"
#include "focus_bridge.h"
#include "word_storage.h"
#include "ws2812b.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "lvgl.h"
#include <string.h>

void app_init_hardware(void)
{
    init_led_strip();
    start_boot_led_blink();
    init_encoder_direction();
    buzzer_init();
    battery_adc_init();
    rx8130_init(I2C_NUM_0, 15, 16);
    if (rx8130_probe(I2C_NUM_0) != ESP_OK) {
        ESP_LOGW("RTC", "RX8130 probe failed (check I2C wiring)");
    } else {
        bool rtc_lost_power = false;
        if (rx8130_startup(I2C_NUM_0, &rtc_lost_power) != ESP_OK) {
            ESP_LOGW("RTC", "RX8130 startup failed");
        } else if (rtc_lost_power) {
            ESP_LOGW("RTC", "RX8130 lost power — sync time via WiFi/NTP when possible");
        }
    }
}

void app_init_storage_and_data(void)
{
    nvs_flash_init();
    wifi_cred_storage_init();
    /* 链入 wificfg 强符号桥接，使列表点击可从 NVS 回填密码 */
    wificfg_bridge_init();
    volume_bridge_init();
    focus_time_init();

    ai_daily_result_load();
    ai_weekly_result_load();
    ai_rate_limit_init();
    memcpy(&g_ui_daily_result, &g_ai_daily_result, sizeof(ui_ai_result_t));
    memcpy(&g_ui_weekly_result, &g_ai_weekly_result, sizeof(ui_ai_result_t));

    init_spiffs();
    word_storage_init();
    app_ui_init_word_counters();

    todo_storage_init();
    ui_todo_list = g_todo_list;
    done_sum = todo_storage_get_done_count();
    todo_sum = g_todo_list.count - done_sum;

    init_wifi();
    app_wifi_register_callbacks();
    init_ntp = false;
}

void app_init_lvgl_ui(void)
{
    lvgl_mutex = xSemaphoreCreateMutex();

    lvgl_tick_timer_init();
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();
    setup_ui(&guider_ui);
    lv_refr_now(NULL);
    lv_indev_set_group(indev_keypad, g);

    /* Pre-convert the clock label to static-text mode so the periodic timer
     * callback can use lv_label_set_text_static() without ever calling
     * lv_mem_free() on LVGL heap, removing the crash root cause. */
    app_ui_init_clock_label_static();

    bat_timer = lv_timer_create(app_ui_battery_timer_cb, 5000, NULL);
    lv_timer_create(app_ui_realtime_clock_timer_cb, 3000, NULL);
}

void app_init_start_workers(void)
{
    xTaskCreatePinnedToCore(app_nvs_worker_task, "nvs_thread", 8192, NULL, 10, NULL, 0);
    xTaskCreatePinnedToCore(app_wifi_worker_task, "time_thread", 4096, NULL, 10, NULL, 1);
}
