#include "bsp_buzzer.h"

#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

static bsp_buzzer_config_t s_cfg;
static bool s_initialized;

static bool buzzer_muted(void)
{
    return s_cfg.mute_cb != NULL && s_cfg.mute_cb(s_cfg.mute_user_ctx);
}

static uint32_t duty_max(void)
{
    return (1UL << s_cfg.duty_resolution) - 1UL;
}

esp_err_t bsp_buzzer_init(const bsp_buzzer_config_t *config)
{
    if (config == NULL || config->gpio_num < 0 || config->initial_freq_hz == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    memcpy(&s_cfg, config, sizeof(s_cfg));

    ledc_timer_config_t timer = {
        .speed_mode = s_cfg.speed_mode,
        .timer_num = s_cfg.timer_num,
        .duty_resolution = s_cfg.duty_resolution,
        .freq_hz = s_cfg.initial_freq_hz,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    esp_err_t err = ledc_timer_config(&timer);
    if (err != ESP_OK) {
        return err;
    }

    ledc_channel_config_t channel = {
        .speed_mode = s_cfg.speed_mode,
        .channel = s_cfg.channel,
        .timer_sel = s_cfg.timer_num,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = s_cfg.gpio_num,
        .duty = 0,
        .hpoint = 0,
    };
    err = ledc_channel_config(&channel);
    if (err == ESP_OK) {
        s_initialized = true;
    }
    return err;
}

void bsp_buzzer_set_mute_cb(bsp_buzzer_mute_cb_t cb, void *user_ctx)
{
    s_cfg.mute_cb = cb;
    s_cfg.mute_user_ctx = user_ctx;
}

esp_err_t bsp_buzzer_set_sound(uint32_t freq_hz, uint8_t volume_percent)
{
    if (!s_initialized || freq_hz == 0 || volume_percent > 100) {
        return ESP_ERR_INVALID_STATE;
    }
    if (buzzer_muted()) {
        volume_percent = 0;
    }

    esp_err_t err = ledc_set_freq(s_cfg.speed_mode, s_cfg.timer_num, freq_hz);
    if (err != ESP_OK) {
        return err;
    }

    uint32_t duty = duty_max() * volume_percent / 100U;
    err = ledc_set_duty(s_cfg.speed_mode, s_cfg.channel, duty);
    if (err != ESP_OK) {
        return err;
    }
    return ledc_update_duty(s_cfg.speed_mode, s_cfg.channel);
}

esp_err_t bsp_buzzer_stop(void)
{
    return bsp_buzzer_set_sound(s_cfg.initial_freq_hz ? s_cfg.initial_freq_hz : 2000, 0);
}

esp_err_t bsp_buzzer_beep(uint32_t freq_hz, uint8_t volume_percent, uint32_t duration_ms)
{
    if (buzzer_muted()) {
        return ESP_OK;
    }
    esp_err_t err = bsp_buzzer_set_sound(freq_hz, volume_percent);
    if (err != ESP_OK) {
        return err;
    }
    vTaskDelay(pdMS_TO_TICKS(duration_ms));
    return bsp_buzzer_stop();
}

esp_err_t bsp_buzzer_alarm(uint32_t freq_hz, uint8_t volume_percent,
                           uint8_t groups, uint8_t beeps_per_group,
                           uint32_t on_ms, uint32_t off_ms,
                           uint32_t group_gap_ms)
{
    if (buzzer_muted()) {
        return ESP_OK;
    }
    for (uint8_t g = 0; g < groups; g++) {
        for (uint8_t i = 0; i < beeps_per_group; i++) {
            ESP_RETURN_ON_ERROR(bsp_buzzer_set_sound(freq_hz, volume_percent), "bsp_buzzer", "sound on failed");
            vTaskDelay(pdMS_TO_TICKS(on_ms));
            ESP_RETURN_ON_ERROR(bsp_buzzer_stop(), "bsp_buzzer", "sound off failed");
            vTaskDelay(pdMS_TO_TICKS(off_ms));
        }
        vTaskDelay(pdMS_TO_TICKS(group_gap_ms));
    }
    return bsp_buzzer_stop();
}
