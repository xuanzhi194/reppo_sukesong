/* Use the real game adapter, physics, renderer, input and LVGL timer engine.
 * Only board hardware and the unrelated Guider main page are simulated. */
#include "lvgl.h"
#include "src/misc/lv_gc.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#define ESP_PLATFORM 1
#include "game_machine.c"
#include "imu_motion.h"
#include "nvs.h"

lv_ui guider_ui;
lv_group_t *g;
static lv_indev_t *keyboard;
static uint32_t key;
static bool pressed;
static bool encoder_pressed;
static int16_t encoder_diff;
static unsigned activity_count;
static unsigned led_frames;
static unsigned led_releases;
static bool fail_led_frame;
static bool fail_led_release;
static lv_color_t framebuffer[320 * 170];
static lv_color_t draw_buffer[320 * 34];
static const char *snapshot_dir;
static bool ui_processing;
static uint32_t flash_record = 7;
static uint32_t tilt_record;
static uint32_t staged_record;
static uint32_t staged_tilt;
static bool stage_dino;
static bool stage_tilt;
static unsigned flash_commits;
static imu_sample_t s_fake_imu;

/* Every NVS access must be outside the simulated LVGL critical section and
 * outside running gameplay, including recovery reads. */
static void assert_storage_context(void)
{
    assert(!ui_processing);
    assert(!s_game.active || !game_is_running());
}

void imu_motion_latest(imu_sample_t *out)
{
    if (out != NULL) *out = s_fake_imu;
}

const char *esp_err_to_name(esp_err_t error)
{
    (void)error;
    return "ok";
}

void test_esp_log(const char *tag, const char *format, ...)
{
    (void)tag;
    (void)format;
}

esp_err_t nvs_open(const char *name, nvs_open_mode_t mode, nvs_handle_t *handle)
{
    (void)mode;
    assert_storage_context();
    assert(strcmp(name, "DinoGame") == 0);
    *handle = 1;
    return ESP_OK;
}

esp_err_t nvs_get_u32(nvs_handle_t handle, const char *name, uint32_t *value)
{
    (void)handle;
    assert_storage_context();
    if (strcmp(name, "tilt") == 0) {
        *value = tilt_record;
        return ESP_OK;
    }
    assert(strcmp(name, "best") == 0);
    *value = flash_record;
    return ESP_OK;
}

esp_err_t nvs_set_u32(nvs_handle_t handle, const char *name, uint32_t value)
{
    (void)handle;
    assert_storage_context();
    if (strcmp(name, "tilt") == 0) {
        assert(value > tilt_record);
        staged_tilt = value;
        stage_tilt = true;
        return ESP_OK;
    }
    assert(strcmp(name, "best") == 0 && value > flash_record);
    staged_record = value;
    stage_dino = true;
    return ESP_OK;
}

esp_err_t nvs_commit(nvs_handle_t handle)
{
    (void)handle;
    assert_storage_context();
    if (stage_dino) {
        flash_record = staged_record;
        stage_dino = false;
    }
    if (stage_tilt) {
        tilt_record = staged_tilt;
        stage_tilt = false;
    }
    ++flash_commits;
    return ESP_OK;
}

void nvs_close(nvs_handle_t handle) { (void)handle; assert_storage_context(); }

void idle_sleep_note_activity(void) { ++activity_count; }
bool game_led_try_frame(const uint8_t rgb[10][3])
{
    (void)rgb;
    ++led_frames;
    return !fail_led_frame;
}
bool game_led_release(void) { ++led_releases; return !fail_led_release; }

static void read_keyboard(lv_indev_drv_t *driver, lv_indev_data_t *data)
{
    (void)driver;
    data->key = key;
    data->state = pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
}

static void read_encoder(lv_indev_drv_t *driver, lv_indev_data_t *data)
{
    (void)driver;
    data->key = LV_KEY_ENTER;
    data->state = encoder_pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    data->enc_diff = encoder_diff;
    encoder_diff = 0;
}

static void flush(lv_disp_drv_t *driver, const lv_area_t *area, lv_color_t *pixels)
{
    for (int y = area->y1; y <= area->y2; ++y) {
        for (int x = area->x1; x <= area->x2; ++x) {
            if (x >= 0 && x < 320 && y >= 0 && y < 170) framebuffer[y * 320 + x] = *pixels;
            ++pixels;
        }
    }
    lv_disp_flush_ready(driver);
}

static void main_event(lv_event_t *event)
{
    lv_obj_t *root = lv_event_get_target(event);
    if (lv_event_get_code(event) == LV_EVENT_SCREEN_LOADED) {
        lv_indev_set_group(keyboard, g);
    } else if (lv_event_get_code(event) == LV_EVENT_DELETE && guider_ui.screen_main == root) {
        guider_ui.screen_main = NULL;
        guider_ui.screen_main_del = true;
    }
}

void setup_scr_screen_main(lv_ui *ui)
{
    ui->screen_main = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_main, 320, 170);
    lv_obj_add_event_cb(ui->screen_main, main_event, LV_EVENT_ALL, NULL);
    lv_obj_t *button = lv_btn_create(ui->screen_main);
    lv_group_add_obj(g, button);
}

static void setup_game(lv_ui *ui)
{
    ui->screen_game = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_game, 320, 170);
}

/* Match Guider's real clean-before-delayed-load order. */
void ui_load_scr_animation(lv_ui *ui, lv_obj_t **next, bool create, bool *old_del,
                           ui_setup_scr_t setup, lv_scr_load_anim_t animation,
                           uint32_t time, uint32_t delay, bool clean, bool auto_del)
{
    if (auto_del && clean) lv_obj_clean(lv_scr_act());
    if (create) setup(ui);
    lv_scr_load_anim(*next, animation, time, delay, auto_del);
    *old_del = auto_del;
}

static void advance(uint32_t ms)
{
    for (uint32_t elapsed = 0; elapsed < ms; elapsed += 5) {
        lv_tick_inc(5);
        ui_processing = true;
        lv_timer_handler();
        game_machine_poll();
        ui_processing = false;
        game_machine_led_poll();
        game_machine_storage_poll();
    }
}

static unsigned timer_count(void)
{
    unsigned count = 0;
    lv_timer_t *timer = NULL;
    while ((timer = lv_timer_get_next(timer)) != NULL) ++count;
    return count;
}

static unsigned group_count(void)
{
    return _lv_ll_get_len(&LV_GC_ROOT(_lv_group_ll));
}

static void send_key(uint32_t value)
{
    key = value;
    pressed = true;
    advance(40);
    pressed = false;
    advance(40);
}

static void snapshot(const char *name)
{
    if (snapshot_dir == NULL) return;
    lv_obj_invalidate(lv_scr_act());
    lv_refr_now(NULL);
    char path[1024];
    snprintf(path, sizeof(path), "%s/%s.ppm", snapshot_dir, name);
    FILE *file = fopen(path, "wb");
    assert(file != NULL);
    fprintf(file, "P6\n320 170\n255\n");
    for (unsigned i = 0; i < 320 * 170; ++i) {
        lv_color32_t color = {.full = lv_color_to32(framebuffer[i])};
        uint8_t bytes[3] = {color.ch.red, color.ch.green, color.ch.blue};
        assert(fwrite(bytes, 1, 3, file) == 3);
    }
    fclose(file);
}

static void enter_game(void)
{
    ui_load_scr_animation(&guider_ui, &guider_ui.screen_game, true,
                          &guider_ui.screen_main_del, setup_game,
                          LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, true, true);
    game_machine_poll(); /* Bind DELETE while the old page is still active. */
    advance(430);
    assert(s_game.active && s_game.stage && s_game.group && s_game.timer);
    assert(keyboard->group == s_game.group);
    assert(s_game.room == GAME_ROOM_SELECT && s_game.catalog == 0);
    assert(game_machine_is_open());
}

static void assert_left(unsigned timers, unsigned groups)
{
    advance(400);
    assert(guider_ui.screen_game == NULL && s_game.screen == NULL);
    assert(s_game.stage == NULL && s_game.timer == NULL && s_game.group == NULL);
    assert(!game_machine_is_open());
    assert(keyboard->group == g && lv_scr_act() == guider_ui.screen_main);
    assert(timer_count() == timers && group_count() == groups);
}

int main(int argc, char **argv)
{
    if (argc == 3 && strcmp(argv[1], "--snapshots") == 0) snapshot_dir = argv[2];
    game_score_storage_init();
    assert(game_score_storage_best() == 7 && flash_commits == 0);
    lv_init();
    static lv_disp_draw_buf_t buffer;
    lv_disp_draw_buf_init(&buffer, draw_buffer, NULL, 320 * 34);
    static lv_disp_drv_t display_driver;
    lv_disp_drv_init(&display_driver);
    display_driver.hor_res = 320;
    display_driver.ver_res = 170;
    display_driver.draw_buf = &buffer;
    display_driver.flush_cb = flush;
    lv_disp_t *display = lv_disp_drv_register(&display_driver);
    static lv_indev_drv_t keyboard_driver;
    lv_indev_drv_init(&keyboard_driver);
    keyboard_driver.type = LV_INDEV_TYPE_KEYPAD;
    keyboard_driver.read_cb = read_keyboard;
    keyboard = lv_indev_drv_register(&keyboard_driver);
    g = lv_group_create();
    lv_indev_set_group(keyboard, g);
    lv_obj_t *blank = lv_scr_act();
    setup_scr_screen_main(&guider_ui);
    lv_scr_load(guider_ui.screen_main);
    lv_obj_del(blank);
    advance(300);
    unsigned baseline_timers = timer_count();
    unsigned baseline_groups = group_count();

    /* A page can be created and deleted before the incoming fade activates it. */
    setup_game(&guider_ui);
    screen_game_custom_init();
    assert(!s_game.active && keyboard->group == g);
    lv_obj_del(guider_ui.screen_game);
    assert(guider_ui.screen_game == NULL && s_game.screen == NULL);
    assert(timer_count() == baseline_timers);

    s_fake_imu.valid = true;
    s_fake_imu.accel_g[2] = 1.0f;
    enter_game();
    assert(s_game.model.best == flash_record && flash_commits == 0);
    snapshot("select");
    s_fake_imu.accel_g[0] = 0.46f;
    advance(400);
    assert(s_game.catalog == 1);
    snapshot("select_tilt");
    s_fake_imu.accel_g[0] = 0.0f;
    advance(80);
    send_key(LV_KEY_ENTER);
    assert(s_game.room == GAME_ROOM_TILT && s_game.tilt.phase == TILT_PHASE_RUNNING);
    advance(180);
    snapshot("tilt_running");
    s_game.tilt.gates[0].active = true;
    s_game.tilt.gates[0].cleared = false;
    s_game.tilt.gates[0].x = s_game.tilt.orb_x - 4.0f;
    s_game.tilt.gates[0].width = 20;
    s_game.tilt.gates[0].gap_y = 128.0f;
    s_game.tilt.gates[0].gap_h = 18.0f;
    for (int i = 0; i < 20 && s_game.tilt.phase == TILT_PHASE_RUNNING; ++i) advance(20);
    assert(s_game.tilt.phase == TILT_PHASE_GAME_OVER);
    advance(200);
    snapshot("tilt_gameover");
    send_key(LV_KEY_NEXT);
    send_key(LV_KEY_ENTER);
    assert_left(baseline_timers, baseline_groups);
    puts("PASS tilt select, IMU switch, orb play and home");

    unsigned score_commits = flash_commits;
    enter_game();
    assert(s_game.room == GAME_ROOM_SELECT && s_game.catalog == 0);
    unsigned active_timers = timer_count();
    unsigned active_groups = group_count();
    for (int i = 0; i < 30; ++i) screen_game_custom_init();
    assert(lv_obj_get_child_cnt(s_game.screen) == 1);
    assert(timer_count() == active_timers && group_count() == active_groups);
    assert(active_timers == baseline_timers + 1 && active_groups == baseline_groups + 1);

    send_key(LV_KEY_ENTER);
    assert(s_game.model.phase == DINO_PHASE_RUNNING);
    unsigned before_activity = activity_count;
    advance(950);
    send_key(LV_KEY_ENTER);
    assert(s_game.model.jump_y > 0 || s_game.model.velocity > 0);
    advance(130);
    snapshot("running");
    assert(flash_commits == score_commits); /* A running new record has not touched Flash. */
    assert(activity_count > before_activity && led_frames > 0);
    send_key(LV_KEY_NEXT);
    assert(s_game.model.phase == DINO_PHASE_PAUSED && s_game.selection == 0);
    assert(s_game.model.best == flash_record && flash_commits == score_commits + 1);
    float paused_distance = s_game.model.distance_px;
    before_activity = activity_count;
    advance(1600);
    assert(s_game.model.distance_px == paused_distance && activity_count == before_activity);
    snapshot("paused");
    send_key(LV_KEY_ENTER);
    assert(s_game.model.phase == DINO_PHASE_RUNNING);
    /* Allow natural collision, without bypassing the real physics timer. */
    for (int i = 0; i < 100 && s_game.model.phase == DINO_PHASE_RUNNING; ++i) advance(100);
    assert(s_game.model.phase == DINO_PHASE_GAME_OVER);
    assert(s_game.model.best == flash_record && flash_commits == score_commits + 2);
    advance(350);
    snapshot("gameover");
    uint32_t best = s_game.model.best;
    send_key(LV_KEY_ENTER);
    assert(s_game.model.phase == DINO_PHASE_RUNNING && s_game.model.best >= best);
    send_key(LV_KEY_NEXT); /* pause */
    send_key(LV_KEY_NEXT); /* HOME */
    assert(s_game.selection == 1);
    send_key(LV_KEY_ENTER);
    assert_left(baseline_timers, baseline_groups);
    assert(led_releases > 0);
    puts("PASS click/jump/pause/resume/retry/home through real LVGL input");
    game_score_storage_init(); /* Discard RAM storage state, retain emulated Flash. */
    assert(game_score_storage_best() == best && flash_commits == score_commits + 2);
    puts("PASS boot record in HI, pause/game-over commits outside LVGL, reboot restore");

    for (int i = 0; i < 300; ++i) {
        enter_game();
        assert(s_game.model.best >= best);
        send_key(LV_KEY_ENTER);
        advance(100);
        unsigned old_commits = flash_commits;
        if (i % 2) {
            /* Seed an earned record to cover exiting an unfinished run. */
            s_game.model.best = best + 50;
            /* Reproduce an unrelated screen replacing ours before its delayed DELETE. */
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_main, true,
                                  &guider_ui.screen_game_del, setup_scr_screen_main,
                                  LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, true, true);
            assert(s_game.timer == NULL && s_game.stage == NULL && !s_game.active);
            lv_indev_set_group(keyboard, g);
        } else {
            send_key(LV_KEY_NEXT);
            send_key(LV_KEY_NEXT);
            send_key(LV_KEY_ENTER);
        }
        assert_left(baseline_timers, baseline_groups);
        if (i % 2) {
            best += 50;
            assert(flash_record == best && flash_commits == old_commits + 1);
        } else {
            assert(flash_commits == old_commits); /* No new record, no writes. */
        }
    }
    puts("PASS 300 repeated entries and Guider clean/fade/delete cycles; timer/group counts stable");

    enter_game();
    fail_led_frame = true;
    advance(100);
    assert(s_led_release_pending);
    fail_led_release = true;
    send_key(LV_KEY_NEXT); /* READY -> HOME */
    send_key(LV_KEY_ENTER);
    assert(s_led_release_pending);
    fail_led_release = false;
    assert_left(baseline_timers, baseline_groups);
    assert(!s_led_release_pending);
    puts("PASS failed LED submission/release retained until cleanup succeeds");

    /* An actual ENCODER can combine click release and motion in the same read. */
    static lv_indev_drv_t encoder_driver;
    lv_indev_drv_init(&encoder_driver);
    encoder_driver.type = LV_INDEV_TYPE_ENCODER;
    encoder_driver.read_cb = read_encoder;
    lv_indev_t *encoder = lv_indev_drv_register(&encoder_driver);
    lv_indev_set_group(encoder, g);
    unsigned encoder_timers = timer_count();
    enter_game();
    send_key(LV_KEY_NEXT);
    assert(s_game.selection == 1 && encoder->group == s_game.group);
    encoder_pressed = true;
    advance(40);
    encoder_pressed = false;
    encoder_diff = 1;
    advance(40);
    assert_left(encoder_timers, baseline_groups);
    assert(encoder->group == g);
    lv_indev_delete(encoder);
    puts("PASS encoder release plus motion does not access a freed group");

    lv_indev_delete(keyboard);
    lv_group_del(g);
    lv_disp_remove(display);
    assert(group_count() == 0);
    puts("PASS LVGL display/input/group teardown");
    return 0;
}
