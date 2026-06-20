#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "led_strip.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int gpio_num;
    uint16_t led_count;
    led_pixel_format_t pixel_format;
    led_model_t led_model;
    uint32_t rmt_resolution_hz;
    bool invert_out;
    bool with_dma;
} bsp_ws2812_strip_config_t;

#define BSP_WS2812_STRIP_DEFAULT_CONFIG() { \
    .gpio_num = 17, \
    .led_count = 10, \
    .pixel_format = LED_PIXEL_FORMAT_GRB, \
    .led_model = LED_MODEL_WS2812, \
    .rmt_resolution_hz = 10 * 1000 * 1000, \
    .invert_out = false, \
    .with_dma = false, \
}

esp_err_t bsp_ws2812_strip_init(const bsp_ws2812_strip_config_t *config);
esp_err_t bsp_ws2812_strip_deinit(void);
led_strip_handle_t bsp_ws2812_strip_handle(void);
esp_err_t bsp_ws2812_strip_clear(void);
esp_err_t bsp_ws2812_strip_set_pixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b);
esp_err_t bsp_ws2812_strip_fill(uint8_t r, uint8_t g, uint8_t b);
esp_err_t bsp_ws2812_strip_flow(uint8_t r, uint8_t g, uint8_t b, uint32_t step_ms);
esp_err_t bsp_ws2812_strip_flow_async(uint8_t r, uint8_t g, uint8_t b,
                                      uint32_t step_ms, UBaseType_t priority);
bool bsp_ws2812_strip_flow_busy(void);

#ifdef __cplusplus
}
#endif
