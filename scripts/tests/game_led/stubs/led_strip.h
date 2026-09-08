#pragma once
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

typedef void *led_strip_handle_t;
typedef int esp_err_t;
#define ESP_OK 0
#define ESP_FAIL -1
#define ESP_ERROR_CHECK(expr) assert((expr) == ESP_OK)
#define LED_PIXEL_FORMAT_GRB 1
#define LED_MODEL_WS2812 1
#define RMT_CLK_SRC_DEFAULT 0

typedef struct {
    int strip_gpio_num;
    int max_leds;
    int led_pixel_format;
    int led_model;
    struct { bool invert_out; } flags;
} led_strip_config_t;

typedef struct {
    int clk_src;
    int resolution_hz;
    struct { bool with_dma; } flags;
} led_strip_rmt_config_t;

esp_err_t led_strip_new_rmt_device(const led_strip_config_t *strip_config,
                                  const led_strip_rmt_config_t *rmt_config,
                                  led_strip_handle_t *handle);
esp_err_t led_strip_set_pixel(led_strip_handle_t handle, uint32_t index,
                              uint32_t red, uint32_t green, uint32_t blue);
esp_err_t led_strip_refresh(led_strip_handle_t handle);
esp_err_t led_strip_clear(led_strip_handle_t handle);
