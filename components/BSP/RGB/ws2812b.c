#include "ws2812b.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

extern bool ble_flow_led_en;

// Define GPIO15 according to the schematic [cite: 197]
#define LED_STRIP_GPIO_PIN  17
// 10 LEDs cascaded in series [cite: 51, 94]
#define LED_STRIP_NUM_LEDS  10 

static led_strip_handle_t led_strip;
static bool s_led_strip_ready = false;
static SemaphoreHandle_t s_led_strip_mutex;

#define LED_EFFECT_TASK_STACK 4096

static bool led_strip_lock(void)
{
    return s_led_strip_mutex == NULL ||
           xSemaphoreTake(s_led_strip_mutex, pdMS_TO_TICKS(250)) == pdTRUE;
}

static void led_strip_unlock(void)
{
    if (s_led_strip_mutex != NULL) {
        xSemaphoreGive(s_led_strip_mutex);
    }
}

/** 上电后立即点亮全部 LED，不等待流水动画 */
static void led_strip_boot_instant_hint(void)
{
    if (!led_strip_lock()) {
        return;
    }
    for (int i = 0; i < LED_STRIP_NUM_LEDS; i++) {
        led_strip_set_pixel(led_strip, i, 51, 64, 69);
    }
    led_strip_refresh(led_strip);
    led_strip_unlock();
}

void init_led_strip(void)
{
    if (s_led_strip_ready) {
        return;
    }
    if (s_led_strip_mutex == NULL) {
        s_led_strip_mutex = xSemaphoreCreateMutex();
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

/** 开机三条流水：步进稍慢 + 多灯拖尾，避免单点闪烁过快、亮度不均 */
#define BOOT_FLOW_STEP_MS           45
#define BOOT_FLOW_TRAIL_LEN         2
#define BOOT_FLOW_PASS_GAP_MS       80

static void boot_flow_light_once(uint32_t r, uint32_t g, uint32_t b)
{
    const int trail = BOOT_FLOW_TRAIL_LEN;

    for (int head = 0; head < LED_STRIP_NUM_LEDS + trail - 1; head++) {
        if (!led_strip_lock()) {
            vTaskDelay(pdMS_TO_TICKS(BOOT_FLOW_STEP_MS));
            continue;
        }
        for (int i = 0; i < LED_STRIP_NUM_LEDS; i++) {
            int age = head - i;
            if (age >= 0 && age < trail) {
                uint32_t scale = 255U - (uint32_t)age * 255U / (uint32_t)trail;
                led_strip_set_pixel(led_strip, i,
                                    (uint32_t)(r * scale) / 255U,
                                    (uint32_t)(g * scale) / 255U,
                                    (uint32_t)(b * scale) / 255U);
            } else {
                led_strip_set_pixel(led_strip, i, 0, 0, 0);
            }
        }
        led_strip_refresh(led_strip);
        led_strip_unlock();
        vTaskDelay(pdMS_TO_TICKS(BOOT_FLOW_STEP_MS));
    }
}

static void flow_light_run(uint32_t r, uint32_t g, uint32_t b, uint32_t step_ms)
{
    for (int i = 0; i < LED_STRIP_NUM_LEDS; i++) {
        if (led_strip_lock()) {
            led_strip_set_pixel(led_strip, i, r, g, b);
            led_strip_refresh(led_strip);
            led_strip_unlock();
        }
        vTaskDelay(pdMS_TO_TICKS(step_ms));
        if (led_strip_lock()) {
            led_strip_clear(led_strip);
            led_strip_unlock();
        }
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
    if (xTaskCreatePinnedToCore(hint_flow_light_task, "hint_flow",
                                LED_EFFECT_TASK_STACK, NULL, 4,
                                &s_hint_flow_task, 0) != pdPASS) {
        s_hint_flow_task = NULL;
    }
}

static void boot_led_blink_task(void *param)
{
    (void)param;
    if (led_strip_lock()) {
        led_strip_clear(led_strip);
        led_strip_refresh(led_strip);
        led_strip_unlock();
    }

    boot_flow_light_once(51, 64, 69);
    vTaskDelay(pdMS_TO_TICKS(BOOT_FLOW_PASS_GAP_MS));
    boot_flow_light_once(10, 24, 49);
    vTaskDelay(pdMS_TO_TICKS(BOOT_FLOW_PASS_GAP_MS));
    boot_flow_light_once(20, 20, 20);

    if (led_strip_lock()) {
        led_strip_clear(led_strip);
        led_strip_refresh(led_strip);
        led_strip_unlock();
    }
    vTaskDelete(NULL);
}

void start_boot_led_blink(void)
{
    if (!s_led_strip_ready) {
        init_led_strip();
    }
    xTaskCreatePinnedToCore(boot_led_blink_task, "boot_led",
                            LED_EFFECT_TASK_STACK, NULL, 8, NULL, 0);
}

static TaskHandle_t s_ble_flow_task = NULL;

static void ble_flow_light_once(uint32_t r, uint32_t g, uint32_t b)
{
    for (int i = 0; i < LED_STRIP_NUM_LEDS; i++) {
        if (!ble_flow_led_en) {
            return;
        }
        if (led_strip_lock()) {
            led_strip_set_pixel(led_strip, i, r, g, b);
            led_strip_refresh(led_strip);
            led_strip_unlock();
        }
        vTaskDelay(pdMS_TO_TICKS(50));
        if (!ble_flow_led_en) {
            return;
        }
        if (led_strip_lock()) {
            led_strip_clear(led_strip);
            led_strip_unlock();
        }
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
    if (led_strip_lock()) {
        led_strip_clear(led_strip);
        led_strip_refresh(led_strip);
        led_strip_unlock();
    }
    s_ble_flow_task = NULL;
    vTaskDelete(NULL);
}

void start_ble_flow_light(void)
{
    if (s_ble_flow_task != NULL) {
        return;
    }
    ble_flow_led_en = true;
    if (xTaskCreatePinnedToCore(ble_flow_led_task, "ble_flow_led",
                                LED_EFFECT_TASK_STACK, NULL, 5,
                                &s_ble_flow_task, 0) != pdPASS) {
        s_ble_flow_task = NULL;
        ble_flow_led_en = false;
    }
}

void stop_ble_flow_light(void)
{
    ble_flow_led_en = false;
}

#define POMODORO_BLACK_FLOW_STEP_MS     60
#define POMODORO_BLACK_FLOW_TRAIL_LEN   2
#define POMODORO_BLACK_FLOW_BASE_R      8
#define POMODORO_BLACK_FLOW_BASE_G      8
#define POMODORO_BLACK_FLOW_BASE_B      8
#define POMODORO_BLACK_FLOW_PEAK_R      36
#define POMODORO_BLACK_FLOW_PEAK_G      36
#define POMODORO_BLACK_FLOW_PEAK_B      36

static TaskHandle_t s_pomodoro_black_flow_task = NULL;
static volatile bool s_pomodoro_black_flow_en;
static volatile uint8_t s_pomodoro_black_lit_count = LED_STRIP_NUM_LEDS;

static uint32_t pomodoro_black_flow_clamp_channel(uint32_t value)
{
    return value > 255U ? 255U : value;
}

void pomodoro_black_flow_set_lit_count(uint8_t lit_count)
{
    if (lit_count > LED_STRIP_NUM_LEDS) {
        lit_count = LED_STRIP_NUM_LEDS;
    }
    s_pomodoro_black_lit_count = lit_count;
}

static void pomodoro_black_flow_once(void)
{
    const int trail = POMODORO_BLACK_FLOW_TRAIL_LEN;
    uint8_t lit_count = s_pomodoro_black_lit_count;

    if (lit_count == 0) {
        if (led_strip_lock()) {
            led_strip_clear(led_strip);
            led_strip_refresh(led_strip);
            led_strip_unlock();
        }
        vTaskDelay(pdMS_TO_TICKS(POMODORO_BLACK_FLOW_STEP_MS));
        return;
    }

    /* 与进度条一致：从条带末端向前缩短，剩余时间减少时先熄低索引端。 */
    uint8_t first_on = LED_STRIP_NUM_LEDS - lit_count;

    for (int head = first_on; head < LED_STRIP_NUM_LEDS + trail - 1; head++) {
        if (!s_pomodoro_black_flow_en) {
            return;
        }
        if (!led_strip_lock()) {
            vTaskDelay(pdMS_TO_TICKS(POMODORO_BLACK_FLOW_STEP_MS));
            continue;
        }
        for (int i = 0; i < LED_STRIP_NUM_LEDS; i++) {
            if (i < first_on) {
                led_strip_set_pixel(led_strip, i, 0, 0, 0);
                continue;
            }

            uint32_t r = POMODORO_BLACK_FLOW_BASE_R;
            uint32_t g = POMODORO_BLACK_FLOW_BASE_G;
            uint32_t b = POMODORO_BLACK_FLOW_BASE_B;
            int age = head - i;

            if (age >= 0 && age < trail) {
                uint32_t scale = 255U - (uint32_t)age * 255U / (uint32_t)trail;
                r += (POMODORO_BLACK_FLOW_PEAK_R * scale) / 255U;
                g += (POMODORO_BLACK_FLOW_PEAK_G * scale) / 255U;
                b += (POMODORO_BLACK_FLOW_PEAK_B * scale) / 255U;
            }

            led_strip_set_pixel(led_strip, i,
                                pomodoro_black_flow_clamp_channel(r),
                                pomodoro_black_flow_clamp_channel(g),
                                pomodoro_black_flow_clamp_channel(b));
        }
        led_strip_refresh(led_strip);
        led_strip_unlock();
        vTaskDelay(pdMS_TO_TICKS(POMODORO_BLACK_FLOW_STEP_MS));
    }
}

static void pomodoro_black_flow_task(void *param)
{
    (void)param;
    while (s_pomodoro_black_flow_en) {
        pomodoro_black_flow_once();
    }
    if (led_strip_lock()) {
        led_strip_clear(led_strip);
        led_strip_refresh(led_strip);
        led_strip_unlock();
    }
    s_pomodoro_black_flow_task = NULL;
    vTaskDelete(NULL);
}

void start_pomodoro_black_flow_light(void)
{
    if (!s_led_strip_ready) {
        init_led_strip();
    }
    if (s_pomodoro_black_flow_task != NULL) {
        return;
    }
    s_pomodoro_black_flow_en = true;
    s_pomodoro_black_lit_count = LED_STRIP_NUM_LEDS;
    if (xTaskCreatePinnedToCore(pomodoro_black_flow_task, "pom_black_flow",
                                LED_EFFECT_TASK_STACK, NULL, 5,
                                &s_pomodoro_black_flow_task, 0) != pdPASS) {
        s_pomodoro_black_flow_task = NULL;
        s_pomodoro_black_flow_en = false;
    }
}

void stop_pomodoro_black_flow_light(void)
{
    s_pomodoro_black_flow_en = false;
}

void set_led_bar_level(uint8_t lit_count, uint32_t r, uint32_t g, uint32_t b)
{
    if (!s_led_strip_ready) {
        init_led_strip();
    }
    if (lit_count > LED_STRIP_NUM_LEDS) {
        lit_count = LED_STRIP_NUM_LEDS;
    }
    /* 从条带末端向前点亮：剩余时间减少时先熄第一颗 */
    uint8_t first_on = LED_STRIP_NUM_LEDS - lit_count;
    if (!led_strip_lock()) {
        return;
    }
    for (int i = 0; i < LED_STRIP_NUM_LEDS; i++) {
        if (i >= first_on) {
            led_strip_set_pixel(led_strip, i, r, g, b);
        } else {
            led_strip_set_pixel(led_strip, i, 0, 0, 0);
        }
    }
    led_strip_refresh(led_strip);
    led_strip_unlock();
}

void clear_led_bar(void)
{
    if (!s_led_strip_ready) {
        return;
    }
    if (led_strip_lock()) {
        led_strip_clear(led_strip);
        led_strip_refresh(led_strip);
        led_strip_unlock();
    }
}