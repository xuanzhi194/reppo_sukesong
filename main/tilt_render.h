#pragma once

#include "lvgl.h"
#include "tilt_game.h"

void tilt_render(lv_draw_ctx_t *ctx, const lv_area_t *bounds,
                 const tilt_game_t *game, uint8_t selection,
                 uint32_t phase_age_ms, uint32_t milestone_ms);

void game_select_render(lv_draw_ctx_t *ctx, const lv_area_t *bounds,
                        uint8_t catalog, uint8_t selection,
                        float tilt_x, uint32_t dino_best, uint32_t tilt_best,
                        uint32_t phase_age_ms);
