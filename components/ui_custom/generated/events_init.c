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
        //refresh done and todo num
        uint8_t total_todo_num = val * 4;
        done_sum = 0;
        todo_sum = 0;
        //calculate done tasks num
        for(int i = 0 ; i < val ; i++) {
            for(int j = 0; j < 4 ; j++) {
                done_sum += done_array[i][j];
            }
        }
        //calculate todo tasks num
        todo_sum = total_todo_num - done_sum;
        lv_label_set_text_fmt(guider_ui.screen_main_label_donenum, "%d", done_sum);
        lv_label_set_text_fmt(guider_ui.screen_main_label_todonum, "%d", todo_sum);

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
        lv_obj_t * act_scr = lv_scr_act();
        lv_obj_clear_flag(act_scr, LV_OBJ_FLAG_CLICKABLE);
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
        lv_obj_t * act_scr = lv_scr_act();
        lv_obj_clear_flag(act_scr, LV_OBJ_FLAG_CLICKABLE);
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
        lv_obj_t * act_scr = lv_scr_act();
        lv_obj_clear_flag(act_scr, LV_OBJ_FLAG_CLICKABLE);
        break;
    }
    default:
        break;
    }
}

static void screen_main_btn_plane_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_animation(guider_ui.screen_main_img_plane, 1000, 0, lv_obj_get_x(guider_ui.screen_main_img_plane), 100, &lv_anim_path_linear, 1, 0, 100, 0, (lv_anim_exec_xcb_t)lv_obj_set_x, NULL, NULL, NULL);
        ui_animation(guider_ui.screen_main_img_plane, 1000, 0, lv_obj_get_y(guider_ui.screen_main_img_plane), 125, &lv_anim_path_linear, 1, 0, 100, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.screen_main_img_plane, 1000, 0, lv_img_get_zoom(guider_ui.screen_main_img_plane), 400, &lv_anim_path_linear, 1, 0, 100, 0, (lv_anim_exec_xcb_t)lv_img_set_zoom, NULL, NULL, NULL);
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
        lv_obj_t * act_scr = lv_scr_act();
        lv_obj_clear_flag(act_scr, LV_OBJ_FLAG_CLICKABLE);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_main (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_main, screen_main_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_main_btn_word, screen_main_btn_word_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_main_btn_timer, screen_main_btn_timer_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_main_btn_setting, screen_main_btn_setting_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_main_btn_plane, screen_main_btn_plane_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_main_btn_todolist, screen_main_btn_todolist_event_handler, LV_EVENT_ALL, ui);
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
        uint8_t total_todo_num = val * 4;
        done_sum = 0;
        todo_sum = 0;
        //calculate done tasks num
        for(int i = 0 ; i < val ; i++) {
            for(int j = 0; j < 4 ; j++) {
                done_sum += done_array[i][j];
            }
        }
        //calculate todo tasks num
        todo_sum = total_todo_num - done_sum;
        lv_label_set_text_fmt(guider_ui.screen_todolist_label_donenum, "%d", done_sum);
        lv_label_set_text_fmt(guider_ui.screen_todolist_label_todonum, "%d", todo_sum);
        lv_label_set_text_fmt(guider_ui.screen_todolist_label_indexall, "%d", val);
        lv_label_set_text_fmt(guider_ui.screen_todolist_label_indexcur, "%d", val_cur);

        if(done_array[val_cur-1][0] == 1) {
            lv_obj_set_style_text_color(guider_ui.screen_todolist_label_todo1, lv_color_hex(0x9a9a9a), LV_PART_MAIN|LV_STATE_DEFAULT);
            done_array[val_cur-1][0] = 1;
        } else {
            lv_obj_set_style_text_color(guider_ui.screen_todolist_label_todo1, lv_color_hex(0xFFFFFF), LV_PART_MAIN|LV_STATE_DEFAULT);
            done_array[val_cur-1][0] = 0;
        }
        if(done_array[val_cur-1][1] == 1) {
            lv_obj_set_style_text_color(guider_ui.screen_todolist_label_todo2, lv_color_hex(0x9a9a9a), LV_PART_MAIN|LV_STATE_DEFAULT);
            done_array[val_cur-1][1] = 1;
        } else {
            lv_obj_set_style_text_color(guider_ui.screen_todolist_label_todo2, lv_color_hex(0xFFFFFF), LV_PART_MAIN|LV_STATE_DEFAULT);
            done_array[val_cur-1][1] = 0;
        }
        if(done_array[val_cur-1][2] == 1) {
            lv_obj_set_style_text_color(guider_ui.screen_todolist_label_todo3, lv_color_hex(0x9a9a9a), LV_PART_MAIN|LV_STATE_DEFAULT);
            done_array[val_cur-1][2] = 1;
        } else {
            lv_obj_set_style_text_color(guider_ui.screen_todolist_label_todo3, lv_color_hex(0xFFFFFF), LV_PART_MAIN|LV_STATE_DEFAULT);
            done_array[val_cur-1][2] = 0;
        }
        if(done_array[val_cur-1][3] == 1) {
            lv_obj_set_style_text_color(guider_ui.screen_todolist_label_todo4, lv_color_hex(0x9a9a9a), LV_PART_MAIN|LV_STATE_DEFAULT);
            done_array[val_cur-1][3] = 1;
        } else {
            lv_obj_set_style_text_color(guider_ui.screen_todolist_label_todo4, lv_color_hex(0xFFFFFF), LV_PART_MAIN|LV_STATE_DEFAULT);
            done_array[val_cur-1][3] = 0;
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
        uint8_t total_todo_num = val * 4;
        done_sum = 0;
        todo_sum = 0;
        if(done_array[val_cur-1][0] == 0) {
            lv_obj_set_style_text_color(guider_ui.screen_todolist_label_todo1, lv_color_hex(0x9a9a9a), LV_PART_MAIN|LV_STATE_DEFAULT);
            done_array[val_cur-1][0] = 1;
        } else {
            lv_obj_set_style_text_color(guider_ui.screen_todolist_label_todo1, lv_color_hex(0xFFFFFF), LV_PART_MAIN|LV_STATE_DEFAULT);
            done_array[val_cur-1][0] = 0;
        }
        //calculate done tasks num
        for(int i = 0 ; i < val ; i++) {
            for(int j = 0; j < 4 ; j++) {
                done_sum += done_array[i][j];
            }
        }
        //calculate todo tasks num
        todo_sum = total_todo_num - done_sum;
        lv_label_set_text_fmt(guider_ui.screen_todolist_label_donenum, "%d", done_sum);
        lv_label_set_text_fmt(guider_ui.screen_todolist_label_todonum, "%d", todo_sum);
        //copy label to todosel menu
        const char * text_ptr = lv_label_get_text(guider_ui.screen_todolist_label_todo1);
        // 3. 安全地拷贝到你的变量中 (防止溢出)
        strncpy(current_task_name, text_ptr, sizeof(current_task_name) - 1);
        current_task_name[sizeof(current_task_name) - 1] = '\0'; // 确保字符串以 \0 结尾
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_todosel, guider_ui.screen_todosel_del, &guider_ui.screen_todolist_del, setup_scr_screen_todosel, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, false, false);
        break;
    }
    case LV_EVENT_FOCUSED:
    {
        lv_label_set_long_mode(guider_ui.screen_todolist_label_todo1, LV_LABEL_LONG_SCROLL_CIRCULAR);
        lv_label_set_long_mode(guider_ui.screen_todolist_label_todo2, LV_LABEL_LONG_CLIP);
        lv_label_set_long_mode(guider_ui.screen_todolist_label_todo3, LV_LABEL_LONG_CLIP);
        lv_label_set_long_mode(guider_ui.screen_todolist_label_todo4, LV_LABEL_LONG_CLIP);
        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_label_set_long_mode(guider_ui.screen_todolist_label_todo1, LV_LABEL_LONG_CLIP);

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
        uint8_t total_todo_num = val * 4;
        if(done_array[val_cur-1][1] == 0) {
            lv_obj_set_style_text_color(guider_ui.screen_todolist_label_todo2, lv_color_hex(0x9a9a9a), LV_PART_MAIN|LV_STATE_DEFAULT);
            done_array[val_cur-1][1] = 1;
        } else {
            lv_obj_set_style_text_color(guider_ui.screen_todolist_label_todo2, lv_color_hex(0xFFFFFF), LV_PART_MAIN|LV_STATE_DEFAULT);
            done_array[val_cur-1][1] = 0;
        }
        done_sum = 0;
        todo_sum = 0;
        //calculate done tasks num
        for(int i = 0 ; i < val ; i++) {
            for(int j = 0; j < 4 ; j++) {
                done_sum += done_array[i][j];
            }
        }
        //calculate todo tasks num
        todo_sum = total_todo_num - done_sum;
        lv_label_set_text_fmt(guider_ui.screen_todolist_label_donenum, "%d", done_sum);
        lv_label_set_text_fmt(guider_ui.screen_todolist_label_todonum, "%d", todo_sum);
        //copy label to todosel menu
        const char * text_ptr = lv_label_get_text(guider_ui.screen_todolist_label_todo2);
        // 3. 安全地拷贝到你的变量中 (防止溢出)
        strncpy(current_task_name, text_ptr, sizeof(current_task_name) - 1);
        current_task_name[sizeof(current_task_name) - 1] = '\0'; // 确保字符串以 \0 结尾
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_todosel, guider_ui.screen_todosel_del, &guider_ui.screen_todolist_del, setup_scr_screen_todosel, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, false, false);
        break;
    }
    case LV_EVENT_FOCUSED:
    {
        lv_label_set_long_mode(guider_ui.screen_todolist_label_todo2, LV_LABEL_LONG_SCROLL_CIRCULAR);
        lv_label_set_long_mode(guider_ui.screen_todolist_label_todo1, LV_LABEL_LONG_CLIP);
        lv_label_set_long_mode(guider_ui.screen_todolist_label_todo3, LV_LABEL_LONG_CLIP);
        lv_label_set_long_mode(guider_ui.screen_todolist_label_todo4, LV_LABEL_LONG_CLIP);
        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_label_set_long_mode(guider_ui.screen_todolist_label_todo2, LV_LABEL_LONG_CLIP);

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
        uint8_t total_todo_num = val * 4;
        if(done_array[val_cur-1][2] == 0) {
            lv_obj_set_style_text_color(guider_ui.screen_todolist_label_todo3, lv_color_hex(0x9a9a9a), LV_PART_MAIN|LV_STATE_DEFAULT);
            done_array[val_cur-1][2] = 1;
        } else {
            lv_obj_set_style_text_color(guider_ui.screen_todolist_label_todo3, lv_color_hex(0xFFFFFF), LV_PART_MAIN|LV_STATE_DEFAULT);
            done_array[val_cur-1][2] = 0;
        }
        done_sum = 0;
        todo_sum = 0;
        //calculate done tasks num
        for(int i = 0 ; i < val ; i++) {
            for(int j = 0; j < 4 ; j++) {
                done_sum += done_array[i][j];
            }
        }
        //calculate todo tasks num
        todo_sum = total_todo_num - done_sum;
        lv_label_set_text_fmt(guider_ui.screen_todolist_label_donenum, "%d", done_sum);
        lv_label_set_text_fmt(guider_ui.screen_todolist_label_todonum, "%d", todo_sum);
        //copy label to todosel menu
        const char * text_ptr = lv_label_get_text(guider_ui.screen_todolist_label_todo3);
        // 3. 安全地拷贝到你的变量中 (防止溢出)
        strncpy(current_task_name, text_ptr, sizeof(current_task_name) - 1);
        current_task_name[sizeof(current_task_name) - 1] = '\0'; // 确保字符串以 \0 结尾
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_todosel, guider_ui.screen_todosel_del, &guider_ui.screen_todolist_del, setup_scr_screen_todosel, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, false, false);
        break;
    }
    case LV_EVENT_FOCUSED:
    {
        lv_label_set_long_mode(guider_ui.screen_todolist_label_todo3, LV_LABEL_LONG_SCROLL_CIRCULAR);
        lv_label_set_long_mode(guider_ui.screen_todolist_label_todo1, LV_LABEL_LONG_CLIP);
        lv_label_set_long_mode(guider_ui.screen_todolist_label_todo2, LV_LABEL_LONG_CLIP);
        lv_label_set_long_mode(guider_ui.screen_todolist_label_todo4, LV_LABEL_LONG_CLIP);
        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_label_set_long_mode(guider_ui.screen_todolist_label_todo3, LV_LABEL_LONG_CLIP);

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
        uint8_t total_todo_num = val * 4;
        if(done_array[val_cur-1][3] == 0) {
            lv_obj_set_style_text_color(guider_ui.screen_todolist_label_todo4, lv_color_hex(0x9a9a9a), LV_PART_MAIN|LV_STATE_DEFAULT);
            done_array[val_cur-1][3] = 1;
        } else {
            lv_obj_set_style_text_color(guider_ui.screen_todolist_label_todo4, lv_color_hex(0xFFFFFF), LV_PART_MAIN|LV_STATE_DEFAULT);
            done_array[val_cur-1][3] = 0;
        }
        done_sum = 0;
        todo_sum = 0;
        //calculate done tasks num
        for(int i = 0 ; i < val ; i++) {
            for(int j = 0; j < 4 ; j++) {
                done_sum += done_array[i][j];
            }
        }
        //calculate todo tasks num
        todo_sum = total_todo_num - done_sum;
        lv_label_set_text_fmt(guider_ui.screen_todolist_label_donenum, "%d", done_sum);
        lv_label_set_text_fmt(guider_ui.screen_todolist_label_todonum, "%d", todo_sum);
        //copy label to todosel menu
        const char * text_ptr = lv_label_get_text(guider_ui.screen_todolist_label_todo4);
        // 3. 安全地拷贝到你的变量中 (防止溢出)
        strncpy(current_task_name, text_ptr, sizeof(current_task_name) - 1);
        current_task_name[sizeof(current_task_name) - 1] = '\0'; // 确保字符串以 \0 结尾
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_todosel, guider_ui.screen_todosel_del, &guider_ui.screen_todolist_del, setup_scr_screen_todosel, LV_SCR_LOAD_ANIM_FADE_ON, 100, 100, false, false);
        break;
    }
    case LV_EVENT_FOCUSED:
    {
        lv_label_set_long_mode(guider_ui.screen_todolist_label_todo4, LV_LABEL_LONG_SCROLL_CIRCULAR);
        lv_label_set_long_mode(guider_ui.screen_todolist_label_todo1, LV_LABEL_LONG_CLIP);
        lv_label_set_long_mode(guider_ui.screen_todolist_label_todo2, LV_LABEL_LONG_CLIP);
        lv_label_set_long_mode(guider_ui.screen_todolist_label_todo3, LV_LABEL_LONG_CLIP);
        break;
    }
    case LV_EVENT_DEFOCUSED:
    {
        lv_label_set_long_mode(guider_ui.screen_todolist_label_todo4, LV_LABEL_LONG_CLIP);
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

static void screen_timing_btn_pomodoro_event_handler (lv_event_t *e)
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

static void screen_timing_btn_back_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_pomodoro, guider_ui.screen_pomodoro_del, &guider_ui.screen_timing_del, setup_scr_screen_pomodoro, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
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
    lv_obj_add_event_cb(ui->screen_timing_btn_pomodoro, screen_timing_btn_pomodoro_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_timing_btn_back, screen_timing_btn_back_event_handler, LV_EVENT_ALL, ui);
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
        //focus or not juge
        if(focus_en == 1) {
            if(guider_ui.screen_pomodoro_label_pomodoro != NULL) {
                lv_label_set_text(guider_ui.screen_pomodoro_label_pomodoro, current_task_name);
            }
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

static void screen_pomodoro_btn_home_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_timing, guider_ui.screen_timing_del, &guider_ui.screen_pomodoro_del, setup_scr_screen_timing, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
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

void events_init_screen_pomodoro (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_pomodoro, screen_pomodoro_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_pomodoro_btn_stopwatch, screen_pomodoro_btn_stopwatch_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_pomodoro_btn_countdown, screen_pomodoro_btn_countdown_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_pomodoro_btn_back, screen_pomodoro_btn_back_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_pomodoro_btn_home, screen_pomodoro_btn_home_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_pomodoro_btn_start, screen_pomodoro_btn_start_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_pomodoro_btn_reset, screen_pomodoro_btn_reset_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_pomodoro_btn_addnum, screen_pomodoro_btn_addnum_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_pomodoro_btn_minus, screen_pomodoro_btn_minus_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_pomodoro_btn_plus, screen_pomodoro_btn_plus_event_handler, LV_EVENT_ALL, ui);
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
        //set info to label_task
        if(guider_ui.screen_todosel_label_task != NULL) {
            lv_label_set_text(guider_ui.screen_todosel_label_task, current_task_name);
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
        focus_en = 1;
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


void events_init(lv_ui *ui)
{

}
