/**
 * screen_gash claw machine (lives in main, not ui_custom/custom).
 * Strong symbols override the Guider-safe weak stubs in custom.c.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lvgl.h"
#include "gui_guider.h"
#include "custom.h"
#include "gash_machine.h"
#include "gash_tasks.h"
#ifdef ESP_PLATFORM
#include "todo_storage.h"
#include "esp_random.h"
#include "esp_log.h"
#endif

static void gash_clear_focus_box(lv_obj_t *obj)
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

static void gash_keypad_group_reset(void)
{
    if (g == NULL) {
        g = lv_group_create();
    } else {
        lv_group_remove_all_objs(g);
    }
    lv_group_set_default(g);
    lv_indev_t *indev = NULL;
    while ((indev = lv_indev_get_next(indev))) {
        if (lv_indev_get_type(indev) == LV_INDEV_TYPE_KEYPAD) {
            lv_indev_set_group(indev, g);
        }
    }
}

static void gash_load_main(void)
{
    ui_load_scr_animation(&guider_ui, &guider_ui.screen_main,
                          guider_ui.screen_main_del, &guider_ui.screen_gash_del,
                          setup_scr_screen_main, LV_SCR_LOAD_ANIM_FADE_ON,
                          100, 100, true, true);
}

 /* ========== screen_gash claw machine ========== */
#define GASH_CLAW_X_REST      41
#define GASH_CLAW_X_MIN       28
#define GASH_CLAW_X_MAX       232
#define GASH_CLAW_X_STEP      12
#define GASH_CLAW_Y_REST      8
#define GASH_CLAW_Y_DOWN      50
#define GASH_CLAW3_X_OFF     (-1)
#define GASH_BALL_W           26
#define GASH_BALL_H           26
#define GASH_BALL_X_NUDGE     1
#define GASH_BALL_Y_OFF       40
#define GASH_TASK_X           88
#define GASH_TASK_Y           60
#define GASH_TASK_W           155
#define GASH_STICK_PIVOT_X    5
#define GASH_STICK_PIVOT_Y    16
#define GASH_STICK_TILT       260
#define GASH_STICK_IDLE_MS    140
#define GASH_TASK_DURATION    25

typedef enum {
    GASH_PHASE_AIM = 0,
    GASH_PHASE_GRAB,
    GASH_PHASE_RESULT,
} gash_phase_t;

typedef enum {
    GASH_GRAB_EXTEND = 0,
    GASH_GRAB_DOWN,
    GASH_GRAB_CLOSE,
    GASH_GRAB_CATCH,
    GASH_GRAB_UP,
    GASH_GRAB_REVEAL,
} gash_grab_step_t;

static struct {
    gash_phase_t phase;
    gash_grab_step_t grab_step;
    lv_coord_t claw_x;
    lv_coord_t claw_x_target;
    lv_coord_t claw_y;
    int32_t stick_angle;
    int8_t cat_idx;
    int8_t ball_idx;
    lv_obj_t *bound_scr;
    lv_obj_t *catcher;
    lv_obj_t *label_cat;
    lv_obj_t *label_task;
    lv_obj_t *cont_cat;
    lv_obj_t *img_cat_ball;
    lv_obj_t *held_ball;
    lv_timer_t *stick_idle;
    const char *task_text;
    bool events_bound;
    bool leaving;
    bool alive;
} s_gash;

static void gash_aim_group_bind(void);
static void gash_result_group_bind(void);
static void gash_grab_run_step(void);
static void gash_on_aim_clicked(lv_event_t *e);
static void gash_on_addtodo(lv_event_t *e);
static void gash_on_again(lv_event_t *e);
static void gash_on_screen_delete(lv_event_t *e);

static bool gash_obj_ok(lv_obj_t *obj)
{
    return obj != NULL && lv_obj_is_valid(obj);
}

static bool gash_scr_ok(void)
{
    return gash_obj_ok(guider_ui.screen_gash);
}

static uint32_t gash_rand(uint32_t max_inclusive)
{
#ifdef ESP_PLATFORM
    if (max_inclusive == 0) {
        return 0;
    }
    return esp_random() % (max_inclusive + 1);
#else
    return lv_rand(0, max_inclusive);
#endif
}

static lv_obj_t *gash_ball_obj(int idx)
{
    switch (idx) {
    case 0:
        return guider_ui.screen_gash_img_ball1;
    case 1:
        return guider_ui.screen_gash_img_ball2;
    case 2:
        return guider_ui.screen_gash_img_ball3;
    default:
        return guider_ui.screen_gash_img_ball4;
    }
}

static void gash_set_hidden(lv_obj_t *obj, bool hidden)
{
    if (!gash_obj_ok(obj)) {
        return;
    }
    if (hidden) {
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
}

static void gash_set_img_opa(lv_obj_t *obj, lv_opa_t opa)
{
    if (gash_obj_ok(obj)) {
        lv_obj_set_style_img_opa(obj, opa, LV_PART_MAIN);
    }
}

static lv_coord_t gash_ball_x_for_claw(lv_coord_t claw_x)
{
    return (lv_coord_t)(claw_x + 7 - (GASH_BALL_W / 2) + GASH_BALL_X_NUDGE);
}

static lv_coord_t gash_ball_y_for_claw(lv_coord_t claw_y)
{
    return (lv_coord_t)(claw_y + GASH_BALL_Y_OFF);
}

static void gash_apply_claw_pose(void)
{
    lv_coord_t x = s_gash.claw_x;
    lv_coord_t y = s_gash.claw_y;

    if (gash_obj_ok(guider_ui.screen_gash_img_claw1)) {
        lv_obj_set_pos(guider_ui.screen_gash_img_claw1, x, y);
    }
    if (gash_obj_ok(guider_ui.screen_gash_img_claw2)) {
        lv_obj_set_pos(guider_ui.screen_gash_img_claw2, x, y);
    }
    if (gash_obj_ok(guider_ui.screen_gash_img_claw3)) {
        lv_obj_set_pos(guider_ui.screen_gash_img_claw3,
                       (lv_coord_t)(x + GASH_CLAW3_X_OFF), y);
    }
    if (gash_obj_ok(s_gash.held_ball)) {
        lv_obj_set_pos(s_gash.held_ball,
                       gash_ball_x_for_claw(x),
                       gash_ball_y_for_claw(y));
    }
}

static void gash_stick_angle_exec(void *obj, int32_t v)
{
    s_gash.stick_angle = v;
    if (gash_obj_ok(obj)) {
        lv_img_set_angle((lv_obj_t *)obj, (int16_t)v);
    }
}

static void gash_img_opa_exec(void *obj, int32_t v)
{
    gash_set_img_opa((lv_obj_t *)obj, (lv_opa_t)v);
}

static void gash_obj_opa_exec(void *obj, int32_t v)
{
    if (gash_obj_ok(obj)) {
        lv_obj_set_style_opa((lv_obj_t *)obj, (lv_opa_t)v, LV_PART_MAIN);
    }
}

static void gash_claw_x_exec(void *var, int32_t v)
{
    (void)var;
    s_gash.claw_x = (lv_coord_t)v;
    gash_apply_claw_pose();
}

static void gash_claw_y_exec(void *var, int32_t v)
{
    (void)var;
    s_gash.claw_y = (lv_coord_t)v;
    gash_apply_claw_pose();
}

static void gash_obj_y_exec(void *obj, int32_t v)
{
    if (gash_obj_ok(obj)) {
        lv_obj_set_y((lv_obj_t *)obj, (lv_coord_t)v);
    }
}

static void gash_ball_zoom_exec(void *obj, int32_t v)
{
    if (gash_obj_ok(obj)) {
        lv_img_set_zoom((lv_obj_t *)obj, (uint16_t)v);
    }
}

static void gash_anim_del_obj(lv_obj_t *obj)
{
    if (gash_obj_ok(obj)) {
        lv_anim_del(obj, NULL);
    }
}

static void gash_stop_motion(void)
{
    if (s_gash.stick_idle != NULL) {
        lv_timer_del(s_gash.stick_idle);
        s_gash.stick_idle = NULL;
    }
    gash_anim_del_obj(guider_ui.screen_gash_img_stick);
    gash_anim_del_obj(guider_ui.screen_gash_img_claw1);
    gash_anim_del_obj(guider_ui.screen_gash_img_claw2);
    gash_anim_del_obj(guider_ui.screen_gash_img_claw3);
    gash_anim_del_obj(guider_ui.screen_gash_img_ball1);
    gash_anim_del_obj(guider_ui.screen_gash_img_ball2);
    gash_anim_del_obj(guider_ui.screen_gash_img_ball3);
    gash_anim_del_obj(guider_ui.screen_gash_img_ball4);
    gash_anim_del_obj(guider_ui.screen_gash_img_randontask);
    gash_anim_del_obj(guider_ui.screen_gash_btn_addtodo);
    gash_anim_del_obj(guider_ui.screen_gash_btn_again);
    gash_anim_del_obj(s_gash.label_cat);
    gash_anim_del_obj(s_gash.label_task);
    gash_anim_del_obj(s_gash.cont_cat);
    gash_anim_del_obj(s_gash.img_cat_ball);
    gash_anim_del_obj(s_gash.catcher);
    if (gash_scr_ok()) {
        lv_anim_del(guider_ui.screen_gash, gash_claw_x_exec);
        lv_anim_del(guider_ui.screen_gash, gash_claw_y_exec);
    }
}

static void gash_clear_group_hooks(void)
{
    if (g != NULL) {
        lv_group_set_edge_cb(g, NULL);
        lv_group_set_wrap(g, true);
    }
}

static void gash_set_stick_angle(int32_t angle, uint32_t time_ms)
{
    lv_obj_t *stick = guider_ui.screen_gash_img_stick;
    if (!gash_obj_ok(stick)) {
        return;
    }
    lv_anim_del(stick, gash_stick_angle_exec);
    if (time_ms == 0) {
        gash_stick_angle_exec(stick, angle);
        return;
    }
    ui_animation(stick, (int32_t)time_ms, 0, s_gash.stick_angle, angle,
                 &lv_anim_path_ease_out, 0, 0, 0, 0,
                 gash_stick_angle_exec, NULL, NULL, NULL);
}

static void gash_stick_idle_cb(lv_timer_t *t)
{
    (void)t;
    s_gash.stick_idle = NULL;
    if (s_gash.phase == GASH_PHASE_AIM) {
        gash_set_stick_angle(0, 180);
    }
}

static void gash_nudge_stick(int dir)
{
    int32_t dest = (dir > 0) ? GASH_STICK_TILT : -GASH_STICK_TILT;
    gash_set_stick_angle(dest, 80);
    if (s_gash.stick_idle != NULL) {
        lv_timer_reset(s_gash.stick_idle);
        lv_timer_set_period(s_gash.stick_idle, GASH_STICK_IDLE_MS);
        lv_timer_set_repeat_count(s_gash.stick_idle, 1);
    } else {
        s_gash.stick_idle = lv_timer_create(gash_stick_idle_cb,
                                            GASH_STICK_IDLE_MS, NULL);
        lv_timer_set_repeat_count(s_gash.stick_idle, 1);
    }
}

static void gash_nudge_claw(int dir)
{
    lv_coord_t dest = (lv_coord_t)(s_gash.claw_x_target + dir * GASH_CLAW_X_STEP);
    if (dest < GASH_CLAW_X_MIN) {
        dest = GASH_CLAW_X_MIN;
    } else if (dest > GASH_CLAW_X_MAX) {
        dest = GASH_CLAW_X_MAX;
    }
    s_gash.claw_x_target = dest;
    if (!gash_scr_ok()) {
        return;
    }
    lv_anim_del(guider_ui.screen_gash, gash_claw_x_exec);
    ui_animation(guider_ui.screen_gash, 140, 0, s_gash.claw_x, dest,
                 &lv_anim_path_ease_out, 0, 0, 0, 0,
                 gash_claw_x_exec, NULL, NULL, NULL);
}

static void gash_aim_edge_cb(lv_group_t *group, bool next)
{
    (void)group;
    if (s_gash.phase != GASH_PHASE_AIM) {
        return;
    }
    if (next) {
        gash_nudge_stick(1);
        gash_nudge_claw(1);
    } else {
        gash_nudge_stick(-1);
        gash_nudge_claw(-1);
    }
}

static void gash_show_claw(int which)
{
    gash_set_hidden(guider_ui.screen_gash_img_claw1, which != 1);
    gash_set_hidden(guider_ui.screen_gash_img_claw2, which != 2);
    gash_set_hidden(guider_ui.screen_gash_img_claw3, which != 3);
    gash_set_img_opa(guider_ui.screen_gash_img_claw1, LV_OPA_COVER);
    gash_set_img_opa(guider_ui.screen_gash_img_claw2, LV_OPA_COVER);
    gash_set_img_opa(guider_ui.screen_gash_img_claw3, LV_OPA_COVER);
}

static void gash_crossfade_claws(lv_obj_t *from, lv_obj_t *to,
                                 uint32_t time_ms, lv_anim_ready_cb_t ready)
{
    if (gash_obj_ok(from)) {
        lv_obj_clear_flag(from, LV_OBJ_FLAG_HIDDEN);
        gash_set_img_opa(from, LV_OPA_COVER);
        ui_animation(from, (int32_t)time_ms, 0, LV_OPA_COVER, LV_OPA_TRANSP,
                     &lv_anim_path_ease_in, 0, 0, 0, 0,
                     gash_img_opa_exec, NULL, NULL, NULL);
    }
    if (gash_obj_ok(to)) {
        lv_obj_clear_flag(to, LV_OBJ_FLAG_HIDDEN);
        gash_set_img_opa(to, LV_OPA_TRANSP);
        ui_animation(to, (int32_t)time_ms, 0, LV_OPA_TRANSP, LV_OPA_COVER,
                     &lv_anim_path_ease_out, 0, 0, 0, 0,
                     gash_img_opa_exec, NULL, ready, NULL);
    } else if (ready != NULL) {
        ready(NULL);
    }
}

static void gash_hide_all_balls(void)
{
    int i;
    s_gash.held_ball = NULL;
    for (i = 0; i < 4; i++) {
        lv_obj_t *ball = gash_ball_obj(i);
        if (!gash_obj_ok(ball)) {
            continue;
        }
        lv_anim_del(ball, NULL);
        lv_img_set_zoom(ball, 256);
        lv_img_set_angle(ball, 0);
        lv_img_set_pivot(ball, GASH_BALL_W / 2, GASH_BALL_H / 2);
        gash_set_img_opa(ball, LV_OPA_COVER);
        lv_obj_add_flag(ball, LV_OBJ_FLAG_HIDDEN);
    }
}

static void gash_pick_task(void)
{
    s_gash.cat_idx = (int8_t)gash_rand(GASH_TASK_CAT_COUNT - 1);
    s_gash.ball_idx = s_gash.cat_idx;
    s_gash.task_text = gash_tasks[s_gash.cat_idx][gash_rand(GASH_TASK_PER_CAT - 1)];
}

static void gash_fade_in(lv_obj_t *obj, uint32_t time_ms, uint32_t delay_ms)
{
    if (!gash_obj_ok(obj)) {
        return;
    }
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_opa(obj, LV_OPA_TRANSP, LV_PART_MAIN);
    ui_animation(obj, (int32_t)time_ms, (int32_t)delay_ms,
                 LV_OPA_TRANSP, LV_OPA_COVER, &lv_anim_path_ease_out,
                 0, 0, 0, 0, gash_obj_opa_exec, NULL, NULL, NULL);
}

static void gash_ensure_result_labels(void)
{
    if (!gash_scr_ok()) {
        return;
    }
    if (!gash_obj_ok(s_gash.label_task)) {
        s_gash.label_task = lv_label_create(guider_ui.screen_gash);
        lv_obj_set_style_text_font(s_gash.label_task, &lv_font_montserratMedium_16,
                                   LV_PART_MAIN);
        lv_obj_set_style_text_align(s_gash.label_task, LV_TEXT_ALIGN_CENTER,
                                    LV_PART_MAIN);
        lv_obj_set_style_text_color(s_gash.label_task, lv_color_hex(0x13407a),
                                    LV_PART_MAIN);
        lv_obj_set_style_text_opa(s_gash.label_task, LV_OPA_COVER, LV_PART_MAIN);
    }
    lv_obj_set_pos(s_gash.label_task, GASH_TASK_X, GASH_TASK_Y);
    lv_obj_set_size(s_gash.label_task, GASH_TASK_W, 22);
    lv_label_set_long_mode(s_gash.label_task, LV_LABEL_LONG_SCROLL_CIRCULAR);
    if (!gash_obj_ok(s_gash.cont_cat)) {
        s_gash.cont_cat = lv_obj_create(guider_ui.screen_gash);
        lv_obj_remove_style_all(s_gash.cont_cat);
        lv_obj_set_flex_flow(s_gash.cont_cat, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(s_gash.cont_cat, LV_FLEX_ALIGN_CENTER,
                              LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_column(s_gash.cont_cat, 6, LV_PART_MAIN);
        lv_obj_set_style_pad_all(s_gash.cont_cat, 0, LV_PART_MAIN);
        lv_obj_set_style_bg_opa(s_gash.cont_cat, LV_OPA_TRANSP, LV_PART_MAIN);
        lv_obj_set_style_border_width(s_gash.cont_cat, 0, LV_PART_MAIN);
        lv_obj_set_style_radius(s_gash.cont_cat, 0, LV_PART_MAIN);
        lv_obj_set_style_clip_corner(s_gash.cont_cat, false, LV_PART_MAIN);
        lv_obj_clear_flag(s_gash.cont_cat, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_clear_flag(s_gash.cont_cat, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_flag(s_gash.cont_cat, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
    }
    lv_obj_set_pos(s_gash.cont_cat, 76, 88);
    lv_obj_set_size(s_gash.cont_cat, 179, 28);
    if (!gash_obj_ok(s_gash.label_cat)) {
        s_gash.label_cat = lv_label_create(s_gash.cont_cat);
        lv_label_set_long_mode(s_gash.label_cat, LV_LABEL_LONG_CLIP);
        lv_obj_set_style_text_font(s_gash.label_cat, &lv_font_montserratMedium_12,
                                   LV_PART_MAIN);
        lv_obj_set_style_text_align(s_gash.label_cat, LV_TEXT_ALIGN_CENTER,
                                    LV_PART_MAIN);
        lv_obj_set_style_text_opa(s_gash.label_cat, LV_OPA_COVER, LV_PART_MAIN);
    }
    if (!gash_obj_ok(s_gash.img_cat_ball)) {
        s_gash.img_cat_ball = lv_img_create(s_gash.cont_cat);
    }
    lv_obj_set_size(s_gash.img_cat_ball, GASH_BALL_W, GASH_BALL_H);
    lv_img_set_pivot(s_gash.img_cat_ball, GASH_BALL_W / 2, GASH_BALL_H / 2);
    lv_img_set_zoom(s_gash.img_cat_ball, 256);
    lv_obj_set_style_radius(s_gash.img_cat_ball, 0, LV_PART_MAIN);
    lv_obj_set_style_clip_corner(s_gash.img_cat_ball, false, LV_PART_MAIN);
    lv_obj_clear_flag(s_gash.img_cat_ball, LV_OBJ_FLAG_SCROLLABLE);
}

static const uint32_t gash_cat_colors[GASH_TASK_CAT_COUNT] = {
    0x13407a, 0x2e7d4f, 0xc47b2b, 0xa33d7c,
};

static void gash_show_result_ui(void)
{
    lv_obj_t *ball = s_gash.held_ball;

    gash_ensure_result_labels();
    if (gash_obj_ok(s_gash.label_task) && s_gash.task_text != NULL) {
        lv_label_set_text(s_gash.label_task, s_gash.task_text);
    }
    if (gash_obj_ok(s_gash.label_cat) &&
        s_gash.cat_idx >= 0 && s_gash.cat_idx < GASH_TASK_CAT_COUNT) {
        lv_label_set_text(s_gash.label_cat, gash_cat_names[s_gash.cat_idx]);
        lv_obj_set_style_text_color(s_gash.label_cat,
                                    lv_color_hex(gash_cat_colors[s_gash.cat_idx]),
                                    LV_PART_MAIN);
    }
    {
        lv_obj_t *src_ball = gash_obj_ok(ball) ? ball : gash_ball_obj(s_gash.ball_idx);
        if (gash_obj_ok(s_gash.img_cat_ball) && gash_obj_ok(src_ball)) {
            lv_img_set_src(s_gash.img_cat_ball, lv_img_get_src(src_ball));
            lv_img_set_zoom(s_gash.img_cat_ball, 256);
            gash_set_img_opa(s_gash.img_cat_ball, LV_OPA_COVER);
            lv_obj_clear_flag(s_gash.img_cat_ball, LV_OBJ_FLAG_HIDDEN);
        }
    }
    if (gash_obj_ok(s_gash.label_cat)) {
        lv_obj_clear_flag(s_gash.label_cat, LV_OBJ_FLAG_HIDDEN);
    }

    if (gash_obj_ok(ball)) {
        ui_animation(ball, 220, 0, LV_OPA_COVER, LV_OPA_TRANSP,
                     &lv_anim_path_ease_in, 0, 0, 0, 0,
                     gash_img_opa_exec, NULL, NULL, NULL);
    }
    gash_set_img_opa(guider_ui.screen_gash_img_claw3, LV_OPA_COVER);
    if (gash_obj_ok(guider_ui.screen_gash_img_claw3)) {
        ui_animation(guider_ui.screen_gash_img_claw3, 220, 0,
                     LV_OPA_COVER, LV_OPA_TRANSP, &lv_anim_path_ease_in,
                     0, 0, 0, 0, gash_img_opa_exec, NULL, NULL, NULL);
    }

    if (gash_obj_ok(guider_ui.screen_gash_img_randontask)) {
        lv_obj_move_foreground(guider_ui.screen_gash_img_randontask);
        lv_obj_set_y(guider_ui.screen_gash_img_randontask, 24);
        gash_fade_in(guider_ui.screen_gash_img_randontask, 260, 40);
        ui_animation(guider_ui.screen_gash_img_randontask, 280, 40, 24, 17,
                     &lv_anim_path_ease_out, 0, 0, 0, 0,
                     gash_obj_y_exec, NULL, NULL, NULL);
    }
    if (gash_obj_ok(s_gash.label_task)) {
        lv_obj_move_foreground(s_gash.label_task);
        gash_fade_in(s_gash.label_task, 240, 90);
    }
    if (gash_obj_ok(s_gash.cont_cat)) {
        lv_obj_move_foreground(s_gash.cont_cat);
        gash_fade_in(s_gash.cont_cat, 240, 120);
    }
    if (gash_obj_ok(guider_ui.screen_gash_btn_addtodo)) {
        lv_obj_move_foreground(guider_ui.screen_gash_btn_addtodo);
        lv_obj_set_y(guider_ui.screen_gash_btn_addtodo, 130);
        gash_fade_in(guider_ui.screen_gash_btn_addtodo, 220, 160);
        ui_animation(guider_ui.screen_gash_btn_addtodo, 240, 160, 130, 122,
                     &lv_anim_path_ease_out, 0, 0, 0, 0,
                     gash_obj_y_exec, NULL, NULL, NULL);
    }
    if (gash_obj_ok(guider_ui.screen_gash_btn_again)) {
        lv_obj_move_foreground(guider_ui.screen_gash_btn_again);
        lv_obj_set_y(guider_ui.screen_gash_btn_again, 130);
        gash_fade_in(guider_ui.screen_gash_btn_again, 220, 180);
        ui_animation(guider_ui.screen_gash_btn_again, 240, 180, 130, 122,
                     &lv_anim_path_ease_out, 0, 0, 0, 0,
                     gash_obj_y_exec, NULL, NULL, NULL);
    }

    s_gash.phase = GASH_PHASE_RESULT;
    gash_result_group_bind();
}

static void gash_grab_ready(lv_anim_t *a)
{
    (void)a;
    if (s_gash.phase != GASH_PHASE_GRAB) {
        return;
    }
    s_gash.grab_step = (gash_grab_step_t)(s_gash.grab_step + 1);
    gash_grab_run_step();
}

static void gash_grab_run_step(void)
{
    lv_obj_t *ball;

    if (!gash_scr_ok() || s_gash.phase != GASH_PHASE_GRAB) {
        return;
    }

    switch (s_gash.grab_step) {
    case GASH_GRAB_EXTEND:
        gash_set_stick_angle(0, 160);
        gash_apply_claw_pose();
        gash_crossfade_claws(guider_ui.screen_gash_img_claw1,
                             guider_ui.screen_gash_img_claw2,
                             90, gash_grab_ready);
        break;
    case GASH_GRAB_DOWN:
        if (gash_obj_ok(guider_ui.screen_gash_img_claw1)) {
            lv_obj_add_flag(guider_ui.screen_gash_img_claw1, LV_OBJ_FLAG_HIDDEN);
            gash_set_img_opa(guider_ui.screen_gash_img_claw1, LV_OPA_COVER);
        }
        lv_anim_del(guider_ui.screen_gash, gash_claw_y_exec);
        ui_animation(guider_ui.screen_gash, 320, 0, s_gash.claw_y, GASH_CLAW_Y_DOWN,
                     &lv_anim_path_ease_in, 0, 0, 0, 0,
                     gash_claw_y_exec, NULL, gash_grab_ready, NULL);
        break;
    case GASH_GRAB_CLOSE:
        gash_crossfade_claws(guider_ui.screen_gash_img_claw2,
                             guider_ui.screen_gash_img_claw3,
                             80, gash_grab_ready);
        break;
    case GASH_GRAB_CATCH:
        if (gash_obj_ok(guider_ui.screen_gash_img_claw2)) {
            lv_obj_add_flag(guider_ui.screen_gash_img_claw2, LV_OBJ_FLAG_HIDDEN);
            gash_set_img_opa(guider_ui.screen_gash_img_claw2, LV_OPA_COVER);
        }
        gash_pick_task();
        ball = gash_ball_obj(s_gash.ball_idx);
        s_gash.held_ball = gash_obj_ok(ball) ? ball : NULL;
        if (gash_obj_ok(ball)) {
            lv_img_set_pivot(ball, GASH_BALL_W / 2, GASH_BALL_H / 2);
            lv_obj_set_style_clip_corner(ball, false, LV_PART_MAIN);
            lv_obj_set_pos(ball, gash_ball_x_for_claw(s_gash.claw_x),
                           gash_ball_y_for_claw(s_gash.claw_y));
            lv_img_set_zoom(ball, 170);
            gash_set_img_opa(ball, LV_OPA_TRANSP);
            lv_obj_clear_flag(ball, LV_OBJ_FLAG_HIDDEN);
            if (gash_obj_ok(guider_ui.screen_gash_img_claw3)) {
                lv_obj_move_foreground(guider_ui.screen_gash_img_claw3);
            }
            ui_animation(ball, 160, 0, 170, 268, &lv_anim_path_overshoot,
                         0, 0, 0, 0, gash_ball_zoom_exec, NULL, NULL, NULL);
            ui_animation(ball, 150, 0, LV_OPA_TRANSP, LV_OPA_COVER,
                         &lv_anim_path_ease_out, 0, 0, 0, 0,
                         gash_img_opa_exec, NULL, gash_grab_ready, NULL);
        } else {
            gash_grab_ready(NULL);
        }
        break;
    case GASH_GRAB_UP:
        if (gash_obj_ok(s_gash.held_ball)) {
            lv_img_set_zoom(s_gash.held_ball, 256);
        }
        lv_anim_del(guider_ui.screen_gash, gash_claw_y_exec);
        ui_animation(guider_ui.screen_gash, 420, 40, s_gash.claw_y, GASH_CLAW_Y_REST,
                     &lv_anim_path_ease_out, 0, 0, 0, 0,
                     gash_claw_y_exec, NULL, gash_grab_ready, NULL);
        break;
    case GASH_GRAB_REVEAL:
        gash_show_result_ui();
        break;
    default:
        break;
    }
}

static void gash_start_grab(void)
{
    if (s_gash.phase != GASH_PHASE_AIM) {
        return;
    }
    s_gash.phase = GASH_PHASE_GRAB;
    s_gash.grab_step = GASH_GRAB_EXTEND;
    gash_clear_group_hooks();
    if (s_gash.stick_idle != NULL) {
        lv_timer_del(s_gash.stick_idle);
        s_gash.stick_idle = NULL;
    }
    lv_anim_del(guider_ui.screen_gash, gash_claw_x_exec);
    gash_set_stick_angle(GASH_STICK_TILT + 80, 90);
    gash_grab_run_step();
}

static void gash_on_aim_clicked(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }
    if (s_gash.phase == GASH_PHASE_AIM) {
        gash_start_grab();
    }
}

static void gash_recalc_todo_counts(void)
{
    uint8_t i;
    done_sum = 0;
    for (i = 0; i < ui_todo_list.count; i++) {
        if (ui_todo_list.items[i].done) {
            done_sum++;
        }
    }
    todo_sum = (int8_t)(ui_todo_list.count - done_sum);
}

#ifdef ESP_PLATFORM
static void gash_persist_todo_list(void)
{
    FILE *f;
    uint8_t i;

    if (!todo_storage_lock(pdMS_TO_TICKS(200))) {
        return;
    }
    g_todo_list = ui_todo_list;
    f = fopen("/spiffs/todo.csv", "w");
    if (f != NULL) {
        for (i = 0; i < g_todo_list.count; i++) {
            fprintf(f, "%s,%u,%u,%s\n",
                    g_todo_list.items[i].name,
                    (unsigned)g_todo_list.items[i].duration_min,
                    (unsigned)g_todo_list.items[i].done,
                    g_todo_list.items[i].planned_date);
        }
        fclose(f);
    }
    _nvs_save_done();
    todo_storage_unlock();
}
#endif

static bool gash_add_current_todo(void)
{
    uint8_t idx;
    if (s_gash.task_text == NULL || s_gash.task_text[0] == '\0') {
        return false;
    }
    if (ui_todo_list.count >= TODO_MAX_ITEMS) {
        return false;
    }
    idx = ui_todo_list.count;
    memset(&ui_todo_list.items[idx], 0, sizeof(ui_todo_list.items[idx]));
    strncpy(ui_todo_list.items[idx].name, s_gash.task_text, TODO_NAME_LEN - 1);
#ifdef ESP_PLATFORM
    todo_storage_sanitize_task_name(ui_todo_list.items[idx].name, TODO_NAME_LEN);
#endif
    ui_todo_list.items[idx].duration_min = GASH_TASK_DURATION;
    ui_todo_list.items[idx].done = 0;
    ui_todo_list.items[idx].planned_date[0] = '\0';
    ui_todo_list.count++;
    gash_recalc_todo_counts();
#ifdef ESP_PLATFORM
    gash_persist_todo_list();
#endif
    return true;
}

static void gash_abandon_session(void)
{
    if (s_gash.stick_idle != NULL) {
        lv_timer_del(s_gash.stick_idle);
        s_gash.stick_idle = NULL;
    }
    gash_clear_group_hooks();
    if (g != NULL) {
        lv_group_remove_all_objs(g);
    }
    s_gash.leaving = true;
    s_gash.alive = false;
    s_gash.events_bound = false;
    s_gash.bound_scr = NULL;
    s_gash.catcher = NULL;
    s_gash.label_cat = NULL;
    s_gash.label_task = NULL;
    s_gash.cont_cat = NULL;
    s_gash.img_cat_ball = NULL;
    s_gash.held_ball = NULL;
}

static void gash_leave_to_main_async(void *unused)
{
    LV_UNUSED(unused);
    if (gash_scr_ok()) {
        gash_stop_motion();
    }
    gash_abandon_session();
    gash_load_main();
}

static void gash_on_addtodo(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }
    if (s_gash.phase != GASH_PHASE_RESULT || s_gash.leaving) {
        return;
    }
    gash_add_current_todo();
    s_gash.leaving = true;
    /* 不能在按钮 CLICKED 里 clean 当前屏，否则会删掉正在跑回调的控件。 */
    lv_async_call(gash_leave_to_main_async, NULL);
}

static void gash_on_again(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }
    if (s_gash.phase != GASH_PHASE_RESULT) {
        return;
    }
    screen_gash_custom_init();
}

static void gash_on_screen_delete(lv_event_t *e)
{
    lv_obj_t *target;

    if (lv_event_get_code(e) != LV_EVENT_DELETE) {
        return;
    }
    target = lv_event_get_target(e);
    if (s_gash.stick_idle != NULL) {
        lv_timer_del(s_gash.stick_idle);
        s_gash.stick_idle = NULL;
    }
    gash_clear_group_hooks();
    if (guider_ui.screen_gash == target) {
        guider_ui.screen_gash = NULL;
        guider_ui.screen_gash_img_stick = NULL;
        guider_ui.screen_gash_img_claw1 = NULL;
        guider_ui.screen_gash_img_claw2 = NULL;
        guider_ui.screen_gash_img_claw3 = NULL;
        guider_ui.screen_gash_img_ball1 = NULL;
        guider_ui.screen_gash_img_ball2 = NULL;
        guider_ui.screen_gash_img_ball3 = NULL;
        guider_ui.screen_gash_img_ball4 = NULL;
        guider_ui.screen_gash_img_randontask = NULL;
        guider_ui.screen_gash_btn_addtodo = NULL;
        guider_ui.screen_gash_btn_again = NULL;
        guider_ui.screen_gash_btn_addtodo_label = NULL;
        guider_ui.screen_gash_btn_again_label = NULL;
    }
    memset(&s_gash, 0, sizeof(s_gash));
}

static void gash_prepare_widgets(void)
{
    lv_obj_t *stick = guider_ui.screen_gash_img_stick;
    int i;

    gash_hide_all_balls();
    gash_show_claw(1);
    gash_set_hidden(guider_ui.screen_gash_img_randontask, true);
    gash_set_hidden(guider_ui.screen_gash_btn_addtodo, true);
    gash_set_hidden(guider_ui.screen_gash_btn_again, true);
    if (gash_obj_ok(s_gash.label_task)) {
        lv_obj_add_flag(s_gash.label_task, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_style_opa(s_gash.label_task, LV_OPA_COVER, LV_PART_MAIN);
    }
    if (gash_obj_ok(s_gash.cont_cat)) {
        lv_obj_add_flag(s_gash.cont_cat, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_style_opa(s_gash.cont_cat, LV_OPA_COVER, LV_PART_MAIN);
    }
    if (gash_obj_ok(guider_ui.screen_gash_img_randontask)) {
        lv_obj_set_pos(guider_ui.screen_gash_img_randontask, 66, 17);
        lv_obj_set_style_opa(guider_ui.screen_gash_img_randontask,
                             LV_OPA_COVER, LV_PART_MAIN);
    }
    if (gash_obj_ok(guider_ui.screen_gash_btn_addtodo)) {
        lv_obj_set_pos(guider_ui.screen_gash_btn_addtodo, 84, 122);
        lv_obj_set_style_opa(guider_ui.screen_gash_btn_addtodo,
                             LV_OPA_COVER, LV_PART_MAIN);
    }
    if (gash_obj_ok(guider_ui.screen_gash_btn_again)) {
        lv_obj_set_pos(guider_ui.screen_gash_btn_again, 170, 122);
        lv_obj_set_style_opa(guider_ui.screen_gash_btn_again,
                             LV_OPA_COVER, LV_PART_MAIN);
    }

    s_gash.claw_x = GASH_CLAW_X_REST;
    s_gash.claw_x_target = GASH_CLAW_X_REST;
    s_gash.claw_y = GASH_CLAW_Y_REST;
    s_gash.stick_angle = 0;
    s_gash.task_text = NULL;
    s_gash.cat_idx = -1;
    s_gash.ball_idx = -1;
    gash_apply_claw_pose();

    if (gash_obj_ok(stick)) {
        lv_obj_set_style_clip_corner(stick, false, LV_PART_MAIN);
        lv_img_set_pivot(stick, GASH_STICK_PIVOT_X, GASH_STICK_PIVOT_Y);
        lv_img_set_angle(stick, 0);
        lv_obj_clear_flag(stick, LV_OBJ_FLAG_HIDDEN);
    }
    for (i = 1; i <= 3; i++) {
        lv_obj_t *claw = (i == 1) ? guider_ui.screen_gash_img_claw1 :
                         (i == 2) ? guider_ui.screen_gash_img_claw2 :
                         
                         guider_ui.screen_gash_img_claw3;
        if (gash_obj_ok(claw)) {
            lv_obj_set_style_clip_corner(claw, false, LV_PART_MAIN);
            lv_obj_clear_flag(claw, LV_OBJ_FLAG_CLICKABLE);
        }
    }
    if (gash_obj_ok(stick)) {
        lv_obj_clear_flag(stick, LV_OBJ_FLAG_CLICKABLE);
    }
}

static void gash_ensure_catcher(void)
{
    if (!gash_scr_ok()) {
        return;
    }
    if (!gash_obj_ok(s_gash.catcher)) {
        s_gash.catcher = lv_obj_create(guider_ui.screen_gash);
        lv_obj_remove_style_all(s_gash.catcher);
        lv_obj_set_pos(s_gash.catcher, 0, 0);
        lv_obj_set_size(s_gash.catcher, 320, 170);
        lv_obj_set_style_bg_opa(s_gash.catcher, LV_OPA_TRANSP, LV_PART_MAIN);
        lv_obj_set_style_border_width(s_gash.catcher, 0, LV_PART_MAIN);
        lv_obj_set_style_outline_width(s_gash.catcher, 0, LV_PART_MAIN);
        lv_obj_set_style_pad_all(s_gash.catcher, 0, LV_PART_MAIN);
        lv_obj_clear_flag(s_gash.catcher, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_flag(s_gash.catcher, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_flag(s_gash.catcher, LV_OBJ_FLAG_CLICK_FOCUSABLE);
        lv_obj_add_event_cb(s_gash.catcher, gash_on_aim_clicked,
                            LV_EVENT_CLICKED, NULL);
        gash_clear_focus_box(s_gash.catcher);
    }
    lv_obj_clear_flag(s_gash.catcher, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(s_gash.catcher);
}

static void gash_aim_group_bind(void)
{
    gash_keypad_group_reset();
    gash_ensure_catcher();
    if (gash_obj_ok(s_gash.catcher)) {
        lv_group_add_obj(g, s_gash.catcher);
        gash_clear_focus_box(s_gash.catcher);
        group_focus_obj(s_gash.catcher);
    }
    if (g != NULL) {
        lv_group_set_wrap(g, true);
        lv_group_set_edge_cb(g, gash_aim_edge_cb);
    }
}

static void gash_result_group_bind(void)
{
    gash_keypad_group_reset();
    gash_clear_group_hooks();
    if (gash_obj_ok(s_gash.catcher)) {
        lv_obj_add_flag(s_gash.catcher, LV_OBJ_FLAG_HIDDEN);
    }
    if (gash_obj_ok(guider_ui.screen_gash_btn_addtodo)) {
        lv_group_add_obj(g, guider_ui.screen_gash_btn_addtodo);
        group_focus_obj(guider_ui.screen_gash_btn_addtodo);
    }
    if (gash_obj_ok(guider_ui.screen_gash_btn_again)) {
        lv_group_add_obj(g, guider_ui.screen_gash_btn_again);
    }
    if (g != NULL) {
        lv_group_set_wrap(g, true);
    }
}

static void gash_bind_events(void)
{
    if (!gash_scr_ok()) {
        return;
    }
    if (s_gash.bound_scr == guider_ui.screen_gash && s_gash.events_bound) {
        return;
    }
    s_gash.bound_scr = guider_ui.screen_gash;
    s_gash.events_bound = true;
    lv_obj_add_event_cb(guider_ui.screen_gash, gash_on_screen_delete,
                        LV_EVENT_DELETE, NULL);
    if (gash_obj_ok(guider_ui.screen_gash_btn_addtodo)) {
        lv_obj_add_event_cb(guider_ui.screen_gash_btn_addtodo, gash_on_addtodo,
                            LV_EVENT_CLICKED, NULL);
    }
    if (gash_obj_ok(guider_ui.screen_gash_btn_again)) {
        lv_obj_add_event_cb(guider_ui.screen_gash_btn_again, gash_on_again,
                            LV_EVENT_CLICKED, NULL);
    }
}

void screen_gash_buttion_add(void)
{
    if (s_gash.phase == GASH_PHASE_RESULT) {
        gash_result_group_bind();
    } else {
        gash_aim_group_bind();
    }
}

void screen_gash_custom_init(void)
{
    if (!gash_scr_ok() || s_gash.leaving) {
        return;
    }
    lv_async_call_cancel(gash_leave_to_main_async, NULL);
    gash_stop_motion();
    s_gash.phase = GASH_PHASE_AIM;
    s_gash.grab_step = GASH_GRAB_EXTEND;
    s_gash.alive = true;
    gash_bind_events();
    gash_prepare_widgets();
    gash_aim_group_bind();
#ifdef ESP_PLATFORM
    ESP_LOGI("gash", "machine ready");
#endif
}

void gash_machine_poll(void)
{
    lv_obj_t *act;

    if (s_gash.leaving) {
        return;
    }
    act = lv_scr_act();
    if (act == NULL || act != guider_ui.screen_gash) {
        return;
    }
    if (!lv_obj_is_valid(act)) {
        return;
    }
    if (s_gash.alive && s_gash.bound_scr == act) {
        return;
    }
    screen_gash_custom_init();
}
