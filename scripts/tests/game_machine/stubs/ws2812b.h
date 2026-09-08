#pragma once
#include <stdbool.h>
#include <stdint.h>
bool game_led_try_frame(const uint8_t rgb[10][3]);
bool game_led_release(void);
