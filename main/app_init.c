/**
 * @file app_init.c
 * @brief app_main 启动阶段的分步初始化
 */
#include "app_init.h"
#include "app_globals.h"
#include "app_nvs_worker.h"
#include "app_ui.h"
#include "app_wifi_worker.h"
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
#include "brightness_bridge.h"
#include "sleep_bridge.h"
#include "focus_bridge.h"
#include "keep_machine.h"
#include "book_machine.h"
#include "game_score_storage.h"
#include "badge_storage.h"
#include "picture_bridge.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "lvgl.h"

void app_init_hardware(void)
{
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
            ESP_LOGW("RTC", "RX8130 lost power — waiting for phone time sync over BLE");
        }
    }
}

void app_init_storage_and_data(void)
{
    nvs_flash_init();
    game_score_storage_init();
    wifi_cred_storage_init();
    /* 链入 wificfg 强符号桥接，使列表点击可从 NVS 回填密码 */
    wificfg_bridge_init();
    volume_bridge_init();
    /* Restore backlight level from NVS before LCD PWM starts, so boot
     * does not flash 100% then drop to the saved setting. */
    brightness_bridge_init();
    sleep_bridge_init();
    focus_time_init();
    keep_machine_init();

    init_spiffs();
    book_machine_init();
    badge_storage_init();
    picture_bridge_init();

    todo_storage_init();
    ui_todo_list = g_todo_list;
    done_sum = todo_storage_get_done_count();
    todo_sum = g_todo_list.count - done_sum;

    init_wifi();
    app_wifi_register_callbacks();
}

void app_init_lvgl_ui(void)
{
    lvgl_mutex = xSemaphoreCreateMutex();

    lvgl_tick_timer_init();
    lv_init();
    lv_port_disp_init();
    brightness_apply();
    lv_port_indev_init();
    setup_ui(&guider_ui);
    lv_refr_now(NULL);
    lv_indev_set_group(indev_keypad, g);

    /* Pre-convert the clock label to static-text mode so the periodic timer
     * callback can use lv_label_set_text_static() without ever calling
     * lv_mem_free() on LVGL heap, removing the crash root cause. */
    app_ui_init_clock_label_static();

    bat_timer = lv_timer_create(app_ui_battery_timer_cb, 5000, NULL);
    clock_timer = lv_timer_create(app_ui_realtime_clock_timer_cb, 3000, NULL);
}

void app_init_start_workers(void)
{
    xTaskCreatePinnedToCore(app_nvs_worker_task, "nvs_thread", 8192, NULL, 10, NULL, 0);
    xTaskCreatePinnedToCore(app_wifi_worker_task, "time_thread", 4096, NULL, 10, NULL, 1);
}
