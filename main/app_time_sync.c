/**
 * @file app_time_sync.c
 * @brief WiFi 连上后用 SNTP 取北京时间，写入 RX8130 供后续走时
 *
 * SNTP 回调跑在 lwIP 线程里，只置标志；I2C 写 RTC 和刷新界面都放在主循环里做。
 * 显示始终读 RX8130，所以写进 RTC 后 timing 屏和主页会自然跟上。
 */
#include "app_time_sync.h"
#include "app_globals.h"
#include "app_ui.h"
#include "keep_machine.h"
#include "rx8130.h"
#include "wifi.h"
#include "esp_log.h"
#include "esp_sntp.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

static const char *TAG = "TIME_SYNC";

/* CONFIG_LWIP_SNTP_MAX_SERVERS=1，所以一次只设一个，超时后轮换。 */
static const char *const s_servers[] = {
    "ntp.aliyun.com",
    "ntp.tencent.com",
    "cn.pool.ntp.org",
};

#define SYNC_RETRY_MS 20000

static volatile bool s_synced;
static bool s_started;
static bool s_wifi_seen;
static bool s_rtc_done;
static int s_server_idx;
static TickType_t s_start_tick;

static void time_sync_cb(struct timeval *tv)
{
    (void)tv;
    s_synced = true;
}

static void time_sync_start(void)
{
    if (!s_started) {
        setenv("TZ", "CST-8", 1);
        tzset();
        esp_sntp_setoperatingmode(ESP_SNTP_OPMODE_POLL);
        esp_sntp_set_time_sync_notification_cb(time_sync_cb);
        esp_sntp_setservername(0, s_servers[s_server_idx]);
        esp_sntp_init();
        s_started = true;
    } else {
        esp_sntp_setservername(0, s_servers[s_server_idx]);
        esp_sntp_restart();
    }
    s_start_tick = xTaskGetTickCount();
    ESP_LOGI(TAG, "SNTP start, server=%s", s_servers[s_server_idx]);
}

static bool time_sync_write_rtc(void)
{
    if (rx8130_sync_from_system(I2C_NUM_0) != ESP_OK) {
        return false;
    }
    keep_machine_refresh_today();
    if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        app_ui_rtc_labels_refresh_now();
        xSemaphoreGive(lvgl_mutex);
    }
    return true;
}

void app_time_sync_poll(void)
{
    bool wifi = is_wifi_connected();

    if (wifi && !s_wifi_seen) {
        s_rtc_done = false;
        time_sync_start();
    }
    s_wifi_seen = wifi;

    if (s_synced) {
        s_synced = false;
        /* 失败时留给下一次 SNTP 周期（默认 1 小时）或下次连网重试。 */
        s_rtc_done = time_sync_write_rtc();
        return;
    }

    if (wifi && s_started && !s_rtc_done &&
        (xTaskGetTickCount() - s_start_tick) > pdMS_TO_TICKS(SYNC_RETRY_MS)) {
        s_server_idx = (s_server_idx + 1) % (int)(sizeof(s_servers) / sizeof(s_servers[0]));
        time_sync_start();
    }
}
