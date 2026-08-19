/**
 * 背光亮度与 LCD PWM 的桥接（放在 main，便于 GUI Guider 移植）
 */
#include "custom.h"
#include "lcd.h"
#include "nvs.h"
#include "esp_log.h"

#define SETTINGS_NVS_NS    "Settings"
#define SETTINGS_NVS_BRT   "bright"
#define BRIGHTNESS_LEVELS  3U

static const char *TAG = "BRIGHTNESS";

static void brightness_prefs_save(uint8_t level)
{
    nvs_handle_t h;
    if (nvs_open(SETTINGS_NVS_NS, NVS_READWRITE, &h) != ESP_OK) {
        ESP_LOGW(TAG, "nvs open failed");
        return;
    }
    nvs_set_u8(h, SETTINGS_NVS_BRT, level);
    nvs_commit(h);
    nvs_close(h);
}

static void brightness_prefs_load(void)
{
    nvs_handle_t h;
    uint8_t stored = 0;
    if (nvs_open(SETTINGS_NVS_NS, NVS_READONLY, &h) != ESP_OK) {
        return;
    }
    if (nvs_get_u8(h, SETTINGS_NVS_BRT, &stored) == ESP_OK &&
        stored < BRIGHTNESS_LEVELS) {
        brightness_level = stored;
    }
    nvs_close(h);
}

void brightness_apply_impl(uint8_t level)
{
    lcd_backlight_set_level(level);
}

void brightness_persist_impl(uint8_t level)
{
    brightness_prefs_save(level);
}

void brightness_bridge_init(void)
{
    brightness_prefs_load();
    /* PWM 尚未就绪时只记下百分比，lcd_backlight_init 会按该值点亮。 */
    lcd_backlight_prefetch_level(brightness_level);
}
