#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "driver/gpio.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    gpio_num_t gpio_num;
    bool active_low;
    gpio_pullup_t pull_up_en;
    gpio_pulldown_t pull_down_en;
    uint32_t debounce_ms;
} bsp_button_config_t;

#define BSP_BUTTON_DEFAULT_CONFIG(gpio) { \
    .gpio_num = (gpio), \
    .active_low = true, \
    .pull_up_en = GPIO_PULLUP_ENABLE, \
    .pull_down_en = GPIO_PULLDOWN_DISABLE, \
    .debounce_ms = 10, \
}

esp_err_t bsp_button_init(const bsp_button_config_t *config);
bool bsp_button_is_pressed(gpio_num_t gpio_num, bool active_low);
bool bsp_button_read_debounced(const bsp_button_config_t *config);

#ifdef __cplusplus
}
#endif

