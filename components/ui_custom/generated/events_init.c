/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "events_init.h"
#include <stdio.h>
#include "lvgl.h"

#if LV_USE_GUIDER_SIMULATOR && LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif

#include "custom.h"
lv_obj_t *main_cont;

int tmp_review_num = 0;
bool is_week = 0;
static void loading_anim_done_cb(lv_anim_t * a)
{
    (void)a;
    /* 与 events_init 里其它页面切换一致 */
    ui_load_scr_animation(
        &guider_ui,
        &guider_ui.screen_getanswer,           /* 目标屏指针 */
        guider_ui.screen_getanswer_del,         /* 若目标未创建则先 setup */
        &guider_ui.screen_loading_del,          /* 当前 loading 的 del 标志 */
        setup_scr_screen_getanswer,            /* 目标屏 setup 函数名以你工程为准 */
        LV_SCR_LOAD_ANIM_FADE_ON,
        200, 200,
        true,   /* is_clean：是否清空旧屏内容，按你其它页面习惯 */
        true    /* auto_del：动画结束后删除旧 screen（你要的“删除现有 screen”） */
    );
}


static void screen_reppo_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        screen_reppo_custom_init();
        break;
    }
    default:
        break;
    }
}

void events_init_screen_reppo (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_reppo, screen_reppo_event_handler, LV_EVENT_ALL, ui);
}

static void screen_main_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        //buttion add
        screen_main_buttion_add();
        //show done sum and todo sum
        lv_label_set_text_fmt(guider_ui.screen_main_label_donenum, "%d", done_sum);
        lv_label_set_text_fmt(guider_ui.screen_main_label_todonum, "%d", todo_sum);

        // //container configure flex
        main_cont = guider_ui.screen_main_cont_main;
        lv_obj_set_style_clip_corner(main_cont, false, 0);
        lv_obj_set_flex_flow(main_cont, LV_FLEX_FLOW_ROW);
        lv_obj_set_scroll_dir(main_cont, LV_DIR_HOR);
        lv_obj_set_flex_align(main_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_scroll_snap_x(main_cont, LV_SCROLL_SNAP_CENTER);
        lv_obj_set_style_pad_column(main_cont, 45, 0); // 设置图标间距
        lv_obj_scroll_to_view(lv_obj_get_child(main_cont, 0), LV_ANIM_ON);

        lv_obj_add_flag(guider_ui.screen_main_img_select_timing, LV_OBJ_FLAG_HIDDEN);


        break;
    }
    default:
        break;
    }
}

static void screen_main_btn_todolist_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_todolist, guider_ui.screen_todolist_del, &guider_ui.screen_main_del, setup_scr_screen_todolist, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, true, true);
        break;
    }
    case LV_EVENT_FOCUSED:
    {
        lv_label_set_text(guider_ui.screen_main_label_chosen,"Todo");
        lv_obj_clear_flag(guider_ui.screen_main_img_todo_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_done_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_book_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_timer_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_ai_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_fun_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_setting_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_pos(guider_ui.screen_main_img_point_icon, 83, 155);
        ui_animation(guider_ui.screen_main_label_chosen, 0, 0, lv_obj_get_y(guider_ui.screen_main_label_chosen), -25, &lv_anim_path_linear, 1, 0, 200, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        lv_obj_set_style_translate_y(guider_ui.screen_main_btn_todolist, -8, LV_STATE_FOCUSED);
        ui_animation(guider_ui.screen_main_img_select, 100, 0, lv_obj_get_x(guider_ui.screen_main_img_select), 115, &lv_anim_path_linear, 0, 0, 100, 0, (lv_anim_exec_xcb_t)lv_obj_set_x, NULL, NULL, NULL);
        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_obj_set_y(guider_ui.screen_main_label_chosen, 10);
        lv_obj_set_x(guider_ui.screen_main_img_select, 132);
        break;
    }
    default:
        break;
    }
}

static void screen_main_btn_done_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_donelist, guider_ui.screen_donelist_del, &guider_ui.screen_main_del, setup_scr_screen_donelist, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, true, true);
        break;
    }
    case LV_EVENT_FOCUSED:
    {
        lv_label_set_text(guider_ui.screen_main_label_chosen,"Done");
        lv_obj_add_flag(guider_ui.screen_main_img_todo_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_main_img_done_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_book_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_timer_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_ai_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_fun_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_setting_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_pos(guider_ui.screen_main_img_point_icon, 108, 155);
        ui_animation(guider_ui.screen_main_label_chosen, 0, 0, lv_obj_get_y(guider_ui.screen_main_label_chosen), -25, &lv_anim_path_linear, 1, 0, 200, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        lv_obj_set_style_translate_y(guider_ui.screen_main_btn_done, -8, LV_STATE_FOCUSED);
        ui_animation(guider_ui.screen_main_img_select, 100, 0, lv_obj_get_x(guider_ui.screen_main_img_select), 115, &lv_anim_path_linear, 0, 0, 100, 0, (lv_anim_exec_xcb_t)lv_obj_set_x, NULL, NULL, NULL);

        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_obj_set_y(guider_ui.screen_main_label_chosen, 10);
        lv_obj_set_x(guider_ui.screen_main_img_select, 132);
        break;
    }
    default:
        break;
    }
}

static void screen_main_btn_word_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_wordmeun, guider_ui.screen_wordmeun_del, &guider_ui.screen_main_del, setup_scr_screen_wordmeun, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, true, true);
        break;
    }
    case LV_EVENT_FOCUSED:
    {
        lv_label_set_text(guider_ui.screen_main_label_chosen,"Word");
        lv_obj_add_flag(guider_ui.screen_main_img_todo_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_done_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_main_img_book_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_timer_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_ai_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_fun_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_setting_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_pos(guider_ui.screen_main_img_point_icon, 134, 155);
        ui_animation(guider_ui.screen_main_label_chosen, 0, 0, lv_obj_get_y(guider_ui.screen_main_label_chosen), -25, &lv_anim_path_linear, 1, 0, 200, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        lv_obj_set_style_translate_y(guider_ui.screen_main_btn_word, -8, LV_STATE_FOCUSED);
        ui_animation(guider_ui.screen_main_img_select, 100, 0, lv_obj_get_x(guider_ui.screen_main_img_select), 115, &lv_anim_path_linear, 0, 0, 100, 0, (lv_anim_exec_xcb_t)lv_obj_set_x, NULL, NULL, NULL);


        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_obj_set_y(guider_ui.screen_main_label_chosen, 10);
        lv_obj_set_x(guider_ui.screen_main_img_select, 132);
        break;
    }
    default:
        break;
    }
}

static void screen_main_btn_timer_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_pomodoro, guider_ui.screen_pomodoro_del, &guider_ui.screen_main_del, setup_scr_screen_pomodoro, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, true, true);
        break;
    }
    case LV_EVENT_FOCUSED:
    {
        lv_label_set_text(guider_ui.screen_main_label_chosen,"Pomodoro");
        lv_obj_add_flag(guider_ui.screen_main_img_todo_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_done_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_book_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_main_img_timer_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_ai_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_fun_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_setting_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_pos(guider_ui.screen_main_img_point_icon, 159, 155);
        ui_animation(guider_ui.screen_main_label_chosen, 0, 0, lv_obj_get_y(guider_ui.screen_main_label_chosen), -25, &lv_anim_path_linear, 1, 0, 200, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        lv_obj_set_style_translate_y(guider_ui.screen_main_btn_timer, -8, LV_STATE_FOCUSED);
        ui_animation(guider_ui.screen_main_img_select, 100, 0, lv_obj_get_x(guider_ui.screen_main_img_select), 115, &lv_anim_path_linear, 0, 0, 100, 0, (lv_anim_exec_xcb_t)lv_obj_set_x, NULL, NULL, NULL);
        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_obj_set_y(guider_ui.screen_main_label_chosen, 10);
        lv_obj_set_x(guider_ui.screen_main_img_select, 132);
        break;
    }
    default:
        break;
    }
}

static void screen_main_btn_ai_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_ai, guider_ui.screen_ai_del, &guider_ui.screen_main_del, setup_scr_screen_ai, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, true, true);
        break;
    }
    case LV_EVENT_FOCUSED:
    {
        lv_label_set_text(guider_ui.screen_main_label_chosen,"AI Summary");
        lv_obj_add_flag(guider_ui.screen_main_img_todo_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_done_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_book_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_timer_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_main_img_ai_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_fun_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_setting_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_pos(guider_ui.screen_main_img_point_icon, 186, 155);
        ui_animation(guider_ui.screen_main_label_chosen, 0, 0, lv_obj_get_y(guider_ui.screen_main_label_chosen), -25, &lv_anim_path_linear, 1, 0, 200, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        lv_obj_set_style_translate_y(guider_ui.screen_main_btn_ai, -8, LV_STATE_FOCUSED);
        ui_animation(guider_ui.screen_main_img_select, 100, 0, lv_obj_get_x(guider_ui.screen_main_img_select), 115, &lv_anim_path_linear, 0, 0, 100, 0, (lv_anim_exec_xcb_t)lv_obj_set_x, NULL, NULL, NULL);
        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_obj_set_y(guider_ui.screen_main_label_chosen, 10);
        lv_obj_set_x(guider_ui.screen_main_img_select, 132);
        break;
    }
    default:
        break;
    }
}

static void screen_main_btn_fun_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_answer, guider_ui.screen_answer_del, &guider_ui.screen_main_del, setup_scr_screen_answer, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, true, true);
        break;
    }
    case LV_EVENT_FOCUSED:
    {
        lv_label_set_text(guider_ui.screen_main_label_chosen,"Book of Answer");
        lv_obj_add_flag(guider_ui.screen_main_img_todo_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_done_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_book_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_timer_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_ai_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_main_img_fun_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_setting_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_pos(guider_ui.screen_main_img_point_icon,213, 155);
        ui_animation(guider_ui.screen_main_label_chosen, 0, 0, lv_obj_get_y(guider_ui.screen_main_label_chosen), -25, &lv_anim_path_linear, 1, 0, 200, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        lv_obj_set_style_translate_y(guider_ui.screen_main_btn_fun, -8, LV_STATE_FOCUSED);
        ui_animation(guider_ui.screen_main_img_select, 100, 0, lv_obj_get_x(guider_ui.screen_main_img_select), 115, &lv_anim_path_linear, 0, 0, 100, 0, (lv_anim_exec_xcb_t)lv_obj_set_x, NULL, NULL, NULL);
        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_obj_set_y(guider_ui.screen_main_label_chosen, 10);
        lv_obj_set_x(guider_ui.screen_main_img_select, 132);
        break;
    }
    default:
        break;
    }
}

static void screen_main_btn_setting_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_setting, guider_ui.screen_setting_del, &guider_ui.screen_main_del, setup_scr_screen_setting, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, true, true);
        break;
    }
    case LV_EVENT_FOCUSED:
    {
        lv_label_set_text(guider_ui.screen_main_label_chosen,"Setting");
        lv_obj_add_flag(guider_ui.screen_main_img_todo_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_done_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_book_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_timer_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_ai_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_fun_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_main_img_setting_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_pos(guider_ui.screen_main_img_point_icon, 238, 155);
        ui_animation(guider_ui.screen_main_label_chosen, 0, 0, lv_obj_get_y(guider_ui.screen_main_label_chosen), -25, &lv_anim_path_linear, 1, 0, 200, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        lv_obj_set_style_translate_y(guider_ui.screen_main_btn_setting, -8, LV_STATE_FOCUSED);
        ui_animation(guider_ui.screen_main_img_select, 100, 0, lv_obj_get_x(guider_ui.screen_main_img_select), 115, &lv_anim_path_linear, 0, 0, 100, 0, (lv_anim_exec_xcb_t)lv_obj_set_x, NULL, NULL, NULL);
        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_obj_set_y(guider_ui.screen_main_label_chosen, 10);
        lv_obj_set_x(guider_ui.screen_main_img_select, 132);
        break;
    }
    default:
        break;
    }
}

static void screen_main_btn_realtime_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_timing, guider_ui.screen_timing_del, &guider_ui.screen_main_del, setup_scr_screen_timing, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, true, true);
        break;
    }
    case LV_EVENT_FOCUSED:
    {
        lv_label_set_text(guider_ui.screen_main_label_chosen,"Real Time");
        lv_obj_add_flag(guider_ui.screen_main_img_todo_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_done_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_book_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_timer_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_ai_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_fun_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_setting_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_main_img_select_timing, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_select, LV_OBJ_FLAG_HIDDEN);
        ui_animation(guider_ui.screen_main_label_chosen, 0, 0, lv_obj_get_y(guider_ui.screen_main_label_chosen), -25, &lv_anim_path_linear, 1, 0, 200, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);


        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_obj_set_y(guider_ui.screen_main_label_chosen, 10);
        lv_obj_add_flag(guider_ui.screen_main_img_select_timing, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_main_img_select, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_main (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_main, screen_main_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_main_btn_todolist, screen_main_btn_todolist_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_main_btn_done, screen_main_btn_done_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_main_btn_word, screen_main_btn_word_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_main_btn_timer, screen_main_btn_timer_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_main_btn_ai, screen_main_btn_ai_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_main_btn_fun, screen_main_btn_fun_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_main_btn_setting, screen_main_btn_setting_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_main_btn_realtime, screen_main_btn_realtime_event_handler, LV_EVENT_ALL, ui);
}

static void screen_todolist_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        //screen_buttion add
        screen_todolist_buttion_add();
        //refresh done and todo num
        int total = done_sum + todo_sum;
        page_num = total / 4;
        if ((total % 4 != 0) || (total == 0)) {
            page_num++;
        }

        //if this page task < 4 , the empty one btn can not be clicked
        if(total < page_cur * 4) {
            int remain_num = 4 - (total % 4);
            if(remain_num == 1) {
                lv_obj_clear_state(guider_ui.screen_todolist_btn_todo1, LV_STATE_DISABLED);
                lv_obj_clear_state(guider_ui.screen_todolist_btn_todo2, LV_STATE_DISABLED);
                lv_obj_clear_state(guider_ui.screen_todolist_btn_todo3, LV_STATE_DISABLED);
                lv_obj_add_state(guider_ui.screen_todolist_btn_todo4, LV_STATE_DISABLED);
            } else if(remain_num == 2) {
                lv_obj_clear_state(guider_ui.screen_todolist_btn_todo1, LV_STATE_DISABLED);
                lv_obj_clear_state(guider_ui.screen_todolist_btn_todo2, LV_STATE_DISABLED);
                lv_obj_add_state(guider_ui.screen_todolist_btn_todo3, LV_STATE_DISABLED);
                lv_obj_add_state(guider_ui.screen_todolist_btn_todo4, LV_STATE_DISABLED);
            } else if(remain_num == 3) {
                lv_obj_clear_state(guider_ui.screen_todolist_btn_todo1, LV_STATE_DISABLED);
                lv_obj_add_state(guider_ui.screen_todolist_btn_todo2, LV_STATE_DISABLED);
                lv_obj_add_state(guider_ui.screen_todolist_btn_todo3, LV_STATE_DISABLED);
                lv_obj_add_state(guider_ui.screen_todolist_btn_todo4, LV_STATE_DISABLED);
            } else {
                lv_obj_clear_state(guider_ui.screen_todolist_btn_todo1, LV_STATE_DISABLED);
                lv_obj_clear_state(guider_ui.screen_todolist_btn_todo2, LV_STATE_DISABLED);
                lv_obj_clear_state(guider_ui.screen_todolist_btn_todo3, LV_STATE_DISABLED);
                lv_obj_clear_state(guider_ui.screen_todolist_btn_todo4, LV_STATE_DISABLED);
            }
        } else {
            lv_obj_clear_state(guider_ui.screen_todolist_btn_todo1, LV_STATE_DISABLED);
            lv_obj_clear_state(guider_ui.screen_todolist_btn_todo2, LV_STATE_DISABLED);
            lv_obj_clear_state(guider_ui.screen_todolist_btn_todo3, LV_STATE_DISABLED);
            lv_obj_clear_state(guider_ui.screen_todolist_btn_todo4, LV_STATE_DISABLED);
        }
        //focus state
        if(todosel_back) {
            todosel_back = 0;
            if(todo_index == 0) {
                lv_group_focus_obj(guider_ui.screen_todolist_btn_todo1);
            } else if(todo_index == 1) {
                lv_group_focus_obj(guider_ui.screen_todolist_btn_todo2);
            } else if(todo_index == 2) {
                lv_group_focus_obj(guider_ui.screen_todolist_btn_todo3);
            } else if(todo_index == 3) {
                lv_group_focus_obj(guider_ui.screen_todolist_btn_todo4);
            }
        } else {
            page_cur = 1;
        }
        //show for the first time when loading todolist ui
        lv_label_set_text_fmt(guider_ui.screen_todolist_label_donenum, "%d", done_sum);
        lv_label_set_text_fmt(guider_ui.screen_todolist_label_todonum, "%d", todo_sum);
        lv_label_set_text_fmt(guider_ui.screen_todolist_label_indexall, "%d", page_num);
        lv_label_set_text_fmt(guider_ui.screen_todolist_label_indexcur, "%d", page_cur);

        //here code to load 1-4 task info into label_todo1 - label_todo4
        lv_label_set_text(guider_ui.screen_todolist_btn_todo1_label,ui_todo_list.items[page_cur*4-4].name);
        lv_label_set_text(guider_ui.screen_todolist_btn_todo2_label,ui_todo_list.items[page_cur*4-3].name);
        lv_label_set_text(guider_ui.screen_todolist_btn_todo3_label,ui_todo_list.items[page_cur*4-2].name);
        lv_label_set_text(guider_ui.screen_todolist_btn_todo4_label,ui_todo_list.items[page_cur*4-1].name);
        //show tasks for their init state color
        if(ui_todo_list.items[(page_cur-1)*4 + 0].done == 1) {
            lv_obj_clear_flag(guider_ui.screen_todolist_line_1, LV_OBJ_FLAG_HIDDEN);
            ui_todo_list.items[(page_cur-1)*4 + 0].done = 1;
        } else {
            lv_obj_add_flag(guider_ui.screen_todolist_line_1, LV_OBJ_FLAG_HIDDEN);
            ui_todo_list.items[(page_cur-1)*4 + 0].done = 0;
        }
        if(ui_todo_list.items[(page_cur-1)*4 + 1].done == 1) {
            lv_obj_clear_flag(guider_ui.screen_todolist_line_2, LV_OBJ_FLAG_HIDDEN);
            ui_todo_list.items[(page_cur-1)*4 + 1].done = 1;
        } else {
            lv_obj_add_flag(guider_ui.screen_todolist_line_2, LV_OBJ_FLAG_HIDDEN);
            ui_todo_list.items[(page_cur-1)*4 + 1].done = 0;
        }
        if(ui_todo_list.items[(page_cur-1)*4 + 2].done == 1) {
            lv_obj_clear_flag(guider_ui.screen_todolist_line_3, LV_OBJ_FLAG_HIDDEN);
            ui_todo_list.items[(page_cur-1)*4 + 2].done = 1;
        } else {
            lv_obj_add_flag(guider_ui.screen_todolist_line_3, LV_OBJ_FLAG_HIDDEN);
            ui_todo_list.items[(page_cur-1)*4 + 2].done = 0;
        }
        if(ui_todo_list.items[(page_cur-1)*4 + 3].done == 1) {
            lv_obj_clear_flag(guider_ui.screen_todolist_line_4, LV_OBJ_FLAG_HIDDEN);
            ui_todo_list.items[(page_cur-1)*4 + 3].done = 1;
        } else {
            lv_obj_add_flag(guider_ui.screen_todolist_line_4, LV_OBJ_FLAG_HIDDEN);
            ui_todo_list.items[(page_cur-1)*4 + 3].done = 0;
        }
        break;
    }
    default:
        break;
    }
}

static void screen_todolist_btn_back_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_main, guider_ui.screen_main_del, &guider_ui.screen_todolist_del, setup_scr_screen_main, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, true, true);
        todo_back_label = 1;
        lv_obj_t * act_scr = lv_scr_act();
        lv_obj_clear_flag(act_scr, LV_OBJ_FLAG_CLICKABLE);
        break;
    }
    default:
        break;
    }
}

static void screen_todolist_btn_todo1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {

        todo_index = 0;
        if(ui_todo_list.items[(page_cur-1)*4 + todo_index].done == 0) {
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_todosel, guider_ui.screen_todosel_del, &guider_ui.screen_todolist_del, setup_scr_screen_todosel, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, false, false);
            break;
        } else {
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_todoback, guider_ui.screen_todoback_del, &guider_ui.screen_todolist_del, setup_scr_screen_todoback, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, false, false);
            break;
        }
        break;
    }
    case LV_EVENT_FOCUSED:
    {
        ui_animation(guider_ui.screen_todolist_btn_todo1, 0, 0, lv_obj_get_y(guider_ui.screen_todolist_btn_todo1), 40, &lv_anim_path_linear, 1, 0, 200, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_obj_set_y(guider_ui.screen_todolist_btn_todo1, 51);
        break;
    }
    default:
        break;
    }
}

static void screen_todolist_btn_todo2_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        todo_index = 1;
        if(ui_todo_list.items[(page_cur-1)*4 + todo_index].done == 0) {
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_todosel, guider_ui.screen_todosel_del, &guider_ui.screen_todolist_del, setup_scr_screen_todosel, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, false, false);
        } else {
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_todoback, guider_ui.screen_todoback_del, &guider_ui.screen_todolist_del, setup_scr_screen_todoback, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, false, false);
        }

        break;
    }
    case LV_EVENT_FOCUSED:
    {
        ui_animation(guider_ui.screen_todolist_btn_todo2, 0, 0, lv_obj_get_y(guider_ui.screen_todolist_btn_todo2), 40, &lv_anim_path_linear, 1, 0, 200, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_obj_set_y(guider_ui.screen_todolist_btn_todo2, 51);
        break;
    }
    default:
        break;
    }
}

static void screen_todolist_btn_todo3_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        todo_index = 2;
        if(ui_todo_list.items[(page_cur-1)*4 + todo_index].done == 0) {
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_todosel, guider_ui.screen_todosel_del, &guider_ui.screen_todolist_del, setup_scr_screen_todosel, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, false, false);
        } else {
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_todoback, guider_ui.screen_todoback_del, &guider_ui.screen_todolist_del, setup_scr_screen_todoback, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, false, false);
        }

        break;
    }
    case LV_EVENT_FOCUSED:
    {
        ui_animation(guider_ui.screen_todolist_btn_todo3, 0, 0, lv_obj_get_y(guider_ui.screen_todolist_btn_todo3), 93, &lv_anim_path_linear, 1, 0, 200, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_obj_set_y(guider_ui.screen_todolist_btn_todo3, 104);
        break;
    }
    default:
        break;
    }
}

static void screen_todolist_btn_todo4_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        todo_index = 3;
        if(ui_todo_list.items[(page_cur-1)*4 + todo_index].done == 0) {
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_todosel, guider_ui.screen_todosel_del, &guider_ui.screen_todolist_del, setup_scr_screen_todosel, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, false, false);
        } else {
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_todoback, guider_ui.screen_todoback_del, &guider_ui.screen_todolist_del, setup_scr_screen_todoback, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, false, false);
        }

        break;
    }
    case LV_EVENT_FOCUSED:
    {
        ui_animation(guider_ui.screen_todolist_btn_todo4, 0, 0, lv_obj_get_y(guider_ui.screen_todolist_btn_todo4), 93, &lv_anim_path_linear, 1, 0, 200, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_obj_set_y(guider_ui.screen_todolist_btn_todo4, 104);
        break;
    }
    default:
        break;
    }
}

static void screen_todolist_btn_lastpage_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        //refresh done and todo num
        if(page_cur > 1) {
            page_cur -= 1;
            int total = done_sum + todo_sum;
            page_num = total / 4;
            if ((total % 4 != 0) || (total == 0)) {
                page_num++;
            }

            //if this page task < 4 , the empty one btn can not be clicked
            if(total < page_cur * 4) {
                int remain_num = 4 - (total % 4);
                if(remain_num == 1) {
                    lv_obj_clear_state(guider_ui.screen_todolist_btn_todo1, LV_STATE_DISABLED);
                    lv_obj_clear_state(guider_ui.screen_todolist_btn_todo2, LV_STATE_DISABLED);
                    lv_obj_clear_state(guider_ui.screen_todolist_btn_todo3, LV_STATE_DISABLED);
                    lv_obj_add_state(guider_ui.screen_todolist_btn_todo4, LV_STATE_DISABLED);
                } else if(remain_num == 2) {
                    lv_obj_clear_state(guider_ui.screen_todolist_btn_todo1, LV_STATE_DISABLED);
                    lv_obj_clear_state(guider_ui.screen_todolist_btn_todo2, LV_STATE_DISABLED);
                    lv_obj_add_state(guider_ui.screen_todolist_btn_todo3, LV_STATE_DISABLED);
                    lv_obj_add_state(guider_ui.screen_todolist_btn_todo4, LV_STATE_DISABLED);
                } else if(remain_num == 3) {
                    lv_obj_clear_state(guider_ui.screen_todolist_btn_todo1, LV_STATE_DISABLED);
                    lv_obj_add_state(guider_ui.screen_todolist_btn_todo2, LV_STATE_DISABLED);
                    lv_obj_add_state(guider_ui.screen_todolist_btn_todo3, LV_STATE_DISABLED);
                    lv_obj_add_state(guider_ui.screen_todolist_btn_todo4, LV_STATE_DISABLED);
                } else {
                    lv_obj_clear_state(guider_ui.screen_todolist_btn_todo1, LV_STATE_DISABLED);
                    lv_obj_clear_state(guider_ui.screen_todolist_btn_todo2, LV_STATE_DISABLED);
                    lv_obj_clear_state(guider_ui.screen_todolist_btn_todo3, LV_STATE_DISABLED);
                    lv_obj_clear_state(guider_ui.screen_todolist_btn_todo4, LV_STATE_DISABLED);
                }
            } else {
                lv_obj_clear_state(guider_ui.screen_todolist_btn_todo1, LV_STATE_DISABLED);
                lv_obj_clear_state(guider_ui.screen_todolist_btn_todo2, LV_STATE_DISABLED);
                lv_obj_clear_state(guider_ui.screen_todolist_btn_todo3, LV_STATE_DISABLED);
                lv_obj_clear_state(guider_ui.screen_todolist_btn_todo4, LV_STATE_DISABLED);
            }
            //show for the first time when loading todolist ui
            lv_label_set_text_fmt(guider_ui.screen_todolist_label_donenum, "%d", done_sum);
            lv_label_set_text_fmt(guider_ui.screen_todolist_label_todonum, "%d", todo_sum);
            lv_label_set_text_fmt(guider_ui.screen_todolist_label_indexall, "%d", page_num);
            lv_label_set_text_fmt(guider_ui.screen_todolist_label_indexcur, "%d", page_cur);

            //here code to load 1-4 task info into label_todo1 - label_todo4
            lv_label_set_text(guider_ui.screen_todolist_btn_todo1_label,ui_todo_list.items[page_cur*4-4].name);
            lv_label_set_text(guider_ui.screen_todolist_btn_todo2_label,ui_todo_list.items[page_cur*4-3].name);
            lv_label_set_text(guider_ui.screen_todolist_btn_todo3_label,ui_todo_list.items[page_cur*4-2].name);
            lv_label_set_text(guider_ui.screen_todolist_btn_todo4_label,ui_todo_list.items[page_cur*4-1].name);
            //show tasks for their init state color
            if(ui_todo_list.items[(page_cur-1)*4 + 0].done == 1) {
                lv_obj_clear_flag(guider_ui.screen_todolist_line_1, LV_OBJ_FLAG_HIDDEN);
                ui_todo_list.items[(page_cur-1)*4 + 0].done = 1;
            } else {
                lv_obj_add_flag(guider_ui.screen_todolist_line_1, LV_OBJ_FLAG_HIDDEN);
                ui_todo_list.items[(page_cur-1)*4 + 0].done = 0;
            }
            if(ui_todo_list.items[(page_cur-1)*4 + 1].done == 1) {
                lv_obj_clear_flag(guider_ui.screen_todolist_line_2, LV_OBJ_FLAG_HIDDEN);
                ui_todo_list.items[(page_cur-1)*4 + 1].done = 1;
            } else {
                lv_obj_add_flag(guider_ui.screen_todolist_line_2, LV_OBJ_FLAG_HIDDEN);
                ui_todo_list.items[(page_cur-1)*4 + 1].done = 0;
            }
            if(ui_todo_list.items[(page_cur-1)*4 + 2].done == 1) {
                lv_obj_clear_flag(guider_ui.screen_todolist_line_3, LV_OBJ_FLAG_HIDDEN);
                ui_todo_list.items[(page_cur-1)*4 + 2].done = 1;
            } else {
                lv_obj_add_flag(guider_ui.screen_todolist_line_3, LV_OBJ_FLAG_HIDDEN);
                ui_todo_list.items[(page_cur-1)*4 + 2].done = 0;
            }
            if(ui_todo_list.items[(page_cur-1)*4 + 3].done == 1) {
                lv_obj_clear_flag(guider_ui.screen_todolist_line_4, LV_OBJ_FLAG_HIDDEN);
                ui_todo_list.items[(page_cur-1)*4 + 3].done = 1;
            } else {
                lv_obj_add_flag(guider_ui.screen_todolist_line_4, LV_OBJ_FLAG_HIDDEN);
                ui_todo_list.items[(page_cur-1)*4 + 3].done = 0;
            }
        }
        break;
    }
    default:
        break;
    }
}

static void screen_todolist_btn_nxtpage_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        //refresh done and todo num
        if(page_cur <  page_num && page_cur < 25) {
            page_cur += 1;
            int total = done_sum + todo_sum;
            page_num = total / 4;
            if ((total % 4 != 0) || (total == 0)) {
                page_num++;
            }

            //if this page task < 4 , the empty one btn can not be clicked
            if(total < page_cur * 4) {
                int remain_num = 4 - (total % 4);
                if(remain_num == 1) {
                    lv_obj_clear_state(guider_ui.screen_todolist_btn_todo1, LV_STATE_DISABLED);
                    lv_obj_clear_state(guider_ui.screen_todolist_btn_todo2, LV_STATE_DISABLED);
                    lv_obj_clear_state(guider_ui.screen_todolist_btn_todo3, LV_STATE_DISABLED);
                    lv_obj_add_state(guider_ui.screen_todolist_btn_todo4, LV_STATE_DISABLED);
                } else if(remain_num == 2) {
                    lv_obj_clear_state(guider_ui.screen_todolist_btn_todo1, LV_STATE_DISABLED);
                    lv_obj_clear_state(guider_ui.screen_todolist_btn_todo2, LV_STATE_DISABLED);
                    lv_obj_add_state(guider_ui.screen_todolist_btn_todo3, LV_STATE_DISABLED);
                    lv_obj_add_state(guider_ui.screen_todolist_btn_todo4, LV_STATE_DISABLED);
                } else if(remain_num == 3) {
                    lv_obj_clear_state(guider_ui.screen_todolist_btn_todo1, LV_STATE_DISABLED);
                    lv_obj_add_state(guider_ui.screen_todolist_btn_todo2, LV_STATE_DISABLED);
                    lv_obj_add_state(guider_ui.screen_todolist_btn_todo3, LV_STATE_DISABLED);
                    lv_obj_add_state(guider_ui.screen_todolist_btn_todo4, LV_STATE_DISABLED);
                } else {
                    lv_obj_clear_state(guider_ui.screen_todolist_btn_todo1, LV_STATE_DISABLED);
                    lv_obj_clear_state(guider_ui.screen_todolist_btn_todo2, LV_STATE_DISABLED);
                    lv_obj_clear_state(guider_ui.screen_todolist_btn_todo3, LV_STATE_DISABLED);
                    lv_obj_clear_state(guider_ui.screen_todolist_btn_todo4, LV_STATE_DISABLED);
                }
            } else {
                lv_obj_clear_state(guider_ui.screen_todolist_btn_todo1, LV_STATE_DISABLED);
                lv_obj_clear_state(guider_ui.screen_todolist_btn_todo2, LV_STATE_DISABLED);
                lv_obj_clear_state(guider_ui.screen_todolist_btn_todo3, LV_STATE_DISABLED);
                lv_obj_clear_state(guider_ui.screen_todolist_btn_todo4, LV_STATE_DISABLED);
            }
            //show for the first time when loading todolist ui
            lv_label_set_text_fmt(guider_ui.screen_todolist_label_donenum, "%d", done_sum);
            lv_label_set_text_fmt(guider_ui.screen_todolist_label_todonum, "%d", todo_sum);
            lv_label_set_text_fmt(guider_ui.screen_todolist_label_indexall, "%d", page_num);
            lv_label_set_text_fmt(guider_ui.screen_todolist_label_indexcur, "%d", page_cur);

            //here code to load 1-4 task info into label_todo1 - label_todo4
            lv_label_set_text(guider_ui.screen_todolist_btn_todo1_label,ui_todo_list.items[page_cur*4-4].name);
            lv_label_set_text(guider_ui.screen_todolist_btn_todo2_label,ui_todo_list.items[page_cur*4-3].name);
            lv_label_set_text(guider_ui.screen_todolist_btn_todo3_label,ui_todo_list.items[page_cur*4-2].name);
            lv_label_set_text(guider_ui.screen_todolist_btn_todo4_label,ui_todo_list.items[page_cur*4-1].name);
            //show tasks for their init state color
            if(ui_todo_list.items[(page_cur-1)*4 + 0].done == 1) {
                lv_obj_clear_flag(guider_ui.screen_todolist_line_1, LV_OBJ_FLAG_HIDDEN);
                ui_todo_list.items[(page_cur-1)*4 + 0].done = 1;
            } else {
                lv_obj_add_flag(guider_ui.screen_todolist_line_1, LV_OBJ_FLAG_HIDDEN);
                ui_todo_list.items[(page_cur-1)*4 + 0].done = 0;
            }
            if(ui_todo_list.items[(page_cur-1)*4 + 1].done == 1) {
                lv_obj_clear_flag(guider_ui.screen_todolist_line_2, LV_OBJ_FLAG_HIDDEN);
                ui_todo_list.items[(page_cur-1)*4 + 1].done = 1;
            } else {
                lv_obj_add_flag(guider_ui.screen_todolist_line_2, LV_OBJ_FLAG_HIDDEN);
                ui_todo_list.items[(page_cur-1)*4 + 1].done = 0;
            }
            if(ui_todo_list.items[(page_cur-1)*4 + 2].done == 1) {
                lv_obj_clear_flag(guider_ui.screen_todolist_line_3, LV_OBJ_FLAG_HIDDEN);
                ui_todo_list.items[(page_cur-1)*4 + 2].done = 1;
            } else {
                lv_obj_add_flag(guider_ui.screen_todolist_line_3, LV_OBJ_FLAG_HIDDEN);
                ui_todo_list.items[(page_cur-1)*4 + 2].done = 0;
            }
            if(ui_todo_list.items[(page_cur-1)*4 + 3].done == 1) {
                lv_obj_clear_flag(guider_ui.screen_todolist_line_4, LV_OBJ_FLAG_HIDDEN);
                ui_todo_list.items[(page_cur-1)*4 + 3].done = 1;
            } else {
                lv_obj_add_flag(guider_ui.screen_todolist_line_4, LV_OBJ_FLAG_HIDDEN);
                ui_todo_list.items[(page_cur-1)*4 + 3].done = 0;
            }
        }
        break;
    }
    default:
        break;
    }
}

void events_init_screen_todolist (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_todolist, screen_todolist_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_todolist_btn_back, screen_todolist_btn_back_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_todolist_btn_todo1, screen_todolist_btn_todo1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_todolist_btn_todo2, screen_todolist_btn_todo2_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_todolist_btn_todo3, screen_todolist_btn_todo3_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_todolist_btn_todo4, screen_todolist_btn_todo4_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_todolist_btn_lastpage, screen_todolist_btn_lastpage_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_todolist_btn_nxtpage, screen_todolist_btn_nxtpage_event_handler, LV_EVENT_ALL, ui);
}

static void screen_todosel_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        //buttion add
        screen_todosel_buttion_add();

        const char *text_ptr = NULL;
        if(todo_index == 0) {
            text_ptr = lv_label_get_text(guider_ui.screen_todolist_btn_todo1_label);
        } else if(todo_index == 1) {
            text_ptr = lv_label_get_text(guider_ui.screen_todolist_btn_todo2_label);
        } else if(todo_index == 2) {
            text_ptr = lv_label_get_text(guider_ui.screen_todolist_btn_todo3_label);
        } else if(todo_index == 3) {
            text_ptr = lv_label_get_text(guider_ui.screen_todolist_btn_todo4_label);
        }

        if(guider_ui.screen_todosel_label_task != NULL && text_ptr != NULL) {
            lv_label_set_text(guider_ui.screen_todosel_label_task, text_ptr);
        }
        break;
    }
    default:
        break;
    }
}

static void screen_todosel_btn_complete_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_todolist, guider_ui.screen_todolist_del, &guider_ui.screen_todosel_del, setup_scr_screen_todolist, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, true, true);
        //task done or not label change
        if(ui_todo_list.items[(page_cur-1)*4 + todo_index].done == 0) {
            ui_todo_list.items[(page_cur-1)*4 + todo_index].done = 1;
            done_sum += 1; //add 1
            todo_sum -= 1;
        } else {
            ui_todo_list.items[(page_cur-1)*4 + todo_index].done = 0;
            done_sum -= 1; //delete 1
            todo_sum += 1;
        }
        //show done sum and todo sum
        lv_label_set_text_fmt(guider_ui.screen_todolist_label_donenum, "%d", done_sum);
        lv_label_set_text_fmt(guider_ui.screen_todolist_label_todonum, "%d", todo_sum);
        //todosel_back label
        todosel_back = 1;
        break;
    }
    default:
        break;
    }
}

static void screen_todosel_btn_focus_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_pomodoro, guider_ui.screen_pomodoro_del, &guider_ui.screen_todosel_del, setup_scr_screen_pomodoro, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, true, true);
        //copy label to todosel menu
        const char *text_ptr = NULL;
        if(todo_index == 0) {
            text_ptr = lv_label_get_text(guider_ui.screen_todolist_btn_todo1_label);
        } else if(todo_index == 1) {
            text_ptr = lv_label_get_text(guider_ui.screen_todolist_btn_todo2_label);
        } else if(todo_index == 2) {
            text_ptr = lv_label_get_text(guider_ui.screen_todolist_btn_todo3_label);
        } else if(todo_index == 3) {
            text_ptr = lv_label_get_text(guider_ui.screen_todolist_btn_todo4_label);
        }
        // copy into current_task_name
        strncpy(current_task_name, text_ptr, sizeof(current_task_name) - 1);
        current_task_name[sizeof(current_task_name) - 1] = '\0'; // make sure string \0 ending
        todo_back_label = 1;
        focus_en = 1;
        focus_session_active = 1;
        current_task_index = (page_cur-1) * 4 + todo_index;
        lv_obj_clear_flag(guider_ui.screen_todolist, LV_OBJ_FLAG_CLICKABLE);
        break;
    }
    default:
        break;
    }
}

static void screen_todosel_btn_cancel_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_todolist, guider_ui.screen_todolist_del, &guider_ui.screen_todosel_del, setup_scr_screen_todolist, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, true, true);
        todosel_back = 1;
        break;
    }
    default:
        break;
    }
}

void events_init_screen_todosel (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_todosel, screen_todosel_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_todosel_btn_complete, screen_todosel_btn_complete_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_todosel_btn_focus, screen_todosel_btn_focus_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_todosel_btn_cancel, screen_todosel_btn_cancel_event_handler, LV_EVENT_ALL, ui);
}

static void screen_todoback_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        //buttion add
        screen_todoback_buttion_add();

        const char *text_ptr = NULL;
        if(todo_index == 0) {
            text_ptr = lv_label_get_text(guider_ui.screen_todolist_btn_todo1_label);
        } else if(todo_index == 1) {
            text_ptr = lv_label_get_text(guider_ui.screen_todolist_btn_todo2_label);
        } else if(todo_index == 2) {
            text_ptr = lv_label_get_text(guider_ui.screen_todolist_btn_todo3_label);
        } else if(todo_index == 3) {
            text_ptr = lv_label_get_text(guider_ui.screen_todolist_btn_todo4_label);
        }

        if(guider_ui.screen_todoback_label_name != NULL && text_ptr != NULL) {
            lv_label_set_text(guider_ui.screen_todoback_label_name, text_ptr);
        }
        break;
    }
    default:
        break;
    }
}

static void screen_todoback_btn_cancel_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_todolist, guider_ui.screen_todolist_del, &guider_ui.screen_todoback_del, setup_scr_screen_todolist, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, true, true);
        todosel_back = 1;
        break;
    }
    default:
        break;
    }
}

static void screen_todoback_btn_recover_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_todolist, guider_ui.screen_todolist_del, &guider_ui.screen_todoback_del, setup_scr_screen_todolist, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, true, true);
        //task done or not label change
        if(ui_todo_list.items[(page_cur-1)*4 + todo_index].done == 0) {
            ui_todo_list.items[(page_cur-1)*4 + todo_index].done = 1;
            done_sum += 1; //add 1
            todo_sum -= 1;
        } else {
            ui_todo_list.items[(page_cur-1)*4 + todo_index].done = 0;
            done_sum -= 1; //delete 1
            todo_sum += 1;
        }
        //todosel_back label
        todosel_back = 1;
        break;
    }
    default:
        break;
    }
}

void events_init_screen_todoback (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_todoback, screen_todoback_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_todoback_btn_cancel, screen_todoback_btn_cancel_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_todoback_btn_recover, screen_todoback_btn_recover_event_handler, LV_EVENT_ALL, ui);
}

static void screen_timing_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        // screen buttion add
        screen_timing_buttion_add();

        lv_obj_add_flag(guider_ui.screen_timing_img_wificonnect,LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_timing_img_wifidis,LV_OBJ_FLAG_HIDDEN);
        //---------//
        task_real_time_en = 1;
        break;
    }
    default:
        break;
    }
}

static void screen_timing_btn_main_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_main, guider_ui.screen_main_del, &guider_ui.screen_timing_del, setup_scr_screen_main, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, true, true);
        task_real_time_en = 0;
        timing_back_en = 1;
        lv_obj_t * act_scr = lv_scr_act();
        lv_obj_clear_flag(act_scr, LV_OBJ_FLAG_CLICKABLE);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_timing (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_timing, screen_timing_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_timing_btn_main, screen_timing_btn_main_event_handler, LV_EVENT_ALL, ui);
}

static void screen_wordmeun_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        //screen buttion add
        screen_wordmenu_buttion_add();
        //-----------------//
        int process_percent = (learned_cards_num*100) / total_cards_num;
        lv_label_set_text_fmt(guider_ui.screen_wordmeun_label_wordpercent,"%d%%",process_percent);
        lv_label_set_text_fmt(guider_ui.screen_wordmeun_label_reviewnum,"Review: %d",review_cards_num);
        lv_bar_set_value(guider_ui.screen_wordmeun_bar_process,process_percent,LV_ANIM_ON);
        break;
    }
    default:
        break;
    }
}

static void screen_wordmeun_btn_learn_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_wordcard, guider_ui.screen_wordcard_del, &guider_ui.screen_wordmeun_del, setup_scr_screen_wordcard, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, true, true);
        break;
    }
    default:
        break;
    }
}

static void screen_wordmeun_btn_review_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_reviewcard, guider_ui.screen_reviewcard_del, &guider_ui.screen_wordmeun_del, setup_scr_screen_reviewcard, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, true, true);
        break;
    }
    default:
        break;
    }
}

static void screen_wordmeun_btn_back_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_main, guider_ui.screen_main_del, &guider_ui.screen_wordmeun_del, setup_scr_screen_main, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, true, true);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_wordmeun (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_wordmeun, screen_wordmeun_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wordmeun_btn_learn, screen_wordmeun_btn_learn_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wordmeun_btn_review, screen_wordmeun_btn_review_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wordmeun_btn_back, screen_wordmeun_btn_back_event_handler, LV_EVENT_ALL, ui);
}

static void screen_wordcard_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        //screen button add
        screen_wordcard_buttion_add();
        //------------------------//
        cards_index = 1;
        lv_label_set_text_fmt(guider_ui.screen_wordcard_label_index,"%d/%d",cards_index,round_cards_num);


        break;
    }
    default:
        break;
    }
}

static void screen_wordcard_btn_dui_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {

        if(cards_index == round_cards_num) {
            led_green_en = 1; // flow light rgb
            cards_index += 1;
            wordcard_change = 1;
            lv_obj_add_state(guider_ui.screen_wordcard_btn_dui, LV_STATE_DISABLED);
            lv_obj_add_state(guider_ui.screen_wordcard_btn_cuo, LV_STATE_DISABLED);
            group_focus_obj(guider_ui.screen_wordcard_btn_back);
            lv_label_set_text_fmt(guider_ui.screen_wordcard_label_index,"%d/%d",round_cards_num,round_cards_num);
        } else if(cards_index < round_cards_num) {
            led_green_en = 1; // flow light rgb
            cards_index += 1;
            wordcard_change = 1;
            lv_label_set_text_fmt(guider_ui.screen_wordcard_label_index,"%d/%d",cards_index,round_cards_num);
        }
        break;
    }
    default:
        break;
    }
}

static void screen_wordcard_btn_cuo_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        led_red_en = 1; //flow light rgb
        review_cards_num += 1;
        cards_index += 1;
        lv_label_set_text_fmt(guider_ui.screen_wordcard_label_index,"%d/%d",cards_index,round_cards_num);
        wordcard_change = 1;
        if(cards_index >= round_cards_num) {
            lv_obj_add_state(guider_ui.screen_wordcard_btn_dui, LV_STATE_DISABLED);
            lv_obj_add_state(guider_ui.screen_wordcard_btn_cuo, LV_STATE_DISABLED);
            group_focus_obj(guider_ui.screen_wordcard_btn_back);
        }
        break;
    }
    default:
        break;
    }
}

static void screen_wordcard_btn_back_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_wordmeun, guider_ui.screen_wordmeun_del, &guider_ui.screen_wordcard_del, setup_scr_screen_wordmeun, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, true, true);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_wordcard (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_wordcard, screen_wordcard_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wordcard_btn_dui, screen_wordcard_btn_dui_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wordcard_btn_cuo, screen_wordcard_btn_cuo_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wordcard_btn_back, screen_wordcard_btn_back_event_handler, LV_EVENT_ALL, ui);
}

static void screen_reviewcard_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        //screen_wordcard
        screen_reviewcard_buttion_add();
        //--------------------//
        if(review_cards_num == 0) {
            lv_label_set_text(guider_ui.screen_reviewcard_label_index,"0/0");
            lv_obj_add_state(guider_ui.screen_reviewcard_btn_dui,LV_STATE_DISABLED);
            group_focus_obj(guider_ui.screen_reviewcard_btn_back);
            lv_label_set_text(guider_ui.screen_reviewcard_label_meaning,"No word need to review");
            lv_label_set_text(guider_ui.screen_reviewcard_label_phonetic,"");
            lv_label_set_text(guider_ui.screen_reviewcard_label_spelling,"Good Job!");
        } else {
            cards_index = 1;
            tmp_review_num = review_cards_num;
            lv_label_set_text_fmt(guider_ui.screen_reviewcard_label_index,"%d/%d",cards_index,tmp_review_num);
            reviewcard_change = 1;
        }

        break;
    }
    default:
        break;
    }
}

static void screen_reviewcard_btn_dui_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        led_green_en = 1;
        if(cards_index > tmp_review_num) {
            lv_obj_add_state(guider_ui.screen_reviewcard_btn_dui, LV_STATE_DISABLED);
            group_focus_obj(guider_ui.screen_reviewcard_btn_back);
        } else if(cards_index == tmp_review_num) {
            cards_index += 1;
            review_cards_num -= 1;
            lv_label_set_text_fmt(guider_ui.screen_reviewcard_label_index,"%d/%d",tmp_review_num,tmp_review_num);
            reviewcard_change = 1;
            lv_obj_add_state(guider_ui.screen_reviewcard_btn_dui, LV_STATE_DISABLED);
            group_focus_obj(guider_ui.screen_reviewcard_btn_back);
        } else {
            cards_index += 1;
            review_cards_num -= 1;
            lv_label_set_text_fmt(guider_ui.screen_reviewcard_label_index,"%d/%d",cards_index,tmp_review_num);
            reviewcard_change = 1;
        }
        break;
    }
    default:
        break;
    }
}

static void screen_reviewcard_btn_back_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_wordmeun, guider_ui.screen_wordmeun_del, &guider_ui.screen_reviewcard_del, setup_scr_screen_wordmeun, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, true, true);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_reviewcard (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_reviewcard, screen_reviewcard_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_reviewcard_btn_dui, screen_reviewcard_btn_dui_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_reviewcard_btn_back, screen_reviewcard_btn_back_event_handler, LV_EVENT_ALL, ui);
}

static void screen_pomodoro_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        //screen_buttion add//
        screen_pomodoro_buttion_add();
        //---------------------//
        pomodoro_countdown = 1;
        pomodoro_stopwatch = 0;
        timer_all_num = 25; //minutes
        timer_all_seconds = 0;
        pomodoro_timer = NULL;
        pomodoro_stopwatch = 0;
        countdown_start_label = 0;
        countdown_process_label = 0;
        stopwatch_start_label = 0;
        stopwatch_process_label = 0;
        lv_obj_add_state(guider_ui.screen_pomodoro_btn_complete, LV_STATE_DISABLED);
        lv_obj_add_state(guider_ui.screen_pomodoro_btn_continue, LV_STATE_DISABLED);
        lv_obj_add_flag(guider_ui.screen_pomodoro_img_timeout, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_pomodoro_img_stopwatch, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_pomodoro_img_countdown, LV_OBJ_FLAG_HIDDEN);
        //focus or not juge
        if(focus_en == 1) {
            if(guider_ui.screen_pomodoro_label_pomodoro != NULL) {
                lv_label_set_text(guider_ui.screen_pomodoro_label_pomodoro, current_task_name);
            }
            focus_en = 0;
        }
        break;
    }
    default:
        break;
    }
}

static void screen_pomodoro_btn_stopwatch_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        if(countdown_start_label == 0) {
            pomodoro_countdown = 0;
            pomodoro_stopwatch = 1;
            lv_obj_clear_flag(guider_ui.screen_pomodoro_img_stopwatch, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(guider_ui.screen_pomodoro_img_countdown, LV_OBJ_FLAG_HIDDEN);
            lv_label_set_text(guider_ui.screen_pomodoro_label_timer,"00:00:00");
            timer_all_num = 0;
        }

        break;
    }
    default:
        break;
    }
}

static void screen_pomodoro_btn_countdown_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        if(countdown_start_label == 0) {
            pomodoro_countdown = 1;
            pomodoro_stopwatch = 0;
            timer_all_num = 25;
            lv_obj_add_flag(guider_ui.screen_pomodoro_img_stopwatch, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(guider_ui.screen_pomodoro_img_countdown, LV_OBJ_FLAG_HIDDEN);
            lv_label_set_text(guider_ui.screen_pomodoro_label_timer,"00:25:00");
        }


        break;
    }
    default:
        break;
    }
}

static void screen_pomodoro_btn_back_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        focus_session_cancel();
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_main, guider_ui.screen_main_del, &guider_ui.screen_pomodoro_del, setup_scr_screen_main, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, true, true);
        if(pomodoro_timer != NULL) {
            lv_timer_del(pomodoro_timer);
            pomodoro_timer = NULL;
        }
        pomodoro_countdown = 1;
        pomodoro_stopwatch = 0;
        timer_all_num = 25; //minutes
        timer_all_seconds = 0;
        pomodoro_timer = NULL;
        pomodoro_stopwatch = 0;
        countdown_start_label = 0;
        countdown_process_label = 0;
        stopwatch_start_label = 0;
        stopwatch_process_label = 0;

        lv_obj_t * act_scr = lv_scr_act();
        lv_obj_clear_flag(act_scr, LV_OBJ_FLAG_CLICKABLE);
        break;
    }
    default:
        break;
    }
}

static void screen_pomodoro_btn_start_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        if(pomodoro_countdown == 1) {
            if(countdown_start_label == 0) {
                countdown_start(timer_all_num);
                countdown_start_label = 1;
                countdown_process_label = 1;
            } else if(countdown_process_label == 1) {
                countdown_process_label = 0;
                lv_timer_pause(pomodoro_timer);
            } else if(countdown_process_label == 0) {
                countdown_process_label = 1;
                lv_timer_resume(pomodoro_timer);
            }
        } else if(pomodoro_stopwatch == 1) {
            if(stopwatch_start_label == 0) {
                stopwatch_start();
                stopwatch_start_label = 1;
                stopwatch_process_label = 1;
            } else if(stopwatch_process_label == 1) {
                stopwatch_process_label = 0;
                lv_timer_pause(pomodoro_timer);
            } else if(stopwatch_process_label == 0) {
                stopwatch_process_label = 1;
                lv_timer_resume(pomodoro_timer);
            }
        }
        break;
    }
    default:
        break;
    }
}

static void screen_pomodoro_btn_reset_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        if(pomodoro_countdown == 1) {
            focus_session_cancel();
            if(pomodoro_timer != NULL) {
                lv_timer_del(pomodoro_timer);
                pomodoro_timer = NULL;
            }
            countdown_start_label = 0;
            countdown_process_label = 0;
            timer_all_num = 25;
            pomodoro_stopwatch = 0;
            lv_label_set_text(guider_ui.screen_pomodoro_label_timer,"00:25:00");
            lv_label_set_text(guider_ui.screen_pomodoro_btn_start_label,"Start");
        } else if(pomodoro_stopwatch == 1) {
            if(pomodoro_timer != NULL) {
                lv_timer_del(pomodoro_timer);
                pomodoro_timer = NULL;
            }
            stopwatch_start_label = 0;
            stopwatch_process_label = 0;
            timer_all_num = 0;
            pomodoro_countdown = 0;
            lv_label_set_text(guider_ui.screen_pomodoro_label_timer,"00:00:00");
            lv_label_set_text(guider_ui.screen_pomodoro_btn_start_label,"Start");
        }
        break;
    }
    default:
        break;
    }
}

static void screen_pomodoro_btn_addnum_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        const char* txt = lv_label_get_text(guider_ui.screen_pomodoro_btn_addnum_label);
        int timer_addnum = atoi(txt);
        if(timer_addnum == 1) {
            lv_label_set_text_fmt(guider_ui.screen_pomodoro_btn_addnum_label,"%d",5);
        } else if(timer_addnum == 5) {
            lv_label_set_text_fmt(guider_ui.screen_pomodoro_btn_addnum_label,"%d",10);
        } else if(timer_addnum == 10) {
            lv_label_set_text_fmt(guider_ui.screen_pomodoro_btn_addnum_label,"%d",1);
        }
        break;
    }
    default:
        break;
    }
}

static void screen_pomodoro_btn_complete_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        pomodoro_countdown = 1;
        pomodoro_stopwatch = 0;
        timer_all_num = 25; //minutes
        timer_all_seconds = 0;
        pomodoro_timer = NULL;
        pomodoro_stopwatch = 0;
        countdown_start_label = 0;
        countdown_process_label = 0;
        stopwatch_start_label = 0;
        stopwatch_process_label = 0;
        lv_label_set_text(guider_ui.screen_pomodoro_label_timer,"00:25:00");
        lv_obj_add_state(guider_ui.screen_pomodoro_btn_complete, LV_STATE_DISABLED);
        lv_obj_add_state(guider_ui.screen_pomodoro_btn_continue, LV_STATE_DISABLED);
        lv_obj_add_flag(guider_ui.screen_pomodoro_img_timeout, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_state(guider_ui.screen_pomodoro_btn_minus,LV_STATE_DISABLED);
        lv_obj_clear_state(guider_ui.screen_pomodoro_btn_plus,LV_STATE_DISABLED);
        lv_obj_clear_state(guider_ui.screen_pomodoro_btn_addnum,LV_STATE_DISABLED);
        lv_obj_clear_state(guider_ui.screen_pomodoro_btn_reset,LV_STATE_DISABLED);
        lv_obj_clear_state(guider_ui.screen_pomodoro_btn_start,LV_STATE_DISABLED);
        lv_obj_clear_state(guider_ui.screen_pomodoro_btn_back,LV_STATE_DISABLED);
        lv_obj_clear_state(guider_ui.screen_pomodoro_btn_countdown,LV_STATE_DISABLED);
        lv_obj_clear_state(guider_ui.screen_pomodoro_btn_stopwatch,LV_STATE_DISABLED);
        lv_obj_clear_flag(guider_ui.screen_pomodoro_btn_start,LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_pomodoro_btn_reset,LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_state(guider_ui.screen_pomodoro_btn_start, LV_STATE_FOCUSED);

        lv_label_set_text(guider_ui.screen_pomodoro_label_pomodoro,"Pomodoro Timer");

        break;
    }
    default:
        break;
    }
}

static void screen_pomodoro_btn_continue_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_label_set_text(guider_ui.screen_pomodoro_label_timer,"00:25:00");
        lv_obj_add_state(guider_ui.screen_pomodoro_btn_complete, LV_STATE_DISABLED);
        lv_obj_add_state(guider_ui.screen_pomodoro_btn_continue, LV_STATE_DISABLED);
        lv_obj_add_flag(guider_ui.screen_pomodoro_img_timeout, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_state(guider_ui.screen_pomodoro_btn_minus,LV_STATE_DISABLED);
        lv_obj_clear_state(guider_ui.screen_pomodoro_btn_plus,LV_STATE_DISABLED);
        lv_obj_clear_state(guider_ui.screen_pomodoro_btn_addnum,LV_STATE_DISABLED);
        lv_obj_clear_state(guider_ui.screen_pomodoro_btn_reset,LV_STATE_DISABLED);
        lv_obj_clear_state(guider_ui.screen_pomodoro_btn_start,LV_STATE_DISABLED);
        lv_obj_clear_state(guider_ui.screen_pomodoro_btn_back,LV_STATE_DISABLED);
        lv_obj_clear_state(guider_ui.screen_pomodoro_btn_countdown,LV_STATE_DISABLED);
        lv_obj_clear_state(guider_ui.screen_pomodoro_btn_stopwatch,LV_STATE_DISABLED);
        lv_obj_clear_flag(guider_ui.screen_pomodoro_btn_start,LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_pomodoro_btn_reset,LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_state(guider_ui.screen_pomodoro_btn_start, LV_STATE_FOCUSED);
        pomodoro_countdown = 1;
        pomodoro_stopwatch = 0;
        timer_all_num = 25; //minutes
        timer_all_seconds = 0;
        pomodoro_timer = NULL;
        pomodoro_stopwatch = 0;
        countdown_start_label = 0;
        countdown_process_label = 0;
        stopwatch_start_label = 0;
        stopwatch_process_label = 0;
        lv_obj_add_state(guider_ui.screen_pomodoro_btn_complete, LV_STATE_DISABLED);
        lv_obj_add_state(guider_ui.screen_pomodoro_btn_continue, LV_STATE_DISABLED);
        lv_obj_add_flag(guider_ui.screen_pomodoro_img_timeout, LV_OBJ_FLAG_HIDDEN);

        break;
    }
    default:
        break;
    }
}

static void screen_pomodoro_btn_plus_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        const char* txt = lv_label_get_text(guider_ui.screen_pomodoro_btn_addnum_label);
        int timer_addnum = atoi(txt);
        int timer_hours;
        int timer_minutes;
        if(pomodoro_countdown == 1 && timer_all_num + timer_addnum <= 1440 && !countdown_start_label && !stopwatch_start_label) {
            timer_all_num += timer_addnum;
            timer_hours = timer_all_num / 60;
            timer_minutes = timer_all_num - (timer_hours*60);
            lv_label_set_text_fmt(guider_ui.screen_pomodoro_label_timer, "%02d:%02d:00",timer_hours,timer_minutes);
        }


        break;
    }
    default:
        break;
    }
}

static void screen_pomodoro_btn_minus_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        const char* txt = lv_label_get_text(guider_ui.screen_pomodoro_btn_addnum_label);
        int timer_addnum = atoi(txt);
        int timer_hours;
        int timer_minutes;
        if(timer_all_num >= timer_addnum && pomodoro_countdown == 1 && !countdown_start_label && !stopwatch_start_label) {
            timer_all_num -= timer_addnum;
            timer_hours = timer_all_num / 60;
            timer_minutes = timer_all_num - (timer_hours*60);
            lv_label_set_text_fmt(guider_ui.screen_pomodoro_label_timer, "%02d:%02d:00",timer_hours,timer_minutes);
        }
        break;
    }
    default:
        break;
    }
}

void events_init_screen_pomodoro (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_pomodoro, screen_pomodoro_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_pomodoro_btn_stopwatch, screen_pomodoro_btn_stopwatch_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_pomodoro_btn_countdown, screen_pomodoro_btn_countdown_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_pomodoro_btn_back, screen_pomodoro_btn_back_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_pomodoro_btn_start, screen_pomodoro_btn_start_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_pomodoro_btn_reset, screen_pomodoro_btn_reset_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_pomodoro_btn_addnum, screen_pomodoro_btn_addnum_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_pomodoro_btn_complete, screen_pomodoro_btn_complete_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_pomodoro_btn_continue, screen_pomodoro_btn_continue_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_pomodoro_btn_plus, screen_pomodoro_btn_plus_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_pomodoro_btn_minus, screen_pomodoro_btn_minus_event_handler, LV_EVENT_ALL, ui);
}

static void screen_setting_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        //screen buttion add
        screen_setting_buttion_add();

        if(wifi_success) {
            lv_obj_clear_flag(guider_ui.screen_setting_img_wifi_open, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(guider_ui.screen_setting_img_wifi_close, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(guider_ui.screen_setting_img_wifi_open, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(guider_ui.screen_setting_img_wifi_close, LV_OBJ_FLAG_HIDDEN);
        }
        lv_obj_add_flag(guider_ui.screen_setting_img_volume_close, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_img_volume_open, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_img_ble, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_img_ble_dis, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_img_update, LV_OBJ_FLAG_HIDDEN);

        lv_obj_add_flag(guider_ui.screen_setting_label_txt1, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_label_txt2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_label_txt3, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_label_txt4, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_label_info1, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_label_info2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_label_info3, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_label_info4, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_label_about, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void screen_setting_btn_back_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_main, guider_ui.screen_main_del, &guider_ui.screen_setting_del, setup_scr_screen_main, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        lv_obj_t * act_scr = lv_scr_act();
        lv_obj_clear_flag(act_scr, LV_OBJ_FLAG_CLICKABLE);
        break;
    }
    default:
        break;
    }
}

static void screen_setting_btn_general_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_FOCUSED:
    {
        ui_animation(guider_ui.screen_setting_btn_general, 100, 0, lv_obj_get_y(guider_ui.screen_setting_btn_general), 130, &lv_anim_path_linear, 0, 0, 100, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);

        lv_obj_add_flag(guider_ui.screen_setting_img_wifi_open, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_img_wifi_close, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_img_volume_close, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_img_volume_open, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_img_ble, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_img_ble_dis, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_img_update, LV_OBJ_FLAG_HIDDEN);

        lv_obj_clear_flag(guider_ui.screen_setting_label_txt1, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_setting_label_txt2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_setting_label_txt3, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_setting_label_txt4, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_setting_label_info1, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_setting_label_info2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_setting_label_info3, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_setting_label_info4, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_setting_label_about, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_obj_set_y(guider_ui.screen_setting_btn_general, 135);
        lv_obj_add_flag(guider_ui.screen_setting_label_txt1, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_label_txt2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_label_txt3, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_label_txt4, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_label_info1, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_label_info2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_label_info3, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_label_info4, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_label_about, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void screen_setting_btn_update_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_FOCUSED:
    {
        lv_obj_add_flag(guider_ui.screen_setting_img_wifi_open, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_img_wifi_close, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_img_volume_close, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_img_volume_open, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_img_ble, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_img_ble_dis, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_setting_img_update, LV_OBJ_FLAG_HIDDEN);
        ui_animation(guider_ui.screen_setting_btn_update, 100, 0, lv_obj_get_y(guider_ui.screen_setting_btn_update), 105, &lv_anim_path_linear, 0, 0, 100, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_obj_set_y(guider_ui.screen_setting_btn_update, 110);
        break;
    }
    default:
        break;
    }
}

static void screen_setting_btn_volume_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_FOCUSED:
    {
        lv_obj_add_flag(guider_ui.screen_setting_img_wifi_open, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_img_wifi_close, LV_OBJ_FLAG_HIDDEN);
        if(volume_en) {
            lv_obj_add_flag(guider_ui.screen_setting_img_volume_close, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(guider_ui.screen_setting_img_volume_open, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_clear_flag(guider_ui.screen_setting_img_volume_close, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(guider_ui.screen_setting_img_volume_open, LV_OBJ_FLAG_HIDDEN);
        }
        lv_obj_add_flag(guider_ui.screen_setting_img_ble, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_img_ble_dis, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_img_update, LV_OBJ_FLAG_HIDDEN);
        ui_animation(guider_ui.screen_setting_btn_volume, 100, 0, lv_obj_get_y(guider_ui.screen_setting_btn_volume), 80, &lv_anim_path_linear, 0, 0, 100, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_obj_set_y(guider_ui.screen_setting_btn_volume, 85);
        break;
    }
    case LV_EVENT_CLICKED:
    {
        if(volume_en) {
            //volume close
            volume_en = 0;
            lv_obj_clear_flag(guider_ui.screen_setting_img_volume_close,LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(guider_ui.screen_setting_img_volume_open,LV_OBJ_FLAG_HIDDEN);
            volume_on_disabled();
        } else {
            //volume open
            volume_en = 1;
            lv_obj_add_flag(guider_ui.screen_setting_img_volume_close,LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(guider_ui.screen_setting_img_volume_open,LV_OBJ_FLAG_HIDDEN);
            volume_on_enabled();
        }
        break;
    }
    default:
        break;
    }
}

static void screen_setting_btn_ble_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_FOCUSED:
    {
        lv_obj_add_flag(guider_ui.screen_setting_img_wifi_open, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_img_wifi_close, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_img_volume_close, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_img_volume_open, LV_OBJ_FLAG_HIDDEN);
        if(ble_success) {
            lv_obj_clear_flag(guider_ui.screen_setting_img_ble, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(guider_ui.screen_setting_img_ble_dis, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(guider_ui.screen_setting_img_ble, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(guider_ui.screen_setting_img_ble_dis, LV_OBJ_FLAG_HIDDEN);
        }

        lv_obj_add_flag(guider_ui.screen_setting_img_update, LV_OBJ_FLAG_HIDDEN);
        ui_animation(guider_ui.screen_setting_btn_ble, 100, 0, lv_obj_get_y(guider_ui.screen_setting_btn_ble), 55, &lv_anim_path_linear, 0, 0, 100, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_obj_set_y(guider_ui.screen_setting_btn_ble, 60);
        break;
    }
    case LV_EVENT_CLICKED:
    {
        if (ble_success) {
            ble_disconnect_req = 1;
        } else if (!ble_enabled) {
            ble_toggle_on_req = 1;
        } else {
            ble_toggle_off_req = 1;
        }
        break;
    }
    default:
        break;
    }
}

static void screen_setting_btn_wifi_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_wificfg, guider_ui.screen_wificfg_del, &guider_ui.screen_setting_del, setup_scr_screen_wificfg, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, true, true);
        break;
    }
    case LV_EVENT_FOCUSED:
    {
        if(wifi_success) {
            lv_obj_clear_flag(guider_ui.screen_setting_img_wifi_open, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(guider_ui.screen_setting_img_wifi_close, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(guider_ui.screen_setting_img_wifi_open, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(guider_ui.screen_setting_img_wifi_close, LV_OBJ_FLAG_HIDDEN);
        }
        lv_obj_add_flag(guider_ui.screen_setting_img_volume_close, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_img_volume_open, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_img_ble, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_img_ble_dis, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_setting_img_update, LV_OBJ_FLAG_HIDDEN);
        ui_animation(guider_ui.screen_setting_btn_wifi, 100, 0, lv_obj_get_y(guider_ui.screen_setting_btn_wifi), 30, &lv_anim_path_linear, 0, 0, 99, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_obj_set_y(guider_ui.screen_setting_btn_wifi, 35);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_setting (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_setting, screen_setting_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_setting_btn_back, screen_setting_btn_back_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_setting_btn_general, screen_setting_btn_general_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_setting_btn_update, screen_setting_btn_update_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_setting_btn_volume, screen_setting_btn_volume_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_setting_btn_ble, screen_setting_btn_ble_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_setting_btn_wifi, screen_setting_btn_wifi_event_handler, LV_EVENT_ALL, ui);
}

static void screen_wificfg_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        //screen buttion add
        wifi_scan_en = 1;
        if(wifi_success) {
            lv_obj_add_flag(guider_ui.screen_wificfg_img_wififalse,LV_OBJ_FLAG_HIDDEN);
            lv_label_set_text(guider_ui.screen_wificfg_label_status, "Connection Status");
            lv_label_set_text(guider_ui.screen_wificfg_label_status2, "Connected");
            lv_obj_set_style_text_color(guider_ui.screen_wificfg_label_status2, lv_color_hex(0x274a6e), 0);
        }
        screen_wificfg_buttion_add();

        lv_obj_t * act_scr = lv_scr_act();
        lv_obj_clear_flag(act_scr, LV_OBJ_FLAG_CLICKABLE);
        break;
    }
    default:
        break;
    }
}

static void screen_wificfg_btn_back_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_setting, guider_ui.screen_setting_del, &guider_ui.screen_wificfg_del, setup_scr_screen_setting, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, true, true);
        break;
    }
    default:
        break;
    }
}

static void screen_wificfg_list_1_item0_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_animation(guider_ui.screen_wificfg_img_statusbg, 200, 0, lv_obj_get_height(guider_ui.screen_wificfg_img_statusbg), 80, &lv_anim_path_linear, 1, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_height, NULL, NULL, NULL);
        wificfg_on_list_item_clicked(guider_ui.screen_wificfg_list_1_item0);
        break;
    }
    default:
        break;
    }
}

static void screen_wificfg_list_1_item1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        wificfg_on_list_item_clicked(guider_ui.screen_wificfg_list_1_item1);
        ui_animation(guider_ui.screen_wificfg_img_statusbg, 200, 0, lv_obj_get_height(guider_ui.screen_wificfg_img_statusbg), 80, &lv_anim_path_linear, 1, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_height, NULL, NULL, NULL);
        break;
    }
    default:
        break;
    }
}

static void screen_wificfg_list_1_item2_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        wificfg_on_list_item_clicked(guider_ui.screen_wificfg_list_1_item2);
        ui_animation(guider_ui.screen_wificfg_img_statusbg, 200, 0, lv_obj_get_height(guider_ui.screen_wificfg_img_statusbg), 80, &lv_anim_path_linear, 1, 0, 0, -1, (lv_anim_exec_xcb_t)lv_obj_set_height, NULL, NULL, NULL);
        break;
    }
    default:
        break;
    }
}

static void screen_wificfg_list_1_item3_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        wificfg_on_list_item_clicked(guider_ui.screen_wificfg_list_1_item3);
        ui_animation(guider_ui.screen_wificfg_img_statusbg, 200, 0, lv_obj_get_height(guider_ui.screen_wificfg_img_statusbg), 80, &lv_anim_path_linear, 1, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_height, NULL, NULL, NULL);
        break;
    }
    default:
        break;
    }
}

static void screen_wificfg_list_1_item4_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        wificfg_on_list_item_clicked(guider_ui.screen_wificfg_list_1_item4);
        ui_animation(guider_ui.screen_wificfg_img_statusbg, 200, 0, lv_obj_get_height(guider_ui.screen_wificfg_img_statusbg), 80, &lv_anim_path_linear, 1, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_height, NULL, NULL, NULL);
        break;
    }
    default:
        break;
    }
}

static void screen_wificfg_btn_scan_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        wifi_scan_en = 1;

        break;
    }
    default:
        break;
    }
}

static void screen_wificfg_btn_disconnect_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_clear_flag(guider_ui.screen_wificfg_img_wififalse, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(guider_ui.screen_wificfg_label_status2, "Disconnected");
        lv_obj_set_style_text_color(guider_ui.screen_wificfg_label_status2, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_DEFAULT);
        wifi_disconnect_en = 1;
        break;
    }
    default:
        break;
    }
}

static void screen_wificfg_btn_cancel_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        if(wifi_success) {
            wifi_connect_show();
        } else {
            wifi_disconnect_show();
        }
        break;
    }
    default:
        break;
    }
}

static void screen_wificfg_btn_connect_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        wifi_connect_en = 1;
        wifi_success = 0;
        lv_obj_clear_flag(guider_ui.screen_wificfg_spinner_connect,LV_OBJ_FLAG_HIDDEN);

        break;
    }
    default:
        break;
    }
}

static void screen_wificfg_ta_password_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        keyboard_start_en = 1;
        lv_obj_t * ta = guider_ui.screen_wificfg_ta_password;
        lv_obj_t * kb = guider_ui.g_kb_top_layer;
        lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_size(kb, 320, 80);
        lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 0);
        lv_keyboard_set_textarea(kb, ta);
        lv_group_add_obj(g, kb);
        lv_group_focus_obj(kb);
        lv_refr_now(NULL);
        lv_group_set_editing(g, true);
        lv_obj_move_foreground(kb);
        lv_obj_add_event_cb(kb, my_kb_event_cb, LV_EVENT_ALL, NULL);
        break;
    }
    case LV_EVENT_FOCUSED:
    {
        lv_obj_t * kb = guider_ui.g_kb_top_layer;
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_wificfg (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_wificfg, screen_wificfg_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wificfg_btn_back, screen_wificfg_btn_back_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wificfg_list_1_item0, screen_wificfg_list_1_item0_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wificfg_list_1_item1, screen_wificfg_list_1_item1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wificfg_list_1_item2, screen_wificfg_list_1_item2_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wificfg_list_1_item3, screen_wificfg_list_1_item3_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wificfg_list_1_item4, screen_wificfg_list_1_item4_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wificfg_btn_scan, screen_wificfg_btn_scan_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wificfg_btn_disconnect, screen_wificfg_btn_disconnect_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wificfg_btn_cancel, screen_wificfg_btn_cancel_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wificfg_btn_connect, screen_wificfg_btn_connect_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wificfg_ta_password, screen_wificfg_ta_password_event_handler, LV_EVENT_ALL, ui);
}

static void screen_ai_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        screen_ai_buttion_add();
        is_week = 0;
        screen_ai_load_nvs_data(is_week);
        int focus_level = 1;
        int remain_focus_time = 0;
        int max_level_time = 0;
        for(int i = 0; i < 10; i++) {
            if(all_focus_time < focus_level_hour[i]*60) {
                focus_level = i + 1;
                max_focus_time = focus_level_hour[i]*60;
                if(i == 0) {
                    remain_focus_time = all_focus_time;
                    max_level_time = focus_level_hour[i]*60;
                } else {
                    remain_focus_time = all_focus_time - focus_level_hour[i-1]*60;
                    max_level_time = focus_level_hour[i]*60 - focus_level_hour[i-1]*60;
                }
                break;
            }
        }

        int focus_percent = remain_focus_time / max_level_time;
        lv_bar_set_value(guider_ui.screen_ai_bar_focus,focus_percent,LV_ANIM_ON);
        lv_label_set_text_fmt(guider_ui.screen_ai_label_duration,"%d/%d",all_focus_time,max_focus_time);
        lv_obj_clear_flag(guider_ui.screen_ai_img_v1,LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_ai_img_v2,LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_ai_img_v3,LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_ai_img_v4,LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_ai_img_v5,LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_ai_img_v6,LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_ai_img_v7,LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_ai_img_v8,LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_ai_img_v9,LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_ai_img_v10,LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

static void screen_ai_btn_week_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ai_week_start = 1;
        is_week = 1;
        screen_ai_load_nvs_data(is_week);
        break;
    }
    default:
        break;
    }
}

static void screen_ai_btn_day_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ai_day_start = 1;
        is_week = 0;
        screen_ai_load_nvs_data(is_week);
        break;
    }
    default:
        break;
    }
}

static void screen_ai_btn_back_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_main, guider_ui.screen_main_del, &guider_ui.screen_ai_del, setup_scr_screen_main, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, true, true);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_ai (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_ai, screen_ai_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_ai_btn_week, screen_ai_btn_week_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_ai_btn_day, screen_ai_btn_day_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_ai_btn_back, screen_ai_btn_back_event_handler, LV_EVENT_ALL, ui);
}

static void screen_donelist_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        //screen_buttion add
        screen_donelist_buttion_add();

        donelist_load_from_csv();
        done_total_tasks = ui_done_list.count;
        donelist_page_cur = 1;
        donelist_page_num = done_total_tasks / 4;
        if ((done_total_tasks % 4 != 0) || (done_total_tasks == 0)) {
            donelist_page_num += 1;
        }

        //if this page task < 4 , the empty one btn can not be clicked
        if(done_total_tasks < donelist_page_cur * 4) {
            int remain_num = 4 - (done_total_tasks % 4);
            if(remain_num == 1) {
                lv_obj_clear_flag(guider_ui.screen_donelist_line_1, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(guider_ui.screen_donelist_line_2, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(guider_ui.screen_donelist_line_3, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(guider_ui.screen_donelist_line_4, LV_OBJ_FLAG_HIDDEN);
            } else if(remain_num == 2) {
                lv_obj_clear_flag(guider_ui.screen_donelist_line_1, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(guider_ui.screen_donelist_line_2, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(guider_ui.screen_donelist_line_3, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(guider_ui.screen_donelist_line_4, LV_OBJ_FLAG_HIDDEN);
            } else if(remain_num == 3) {
                lv_obj_clear_flag(guider_ui.screen_donelist_line_1, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(guider_ui.screen_donelist_line_2, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(guider_ui.screen_donelist_line_3, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(guider_ui.screen_donelist_line_4, LV_OBJ_FLAG_HIDDEN);
            } else if(done_total_tasks == 0) {
                lv_obj_add_flag(guider_ui.screen_donelist_line_1, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(guider_ui.screen_donelist_line_2, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(guider_ui.screen_donelist_line_3, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(guider_ui.screen_donelist_line_4, LV_OBJ_FLAG_HIDDEN);
            } else {
                lv_obj_clear_flag(guider_ui.screen_donelist_line_1, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(guider_ui.screen_donelist_line_2, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(guider_ui.screen_donelist_line_3, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(guider_ui.screen_donelist_line_4, LV_OBJ_FLAG_HIDDEN);
            }
        } else {
            lv_obj_clear_flag(guider_ui.screen_donelist_line_1, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(guider_ui.screen_donelist_line_2, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(guider_ui.screen_donelist_line_3, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(guider_ui.screen_donelist_line_4, LV_OBJ_FLAG_HIDDEN);
        }
        //show for the first time when loading todolist ui
        lv_label_set_text_fmt(guider_ui.screen_donelist_label_donenum, "%d", done_total_tasks);
        lv_label_set_text_fmt(guider_ui.screen_donelist_label_nosavenum, "%d", done_sum);
        lv_label_set_text_fmt(guider_ui.screen_donelist_label_page, "%d/%d", donelist_page_cur,donelist_page_num);
        donelist_render_page(donelist_page_cur);
        break;
    }
    default:
        break;
    }
}

static void screen_donelist_btn_save_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        todo_archive_label = 1;
        donelist_load_label = 0;
        //the task_done.csv will change
        //reload screen_donelist ui info
        //pagecur start to 1
        donelist_page_cur = 1;
        done_total_tasks += done_sum;
        // donelist_page_num = done_total_tasks / 4;
        // if ((done_total_tasks % 4 != 0) || (done_total_tasks == 0)) {
        //     donelist_page_num += 1;
        // }

        //if this page task < 4 , the empty one btn can not be clicked
        if(done_total_tasks < donelist_page_cur * 4) {
            int remain_num = 4 - (done_total_tasks % 4);
            if(remain_num == 1) {
                lv_obj_clear_flag(guider_ui.screen_donelist_line_1, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(guider_ui.screen_donelist_line_2, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(guider_ui.screen_donelist_line_3, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(guider_ui.screen_donelist_line_4, LV_OBJ_FLAG_HIDDEN);
            } else if(remain_num == 2) {
                lv_obj_clear_flag(guider_ui.screen_donelist_line_1, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(guider_ui.screen_donelist_line_2, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(guider_ui.screen_donelist_line_3, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(guider_ui.screen_donelist_line_4, LV_OBJ_FLAG_HIDDEN);
            } else if(remain_num == 3) {
                lv_obj_clear_flag(guider_ui.screen_donelist_line_1, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(guider_ui.screen_donelist_line_2, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(guider_ui.screen_donelist_line_3, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(guider_ui.screen_donelist_line_4, LV_OBJ_FLAG_HIDDEN);
            } else if(done_total_tasks == 0) {
                lv_obj_add_flag(guider_ui.screen_donelist_line_1, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(guider_ui.screen_donelist_line_2, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(guider_ui.screen_donelist_line_3, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(guider_ui.screen_donelist_line_4, LV_OBJ_FLAG_HIDDEN);
            } else {
                lv_obj_clear_flag(guider_ui.screen_donelist_line_1, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(guider_ui.screen_donelist_line_2, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(guider_ui.screen_donelist_line_3, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(guider_ui.screen_donelist_line_4, LV_OBJ_FLAG_HIDDEN);
            }
        } else {
            lv_obj_clear_flag(guider_ui.screen_donelist_line_1, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(guider_ui.screen_donelist_line_2, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(guider_ui.screen_donelist_line_3, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(guider_ui.screen_donelist_line_4, LV_OBJ_FLAG_HIDDEN);
        }

        //show for the first time when loading todolist ui
        // lv_label_set_text_fmt(guider_ui.screen_donelist_label_donenum, "%d", done_total_tasks);
        // lv_label_set_text_fmt(guider_ui.screen_donelist_label_nosavenum, "%d", done_sum);
        // lv_label_set_text_fmt(guider_ui.screen_donelist_label_page, "%d/%d", donelist_page_cur,donelist_page_num);
        break;
    }
    default:
        break;
    }
}

static void screen_donelist_btn_nxtpage_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        if(donelist_page_cur < donelist_page_num) {
            donelist_page_cur += 1;

            lv_label_set_text_fmt(guider_ui.screen_donelist_label_page,"%d/%d",donelist_page_cur,donelist_page_num);
            //if this page task < 4 , the empty one btn can not be clicked
            if(done_total_tasks < donelist_page_cur * 4) {
                int remain_num = 4 - (done_total_tasks % 4);
                if(remain_num == 1) {
                    lv_obj_clear_flag(guider_ui.screen_donelist_line_1, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(guider_ui.screen_donelist_line_2, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(guider_ui.screen_donelist_line_3, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_add_flag(guider_ui.screen_donelist_line_4, LV_OBJ_FLAG_HIDDEN);
                } else if(remain_num == 2) {
                    lv_obj_clear_flag(guider_ui.screen_donelist_line_1, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(guider_ui.screen_donelist_line_2, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_add_flag(guider_ui.screen_donelist_line_3, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_add_flag(guider_ui.screen_donelist_line_4, LV_OBJ_FLAG_HIDDEN);
                } else if(remain_num == 3) {
                    lv_obj_clear_flag(guider_ui.screen_donelist_line_1, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_add_flag(guider_ui.screen_donelist_line_2, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_add_flag(guider_ui.screen_donelist_line_3, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_add_flag(guider_ui.screen_donelist_line_4, LV_OBJ_FLAG_HIDDEN);
                } else if(done_total_tasks == 0) {
                    lv_obj_add_flag(guider_ui.screen_donelist_line_1, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_add_flag(guider_ui.screen_donelist_line_2, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_add_flag(guider_ui.screen_donelist_line_3, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_add_flag(guider_ui.screen_donelist_line_4, LV_OBJ_FLAG_HIDDEN);
                } else {
                    lv_obj_clear_flag(guider_ui.screen_donelist_line_1, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(guider_ui.screen_donelist_line_2, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(guider_ui.screen_donelist_line_3, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(guider_ui.screen_donelist_line_4, LV_OBJ_FLAG_HIDDEN);
                }
            } else {
                lv_obj_clear_flag(guider_ui.screen_donelist_line_1, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(guider_ui.screen_donelist_line_2, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(guider_ui.screen_donelist_line_3, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(guider_ui.screen_donelist_line_4, LV_OBJ_FLAG_HIDDEN);
            }

            donelist_render_page(donelist_page_cur);
        }
        break;
    }
    default:
        break;
    }
}

static void screen_donelist_btn_lastpage_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        if(donelist_page_cur > 1) {
            donelist_page_cur -= 1;
            lv_label_set_text_fmt(guider_ui.screen_donelist_label_page,"%d/%d",donelist_page_cur,donelist_page_num);
            //if this page task < 4 , the empty one btn can not be clicked
            if(done_total_tasks < donelist_page_cur * 4) {
                int remain_num = 4 - (done_total_tasks % 4);
                if(remain_num == 1) {
                    lv_obj_clear_flag(guider_ui.screen_donelist_line_1, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(guider_ui.screen_donelist_line_2, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(guider_ui.screen_donelist_line_3, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_add_flag(guider_ui.screen_donelist_line_4, LV_OBJ_FLAG_HIDDEN);
                } else if(remain_num == 2) {
                    lv_obj_clear_flag(guider_ui.screen_donelist_line_1, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(guider_ui.screen_donelist_line_2, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_add_flag(guider_ui.screen_donelist_line_3, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_add_flag(guider_ui.screen_donelist_line_4, LV_OBJ_FLAG_HIDDEN);
                } else if(remain_num == 3) {
                    lv_obj_clear_flag(guider_ui.screen_donelist_line_1, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_add_flag(guider_ui.screen_donelist_line_2, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_add_flag(guider_ui.screen_donelist_line_3, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_add_flag(guider_ui.screen_donelist_line_4, LV_OBJ_FLAG_HIDDEN);
                } else if(done_total_tasks == 0) {
                    lv_obj_add_flag(guider_ui.screen_donelist_line_1, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_add_flag(guider_ui.screen_donelist_line_2, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_add_flag(guider_ui.screen_donelist_line_3, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_add_flag(guider_ui.screen_donelist_line_4, LV_OBJ_FLAG_HIDDEN);
                } else {
                    lv_obj_clear_flag(guider_ui.screen_donelist_line_1, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(guider_ui.screen_donelist_line_2, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(guider_ui.screen_donelist_line_3, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(guider_ui.screen_donelist_line_4, LV_OBJ_FLAG_HIDDEN);
                }
            } else {
                lv_obj_clear_flag(guider_ui.screen_donelist_line_1, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(guider_ui.screen_donelist_line_2, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(guider_ui.screen_donelist_line_3, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(guider_ui.screen_donelist_line_4, LV_OBJ_FLAG_HIDDEN);
            }


            donelist_render_page(donelist_page_cur);
        }
        break;
    }
    default:
        break;
    }
}

static void screen_donelist_btn_back_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_main, guider_ui.screen_main_del, &guider_ui.screen_donelist_del, setup_scr_screen_main, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, true, true);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_donelist (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_donelist, screen_donelist_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_donelist_btn_save, screen_donelist_btn_save_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_donelist_btn_nxtpage, screen_donelist_btn_nxtpage_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_donelist_btn_lastpage, screen_donelist_btn_lastpage_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_donelist_btn_back, screen_donelist_btn_back_event_handler, LV_EVENT_ALL, ui);
}

static void screen_answer_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        screen_answer_buttion_add();
        break;
    }
    default:
        break;
    }
}

static void screen_answer_btn_start_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_loading, guider_ui.screen_loading_del, &guider_ui.screen_answer_del, setup_scr_screen_loading, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, true, true);
        answer_book_rand_req =1;
        break;
    }
    default:
        break;
    }
}

void events_init_screen_answer (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_answer, screen_answer_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_answer_btn_start, screen_answer_btn_start_event_handler, LV_EVENT_ALL, ui);
}

static void screen_loading_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        lv_img_set_pivot(guider_ui.screen_loading_img_icon, 29, 29);
        ui_animation(guider_ui.screen_loading_img_icon, 3000, 0, lv_img_get_angle(guider_ui.screen_loading_img_icon), 360*10, &lv_anim_path_linear, 1, 0, 0, 0, (lv_anim_exec_xcb_t)lv_img_set_angle, NULL, loading_anim_done_cb, NULL);

        break;
    }
    default:
        break;
    }
}

void events_init_screen_loading (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_loading, screen_loading_event_handler, LV_EVENT_ALL, ui);
}

static void screen_getanswer_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        screen_getanswer_buttion_add();
        answer_book_show_random_answer();
        break;
    }
    default:
        break;
    }
}

static void screen_getanswer_btn_answer_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_main, guider_ui.screen_main_del, &guider_ui.screen_getanswer_del, setup_scr_screen_main, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, true, true);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_getanswer (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_getanswer, screen_getanswer_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_getanswer_btn_answer, screen_getanswer_btn_answer_event_handler, LV_EVENT_ALL, ui);
}


void events_init(lv_ui *ui)
{

}
