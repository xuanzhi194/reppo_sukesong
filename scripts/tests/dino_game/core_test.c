/* Test actual production physics, including a pilot over generated courses. */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "dino_game.h"

static float abs_value(float value)
{
    return value < 0.0f ? -value : value;
}

static void clear_course(dino_game_t *game)
{
    memset(game->obstacles, 0, sizeof(game->obstacles));
    game->spawn_distance_remaining = 1000000.0f;
}

static void test_jump_and_input(void)
{
    dino_game_t game;
    uint32_t events;
    unsigned landed = 0u;
    float peak = 0.0f;

    dino_game_init(&game, 0u, 77u);
    assert(game.phase == DINO_PHASE_READY && game.best == 77u);
    assert(!dino_game_jump(&game));
    assert(dino_game_step(&game, 100u) == 0u && game.distance_px == 0.0f);
    dino_game_start(&game);
    clear_course(&game);
    assert(dino_game_jump(&game));
    assert(!dino_game_jump(&game)); /* Reject another press before even one tick. */
    for (unsigned i = 0; i < 80u; ++i) {
        events = dino_game_step(&game, 10u);
        assert((events & DINO_EVENT_HIT) == 0u);
        if (events & DINO_EVENT_LANDED) {
            ++landed;
        }
        if (game.jump_y > peak) {
            peak = game.jump_y;
        }
        if (game.jump_y > 0.0f) {
            assert(!dino_game_jump(&game));
        }
    }
    assert(landed == 1u && peak > 64.0f && peak < 65.1f);
    assert(game.jump_y == 0.0f && game.velocity == 0.0f);
    assert(game.best == 77u && dino_game_jump(&game));
}

static void test_collision_restart_and_first_obstacle(void)
{
    dino_game_t game, dead;
    unsigned elapsed = 0u;
    dino_game_init(&game, 123u, 0u);
    dino_game_start(&game);
    while (game.phase == DINO_PHASE_RUNNING && elapsed < 4000u) {
        uint32_t events = dino_game_step(&game, 10u);
        elapsed += 10u;
        if (game.phase == DINO_PHASE_GAME_OVER) {
            assert(events & DINO_EVENT_HIT);
        } else {
            assert((events & DINO_EVENT_HIT) == 0u);
        }
    }
    assert(game.phase == DINO_PHASE_GAME_OVER && elapsed > 1500u);
    assert(game.score > 0u && game.best == game.score);
    dead = game;
    assert(dino_game_step(&game, 60000u) == 0u);
    assert(!dino_game_jump(&game));
    dino_game_pause(&game);
    dino_game_resume(&game);
    assert(memcmp(&game, &dead, sizeof(game)) == 0);
    dino_game_start(&game);
    assert(game.phase == DINO_PHASE_RUNNING && game.best == dead.best);
    assert(game.score == 0u && game.distance_px == 0.0f);
    assert(game.jump_y == 0.0f && game.velocity == 0.0f);
    assert(game.speed_px_s == DINO_START_SPEED && game.accumulator_ms == 0u);
    assert(game.obstacles[0].active && game.obstacles[0].x > DINO_SCREEN_WIDTH);
}

static void test_pause(void)
{
    dino_game_t game, paused;
    dino_game_init(&game, 333u, 0u);
    dino_game_start(&game);
    assert(dino_game_jump(&game));
    dino_game_step(&game, 47u);
    assert(game.jump_y > 0.0f && game.accumulator_ms == 7u);
    dino_game_pause(&game);
    assert(game.phase == DINO_PHASE_PAUSED && game.accumulator_ms == 0u);
    paused = game;
    assert(dino_game_step(&game, UINT32_MAX) == 0u);
    assert(!dino_game_jump(&game));
    assert(memcmp(&game, &paused, sizeof(game)) == 0);
    dino_game_resume(&game);
    assert(game.phase == DINO_PHASE_RUNNING);
    assert(dino_game_step(&game, 9u) == 0u && game.jump_y == paused.jump_y);
    dino_game_step(&game, 1u);
    assert(game.jump_y > paused.jump_y && game.distance_px > paused.distance_px);
}

static void test_time_partition_and_stall(void)
{
    const unsigned pattern[] = {1u, 7u, 16u, 33u, 4u, 27u, 9u, 3u};
    dino_game_t regular, irregular, stalled, capped;
    unsigned elapsed = 0u, index = 0u;
    uint32_t events_regular = 0u, events_irregular = 0u;
    dino_game_init(&regular, 98u, 0u);
    dino_game_start(&regular);
    assert(dino_game_jump(&regular));
    irregular = regular;
    for (unsigned i = 0u; i < 200u; ++i) {
        events_regular |= dino_game_step(&regular, 10u);
    }
    while (elapsed < 2000u) {
        unsigned dt = pattern[index++ % (sizeof(pattern) / sizeof(pattern[0]))];
        if (dt > 2000u - elapsed) {
            dt = 2000u - elapsed;
        }
        events_irregular |= dino_game_step(&irregular, dt);
        elapsed += dt;
    }
    assert(events_regular == events_irregular);
    assert(memcmp(&regular, &irregular, sizeof(regular)) == 0);
    stalled = capped = regular;
    assert(dino_game_step(&stalled, UINT32_MAX) == dino_game_step(&capped, 100u));
    assert(memcmp(&stalled, &capped, sizeof(stalled)) == 0);
    assert(stalled.distance_px - regular.distance_px <= DINO_MAX_SPEED * 0.1f);
    assert(dino_game_step(&stalled, 0u) == 0u);
    assert(memcmp(&stalled, &capped, sizeof(stalled)) == 0); /* No backlog. */
}

static void test_cactus_dimensions_and_jump_window(void)
{
    const unsigned widths[] = {12u, 24u};
    const unsigned heights[] = {20u, 34u};
    const float speeds[] = {DINO_START_SPEED, DINO_MAX_SPEED};
    for (unsigned speed = 0; speed < 2u; ++speed) {
        for (unsigned width = 0; width < 2u; ++width) {
            for (unsigned height = 0; height < 2u; ++height) {
                dino_game_t game;
                dino_game_init(&game, 22u, 0u);
                dino_game_start(&game);
                clear_course(&game);
                game.distance_px = (speeds[speed] - DINO_START_SPEED) / 0.015f;
                game.speed_px_s = speeds[speed];
                game.obstacles[0] = (dino_obstacle_t){
                    .active = true,
                    .x = DINO_X + DINO_WIDTH + speeds[speed] * 0.18f,
                    .width = (uint8_t)widths[width], .height = (uint8_t)heights[height],
                };
                assert(dino_game_jump(&game));
                for (unsigned i = 0; i < 90u; ++i) {
                    assert(!(dino_game_step(&game, 10u) & DINO_EVENT_HIT));
                }
                assert(game.phase == DINO_PHASE_RUNNING && game.jump_y == 0.0f);
            }
        }
    }
}

static void test_long_generated_courses(void)
{
    unsigned total_jumps = 0u;
    for (uint32_t seed = 1u; seed <= 12u; ++seed) {
        dino_game_t game;
        unsigned jumps = 0u, milestones = 0u;
        uint32_t old_score = 0u;
        /* Vary the reaction point and input cadence across seeds to test
         * useful timing windows rather than a single perfect trajectory. */
        const float trigger_seconds = 0.17f + (seed % 4u) * 0.02f;
        const uint32_t frame_ms = 10u + (seed % 3u) * 11u;
        dino_game_init(&game, seed, 0u);
        dino_game_start(&game);
        for (uint32_t elapsed = 0u; elapsed < 180000u; elapsed += frame_ms) {
            float nearest = 10000.0f;
            for (unsigned i = 0; i < DINO_MAX_OBSTACLES; ++i) {
                const dino_obstacle_t *obstacle = &game.obstacles[i];
                if (!obstacle->active) {
                    continue;
                }
                assert(obstacle->width >= 12u && obstacle->width <= 24u);
                assert(obstacle->height >= 20u && obstacle->height <= 34u);
                if (obstacle->x + obstacle->width >= DINO_X && obstacle->x < nearest) {
                    nearest = obstacle->x;
                }
                for (unsigned j = 0; j < DINO_MAX_OBSTACLES; ++j) {
                    const dino_obstacle_t *next = &game.obstacles[j];
                    if (next->active && next->x > obstacle->x) {
                        assert(next->x - obstacle->x - obstacle->width > 239.0f);
                    }
                }
            }
            if (game.jump_y == 0.0f && game.velocity == 0.0f &&
                (nearest + 2.0f - (DINO_X + DINO_WIDTH - 3.0f)) /
                    game.speed_px_s <= trigger_seconds) {
                assert(dino_game_jump(&game));
                ++jumps;
            }
            uint32_t events = dino_game_step(&game, frame_ms);
            if (events & DINO_EVENT_HIT) {
                fprintf(stderr, "pilot hit: seed=%u t=%u score=%u speed=%.2f y=%.2f\n",
                        seed, elapsed, game.score, game.speed_px_s, game.jump_y);
            }
            assert((events & DINO_EVENT_HIT) == 0u);
            if (events & DINO_EVENT_MILESTONE) {
                ++milestones;
            }
            assert(game.phase == DINO_PHASE_RUNNING);
            assert(game.score >= old_score && game.best == game.score);
            assert(game.speed_px_s >= DINO_START_SPEED && game.speed_px_s <= DINO_MAX_SPEED);
            old_score = game.score;
        }
        assert(abs_value(game.speed_px_s - DINO_MAX_SPEED) < 0.001f);
        assert(game.score > 3000u && jumps > 80u);
        assert(milestones == game.score / DINO_MILESTONE_SCORE);
        total_jumps += jumps;
    }
    printf("12 generated courses survived for 3 minutes each; %u jumps at varied timings.\n",
           total_jumps);
}

int main(void)
{
    test_jump_and_input();
    test_collision_restart_and_first_obstacle();
    test_pause();
    test_time_partition_and_stall();
    test_cactus_dimensions_and_jump_window();
    test_long_generated_courses();
    puts("Dino physics: input, jump, collision, pause, restart, fixed-step and stall tests passed.");
    return 0;
}
