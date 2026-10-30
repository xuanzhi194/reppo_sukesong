/**
 * screen_keep olive-tree care (lives in main, not ui_custom/custom).
 * Strong symbols override the Guider-safe weak stubs in custom.c.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lvgl.h"
#include "gui_guider.h"
#include "custom.h"
#include "ui_locale.h"
#include "keep_machine.h"
#include "imu_motion.h"
#include "keep_ai.h"
#include "keep_tilt.h"
#ifdef ESP_PLATFORM
#include "nvs.h"
#include "rx8130.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#endif

#define KEEP_LEVEL_COUNT    10
#define KEEP_WITHER_DAYS    1
#define KEEP_NVS_NS         "Keep"
#define KEEP_NVS_KEY        "last_ymd"
#define KEEP_DOUBLE_CLICK_MS 450

/* Cumulative focus hours to reach each level (same table as the old AI page). */
static const int keep_level_hour[KEEP_LEVEL_COUNT] = {
    3, 9, 15, 30, 60, 100, 150, 230, 350, 500
};

static const char * const keep_state_names[KEEP_LEVEL_COUNT] = {
    "First Sprout",
    "Slender Stem",
    "Unfurling Leaf",
    "Young Branch",
    "Reaching Skyward",
    "Lush Foliage",
    "Gentle Bloom",
    "Tender Fruit",
    "Olive Crown",
    "Timeless Roots",
};

static struct {
    bool alive;
    bool leaving;
    bool events_bound;
    bool back_bound;
    bool displayed_withered;
    lv_obj_t *bound_scr;
    int last_ymd;
    int today_ymd;
    unsigned summary_index;
    uint32_t summary_at, ai_poll_at;
    bool click_pending;
    uint32_t click_at;
    int preview_level; /* 0 = actual focus level; RAM only, cleared on entry. */
    keep_tilt_t tilt;
    lv_obj_t *tilt_hint;
} s_keep;

static void keep_clear_focus_box(lv_obj_t *obj)
{
    if (obj == NULL || !lv_obj_is_valid(obj)) {
        return;
    }
    lv_obj_set_style_outline_width(obj, 0, LV_STATE_FOCUSED);
    lv_obj_set_style_outline_width(obj, 0, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_border_width(obj, 0, LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(obj, 0, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_shadow_width(obj, 0, LV_STATE_FOCUSED);
}

static void keep_keypad_group_reset(void)
{
    if (g == NULL) {
        g = lv_group_create();
    } else {
        lv_group_remove_all_objs(g);
        lv_group_set_edge_cb(g, NULL);
        lv_group_set_wrap(g, true);
    }
    lv_group_set_default(g);
    lv_indev_t *indev = NULL;
    while ((indev = lv_indev_get_next(indev))) {
        if (lv_indev_get_type(indev) == LV_INDEV_TYPE_KEYPAD) {
            lv_indev_set_group(indev, g);
        }
    }
}

static void keep_forget_screen(void)
{
    if (guider_ui.screen_keep != NULL &&
        !lv_obj_is_valid(guider_ui.screen_keep)) {
        guider_ui.screen_keep = NULL;
        guider_ui.screen_keep_img_bg = NULL;
        guider_ui.screen_keep_img_dialog = NULL;
        guider_ui.screen_keep_label_state = NULL;
        guider_ui.screen_keep_label_conversion = NULL;
        guider_ui.screen_keep_label_focustime = NULL;
        guider_ui.screen_keep_bar_focustime = NULL;
        guider_ui.screen_keep_img_process_icon = NULL;
    }
    s_keep.alive = false;
    s_keep.bound_scr = NULL;
    s_keep.tilt_hint = NULL;
    s_keep.events_bound = false;
    s_keep.back_bound = false;
    s_keep.leaving = false;
    s_keep.click_pending = false;
}

static bool keep_scr_ok(void)
{
    if (guider_ui.screen_keep == NULL ||
        !lv_obj_is_valid(guider_ui.screen_keep)) {
        keep_forget_screen();
        return false;
    }
    return true;
}

static bool keep_obj_ok(lv_obj_t *obj)
{
    return obj != NULL && lv_obj_is_valid(obj);
}

static int keep_ymd_pack(int y, int m, int d)
{
    if (y < 2000 || m < 1 || m > 12 || d < 1 || d > 31) {
        return 0;
    }
    return y * 10000 + m * 100 + d;
}

static int keep_ymd_to_days(int ymd)
{
    int y;
    int m;
    int d;

    if (ymd <= 0) {
        return 0;
    }
    y = ymd / 10000;
    m = (ymd / 100) % 100;
    d = ymd % 100;
    if (m < 3) {
        y--;
        m += 12;
    }
    return 365 * y + y / 4 - y / 100 + y / 400 + (153 * m - 457) / 5 + d;
}

#ifdef ESP_PLATFORM
static int keep_today_ymd(void)
{
    rx8130_time_t now;

    if (rx8130_get_time(I2C_NUM_0, &now) != ESP_OK) {
        return 0;
    }
    return keep_ymd_pack((int)now.year, (int)now.month, (int)now.date);
}

static void keep_nvs_save_ymd(int ymd)
{
    nvs_handle_t h;

    if (ymd <= 0) {
        return;
    }
    if (nvs_open(KEEP_NVS_NS, NVS_READWRITE, &h) != ESP_OK) {
        return;
    }
    nvs_set_i32(h, KEEP_NVS_KEY, (int32_t)ymd);
    nvs_commit(h);
    nvs_close(h);
}

static int keep_nvs_load_ymd(void)
{
    nvs_handle_t h;
    int32_t stored = 0;

    if (nvs_open(KEEP_NVS_NS, NVS_READONLY, &h) != ESP_OK) {
        return 0;
    }
    if (nvs_get_i32(h, KEEP_NVS_KEY, &stored) != ESP_OK || stored <= 0) {
        nvs_close(h);
        return 0;
    }
    nvs_close(h);
    return (int)stored;
}
#endif

#ifdef ESP_PLATFORM
static void keep_cache_today(void)
{
    int today = keep_today_ymd();

    if (today > 0) {
        s_keep.today_ymd = today;
    }
}
#endif

static bool keep_is_withered(void)
{
    int today = s_keep.today_ymd;
    int last = s_keep.last_ymd;
    int idle;

    if (today <= 0 || last <= 0) {
        return false;
    }
    idle = keep_ymd_to_days(today) - keep_ymd_to_days(last);
    return idle >= KEEP_WITHER_DAYS;
}

static void keep_level_from_focus(int *level, int *span_cur,
                                 int *span_need, int *next_total)
{
    int i;
    int total = all_focus_time;
    int prev = 0;

    if (total < 0) {
        total = 0;
    }
    for (i = 0; i < KEEP_LEVEL_COUNT; i++) {
        int need = keep_level_hour[i] * 60;
        if (total < need) {
            *level = i + 1;
            *span_cur = total - prev;
            *span_need = need - prev;
            *next_total = need;
            return;
        }
        prev = need;
    }
    *level = KEEP_LEVEL_COUNT;
    *span_cur = 1;
    *span_need = 1;
    *next_total = keep_level_hour[KEEP_LEVEL_COUNT - 1] * 60;
}

static void keep_collect_plant_imgs(lv_obj_t **ok, lv_obj_t **wither)
{
    ok[0] = guider_ui.screen_keep_img_lv1;
    ok[1] = guider_ui.screen_keep_img_lv2;
    ok[2] = guider_ui.screen_keep_img_lv3;
    ok[3] = guider_ui.screen_keep_img_lv4;
    ok[4] = guider_ui.screen_keep_img_lv5;
    ok[5] = guider_ui.screen_keep_img_lv6;
    ok[6] = guider_ui.screen_keep_img_lv7;
    ok[7] = guider_ui.screen_keep_img_lv8;
    ok[8] = guider_ui.screen_keep_img_lv9;
    ok[9] = guider_ui.screen_keep_img_lv10;
    wither[0] = guider_ui.screen_keep_img_lv1_wither;
    wither[1] = guider_ui.screen_keep_img_lv2_wither;
    wither[2] = guider_ui.screen_keep_img_lv3_wither;
    wither[3] = guider_ui.screen_keep_img_lv4_wither;
    wither[4] = guider_ui.screen_keep_img_lv5_wither;
    wither[5] = guider_ui.screen_keep_img_lv6_wither;
    wither[6] = guider_ui.screen_keep_img_lv7_wither;
    wither[7] = guider_ui.screen_keep_img_lv8_wither;
    wither[8] = guider_ui.screen_keep_img_lv9_wither;
    wither[9] = guider_ui.screen_keep_img_lv10_wither;
}

static void keep_set_hidden(lv_obj_t *obj, bool hidden)
{
    if (!keep_obj_ok(obj)) {
        return;
    }
    if (hidden) {
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
}

static void keep_sync_process_icon(int percent)
{
    lv_obj_t *bar = guider_ui.screen_keep_bar_focustime;
    lv_obj_t *icon = guider_ui.screen_keep_img_process_icon;
    lv_coord_t bar_x;
    lv_coord_t bar_w;
    lv_coord_t icon_w;
    lv_coord_t travel;
    lv_coord_t x;

    if (!keep_obj_ok(bar) || !keep_obj_ok(icon)) {
        return;
    }
    if (percent < 0) {
        percent = 0;
    }
    if (percent > 100) {
        percent = 100;
    }
    bar_x = lv_obj_get_x(bar);
    bar_w = lv_obj_get_width(bar);
    icon_w = lv_obj_get_width(icon);
    if (icon_w < 1) {
        icon_w = 14;
    }
    travel = bar_w - icon_w;
    if (travel < 0) {
        travel = 0;
    }
    x = bar_x + (lv_coord_t)(travel * percent / 100);
    lv_obj_set_x(icon, x);
}

static void keep_refresh_ui(void)
{
    lv_obj_t *ok[KEEP_LEVEL_COUNT];
    lv_obj_t *wither[KEEP_LEVEL_COUNT];
    int level = 1;
    int span_cur = 0;
    int span_need = 180;
    int next_total = 180;
    int percent = 0;
    int shown;
    bool wilted;
    int i;

    if (!keep_scr_ok()) {
        return;
    }
    keep_collect_plant_imgs(ok, wither);
    keep_level_from_focus(&level, &span_cur, &span_need, &next_total);
    if (s_keep.preview_level > 0) {
        level = s_keep.preview_level;
    }
    wilted = keep_is_withered();
    s_keep.displayed_withered = wilted;
    shown = all_focus_time < 0 ? 0 : all_focus_time;

    for (i = 0; i < KEEP_LEVEL_COUNT; i++) {
        bool show_ok = (!wilted && (i + 1) == level);
        bool show_w = (wilted && (i + 1) == level);

        keep_set_hidden(ok[i], !show_ok);
        keep_set_hidden(wither[i], !show_w);
        if (keep_obj_ok(ok[i])) {
            lv_obj_clear_flag(ok[i], LV_OBJ_FLAG_CLICKABLE);
        }
        if (keep_obj_ok(wither[i])) {
            lv_obj_clear_flag(wither[i], LV_OBJ_FLAG_CLICKABLE);
        }
    }

    if (level >= KEEP_LEVEL_COUNT && shown >= next_total) {
        percent = 100;
    } else if (span_need > 0) {
        percent = span_cur * 100 / span_need;
        if (percent > 100) {
            percent = 100;
        }
        if (percent < 0) {
            percent = 0;
        }
    }

    if (s_keep.preview_level > 0) {
        percent = (level - 1) * 100 / (KEEP_LEVEL_COUNT - 1);
    }
    if (keep_obj_ok(guider_ui.screen_keep_bar_focustime)) {
        lv_bar_set_value(guider_ui.screen_keep_bar_focustime, percent, LV_ANIM_OFF);
    }
    keep_sync_process_icon(percent);
    if (keep_obj_ok(guider_ui.screen_keep_label_focustime)) {
        if (s_keep.preview_level > 0) {
            lv_label_set_text_fmt(guider_ui.screen_keep_label_focustime,
                                  "DEMO %d/%d", level, KEEP_LEVEL_COUNT);
        } else {
            lv_label_set_text_fmt(guider_ui.screen_keep_label_focustime,
                                  "%d/%d MIN", shown, next_total);
        }
    }
    if (keep_obj_ok(guider_ui.screen_keep_label_state) &&
        level >= 1 && level <= KEEP_LEVEL_COUNT) {
        lv_label_set_text(guider_ui.screen_keep_label_state,
                          ui_tr(keep_state_names[level - 1],
                                (const char *[]){
                                    "初芽", "细茎", "展叶", "新枝", "向阳",
                                    "繁叶", "初花", "青果", "冠成", "深根",
                                }[level - 1]));
    }
    if (keep_obj_ok(guider_ui.screen_keep_label_conversion) &&
        level >= 1 && level <= KEEP_LEVEL_COUNT) {
        const char *text = keep_ai_text(s_keep.summary_index, ui_lang_is_zh());
        /* Do not restart long-text scrolling at every status poll. */
        if (strcmp(lv_label_get_text(guider_ui.screen_keep_label_conversion), text) != 0) {
            lv_point_t size;
            lv_txt_get_size(&size, text, ui_font_16(), 0, 0, 187, LV_TEXT_FLAG_NONE);
            lv_label_set_long_mode(guider_ui.screen_keep_label_conversion,
                                   size.y > 56 ? LV_LABEL_LONG_SCROLL : LV_LABEL_LONG_WRAP);
            lv_label_set_text(guider_ui.screen_keep_label_conversion, text);
        }
    }
}

static void keep_update_tilt_hint(void)
{
    if (!keep_obj_ok(s_keep.tilt_hint)) return;
    const char *text;
    if (idle_sleep_is_asleep()) text = ui_tr("Wake to browse", "唤醒后继续展示");
    else if (!s_keep.tilt.valid) text = ui_tr("Waiting for IMU", "等待倾斜信号");
    else if (s_keep.tilt.direction > 0) text = ui_tr("Right: next levels", "右倾：连续下一级");
    else if (s_keep.tilt.direction < 0) text = ui_tr("Left: previous levels", "左倾：连续上一级");
    else text = ui_tr("Tilt to browse", "左右倾斜即可切换");
    if (strcmp(lv_label_get_text(s_keep.tilt_hint), text) != 0)
        lv_label_set_text(s_keep.tilt_hint, text);
}

/* Runs only on the active Keep page under the existing LVGL mutex. Reads the
 * cached IMU sample, never I2C; no focus counters or NVS writes in this path. */
static void keep_poll_tilt(void)
{
    uint32_t now = lv_tick_get();
    imu_sample_t sample = {0};
    int direction = 0;
    if (idle_sleep_is_asleep()) {
        keep_tilt_reset(&s_keep.tilt);
    } else {
        imu_motion_latest(&sample);
        direction = keep_tilt_update(&s_keep.tilt, &sample);
    }
    keep_update_tilt_hint();
    if (!direction) return;

    int level, span_cur, span_need, next_total;
    keep_level_from_focus(&level, &span_cur, &span_need, &next_total);
    if (s_keep.preview_level > 0) level = s_keep.preview_level;
    s_keep.preview_level = (level - 1 + direction + KEEP_LEVEL_COUNT) %
                          KEEP_LEVEL_COUNT + 1;
    s_keep.summary_index = (s_keep.summary_index + 1) % KEEP_AI_LINES;
    s_keep.summary_at = now;
    keep_refresh_ui();
    idle_sleep_note_activity();
#ifdef ESP_PLATFORM
    ESP_LOGI("keep", "tilt demo level=%d direction=%d", s_keep.preview_level, direction);
#endif
}

static void keep_leave_to_main_async(void *unused)
{
    LV_UNUSED(unused);
    if (!keep_scr_ok()) {
        s_keep.leaving = false;
        return;
    }
    if (g != NULL) {
        lv_group_set_edge_cb(g, NULL);
        lv_group_set_wrap(g, true);
    }
    ui_load_scr_animation(&guider_ui, &guider_ui.screen_main,
                          guider_ui.screen_main_del, &guider_ui.screen_keep_del,
                          setup_scr_screen_main, LV_SCR_LOAD_ANIM_FADE_ON,
                          100, 100, true, true);
}

static void keep_on_screen_delete(lv_event_t *e)
{
    lv_obj_t *target;

    if (lv_event_get_code(e) != LV_EVENT_DELETE) {
        return;
    }
    target = lv_event_get_target(e);
    if (guider_ui.screen_keep == target) {
        guider_ui.screen_keep = NULL;
        guider_ui.screen_keep_img_bg = NULL;
        guider_ui.screen_keep_img_dialog = NULL;
        guider_ui.screen_keep_label_state = NULL;
        guider_ui.screen_keep_label_conversion = NULL;
        guider_ui.screen_keep_label_focustime = NULL;
        guider_ui.screen_keep_bar_focustime = NULL;
        guider_ui.screen_keep_img_process_icon = NULL;
    }
    s_keep.alive = false;
    s_keep.bound_scr = NULL;
    s_keep.tilt_hint = NULL;
    s_keep.events_bound = false;
    s_keep.back_bound = false;
    s_keep.leaving = false;
    s_keep.click_pending = false;
}

static void keep_refresh_summary_now(void)
{
    s_keep.click_pending = false;
    s_keep.summary_index = 0;
    s_keep.summary_at = s_keep.ai_poll_at = lv_tick_get();
    keep_ai_refresh(s_keep.today_ymd, ui_lang_is_zh());
    keep_refresh_ui();
}

static void keep_on_click(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }
    if (s_keep.leaving) {
        return;
    }
    if (!keep_scr_ok() || lv_scr_act() != guider_ui.screen_keep) {
        return;
    }
    idle_sleep_note_activity();
    if (s_keep.click_pending && lv_tick_elaps(s_keep.click_at) < KEEP_DOUBLE_CLICK_MS) {
        s_keep.click_pending = false;
        s_keep.leaving = true;
        lv_async_call(keep_leave_to_main_async, NULL);
        return;
    }
    /* A late second click starts a new single-click window. */
    if (s_keep.click_pending) keep_refresh_summary_now();
    s_keep.click_pending = true;
    s_keep.click_at = lv_tick_get();
}

static void keep_bind_back_obj(lv_obj_t *obj)
{
    if (!keep_obj_ok(obj)) {
        return;
    }
    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(obj, keep_on_click, LV_EVENT_CLICKED, NULL);
}

static void keep_bind_delete(void)
{
    if (!keep_obj_ok(guider_ui.screen_keep) || s_keep.events_bound) {
        return;
    }
    lv_obj_add_event_cb(guider_ui.screen_keep, keep_on_screen_delete,
                        LV_EVENT_DELETE, NULL);
    s_keep.events_bound = true;
}

void screen_keep_buttion_add(void)
{
    if (!keep_scr_ok() || lv_scr_act() != guider_ui.screen_keep) {
        return;
    }
    keep_bind_delete();
    keep_keypad_group_reset();
    lv_obj_add_flag(guider_ui.screen_keep, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(guider_ui.screen_keep, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    if (!s_keep.back_bound) {
        keep_bind_back_obj(guider_ui.screen_keep);
        keep_bind_back_obj(guider_ui.screen_keep_img_bg);
        keep_bind_back_obj(guider_ui.screen_keep_img_dialog);
        keep_bind_back_obj(guider_ui.screen_keep_label_state);
        keep_bind_back_obj(guider_ui.screen_keep_label_conversion);
        keep_bind_back_obj(guider_ui.screen_keep_label_focustime);
        s_keep.back_bound = true;
    }
    lv_group_add_obj(g, guider_ui.screen_keep);
    ui_font_bind(guider_ui.screen_keep);
    keep_clear_focus_box(guider_ui.screen_keep);
    group_focus_obj(guider_ui.screen_keep);
}

void screen_keep_custom_init(void)
{
    if (!keep_scr_ok() || s_keep.leaving) {
        return;
    }
    /* Bind DELETE before the fade delay ends so auto_del cannot leak the
     * pointer. Do not steal the shared keypad group until this screen is act. */
    keep_bind_delete();
    if (lv_scr_act() != guider_ui.screen_keep) {
        return;
    }
    lv_async_call_cancel(keep_leave_to_main_async, NULL);
    if (!s_keep.alive || s_keep.bound_scr != guider_ui.screen_keep) {
        s_keep.click_pending = false;
        s_keep.preview_level = 0;
        s_keep.summary_index = 0;
        s_keep.summary_at = s_keep.ai_poll_at = lv_tick_get();
        keep_ai_request(s_keep.today_ymd, ui_lang_is_zh());
        keep_tilt_reset(&s_keep.tilt);
    }
    s_keep.alive = true;
    s_keep.bound_scr = guider_ui.screen_keep;
    if (keep_obj_ok(guider_ui.screen_keep_label_conversion)) {
        lv_obj_t *label = guider_ui.screen_keep_label_conversion;
        lv_obj_set_pos(label, 116, 43);
        lv_obj_set_size(label, 187, 56);
        lv_obj_set_style_text_font(label, ui_font_16(), LV_PART_MAIN);
        lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    }
    if (!keep_obj_ok(s_keep.tilt_hint)) {
        s_keep.tilt_hint = lv_label_create(guider_ui.screen_keep);
        lv_obj_set_pos(s_keep.tilt_hint, 9, 154);
        lv_obj_set_size(s_keep.tilt_hint, 212, 16);
        lv_obj_set_style_text_font(s_keep.tilt_hint, ui_font_16(), LV_PART_MAIN);
        lv_obj_set_style_text_color(s_keep.tilt_hint, lv_color_hex(0x64716b), LV_PART_MAIN);
        lv_obj_clear_flag(s_keep.tilt_hint, LV_OBJ_FLAG_CLICKABLE);
        lv_label_set_long_mode(s_keep.tilt_hint, LV_LABEL_LONG_CLIP);
    }
    keep_update_tilt_hint();
    keep_refresh_ui();
    screen_keep_buttion_add();
#ifdef ESP_PLATFORM
    ESP_LOGI("keep", "olive tree ready, focus=%d wither=%d",
             all_focus_time, (int)keep_is_withered());
#endif
}

void keep_machine_init(void)
{
#ifdef ESP_PLATFORM
    s_keep.last_ymd = keep_nvs_load_ymd();
    keep_cache_today();
#endif
}

void keep_note_activity(void)
{
#ifdef ESP_PLATFORM
    keep_cache_today();
    if (s_keep.today_ymd <= 0) {
        return;
    }
    if (s_keep.today_ymd == s_keep.last_ymd) {
        return;
    }
    s_keep.last_ymd = s_keep.today_ymd;
    keep_nvs_save_ymd(s_keep.last_ymd);
#endif
}

void keep_machine_refresh_today(void)
{
#ifdef ESP_PLATFORM
    keep_cache_today();
#endif
}

void keep_machine_tick(void)
{
#ifdef ESP_PLATFORM
    static TickType_t last_tick;
    TickType_t now = xTaskGetTickCount();

    if (s_keep.today_ymd > 0 &&
        (now - last_tick) < pdMS_TO_TICKS(30000)) {
        return;
    }
    last_tick = now;
    keep_cache_today();
#endif
}

void keep_machine_poll(void)
{
    lv_obj_t *act = lv_scr_act();
    bool summary_changed = keep_ai_poll();

    if (!keep_scr_ok() || act == NULL || act != guider_ui.screen_keep) {
        s_keep.click_pending = false;
        if (!s_keep.leaving) {
            s_keep.alive = false;
            s_keep.bound_scr = NULL;
        }
        return;
    }
    if (s_keep.leaving) {
        return;
    }
    if (s_keep.alive && s_keep.bound_scr == act) {
        /* RTC sync or a day change can alter the plant while this page stays open.
         * poll runs under the LVGL mutex; cache updates never touch widgets. */
        uint32_t now = lv_tick_get();
        if (idle_sleep_is_asleep()) s_keep.click_pending = false;
        if (s_keep.click_pending && now - s_keep.click_at >= KEEP_DOUBLE_CLICK_MS)
            keep_refresh_summary_now();
        if (now - s_keep.ai_poll_at >= 1000) {
            s_keep.ai_poll_at = now;
            if (!s_keep.click_pending && !idle_sleep_is_asleep())
                keep_ai_request(s_keep.today_ymd, ui_lang_is_zh());
            summary_changed = true;
        }
        if (now - s_keep.summary_at >= 7000) {
            s_keep.summary_at = now;
            s_keep.summary_index = (s_keep.summary_index + 1) % KEEP_AI_LINES;
            summary_changed = true;
        }
        if (summary_changed || s_keep.displayed_withered != keep_is_withered()) {
            keep_refresh_ui();
        }
        keep_poll_tilt();
        return;
    }
    screen_keep_custom_init();
}
