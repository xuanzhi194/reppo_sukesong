/* Real Keep adapter and LVGL objects; only board samples and unrelated UI stubbed. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "keep_machine.c"

lv_ui guider_ui;
lv_group_t *g;
int all_focus_time;
static imu_sample_t sample;
static uint32_t sample_sequence;
static bool asleep;
static unsigned activity;
static unsigned manual_refreshes;
static lv_color_t framebuffer[320 * 170];
static const char *preview_text;
static lv_obj_t *healthy[10], *withered[10];

void imu_motion_latest(imu_sample_t *out) { *out = sample; }
void idle_sleep_note_activity(void) { ++activity; }
bool idle_sleep_is_asleep(void) { return asleep; }
void group_focus_obj(lv_obj_t *obj) { lv_group_focus_obj(obj); }
void ui_font_bind(lv_obj_t *obj) { (void)obj; }
const char *ui_tr(const char *en, const char *zh) { (void)zh; return en; }
bool ui_lang_is_zh(void) { return true; }
extern const lv_font_t ui_todo_cjk_font;
const lv_font_t *ui_font_16(void) {
    static lv_font_t font;
    font = lv_font_SourceHanSansSC_Medium_16;
    font.fallback = &ui_todo_cjk_font;
    return &font;
}
void keep_ai_request(int date, bool chinese) { (void)date; (void)chinese; }
void keep_ai_refresh(int date, bool chinese) { (void)date; (void)chinese; ++manual_refreshes; }
bool keep_ai_poll(void) { return false; }
const char *keep_ai_text(unsigned index, bool chinese) {
    (void)chinese;
    if (preview_text) return preview_text;
    return (const char *[]){"AI pending recap", "AI done recap", "AI idea recap"}[index % 3];
}
void setup_scr_screen_main(lv_ui *ui) { ui->screen_main = lv_obj_create(NULL); }
void ui_load_scr_animation(lv_ui *ui, lv_obj_t **new_scr, bool new_scr_del,
                          bool *old_scr_del, ui_setup_scr_t setup_scr,
                          lv_scr_load_anim_t anim_type, uint32_t time,
                          uint32_t delay, bool is_clean, bool auto_del)
{
    (void)new_scr_del; (void)old_scr_del; (void)anim_type; (void)time;
    (void)delay; (void)is_clean; (void)auto_del;
    lv_obj_t *old = lv_scr_act();
    if (*new_scr == NULL) setup_scr(ui);
    lv_scr_load(*new_scr);
    lv_obj_del(old);
}

static void flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *pixels)
{
    for (int y = area->y1; y <= area->y2; ++y)
        for (int x = area->x1; x <= area->x2; ++x) {
            if (x >= 0 && x < 320 && y >= 0 && y < 170) framebuffer[y * 320 + x] = *pixels;
            ++pixels;
        }
    lv_disp_flush_ready(drv);
}

static void advance(unsigned ms)
{
    for (unsigned elapsed = 0; elapsed < ms; elapsed += 10) {
        lv_tick_inc(10);
        /* Hardware is 20Hz; UI polls much more often and sees duplicates. */
        if (lv_tick_get() % 50 == 0) {
            sample.sequence = ++sample_sequence;
            sample.timestamp_ms = lv_tick_get();
        }
        lv_timer_handler();
        keep_machine_poll();
    }
}

static void at_x(float x, unsigned ms)
{
    sample.valid = true;
    sample.accel_g[0] = x; sample.accel_g[1] = 0; sample.accel_g[2] = 0.85f;
    advance(ms);
}

static void enter(void)
{
    setup_scr_screen_keep(&guider_ui);
    lv_obj_t *screen = guider_ui.screen_keep;
    keep_collect_plant_imgs(healthy, withered);
    lv_scr_load(screen);
    keep_machine_poll();
    /* Picking up/holding the device must not block initial calibration. */
    sample.valid = true;
    sample.accel_g[0] = sample.accel_g[1] = 0; sample.accel_g[2] = 0.85f;
    sample.gyro_dps[0] = 35;
    advance(300);
    assert(s_keep.tilt.valid);
    at_x(0, 100);
}

static void visible(int level, bool wilted)
{
    for (int i = 0; i < 10; ++i) {
        assert(lv_obj_has_flag(healthy[i], LV_OBJ_FLAG_HIDDEN) ==
               (wilted || i != level - 1));
        assert(lv_obj_has_flag(withered[i], LV_OBJ_FLAG_HIDDEN) ==
               (!wilted || i != level - 1));
    }
    assert(strcmp(lv_label_get_text(guider_ui.screen_keep_label_state),
                  keep_state_names[level - 1]) == 0);
}

static void gesture(float x) { at_x(x, 100); at_x(0, 100); }
static void double_click(void)
{
    lv_event_send(guider_ui.screen_keep, LV_EVENT_CLICKED, NULL);
    advance(100);
    lv_event_send(guider_ui.screen_keep, LV_EVENT_CLICKED, NULL);
    advance(100);
}

int main(void)
{
    lv_init();
    static lv_color_t pixels[320 * 20];
    static lv_disp_draw_buf_t buffer;
    static lv_disp_drv_t display;
    lv_disp_draw_buf_init(&buffer, pixels, NULL, 320 * 20);
    lv_disp_drv_init(&display);
    display.hor_res = 320; display.ver_res = 170;
    display.draw_buf = &buffer; display.flush_cb = flush;
    lv_disp_drv_register(&display);
    setup_scr_screen_main(&guider_ui);
    lv_scr_load(guider_ui.screen_main);

    all_focus_time = 600; /* Actual level 3. */
    enter();
    visible(3, false);
    assert(strcmp(lv_label_get_text(guider_ui.screen_keep_label_focustime),
                  "600/900 MIN") == 0);
    at_x(0.04f, 500); visible(3, false); /* Small tremor. */
    at_x(0.5f, 100); visible(4, false); /* First fresh tilted sample switches. */
    at_x(-0.5f, 100); visible(3, false);
    at_x(0, 100);
    gesture(0.14f); visible(4, false); /* Gentle tilt below game's 0.20g gate. */
    assert(strcmp(lv_label_get_text(guider_ui.screen_keep_label_focustime),
                  "DEMO 4/10") == 0);
    assert(lv_bar_get_value(guider_ui.screen_keep_bar_focustime) == 33);
    assert(strcmp(lv_label_get_text(guider_ui.screen_keep_label_conversion), "AI pending recap") == 0);
    assert(lv_label_get_long_mode(guider_ui.screen_keep_label_conversion) == LV_LABEL_LONG_WRAP);
    if (getenv("REPPO_KEEP_SNAPSHOT")) {
        preview_text = "修好倾斜切换，代码终于不闹脾气了";
        keep_refresh_ui(); advance(100);
        FILE *file = fopen(getenv("REPPO_KEEP_SNAPSHOT"), "wb");
        assert(file);
        fprintf(file, "P6\n320 170\n255\n");
        for (unsigned i = 0; i < 320 * 170; ++i) {
            lv_color32_t color = {.full = lv_color_to32(framebuffer[i])};
            unsigned char rgb[] = {color.ch.red, color.ch.green, color.ch.blue};
            fwrite(rgb, 1, sizeof(rgb), file);
        }
        fclose(file); preview_text = NULL; keep_refresh_ui();
    }
    at_x(0.5f, 1850); visible(8, false); /* Immediate + three timed repeats. */
    at_x(-0.5f, 100); visible(7, false); /* Reversal ignores repeat cooldown. */
    at_x(-0.5f, 600); visible(6, false);
    assert(strstr(lv_label_get_text(s_keep.tilt_hint), "Left") != NULL);
    gesture(0.14f); visible(7, false);
    gesture(-0.14f); visible(6, false);
    for (int i = 0; i < 4; ++i) gesture(0.5f);
    visible(10, false);
    gesture(0.5f); visible(1, false);
    gesture(-0.5f); visible(10, false);
    assert(all_focus_time == 600 && s_keep.last_ymd == 0);
    const unsigned old_line = s_keep.summary_index;
    advance(7100);
    assert(s_keep.summary_index == (old_line + 1) % KEEP_AI_LINES);
    visible(10, false);
    puts("PASS immediate tilt, held repeats, immediate reversal, level wrapping and unchanged focus");

    sample.valid = false; advance(500); visible(10, false);
    at_x(4, 300); visible(10, false); /* Shock is not a gesture. */
    asleep = true; at_x(0.5f, 500); visible(10, false);
    asleep = false; at_x(0, 500); gesture(0.5f); visible(1, false);
    s_keep.last_ymd = 20261029; s_keep.today_ymd = 20261030;
    advance(50); visible(1, true);
    gesture(0.5f); visible(2, true);
    s_keep.last_ymd = 20261030;
    advance(50); visible(2, false);
    puts("PASS missing IMU, shocks, sleep/resume and RTC wither updates during preview");

    /* The first click waits for the double-click window; it cannot exit. */
    lv_event_send(guider_ui.screen_keep, LV_EVENT_CLICKED, NULL);
    advance(200);
    assert(manual_refreshes == 0 && lv_scr_act() == guider_ui.screen_keep);
    advance(260);
    assert(manual_refreshes == 1 && lv_scr_act() == guider_ui.screen_keep);
    assert(s_keep.summary_index == 0);
    advance(500);
    assert(manual_refreshes == 1);
    /* Pending clicks must not survive a switch away or a deleted screen. */
    lv_event_send(guider_ui.screen_keep, LV_EVENT_CLICKED, NULL);

    /* Switching away without deleting must also discard preview on re-entry. */
    lv_scr_load(guider_ui.screen_main); advance(50);
    at_x(0.5f, 500);
    assert(manual_refreshes == 1);
    at_x(0, 100);
    lv_scr_load(guider_ui.screen_keep); advance(50);
    visible(3, false);
    at_x(0, 500); gesture(0.5f); visible(4, false);
    double_click();
    unsigned old_activity = activity;
    assert(lv_scr_act() == guider_ui.screen_main);
    assert(guider_ui.screen_keep == NULL);
    at_x(-0.5f, 500);
    assert(activity == old_activity);
    assert(manual_refreshes == 1); /* Double click never starts an AI refresh. */
    puts("PASS single refresh after 450ms, no double-click refresh, cancelled pending click on exit");
    for (int i = 0; i < 20; ++i) {
        enter(); visible(3, false);
        gesture(-0.5f); visible(2, false);
        /* Guider can clean children before deleting the screen. */
        lv_obj_clean(guider_ui.screen_keep);
        advance(100);
        double_click();
        assert(guider_ui.screen_keep == NULL);
    }
    assert(all_focus_time == 600);
    assert(manual_refreshes == 1);
    puts("PASS inactive page isolation, click exit, re-entry reset and 20 clean/delete cycles");
    return 0;
}
