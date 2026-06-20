#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "driver/gpio.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BSP_ENCODER_EVENT_NONE = 0,
    BSP_ENCODER_EVENT_CW = 1,
    BSP_ENCODER_EVENT_CCW = 2,
} bsp_encoder_event_t;

typedef struct {
    gpio_num_t gpio_a;
    gpio_num_t gpio_b;
    int step_cw;
    int step_ccw;
    int64_t event_gap_us;
    int64_t idle_reset_us;
    int reverse_noise;
    int reverse_budget;
    int64_t dir_lock_us;
    int dominance_margin;
    uint32_t glitch_filter_ns;
    bool invert_direction;
} bsp_encoder_pcnt_config_t;

#define BSP_ENCODER_PCNT_DEFAULT_CONFIG() { \
    .gpio_a = GPIO_NUM_1, \
    .gpio_b = GPIO_NUM_2, \
    .step_cw = 8, \
    .step_ccw = 8, \
    .event_gap_us = 50000, \
    .idle_reset_us = 150000, \
    .reverse_noise = 1, \
    .reverse_budget = 5, \
    .dir_lock_us = 350000, \
    .dominance_margin = 2, \
    .glitch_filter_ns = 12000, \
    .invert_direction = false, \
}

esp_err_t bsp_encoder_pcnt_init(const bsp_encoder_pcnt_config_t *config);
esp_err_t bsp_encoder_pcnt_reset(void);
bsp_encoder_event_t bsp_encoder_pcnt_poll(void);

#ifdef __cplusplus
}
#endif

