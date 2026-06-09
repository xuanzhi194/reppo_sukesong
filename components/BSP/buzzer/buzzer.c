#include "buzzer.h"

static bool (*s_mute_check)(void);

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
        // update frequency
        ledc_set_freq(BUZZER_LEDC_MODE, BUZZER_LEDC_TIMER, freq);
        // set duty cycle (2^13 / 2 = 4096)
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
    if (buzzer_muted()) {
        return;
    }
    uint32_t alarm_freq = 2700;
    uint8_t volume = 35;  /* 降低占空比，减轻 3.3V 纹波对背光/LCD 的影响 */

    for (int round = 0; round < 5; round++) {
        for(int i = 0; i < 3; i++) {
            buzzer_set_sound(alarm_freq, volume);   
            vTaskDelay(pdMS_TO_TICKS(200));       
            buzzer_set_sound(alarm_freq, 0);       
            vTaskDelay(pdMS_TO_TICKS(200));         
        }
        vTaskDelay(pdMS_TO_TICKS(400)); 
    }
    buzzer_set_sound(alarm_freq, 0);
}