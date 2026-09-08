#pragma once

#include <stdint.h>

/* Single owner: the main task. Call once after nvs_flash_init(), before UI init. */
void game_score_storage_init(void);
uint32_t game_score_storage_best(void);

/* RAM only: safe inside an LVGL event/timer. Never decreases the record. */
void game_score_storage_offer(uint32_t best);

/* Call outside lvgl_mutex while gameplay is stopped. Failed I/O retries after 5s. */
void game_score_storage_poll(uint32_t now_ms);
