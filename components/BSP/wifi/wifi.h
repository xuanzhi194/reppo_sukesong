#ifndef __WIFI_H_
#define __WIFI_H_
 
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_netif.h"    
#include "esp_err.h"
#include "esp_log.h"

#define MAX_SCAN_RECORDS 5 
// Define a structure to hold necessary WiFi info
typedef struct {
    char ssid[33];    // SSID is max 32 chars + null terminator
    int rssi;         // Signal strength
} wifi_scan_info_t;

// Global array to hold the last scan results
extern wifi_scan_info_t g_scan_results[MAX_SCAN_RECORDS];
extern uint16_t g_scan_ap_num; // Actual number of APs found (up to 5)

 // Definitions of the global variables
void init_wifi();
bool is_wifi_connected();
void wifi_scan_top_5(void);
void wifi_connect_to_new_network(const char *ssid, const char *password);
/** 连接 WiFi 并等待拿到 IP（含自动重试，避免首次 AUTH_EXPIRE 需手动再连） */
bool wifi_connect_and_wait(const char *ssid, const char *password);

/** STA 断连时回调（由应用层同步 wifi_success 等状态） */
typedef void (*wifi_disconnect_cb_t)(void);
void wifi_set_disconnect_callback(wifi_disconnect_cb_t cb);
#endif