#include "ws2812b.h"

// Define GPIO15 according to the schematic [cite: 197]
#define LED_STRIP_GPIO_PIN  17
// 10 LEDs cascaded in series [cite: 51, 94]
#define LED_STRIP_NUM_LEDS  10 

static led_strip_handle_t led_strip;
void init_led_strip() {
    /* 1. LED strip general configuration */
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_STRIP_GPIO_PIN,
        .max_leds = LED_STRIP_NUM_LEDS,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB, // WS2812B uses GRB data structure [cite: 531]
        .led_model = LED_MODEL_WS2812,
        .flags.invert_out = false,
    };

    /* 2. RMT peripheral specific configuration */
    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000, // 10MHz resolution for precise timing
        .flags.with_dma = false,
    };

    /* Create the LED strip object using RMT backend */
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    
    /* Clear all LEDs (Power-on reset state) */
    led_strip_clear(led_strip);
}

/**
 * @brief Simple flow light effect
 */
void set_flow_light(uint32_t r, uint32_t g, uint32_t b) {
    for (int i = 0; i < LED_STRIP_NUM_LEDS; i++) {
        // Set individual pixel color [cite: 531]
        led_strip_set_pixel(led_strip, i, r, g, b);
        // Refresh the hardware to show the color
        led_strip_refresh(led_strip);
        vTaskDelay(pdMS_TO_TICKS(50));
        // Clear before next step for flow animation
        led_strip_clear(led_strip);
    }
}