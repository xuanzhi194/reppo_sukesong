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



void setup_scr_screen_todolist(lv_ui *ui)
{
    //Write codes screen_todolist
    ui->screen_todolist = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_todolist, 320, 170);
    lv_obj_set_scrollbar_mode(ui->screen_todolist, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_todolist, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_todolist, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_todolist, &_AAA_todolist_bg_320x170, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_todolist, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_todolist, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_todolist_btn_back
    ui->screen_todolist_btn_back = lv_btn_create(ui->screen_todolist);
    ui->screen_todolist_btn_back_label = lv_label_create(ui->screen_todolist_btn_back);
    lv_label_set_text(ui->screen_todolist_btn_back_label, "");
    lv_label_set_long_mode(ui->screen_todolist_btn_back_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_todolist_btn_back_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_todolist_btn_back, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_todolist_btn_back_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_todolist_btn_back, 296, 12);
    lv_obj_set_size(ui->screen_todolist_btn_back, 17, 18);

    //Write style for screen_todolist_btn_back, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_todolist_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_todolist_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_todolist_btn_back, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_todolist_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_todolist_btn_back, lv_color_hex(0x2f6098), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_todolist_btn_back, &lv_font_montserratMedium_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_todolist_btn_back, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_todolist_btn_back, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_todolist_btn_back, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_todolist_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_todolist_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_todolist_btn_back, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_todolist_btn_back, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_todolist_btn_back, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_todolist_btn_back, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_todolist_btn_back, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_todolist_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_todolist_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_todolist_btn_back, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_todolist_btn_back, &lv_font_montserratMedium_20, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_todolist_btn_back, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_todolist_btn_todo1
    ui->screen_todolist_btn_todo1 = lv_btn_create(ui->screen_todolist);
    ui->screen_todolist_btn_todo1_label = lv_label_create(ui->screen_todolist_btn_todo1);
    lv_label_set_text(ui->screen_todolist_btn_todo1_label, "reppo");
    lv_label_set_long_mode(ui->screen_todolist_btn_todo1_label, LV_LABEL_LONG_CLIP);
    lv_obj_align(ui->screen_todolist_btn_todo1_label, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_todolist_btn_todo1, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_todolist_btn_todo1_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_todolist_btn_todo1, 19, 51);
    lv_obj_set_size(ui->screen_todolist_btn_todo1, 130, 20);

    //Write style for screen_todolist_btn_todo1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_todolist_btn_todo1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_todolist_btn_todo1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_todolist_btn_todo1, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_todolist_btn_todo1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_todolist_btn_todo1, lv_color_hex(0x2f6098), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_todolist_btn_todo1, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_todolist_btn_todo1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_todolist_btn_todo1, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_todolist_btn_todo1, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_todolist_btn_todo1, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_todolist_btn_todo1, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_todolist_btn_todo1, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_todolist_btn_todo1, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_todolist_btn_todo1, lv_color_hex(0x022243), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_todolist_btn_todo1, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_todolist_btn_todo1, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_todolist_btn_todo2
    ui->screen_todolist_btn_todo2 = lv_btn_create(ui->screen_todolist);
    ui->screen_todolist_btn_todo2_label = lv_label_create(ui->screen_todolist_btn_todo2);
    lv_label_set_text(ui->screen_todolist_btn_todo2_label, "hello");
    lv_label_set_long_mode(ui->screen_todolist_btn_todo2_label, LV_LABEL_LONG_CLIP);
    lv_obj_align(ui->screen_todolist_btn_todo2_label, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_todolist_btn_todo2, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_todolist_btn_todo2_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_todolist_btn_todo2, 177, 51);
    lv_obj_set_size(ui->screen_todolist_btn_todo2, 130, 20);

    //Write style for screen_todolist_btn_todo2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_todolist_btn_todo2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_todolist_btn_todo2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_todolist_btn_todo2, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_todolist_btn_todo2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_todolist_btn_todo2, lv_color_hex(0x2f6098), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_todolist_btn_todo2, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_todolist_btn_todo2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_todolist_btn_todo2, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_todolist_btn_todo2, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_todolist_btn_todo2, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_todolist_btn_todo2, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_todolist_btn_todo2, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_todolist_btn_todo2, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_todolist_btn_todo2, lv_color_hex(0x022243), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_todolist_btn_todo2, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_todolist_btn_todo2, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_todolist_btn_todo3
    ui->screen_todolist_btn_todo3 = lv_btn_create(ui->screen_todolist);
    ui->screen_todolist_btn_todo3_label = lv_label_create(ui->screen_todolist_btn_todo3);
    lv_label_set_text(ui->screen_todolist_btn_todo3_label, "world");
    lv_label_set_long_mode(ui->screen_todolist_btn_todo3_label, LV_LABEL_LONG_CLIP);
    lv_obj_align(ui->screen_todolist_btn_todo3_label, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_todolist_btn_todo3, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_todolist_btn_todo3_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_todolist_btn_todo3, 19, 104);
    lv_obj_set_size(ui->screen_todolist_btn_todo3, 130, 20);

    //Write style for screen_todolist_btn_todo3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_todolist_btn_todo3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_todolist_btn_todo3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_todolist_btn_todo3, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_todolist_btn_todo3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_todolist_btn_todo3, lv_color_hex(0x2f6098), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_todolist_btn_todo3, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_todolist_btn_todo3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_todolist_btn_todo3, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_todolist_btn_todo3, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_todolist_btn_todo3, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_todolist_btn_todo3, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_todolist_btn_todo3, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_todolist_btn_todo3, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_todolist_btn_todo3, lv_color_hex(0x022243), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_todolist_btn_todo3, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_todolist_btn_todo3, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_todolist_btn_todo4
    ui->screen_todolist_btn_todo4 = lv_btn_create(ui->screen_todolist);
    ui->screen_todolist_btn_todo4_label = lv_label_create(ui->screen_todolist_btn_todo4);
    lv_label_set_text(ui->screen_todolist_btn_todo4_label, "thanks");
    lv_label_set_long_mode(ui->screen_todolist_btn_todo4_label, LV_LABEL_LONG_CLIP);
    lv_obj_align(ui->screen_todolist_btn_todo4_label, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_todolist_btn_todo4, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_todolist_btn_todo4_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_todolist_btn_todo4, 177, 104);
    lv_obj_set_size(ui->screen_todolist_btn_todo4, 130, 20);

    //Write style for screen_todolist_btn_todo4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_todolist_btn_todo4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_todolist_btn_todo4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_todolist_btn_todo4, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_todolist_btn_todo4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_todolist_btn_todo4, lv_color_hex(0x2f6098), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_todolist_btn_todo4, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_todolist_btn_todo4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_todolist_btn_todo4, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_todolist_btn_todo4, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_todolist_btn_todo4, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_todolist_btn_todo4, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_todolist_btn_todo4, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_todolist_btn_todo4, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_todolist_btn_todo4, lv_color_hex(0x022243), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_todolist_btn_todo4, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_todolist_btn_todo4, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_todolist_label_indexcut
    ui->screen_todolist_label_indexcut = lv_label_create(ui->screen_todolist);
    lv_label_set_text(ui->screen_todolist_label_indexcut, "/");
    lv_label_set_long_mode(ui->screen_todolist_label_indexcut, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_todolist_label_indexcut, 152, 146);
    lv_obj_set_size(ui->screen_todolist_label_indexcut, 17, 14);

    //Write style for screen_todolist_label_indexcut, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_todolist_label_indexcut, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_todolist_label_indexcut, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_todolist_label_indexcut, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_todolist_label_indexcut, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_todolist_label_indexcut, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_todolist_label_indexcut, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_todolist_label_indexcut, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_todolist_label_indexcut, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_todolist_label_indexcut, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_todolist_label_indexcut, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_todolist_label_indexcut, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_todolist_label_indexcut, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_todolist_label_indexcut, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_todolist_label_indexcut, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_todolist_label_indexcur
    ui->screen_todolist_label_indexcur = lv_label_create(ui->screen_todolist);
    lv_label_set_text(ui->screen_todolist_label_indexcur, "1");
    lv_label_set_long_mode(ui->screen_todolist_label_indexcur, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_todolist_label_indexcur, 138, 146);
    lv_obj_set_size(ui->screen_todolist_label_indexcur, 20, 15);

    //Write style for screen_todolist_label_indexcur, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_todolist_label_indexcur, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_todolist_label_indexcur, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_todolist_label_indexcur, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_todolist_label_indexcur, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_todolist_label_indexcur, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_todolist_label_indexcur, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_todolist_label_indexcur, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_todolist_label_indexcur, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_todolist_label_indexcur, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_todolist_label_indexcur, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_todolist_label_indexcur, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_todolist_label_indexcur, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_todolist_label_indexcur, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_todolist_label_indexcur, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_todolist_label_indexall
    ui->screen_todolist_label_indexall = lv_label_create(ui->screen_todolist);
    lv_label_set_text(ui->screen_todolist_label_indexall, "1");
    lv_label_set_long_mode(ui->screen_todolist_label_indexall, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_todolist_label_indexall, 163, 146);
    lv_obj_set_size(ui->screen_todolist_label_indexall, 20, 15);

    //Write style for screen_todolist_label_indexall, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_todolist_label_indexall, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_todolist_label_indexall, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_todolist_label_indexall, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_todolist_label_indexall, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_todolist_label_indexall, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_todolist_label_indexall, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_todolist_label_indexall, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_todolist_label_indexall, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_todolist_label_indexall, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_todolist_label_indexall, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_todolist_label_indexall, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_todolist_label_indexall, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_todolist_label_indexall, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_todolist_label_indexall, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_todolist_label_donenum
    ui->screen_todolist_label_donenum = lv_label_create(ui->screen_todolist);
    lv_label_set_text(ui->screen_todolist_label_donenum, "0");
    lv_label_set_long_mode(ui->screen_todolist_label_donenum, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_todolist_label_donenum, 43, 8);
    lv_obj_set_size(ui->screen_todolist_label_donenum, 25, 15);

    //Write style for screen_todolist_label_donenum, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_todolist_label_donenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_todolist_label_donenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_todolist_label_donenum, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_todolist_label_donenum, &lv_font_Acme_Regular_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_todolist_label_donenum, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_todolist_label_donenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_todolist_label_donenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_todolist_label_donenum, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_todolist_label_donenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_todolist_label_donenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_todolist_label_donenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_todolist_label_donenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_todolist_label_donenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_todolist_label_donenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_todolist_label_todonum
    ui->screen_todolist_label_todonum = lv_label_create(ui->screen_todolist);
    lv_label_set_text(ui->screen_todolist_label_todonum, "0");
    lv_label_set_long_mode(ui->screen_todolist_label_todonum, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_todolist_label_todonum, 43, 20);
    lv_obj_set_size(ui->screen_todolist_label_todonum, 25, 15);

    //Write style for screen_todolist_label_todonum, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_todolist_label_todonum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_todolist_label_todonum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_todolist_label_todonum, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_todolist_label_todonum, &lv_font_Acme_Regular_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_todolist_label_todonum, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_todolist_label_todonum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_todolist_label_todonum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_todolist_label_todonum, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_todolist_label_todonum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_todolist_label_todonum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_todolist_label_todonum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_todolist_label_todonum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_todolist_label_todonum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_todolist_label_todonum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_todolist_btn_lastpage
    ui->screen_todolist_btn_lastpage = lv_btn_create(ui->screen_todolist);
    ui->screen_todolist_btn_lastpage_label = lv_label_create(ui->screen_todolist_btn_lastpage);
    lv_label_set_text(ui->screen_todolist_btn_lastpage_label, "");
    lv_label_set_long_mode(ui->screen_todolist_btn_lastpage_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_todolist_btn_lastpage_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_todolist_btn_lastpage, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_todolist_btn_lastpage_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_todolist_btn_lastpage, 7, 143);
    lv_obj_set_size(ui->screen_todolist_btn_lastpage, 95, 19);

    //Write style for screen_todolist_btn_lastpage, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_todolist_btn_lastpage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_todolist_btn_lastpage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_todolist_btn_lastpage, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_todolist_btn_lastpage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_todolist_btn_lastpage, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_todolist_btn_lastpage, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_todolist_btn_lastpage, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_todolist_btn_lastpage, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_todolist_btn_lastpage, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_todolist_btn_lastpage, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_todolist_btn_lastpage, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_todolist_btn_lastpage, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_todolist_btn_lastpage, 6, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_todolist_btn_lastpage, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_todolist_btn_lastpage, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_todolist_btn_lastpage, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_todolist_btn_lastpage, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_todolist_btn_lastpage, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_todolist_btn_lastpage, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_todolist_btn_lastpage, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_todolist_btn_lastpage, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_todolist_btn_nxtpage
    ui->screen_todolist_btn_nxtpage = lv_btn_create(ui->screen_todolist);
    ui->screen_todolist_btn_nxtpage_label = lv_label_create(ui->screen_todolist_btn_nxtpage);
    lv_label_set_text(ui->screen_todolist_btn_nxtpage_label, "");
    lv_label_set_long_mode(ui->screen_todolist_btn_nxtpage_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_todolist_btn_nxtpage_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_todolist_btn_nxtpage, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_todolist_btn_nxtpage_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_todolist_btn_nxtpage, 214, 144);
    lv_obj_set_size(ui->screen_todolist_btn_nxtpage, 97, 18);

    //Write style for screen_todolist_btn_nxtpage, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_todolist_btn_nxtpage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_todolist_btn_nxtpage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_todolist_btn_nxtpage, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_todolist_btn_nxtpage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_todolist_btn_nxtpage, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_todolist_btn_nxtpage, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_todolist_btn_nxtpage, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_todolist_btn_nxtpage, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_todolist_btn_nxtpage, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_todolist_btn_nxtpage, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_todolist_btn_nxtpage, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_todolist_btn_nxtpage, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_todolist_btn_nxtpage, 6, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_todolist_btn_nxtpage, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_todolist_btn_nxtpage, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_todolist_btn_nxtpage, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_todolist_btn_nxtpage, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_todolist_btn_nxtpage, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_todolist_btn_nxtpage, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_todolist_btn_nxtpage, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_todolist_btn_nxtpage, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_todolist_line_1
    ui->screen_todolist_line_1 = lv_line_create(ui->screen_todolist);
    static lv_point_t screen_todolist_line_1[] = {{0, 0},{300, 0},};
    lv_line_set_points(ui->screen_todolist_line_1, screen_todolist_line_1, 2);
    lv_obj_set_pos(ui->screen_todolist_line_1, 16, 62);
    lv_obj_set_size(ui->screen_todolist_line_1, 129, 1);

    //Write style for screen_todolist_line_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_line_width(ui->screen_todolist_line_1, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui->screen_todolist_line_1, lv_color_hex(0x7698bc), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui->screen_todolist_line_1, 169, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_todolist_line_2
    ui->screen_todolist_line_2 = lv_line_create(ui->screen_todolist);
    static lv_point_t screen_todolist_line_2[] = {{0, 0},{300, 0},};
    lv_line_set_points(ui->screen_todolist_line_2, screen_todolist_line_2, 2);
    lv_obj_set_pos(ui->screen_todolist_line_2, 173, 62);
    lv_obj_set_size(ui->screen_todolist_line_2, 127, 1);

    //Write style for screen_todolist_line_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_line_width(ui->screen_todolist_line_2, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui->screen_todolist_line_2, lv_color_hex(0x7698bc), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui->screen_todolist_line_2, 169, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_todolist_line_3
    ui->screen_todolist_line_3 = lv_line_create(ui->screen_todolist);
    static lv_point_t screen_todolist_line_3[] = {{0, 0},{300, 0},};
    lv_line_set_points(ui->screen_todolist_line_3, screen_todolist_line_3, 2);
    lv_obj_set_pos(ui->screen_todolist_line_3, 16, 115);
    lv_obj_set_size(ui->screen_todolist_line_3, 129, 1);

    //Write style for screen_todolist_line_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_line_width(ui->screen_todolist_line_3, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui->screen_todolist_line_3, lv_color_hex(0x7698bc), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui->screen_todolist_line_3, 169, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_todolist_line_4
    ui->screen_todolist_line_4 = lv_line_create(ui->screen_todolist);
    static lv_point_t screen_todolist_line_4[] = {{0, 0},{300, 0},};
    lv_line_set_points(ui->screen_todolist_line_4, screen_todolist_line_4, 2);
    lv_obj_set_pos(ui->screen_todolist_line_4, 173, 115);
    lv_obj_set_size(ui->screen_todolist_line_4, 127, 1);

    //Write style for screen_todolist_line_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_line_width(ui->screen_todolist_line_4, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui->screen_todolist_line_4, lv_color_hex(0x7698bc), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui->screen_todolist_line_4, 176, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_todolist.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_todolist);

    //Init events for screen.
    events_init_screen_todolist(ui);
}
