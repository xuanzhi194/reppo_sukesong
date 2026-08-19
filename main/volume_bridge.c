/**
 * 音量开关与蜂鸣器的桥接（放在 main，不放入 ui_custom，便于 GUI Guider 移植）
 */
#include "buzzer.h"
#include "custom.h"
#include "nvs.h"
#include "esp_log.h"

#define SETTINGS_NVS_NS   "Settings"
#define SETTINGS_NVS_VOL  "vol_en"

static const char *TAG = "VOLUME";

static bool volume_is_muted(void)
{
    return !volume_en;
}

static void volume_prefs_save(void)
{
    nvs_handle_t h;
    if (nvs_open(SETTINGS_NVS_NS, NVS_READWRITE, &h) != ESP_OK) {
        ESP_LOGW(TAG, "nvs open failed");
        return;
    }
    nvs_set_u8(h, SETTINGS_NVS_VOL, volume_en ? 1U : 0U);
    nvs_commit(h);
    nvs_close(h);
}

static void volume_prefs_load(void)
{
    nvs_handle_t h;
    uint8_t stored = 0;
    if (nvs_open(SETTINGS_NVS_NS, NVS_READONLY, &h) != ESP_OK) {
        return;
    }
    if (nvs_get_u8(h, SETTINGS_NVS_VOL, &stored) == ESP_OK) {
        volume_en = (stored != 0);
    }
    nvs_close(h);
}

void volume_bridge_init(void)
{
    volume_prefs_load();
    buzzer_bind_mute_check(volume_is_muted);
}

void volume_enabled_impl(void)
{
    volume_prefs_save();
    buzzer_beep_confirm();
}

void volume_disabled_impl(void)
{
    volume_prefs_save();
    buzzer_set_sound(2000, 0);
}
