/**
 * 息屏超时档位的 NVS 持久化（放在 main，不放入 ui_custom）
 */
#include "custom.h"
#include "nvs.h"
#include "esp_log.h"

#define SETTINGS_NVS_NS     "Settings"
#define SETTINGS_NVS_SLEEP  "sleep"

static const char *TAG = "SLEEP";

static void sleep_prefs_save(uint8_t level)
{
    nvs_handle_t h;
    if (nvs_open(SETTINGS_NVS_NS, NVS_READWRITE, &h) != ESP_OK) {
        ESP_LOGW(TAG, "nvs open failed");
        return;
    }
    nvs_set_u8(h, SETTINGS_NVS_SLEEP, level);
    nvs_commit(h);
    nvs_close(h);
}

static void sleep_prefs_load(void)
{
    nvs_handle_t h;
    uint8_t stored = 0;
    if (nvs_open(SETTINGS_NVS_NS, NVS_READONLY, &h) != ESP_OK) {
        return;
    }
    if (nvs_get_u8(h, SETTINGS_NVS_SLEEP, &stored) == ESP_OK &&
        stored < SLEEP_LEVEL_COUNT) {
        sleep_timeout_level = stored;
    }
    nvs_close(h);
}

void sleep_persist_impl(uint8_t level)
{
    if (level >= SLEEP_LEVEL_COUNT) {
        level = 0;
    }
    sleep_prefs_save(level);
}

void sleep_bridge_init(void)
{
    sleep_prefs_load();
}
