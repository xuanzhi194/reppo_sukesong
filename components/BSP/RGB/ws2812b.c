#include "ws2812b.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern bool ble_flow_led_en;

// Define GPIO15 according to the schematic [cite: 197]
#define LED_STRIP_GPIO_PIN  17
// 10 LEDs cascaded in series [cite: 51, 94]
#define LED_STRIP_NUM_LEDS  10 

static led_strip_handle_t led_strip;
static bool s_led_strip_ready = false;

/** 上电后立即点亮全部 LED，不等待流水动画 */
static void led_strip_boot_instant_hint(void)
{
    for (int i = 0; i < LED_STRIP_NUM_LEDS; i++) {
        led_strip_set_pixel(led_strip, i, 51, 64, 69);
    }
    led_strip_refresh(led_strip);
}

void init_led_strip(void)
{
    if (s_led_strip_ready) {
        return;
    }

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

    s_led_strip_ready = true;
    led_strip_boot_instant_hint();
}

#define FLOW_LIGHT_STEP_MS_DEFAULT  50
#define FLOW_LIGHT_STEP_MS_FAST     25

static void flow_light_run(uint32_t r, uint32_t g, uint32_t b, uint32_t step_ms)
{
    for (int i = 0; i < LED_STRIP_NUM_LEDS; i++) {
        led_strip_set_pixel(led_strip, i, r, g, b);
        led_strip_refresh(led_strip);
        vTaskDelay(pdMS_TO_TICKS(step_ms));
        led_strip_clear(led_strip);
    }
}

void set_flow_light(uint32_t r, uint32_t g, uint32_t b)
{
    flow_light_run(r, g, b, FLOW_LIGHT_STEP_MS_DEFAULT);
}

void set_flow_light_fast(uint32_t r, uint32_t g, uint32_t b)
{
    flow_light_run(r, g, b, FLOW_LIGHT_STEP_MS_FAST);
}

static TaskHandle_t s_hint_flow_task = NULL;
static uint32_t s_hint_flow_r;
static uint32_t s_hint_flow_g;
static uint32_t s_hint_flow_b;

static void hint_flow_light_task(void *param)
{
    (void)param;
    flow_light_run(s_hint_flow_r, s_hint_flow_g, s_hint_flow_b, FLOW_LIGHT_STEP_MS_FAST);
    s_hint_flow_task = NULL;
    vTaskDelete(NULL);
}

void start_flow_light_fast_async(uint32_t r, uint32_t g, uint32_t b)
{
    s_hint_flow_r = r;
    s_hint_flow_g = g;
    s_hint_flow_b = b;
    if (s_hint_flow_task != NULL) {
        return;
    }
    xTaskCreatePinnedToCore(hint_flow_light_task, "hint_flow", 2048, NULL, 4, &s_hint_flow_task, 0);
}

static void boot_led_blink_task(void *param)
{
    (void)param;
    set_flow_light_fast(51, 64, 69);
    set_flow_light_fast(10, 24, 49);
    set_flow_light_fast(20, 20, 20);
    led_strip_clear(led_strip);
    led_strip_refresh(led_strip);
    vTaskDelete(NULL);
}

void start_boot_led_blink(void)
{
    if (!s_led_strip_ready) {
        init_led_strip();
    }
    xTaskCreatePinnedToCore(boot_led_blink_task, "boot_led", 2048, NULL, 8, NULL, 0);
}

static TaskHandle_t s_ble_flow_task = NULL;

static void ble_flow_light_once(uint32_t r, uint32_t g, uint32_t b)
{
    for (int i = 0; i < LED_STRIP_NUM_LEDS; i++) {
        if (!ble_flow_led_en) {
            return;
        }
        led_strip_set_pixel(led_strip, i, r, g, b);
        led_strip_refresh(led_strip);
        vTaskDelay(pdMS_TO_TICKS(50));
        if (!ble_flow_led_en) {
            return;
        }
        led_strip_clear(led_strip);
    }
}

static void ble_flow_led_task(void *param)
{
    (void)param;
    /* Short pairing hint; stops when BLE advertising is ready */
    ble_flow_light_once(51, 64, 69);
    if (ble_flow_led_en) {
        ble_flow_light_once(10, 24, 49);
    }
    if (ble_flow_led_en) {
        ble_flow_light_once(20, 20, 20);
    }
    led_strip_clear(led_strip);
    led_strip_refresh(led_strip);
    s_ble_flow_task = NULL;
    vTaskDelete(NULL);
}

void start_ble_flow_light(void)
{
    if (s_ble_flow_task != NULL) {
        return;
    }
    ble_flow_led_en = true;
    xTaskCreatePinnedToCore(ble_flow_led_task, "ble_flow_led", 2048, NULL, 5, &s_ble_flow_task, 0);
}

void stop_ble_flow_light(void)
{
    ble_flow_led_en = false;
}