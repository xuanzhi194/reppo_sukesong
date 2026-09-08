/** Pure C runner physics. All functions are called by the owning UI thread. */
#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DINO_SCREEN_WIDTH       320
#define DINO_SCREEN_HEIGHT      170
#define DINO_GROUND_Y            138
#define DINO_X                   46
#define DINO_WIDTH               28
#define DINO_HEIGHT              30
#define DINO_MAX_OBSTACLES        3
#define DINO_STEP_MS             10u
#define DINO_MAX_ELAPSED_MS     100u
#define DINO_START_SPEED        120.0f
#define DINO_MAX_SPEED          235.0f
#define DINO_JUMP_VELOCITY      360.0f
#define DINO_GRAVITY           1000.0f
#define DINO_MILESTONE_SCORE    100u

#define DINO_EVENT_LANDED       (1u << 0)
#define DINO_EVENT_HIT          (1u << 1)
#define DINO_EVENT_MILESTONE    (1u << 2)

typedef enum {
    DINO_PHASE_READY = 0,
    DINO_PHASE_RUNNING,
    DINO_PHASE_PAUSED,
    DINO_PHASE_GAME_OVER,
} dino_phase_t;

typedef struct {
    bool active;
    float x;
    uint8_t width;
    uint8_t height;
} dino_obstacle_t;

typedef struct {
    dino_phase_t phase;
    float jump_y;             /**< Height above the ground, in pixels. */
    float velocity;           /**< Positive values move upward. */
    float speed_px_s;
    float distance_px;
    uint32_t score;
    uint32_t best;            /**< Record seeded by storage; preserved on restart. */
    dino_obstacle_t obstacles[DINO_MAX_OBSTACLES];
    /* Internal bookkeeping; callers should only read the fields above. */
    uint32_t rng;
    uint32_t accumulator_ms;
    float spawn_distance_remaining;
} dino_game_t;

void dino_game_init(dino_game_t *game, uint32_t seed, uint32_t best);
void dino_game_start(dino_game_t *game);
bool dino_game_jump(dino_game_t *game);
void dino_game_pause(dino_game_t *game);
void dino_game_resume(dino_game_t *game);
/** Advance in fixed 10 ms steps; discard elapsed time beyond 100 ms per call. */
uint32_t dino_game_step(dino_game_t *game, uint32_t elapsed_ms);

#ifdef __cplusplus
}
#endif
