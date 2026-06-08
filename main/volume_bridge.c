/**
 * 音量开关与蜂鸣器的桥接（放在 main，不放入 ui_custom，便于 GUI Guider 移植）
 */
#include "buzzer.h"
#include "custom.h"

static bool volume_is_muted(void)
{
    return !volume_en;
}

void volume_bridge_init(void)
{
    buzzer_bind_mute_check(volume_is_muted);
}

void volume_enabled_impl(void)
{
    buzzer_beep_confirm();
}

void volume_disabled_impl(void)
{
    buzzer_set_sound(2000, 0);
}
