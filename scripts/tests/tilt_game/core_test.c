#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "tilt_game.h"

static float abs_value(float value)
{
    return value < 0.0f ? -value : value;
}

static void clear_course(tilt_game_t *game)
{
    memset(game->gates, 0, sizeof(game->gates));
    memset(game->pickups, 0, sizeof(game->pickups));
    game->spawn_distance_remaining = 1000000.0f;
}

static void test_steer_and_bounds(void)
{
    tilt_game_t game;
    tilt_game_init(&game, 1u, 9u);
    assert(game.phase == TILT_PHASE_READY && game.best == 9u);
    assert(game.orb_x > 0.0f && game.orb_y > (float)TILT_PLAY_TOP);
    tilt_game_steer(&game, 4.0f, -4.0f);
    assert(game.steer_x <= 1.25f && game.steer_y >= -1.25f);
    assert(tilt_game_step(&game, 50u) == 0u && game.distance_px == 0.0f);
    tilt_game_start(&game);
    clear_course(&game);
    tilt_game_steer(&game, 1.0f, -1.0f);
    float start_x = game.orb_x;
    float start_y = game.orb_y;
    for (unsigned i = 0; i < 40u; ++i) {
        assert((tilt_game_step(&game, 10u) & TILT_EVENT_HIT) == 0u);
    }
    assert(game.orb_x > start_x && game.orb_y < start_y);
    tilt_game_steer(&game, -1.2f, 1.2f);
    for (unsigned i = 0; i < 200u; ++i) {
        tilt_game_step(&game, 10u);
    }
    assert(game.orb_x >= 18.0f && game.orb_x <= 292.0f);
    assert(game.orb_y >= (float)TILT_PLAY_TOP + TILT_ORB_RADIUS);
    assert(game.orb_y <= (float)TILT_PLAY_BOTTOM - TILT_ORB_RADIUS);
}

static void test_collision_pause_restart(void)
{
    tilt_game_t game, dead;
    unsigned elapsed = 0u;
    tilt_game_init(&game, 77u, 0u);
    tilt_game_start(&game);
    game.gates[0].active = true;
    game.gates[0].cleared = false;
    game.gates[0].x = game.orb_x - 4.0f;
    game.gates[0].width = 20;
    game.gates[0].gap_y = 128.0f;
    game.gates[0].gap_h = 18.0f;
    while (game.phase == TILT_PHASE_RUNNING && elapsed < 200u) {
        uint32_t events = tilt_game_step(&game, 10u);
        elapsed += 10u;
        if (game.phase == TILT_PHASE_GAME_OVER) {
            assert(events & TILT_EVENT_HIT);
        }
    }
    assert(game.phase == TILT_PHASE_GAME_OVER);
    dead = game;
    assert(tilt_game_step(&game, 60000u) == 0u);
    tilt_game_pause(&game);
    tilt_game_resume(&game);
    assert(memcmp(&game, &dead, sizeof(game)) == 0);
    tilt_game_start(&game);
    assert(game.phase == TILT_PHASE_RUNNING && game.best == dead.best);
    assert(game.score == 0u && game.distance_px == 0.0f);
    assert(game.gates[0].active && game.gates[0].x > game.orb_x + 80.0f);
}

static void test_pause_and_time_partition(void)
{
    const unsigned pattern[] = {1u, 7u, 16u, 33u, 4u, 27u, 9u, 3u};
    tilt_game_t regular, irregular, paused;
    unsigned elapsed = 0u, index = 0u;
    uint32_t events_regular = 0u, events_irregular = 0u;

    tilt_game_init(&regular, 44u, 0u);
    tilt_game_start(&regular);
    clear_course(&regular);
    tilt_game_steer(&regular, 0.15f, -0.1f);
    irregular = regular;
    for (unsigned i = 0u; i < 160u; ++i) {
        events_regular |= tilt_game_step(&regular, 10u);
    }
    while (elapsed < 1600u) {
        unsigned dt = pattern[index++ % (sizeof(pattern) / sizeof(pattern[0]))];
        if (dt > 1600u - elapsed) dt = 1600u - elapsed;
        events_irregular |= tilt_game_step(&irregular, dt);
        elapsed += dt;
    }
    assert(events_regular == events_irregular);
    assert(memcmp(&regular, &irregular, sizeof(regular)) == 0);

    tilt_game_pause(&regular);
    assert(regular.phase == TILT_PHASE_PAUSED && regular.accumulator_ms == 0u);
    paused = regular;
    assert(tilt_game_step(&regular, UINT32_MAX) == 0u);
    assert(memcmp(&regular, &paused, sizeof(regular)) == 0);
    tilt_game_resume(&regular);
    assert(regular.phase == TILT_PHASE_RUNNING);
}

static void test_star_and_pilot(void)
{
    unsigned clears = 0u;
    for (uint32_t seed = 1u; seed <= 8u; ++seed) {
        tilt_game_t game;
        uint32_t old_score = 0u;
        tilt_game_init(&game, seed, 0u);
        tilt_game_start(&game);
        for (uint32_t elapsed = 0u; elapsed < 45000u &&
             game.phase == TILT_PHASE_RUNNING; elapsed += 10u) {
            float target = game.orb_y;
            for (unsigned i = 0; i < TILT_MAX_GATES; ++i) {
                const tilt_gate_t *gate = &game.gates[i];
                if (gate->active && gate->x + gate->width >= game.orb_x - 4.0f) {
                    target = gate->gap_y;
                    break;
                }
            }
            tilt_game_steer(&game, 0.05f, (target - game.orb_y) / 50.0f);
            uint32_t events = tilt_game_step(&game, 10u);
            if (events & TILT_EVENT_HIT) {
                fprintf(stderr, "pilot hit: seed=%u t=%u score=%u y=%.1f\n",
                        seed, elapsed, game.score, game.orb_y);
            }
            assert((events & TILT_EVENT_HIT) == 0u);
            if (events & TILT_EVENT_CLEAR) ++clears;
            assert(game.score >= old_score && game.best == game.score);
            old_score = game.score;
        }
        assert(game.phase == TILT_PHASE_RUNNING);
        assert(game.score > 80u);
        assert(abs_value(game.speed_px_s - TILT_MAX_SPEED) < 0.2f ||
               game.speed_px_s <= TILT_MAX_SPEED);
    }
    assert(clears > 20u);
    printf("8 generated tilt courses survived for 45s each; %u gates cleared.\n", clears);
}

int main(void)
{
    test_steer_and_bounds();
    test_collision_pause_restart();
    test_pause_and_time_partition();
    test_star_and_pilot();
    puts("Tilt physics: steer, bounds, collision, pause, restart and pilots passed.");
    return 0;
}
