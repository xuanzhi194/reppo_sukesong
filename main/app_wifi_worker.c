/**
 * @file app_wifi_worker.c
 * @brief WiFi 与时间相关后台逻辑（与 LVGL 通过 lvgl_mutex 交互）
 */
#include "app_wifi_worker.h"
#include "app_globals.h"
#include "app_ui.h"
#include "custom.h"
#include "rx8130.h"
#include "esp_log.h"
#include "wifi.h"
#include "wifi_cred_storage.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

static const char *TAG_REALTIME = "REAL_TIME";
static const char *TAG_WIFI = "WIFI";

static void app_wifi_on_sta_disconnected(void)
{
    wifi_success = 0;
    init_ntp = false;
}

void app_wifi_register_callbacks(void)
{
    wifi_set_disconnect_callback(app_wifi_on_sta_disconnected);
}

/** 将 NTP 时间写入全局 timing_* 并刷新 timing 屏 */
static void wifi_worker_timing_labels_from_tm(const struct tm *timeinfo)
{
    timing_year = timeinfo->tm_year + 1900;
    timing_month = timeinfo->tm_mon + 1;
    timing_day = timeinfo->tm_mday;
    timing_weekday = timeinfo->tm_wday;

    char date_str[30];
    char time_str[20];
    snprintf(date_str, sizeof(date_str), "%04d-%02d-%02d",
             timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday);
    snprintf(time_str, sizeof(time_str), "%02d:%02d",
             timeinfo->tm_hour, timeinfo->tm_min);

    if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(50)) != pdTRUE) {
        return;
    }
    if (timeinfo->tm_year + 1900 >= 2026) {
        int wday = timeinfo->tm_wday;
        if (wday < 0 || wday > 6) {
            wday = 0;
        }
        app_ui_timing_labels_update(date_str, time_str, week_days[wday]);
    }
    if (app_ui_timing_screen_is_alive()) {
        app_ui_timing_status_update(true);
    }
    xSemaphoreGive(lvgl_mutex);
}

/** NTP 校时到 RX8130 */
static void wifi_worker_sync_rtc_from_tm(const struct tm *timeinfo)
{
    (void)timeinfo;
    rx8130_sync_from_system(I2C_NUM_0);
}

/** WiFi 已连接：NTP 取时并刷新 UI */
static void wifi_worker_handle_realtime_online(void)
{
    sntp_retry_if_needed();

    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);

    if (is_system_time_valid()) {
        ESP_LOGI(TAG_REALTIME, "Current time: %04d-%02d-%02d %02d:%02d:%02d",
                 timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
                 timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        wifi_worker_timing_labels_from_tm(&timeinfo);
        wifi_worker_sync_rtc_from_tm(&timeinfo);
    } else {
        ESP_LOGW(TAG_REALTIME, "NTP not synced yet (%04d), retrying...",
                 timeinfo.tm_year + 1900);
    }
    vTaskDelay(pdMS_TO_TICKS(990));
}

/** 无 WiFi：从 RX8130 读时间并刷新 UI */
static void wifi_worker_handle_realtime_offline(void)
{
    rx8130_time_t now = {0};
    esp_err_t rtc_err = rx8130_get_time(I2C_NUM_0, &now);
    char date_str[30];
    char time_str[20];
    snprintf(date_str, sizeof(date_str), "%04d-%02d-%02d",
             now.year, now.month, now.date);
    snprintf(time_str, sizeof(time_str), "%02d:%02d", now.hour, now.minute);
    int wday = (rtc_err == ESP_OK && now.day < 7) ? now.day : 0;

    if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(50)) != pdTRUE) {
        return;
    }
    if (rtc_err == ESP_OK) {
        app_ui_timing_labels_update(date_str, time_str, week_days[wday]);
    }
    if (app_ui_timing_screen_is_alive()) {
        app_ui_timing_status_update(false);
    }
    xSemaphoreGive(lvgl_mutex);
    vTaskDelay(pdMS_TO_TICKS(990));
}

/** 扫描 AP 并更新 wificfg 列表标签 */
static void wifi_worker_handle_scan(void)
{
    wifi_scan_top_5();
    if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(100)) != pdTRUE) {
        wifi_scan_en = 0;
        return;
    }
    if (app_ui_wificfg_screen_is_alive() &&
        guider_ui.screen_wificfg_list_1 != NULL &&
        lv_obj_is_valid(guider_ui.screen_wificfg_list_1)) {
        for (int i = 0; i < g_scan_ap_num; i++) {
            lv_obj_t *list_btn = lv_obj_get_child(guider_ui.screen_wificfg_list_1, i);
            if (list_btn == NULL || !lv_obj_is_valid(list_btn)) {
                continue;
            }
            lv_obj_t *label = lv_obj_get_child(list_btn, -1);
            if (label != NULL && lv_obj_is_valid(label)) {
                lv_label_set_text(label, g_scan_results[i].ssid);
            }
        }
    }
    xSemaphoreGive(lvgl_mutex);
    wifi_scan_en = 0;
}

/** 连接 WiFi、保存凭据、更新连接状态 UI */
static void wifi_worker_handle_connect(void)
{
    char ssid_snap[33] = {0};
    char wifi_pw_buf[64] = {0};

    if (wifi_connect_ssid[0] != '\0') {
        strncpy(ssid_snap, wifi_connect_ssid, sizeof(ssid_snap) - 1);
    } else if (wifi_ssid != NULL) {
        strncpy(ssid_snap, wifi_ssid, sizeof(ssid_snap) - 1);
    }

    if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        if (app_ui_wificfg_screen_is_alive() &&
            guider_ui.screen_wificfg_ta_password != NULL &&
            lv_obj_is_valid(guider_ui.screen_wificfg_ta_password)) {
            const char *wifi_password =
                lv_textarea_get_text(guider_ui.screen_wificfg_ta_password);
            strncpy(wifi_pw_buf, wifi_password, sizeof(wifi_pw_buf) - 1);
        }
        xSemaphoreGive(lvgl_mutex);
    }

    if (ssid_snap[0] == '\0') {
        ESP_LOGW(TAG_WIFI, "WiFi connect aborted: empty SSID snapshot");
        wifi_connect_en = 0;
        return;
    }

    wifi_success = wifi_connect_and_wait(ssid_snap, wifi_pw_buf);

    if (xSemaphoreTake(lvgl_mutex, portMAX_DELAY) != pdTRUE) {
        memset(wifi_pw_buf, 0, sizeof(wifi_pw_buf));
        wifi_connect_en = 0;
        return;
    }
    if (wifi_success) {
        esp_err_t cred_err = wifi_cred_save(ssid_snap, wifi_pw_buf);
        if (cred_err != ESP_OK) {
            ESP_LOGW(TAG_WIFI, "Failed to save WiFi credential: %s", esp_err_to_name(cred_err));
        }
        wifi_connect_show();
        ensure_sntp_started();
        init_ntp = true;
    } else {
        wifi_disconnect_show();
    }
    memset(wifi_pw_buf, 0, sizeof(wifi_pw_buf));
    xSemaphoreGive(lvgl_mutex);
    wifi_connect_en = 0;
}

static void wifi_worker_handle_disconnect(void)
{
    esp_wifi_disconnect();
    wifi_disconnect_en = 0;
    wifi_success = 0;
    init_ntp = false;
    stop_sntp();
}

void app_wifi_worker_task(void *arg)
{
    (void)arg;
    while (1) {
        if (task_real_time_en) {
            if (is_wifi_connected()) {
                wifi_worker_handle_realtime_online();
            } else {
                wifi_worker_handle_realtime_offline();
            }
        } else if (wifi_scan_en) {
            wifi_worker_handle_scan();
        } else if (wifi_connect_en) {
            wifi_worker_handle_connect();
        } else if (wifi_disconnect_en) {
            wifi_worker_handle_disconnect();
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
