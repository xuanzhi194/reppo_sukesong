/* screen_game adapter. All UI state belongs to the main/LVGL task; no worker
 * touches LVGL. The Guider screen is only a host for our own draw object. */
#include "game_machine.h"
#include "game_score_storage.h"
#include "dino_game.h"
#include "dino_render.h"
#include "tilt_game.h"
#include "tilt_render.h"
#include "imu_motion.h"
#include "custom.h"
#include "gui_guider.h"
#include <math.h>
#include <string.h>
#ifdef ESP_PLATFORM
#include "esp_random.h"
#include "ws2812b.h"
#endif

#define GAME_FRAME_MS  33
#define GAME_INPUT_GUARD_MS  220
#define SELECT_TILT_ENTER  0.20f
#define SELECT_TILT_HOLD_MS  160

typedef enum {
    GAME_ROOM_SELECT = 0,
    GAME_ROOM_DINO,
    GAME_ROOM_TILT,
} game_room_t;

static struct {
    lv_obj_t *screen;
    lv_obj_t *stage;
    lv_group_t *group;
    lv_timer_t *timer;
    game_room_t room;
    uint8_t catalog;
    uint8_t selection;
    dino_game_t model;
    tilt_game_t tilt;
    imu_sample_t rest;
    float tilt_x;
    float tilt_y;
    int8_t tilt_want;
    uint32_t tilt_hold_tick;
    uint32_t last_frame;
    uint32_t phase_tick;
    uint32_t jump_tick;
    uint32_t milestone_tick;
    uint32_t input_tick;
    bool rest_ok;
    bool has_jump;
    bool has_milestone;
    bool active;
    bool leaving;
    bool exit_requested;
} s_game;
static bool s_led_release_pending;
static volatile bool s_game_open;

static void game_timer_cb(lv_timer_t *timer);
static void game_stage_event(lv_event_t *event);
static void game_screen_event(lv_event_t *event);
static void game_turn(lv_group_t *group, bool forward);

bool game_machine_is_open(void)
{
    return s_game_open;
}

static bool game_visible(void)
{
    return s_game.active && !s_game.leaving && s_game.stage != NULL &&
           s_game.screen != NULL && lv_scr_act() == s_game.screen;
}

static bool game_is_running(void)
{
    return (s_game.room == GAME_ROOM_DINO && s_game.model.phase == DINO_PHASE_RUNNING) ||
           (s_game.room == GAME_ROOM_TILT && s_game.tilt.phase == TILT_PHASE_RUNNING);
}

static void game_offer_scores(void)
{
    game_score_storage_offer(s_game.model.best);
    game_score_storage_tilt_offer(s_game.tilt.best);
}

static void game_set_paper(uint32_t color)
{
    if (s_game.screen == NULL) return;
    lv_obj_set_style_bg_color(s_game.screen, lv_color_hex(color), LV_PART_MAIN);
}

static void game_detach_input(void)
{
    if (s_game.group == NULL) return;
    lv_group_set_edge_cb(s_game.group, NULL);
    lv_indev_t *input = NULL;
    while ((input = lv_indev_get_next(input)) != NULL) {
        /* A new page may already own the device: never rebind it in that case. */
        if (input->group == s_game.group) {
            /* Abort any in-progress release/encoder step before freeing its group. */
            lv_indev_reset(input, NULL);
            lv_indev_wait_release(input);
            lv_indev_set_group(input, g);
        }
    }
    lv_group_del(s_game.group);
    s_game.group = NULL;
}

static void game_stop(void)
{
    if (s_game.timer != NULL) {
        lv_timer_del(s_game.timer);
        s_game.timer = NULL;
    }
    game_offer_scores();
    s_game.active = false;
    s_game_open = false;
    s_led_release_pending = true;
    game_detach_input();
}

static void game_phase_changed(void)
{
    if (!game_is_running()) {
        /* Pause/game-over checkpoint: RAM only while LVGL holds its mutex. */
        game_offer_scores();
    }
    s_game.phase_tick = lv_tick_get();
    s_game.last_frame = s_game.phase_tick;
    s_game.selection = 0;
    s_game.has_jump = false;
    s_game.has_milestone = false;
    if (s_game.stage != NULL) lv_obj_invalidate(s_game.stage);
}

static void game_turn(lv_group_t *group, bool forward)
{
    (void)forward;
    if (group != s_game.group || !game_visible() ||
        lv_tick_elaps(s_game.input_tick) < GAME_INPUT_GUARD_MS) return;
    if (game_is_running()) {
        if (s_game.room == GAME_ROOM_DINO) dino_game_pause(&s_game.model);
        else tilt_game_pause(&s_game.tilt);
        game_phase_changed();
    } else {
        s_game.selection ^= 1U;
        lv_obj_invalidate(s_game.stage);
    }
}

void screen_game_buttion_add(void)
{
    if (!game_visible() || s_game.group != NULL) return;
    s_game.group = lv_group_create();
    lv_group_set_wrap(s_game.group, false);
    lv_group_add_obj(s_game.group, s_game.stage);
    lv_group_focus_obj(s_game.stage);
    lv_group_set_edge_cb(s_game.group, game_turn);
    lv_indev_t *input = NULL;
    while ((input = lv_indev_get_next(input)) != NULL) {
        if (lv_indev_get_type(input) == LV_INDEV_TYPE_KEYPAD ||
            lv_indev_get_type(input) == LV_INDEV_TYPE_ENCODER) {
            lv_indev_wait_release(input);
            lv_indev_set_group(input, s_game.group);
        }
    }
}

static uint32_t game_seed(void)
{
    uint32_t seed = lv_tick_get() ^ 0x52455050U;
#ifdef ESP_PLATFORM
    seed ^= esp_random();
#endif
    return seed;
}

static void enter_catalog(bool start)
{
    uint32_t seed = game_seed();
    if (s_game.catalog == 0) {
        s_game.room = GAME_ROOM_DINO;
        dino_game_init(&s_game.model, seed, game_score_storage_best());
        if (start) dino_game_start(&s_game.model);
        game_set_paper(0xF5F5EB);
    } else {
        s_game.room = GAME_ROOM_TILT;
        tilt_game_init(&s_game.tilt, seed, game_score_storage_tilt_best());
        if (start) tilt_game_start(&s_game.tilt);
        game_set_paper(0x0B1724);
    }
    game_phase_changed();
}

static void game_click(void)
{
    if (!game_visible() || lv_tick_elaps(s_game.input_tick) < GAME_INPUT_GUARD_MS) return;
    if (s_game.room == GAME_ROOM_SELECT) {
        if (s_game.selection == 1) {
            s_game.exit_requested = true;
            s_game.leaving = true;
            game_stop();
            return;
        }
        enter_catalog(true);
        idle_sleep_note_activity();
        lv_obj_invalidate(s_game.stage);
        return;
    }
    if (s_game.room == GAME_ROOM_DINO && s_game.model.phase == DINO_PHASE_RUNNING) {
        if (dino_game_jump(&s_game.model)) {
            s_game.jump_tick = lv_tick_get();
            s_game.has_jump = true;
        }
    } else if (s_game.room == GAME_ROOM_TILT && s_game.tilt.phase == TILT_PHASE_RUNNING) {
        tilt_game_pause(&s_game.tilt);
        game_phase_changed();
    } else {
        uint32_t ready_guard = (s_game.room == GAME_ROOM_DINO &&
                                s_game.model.phase == DINO_PHASE_GAME_OVER) ||
                               (s_game.room == GAME_ROOM_TILT &&
                                s_game.tilt.phase == TILT_PHASE_GAME_OVER) ? 300 : 0;
        if (ready_guard && lv_tick_elaps(s_game.phase_tick) < ready_guard) return;
        if (s_game.selection == 1) {
            s_game.exit_requested = true;
            s_game.leaving = true;
            game_stop();
            return;
        }
        if (s_game.room == GAME_ROOM_DINO) {
            if (s_game.model.phase == DINO_PHASE_PAUSED) dino_game_resume(&s_game.model);
            else dino_game_start(&s_game.model);
        } else {
            if (s_game.tilt.phase == TILT_PHASE_PAUSED) tilt_game_resume(&s_game.tilt);
            else tilt_game_start(&s_game.tilt);
        }
        game_phase_changed();
        idle_sleep_note_activity();
    }
    lv_obj_invalidate(s_game.stage);
}

static void sample_tilt(void)
{
    imu_sample_t sample;
    float ax;
    float ay;
    imu_motion_latest(&sample);
    if (!sample.valid) {
        s_game.tilt_x *= 0.85f;
        s_game.tilt_y *= 0.85f;
        return;
    }
    if (!s_game.rest_ok) {
        float mag = sqrtf(sample.accel_g[0] * sample.accel_g[0] +
                          sample.accel_g[1] * sample.accel_g[1] +
                          sample.accel_g[2] * sample.accel_g[2]);
        if (mag > 0.65f && mag < 1.35f) {
            s_game.rest = sample;
            s_game.rest_ok = true;
        }
        return;
    }
    /* Desk-rest subtracted. accel[0] is left/right, accel[1] is toward/away. */
    ax = sample.accel_g[0] - s_game.rest.accel_g[0];
    ay = sample.accel_g[1] - s_game.rest.accel_g[1];
    s_game.tilt_x += (ax - s_game.tilt_x) * 0.28f;
    s_game.tilt_y += (ay - s_game.tilt_y) * 0.28f;
}

static void select_poll_tilt(uint32_t now)
{
    int want;

    sample_tilt();
    want = s_game.tilt_x > SELECT_TILT_ENTER ? 1 :
           s_game.tilt_x < -SELECT_TILT_ENTER ? 0 : -1;
    if (want < 0) {
        s_game.tilt_want = -1;
        return;
    }
    if (want != s_game.tilt_want) {
        s_game.tilt_want = (int8_t)want;
        s_game.tilt_hold_tick = now;
        return;
    }
    if (now - s_game.tilt_hold_tick >= SELECT_TILT_HOLD_MS &&
        s_game.catalog != (uint8_t)want) {
        s_game.catalog = (uint8_t)want;
        idle_sleep_note_activity();
    }
}

static void game_stage_event(lv_event_t *event)
{
    if (lv_event_get_target(event) != s_game.stage) return;
    switch (lv_event_get_code(event)) {
    case LV_EVENT_DRAW_MAIN: {
        lv_area_t bounds;
        lv_obj_get_coords(s_game.stage, &bounds);
        uint32_t milestone_age = lv_tick_elaps(s_game.milestone_tick);
        uint32_t remaining = s_game.has_milestone && milestone_age < 900 ? 900 - milestone_age : 0;
        if (s_game.room == GAME_ROOM_SELECT) {
            game_select_render(lv_event_get_draw_ctx(event), &bounds,
                               s_game.catalog, s_game.selection, s_game.tilt_x,
                               s_game.model.best, s_game.tilt.best,
                               lv_tick_elaps(s_game.phase_tick));
        } else if (s_game.room == GAME_ROOM_TILT) {
            tilt_render(lv_event_get_draw_ctx(event), &bounds, &s_game.tilt,
                        s_game.selection, lv_tick_elaps(s_game.phase_tick), remaining);
        } else {
            dino_render(lv_event_get_draw_ctx(event), &bounds, &s_game.model,
                        s_game.selection, lv_tick_elaps(s_game.phase_tick), remaining);
        }
        break;
    }
    case LV_EVENT_CLICKED:
        game_click();
        break;
    case LV_EVENT_KEY: {
        uint32_t key = lv_event_get_key(event);
        if (key == LV_KEY_LEFT || key == LV_KEY_RIGHT) game_turn(s_game.group, key == LV_KEY_RIGHT);
        else if (key == LV_KEY_ESC && game_visible()) {
            if (game_is_running()) {
                if (s_game.room == GAME_ROOM_DINO) dino_game_pause(&s_game.model);
                else tilt_game_pause(&s_game.tilt);
                game_phase_changed();
            } else {
                s_game.selection = 1;
                game_click();
            }
        }
        break;
    }
    case LV_EVENT_DELETE:
        /* Guider cleans children before the screen's delayed UNLOAD/DELETE. */
        game_stop();
        s_game.stage = NULL;
        s_game.leaving = true;
        break;
    default:
        break;
    }
}

static void game_screen_event(lv_event_t *event)
{
    if (lv_event_get_target(event) != s_game.screen) return;
    lv_event_code_t code = lv_event_get_code(event);
    if (code == LV_EVENT_DELETE) {
        lv_obj_t *old = s_game.screen;
        game_stop();
        memset(&s_game, 0, sizeof(s_game));
        s_game_open = false;
        if (guider_ui.screen_game == old) {
            guider_ui.screen_game = NULL;
            guider_ui.screen_game_del = true;
        }
    } else if (code == LV_EVENT_SCREEN_UNLOAD_START) {
        s_game.leaving = true;
        game_stop();
    } else if (code == LV_EVENT_SCREEN_LOAD_START) {
        s_game.leaving = false;
    }
}

void screen_game_custom_init(void)
{
    lv_obj_t *screen = guider_ui.screen_game;
    if (screen == NULL || !lv_obj_is_valid(screen)) return;
    if (s_game.screen != screen) {
        /* Bind even during the incoming fade, before taking over any input. */
        if (s_game.screen != NULL) game_stop();
        s_game.screen = screen;
        s_game.stage = NULL;
        s_game.leaving = false;
        s_game.exit_requested = false;
        lv_obj_add_event_cb(screen, game_screen_event, LV_EVENT_ALL, NULL);
    }
    if (s_game.leaving || s_game.active || lv_scr_act() != screen) return;
    dino_game_init(&s_game.model, game_seed(), game_score_storage_best());
    tilt_game_init(&s_game.tilt, game_seed() ^ 0x7f4a7c15u, game_score_storage_tilt_best());
    s_game.room = GAME_ROOM_SELECT;
    s_game.catalog = 0;
    s_game.selection = 0;
    s_game.rest_ok = false;
    s_game.tilt_x = s_game.tilt_y = 0.0f;
    s_game.tilt_want = -1;
    s_game.has_jump = s_game.has_milestone = false;
    s_game.input_tick = s_game.phase_tick = s_game.last_frame = lv_tick_get();
    s_game.active = true;
    s_game_open = true;
    game_set_paper(0x0B1724);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_pad_all(screen, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(screen, 0, LV_PART_MAIN);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    if (s_game.stage == NULL) {
        s_game.stage = lv_obj_create(screen);
        lv_obj_remove_style_all(s_game.stage);
        lv_obj_set_pos(s_game.stage, 0, 0);
        lv_obj_set_size(s_game.stage, 320, 170);
        lv_obj_clear_flag(s_game.stage, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ON_FOCUS);
        lv_obj_add_flag(s_game.stage, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(s_game.stage, game_stage_event, LV_EVENT_ALL, NULL);
    }
    s_game.timer = lv_timer_create(game_timer_cb, GAME_FRAME_MS, NULL);
    screen_game_buttion_add();
    lv_obj_invalidate(s_game.stage);
}

static void game_timer_cb(lv_timer_t *timer)
{
    (void)timer;
    if (!game_visible()) return;
    uint32_t now = lv_tick_get();
    uint32_t elapsed = now - s_game.last_frame;
    s_game.last_frame = now;
    if (s_game.room == GAME_ROOM_SELECT) {
        select_poll_tilt(now);
        lv_obj_invalidate(s_game.stage);
        return;
    }
    if (s_game.room == GAME_ROOM_TILT && s_game.tilt.phase == TILT_PHASE_RUNNING) {
        uint32_t events;
        idle_sleep_note_activity();
        sample_tilt();
        tilt_game_steer(&s_game.tilt, s_game.tilt_x, -s_game.tilt_y);
        events = tilt_game_step(&s_game.tilt, elapsed);
        if (events & TILT_EVENT_HIT) {
            game_phase_changed();
        } else if (events & (TILT_EVENT_MILESTONE | TILT_EVENT_STAR | TILT_EVENT_CLEAR)) {
            s_game.has_milestone = (events & TILT_EVENT_MILESTONE) != 0;
            if (s_game.has_milestone || (events & TILT_EVENT_STAR)) {
                s_game.milestone_tick = now;
                s_game.has_milestone = true;
            }
        }
        lv_obj_invalidate(s_game.stage);
    } else if (s_game.room == GAME_ROOM_DINO && s_game.model.phase == DINO_PHASE_RUNNING) {
        idle_sleep_note_activity();
        uint32_t events = dino_game_step(&s_game.model, elapsed);
        if (events & DINO_EVENT_HIT) {
            game_phase_changed();
        } else if (events & DINO_EVENT_MILESTONE) {
            s_game.has_milestone = true;
            s_game.milestone_tick = now;
        }
        lv_obj_invalidate(s_game.stage);
    } else if (((s_game.room == GAME_ROOM_DINO && s_game.model.phase == DINO_PHASE_GAME_OVER) ||
                (s_game.room == GAME_ROOM_TILT && s_game.tilt.phase == TILT_PHASE_GAME_OVER)) &&
               lv_tick_elaps(s_game.phase_tick) < 600) {
        lv_obj_invalidate(s_game.stage);
    }
}

void game_machine_poll(void)
{
    if (s_game.exit_requested) {
        s_game.exit_requested = false;
        if (s_game.screen != NULL && lv_scr_act() == s_game.screen) {
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_main,
                                   guider_ui.screen_main_del, &guider_ui.screen_game_del,
                                  setup_scr_screen_main, LV_SCR_LOAD_ANIM_FADE_ON,
                                  100, 100, true, true);
        }
        return;
    }
    /* Detect screens made by freshly exported Guider code, without events edits. */
    screen_game_custom_init();
    /* A startup NVS read may have recovered on a later storage poll. */
    uint32_t best = game_score_storage_best();
    uint32_t tilt_best = game_score_storage_tilt_best();
    if (s_game.active && best > s_game.model.best) {
        s_game.model.best = best;
        if (s_game.stage != NULL) lv_obj_invalidate(s_game.stage);
    }
    if (s_game.active && tilt_best > s_game.tilt.best) {
        s_game.tilt.best = tilt_best;
        if (s_game.stage != NULL) lv_obj_invalidate(s_game.stage);
    }
}

void game_machine_storage_poll(void)
{
    /* Same main task as LVGL, but outside its lock and outside running frames. */
    if (s_game.active && game_is_running()) return;
    game_score_storage_poll(lv_tick_get());
}

void game_machine_led_poll(void)
{
#ifdef ESP_PLATFORM
    static uint32_t last_led_tick;
    uint32_t now = lv_tick_get();
    bool enabled = s_game.active && !s_game.leaving;
    uint32_t phase_age = now - s_game.phase_tick;
    if (enabled && s_game.room != GAME_ROOM_SELECT && !game_is_running() &&
        phase_age > 1400) enabled = false;
    if (!enabled) {
        if (s_led_release_pending && game_led_release()) s_led_release_pending = false;
        return;
    }
    if (now - last_led_tick < 40) return;
    last_led_tick = now;
    uint8_t rgb[10][3] = {{0}};
    if (s_game.room == GAME_ROOM_SELECT) {
        unsigned brightness = phase_age < 1000 ? (1000 - phase_age) * 22 / 1000 : 14;
        int head = s_game.catalog ? 7 : 2;
        rgb[head][1] = brightness;
        rgb[head][2] = brightness * 2 / 3;
        rgb[4][2] = rgb[5][2] = brightness / 3;
    } else if ((s_game.room == GAME_ROOM_DINO && s_game.model.phase == DINO_PHASE_GAME_OVER) ||
               (s_game.room == GAME_ROOM_TILT && s_game.tilt.phase == TILT_PHASE_GAME_OVER)) {
        unsigned brightness = phase_age < 600 ? (600 - phase_age) * 48 / 600 : 0;
        for (int i = 0; i < 10; ++i) { rgb[i][0] = brightness; rgb[i][1] = brightness / 10; }
    } else if (s_game.has_milestone && now - s_game.milestone_tick < 700) {
        unsigned brightness = ((now - s_game.milestone_tick) / 120) % 2 ? 14 : 44;
        for (int i = 0; i < 10; ++i) { rgb[i][0] = brightness; rgb[i][1] = brightness * 2 / 3; }
    } else if (s_game.has_jump && now - s_game.jump_tick < 230) {
        unsigned radius = (now - s_game.jump_tick) / 45;
        for (int i = 0; i < 10; ++i) {
            unsigned distance = i < 5 ? 4 - i : i - 5;
            if (distance == radius) { rgb[i][0] = 8; rgb[i][1] = 52; rgb[i][2] = 25; }
        }
    } else if (s_game.room == GAME_ROOM_TILT && s_game.tilt.phase == TILT_PHASE_RUNNING) {
        int head = (int)((s_game.tilt.orb_y - TILT_PLAY_TOP) * 9.0f /
                         (TILT_PLAY_BOTTOM - TILT_PLAY_TOP));
        if (head < 0) head = 0;
        if (head > 9) head = 9;
        rgb[head][1] = 36;
        rgb[head][2] = 28;
        if (head > 0) { rgb[head - 1][1] = 12; rgb[head - 1][2] = 10; }
        if (head < 9) { rgb[head + 1][1] = 12; rgb[head + 1][2] = 10; }
    } else if (s_game.model.phase == DINO_PHASE_RUNNING) {
        int head = (int)(s_game.model.distance_px / 9.0f) % 10;
        for (int i = 0; i < 10; ++i) {
            unsigned age = (head + 10 - i) % 10;
            if (age < 3) { rgb[i][0] = 2; rgb[i][1] = 28 / (age + 1); rgb[i][2] = 19 / (age + 1); }
        }
    } else {
        unsigned brightness = phase_age < 1000 ? (1000 - phase_age) * 18 / 1000 : 0;
        rgb[4][0] = rgb[5][0] = brightness / 3;
        rgb[4][1] = rgb[5][1] = brightness;
        rgb[4][2] = rgb[5][2] = brightness / 2;
    }
    /* A failed driver write may still have partially prepared a game frame. */
    s_led_release_pending = true;
    (void)game_led_try_frame(rgb);
#endif
}
