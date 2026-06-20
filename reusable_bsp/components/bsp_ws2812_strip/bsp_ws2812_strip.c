#include "bsp_ws2812_strip.h"

#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

static led_strip_handle_t s_strip;
static bsp_ws2812_strip_config_t s_cfg;
static TaskHandle_t s_flow_task;
static uint8_t s_flow_r;
static uint8_t s_flow_g;
static uint8_t s_flow_b;
static uint32_t s_flow_step_ms;

esp_err_t bsp_ws2812_strip_init(const bsp_ws2812_strip_config_t *config)
{
    if (config == NULL || config->gpio_num < 0 || config->led_count == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    memcpy(&s_cfg, config, sizeof(s_cfg));

    led_strip_config_t strip_cfg = {
        .strip_gpio_num = s_cfg.gpio_num,
        .max_leds = s_cfg.led_count,
        .led_pixel_format = s_cfg.pixel_format,
        .led_model = s_cfg.led_model,
        .flags.invert_out = s_cfg.invert_out,
    };
    led_strip_rmt_config_t rmt_cfg = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = s_cfg.rmt_resolution_hz,
        .flags.with_dma = s_cfg.with_dma,
    };

    esp_err_t err = led_strip_new_rmt_device(&strip_cfg, &rmt_cfg, &s_strip);
    if (err != ESP_OK) {
        return err;
    }
    return bsp_ws2812_strip_clear();
}

esp_err_t bsp_ws2812_strip_deinit(void)
{
    if (s_flow_task != NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    if (s_strip != NULL) {
        led_strip_del(s_strip);
        s_strip = NULL;
    }
    return ESP_OK;
}

led_strip_handle_t bsp_ws2812_strip_handle(void)
{
    return s_strip;
}

esp_err_t bsp_ws2812_strip_clear(void)
{
    if (s_strip == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    return led_strip_clear(s_strip);
}

esp_err_t bsp_ws2812_strip_set_pixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b)
{
    if (s_strip == NULL || index >= s_cfg.led_count) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t err = led_strip_set_pixel(s_strip, index, r, g, b);
    if (err != ESP_OK) {
        return err;
    }
    return led_strip_refresh(s_strip);
}

esp_err_t bsp_ws2812_strip_fill(uint8_t r, uint8_t g, uint8_t b)
{
    if (s_strip == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    for (uint16_t i = 0; i < s_cfg.led_count; i++) {
        esp_err_t err = led_strip_set_pixel(s_strip, i, r, g, b);
        if (err != ESP_OK) {
            return err;
        }
    }
    return led_strip_refresh(s_strip);
}

esp_err_t bsp_ws2812_strip_flow(uint8_t r, uint8_t g, uint8_t b, uint32_t step_ms)
{
    if (s_strip == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    for (uint16_t i = 0; i < s_cfg.led_count; i++) {
        ESP_ERROR_CHECK_WITHOUT_ABORT(led_strip_set_pixel(s_strip, i, r, g, b));
        ESP_ERROR_CHECK_WITHOUT_ABORT(led_strip_refresh(s_strip));
        vTaskDelay(pdMS_TO_TICKS(step_ms));
        ESP_ERROR_CHECK_WITHOUT_ABORT(led_strip_clear(s_strip));
    }
    return led_strip_refresh(s_strip);
}

static void flow_task(void *param)
{
    (void)param;
    bsp_ws2812_strip_flow(s_flow_r, s_flow_g, s_flow_b, s_flow_step_ms);
    s_flow_task = NULL;
    vTaskDelete(NULL);
}

esp_err_t bsp_ws2812_strip_flow_async(uint8_t r, uint8_t g, uint8_t b,
                                      uint32_t step_ms, UBaseType_t priority)
{
    if (s_strip == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    if (s_flow_task != NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    s_flow_r = r;
    s_flow_g = g;
    s_flow_b = b;
    s_flow_step_ms = step_ms;

    BaseType_t ok = xTaskCreate(flow_task, "ws2812_flow", 2048, NULL, priority, &s_flow_task);
    return (ok == pdPASS) ? ESP_OK : ESP_ERR_NO_MEM;
}

bool bsp_ws2812_strip_flow_busy(void)
{
    return s_flow_task != NULL;
}
