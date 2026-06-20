#include "bsp_wifi_cred.h"

#include "nvs.h"
#include "nvs_flash.h"
#include <string.h>

#define DEFAULT_NVS_NS  "WifiCred"
#define DEFAULT_NVS_KEY "cred_store"

typedef struct {
    uint8_t count;
    bsp_wifi_cred_entry_t entries[BSP_WIFI_CRED_MAX_ENTRIES];
} cred_store_t;

static cred_store_t s_store;
static char s_ns[16] = DEFAULT_NVS_NS;
static char s_key[16] = DEFAULT_NVS_KEY;
static bool s_initialized;

static esp_err_t store_load(void)
{
    nvs_handle_t h;
    esp_err_t err = nvs_open(s_ns, NVS_READONLY, &h);
    if (err != ESP_OK) {
        memset(&s_store, 0, sizeof(s_store));
        return (err == ESP_ERR_NVS_NOT_FOUND) ? ESP_OK : err;
    }

    size_t size = sizeof(s_store);
    err = nvs_get_blob(h, s_key, &s_store, &size);
    nvs_close(h);

    if (err == ESP_ERR_NVS_NOT_FOUND) {
        memset(&s_store, 0, sizeof(s_store));
        return ESP_OK;
    }
    if (err != ESP_OK || size != sizeof(s_store) || s_store.count > BSP_WIFI_CRED_MAX_ENTRIES) {
        memset(&s_store, 0, sizeof(s_store));
        return (err == ESP_OK) ? ESP_ERR_INVALID_SIZE : err;
    }
    return ESP_OK;
}

static esp_err_t store_commit(void)
{
    nvs_handle_t h;
    esp_err_t err = nvs_open(s_ns, NVS_READWRITE, &h);
    if (err != ESP_OK) {
        return err;
    }
    err = nvs_set_blob(h, s_key, &s_store, sizeof(s_store));
    if (err == ESP_OK) {
        err = nvs_commit(h);
    }
    nvs_close(h);
    return err;
}

esp_err_t bsp_wifi_cred_init(const char *nvs_namespace, const char *nvs_key)
{
    if (nvs_namespace && nvs_namespace[0]) {
        strncpy(s_ns, nvs_namespace, sizeof(s_ns) - 1);
        s_ns[sizeof(s_ns) - 1] = '\0';
    }
    if (nvs_key && nvs_key[0]) {
        strncpy(s_key, nvs_key, sizeof(s_key) - 1);
        s_key[sizeof(s_key) - 1] = '\0';
    }

    memset(&s_store, 0, sizeof(s_store));
    esp_err_t err = store_load();
    if (err == ESP_OK) {
        s_initialized = true;
    }
    return err;
}

esp_err_t bsp_wifi_cred_save(const char *ssid, const char *password)
{
    if (!s_initialized || ssid == NULL || ssid[0] == '\0') {
        return ESP_ERR_INVALID_ARG;
    }

    const char *pw = password ? password : "";
    for (uint8_t i = 0; i < s_store.count; i++) {
        if (strcmp(s_store.entries[i].ssid, ssid) == 0) {
            strncpy(s_store.entries[i].password, pw, BSP_WIFI_CRED_PASS_LEN - 1);
            s_store.entries[i].password[BSP_WIFI_CRED_PASS_LEN - 1] = '\0';
            return store_commit();
        }
    }

    if (s_store.count >= BSP_WIFI_CRED_MAX_ENTRIES) {
        memmove(&s_store.entries[0], &s_store.entries[1],
                (BSP_WIFI_CRED_MAX_ENTRIES - 1) * sizeof(bsp_wifi_cred_entry_t));
        s_store.count = BSP_WIFI_CRED_MAX_ENTRIES - 1;
    }

    bsp_wifi_cred_entry_t *entry = &s_store.entries[s_store.count++];
    memset(entry, 0, sizeof(*entry));
    strncpy(entry->ssid, ssid, BSP_WIFI_CRED_SSID_LEN - 1);
    strncpy(entry->password, pw, BSP_WIFI_CRED_PASS_LEN - 1);
    return store_commit();
}

bool bsp_wifi_cred_get_password(const char *ssid, char *password, size_t password_len)
{
    if (!s_initialized || ssid == NULL || password == NULL || password_len == 0) {
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

esp_err_t bsp_wifi_cred_erase(const char *ssid)
{
    if (!s_initialized || ssid == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    for (uint8_t i = 0; i < s_store.count; i++) {
        if (strcmp(s_store.entries[i].ssid, ssid) == 0) {
            memmove(&s_store.entries[i], &s_store.entries[i + 1],
                    (s_store.count - i - 1) * sizeof(bsp_wifi_cred_entry_t));
            s_store.count--;
            memset(&s_store.entries[s_store.count], 0, sizeof(bsp_wifi_cred_entry_t));
            return store_commit();
        }
    }
    return ESP_ERR_NOT_FOUND;
}

esp_err_t bsp_wifi_cred_clear(void)
{
    if (!s_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    memset(&s_store, 0, sizeof(s_store));
    return store_commit();
}

uint8_t bsp_wifi_cred_count(void)
{
    return s_store.count;
}

bool bsp_wifi_cred_get_entry(uint8_t index, bsp_wifi_cred_entry_t *entry)
{
    if (!s_initialized || entry == NULL || index >= s_store.count) {
        return false;
    }
    *entry = s_store.entries[index];
    return true;
}

