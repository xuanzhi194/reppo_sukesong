/**
 * 背光亮度与 LCD PWM 的桥接（放在 main，便于 GUI Guider 移植）
 */
#include "custom.h"
#include "lcd.h"

void brightness_apply_impl(uint8_t level)
{
    lcd_backlight_set_level(level);
}

void brightness_bridge_init(void)
{
    brightness_apply();
}
