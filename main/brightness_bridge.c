/**
 * 背光亮度与 LCD PWM 的桥接（放在 main，便于 GUI Guider 移植）
 */
#include "custom.h"
#include "lcd.h"
#include "nvs.h"
#include "esp_log.h"

#if BRIGHTNESS_LEVEL_COUNT != LCD_BACKLIGHT_LEVEL_COUNT
#error "UI brightness steps must match LCD backlight levels"
#endif

#define SETTINGS_NVS_NS    "Settings"
#define SETTINGS_NVS_BRT   "bright"
#define SETTINGS_NVS_BRT_N "brt_n"

static const char *TAG = "BRIGHTNESS";
/* 旧三档 30/65/100 → 新五档里最接近的室内/明亮/最大 */
static const uint8_t s_legacy_level_map[3] = { 2, 3, 4 };

static void brightness_prefs_save(uint8_t level)
{
    nvs_handle_t h;
    if (nvs_open(SETTINGS_NVS_NS, NVS_READWRITE, &h) != ESP_OK) {
        ESP_LOGW(TAG, "nvs open failed");
        return;
    }
    nvs_set_u8(h, SETTINGS_NVS_BRT, level);
    nvs_set_u8(h, SETTINGS_NVS_BRT_N, (uint8_t)LCD_BACKLIGHT_LEVEL_COUNT);
    nvs_commit(h);
    nvs_close(h);
}

static void brightness_prefs_load(void)
{
    nvs_handle_t h;
    uint8_t stored = 0;
    uint8_t schema = 0;
    if (nvs_open(SETTINGS_NVS_NS, NVS_READONLY, &h) != ESP_OK) {
        return;
    }
    if (nvs_get_u8(h, SETTINGS_NVS_BRT, &stored) != ESP_OK) {
        nvs_close(h);
        return;
    }
    if (nvs_get_u8(h, SETTINGS_NVS_BRT_N, &schema) == ESP_OK &&
        schema == (uint8_t)LCD_BACKLIGHT_LEVEL_COUNT &&
        stored < LCD_BACKLIGHT_LEVEL_COUNT) {
        brightness_level = stored;
        nvs_close(h);
        return;
    }
    nvs_close(h);

    /* 无档位数标记时按旧三档迁移，避免原先最高档被读成新的中间档 */
    if (stored < 3U) {
        brightness_level = s_legacy_level_map[stored];
        brightness_prefs_save(brightness_level);
    }
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
