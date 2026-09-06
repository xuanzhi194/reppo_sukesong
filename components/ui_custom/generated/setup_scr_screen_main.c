/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"



void setup_scr_screen_main(lv_ui *ui)
{
    //Write codes screen_main
    ui->screen_main = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_main, 320, 170);
    lv_obj_set_scrollbar_mode(ui->screen_main, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_main, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_main, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_main, lv_color_hex(0x0e1b28), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_main, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_main, &_AAA_main_bg_320x170, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_main, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_main, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_cont_main
    ui->screen_main_cont_main = lv_obj_create(ui->screen_main);
    lv_obj_set_pos(ui->screen_main_cont_main, 0, 28);
    lv_obj_set_size(ui->screen_main_cont_main, 320, 100);
    lv_obj_set_scrollbar_mode(ui->screen_main_cont_main, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_main_cont_main, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_main_cont_main, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_cont_main, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_main_cont_main, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_main_cont_main, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_main_cont_main, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_main_cont_main, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_main_cont_main, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_cont_main, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_btn_todolist
    ui->screen_main_btn_todolist = lv_btn_create(ui->screen_main_cont_main);
    ui->screen_main_btn_todolist_label = lv_label_create(ui->screen_main_btn_todolist);
    lv_label_set_text(ui->screen_main_btn_todolist_label, "");
    lv_label_set_long_mode(ui->screen_main_btn_todolist_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_main_btn_todolist_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_main_btn_todolist, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_main_btn_todolist_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_main_btn_todolist, 5, 32);
    lv_obj_set_size(ui->screen_main_btn_todolist, 39, 54);

    //Write style for screen_main_btn_todolist, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_todolist, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_main_btn_todolist, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_btn_todolist, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_todolist, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_main_btn_todolist, &_AAA_main_todo_0905_39x54, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_main_btn_todolist, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_main_btn_todolist, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_main_btn_todolist, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_btn_todolist, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_main_btn_todolist, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_main_btn_todolist, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_main_btn_todolist, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_todolist, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_main_btn_todolist, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_main_btn_todolist, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_todolist, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_src(ui->screen_main_btn_todolist, &_AAA_main_todo_0905_39x54, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_opa(ui->screen_main_btn_todolist, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_main_btn_todolist, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_main_btn_todolist, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_main_btn_todolist, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_main_btn_todolist, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write style for screen_main_btn_todolist, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_todolist, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_border_width(ui->screen_main_btn_todolist, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_radius(ui->screen_main_btn_todolist, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_todolist, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->screen_main_btn_todolist, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->screen_main_btn_todolist, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->screen_main_btn_todolist, 255, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for screen_main_btn_todolist, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_todolist, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(ui->screen_main_btn_todolist, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_bg_grad_dir(ui->screen_main_btn_todolist, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_border_width(ui->screen_main_btn_todolist, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_radius(ui->screen_main_btn_todolist, 5, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_todolist, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_main_btn_todolist, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->screen_main_btn_todolist, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->screen_main_btn_todolist, 255, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for screen_main_btn_todolist, Part: LV_PART_MAIN, State: LV_STATE_DISABLED.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_todolist, 255, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_bg_color(ui->screen_main_btn_todolist, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_bg_grad_dir(ui->screen_main_btn_todolist, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_border_width(ui->screen_main_btn_todolist, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_radius(ui->screen_main_btn_todolist, 5, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_todolist, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_text_color(ui->screen_main_btn_todolist, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_text_font(ui->screen_main_btn_todolist, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_text_opa(ui->screen_main_btn_todolist, 255, LV_PART_MAIN|LV_STATE_DISABLED);

    //Write codes screen_main_btn_done
    ui->screen_main_btn_done = lv_btn_create(ui->screen_main_cont_main);
    ui->screen_main_btn_done_label = lv_label_create(ui->screen_main_btn_done);
    lv_label_set_text(ui->screen_main_btn_done_label, "");
    lv_label_set_long_mode(ui->screen_main_btn_done_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_main_btn_done_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_main_btn_done, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_main_btn_done_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_main_btn_done, 33, 30);
    lv_obj_set_size(ui->screen_main_btn_done, 45, 54);

    //Write style for screen_main_btn_done, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_done, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_main_btn_done, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_btn_done, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_done, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_main_btn_done, &_AAA_main_donelist_0905_45x54, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_main_btn_done, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_main_btn_done, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_main_btn_done, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_btn_done, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_main_btn_done, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_main_btn_done, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_main_btn_done, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_done, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_main_btn_done, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_main_btn_done, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_done, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_src(ui->screen_main_btn_done, &_AAA_main_donelist_0905_45x54, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_opa(ui->screen_main_btn_done, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_main_btn_done, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_main_btn_done, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_main_btn_done, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_main_btn_done, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write style for screen_main_btn_done, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_done, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_border_width(ui->screen_main_btn_done, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_radius(ui->screen_main_btn_done, 5, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_done, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->screen_main_btn_done, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->screen_main_btn_done, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->screen_main_btn_done, 255, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for screen_main_btn_done, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_done, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(ui->screen_main_btn_done, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_bg_grad_dir(ui->screen_main_btn_done, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_border_width(ui->screen_main_btn_done, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_radius(ui->screen_main_btn_done, 5, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_done, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_main_btn_done, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->screen_main_btn_done, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->screen_main_btn_done, 255, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for screen_main_btn_done, Part: LV_PART_MAIN, State: LV_STATE_DISABLED.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_done, 255, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_bg_color(ui->screen_main_btn_done, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_bg_grad_dir(ui->screen_main_btn_done, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_border_width(ui->screen_main_btn_done, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_radius(ui->screen_main_btn_done, 5, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_done, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_text_color(ui->screen_main_btn_done, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_text_font(ui->screen_main_btn_done, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_text_opa(ui->screen_main_btn_done, 255, LV_PART_MAIN|LV_STATE_DISABLED);

    //Write codes screen_main_btn_timer
    ui->screen_main_btn_timer = lv_btn_create(ui->screen_main_cont_main);
    ui->screen_main_btn_timer_label = lv_label_create(ui->screen_main_btn_timer);
    lv_label_set_text(ui->screen_main_btn_timer_label, "");
    lv_label_set_long_mode(ui->screen_main_btn_timer_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_main_btn_timer_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_main_btn_timer, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_main_btn_timer_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_main_btn_timer, 66, 26);
    lv_obj_set_size(ui->screen_main_btn_timer, 43, 55);

    //Write style for screen_main_btn_timer, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_timer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_main_btn_timer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_btn_timer, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_timer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_main_btn_timer, &_AAA_main_time_0905_43x55, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_main_btn_timer, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_main_btn_timer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_main_btn_timer, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_btn_timer, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_main_btn_timer, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_main_btn_timer, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_main_btn_timer, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_timer, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_main_btn_timer, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_main_btn_timer, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_timer, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_src(ui->screen_main_btn_timer, &_AAA_main_time_0905_43x55, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_opa(ui->screen_main_btn_timer, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_main_btn_timer, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_main_btn_timer, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_main_btn_timer, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_main_btn_timer, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_main_btn_ai
    ui->screen_main_btn_ai = lv_btn_create(ui->screen_main_cont_main);
    ui->screen_main_btn_ai_label = lv_label_create(ui->screen_main_btn_ai);
    lv_label_set_text(ui->screen_main_btn_ai_label, "");
    lv_label_set_long_mode(ui->screen_main_btn_ai_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_main_btn_ai_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_main_btn_ai, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_main_btn_ai_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_main_btn_ai, 128, 32);
    lv_obj_set_size(ui->screen_main_btn_ai, 36, 58);

    //Write style for screen_main_btn_ai, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_ai, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_main_btn_ai, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_btn_ai, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_ai, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_main_btn_ai, &_AAA_main_keep_0905_36x58, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_main_btn_ai, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_main_btn_ai, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_main_btn_ai, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_btn_ai, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_main_btn_ai, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_main_btn_ai, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_main_btn_ai, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_ai, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_main_btn_ai, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_main_btn_ai, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_ai, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_src(ui->screen_main_btn_ai, &_AAA_main_keep_0905_36x58, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_opa(ui->screen_main_btn_ai, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_main_btn_ai, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_main_btn_ai, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_main_btn_ai, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_main_btn_ai, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_main_btn_gashapon
    ui->screen_main_btn_gashapon = lv_btn_create(ui->screen_main_cont_main);
    ui->screen_main_btn_gashapon_label = lv_label_create(ui->screen_main_btn_gashapon);
    lv_label_set_text(ui->screen_main_btn_gashapon_label, "");
    lv_label_set_long_mode(ui->screen_main_btn_gashapon_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_main_btn_gashapon_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_main_btn_gashapon, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_main_btn_gashapon_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_main_btn_gashapon, 263, 33);
    lv_obj_set_size(ui->screen_main_btn_gashapon, 38, 58);

    //Write style for screen_main_btn_gashapon, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_gashapon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_main_btn_gashapon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_btn_gashapon, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_gashapon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_main_btn_gashapon, &_AAA_main_egg_0905_38x58, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_main_btn_gashapon, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_main_btn_gashapon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_main_btn_gashapon, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_btn_gashapon, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_main_btn_gashapon, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_main_btn_gashapon, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_main_btn_gashapon, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_gashapon, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_main_btn_gashapon, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_main_btn_gashapon, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_gashapon, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_src(ui->screen_main_btn_gashapon, &_AAA_main_egg_0905_38x58, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_opa(ui->screen_main_btn_gashapon, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_main_btn_gashapon, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_main_btn_gashapon, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_main_btn_gashapon, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_main_btn_gashapon, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_main_btn_word
    ui->screen_main_btn_word = lv_btn_create(ui->screen_main_cont_main);
    ui->screen_main_btn_word_label = lv_label_create(ui->screen_main_btn_word);
    lv_label_set_text(ui->screen_main_btn_word_label, "");
    lv_label_set_long_mode(ui->screen_main_btn_word_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_main_btn_word_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_main_btn_word, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_main_btn_word_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_main_btn_word, 94, 32);
    lv_obj_set_size(ui->screen_main_btn_word, 69, 51);

    //Write style for screen_main_btn_word, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_word, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_main_btn_word, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_btn_word, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_word, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_main_btn_word, &_AAA_main_book_0905_69x51, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_main_btn_word, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_main_btn_word, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_main_btn_word, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_btn_word, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_main_btn_word, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_main_btn_word, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_main_btn_word, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_word, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_main_btn_word, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_main_btn_word, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_word, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_src(ui->screen_main_btn_word, &_AAA_main_book_0905_69x51, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_opa(ui->screen_main_btn_word, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_main_btn_word, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_main_btn_word, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_main_btn_word, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_main_btn_word, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_main_btn_fun
    ui->screen_main_btn_fun = lv_btn_create(ui->screen_main_cont_main);
    ui->screen_main_btn_fun_label = lv_label_create(ui->screen_main_btn_fun);
    lv_label_set_text(ui->screen_main_btn_fun_label, "");
    lv_label_set_long_mode(ui->screen_main_btn_fun_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_main_btn_fun_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_main_btn_fun, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_main_btn_fun_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_main_btn_fun, 171, 33);
    lv_obj_set_size(ui->screen_main_btn_fun, 55, 54);

    //Write style for screen_main_btn_fun, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_fun, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_main_btn_fun, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_btn_fun, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_fun, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_main_btn_fun, &_AAA_main_innovationbox_0905_55x54, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_main_btn_fun, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_main_btn_fun, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_main_btn_fun, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_btn_fun, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_main_btn_fun, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_main_btn_fun, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_main_btn_fun, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_fun, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_main_btn_fun, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_main_btn_fun, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_fun, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_src(ui->screen_main_btn_fun, &_AAA_main_innovationbox_0905_55x54, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_opa(ui->screen_main_btn_fun, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_main_btn_fun, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_main_btn_fun, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_main_btn_fun, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_main_btn_fun, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_main_btn_photo
    ui->screen_main_btn_photo = lv_btn_create(ui->screen_main_cont_main);
    ui->screen_main_btn_photo_label = lv_label_create(ui->screen_main_btn_photo);
    lv_label_set_text(ui->screen_main_btn_photo_label, "");
    lv_label_set_long_mode(ui->screen_main_btn_photo_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_main_btn_photo_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_main_btn_photo, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_main_btn_photo_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_main_btn_photo, 251, 32);
    lv_obj_set_size(ui->screen_main_btn_photo, 43, 55);

    //Write style for screen_main_btn_photo, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_photo, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_main_btn_photo, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_btn_photo, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_photo, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_main_btn_photo, &_AAA_main_photo_0905_43x55, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_main_btn_photo, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_main_btn_photo, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_main_btn_photo, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_btn_photo, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_main_btn_photo, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_main_btn_photo, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_main_btn_photo, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_photo, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_main_btn_photo, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_main_btn_photo, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_photo, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_src(ui->screen_main_btn_photo, &_AAA_main_photo_0905_43x55, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_opa(ui->screen_main_btn_photo, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_main_btn_photo, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_main_btn_photo, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_main_btn_photo, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_main_btn_photo, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_main_btn_game
    ui->screen_main_btn_game = lv_btn_create(ui->screen_main_cont_main);
    ui->screen_main_btn_game_label = lv_label_create(ui->screen_main_btn_game);
    lv_label_set_text(ui->screen_main_btn_game_label, "");
    lv_label_set_long_mode(ui->screen_main_btn_game_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_main_btn_game_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_main_btn_game, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_main_btn_game_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_main_btn_game, 223, -4);
    lv_obj_set_size(ui->screen_main_btn_game, 46, 54);

    //Write style for screen_main_btn_game, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_game, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_main_btn_game, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_btn_game, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_game, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_main_btn_game, &_AAA_main_play_0905_46x54, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_main_btn_game, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_main_btn_game, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_main_btn_game, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_btn_game, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_main_btn_game, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_main_btn_game, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_main_btn_game, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_game, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_main_btn_game, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_main_btn_game, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_game, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_src(ui->screen_main_btn_game, &_AAA_main_play_0905_46x54, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_opa(ui->screen_main_btn_game, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_main_btn_game, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_main_btn_game, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_main_btn_game, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_main_btn_game, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_main_btn_setting
    ui->screen_main_btn_setting = lv_btn_create(ui->screen_main_cont_main);
    ui->screen_main_btn_setting_label = lv_label_create(ui->screen_main_btn_setting);
    lv_label_set_text(ui->screen_main_btn_setting_label, "");
    lv_label_set_long_mode(ui->screen_main_btn_setting_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_main_btn_setting_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_main_btn_setting, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_main_btn_setting_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_main_btn_setting, 212, 35);
    lv_obj_set_size(ui->screen_main_btn_setting, 46, 54);

    //Write style for screen_main_btn_setting, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_main_btn_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_btn_setting, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_main_btn_setting, &_AAA_main_setting_0905_46x54, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_main_btn_setting, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_main_btn_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_main_btn_setting, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_btn_setting, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_main_btn_setting, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_main_btn_setting, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_main_btn_setting, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_setting, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_main_btn_setting, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_main_btn_setting, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_setting, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_src(ui->screen_main_btn_setting, &_AAA_main_setting_0905_46x54, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_opa(ui->screen_main_btn_setting, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_main_btn_setting, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_main_btn_setting, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_main_btn_setting, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_main_btn_setting, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_main_label_donenum
    ui->screen_main_label_donenum = lv_label_create(ui->screen_main);
    lv_label_set_text(ui->screen_main_label_donenum, "0");
    lv_label_set_long_mode(ui->screen_main_label_donenum, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_main_label_donenum, 46, 133);
    lv_obj_set_size(ui->screen_main_label_donenum, 22, 12);

    //Write style for screen_main_label_donenum, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_main_label_donenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_label_donenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_main_label_donenum, lv_color_hex(0x2b66a7), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_label_donenum, &lv_font_Acme_Regular_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_main_label_donenum, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_main_label_donenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_main_label_donenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_main_label_donenum, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_main_label_donenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_main_label_donenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_main_label_donenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_main_label_donenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_main_label_donenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_label_donenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_label_todonum
    ui->screen_main_label_todonum = lv_label_create(ui->screen_main);
    lv_label_set_text(ui->screen_main_label_todonum, "0");
    lv_label_set_long_mode(ui->screen_main_label_todonum, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_main_label_todonum, 46, 151);
    lv_obj_set_size(ui->screen_main_label_todonum, 22, 12);

    //Write style for screen_main_label_todonum, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_main_label_todonum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_label_todonum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_main_label_todonum, lv_color_hex(0x2b66a7), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_label_todonum, &lv_font_Acme_Regular_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_main_label_todonum, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_main_label_todonum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_main_label_todonum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_main_label_todonum, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_main_label_todonum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_main_label_todonum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_main_label_todonum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_main_label_todonum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_main_label_todonum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_label_todonum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_btn_realtime
    ui->screen_main_btn_realtime = lv_btn_create(ui->screen_main);
    ui->screen_main_btn_realtime_label = lv_label_create(ui->screen_main_btn_realtime);
    lv_label_set_text(ui->screen_main_btn_realtime_label, "10:35");
    lv_label_set_long_mode(ui->screen_main_btn_realtime_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_main_btn_realtime_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_main_btn_realtime, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_main_btn_realtime_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_main_btn_realtime, 272, 145);
    lv_obj_set_size(ui->screen_main_btn_realtime, 45, 16);

    //Write style for screen_main_btn_realtime, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_realtime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_main_btn_realtime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_btn_realtime, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_realtime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_main_btn_realtime, lv_color_hex(0x0a2a4c), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_btn_realtime, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_main_btn_realtime, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_main_btn_realtime, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_main_btn_realtime, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_realtime, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_main_btn_realtime, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_main_btn_realtime, 10, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_realtime, 11, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_main_btn_realtime, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_main_btn_realtime, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_main_btn_realtime, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_main_btn_realtime, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_main_btn_realtime, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_main_btn_realtime, lv_color_hex(0x0a2a4c), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_main_btn_realtime, &lv_font_SourceHanSansSC_Medium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_main_btn_realtime, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_main_img_todo_icon
    ui->screen_main_img_todo_icon = lv_img_create(ui->screen_main);
    lv_obj_add_flag(ui->screen_main_img_todo_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_main_img_todo_icon, &_AAA_small_todo_alpha_16x14);
    lv_img_set_pivot(ui->screen_main_img_todo_icon, 50,50);
    lv_img_set_angle(ui->screen_main_img_todo_icon, 0);
    lv_obj_set_pos(ui->screen_main_img_todo_icon, 75, 133);
    lv_obj_set_size(ui->screen_main_img_todo_icon, 16, 14);

    //Write style for screen_main_img_todo_icon, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_main_img_todo_icon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_main_img_todo_icon, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_img_todo_icon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_main_img_todo_icon, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_img_fun_icon
    ui->screen_main_img_fun_icon = lv_img_create(ui->screen_main);
    lv_obj_add_flag(ui->screen_main_img_fun_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_main_img_fun_icon, &_AAA_smallfun_alpha_19x18);
    lv_img_set_pivot(ui->screen_main_img_fun_icon, 50,50);
    lv_img_set_angle(ui->screen_main_img_fun_icon, 0);
    lv_obj_set_pos(ui->screen_main_img_fun_icon, 185, 131);
    lv_obj_set_size(ui->screen_main_img_fun_icon, 19, 18);

    //Write style for screen_main_img_fun_icon, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_main_img_fun_icon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_main_img_fun_icon, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_img_fun_icon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_main_img_fun_icon, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_img_setting_icon
    ui->screen_main_img_setting_icon = lv_img_create(ui->screen_main);
    lv_obj_add_flag(ui->screen_main_img_setting_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_main_img_setting_icon, &_AAA_small_setting_alpha_15x15);
    lv_img_set_pivot(ui->screen_main_img_setting_icon, 50,50);
    lv_img_set_angle(ui->screen_main_img_setting_icon, 0);
    lv_obj_set_pos(ui->screen_main_img_setting_icon, 240, 133);
    lv_obj_set_size(ui->screen_main_img_setting_icon, 15, 15);

    //Write style for screen_main_img_setting_icon, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_main_img_setting_icon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_main_img_setting_icon, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_img_setting_icon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_main_img_setting_icon, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_img_ai_icon
    ui->screen_main_img_ai_icon = lv_img_create(ui->screen_main);
    lv_obj_add_flag(ui->screen_main_img_ai_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_main_img_ai_icon, &_AAA_small_keep_0905_alpha_20x16);
    lv_img_set_pivot(ui->screen_main_img_ai_icon, 50,50);
    lv_img_set_angle(ui->screen_main_img_ai_icon, 0);
    lv_obj_set_pos(ui->screen_main_img_ai_icon, 130, 133);
    lv_obj_set_size(ui->screen_main_img_ai_icon, 20, 16);

    //Write style for screen_main_img_ai_icon, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_main_img_ai_icon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_main_img_ai_icon, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_img_ai_icon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_main_img_ai_icon, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_img_timer_icon
    ui->screen_main_img_timer_icon = lv_img_create(ui->screen_main);
    lv_obj_add_flag(ui->screen_main_img_timer_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_main_img_timer_icon, &_AAA_small_timer_alpha_15x15);
    lv_img_set_pivot(ui->screen_main_img_timer_icon, 50,50);
    lv_img_set_angle(ui->screen_main_img_timer_icon, 0);
    lv_obj_set_pos(ui->screen_main_img_timer_icon, 113, 133);
    lv_obj_set_size(ui->screen_main_img_timer_icon, 15, 15);

    //Write style for screen_main_img_timer_icon, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_main_img_timer_icon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_main_img_timer_icon, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_img_timer_icon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_main_img_timer_icon, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_img_book_icon
    ui->screen_main_img_book_icon = lv_img_create(ui->screen_main);
    lv_obj_add_flag(ui->screen_main_img_book_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_main_img_book_icon, &_AAA_small_words_alpha_17x15);
    lv_img_set_pivot(ui->screen_main_img_book_icon, 50,50);
    lv_img_set_angle(ui->screen_main_img_book_icon, 0);
    lv_obj_set_pos(ui->screen_main_img_book_icon, 169, 133);
    lv_obj_set_size(ui->screen_main_img_book_icon, 17, 15);

    //Write style for screen_main_img_book_icon, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_main_img_book_icon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_main_img_book_icon, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_img_book_icon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_main_img_book_icon, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_img_done_icon
    ui->screen_main_img_done_icon = lv_img_create(ui->screen_main);
    lv_obj_add_flag(ui->screen_main_img_done_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_main_img_done_icon, &_AAA_small_Done_alpha_15x15);
    lv_img_set_pivot(ui->screen_main_img_done_icon, 50,50);
    lv_img_set_angle(ui->screen_main_img_done_icon, 0);
    lv_obj_set_pos(ui->screen_main_img_done_icon, 93, 133);
    lv_obj_set_size(ui->screen_main_img_done_icon, 15, 15);

    //Write style for screen_main_img_done_icon, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_main_img_done_icon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_main_img_done_icon, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_img_done_icon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_main_img_done_icon, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_img_point_icon
    ui->screen_main_img_point_icon = lv_img_create(ui->screen_main);
    lv_obj_add_flag(ui->screen_main_img_point_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_main_img_point_icon, &_AAA_main_dian_alpha_6x6);
    lv_img_set_pivot(ui->screen_main_img_point_icon, 50,50);
    lv_img_set_angle(ui->screen_main_img_point_icon, 0);
    lv_obj_set_pos(ui->screen_main_img_point_icon, 79, 155);
    lv_obj_set_size(ui->screen_main_img_point_icon, 6, 6);

    //Write style for screen_main_img_point_icon, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_main_img_point_icon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_main_img_point_icon, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_img_point_icon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_main_img_point_icon, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_label_chosen
    ui->screen_main_label_chosen = lv_label_create(ui->screen_main);
    lv_label_set_text(ui->screen_main_label_chosen, "TODO");
    lv_label_set_long_mode(ui->screen_main_label_chosen, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_main_label_chosen, 104, 10);
    lv_obj_set_size(ui->screen_main_label_chosen, 122, 18);

    //Write style for screen_main_label_chosen, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_main_label_chosen, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_label_chosen, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_main_label_chosen, lv_color_hex(0x0a2a4c), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_label_chosen, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_main_label_chosen, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_main_label_chosen, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_main_label_chosen, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_main_label_chosen, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_main_label_chosen, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_main_label_chosen, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_main_label_chosen, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_main_label_chosen, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_main_label_chosen, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_label_chosen, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_arc_battery
    ui->screen_main_arc_battery = lv_arc_create(ui->screen_main);
    lv_arc_set_mode(ui->screen_main_arc_battery, LV_ARC_MODE_NORMAL);
    lv_arc_set_range(ui->screen_main_arc_battery, 0, 100);
    lv_arc_set_bg_angles(ui->screen_main_arc_battery, 135, 45);
    lv_arc_set_value(ui->screen_main_arc_battery, 70);
    lv_arc_set_rotation(ui->screen_main_arc_battery, 0);
    lv_obj_set_style_arc_rounded(ui->screen_main_arc_battery, 0,  LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_rounded(ui->screen_main_arc_battery, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(ui->screen_main_arc_battery, 261, -6);
    lv_obj_set_size(ui->screen_main_arc_battery, 73, 54);

    //Write style for screen_main_arc_battery, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_main_arc_battery, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_main_arc_battery, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui->screen_main_arc_battery, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui->screen_main_arc_battery, 113, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui->screen_main_arc_battery, lv_color_hex(0x8fb0d3), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_arc_battery, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_main_arc_battery, 15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_main_arc_battery, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_main_arc_battery, 29, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_main_arc_battery, 15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_arc_battery, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_main_arc_battery, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_main_arc_battery, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(ui->screen_main_arc_battery, lv_color_hex(0xf6f6f6), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_grad_dir(ui->screen_main_arc_battery, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_main_arc_battery, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_arc_width(ui->screen_main_arc_battery, 12, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_arc_opa(ui->screen_main_arc_battery, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_arc_color(ui->screen_main_arc_battery, lv_color_hex(0xe6e6e6), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_main_arc_battery, 6, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_pad_top(ui->screen_main_arc_battery, 20, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_pad_bottom(ui->screen_main_arc_battery, 20, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_pad_left(ui->screen_main_arc_battery, 20, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_pad_right(ui->screen_main_arc_battery, 20, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_main_arc_battery, 0, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write style for screen_main_arc_battery, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_arc_width(ui->screen_main_arc_battery, 2, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui->screen_main_arc_battery, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui->screen_main_arc_battery, lv_color_hex(0x0a2a4c), LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for screen_main_arc_battery, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_main_arc_battery, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui->screen_main_arc_battery, 0, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_main_img_select_timing
    ui->screen_main_img_select_timing = lv_img_create(ui->screen_main);
    lv_obj_add_flag(ui->screen_main_img_select_timing, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_main_img_select_timing, &_AAA_select_alpha_46x14);
    lv_img_set_pivot(ui->screen_main_img_select_timing, 50,50);
    lv_img_set_angle(ui->screen_main_img_select_timing, 0);
    lv_obj_set_pos(ui->screen_main_img_select_timing, 269, 148);
    lv_obj_set_size(ui->screen_main_img_select_timing, 46, 14);

    //Write style for screen_main_img_select_timing, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_main_img_select_timing, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_main_img_select_timing, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_img_select_timing, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_main_img_select_timing, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_img_select
    ui->screen_main_img_select = lv_img_create(ui->screen_main);
    lv_obj_add_flag(ui->screen_main_img_select, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_main_img_select, &_AAA_select_alpha_56x23);
    lv_img_set_pivot(ui->screen_main_img_select, 50,50);
    lv_img_set_angle(ui->screen_main_img_select, 0);
    lv_obj_set_pos(ui->screen_main_img_select, 132, 82);
    lv_obj_set_size(ui->screen_main_img_select, 56, 23);

    //Write style for screen_main_img_select, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_main_img_select, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_main_img_select, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_img_select, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_main_img_select, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_img_photo_icon
    ui->screen_main_img_photo_icon = lv_img_create(ui->screen_main);
    lv_obj_add_flag(ui->screen_main_img_photo_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_main_img_photo_icon, &_AAA_small_photo_alpha_15x14);
    lv_img_set_pivot(ui->screen_main_img_photo_icon, 50,50);
    lv_img_set_angle(ui->screen_main_img_photo_icon, 0);
    lv_obj_set_pos(ui->screen_main_img_photo_icon, 205, 133);
    lv_obj_set_size(ui->screen_main_img_photo_icon, 15, 14);

    //Write style for screen_main_img_photo_icon, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_main_img_photo_icon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_main_img_photo_icon, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_img_photo_icon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_main_img_photo_icon, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_cat_left
    ui->screen_main_cat_left = lv_animimg_create(ui->screen_main);
    lv_animimg_set_src(ui->screen_main_cat_left, (const void **) screen_main_cat_left_imgs, 30, false);
    lv_animimg_set_duration(ui->screen_main_cat_left, 70*30);
    lv_animimg_set_repeat_count(ui->screen_main_cat_left, LV_ANIM_REPEAT_INFINITE);
    lv_animimg_start(ui->screen_main_cat_left);
    lv_obj_set_pos(ui->screen_main_cat_left, 0, 50);
    lv_obj_set_size(ui->screen_main_cat_left, 55, 55);
    lv_obj_add_flag(ui->screen_main_cat_left, LV_OBJ_FLAG_HIDDEN);

    //Write codes screen_main_cat_right
    ui->screen_main_cat_right = lv_animimg_create(ui->screen_main);
    lv_animimg_set_src(ui->screen_main_cat_right, (const void **) screen_main_cat_right_imgs, 30, false);
    lv_animimg_set_duration(ui->screen_main_cat_right, 70*30);
    lv_animimg_set_repeat_count(ui->screen_main_cat_right, LV_ANIM_REPEAT_INFINITE);
    lv_animimg_start(ui->screen_main_cat_right);
    lv_obj_set_pos(ui->screen_main_cat_right, 265, 50);
    lv_obj_set_size(ui->screen_main_cat_right, 55, 55);
    lv_obj_add_flag(ui->screen_main_cat_right, LV_OBJ_FLAG_HIDDEN);

    //Write codes screen_main_img_gash_icon
    ui->screen_main_img_gash_icon = lv_img_create(ui->screen_main);
    lv_obj_add_flag(ui->screen_main_img_gash_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_main_img_gash_icon, &_AAA_small_egg_0905_alpha_16x14);
    lv_img_set_pivot(ui->screen_main_img_gash_icon, 50,50);
    lv_img_set_angle(ui->screen_main_img_gash_icon, 0);
    lv_obj_set_pos(ui->screen_main_img_gash_icon, 151, 133);
    lv_obj_set_size(ui->screen_main_img_gash_icon, 16, 14);

    //Write style for screen_main_img_gash_icon, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_main_img_gash_icon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_main_img_gash_icon, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_img_gash_icon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_main_img_gash_icon, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_img_game_icon
    ui->screen_main_img_game_icon = lv_img_create(ui->screen_main);
    lv_obj_add_flag(ui->screen_main_img_game_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_main_img_game_icon, &_AAA_small_game_0905_alpha_16x14);
    lv_img_set_pivot(ui->screen_main_img_game_icon, 50,50);
    lv_img_set_angle(ui->screen_main_img_game_icon, 0);
    lv_obj_set_pos(ui->screen_main_img_game_icon, 223, 133);
    lv_obj_set_size(ui->screen_main_img_game_icon, 16, 14);

    //Write style for screen_main_img_game_icon, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_main_img_game_icon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_main_img_game_icon, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_img_game_icon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_main_img_game_icon, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_main.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_main);

    //Init events for screen.
    events_init_screen_main(ui);
}
