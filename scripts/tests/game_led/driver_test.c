/* Real ws2812b.c, deterministic ticks and injected mutex/driver failures.
 * The mock checks every hardware write owns the same strip mutex. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "ws2812b.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

static unsigned create_count, task_count, write_count, refresh_count, clear_count;
static unsigned take_count;
static TickType_t fake_tick, last_wait;
static bool mutex_busy, mutex_held;
static bool refresh_fail, clear_fail;
static int fail_pixel = -1;
static uint8_t pixels[10][3];
bool ble_flow_led_en;

SemaphoreHandle_t xSemaphoreCreateMutex(void)
{
    return &mutex_held;
}

BaseType_t xSemaphoreTake(SemaphoreHandle_t mutex, TickType_t wait)
{
    assert(mutex == &mutex_held);
    take_count++;
    last_wait = wait;
    if (mutex_busy) return 0;
    assert(!mutex_held);
    mutex_held = true;
    return pdTRUE;
}

BaseType_t xSemaphoreGive(SemaphoreHandle_t mutex)
{
    assert(mutex == &mutex_held && mutex_held);
    mutex_held = false;
    return pdTRUE;
}

TickType_t xTaskGetTickCount(void) { return fake_tick; }
void vTaskDelay(TickType_t ticks) { fake_tick += ticks; }
void vTaskDelete(TaskHandle_t task) { (void)task; }

BaseType_t xTaskCreatePinnedToCore(void (*function)(void *), const char *name,
                                 unsigned stack_size, void *param,
                                 unsigned priority, TaskHandle_t *handle, int core)
{
    (void)function; (void)name; (void)stack_size; (void)param;
    (void)priority; (void)handle; (void)core;
    task_count++;
    return pdPASS;
}

esp_err_t led_strip_new_rmt_device(const led_strip_config_t *strip_config,
                                  const led_strip_rmt_config_t *rmt_config,
                                  led_strip_handle_t *handle)
{
    assert(strip_config->max_leds == 10);
    (void)rmt_config;
    create_count++;
    *handle = pixels;
    return ESP_OK;
}

esp_err_t led_strip_set_pixel(led_strip_handle_t handle, uint32_t index,
                              uint32_t red, uint32_t green, uint32_t blue)
{
    assert(handle == pixels && mutex_held && index < 10);
    write_count++;
    if ((int)index == fail_pixel) return ESP_FAIL;
    pixels[index][0] = red;
    pixels[index][1] = green;
    pixels[index][2] = blue;
    return ESP_OK;
}

esp_err_t led_strip_refresh(led_strip_handle_t handle)
{
    assert(handle == pixels && mutex_held);
    refresh_count++;
    return refresh_fail ? ESP_FAIL : ESP_OK;
}

esp_err_t led_strip_clear(led_strip_handle_t handle)
{
    assert(handle == pixels && mutex_held);
    clear_count++;
    if (clear_fail) return ESP_FAIL;
    memset(pixels, 0, sizeof(pixels));
    return ESP_OK;
}

static void test_existing_effects(void)
{
    init_led_strip();
    init_led_strip();
    assert(create_count == 1 && write_count == 10 && refresh_count == 1);
    assert(last_wait == 250 && !mutex_held);

    set_led_bar_level(3, 21, 32, 43);
    for (int i = 0; i < 10; i++) {
        assert(pixels[i][0] == (i >= 7 ? 21 : 0));
        assert(pixels[i][1] == (i >= 7 ? 32 : 0));
        assert(pixels[i][2] == (i >= 7 ? 43 : 0));
    }
    unsigned saved_writes = write_count, saved_refreshes = refresh_count;
    set_flow_light(1, 2, 3);
    assert(fake_tick == 500 && clear_count == 10);
    assert(write_count == saved_writes + 10 && refresh_count == saved_refreshes + 10);
    set_flow_light_fast(4, 5, 6);
    assert(fake_tick == 750 && clear_count == 20);
    assert(write_count == saved_writes + 20 && refresh_count == saved_refreshes + 20);
    clear_led_bar();
    assert(clear_count == 21 && refresh_count == saved_refreshes + 21);
    assert(last_wait == 250 && !mutex_held && task_count == 0);
    puts("PASS: existing static, normal/fast flow, clear and initialization behavior");
}

static void test_game_frames(void)
{
    uint8_t frame[10][3];
    for (int i = 0; i < 10; i++) {
        frame[i][0] = i;
        frame[i][1] = i + 10;
        frame[i][2] = i + 20;
    }
    assert(!game_led_try_frame(frame) && game_led_release());
    assert(create_count == 0 && take_count == 0);
    init_led_strip();
    assert(create_count == 1 && !mutex_held);
    unsigned saved_writes = write_count;
    assert(!game_led_try_frame(NULL) && write_count == saved_writes);
    fake_tick = 199;
    assert(!game_led_try_frame(frame));
    assert(write_count == saved_writes && last_wait == 0 && !mutex_held);
    fake_tick = 200;
    assert(game_led_try_frame(frame) && write_count == saved_writes + 10);
    assert(memcmp(frame, pixels, sizeof(frame)) == 0);
    fake_tick = 210;
    assert(game_led_try_frame(frame)); /* Game frames do not reset quiet time. */

    fake_tick = 215;
    set_led_bar_level(10, 1, 2, 3);
    saved_writes = write_count;
    unsigned saved_clears = clear_count;
    assert(game_led_release() && clear_count == saved_clears);
    fake_tick = 414;
    assert(!game_led_try_frame(frame) && write_count == saved_writes);
    fake_tick = 415;
    assert(game_led_try_frame(frame));

    mutex_busy = true;
    saved_writes = write_count;
    assert(!game_led_try_frame(frame) && last_wait == 0);
    assert(!game_led_release() && last_wait == 0 && write_count == saved_writes);
    mutex_busy = false;
    clear_fail = true;
    assert(!game_led_release());
    clear_fail = false;
    assert(game_led_release());
    saved_clears = clear_count;
    assert(game_led_release() && clear_count == saved_clears);

    fail_pixel = 4;
    assert(!game_led_try_frame(frame));
    fail_pixel = -1;
    assert(game_led_release() && clear_count == saved_clears + 1);
    refresh_fail = true;
    assert(!game_led_try_frame(frame));
    refresh_fail = false;
    assert(game_led_release() && clear_count == saved_clears + 2);

    fake_tick = UINT32_MAX - 100;
    clear_led_bar();
    fake_tick = 98;
    assert(!game_led_try_frame(frame));
    fake_tick = 99;
    assert(game_led_try_frame(frame));

    fake_tick = 1000;
    assert(game_led_try_frame(frame));
    mutex_busy = true;
    clear_led_bar(); /* A failed legacy lock must not steal game ownership. */
    assert(last_wait == 250);
    mutex_busy = false;
    saved_clears = clear_count;
    assert(game_led_release() && clear_count == saved_clears + 1);
    assert(!mutex_held && task_count == 0);
    puts("PASS: game priority, zero-wait locks, release/retry, driver failures and tick wrap");
}

int main(int argc, char **argv)
{
    assert(argc == 2);
    if (strcmp(argv[1], "legacy") == 0) {
        test_existing_effects();
    } else {
        assert(strcmp(argv[1], "game") == 0);
        test_game_frames();
    }
    return 0;
}
