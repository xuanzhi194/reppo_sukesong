#include "dino_game.h"

#include <string.h>

#define DINO_SPAWN_X         (DINO_SCREEN_WIDTH + 32.0f)
#define DINO_MIN_GAP         240u
#define DINO_GAP_VARIATION    90u
#define DINO_SPEED_PER_PIXEL   0.015f

static uint32_t random_next(dino_game_t *game)
{
    uint32_t x = game->rng;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    game->rng = x;
    return x;
}

static void spawn_obstacle(dino_game_t *game, float overshoot)
{
    unsigned i;
    for (i = 0; i < DINO_MAX_OBSTACLES; ++i) {
        dino_obstacle_t *obstacle = &game->obstacles[i];
        if (obstacle->active) {
            continue;
        }
        obstacle->active = true;
        obstacle->x = DINO_SPAWN_X + overshoot;
        obstacle->width = (uint8_t)(12u + random_next(game) % 13u);
        obstacle->height = (uint8_t)(20u + random_next(game) % 15u);
        /* At maximum speed this leaves >= 1.02 s of clear ground. The
         * complete jump lasts 0.72 s, so landing and reacting remain possible. */
        game->spawn_distance_remaining += (float)(obstacle->width + DINO_MIN_GAP +
                                                  random_next(game) % (DINO_GAP_VARIATION + 1u));
        return;
    }
}

void dino_game_init(dino_game_t *game, uint32_t seed, uint32_t best)
{
    memset(game, 0, sizeof(*game));
    game->phase = DINO_PHASE_READY;
    game->rng = seed != 0u ? seed : 0x6d2b79f5u;
    game->best = best;
    game->speed_px_s = DINO_START_SPEED;
}

void dino_game_start(dino_game_t *game)
{
    uint32_t seed = game->rng;
    uint32_t best = game->best;
    dino_game_init(game, seed, best);
    game->phase = DINO_PHASE_RUNNING;
    spawn_obstacle(game, 0.0f);
}

bool dino_game_jump(dino_game_t *game)
{
    if (game->phase != DINO_PHASE_RUNNING || game->jump_y > 0.0f ||
        game->velocity != 0.0f) {
        return false;
    }
    game->velocity = DINO_JUMP_VELOCITY;
    return true;
}

void dino_game_pause(dino_game_t *game)
{
    if (game->phase == DINO_PHASE_RUNNING) {
        game->phase = DINO_PHASE_PAUSED;
        game->accumulator_ms = 0u;
    }
}

void dino_game_resume(dino_game_t *game)
{
    if (game->phase == DINO_PHASE_PAUSED) {
        game->phase = DINO_PHASE_RUNNING;
        game->accumulator_ms = 0u;
    }
}

static bool collision(const dino_game_t *game, const dino_obstacle_t *obstacle)
{
    /* Inset both silhouettes slightly: a pixel near a foot or cactus spine
     * should not end the run. Fixed steps keep motion below 2.35 px per test. */
    const float player_left = DINO_X + 4.0f;
    const float player_right = DINO_X + DINO_WIDTH - 3.0f;
    const float player_top = DINO_GROUND_Y - DINO_HEIGHT + 4.0f - game->jump_y;
    const float player_bottom = DINO_GROUND_Y - 3.0f - game->jump_y;
    const float obstacle_left = obstacle->x + 2.0f;
    const float obstacle_right = obstacle->x + obstacle->width - 2.0f;
    const float obstacle_top = DINO_GROUND_Y - obstacle->height + 3.0f;

    return player_left < obstacle_right && player_right > obstacle_left &&
           player_bottom > obstacle_top && player_top < DINO_GROUND_Y;
}

static uint32_t fixed_step(dino_game_t *game)
{
    const float dt = DINO_STEP_MS / 1000.0f;
    uint32_t events = 0u;
    uint32_t old_score = game->score;
    float travel;
    unsigned i;

    game->speed_px_s = DINO_START_SPEED + game->distance_px * DINO_SPEED_PER_PIXEL;
    if (game->speed_px_s > DINO_MAX_SPEED) {
        game->speed_px_s = DINO_MAX_SPEED;
    }
    travel = game->speed_px_s * dt;
    game->distance_px += travel;
    /* Guard the float-to-integer conversion even for an exceptionally long run. */
    if (game->distance_px / 10.0f >= (float)UINT32_MAX) {
        game->score = UINT32_MAX;
    } else {
        game->score = (uint32_t)(game->distance_px / 10.0f);
    }
    if (game->score > game->best) {
        game->best = game->score;
    }
    if (game->score / DINO_MILESTONE_SCORE != old_score / DINO_MILESTONE_SCORE) {
        events |= DINO_EVENT_MILESTONE;
    }

    if (game->jump_y > 0.0f || game->velocity > 0.0f) {
        game->jump_y += game->velocity * dt - 0.5f * DINO_GRAVITY * dt * dt;
        game->velocity -= DINO_GRAVITY * dt;
        if (game->jump_y <= 0.0f) {
            game->jump_y = 0.0f;
            game->velocity = 0.0f;
            events |= DINO_EVENT_LANDED;
        }
    }

    for (i = 0; i < DINO_MAX_OBSTACLES; ++i) {
        dino_obstacle_t *obstacle = &game->obstacles[i];
        if (!obstacle->active) {
            continue;
        }
        obstacle->x -= travel;
        if (obstacle->x + obstacle->width < 0.0f) {
            obstacle->active = false;
        } else if (collision(game, obstacle)) {
            game->phase = DINO_PHASE_GAME_OVER;
            return events | DINO_EVENT_HIT;
        }
    }

    game->spawn_distance_remaining -= travel;
    if (game->spawn_distance_remaining <= 0.0f) {
        spawn_obstacle(game, game->spawn_distance_remaining);
    }
    return events;
}

uint32_t dino_game_step(dino_game_t *game, uint32_t elapsed_ms)
{
    uint32_t events = 0u;
    if (game->phase != DINO_PHASE_RUNNING) {
        return events;
    }
    if (elapsed_ms > DINO_MAX_ELAPSED_MS) {
        elapsed_ms = DINO_MAX_ELAPSED_MS;
    }
    game->accumulator_ms += elapsed_ms;
    while (game->accumulator_ms >= DINO_STEP_MS) {
        game->accumulator_ms -= DINO_STEP_MS;
        events |= fixed_step(game);
        if (game->phase != DINO_PHASE_RUNNING) {
            game->accumulator_ms = 0u;
            break;
        }
    }
    return events;
}
