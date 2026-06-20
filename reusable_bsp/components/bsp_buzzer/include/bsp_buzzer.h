#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "driver/ledc.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef bool (*bsp_buzzer_mute_cb_t)(void *user_ctx);

typedef struct {
    int gpio_num;
    ledc_mode_t speed_mode;
    ledc_timer_t timer_num;
    ledc_channel_t channel;
    ledc_timer_bit_t duty_resolution;
    uint32_t initial_freq_hz;
    bsp_buzzer_mute_cb_t mute_cb;
    void *mute_user_ctx;
} bsp_buzzer_config_t;

#define BSP_BUZZER_DEFAULT_CONFIG() { \
    .gpio_num = 7, \
    .speed_mode = LEDC_LOW_SPEED_MODE, \
    .timer_num = LEDC_TIMER_0, \
    .channel = LEDC_CHANNEL_0, \
    .duty_resolution = LEDC_TIMER_13_BIT, \
    .initial_freq_hz = 2000, \
    .mute_cb = NULL, \
    .mute_user_ctx = NULL, \
}

esp_err_t bsp_buzzer_init(const bsp_buzzer_config_t *config);
void bsp_buzzer_set_mute_cb(bsp_buzzer_mute_cb_t cb, void *user_ctx);
esp_err_t bsp_buzzer_set_sound(uint32_t freq_hz, uint8_t volume_percent);
esp_err_t bsp_buzzer_stop(void);
esp_err_t bsp_buzzer_beep(uint32_t freq_hz, uint8_t volume_percent, uint32_t duration_ms);
esp_err_t bsp_buzzer_alarm(uint32_t freq_hz, uint8_t volume_percent,
                           uint8_t groups, uint8_t beeps_per_group,
                           uint32_t on_ms, uint32_t off_ms,
                           uint32_t group_gap_ms);

#ifdef __cplusplus
}
#endif

