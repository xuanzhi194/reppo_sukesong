/** Pure C tilt-orb physics. All functions are called by the owning UI thread. */
#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TILT_SCREEN_WIDTH        320
#define TILT_SCREEN_HEIGHT       170
#define TILT_PLAY_TOP             34
#define TILT_PLAY_BOTTOM         148
#define TILT_ORB_RADIUS            8.0f
#define TILT_MAX_GATES             3
#define TILT_MAX_STARS             2
#define TILT_STEP_MS              10u
#define TILT_MAX_ELAPSED_MS      100u
#define TILT_START_SPEED          78.0f
#define TILT_MAX_SPEED           168.0f
#define TILT_SENSITIVITY         240.0f
#define TILT_DAMP                  9.0f
#define TILT_MILESTONE_SCORE      50u

#define TILT_EVENT_HIT           (1u << 0)
#define TILT_EVENT_STAR          (1u << 1)
#define TILT_EVENT_MILESTONE     (1u << 2)
#define TILT_EVENT_CLEAR         (1u << 3)

typedef enum {
    TILT_PHASE_READY = 0,
    TILT_PHASE_RUNNING,
    TILT_PHASE_PAUSED,
    TILT_PHASE_GAME_OVER,
} tilt_phase_t;

typedef struct {
    bool active;
    bool cleared;
    float x;
    float gap_y;
    float gap_h;
    uint8_t width;
} tilt_gate_t;

typedef struct {
    bool active;
    float x;
    float y;
} tilt_star_t;

typedef struct {
    tilt_phase_t phase;
    float orb_x;
    float orb_y;
    float vx;
    float vy;
    float steer_x;
    float steer_y;
    float speed_px_s;
    float distance_px;
    uint32_t score;
    uint32_t best;
    uint8_t stars;
    tilt_gate_t gates[TILT_MAX_GATES];
    tilt_star_t pickups[TILT_MAX_STARS];
    uint32_t rng;
    uint32_t accumulator_ms;
    float spawn_distance_remaining;
} tilt_game_t;

void tilt_game_init(tilt_game_t *game, uint32_t seed, uint32_t best);
void tilt_game_start(tilt_game_t *game);
void tilt_game_steer(tilt_game_t *game, float tilt_x, float tilt_y);
void tilt_game_pause(tilt_game_t *game);
void tilt_game_resume(tilt_game_t *game);
uint32_t tilt_game_step(tilt_game_t *game, uint32_t elapsed_ms);

#ifdef __cplusplus
}
#endif
