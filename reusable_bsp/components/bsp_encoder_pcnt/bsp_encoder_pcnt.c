#include "bsp_encoder_pcnt.h"

#include "driver/pulse_cnt.h"
#include "esp_check.h"
#include "esp_timer.h"
#include <stdlib.h>
#include <string.h>

static const char *TAG = "bsp_encoder_pcnt";

static pcnt_unit_handle_t s_unit;
static bsp_encoder_pcnt_config_t s_cfg;
static int32_t s_accumulated;
static int64_t s_last_emit_us = -1;
static int64_t s_last_motion_us = -1;
static bool s_release_pending;
static int8_t s_locked_dir;
static int32_t s_reverse_budget;
static int32_t s_dir_balance;

static int step_for_sign(int sign)
{
    return sign > 0 ? s_cfg.step_cw : s_cfg.step_ccw;
}

static void cap_to_one_step(void)
{
    if (s_accumulated == 0) {
        return;
    }
    int sign = s_accumulated > 0 ? 1 : -1;
    int step = step_for_sign(sign);
    if (abs((int)s_accumulated) > step) {
        s_accumulated = sign * step;
    }
}

esp_err_t bsp_encoder_pcnt_reset(void)
{
    s_accumulated = 0;
    s_release_pending = false;
    s_locked_dir = 0;
    s_reverse_budget = 0;
    s_dir_balance = 0;
    if (s_unit != NULL) {
        return pcnt_unit_clear_count(s_unit);
    }
    return ESP_OK;
}

esp_err_t bsp_encoder_pcnt_init(const bsp_encoder_pcnt_config_t *config)
{
    if (config == NULL || config->gpio_a < 0 || config->gpio_b < 0 ||
        config->step_cw <= 0 || config->step_ccw <= 0) {
        return ESP_ERR_INVALID_ARG;
    }
    memcpy(&s_cfg, config, sizeof(s_cfg));

    gpio_set_pull_mode(s_cfg.gpio_a, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(s_cfg.gpio_b, GPIO_PULLUP_ONLY);

    pcnt_unit_config_t unit_cfg = {
        .high_limit = 1000,
        .low_limit = -1000,
    };
    ESP_RETURN_ON_ERROR(pcnt_new_unit(&unit_cfg, &s_unit), TAG, "pcnt_new_unit failed");

    pcnt_glitch_filter_config_t filter_cfg = {
        .max_glitch_ns = s_cfg.glitch_filter_ns,
    };
    ESP_RETURN_ON_ERROR(pcnt_unit_set_glitch_filter(s_unit, &filter_cfg), TAG, "filter failed");

    pcnt_channel_handle_t chan_a = NULL;
    pcnt_chan_config_t chan_a_cfg = {
        .edge_gpio_num = s_cfg.gpio_a,
        .level_gpio_num = s_cfg.gpio_b,
    };
    ESP_RETURN_ON_ERROR(pcnt_new_channel(s_unit, &chan_a_cfg, &chan_a), TAG, "chan_a failed");

    pcnt_channel_handle_t chan_b = NULL;
    pcnt_chan_config_t chan_b_cfg = {
        .edge_gpio_num = s_cfg.gpio_b,
        .level_gpio_num = s_cfg.gpio_a,
    };
    ESP_RETURN_ON_ERROR(pcnt_new_channel(s_unit, &chan_b_cfg, &chan_b), TAG, "chan_b failed");

    ESP_RETURN_ON_ERROR(pcnt_channel_set_edge_action(
        chan_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE), TAG, "edge a");
    ESP_RETURN_ON_ERROR(pcnt_channel_set_level_action(
        chan_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE), TAG, "level a");
    ESP_RETURN_ON_ERROR(pcnt_channel_set_edge_action(
        chan_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE), TAG, "edge b");
    ESP_RETURN_ON_ERROR(pcnt_channel_set_level_action(
        chan_b, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE), TAG, "level b");

    ESP_RETURN_ON_ERROR(pcnt_unit_enable(s_unit), TAG, "enable failed");
    ESP_RETURN_ON_ERROR(pcnt_unit_clear_count(s_unit), TAG, "clear failed");
    ESP_RETURN_ON_ERROR(pcnt_unit_start(s_unit), TAG, "start failed");
    return bsp_encoder_pcnt_reset();
}

bsp_encoder_event_t bsp_encoder_pcnt_poll(void)
{
    if (s_unit == NULL) {
        return BSP_ENCODER_EVENT_NONE;
    }

    int curr_hw_count = 0;
    int64_t now = esp_timer_get_time();
    if (pcnt_unit_get_count(s_unit, &curr_hw_count) != ESP_OK) {
        return BSP_ENCODER_EVENT_NONE;
    }

    if (s_last_motion_us >= 0 && (now - s_last_motion_us) > s_cfg.idle_reset_us) {
        s_last_emit_us = -1;
        if (curr_hw_count == 0) {
            bsp_encoder_pcnt_reset();
            return BSP_ENCODER_EVENT_NONE;
        }
        bsp_encoder_pcnt_reset();
    }

    if (s_locked_dir != 0 && s_last_emit_us >= 0 &&
        (now - s_last_emit_us) > s_cfg.dir_lock_us) {
        s_locked_dir = 0;
    }

    if (curr_hw_count != 0) {
        s_last_motion_us = now;
        int new_sign = curr_hw_count > 0 ? 1 : -1;
        int est_dir = s_locked_dir;
        if (est_dir == 0 && s_accumulated != 0) {
            est_dir = s_accumulated > 0 ? 1 : -1;
        }

        bool consumed = false;
        if (est_dir != 0 && new_sign != est_dir &&
            abs(curr_hw_count) <= s_cfg.reverse_noise) {
            s_reverse_budget += abs(curr_hw_count);
            consumed = true;
            if (s_reverse_budget >= s_cfg.reverse_budget) {
                s_accumulated = 0;
                s_locked_dir = 0;
                s_reverse_budget = 0;
                s_dir_balance = 0;
                s_accumulated += curr_hw_count;
                s_dir_balance += curr_hw_count;
                cap_to_one_step();
            }
        }

        if (!consumed) {
            if (est_dir != 0 && new_sign != est_dir) {
                s_accumulated = 0;
                s_locked_dir = 0;
                s_dir_balance = 0;
            }
            if (new_sign == est_dir || est_dir == 0) {
                s_reverse_budget = 0;
            }
            s_accumulated += curr_hw_count;
            s_dir_balance += curr_hw_count;
            int active_step = step_for_sign(s_dir_balance >= 0 ? 1 : -1);
            if (s_dir_balance > active_step) {
                s_dir_balance = active_step;
            }
            if (s_dir_balance < -active_step) {
                s_dir_balance = -active_step;
            }
            cap_to_one_step();
        }
        pcnt_unit_clear_count(s_unit);
    }

    if (s_release_pending) {
        s_release_pending = false;
        return BSP_ENCODER_EVENT_NONE;
    }
    if (s_accumulated == 0) {
        return BSP_ENCODER_EVENT_NONE;
    }

    int sign = s_accumulated > 0 ? 1 : -1;
    int step = step_for_sign(sign);
    if (abs((int)s_accumulated) < step) {
        return BSP_ENCODER_EVENT_NONE;
    }
    if (sign > 0 && s_dir_balance < s_cfg.dominance_margin) {
        return BSP_ENCODER_EVENT_NONE;
    }
    if (sign < 0 && s_dir_balance > -s_cfg.dominance_margin) {
        return BSP_ENCODER_EVENT_NONE;
    }
    if (s_last_emit_us >= 0 && (now - s_last_emit_us) < s_cfg.event_gap_us) {
        return BSP_ENCODER_EVENT_NONE;
    }

    s_last_emit_us = now;
    s_release_pending = true;
    s_accumulated = 0;
    s_reverse_budget = 0;
    s_dir_balance = 0;
    s_locked_dir = sign;

    if (s_cfg.invert_direction) {
        sign = -sign;
    }
    return sign > 0 ? BSP_ENCODER_EVENT_CW : BSP_ENCODER_EVENT_CCW;
}

