#include "tilt_game.h"

#include <string.h>

#define TILT_SPAWN_X            (TILT_SCREEN_WIDTH + 28.0f)
#define TILT_FIRST_GATE_X        236.0f
#define TILT_MIN_GAP             148.0f
#define TILT_GAP_VARIATION        56.0f
#define TILT_SPEED_PER_PIXEL       0.018f
#define TILT_HIT_RADIUS            6.5f
#define TILT_STAR_RADIUS           7.0f
#define TILT_ORB_MIN_X            18.0f
#define TILT_ORB_MAX_X           292.0f

static float clampf(float value, float lo, float hi)
{
    if (value < lo) return lo;
    if (value > hi) return hi;
    return value;
}

static uint32_t random_next(tilt_game_t *game)
{
    uint32_t x = game->rng;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    game->rng = x;
    return x;
}

static float play_mid(void)
{
    return (TILT_PLAY_TOP + TILT_PLAY_BOTTOM) * 0.5f;
}

static void spawn_star(tilt_game_t *game, float x, float y)
{
    unsigned i;
    for (i = 0; i < TILT_MAX_STARS; ++i) {
        if (game->pickups[i].active) {
            continue;
        }
        game->pickups[i].active = true;
        game->pickups[i].x = x;
        game->pickups[i].y = y;
        return;
    }
}

static void spawn_gate(tilt_game_t *game, float overshoot)
{
    unsigned i;
    float shrink;
    float gap_h;
    float travel;
    float gap_y;

    for (i = 0; i < TILT_MAX_GATES; ++i) {
        tilt_gate_t *gate = &game->gates[i];
        if (gate->active) {
            continue;
        }
        shrink = game->distance_px * 0.012f;
        if (shrink > 34.0f) {
            shrink = 34.0f;
        }
        gap_h = 80.0f - shrink;
        travel = (float)(TILT_PLAY_BOTTOM - TILT_PLAY_TOP) - gap_h - 8.0f;
        if (travel < 4.0f) {
            travel = 4.0f;
        }
        gap_y = TILT_PLAY_TOP + 4.0f + gap_h * 0.5f +
                (float)(random_next(game) % ((uint32_t)travel + 1u));
        gate->active = true;
        gate->cleared = false;
        gate->x = TILT_SPAWN_X + overshoot;
        gate->gap_y = gap_y;
        gate->gap_h = gap_h;
        gate->width = (uint8_t)(16u + random_next(game) % 7u);
        game->spawn_distance_remaining += gate->width + TILT_MIN_GAP +
            (float)(random_next(game) % ((uint32_t)TILT_GAP_VARIATION + 1u));
        if ((random_next(game) & 3u) != 0u) {
            spawn_star(game, gate->x + gate->width * 0.5f + 18.0f, gap_y);
        }
        return;
    }
}

void tilt_game_init(tilt_game_t *game, uint32_t seed, uint32_t best)
{
    memset(game, 0, sizeof(*game));
    game->phase = TILT_PHASE_READY;
    game->rng = seed != 0u ? seed : 0x51ed270bu;
    game->best = best;
    game->orb_x = 72.0f;
    game->orb_y = play_mid();
    game->speed_px_s = TILT_START_SPEED;
}

void tilt_game_start(tilt_game_t *game)
{
    uint32_t seed = game->rng;
    uint32_t best = game->best;
    tilt_game_init(game, seed, best);
    game->phase = TILT_PHASE_RUNNING;
    spawn_gate(game, 0.0f);
    if (game->gates[0].active) {
        float shift = game->gates[0].x - TILT_FIRST_GATE_X;
        game->gates[0].x = TILT_FIRST_GATE_X;
        if (game->pickups[0].active) {
            game->pickups[0].x -= shift;
        }
    }
}

void tilt_game_steer(tilt_game_t *game, float tilt_x, float tilt_y)
{
    game->steer_x = clampf(tilt_x, -1.25f, 1.25f);
    game->steer_y = clampf(tilt_y, -1.25f, 1.25f);
}

void tilt_game_pause(tilt_game_t *game)
{
    if (game->phase == TILT_PHASE_RUNNING) {
        game->phase = TILT_PHASE_PAUSED;
        game->accumulator_ms = 0u;
    }
}

void tilt_game_resume(tilt_game_t *game)
{
    if (game->phase == TILT_PHASE_PAUSED) {
        game->phase = TILT_PHASE_RUNNING;
        game->accumulator_ms = 0u;
    }
}

static bool circle_rect(float cx, float cy, float radius,
                        float x, float y, float w, float h)
{
    float nearest_x = clampf(cx, x, x + w);
    float nearest_y = clampf(cy, y, y + h);
    float dx = cx - nearest_x;
    float dy = cy - nearest_y;
    return dx * dx + dy * dy < radius * radius;
}

static bool hit_gate(const tilt_game_t *game, const tilt_gate_t *gate)
{
    float top_h = gate->gap_y - gate->gap_h * 0.5f - (float)TILT_PLAY_TOP;
    float bottom_y = gate->gap_y + gate->gap_h * 0.5f;
    float bottom_h = (float)TILT_PLAY_BOTTOM - bottom_y;
    if (top_h > 0.0f &&
        circle_rect(game->orb_x, game->orb_y, TILT_HIT_RADIUS,
                    gate->x, (float)TILT_PLAY_TOP, gate->width, top_h)) {
        return true;
    }
    if (bottom_h > 0.0f &&
        circle_rect(game->orb_x, game->orb_y, TILT_HIT_RADIUS,
                    gate->x, bottom_y, gate->width, bottom_h)) {
        return true;
    }
    return false;
}

static uint32_t fixed_step(tilt_game_t *game)
{
    const float dt = TILT_STEP_MS / 1000.0f;
    const float blend = TILT_DAMP * dt > 1.0f ? 1.0f : TILT_DAMP * dt;
    uint32_t events = 0u;
    uint32_t old_score = game->score;
    float travel;
    float target_vx = game->steer_x * TILT_SENSITIVITY;
    float target_vy = game->steer_y * TILT_SENSITIVITY;
    unsigned i;

    game->vx += (target_vx - game->vx) * blend;
    game->vy += (target_vy - game->vy) * blend;
    game->orb_x = clampf(game->orb_x + game->vx * dt, TILT_ORB_MIN_X, TILT_ORB_MAX_X);
    game->orb_y = clampf(game->orb_y + game->vy * dt,
                         (float)TILT_PLAY_TOP + TILT_ORB_RADIUS,
                         (float)TILT_PLAY_BOTTOM - TILT_ORB_RADIUS);
    if (game->orb_x == TILT_ORB_MIN_X || game->orb_x == TILT_ORB_MAX_X) {
        game->vx = 0.0f;
    }
    if (game->orb_y <= (float)TILT_PLAY_TOP + TILT_ORB_RADIUS ||
        game->orb_y >= (float)TILT_PLAY_BOTTOM - TILT_ORB_RADIUS) {
        game->vy = 0.0f;
    }

    game->speed_px_s = TILT_START_SPEED + game->distance_px * TILT_SPEED_PER_PIXEL;
    if (game->speed_px_s > TILT_MAX_SPEED) {
        game->speed_px_s = TILT_MAX_SPEED;
    }
    travel = game->speed_px_s * dt;
    game->distance_px += travel;

    for (i = 0; i < TILT_MAX_STARS; ++i) {
        tilt_star_t *star = &game->pickups[i];
        float dx;
        float dy;
        if (!star->active) {
            continue;
        }
        star->x -= travel;
        if (star->x < -12.0f) {
            star->active = false;
            continue;
        }
        dx = game->orb_x - star->x;
        dy = game->orb_y - star->y;
        if (dx * dx + dy * dy <= TILT_STAR_RADIUS * TILT_STAR_RADIUS) {
            star->active = false;
            if (game->stars < 255u) {
                game->stars++;
            }
            events |= TILT_EVENT_STAR;
        }
    }

    for (i = 0; i < TILT_MAX_GATES; ++i) {
        tilt_gate_t *gate = &game->gates[i];
        if (!gate->active) {
            continue;
        }
        gate->x -= travel;
        if (!gate->cleared && game->orb_x > gate->x + gate->width) {
            gate->cleared = true;
            events |= TILT_EVENT_CLEAR;
        }
        if (gate->x + gate->width < -4.0f) {
            gate->active = false;
        } else if (hit_gate(game, gate)) {
            game->phase = TILT_PHASE_GAME_OVER;
            return events | TILT_EVENT_HIT;
        }
    }

    if (game->distance_px / 8.0f + (float)game->stars * 25.0f >= (float)UINT32_MAX) {
        game->score = UINT32_MAX;
    } else {
        game->score = (uint32_t)(game->distance_px / 8.0f) +
                      (uint32_t)game->stars * 25u;
    }
    if (game->score > game->best) {
        game->best = game->score;
    }
    if (game->score / TILT_MILESTONE_SCORE != old_score / TILT_MILESTONE_SCORE) {
        events |= TILT_EVENT_MILESTONE;
    }

    game->spawn_distance_remaining -= travel;
    if (game->spawn_distance_remaining <= 0.0f) {
        spawn_gate(game, game->spawn_distance_remaining);
    }
    return events;
}

uint32_t tilt_game_step(tilt_game_t *game, uint32_t elapsed_ms)
{
    uint32_t events = 0u;
    if (game->phase != TILT_PHASE_RUNNING) {
        return events;
    }
    if (elapsed_ms > TILT_MAX_ELAPSED_MS) {
        elapsed_ms = TILT_MAX_ELAPSED_MS;
    }
    game->accumulator_ms += elapsed_ms;
    while (game->accumulator_ms >= TILT_STEP_MS) {
        game->accumulator_ms -= TILT_STEP_MS;
        events |= fixed_step(game);
        if (game->phase != TILT_PHASE_RUNNING) {
            game->accumulator_ms = 0u;
            break;
        }
    }
    return events;
}
