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

int8_t wordbook_totalnum = 3;
int review_solve_num = 0;
lv_obj_t* menu_cont;
bool is_week = 0;

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
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_todolist, guider_ui.screen_todolist_del, &guider_ui.screen_main_del, setup_scr_screen_todolist, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    case LV_EVENT_FOCUSED:
    {
        lv_label_set_text(guider_ui.screen_main_label_chosen,"TODO");
        lv_obj_clear_flag(guider_ui.screen_main_img_todo_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_done_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_book_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_timer_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_ai_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_fun_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_setting_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_pos(guider_ui.screen_main_img_point_icon, 83, 155);
        ui_animation(guider_ui.screen_main_label_chosen, 0, 0, lv_obj_get_y(guider_ui.screen_main_label_chosen), -25, &lv_anim_path_linear, 1, 0, 200, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        lv_obj_set_style_translate_y(guider_ui.screen_main_btn_todolist, -5, LV_STATE_FOCUSED);
        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_obj_set_y(guider_ui.screen_main_label_chosen, 10);
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
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_donelist, guider_ui.screen_donelist_del, &guider_ui.screen_main_del, setup_scr_screen_donelist, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    case LV_EVENT_FOCUSED:
    {
        lv_label_set_text(guider_ui.screen_main_label_chosen,"DONE");
        lv_obj_add_flag(guider_ui.screen_main_img_todo_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_main_img_done_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_book_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_timer_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_ai_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_fun_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_setting_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_pos(guider_ui.screen_main_img_point_icon, 108, 155);
        ui_animation(guider_ui.screen_main_label_chosen, 0, 0, lv_obj_get_y(guider_ui.screen_main_label_chosen), -25, &lv_anim_path_linear, 1, 0, 200, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        lv_obj_set_style_translate_y(guider_ui.screen_main_btn_done, -5, LV_STATE_FOCUSED);

        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_obj_set_y(guider_ui.screen_main_label_chosen, 10);
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
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_wordmeun, guider_ui.screen_wordmeun_del, &guider_ui.screen_main_del, setup_scr_screen_wordmeun, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    case LV_EVENT_FOCUSED:
    {
        lv_label_set_text(guider_ui.screen_main_label_chosen,"WORD");
        lv_obj_add_flag(guider_ui.screen_main_img_todo_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_done_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_main_img_book_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_timer_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_ai_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_fun_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_setting_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_pos(guider_ui.screen_main_img_point_icon, 134, 155);
        ui_animation(guider_ui.screen_main_label_chosen, 0, 0, lv_obj_get_y(guider_ui.screen_main_label_chosen), -25, &lv_anim_path_linear, 1, 0, 200, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        lv_obj_set_style_translate_y(guider_ui.screen_main_btn_word, -5, LV_STATE_FOCUSED);


        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_obj_set_y(guider_ui.screen_main_label_chosen, 10);
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
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_pomodoro, guider_ui.screen_pomodoro_del, &guider_ui.screen_main_del, setup_scr_screen_pomodoro, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    case LV_EVENT_FOCUSED:
    {
        lv_label_set_text(guider_ui.screen_main_label_chosen,"POMODORO");
        lv_obj_add_flag(guider_ui.screen_main_img_todo_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_done_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_book_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_main_img_timer_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_ai_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_fun_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_setting_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_pos(guider_ui.screen_main_img_point_icon, 159, 155);
        ui_animation(guider_ui.screen_main_label_chosen, 0, 0, lv_obj_get_y(guider_ui.screen_main_label_chosen), -25, &lv_anim_path_linear, 1, 0, 200, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        lv_obj_set_style_translate_y(guider_ui.screen_main_btn_timer, -5, LV_STATE_FOCUSED);

        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_obj_set_y(guider_ui.screen_main_label_chosen, 10);
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
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_ai, guider_ui.screen_ai_del, &guider_ui.screen_main_del, setup_scr_screen_ai, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    case LV_EVENT_FOCUSED:
    {
        lv_label_set_text(guider_ui.screen_main_label_chosen,"AI SUMMARY");
        lv_obj_add_flag(guider_ui.screen_main_img_todo_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_done_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_book_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_timer_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_main_img_ai_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_fun_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_setting_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_pos(guider_ui.screen_main_img_point_icon, 186, 155);
        ui_animation(guider_ui.screen_main_label_chosen, 0, 0, lv_obj_get_y(guider_ui.screen_main_label_chosen), -25, &lv_anim_path_linear, 1, 0, 200, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        lv_obj_set_style_translate_y(guider_ui.screen_main_btn_ai, -5, LV_STATE_FOCUSED);

        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_obj_set_y(guider_ui.screen_main_label_chosen, 10);
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
        break;
    }
    case LV_EVENT_FOCUSED:
    {
        lv_label_set_text(guider_ui.screen_main_label_chosen,"FUN");
        lv_obj_add_flag(guider_ui.screen_main_img_todo_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_done_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_book_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_timer_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_ai_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_main_img_fun_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_setting_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_pos(guider_ui.screen_main_img_point_icon,213, 155);
        ui_animation(guider_ui.screen_main_label_chosen, 0, 0, lv_obj_get_y(guider_ui.screen_main_label_chosen), -25, &lv_anim_path_linear, 1, 0, 200, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        lv_obj_set_style_translate_y(guider_ui.screen_main_btn_fun, -5, LV_STATE_FOCUSED);

        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_obj_set_y(guider_ui.screen_main_label_chosen, 10);
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
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_setting, guider_ui.screen_setting_del, &guider_ui.screen_main_del, setup_scr_screen_setting, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    case LV_EVENT_FOCUSED:
    {
        lv_label_set_text(guider_ui.screen_main_label_chosen,"SETTING");
        lv_obj_add_flag(guider_ui.screen_main_img_todo_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_done_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_book_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_timer_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_ai_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_main_img_fun_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_main_img_setting_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_pos(guider_ui.screen_main_img_point_icon, 238, 155);
        ui_animation(guider_ui.screen_main_label_chosen, 0, 0, lv_obj_get_y(guider_ui.screen_main_label_chosen), -25, &lv_anim_path_linear, 1, 0, 200, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        lv_obj_set_style_translate_y(guider_ui.screen_main_btn_setting, -5, LV_STATE_FOCUSED);

        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_obj_set_y(guider_ui.screen_main_label_chosen, 10);
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
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_timing, guider_ui.screen_timing_del, &guider_ui.screen_main_del, setup_scr_screen_timing, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
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
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_main, guider_ui.screen_main_del, &guider_ui.screen_todolist_del, setup_scr_screen_main, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
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
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_todosel, guider_ui.screen_todosel_del, &guider_ui.screen_todolist_del, setup_scr_screen_todosel, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, false, false);
        break;
    }
    case LV_EVENT_FOCUSED:
    {
        ui_animation(guider_ui.screen_todolist_btn_todo1, 0, 0, lv_obj_get_y(guider_ui.screen_todolist_btn_todo1), 45, &lv_anim_path_linear, 1, 0, 200, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
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
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_todosel, guider_ui.screen_todosel_del, &guider_ui.screen_todolist_del, setup_scr_screen_todosel, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, false, false);
        break;
    }
    case LV_EVENT_FOCUSED:
    {
        ui_animation(guider_ui.screen_todolist_btn_todo2, 0, 0, lv_obj_get_y(guider_ui.screen_todolist_btn_todo2), 45, &lv_anim_path_linear, 1, 0, 200, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
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
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_todosel, guider_ui.screen_todosel_del, &guider_ui.screen_todolist_del, setup_scr_screen_todosel, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, false, false);
        break;
    }
    case LV_EVENT_FOCUSED:
    {
        ui_animation(guider_ui.screen_todolist_btn_todo3, 0, 0, lv_obj_get_y(guider_ui.screen_todolist_btn_todo3), 98, &lv_anim_path_linear, 1, 0, 200, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
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
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_todosel, guider_ui.screen_todosel_del, &guider_ui.screen_todolist_del, setup_scr_screen_todosel, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, false, false);
        todo_index = 3;
        break;
    }
    case LV_EVENT_FOCUSED:
    {
        ui_animation(guider_ui.screen_todolist_btn_todo4, 0, 0, lv_obj_get_y(guider_ui.screen_todolist_btn_todo4), 98, &lv_anim_path_linear, 1, 0, 200, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
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

static void screen_timing_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        // screen buttion add
        screen_timing_buttion_add();

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
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_main, guider_ui.screen_main_del, &guider_ui.screen_timing_del, setup_scr_screen_main, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
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
        int process_percent = (book_info[wordbook_index].word_learned_num*100) / book_info[wordbook_index].word_total_num;
        lv_label_set_text(guider_ui.screen_wordmeun_label_bookname,book_info[wordbook_index].book_name);
        lv_label_set_text_fmt(guider_ui.screen_wordmeun_label_divdernum,"%d/%d words (%d%%)",book_info[wordbook_index].word_learned_num,book_info[wordbook_index].word_total_num,process_percent);
        lv_label_set_text_fmt(guider_ui.screen_wordmeun_label_wordindicate,"Learn: %d Review: %d",book_info[wordbook_index].word_learned_num,book_info[wordbook_index].word_review_num);
        lv_bar_set_value(guider_ui.screen_wordmeun_bar_process,process_percent,LV_ANIM_ON);
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
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_main, guider_ui.screen_main_del, &guider_ui.screen_wordmeun_del, setup_scr_screen_main, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        lv_obj_t * act_scr = lv_scr_act();
        lv_obj_clear_flag(act_scr, LV_OBJ_FLAG_CLICKABLE);
        break;
    }
    default:
        break;
    }
}

static void screen_wordmeun_btn_bookleft_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        if(wordbook_index >= 1) {
            wordbook_index -= 1;
            int16_t process_percent = (book_info[wordbook_index].word_learned_num* 100) / book_info[wordbook_index].word_total_num;
            lv_label_set_text(guider_ui.screen_wordmeun_label_bookname,book_info[wordbook_index].book_name);
            lv_label_set_text_fmt(guider_ui.screen_wordmeun_label_divdernum,"%d/%d words (%d%%)",book_info[wordbook_index].word_learned_num,book_info[wordbook_index].word_total_num,process_percent);
            lv_label_set_text_fmt(guider_ui.screen_wordmeun_label_wordindicate,"Learn: %d Review: %d",book_info[wordbook_index].word_learned_num,book_info[wordbook_index].word_review_num);
            lv_bar_set_value(guider_ui.screen_wordmeun_bar_process,process_percent,LV_ANIM_ON);
        }
        break;
    }
    default:
        break;
    }
}

static void screen_wordmeun_btn_bookright_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        if(wordbook_index < wordbook_totalnum-1) {
            wordbook_index += 1;
            int16_t process_percent = (book_info[wordbook_index].word_learned_num* 100) / book_info[wordbook_index].word_total_num;
            lv_label_set_text(guider_ui.screen_wordmeun_label_bookname,book_info[wordbook_index].book_name);
            lv_label_set_text_fmt(guider_ui.screen_wordmeun_label_divdernum,"%d/%d words (%d%%)",book_info[wordbook_index].word_learned_num,book_info[wordbook_index].word_total_num,process_percent);
            lv_label_set_text_fmt(guider_ui.screen_wordmeun_label_wordindicate,"Learn: %d Review: %d",book_info[wordbook_index].word_learned_num,book_info[wordbook_index].word_review_num);
            lv_bar_set_value(guider_ui.screen_wordmeun_bar_process,process_percent,LV_ANIM_ON);
        }

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
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_wordcard, guider_ui.screen_wordcard_del, &guider_ui.screen_wordmeun_del, setup_scr_screen_wordcard, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
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
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_reviewcard, guider_ui.screen_reviewcard_del, &guider_ui.screen_wordmeun_del, setup_scr_screen_reviewcard, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_wordmeun (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_wordmeun, screen_wordmeun_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wordmeun_btn_back, screen_wordmeun_btn_back_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wordmeun_btn_bookleft, screen_wordmeun_btn_bookleft_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wordmeun_btn_bookright, screen_wordmeun_btn_bookright_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wordmeun_btn_learn, screen_wordmeun_btn_learn_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wordmeun_btn_review, screen_wordmeun_btn_review_event_handler, LV_EVENT_ALL, ui);
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
            lv_label_set_text(guider_ui.screen_pomodoro_label_funcstate,"StopWatch");
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
            lv_label_set_text(guider_ui.screen_pomodoro_label_funcstate,"CountDown");
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
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_main, guider_ui.screen_main_del, &guider_ui.screen_pomodoro_del, setup_scr_screen_main, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
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
                lv_label_set_text(guider_ui.screen_pomodoro_btn_start_label,"Pause");
            } else if(countdown_process_label == 1) {
                countdown_process_label = 0;
                lv_timer_pause(pomodoro_timer);
                lv_label_set_text(guider_ui.screen_pomodoro_btn_start_label,"Start");
            } else if(countdown_process_label == 0) {
                countdown_process_label = 1;
                lv_timer_resume(pomodoro_timer);
                lv_label_set_text(guider_ui.screen_pomodoro_btn_start_label,"Pause");
            }
        } else if(pomodoro_stopwatch == 1) {
            if(stopwatch_start_label == 0) {
                stopwatch_start();
                stopwatch_start_label = 1;
                stopwatch_process_label = 1;
                lv_label_set_text(guider_ui.screen_pomodoro_btn_start_label,"Pause");
            } else if(stopwatch_process_label == 1) {
                stopwatch_process_label = 0;
                lv_timer_pause(pomodoro_timer);
                lv_label_set_text(guider_ui.screen_pomodoro_btn_start_label,"Start");
            } else if(stopwatch_process_label == 0) {
                stopwatch_process_label = 1;
                lv_timer_resume(pomodoro_timer);
                lv_label_set_text(guider_ui.screen_pomodoro_btn_start_label,"Pause");
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
        const char* txt = lv_label_get_text(guider_ui.screen_pomodoro_label_addnum);
        int timer_addnum = atoi(txt);
        if(timer_addnum == 1) {
            lv_label_set_text_fmt(guider_ui.screen_pomodoro_label_addnum,"%d",5);
        } else if(timer_addnum == 5) {
            lv_label_set_text_fmt(guider_ui.screen_pomodoro_label_addnum,"%d",10);
        } else if(timer_addnum == 10) {
            lv_label_set_text_fmt(guider_ui.screen_pomodoro_label_addnum,"%d",1);
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
        const char* txt = lv_label_get_text(guider_ui.screen_pomodoro_label_addnum);
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

static void screen_pomodoro_btn_plus_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        const char* txt = lv_label_get_text(guider_ui.screen_pomodoro_label_addnum);
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

        //task done
        ui_todo_list.items[current_task_index].done = 1;
        done_sum += 1; //add 1
        todo_sum -= 1;
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

void events_init_screen_pomodoro (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_pomodoro, screen_pomodoro_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_pomodoro_btn_stopwatch, screen_pomodoro_btn_stopwatch_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_pomodoro_btn_countdown, screen_pomodoro_btn_countdown_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_pomodoro_btn_back, screen_pomodoro_btn_back_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_pomodoro_btn_start, screen_pomodoro_btn_start_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_pomodoro_btn_reset, screen_pomodoro_btn_reset_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_pomodoro_btn_addnum, screen_pomodoro_btn_addnum_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_pomodoro_btn_minus, screen_pomodoro_btn_minus_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_pomodoro_btn_plus, screen_pomodoro_btn_plus_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_pomodoro_btn_complete, screen_pomodoro_btn_complete_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_pomodoro_btn_continue, screen_pomodoro_btn_continue_event_handler, LV_EVENT_ALL, ui);
}

static void screen_wordcard_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        //screen_wordcard
        screen_wordcard_buttion_add();
        //--------------------//
        lv_label_set_text_fmt(guider_ui.screen_wordcard_label_index,"%d/%d",book_info[wordbook_index].word_index + 1,book_info[wordbook_index].word_total_num);
        if(book_info[wordbook_index].word_state[book_info[wordbook_index].word_index] == 0) {
            lv_obj_set_style_text_color(guider_ui.screen_wordcard_label_spelling, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
            lv_obj_clear_state(guider_ui.screen_wordcard_btn_dui, LV_STATE_DISABLED);
            lv_obj_clear_state(guider_ui.screen_wordcard_btn_cuo, LV_STATE_DISABLED);
        } else if(book_info[wordbook_index].word_state[book_info[wordbook_index].word_index] == 1) {
            lv_obj_set_style_text_color(guider_ui.screen_wordcard_label_spelling, lv_color_hex(0xdaffcb), LV_PART_MAIN|LV_STATE_DEFAULT);
            lv_obj_add_state(guider_ui.screen_wordcard_btn_dui, LV_STATE_DISABLED);
            lv_obj_clear_state(guider_ui.screen_wordcard_btn_cuo, LV_STATE_DISABLED);
        } else if(book_info[wordbook_index].word_state[book_info[wordbook_index].word_index] == 2) {
            lv_obj_set_style_text_color(guider_ui.screen_wordcard_label_spelling, lv_color_hex(0xefcbd1), LV_PART_MAIN|LV_STATE_DEFAULT);
            lv_obj_add_state(guider_ui.screen_wordcard_btn_dui, LV_STATE_DISABLED);
            lv_obj_add_state(guider_ui.screen_wordcard_btn_cuo, LV_STATE_DISABLED);
        }
        wordcard_change = 1;

        lv_obj_t * act_scr = lv_scr_act();
        lv_obj_clear_flag(act_scr, LV_OBJ_FLAG_CLICKABLE);
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
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_wordmeun, guider_ui.screen_wordmeun_del, &guider_ui.screen_wordcard_del, setup_scr_screen_wordmeun, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

static void screen_wordcard_btn_left_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        if(book_info[wordbook_index].word_index >= 1) {
            book_info[wordbook_index].word_index -= 1;
            lv_label_set_text_fmt(guider_ui.screen_wordcard_label_index,"%d/%d",book_info[wordbook_index].word_index + 1,book_info[wordbook_index].word_total_num);
            if(book_info[wordbook_index].word_state[book_info[wordbook_index].word_index] == 0) {
                lv_obj_set_style_text_color(guider_ui.screen_wordcard_label_spelling, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
                lv_obj_clear_state(guider_ui.screen_wordcard_btn_dui, LV_STATE_DISABLED);
                lv_obj_clear_state(guider_ui.screen_wordcard_btn_cuo, LV_STATE_DISABLED);
            } else if(book_info[wordbook_index].word_state[book_info[wordbook_index].word_index] == 1) {
                lv_obj_set_style_text_color(guider_ui.screen_wordcard_label_spelling, lv_color_hex(0xdaffcb), LV_PART_MAIN|LV_STATE_DEFAULT);
                lv_obj_add_state(guider_ui.screen_wordcard_btn_dui, LV_STATE_DISABLED);
                lv_obj_clear_state(guider_ui.screen_wordcard_btn_cuo, LV_STATE_DISABLED);
            } else if(book_info[wordbook_index].word_state[book_info[wordbook_index].word_index] == 2) {
                lv_obj_set_style_text_color(guider_ui.screen_wordcard_label_spelling, lv_color_hex(0xefcbd1), LV_PART_MAIN|LV_STATE_DEFAULT);
                lv_obj_add_state(guider_ui.screen_wordcard_btn_dui, LV_STATE_DISABLED);
                lv_obj_add_state(guider_ui.screen_wordcard_btn_cuo, LV_STATE_DISABLED);
            }
            wordcard_change = 1;
        }
        break;
    }
    default:
        break;
    }
}

static void screen_wordcard_btn_right_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        if(book_info[wordbook_index].word_index < book_info[wordbook_index].word_total_num - 1) {
            book_info[wordbook_index].word_index += 1;
            lv_label_set_text_fmt(guider_ui.screen_wordcard_label_index,"%d/%d",book_info[wordbook_index].word_index + 1,book_info[wordbook_index].word_total_num);
            if(book_info[wordbook_index].word_state[book_info[wordbook_index].word_index] == 0) {
                lv_obj_set_style_text_color(guider_ui.screen_wordcard_label_spelling, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
                lv_obj_clear_state(guider_ui.screen_wordcard_btn_dui, LV_STATE_DISABLED);
                lv_obj_clear_state(guider_ui.screen_wordcard_btn_cuo, LV_STATE_DISABLED);
            } else if(book_info[wordbook_index].word_state[book_info[wordbook_index].word_index] == 1) {
                lv_obj_set_style_text_color(guider_ui.screen_wordcard_label_spelling, lv_color_hex(0xdaffcb), LV_PART_MAIN|LV_STATE_DEFAULT);
                lv_obj_add_state(guider_ui.screen_wordcard_btn_dui, LV_STATE_DISABLED);
                lv_obj_clear_state(guider_ui.screen_wordcard_btn_cuo, LV_STATE_DISABLED);
            } else if(book_info[wordbook_index].word_state[book_info[wordbook_index].word_index] == 2) {
                lv_obj_set_style_text_color(guider_ui.screen_wordcard_label_spelling, lv_color_hex(0xefcbd1), LV_PART_MAIN|LV_STATE_DEFAULT);
                lv_obj_add_state(guider_ui.screen_wordcard_btn_dui, LV_STATE_DISABLED);
                lv_obj_add_state(guider_ui.screen_wordcard_btn_cuo, LV_STATE_DISABLED);
            }
            wordcard_change = 1;
        }
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
        led_green_en = 1;
        int word_index_cur = book_info[wordbook_index].word_index;
        if(book_info[wordbook_index].word_state[word_index_cur] == 0) {
            book_info[wordbook_index].word_learned_num += 1;
            book_info[wordbook_index].word_state[word_index_cur] = 1;
        } else if(book_info[wordbook_index].word_state[word_index_cur] == 2) {
            book_info[wordbook_index].word_learned_num += 1;
            book_info[wordbook_index].word_review_num  -= 1;
            book_info[wordbook_index].word_state[word_index_cur] = 1;
        }
        if(book_info[wordbook_index].word_index < book_info[wordbook_index].word_total_num - 1) {
            book_info[wordbook_index].word_index += 1;
            lv_label_set_text_fmt(guider_ui.screen_wordcard_label_index,"%d/%d",book_info[wordbook_index].word_index + 1,book_info[wordbook_index].word_total_num);
            wordcard_change = 1;
        }

        if(book_info[wordbook_index].word_state[book_info[wordbook_index].word_index] == 0) {
            lv_obj_set_style_text_color(guider_ui.screen_wordcard_label_spelling, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
            lv_obj_clear_state(guider_ui.screen_wordcard_btn_dui, LV_STATE_DISABLED);
            lv_obj_clear_state(guider_ui.screen_wordcard_btn_cuo, LV_STATE_DISABLED);
        } else if(book_info[wordbook_index].word_state[book_info[wordbook_index].word_index] == 1) {
            lv_obj_set_style_text_color(guider_ui.screen_wordcard_label_spelling, lv_color_hex(0xdaffcb), LV_PART_MAIN|LV_STATE_DEFAULT);
            lv_obj_add_state(guider_ui.screen_wordcard_btn_dui, LV_STATE_DISABLED);
            lv_obj_clear_state(guider_ui.screen_wordcard_btn_cuo, LV_STATE_DISABLED);
        } else if(book_info[wordbook_index].word_state[book_info[wordbook_index].word_index] == 2) {
            lv_obj_set_style_text_color(guider_ui.screen_wordcard_label_spelling, lv_color_hex(0xefcbd1), LV_PART_MAIN|LV_STATE_DEFAULT);
            lv_obj_add_state(guider_ui.screen_wordcard_btn_dui, LV_STATE_DISABLED);
            lv_obj_add_state(guider_ui.screen_wordcard_btn_cuo, LV_STATE_DISABLED);
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
        led_red_en = 1;
        int word_index_cur = book_info[wordbook_index].word_index;
        int review_num_cur = book_info[wordbook_index].word_review_num;

        if(book_info[wordbook_index].word_state[word_index_cur] == 0) {
            book_info[wordbook_index].review_absoulte_index[review_num_cur] = word_index_cur;
            book_info[wordbook_index].word_review_num += 1;
            book_info[wordbook_index].word_state[word_index_cur] = 2;
        } else if(book_info[wordbook_index].word_state[word_index_cur] == 1) {
            book_info[wordbook_index].review_absoulte_index[review_num_cur] = word_index_cur;
            book_info[wordbook_index].word_review_num += 1;
            book_info[wordbook_index].word_learned_num -= 1;
            book_info[wordbook_index].word_state[word_index_cur] = 2;
        }
        //word_index add and show next word info to label
        if(book_info[wordbook_index].word_index < book_info[wordbook_index].word_total_num - 1) {
            book_info[wordbook_index].word_index += 1;
            lv_label_set_text_fmt(guider_ui.screen_wordcard_label_index,"%d/%d",book_info[wordbook_index].word_index + 1,book_info[wordbook_index].word_total_num);
            wordcard_change = 1;
        }

        if(book_info[wordbook_index].word_state[book_info[wordbook_index].word_index] == 0) {
            lv_obj_set_style_text_color(guider_ui.screen_wordcard_label_spelling, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
            lv_obj_clear_state(guider_ui.screen_wordcard_btn_dui, LV_STATE_DISABLED);
            lv_obj_clear_state(guider_ui.screen_wordcard_btn_cuo, LV_STATE_DISABLED);
        } else if(book_info[wordbook_index].word_state[book_info[wordbook_index].word_index] == 1) {
            lv_obj_set_style_text_color(guider_ui.screen_wordcard_label_spelling, lv_color_hex(0xdaffcb), LV_PART_MAIN|LV_STATE_DEFAULT);
            lv_obj_add_state(guider_ui.screen_wordcard_btn_dui, LV_STATE_DISABLED);
            lv_obj_clear_state(guider_ui.screen_wordcard_btn_cuo, LV_STATE_DISABLED);
        } else if(book_info[wordbook_index].word_state[book_info[wordbook_index].word_index] == 2) {
            lv_obj_set_style_text_color(guider_ui.screen_wordcard_label_spelling, lv_color_hex(0xefcbd1), LV_PART_MAIN|LV_STATE_DEFAULT);
            lv_obj_add_state(guider_ui.screen_wordcard_btn_dui, LV_STATE_DISABLED);
            lv_obj_add_state(guider_ui.screen_wordcard_btn_cuo, LV_STATE_DISABLED);
        }
        break;
    }
    default:
        break;
    }
}

static void screen_wordcard_btn_home_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_main, guider_ui.screen_main_del, &guider_ui.screen_wordcard_del, setup_scr_screen_main, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        lv_obj_t * act_scr = lv_scr_act();
        lv_obj_clear_flag(act_scr, LV_OBJ_FLAG_CLICKABLE);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_wordcard (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_wordcard, screen_wordcard_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wordcard_btn_back, screen_wordcard_btn_back_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wordcard_btn_left, screen_wordcard_btn_left_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wordcard_btn_right, screen_wordcard_btn_right_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wordcard_btn_dui, screen_wordcard_btn_dui_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wordcard_btn_cuo, screen_wordcard_btn_cuo_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wordcard_btn_home, screen_wordcard_btn_home_event_handler, LV_EVENT_ALL, ui);
}

static void screen_reviewcard_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        //screen button add
        screen_reviewcard_buttion_add();
        //------------------------//
        review_index = 0;
        review_solve_num = 0;
        if(book_info[wordbook_index].word_review_num == 0) {
            lv_obj_set_style_text_color(guider_ui.screen_reviewcard_label_spelling, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
            lv_obj_add_state(guider_ui.screen_reviewcard_btn_dui, LV_STATE_DISABLED);
            lv_obj_add_state(guider_ui.screen_reviewcard_btn_cuo, LV_STATE_DISABLED);
            lv_label_set_text(guider_ui.screen_reviewcard_label_index,"0/0");
        } else {
            lv_obj_set_style_text_color(guider_ui.screen_reviewcard_label_spelling, lv_color_hex(0xefcbd1), LV_PART_MAIN|LV_STATE_DEFAULT);
            lv_obj_clear_state(guider_ui.screen_reviewcard_btn_dui, LV_STATE_DISABLED);
            lv_obj_add_state(guider_ui.screen_reviewcard_btn_cuo, LV_STATE_DISABLED);
            review_index = 0;
            lv_label_set_text_fmt(guider_ui.screen_reviewcard_label_index,"%d/%d",review_index + 1,book_info[wordbook_index].word_review_num);
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
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_wordmeun, guider_ui.screen_wordmeun_del, &guider_ui.screen_reviewcard_del, setup_scr_screen_wordmeun, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        int review_new_index = 0;
        for(int i = 0 ; i < book_info[wordbook_index].word_review_num ; i++) {
            int word_index_tmp = book_info[wordbook_index].review_absoulte_index[i];
            if(book_info[wordbook_index].word_state[word_index_tmp] == 2) {
                book_info[wordbook_index].review_absoulte_index[review_new_index] = word_index_tmp;
                review_new_index = review_new_index + 1;
            }
        }
        book_info[wordbook_index].word_review_num -= review_solve_num;

        lv_obj_t * act_scr = lv_scr_act();
        lv_obj_clear_flag(act_scr, LV_OBJ_FLAG_CLICKABLE);



        break;
    }
    default:
        break;
    }
}

static void screen_reviewcard_btn_left_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        if(review_index >= 1) {
            review_index -= 1;
            lv_label_set_text_fmt(guider_ui.screen_reviewcard_label_index,"%d/%d",review_index + 1,book_info[wordbook_index].word_review_num);
            if(book_info[wordbook_index].word_state[book_info[wordbook_index].review_absoulte_index[review_index]] == 1) {
                lv_obj_set_style_text_color(guider_ui.screen_reviewcard_label_spelling, lv_color_hex(0xdaffcb), LV_PART_MAIN|LV_STATE_DEFAULT);
                lv_obj_add_state(guider_ui.screen_reviewcard_btn_dui, LV_STATE_DISABLED);
                lv_obj_clear_state(guider_ui.screen_reviewcard_btn_cuo, LV_STATE_DISABLED);
            } else if(book_info[wordbook_index].word_state[book_info[wordbook_index].review_absoulte_index[review_index]] == 2) {
                lv_obj_set_style_text_color(guider_ui.screen_reviewcard_label_spelling, lv_color_hex(0xefcbd1), LV_PART_MAIN|LV_STATE_DEFAULT);
                lv_obj_clear_state(guider_ui.screen_reviewcard_btn_dui, LV_STATE_DISABLED);
                lv_obj_add_state(guider_ui.screen_reviewcard_btn_cuo, LV_STATE_DISABLED);
            }
            reviewcard_change = 1;
        }
        break;
    }
    default:
        break;
    }
}

static void screen_reviewcard_btn_right_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        if(review_index < book_info[wordbook_index].word_review_num - 1) {
            review_index += 1;
            lv_label_set_text_fmt(guider_ui.screen_reviewcard_label_index,"%d/%d",review_index + 1,book_info[wordbook_index].word_review_num);
            if(book_info[wordbook_index].word_state[book_info[wordbook_index].review_absoulte_index[review_index]] == 1) {
                lv_obj_set_style_text_color(guider_ui.screen_reviewcard_label_spelling, lv_color_hex(0xdaffcb), LV_PART_MAIN|LV_STATE_DEFAULT);
                lv_obj_add_state(guider_ui.screen_reviewcard_btn_dui, LV_STATE_DISABLED);
                lv_obj_clear_state(guider_ui.screen_reviewcard_btn_cuo, LV_STATE_DISABLED);
            } else if(book_info[wordbook_index].word_state[book_info[wordbook_index].review_absoulte_index[review_index]] == 2) {
                lv_obj_set_style_text_color(guider_ui.screen_reviewcard_label_spelling, lv_color_hex(0xefcbd1), LV_PART_MAIN|LV_STATE_DEFAULT);
                lv_obj_clear_state(guider_ui.screen_reviewcard_btn_dui, LV_STATE_DISABLED);
                lv_obj_add_state(guider_ui.screen_reviewcard_btn_cuo, LV_STATE_DISABLED);
            }
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
        led_green_en = 1; // flow light rgb
        if(review_index < book_info[wordbook_index].word_review_num - 1) {
            book_info[wordbook_index].word_learned_num += 1;
            review_solve_num += 1;
            book_info[wordbook_index].word_state[book_info[wordbook_index].review_absoulte_index[review_index]] = 1;
            review_index += 1;
            lv_label_set_text_fmt(guider_ui.screen_reviewcard_label_index,"%d/%d",review_index + 1,book_info[wordbook_index].word_review_num);
            reviewcard_change = 1;
        } else {
            book_info[wordbook_index].word_learned_num += 1;
            review_solve_num += 1;
            book_info[wordbook_index].word_state[book_info[wordbook_index].review_absoulte_index[review_index]] = 1;
            lv_label_set_text_fmt(guider_ui.screen_reviewcard_label_index,"%d/%d",review_index + 1,book_info[wordbook_index].word_review_num);
        }

        if(book_info[wordbook_index].word_state[book_info[wordbook_index].review_absoulte_index[review_index]] == 1) {
            lv_obj_set_style_text_color(guider_ui.screen_reviewcard_label_spelling, lv_color_hex(0xdaffcb), LV_PART_MAIN|LV_STATE_DEFAULT);
            lv_obj_add_state(guider_ui.screen_reviewcard_btn_dui, LV_STATE_DISABLED);
            lv_obj_clear_state(guider_ui.screen_reviewcard_btn_cuo, LV_STATE_DISABLED);
        } else if(book_info[wordbook_index].word_state[book_info[wordbook_index].review_absoulte_index[review_index]] == 2) {
            lv_obj_set_style_text_color(guider_ui.screen_reviewcard_label_spelling, lv_color_hex(0xefcbd1), LV_PART_MAIN|LV_STATE_DEFAULT);
            lv_obj_clear_state(guider_ui.screen_reviewcard_btn_dui, LV_STATE_DISABLED);
            lv_obj_add_state(guider_ui.screen_reviewcard_btn_cuo, LV_STATE_DISABLED);
        }
        break;
    }
    default:
        break;
    }
}

static void screen_reviewcard_btn_home_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_main, guider_ui.screen_main_del, &guider_ui.screen_reviewcard_del, setup_scr_screen_main, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        int review_new_index = 0;
        for(int i = 0 ; i < book_info[wordbook_index].word_review_num ; i++) {
            int word_index_tmp = book_info[wordbook_index].review_absoulte_index[i];
            if(book_info[wordbook_index].word_state[word_index_tmp] == 2) {
                book_info[wordbook_index].review_absoulte_index[review_new_index] = word_index_tmp;
                review_new_index = review_new_index + 1;
            }
        }
        book_info[wordbook_index].word_review_num -= review_solve_num;

        lv_obj_t * act_scr = lv_scr_act();
        lv_obj_clear_flag(act_scr, LV_OBJ_FLAG_CLICKABLE);

        break;
    }
    default:
        break;
    }
}

static void screen_reviewcard_btn_cuo_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        led_red_en = 1; //flow light rgb
        if(review_index < book_info[wordbook_index].word_review_num - 1) {
            book_info[wordbook_index].word_learned_num -= 1;
            review_solve_num -= 1;
            book_info[wordbook_index].word_state[book_info[wordbook_index].review_absoulte_index[review_index]] = 2;
            review_index += 1;
            lv_label_set_text_fmt(guider_ui.screen_reviewcard_label_index,"%d/%d",review_index + 1,book_info[wordbook_index].word_review_num);
            reviewcard_change = 1;
        } else {
            book_info[wordbook_index].word_learned_num -= 1;
            review_solve_num -= 1;
            book_info[wordbook_index].word_state[book_info[wordbook_index].review_absoulte_index[review_index]] = 2;
            lv_label_set_text_fmt(guider_ui.screen_reviewcard_label_index,"%d/%d",review_index + 1,book_info[wordbook_index].word_review_num);
        }

        if(book_info[wordbook_index].word_state[book_info[wordbook_index].review_absoulte_index[review_index]] == 1) {
            lv_obj_set_style_text_color(guider_ui.screen_reviewcard_label_spelling, lv_color_hex(0xdaffcb), LV_PART_MAIN|LV_STATE_DEFAULT);
            lv_obj_add_state(guider_ui.screen_reviewcard_btn_dui, LV_STATE_DISABLED);
            lv_obj_clear_state(guider_ui.screen_reviewcard_btn_cuo, LV_STATE_DISABLED);
        } else if(book_info[wordbook_index].word_state[book_info[wordbook_index].review_absoulte_index[review_index]] == 2) {
            lv_obj_set_style_text_color(guider_ui.screen_reviewcard_label_spelling, lv_color_hex(0xefcbd1), LV_PART_MAIN|LV_STATE_DEFAULT);
            lv_obj_clear_state(guider_ui.screen_reviewcard_btn_dui, LV_STATE_DISABLED);
            lv_obj_add_state(guider_ui.screen_reviewcard_btn_cuo, LV_STATE_DISABLED);
        }
        break;
    }
    default:
        break;
    }
}

void events_init_screen_reviewcard (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_reviewcard, screen_reviewcard_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_reviewcard_btn_back, screen_reviewcard_btn_back_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_reviewcard_btn_left, screen_reviewcard_btn_left_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_reviewcard_btn_right, screen_reviewcard_btn_right_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_reviewcard_btn_dui, screen_reviewcard_btn_dui_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_reviewcard_btn_home, screen_reviewcard_btn_home_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_reviewcard_btn_cuo, screen_reviewcard_btn_cuo_event_handler, LV_EVENT_ALL, ui);
}

static void screen_setting_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        //screen buttion add
        screen_setting_buttion_add();
        //container configure flex
        menu_cont = guider_ui.screen_setting_cont_menu;
        lv_obj_set_style_clip_corner(menu_cont, false, 0);
        lv_obj_set_flex_flow(menu_cont, LV_FLEX_FLOW_ROW);
        lv_obj_set_scroll_dir(menu_cont, LV_DIR_HOR);
        lv_obj_set_flex_align(menu_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_scroll_snap_x(menu_cont, LV_SCROLL_SNAP_CENTER);
        lv_obj_set_style_pad_column(menu_cont, 35, 0); // 设置图标间距
        lv_obj_scroll_to_view(lv_obj_get_child(menu_cont, 0), LV_ANIM_ON);
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
    case LV_EVENT_FOCUSED:
    {
        lv_label_set_text(guider_ui.screen_setting_label_settings, "Wifi Configuration");
        ui_animation(guider_ui.screen_setting_label_settings, 0, 0, lv_obj_get_y(guider_ui.screen_setting_label_settings), -25, &lv_anim_path_linear, 1, 0, 200, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        lv_obj_clear_flag(guider_ui.screen_setting_img_point1, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_obj_add_flag(guider_ui.screen_setting_img_point1, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_y(guider_ui.screen_setting_label_settings, 8);
        break;
    }
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_wificfg, guider_ui.screen_wificfg_del, &guider_ui.screen_setting_del, setup_scr_screen_wificfg, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        lv_obj_t * act_scr = lv_scr_act();
        lv_obj_clear_flag(act_scr, LV_OBJ_FLAG_CLICKABLE);
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
        lv_label_set_text(guider_ui.screen_setting_label_settings, "BlueTooth");
        ui_animation(guider_ui.screen_setting_label_settings, 0, 0, lv_obj_get_y(guider_ui.screen_setting_label_settings), -25, &lv_anim_path_linear, 1, 0, 200, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        lv_obj_clear_flag(guider_ui.screen_setting_img_point5, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_obj_add_flag(guider_ui.screen_setting_img_point5, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_y(guider_ui.screen_setting_label_settings, 8);
        break;
    }
    default:
        break;
    }
}

static void screen_setting_btn_data_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_FOCUSED:
    {
        lv_label_set_text(guider_ui.screen_setting_label_settings, "AI Summary");
        ui_animation(guider_ui.screen_setting_label_settings, 0, 0, lv_obj_get_y(guider_ui.screen_setting_label_settings), -25, &lv_anim_path_linear, 1, 0, 200, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        lv_obj_clear_flag(guider_ui.screen_setting_img_point7, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_obj_add_flag(guider_ui.screen_setting_img_point7, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_y(guider_ui.screen_setting_label_settings, 8);
        break;
    }
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_ai, guider_ui.screen_ai_del, &guider_ui.screen_setting_del, setup_scr_screen_ai, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

static void screen_setting_btn_todo_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_FOCUSED:
    {
        lv_label_set_text(guider_ui.screen_setting_label_settings, "Tasks Import");
        ui_animation(guider_ui.screen_setting_label_settings, 0, 0, lv_obj_get_y(guider_ui.screen_setting_label_settings), -25, &lv_anim_path_linear, 1, 0, 200, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        lv_obj_clear_flag(guider_ui.screen_setting_img_point8, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_obj_add_flag(guider_ui.screen_setting_img_point8, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_y(guider_ui.screen_setting_label_settings, 8);
        break;
    }
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_donelist, guider_ui.screen_donelist_del, &guider_ui.screen_setting_del, setup_scr_screen_donelist, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

static void screen_setting_btn_vocab_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_FOCUSED:
    {
        lv_label_set_text(guider_ui.screen_setting_label_settings, "Vocab Import");
        ui_animation(guider_ui.screen_setting_label_settings, 0, 0, lv_obj_get_y(guider_ui.screen_setting_label_settings), -25, &lv_anim_path_linear, 1, 0, 200, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        lv_obj_clear_flag(guider_ui.screen_setting_img_point2, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_obj_add_flag(guider_ui.screen_setting_img_point2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_y(guider_ui.screen_setting_label_settings, 8);
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
        lv_label_set_text(guider_ui.screen_setting_label_settings, "General Info");
        ui_animation(guider_ui.screen_setting_label_settings, 0, 0, lv_obj_get_y(guider_ui.screen_setting_label_settings), -25, &lv_anim_path_linear, 1, 0, 200, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        lv_obj_clear_flag(guider_ui.screen_setting_img_point6, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_obj_add_flag(guider_ui.screen_setting_img_point6, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_y(guider_ui.screen_setting_label_settings, 8);
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

void events_init_screen_setting (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_setting, screen_setting_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_setting_btn_wifi, screen_setting_btn_wifi_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_setting_btn_ble, screen_setting_btn_ble_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_setting_btn_data, screen_setting_btn_data_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_setting_btn_todo, screen_setting_btn_todo_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_setting_btn_vocab, screen_setting_btn_vocab_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_setting_btn_general, screen_setting_btn_general_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_setting_btn_back, screen_setting_btn_back_event_handler, LV_EVENT_ALL, ui);
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
            lv_obj_set_style_text_color(guider_ui.screen_wificfg_label_status2, lv_color_hex(0xB7F0B8), 0);
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
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_setting, guider_ui.screen_setting_del, &guider_ui.screen_wificfg_del, setup_scr_screen_setting, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
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
        //add and clear some flag -> hidden
        lv_obj_add_flag(guider_ui.screen_wificfg_img_wififalse, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_wificfg_img_wifiicon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_wificfg_label_status2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_wificfg_btn_cancel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_wificfg_btn_connect, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_wificfg_img_passicon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_wificfg_ta_password, LV_OBJ_FLAG_HIDDEN);
        // // get label from btn_wifi1
        const char * txt = lv_list_get_btn_text(guider_ui.screen_wificfg_list_1, guider_ui.screen_wificfg_list_1_item0);
        lv_label_set_text_fmt(guider_ui.screen_wificfg_label_status,"Connect to %s",txt);
        // If wifi_ssid already points to something, free it first to avoid memory leaks
        if(wifi_ssid != NULL) free(wifi_ssid);
        // Allocate new memory and copy the string
        wifi_ssid = strdup(txt);
        // //focus state edit
        lv_group_focus_obj(guider_ui.screen_wificfg_ta_password);

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
        //add and clear some flag -> hidden
        lv_obj_add_flag(guider_ui.screen_wificfg_img_wififalse, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_wificfg_img_wifiicon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_wificfg_label_status2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_wificfg_img_wififalse, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_wificfg_btn_cancel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_wificfg_btn_connect, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_wificfg_img_passicon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_wificfg_ta_password, LV_OBJ_FLAG_HIDDEN);
        // get label from btn_wifi1
        const char * txt = lv_list_get_btn_text(guider_ui.screen_wificfg_list_1, guider_ui.screen_wificfg_list_1_item1);
        lv_label_set_text_fmt(guider_ui.screen_wificfg_label_status,"Connect to %s",txt);
        // If wifi_ssid already points to something, free it first to avoid memory leaks
        if(wifi_ssid != NULL) free(wifi_ssid);
        // Allocate new memory and copy the string
        wifi_ssid = strdup(txt);
        //focus state edit
        lv_group_focus_obj(guider_ui.screen_wificfg_ta_password);
        lv_refr_now(NULL);
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
        //add and clear some flag -> hidden
        lv_obj_add_flag(guider_ui.screen_wificfg_img_wififalse, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_wificfg_img_wifiicon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_wificfg_label_status2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_wificfg_img_wififalse, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_wificfg_btn_cancel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_wificfg_btn_connect, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_wificfg_img_passicon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_wificfg_ta_password, LV_OBJ_FLAG_HIDDEN);
        // get label from btn_wifi1
        const char * txt = lv_list_get_btn_text(guider_ui.screen_wificfg_list_1, guider_ui.screen_wificfg_list_1_item2);
        lv_label_set_text_fmt(guider_ui.screen_wificfg_label_status,"Connect to %s",txt);
        // If wifi_ssid already points to something, free it first to avoid memory leaks
        if(wifi_ssid != NULL) free(wifi_ssid);
        // Allocate new memory and copy the string
        wifi_ssid = strdup(txt);
        //focus state edit
        lv_group_focus_obj(guider_ui.screen_wificfg_ta_password);
        lv_refr_now(NULL);
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
        //add and clear some flag -> hidden
        lv_obj_add_flag(guider_ui.screen_wificfg_img_wififalse, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_wificfg_img_wifiicon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_wificfg_label_status2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_wificfg_img_wififalse, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_wificfg_btn_cancel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_wificfg_btn_connect, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_wificfg_img_passicon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_wificfg_ta_password, LV_OBJ_FLAG_HIDDEN);
        // get label from btn_wifi1
        const char * txt = lv_list_get_btn_text(guider_ui.screen_wificfg_list_1, guider_ui.screen_wificfg_list_1_item3);
        lv_label_set_text_fmt(guider_ui.screen_wificfg_label_status,"Connect to %s",txt);
        // If wifi_ssid already points to something, free it first to avoid memory leaks
        if(wifi_ssid != NULL) free(wifi_ssid);
        // Allocate new memory and copy the string
        wifi_ssid = strdup(txt);
        //focus state edit
        lv_group_focus_obj(guider_ui.screen_wificfg_ta_password);
        lv_refr_now(NULL);
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
        //add and clear some flag -> hidden
        lv_obj_add_flag(guider_ui.screen_wificfg_img_wififalse, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_wificfg_img_wifiicon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_wificfg_label_status2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.screen_wificfg_img_wififalse, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_wificfg_btn_cancel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_wificfg_btn_connect, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_wificfg_img_passicon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.screen_wificfg_ta_password, LV_OBJ_FLAG_HIDDEN);
        // get label from btn_wifi1
        const char * txt = lv_list_get_btn_text(guider_ui.screen_wificfg_list_1, guider_ui.screen_wificfg_list_1_item4);
        lv_label_set_text_fmt(guider_ui.screen_wificfg_label_status,"Connect to %s",txt);
        // If wifi_ssid already points to something, free it first to avoid memory leaks
        if(wifi_ssid != NULL) free(wifi_ssid);
        // Allocate new memory and copy the string
        wifi_ssid = strdup(txt);
        //focus state edit
        lv_group_focus_obj(guider_ui.screen_wificfg_ta_password);
        lv_refr_now(NULL);
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
        lv_obj_set_style_text_color(guider_ui.screen_wificfg_label_status2, lv_color_hex(0xdfbcca), LV_PART_MAIN|LV_STATE_DEFAULT);
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
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_todolist, guider_ui.screen_todolist_del, &guider_ui.screen_todosel_del, setup_scr_screen_todolist, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, true, true);
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
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_pomodoro, guider_ui.screen_pomodoro_del, &guider_ui.screen_todosel_del, setup_scr_screen_pomodoro, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, true, true);
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
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_todolist, guider_ui.screen_todolist_del, &guider_ui.screen_todosel_del, setup_scr_screen_todolist, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, true, true);
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

static void screen_ai_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        screen_ai_buttion_add();
        is_week = 0;
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

static void screen_ai_btn_back_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_main, guider_ui.screen_main_del, &guider_ui.screen_ai_del, setup_scr_screen_main, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_ai (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_ai, screen_ai_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_ai_btn_day, screen_ai_btn_day_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_ai_btn_week, screen_ai_btn_week_event_handler, LV_EVENT_ALL, ui);
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
        donelist_load_label = 1;
        //the task_done.csv will change
        //reload screen_donelist ui info
        //pagecur start to 1
        donelist_page_cur = 1;
        done_total_tasks += done_sum;
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
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_setting, guider_ui.screen_setting_del, &guider_ui.screen_donelist_del, setup_scr_screen_setting, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
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


void events_init(lv_ui *ui)
{

}
