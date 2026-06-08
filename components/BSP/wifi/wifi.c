#include "wifi.h"
#include "rx8130.h"
#include "driver/i2c.h"
#include "esp_timer.h"
#include "lwip/ip4_addr.h"
#include "freertos/semphr.h"

static const char *TAG = "NTP_TIME";

#define WIFI_CONNECT_MAX_ATTEMPTS      3
#define WIFI_CONNECT_TIMEOUT_MS        15000
#define WIFI_DISCONNECT_SETTLE_MS      150

static bool s_sntp_started = false;
static int64_t s_last_sntp_retry_us = 0;
static SemaphoreHandle_t s_wifi_ip_sem = NULL;
static volatile bool s_connect_wait_active = false;
static volatile int s_last_disc_reason = -1;

wifi_scan_info_t g_scan_results[MAX_SCAN_RECORDS];
uint16_t g_scan_ap_num = 0;

static void sntp_on_sync(struct timeval *tv)
{
    (void)tv;
    ESP_LOGI(TAG, "NTP time synchronized");
    /* 主屏时钟读 RTC; 必须在 NTP 成功时写入芯片, 不能只在 timing 屏才同步 */
    if (rx8130_sync_from_system(I2C_NUM_0) != ESP_OK) {
        ESP_LOGW(TAG, "RTC sync from NTP failed");
    }
}

static bool system_time_valid_from_tm(const struct tm *timeinfo)
{
    return timeinfo && (timeinfo->tm_year + 1900 >= 2025);
}

bool is_system_time_valid(void)
{
    time_t now = 0;
    struct tm timeinfo = {0};
    time(&now);
    localtime_r(&now, &timeinfo);
    return system_time_valid_from_tm(&timeinfo);
}

void ensure_sntp_started(void)
{
    if (s_sntp_started) {
        esp_sntp_restart();
        ESP_LOGI(TAG, "SNTP restart");
        return;
    }

    ESP_LOGI(TAG, "Initializing SNTP");
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
    esp_sntp_set_time_sync_notification_cb(sntp_on_sync);

    /* 优先 IP，避免手机热点 DNS 慢/失败；再备域名 */
    ip_addr_t ip_aliyun;
    IP4_ADDR(&ip_aliyun.u_addr.ip4, 203, 107, 6, 88);
    ip_aliyun.type = IPADDR_TYPE_V4;
    esp_sntp_setserver(0, &ip_aliyun);

    ip_addr_t ip_ntp_cn;
    IP4_ADDR(&ip_ntp_cn.u_addr.ip4, 114, 118, 7, 163);
    ip_ntp_cn.type = IPADDR_TYPE_V4;
    esp_sntp_setserver(1, &ip_ntp_cn);

    esp_sntp_setservername(2, "ntp1.aliyun.com");

    esp_sntp_init();
    setenv("TZ", "CST-8", 1);
    tzset();
    s_sntp_started = true;
    s_last_sntp_retry_us = esp_timer_get_time();
}

void initialize_sntp(void)
{
    ensure_sntp_started();
}

void sntp_retry_if_needed(void)
{
    if (is_system_time_valid()) {
        return;
    }
    if (!s_sntp_started) {
        ensure_sntp_started();
        return;
    }

    int64_t now_us = esp_timer_get_time();
    if (now_us - s_last_sntp_retry_us < 30 * 1000000LL) {
        return;
    }
    s_last_sntp_retry_us = now_us;
    ESP_LOGW(TAG, "NTP still unsynced, retry SNTP (common on phone hotspot)");
    esp_sntp_restart();
}

bool wait_system_time_sync(uint32_t timeout_ms)
{
    uint32_t waited = 0;
    while (waited < timeout_ms) {
        sntp_retry_if_needed();
        if (is_system_time_valid()) {
            return true;
        }
        vTaskDelay(pdMS_TO_TICKS(500));
        waited += 500;
    }
    return is_system_time_valid();
}

void stop_sntp(void)
{
    if (!s_sntp_started) {
        return;
    }
    esp_sntp_stop();
    s_sntp_started = false;
    ESP_LOGI(TAG, "SNTP stopped");
}

void my_esp_event(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT) {
        if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
            wifi_event_sta_disconnected_t *disc = (wifi_event_sta_disconnected_t *)event_data;
            int reason = disc ? disc->reason : -1;
            printf("WiFi Connection Disconnected.\n");
            ESP_LOGW(TAG, "STA disconnected, reason=%d", reason);
            if (s_connect_wait_active) {
                s_last_disc_reason = reason;
            }
            stop_sntp();
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        printf("IP Address obtained successfully!\n");
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        char ip_str[16];
        sprintf(ip_str, IPSTR, IP2STR(&event->ip_info.ip));
        printf("Station IP Address: %s\n", ip_str);
        if (s_wifi_ip_sem != NULL) {
            xSemaphoreGive(s_wifi_ip_sem);
        }
        /* 拿到 IP 后再启 SNTP，比固定 delay 更稳（尤其手机热点） */
        ensure_sntp_started();
    }
}

void init_wifi()
{
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    printf("WiFi stack initialization complete!\n");

    esp_event_handler_instance_t wifi_event_handler;
    esp_event_handler_instance_register(WIFI_EVENT,
                                        ESP_EVENT_ANY_ID,
                                        &my_esp_event,
                                        NULL,
                                        &wifi_event_handler);

    esp_event_handler_instance_t ip_event_handler;
    esp_event_handler_instance_register(IP_EVENT,
                                        IP_EVENT_STA_GOT_IP,
                                        &my_esp_event,
                                        NULL,
                                        &ip_event_handler);
    printf("WiFi and IP event handlers registered!\n");

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
    esp_wifi_set_ps(WIFI_PS_NONE);
    if (s_wifi_ip_sem == NULL) {
        s_wifi_ip_sem = xSemaphoreCreateBinary();
    }
    printf("WiFi driver started!\n");
}

void wifi_connect_to_new_network(const char *ssid, const char *password) {
    wifi_config_t wifi_config = { 0 };

    strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
    if (password != NULL) {
        strncpy((char *)wifi_config.sta.password, password, sizeof(wifi_config.sta.password));
    }
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    wifi_config.sta.pmf_cfg.capable = true;
    wifi_config.sta.pmf_cfg.required = false;
    wifi_config.sta.scan_method = WIFI_FAST_SCAN;
    wifi_config.sta.sort_method = WIFI_CONNECT_AP_BY_SIGNAL;

    /* 扫描/断连与连接竞态时易出现 AUTH_EXPIRE，先停扫并等待断连稳定 */
    esp_wifi_scan_stop();
    esp_wifi_disconnect();
    vTaskDelay(pdMS_TO_TICKS(WIFI_DISCONNECT_SETTLE_MS));

    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_err_t err = esp_wifi_connect();
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "esp_wifi_connect failed: %s", esp_err_to_name(err));
    }

    printf("Connecting to WiFi: %s...\n", ssid);
}

bool wifi_connect_and_wait(const char *ssid, const char *password)
{
    if (ssid == NULL || ssid[0] == '\0') {
        return false;
    }
    if (s_wifi_ip_sem == NULL) {
        s_wifi_ip_sem = xSemaphoreCreateBinary();
        if (s_wifi_ip_sem == NULL) {
            return false;
        }
    }

    for (int attempt = 1; attempt <= WIFI_CONNECT_MAX_ATTEMPTS; attempt++) {
        xSemaphoreTake(s_wifi_ip_sem, 0);
        s_last_disc_reason = -1;
        s_connect_wait_active = true;

        wifi_connect_to_new_network(ssid, password);

        bool connected = false;
        for (uint32_t waited = 0; waited < WIFI_CONNECT_TIMEOUT_MS; waited += 200) {
            if (is_wifi_connected()) {
                connected = true;
                break;
            }
            if (xSemaphoreTake(s_wifi_ip_sem, pdMS_TO_TICKS(200)) == pdTRUE) {
                connected = true;
                break;
            }
            /* AUTH_EXPIRE 等失败常在 1~2s 内出现，不必傻等满 15s */
            if (s_last_disc_reason >= 0 && waited >= 1200) {
                ESP_LOGW(TAG, "Early retry: disconnected (reason=%d) on attempt %d",
                         s_last_disc_reason, attempt);
                break;
            }
        }
        s_connect_wait_active = false;

        if (connected || is_wifi_connected()) {
            ESP_LOGI(TAG, "WiFi connected on attempt %d", attempt);
            return true;
        }

        ESP_LOGW(TAG, "WiFi connect attempt %d/%d failed, retrying...",
                 attempt, WIFI_CONNECT_MAX_ATTEMPTS);
        esp_wifi_disconnect();
        vTaskDelay(pdMS_TO_TICKS(600 * (uint32_t)attempt));
    }

    return is_wifi_connected();
}

bool is_wifi_connected(void)
{
    esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    if (netif == NULL) {
        return false;
    }

    esp_netif_ip_info_t ip_info;
    if (esp_netif_get_ip_info(netif, &ip_info) != ESP_OK) {
        return false;
    }

    return (ip_info.ip.addr != 0);
}

void wifi_scan_top_5(void) {
    wifi_scan_config_t scan_config = {
        .show_hidden = false,
        .scan_type = WIFI_SCAN_TYPE_ACTIVE,
    };

    printf("Starting WiFi scan...\n");
    esp_err_t ret = esp_wifi_scan_start(&scan_config, true);
    if (ret != ESP_OK) {
        return;
    }

    uint16_t ap_count = 0;
    esp_wifi_scan_get_ap_num(&ap_count);

    wifi_ap_record_t *ap_records = malloc(sizeof(wifi_ap_record_t) * ap_count);
    if (ap_records == NULL) {
        return;
    }

    esp_wifi_scan_get_ap_records(&ap_count, ap_records);

    g_scan_ap_num = 0;
    memset(g_scan_results, 0, sizeof(g_scan_results));

    for (int i = 0; i < ap_count; i++) {
        bool already_exists = false;
        char *current_ssid = (char *)ap_records[i].ssid;

        for (int j = 0; j < g_scan_ap_num; j++) {
            if (strcmp(g_scan_results[j].ssid, current_ssid) == 0) {
                already_exists = true;
                if (ap_records[i].rssi > g_scan_results[j].rssi) {
                    g_scan_results[j].rssi = ap_records[i].rssi;
                }
                break;
            }
        }

        if (!already_exists && g_scan_ap_num < MAX_SCAN_RECORDS) {
            strncpy(g_scan_results[g_scan_ap_num].ssid, current_ssid, 32);
            g_scan_results[g_scan_ap_num].rssi = ap_records[i].rssi;
            g_scan_ap_num++;
        }
    }

    for (int k = 0; k < g_scan_ap_num; k++) {
        printf("Final List [%d]: %s (%d dBm)\n", k, g_scan_results[k].ssid, g_scan_results[k].rssi);
    }

    free(ap_records);
    esp_wifi_scan_stop();
}
