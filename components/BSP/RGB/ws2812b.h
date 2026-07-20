#ifndef __WS2812B_H_
#define __WS2812B_H_

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

#endif