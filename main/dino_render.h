#pragma once

#include "lvgl.h"
#include "dino_game.h"

/* Draw into an existing LVGL object. No heap allocations, timers or hardware. */
void dino_render(lv_draw_ctx_t *ctx, const lv_area_t *bounds,
                 const dino_game_t *game, uint8_t selection,
                 uint32_t phase_age_ms, uint32_t milestone_ms);
