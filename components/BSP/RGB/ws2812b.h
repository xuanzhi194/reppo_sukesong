#ifndef __WS2812B_H_
#define __WS2812B_H_

#include <stdbool.h>
#include <stdint.h>
#include "led_strip.h"

void init_led_strip();
void set_flow_light(uint32_t r, uint32_t g, uint32_t b);
void set_flow_light_fast(uint32_t r, uint32_t g, uint32_t b);
void start_flow_light_fast_async(uint32_t r, uint32_t g, uint32_t b);
void start_boot_led_blink(void);
void start_ble_flow_light(void);
void stop_ble_flow_light(void);

/** 静态点亮前 lit_count 颗 LED（0~10），其余熄灭 */
void set_led_bar_level(uint8_t lit_count, uint32_t r, uint32_t g, uint32_t b);
void clear_led_bar(void);

/** 番茄钟黑屏：微亮进度底光 + 流水高亮，退出时 stop。 */
void start_pomodoro_black_flow_light(void);
void stop_pomodoro_black_flow_light(void);
void pomodoro_black_flow_set_lit_count(uint8_t lit_count);

/** 息屏充电：微亮底光 + 流水高亮；lit_count 与电量成比例，pale_green 为 >90%。 */
void start_idle_charge_flow_light(void);
void stop_idle_charge_flow_light(void);
void idle_charge_flow_update(uint8_t lit_count, bool pale_green);

/**
 * Try one low-priority game frame, indexed by physical LED then R/G/B.
 * Uses zero wait for the strip mutex and skips frames for 200 ms after any
 * existing effect writes. Returns true only when the frame was transmitted.
 * Does not initialize hardware or create tasks. Call outside the LVGL lock:
 * the driver still performs a synchronous, single-frame RMT transmission.
 *
 * This is a recent-activity guard, not a persistent owner reservation. Static
 * non-game frames are not saved/restored. Submit only while the game is visible
 * and awake; stop submitting during sleep or whenever another static display
 * must be preserved. Existing animated effects keep their normal priority.
 */
bool game_led_try_frame(const uint8_t rgb[10][3]);

/**
 * Clear only a frame still owned by the game; leave newer non-game frames alone.
 * True means released/no game frame remains (also true before initialization).
 * False means mutex busy or a driver error; retry on a later main-loop pass.
 * Call outside the LVGL lock after stopping game frame submissions.
 */
bool game_led_release(void);

#endif
