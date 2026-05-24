#include "wifi_cred_storage.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include <string.h>

#define TAG           "WIFI_CRED"
#define NVS_NS        "WifiCred"
#define NVS_KEY_STORE "cred_store"

typedef struct {
    char ssid[WIFI_CRED_SSID_LEN];
    char password[WIFI_CRED_PASS_LEN];
} wifi_cred_entry_t;

typedef struct {
    uint8_t count;
    wifi_cred_entry_t entries[WIFI_CRED_MAX_ENTRIES];
} wifi_cred_store_t;

static wifi_cred_store_t s_store;

static esp_err_t cred_store_load(void)
{
    nvs_handle_t h;
    esp_err_t err = nvs_open(NVS_NS, NVS_READONLY, &h);
    if (err != ESP_OK) {
        return err;
    }

    size_t sz = sizeof(s_store);
    err = nvs_get_blob(h, NVS_KEY_STORE, &s_store, &sz);
    nvs_close(h);

    if (err == ESP_OK && sz == sizeof(s_store) && s_store.count <= WIFI_CRED_MAX_ENTRIES) {
        return ESP_OK;
    }

    memset(&s_store, 0, sizeof(s_store));
    return (err == ESP_ERR_NVS_NOT_FOUND) ? ESP_OK : err;
}

static esp_err_t cred_store_commit(void)
{
    nvs_handle_t h;
    esp_err_t err = nvs_open(NVS_NS, NVS_READWRITE, &h);
    if (err != ESP_OK) {
        return err;
    }

    err = nvs_set_blob(h, NVS_KEY_STORE, &s_store, sizeof(s_store));
    if (err == ESP_OK) {
        err = nvs_commit(h);
    }
    nvs_close(h);
    return err;
}

esp_err_t wifi_cred_storage_init(void)
{
    memset(&s_store, 0, sizeof(s_store));
    esp_err_t err = cred_store_load();
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Loaded %u saved WiFi credential(s)", (unsigned)s_store.count);
    }
    return err;
}

bool wifi_cred_get_password(const char *ssid, char *password, size_t password_len)
{
    if (!ssid || !password || password_len == 0) {
        return false;
    }

    for (uint8_t i = 0; i < s_store.count; i++) {
        if (strcmp(s_store.entries[i].ssid, ssid) == 0) {
            strncpy(password, s_store.entries[i].password, password_len - 1);
            password[password_len - 1] = '\0';
            return true;
        }
    }
    return false;
}

esp_err_t wifi_cred_save(const char *ssid, const char *password)
{
    if (!ssid || ssid[0] == '\0') {
        return ESP_ERR_INVALID_ARG;
    }

    const char *pw = password ? password : "";

    for (uint8_t i = 0; i < s_store.count; i++) {
        if (strcmp(s_store.entries[i].ssid, ssid) == 0) {
            strncpy(s_store.entries[i].password, pw, WIFI_CRED_PASS_LEN - 1);
            s_store.entries[i].password[WIFI_CRED_PASS_LEN - 1] = '\0';
            ESP_LOGI(TAG, "Updated saved password for \"%s\"", ssid);
            return cred_store_commit();
        }
    }

    if (s_store.count >= WIFI_CRED_MAX_ENTRIES) {
        /* Drop oldest entry to make room */
        memmove(&s_store.entries[0], &s_store.entries[1],
                (WIFI_CRED_MAX_ENTRIES - 1) * sizeof(wifi_cred_entry_t));
        s_store.count = WIFI_CRED_MAX_ENTRIES - 1;
    }

    wifi_cred_entry_t *e = &s_store.entries[s_store.count];
    strncpy(e->ssid, ssid, WIFI_CRED_SSID_LEN - 1);
    e->ssid[WIFI_CRED_SSID_LEN - 1] = '\0';
    strncpy(e->password, pw, WIFI_CRED_PASS_LEN - 1);
    e->password[WIFI_CRED_PASS_LEN - 1] = '\0';
    s_store.count++;

    ESP_LOGI(TAG, "Saved WiFi credential for \"%s\" (%u total)", ssid, (unsigned)s_store.count);
    return cred_store_commit();
}
