#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BSP_WIFI_CRED_MAX_ENTRIES
#define BSP_WIFI_CRED_MAX_ENTRIES 8
#endif

#ifndef BSP_WIFI_CRED_SSID_LEN
#define BSP_WIFI_CRED_SSID_LEN 33
#endif

#ifndef BSP_WIFI_CRED_PASS_LEN
#define BSP_WIFI_CRED_PASS_LEN 65
#endif

typedef struct {
    char ssid[BSP_WIFI_CRED_SSID_LEN];
    char password[BSP_WIFI_CRED_PASS_LEN];
} bsp_wifi_cred_entry_t;

esp_err_t bsp_wifi_cred_init(const char *nvs_namespace, const char *nvs_key);
esp_err_t bsp_wifi_cred_save(const char *ssid, const char *password);
bool bsp_wifi_cred_get_password(const char *ssid, char *password, size_t password_len);
esp_err_t bsp_wifi_cred_erase(const char *ssid);
esp_err_t bsp_wifi_cred_clear(void);
uint8_t bsp_wifi_cred_count(void);
bool bsp_wifi_cred_get_entry(uint8_t index, bsp_wifi_cred_entry_t *entry);

#ifdef __cplusplus
}
#endif

