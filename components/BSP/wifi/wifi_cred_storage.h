#ifndef WIFI_CRED_STORAGE_H
#define WIFI_CRED_STORAGE_H

#include <stdbool.h>
#include <stddef.h>
#include "esp_err.h"

#define WIFI_CRED_MAX_ENTRIES  8
#define WIFI_CRED_SSID_LEN     33
#define WIFI_CRED_PASS_LEN     33

esp_err_t wifi_cred_storage_init(void);
bool wifi_cred_get_password(const char *ssid, char *password, size_t password_len);
esp_err_t wifi_cred_save(const char *ssid, const char *password);

#endif
