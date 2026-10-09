/**
 * @file app_wifi_worker.c
 * @brief WiFi 扫描/连接，以及 timing 屏从 RTC 刷新时间
 */
#include "app_wifi_worker.h"
#include "app_globals.h"
#include "app_ui.h"
#include "custom.h"
#include "ui_locale.h"
#include "rx8130.h"
#include "esp_log.h"
#include "wifi.h"
#include "wifi_cred_storage.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <string.h>

static const char *TAG_WIFI = "WIFI";

static void app_wifi_on_sta_disconnected(void)
{
    wifi_success = 0;
}

void app_wifi_register_callbacks(void)
{
    wifi_set_disconnect_callback(app_wifi_on_sta_disconnected);
}

/** 从 RX8130 读时间并刷新 timing 屏（RTC 由 app_time_sync / 手机蓝牙校准） */
static void wifi_worker_handle_realtime(void)
{
    rx8130_time_t now = {0};
    esp_err_t rtc_err = rx8130_get_time(I2C_NUM_0, &now);
    char date_str[30];
    char time_str[20];
    snprintf(date_str, sizeof(date_str), "%04d-%02d-%02d",
             now.year, now.month, now.date);
    snprintf(time_str, sizeof(time_str), "%02d:%02d", now.hour, now.minute);
    int wday = (rtc_err == ESP_OK && now.day < 7) ? now.day : 0;

    if (rtc_err == ESP_OK) {
        timing_year = now.year;
        timing_month = now.month;
        timing_day = now.date;
        timing_weekday = wday;
    }

    if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(50)) != pdTRUE) {
        vTaskDelay(pdMS_TO_TICKS(990));
        return;
    }
    if (rtc_err == ESP_OK) {
        app_ui_timing_labels_update(date_str, time_str, ui_weekday(wday));
    }
    if (app_ui_timing_screen_is_alive()) {
        app_ui_timing_status_update(is_wifi_connected());
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
}

void app_wifi_worker_task(void *arg)
{
    (void)arg;
    while (1) {
        if (task_real_time_en) {
            wifi_worker_handle_realtime();
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
