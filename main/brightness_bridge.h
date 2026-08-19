#pragma once

#include <stdint.h>

void brightness_bridge_init(void);
void brightness_persist_impl(uint8_t level);
