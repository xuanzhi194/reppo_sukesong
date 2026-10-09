#include "buzzer.h"

static bool (*s_mute_check)(void);
static volatile bool s_alarm_abort;

void buzzer_alarm_abort(void)
{
    s_alarm_abort = true;
}

void buzzer_alarm_abort_clear(void)
{
    s_alarm_abort = false;
}

bool buzzer_alarm_is_aborted(void)
{
    return s_alarm_abort;
}

/** 拆成 20 ms，便于晃动或按键中途停掉。返回 true 表示已经静音并应结束整段闹钟。 */
static bool buzzer_wait(uint32_t ms)
{
    while (ms > 0) {
        if (s_alarm_abort) {
            buzzer_set_sound(2700, 0);
            return true;
        }
        uint32_t step = ms > 20 ? 20 : ms;
        vTaskDelay(pdMS_TO_TICKS(step));
        ms -= step;
    }
    if (s_alarm_abort) {
        buzzer_set_sound(2700, 0);
        return true;
    }
    return false;
}

void buzzer_bind_mute_check(bool (*fn)(void))
{
    s_mute_check = fn;
}

static bool buzzer_muted(void)
{
    return s_mute_check != NULL && s_mute_check();
}

// 定义配置参数
#define BUZZER_IO           (7)               // 使用 IO7
#define BUZZER_LEDC_TIMER   LEDC_TIMER_0
#define BUZZER_LEDC_MODE    LEDC_LOW_SPEED_MODE
#define BUZZER_LEDC_CHANNEL LEDC_CHANNEL_0
#define BUZZER_DUTY_RES     LEDC_TIMER_13_BIT // 13位分辨率
#define BUZZER_INITIAL_FREQ (2000)            // 初始频率 2kHz

/**
 * 初始化 PWM 配置
 */
void buzzer_init() {
    // 1. 定时器配置
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = BUZZER_LEDC_MODE,
        .timer_num        = BUZZER_LEDC_TIMER,
        .duty_resolution  = BUZZER_DUTY_RES,
        .freq_hz          = BUZZER_INITIAL_FREQ,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    // 2. 通道配置
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = BUZZER_LEDC_MODE,
        .channel        = BUZZER_LEDC_CHANNEL,
        .timer_sel      = BUZZER_LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = BUZZER_IO,
        .duty           = 0, // init close
        .hpoint         = 0
    };
    ledc_channel_config(&ledc_channel);
}

/**
 * setting booming
 * @param freq 
 * @param state
 */
void buzzer_set_sound(uint32_t freq, uint8_t volume_percent) {
        if (buzzer_muted()) {
            volume_percent = 0;
        }

        /* volume=0 时仅关断输出；不可把 freq 设为 0，否则 ledc_set_freq 会除零崩溃 */
        if (volume_percent == 0) {
            ledc_set_duty(BUZZER_LEDC_MODE, BUZZER_LEDC_CHANNEL, 0);
            ledc_update_duty(BUZZER_LEDC_MODE, BUZZER_LEDC_CHANNEL);
            return;
        }

        if (freq == 0) {
            freq = BUZZER_INITIAL_FREQ;
        }

        ledc_set_freq(BUZZER_LEDC_MODE, BUZZER_LEDC_TIMER, freq);
        uint32_t duty_param = (8192 * volume_percent) / 100;
        ledc_set_duty(BUZZER_LEDC_MODE, BUZZER_LEDC_CHANNEL, duty_param);
        ledc_update_duty(BUZZER_LEDC_MODE, BUZZER_LEDC_CHANNEL);
}

void buzzer_beep(void)
{
    if (buzzer_muted()) {
        return;
    }
    buzzer_set_sound(2000, 45);
    vTaskDelay(pdMS_TO_TICKS(80));
    buzzer_set_sound(2000, 0);
}

void buzzer_beep_confirm(void)
{
    if (buzzer_muted()) {
        return;
    }
    buzzer_set_sound(2400, 95);
    vTaskDelay(pdMS_TO_TICKS(150));
    buzzer_set_sound(2400, 0);
}

//buzzer alarm combination
void buzzer_timeout() {
    if (buzzer_muted() || s_alarm_abort) {
        buzzer_set_sound(2700, 0);
        return;
    }
    uint32_t alarm_freq = 2700;
    uint8_t volume = 35;  /* 降低占空比，减轻 3.3V 纹波对背光/LCD 的影响 */

    for (int round = 0; round < 5; round++) {
        for(int i = 0; i < 3; i++) {
            if (s_alarm_abort) {
                buzzer_set_sound(alarm_freq, 0);
                return;
            }
            buzzer_set_sound(alarm_freq, volume);
            if (buzzer_wait(200)) {
                return;
            }
            buzzer_set_sound(alarm_freq, 0);
            if (buzzer_wait(200)) {
                return;
            }
        }
        if (buzzer_wait(400)) {
            return;
        }
    }
    buzzer_set_sound(alarm_freq, 0);
}