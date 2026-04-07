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



void setup_scr_screen_ai(lv_ui *ui)
{
    //Write codes screen_ai
    ui->screen_ai = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_ai, 320, 170);
    lv_obj_set_scrollbar_mode(ui->screen_ai, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_ai, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_ai, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_ai, &_v1_320x170, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_ai, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_ai, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_ai_ai_cont
    ui->screen_ai_ai_cont = lv_obj_create(ui->screen_ai);
    lv_obj_set_pos(ui->screen_ai_ai_cont, 0, 0);
    lv_obj_set_size(ui->screen_ai_ai_cont, 320, 510);
    lv_obj_set_scrollbar_mode(ui->screen_ai_ai_cont, LV_SCROLLBAR_MODE_AUTO);

    //Write style for screen_ai_ai_cont, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_ai_ai_cont, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_ai_ai_cont, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_ai_ai_cont, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_ai_ai_cont, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_ai_cont, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_ai_ai_cont, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_ai_ai_cont, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_ai_ai_cont, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_ai_ai_cont, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_ai_ai_cont, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_ai_ai_cont, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_ai_ai_cont, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_ai_ai_cont, &_reppo_ai_bg_320x510, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_ai_ai_cont, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_ai_ai_cont, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_ai_ai_cont, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_ai_img_bt_bg
    ui->screen_ai_img_bt_bg = lv_img_create(ui->screen_ai_ai_cont);
    lv_obj_add_flag(ui->screen_ai_img_bt_bg, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_ai_img_bt_bg, &_ai_bt_bg_alpha_155x36);
    lv_img_set_pivot(ui->screen_ai_img_bt_bg, 50,50);
    lv_img_set_angle(ui->screen_ai_img_bt_bg, 0);
    lv_obj_set_pos(ui->screen_ai_img_bt_bg, 7, 135);
    lv_obj_set_size(ui->screen_ai_img_bt_bg, 155, 36);

    //Write style for screen_ai_img_bt_bg, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_ai_img_bt_bg, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_ai_img_bt_bg, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_img_bt_bg, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_ai_img_bt_bg, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_ai_img_v1
    ui->screen_ai_img_v1 = lv_img_create(ui->screen_ai_ai_cont);
    lv_obj_add_flag(ui->screen_ai_img_v1, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_ai_img_v1, &_v1_alpha_304x126);
    lv_img_set_pivot(ui->screen_ai_img_v1, 50,50);
    lv_img_set_angle(ui->screen_ai_img_v1, 0);
    lv_obj_set_pos(ui->screen_ai_img_v1, 8, 12);
    lv_obj_set_size(ui->screen_ai_img_v1, 304, 126);

    //Write style for screen_ai_img_v1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_ai_img_v1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_ai_img_v1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_img_v1, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_ai_img_v1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_ai_bar_level
    ui->screen_ai_bar_level = lv_bar_create(ui->screen_ai_ai_cont);
    lv_obj_set_style_anim_time(ui->screen_ai_bar_level, 1000, 0);
    lv_bar_set_mode(ui->screen_ai_bar_level, LV_BAR_MODE_NORMAL);
    lv_bar_set_range(ui->screen_ai_bar_level, 0, 100);
    lv_bar_set_value(ui->screen_ai_bar_level, 57, LV_ANIM_OFF);
    lv_obj_set_pos(ui->screen_ai_bar_level, 25, 105);
    lv_obj_set_size(ui->screen_ai_bar_level, 266, 5);

    //Write style for screen_ai_bar_level, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_ai_bar_level, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_ai_bar_level, lv_color_hex(0x7c91b2), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_ai_bar_level, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_bar_level, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_ai_bar_level, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_ai_bar_level, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_ai_bar_level, 152, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_ai_bar_level, lv_color_hex(0xe3f0ff), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_ai_bar_level, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_bar_level, 5, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write codes screen_ai_img_bt_bg2
    ui->screen_ai_img_bt_bg2 = lv_img_create(ui->screen_ai_ai_cont);
    lv_obj_add_flag(ui->screen_ai_img_bt_bg2, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_ai_img_bt_bg2, &_ai_bt_bg_alpha_155x36);
    lv_img_set_pivot(ui->screen_ai_img_bt_bg2, 50,50);
    lv_img_set_angle(ui->screen_ai_img_bt_bg2, 0);
    lv_obj_set_pos(ui->screen_ai_img_bt_bg2, 155, 135);
    lv_obj_set_size(ui->screen_ai_img_bt_bg2, 155, 36);

    //Write style for screen_ai_img_bt_bg2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_ai_img_bt_bg2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_ai_img_bt_bg2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_img_bt_bg2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_ai_img_bt_bg2, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_ai_label_bg2
    ui->screen_ai_label_bg2 = lv_label_create(ui->screen_ai_ai_cont);
    lv_label_set_text(ui->screen_ai_label_bg2, "每周总结");
    lv_label_set_long_mode(ui->screen_ai_label_bg2, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_ai_label_bg2, 186, 138);
    lv_obj_set_size(ui->screen_ai_label_bg2, 98, 17);

    //Write style for screen_ai_label_bg2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_ai_label_bg2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_label_bg2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_ai_label_bg2, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_ai_label_bg2, &lv_font_AdobeFangsongStdRegular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_ai_label_bg2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_ai_label_bg2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_ai_label_bg2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_ai_label_bg2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_ai_label_bg2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_ai_label_bg2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_ai_label_bg2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_ai_label_bg2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_ai_label_bg2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_ai_label_bg2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_ai_label_bg
    ui->screen_ai_label_bg = lv_label_create(ui->screen_ai_ai_cont);
    lv_label_set_text(ui->screen_ai_label_bg, "每日总结");
    lv_label_set_long_mode(ui->screen_ai_label_bg, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_ai_label_bg, 37, 138);
    lv_obj_set_size(ui->screen_ai_label_bg, 98, 17);

    //Write style for screen_ai_label_bg, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_ai_label_bg, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_label_bg, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_ai_label_bg, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_ai_label_bg, &lv_font_AdobeFangsongStdRegular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_ai_label_bg, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_ai_label_bg, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_ai_label_bg, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_ai_label_bg, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_ai_label_bg, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_ai_label_bg, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_ai_label_bg, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_ai_label_bg, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_ai_label_bg, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_ai_label_bg, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_ai_btn_day
    ui->screen_ai_btn_day = lv_btn_create(ui->screen_ai_ai_cont);
    ui->screen_ai_btn_day_label = lv_label_create(ui->screen_ai_btn_day);
    lv_label_set_text(ui->screen_ai_btn_day_label, "");
    lv_label_set_long_mode(ui->screen_ai_btn_day_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_ai_btn_day_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_ai_btn_day, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_ai_btn_day_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_ai_btn_day, 16, 138);
    lv_obj_set_size(ui->screen_ai_btn_day, 138, 16);

    //Write style for screen_ai_btn_day, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_ai_btn_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_ai_btn_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_btn_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_ai_btn_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_ai_btn_day, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_ai_btn_day, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_ai_btn_day, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_ai_btn_day, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_ai_btn_day, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_ai_btn_day, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_ai_btn_day, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_ai_btn_day, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_ai_btn_day, 11, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_ai_btn_day, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_ai_btn_day, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_ai_btn_day, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_ai_btn_day, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_ai_btn_day, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_ai_btn_day, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_ai_btn_day, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_ai_btn_day, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_ai_btn_week
    ui->screen_ai_btn_week = lv_btn_create(ui->screen_ai_ai_cont);
    ui->screen_ai_btn_week_label = lv_label_create(ui->screen_ai_btn_week);
    lv_label_set_text(ui->screen_ai_btn_week_label, "");
    lv_label_set_long_mode(ui->screen_ai_btn_week_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_ai_btn_week_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_ai_btn_week, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_ai_btn_week_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_ai_btn_week, 164, 138);
    lv_obj_set_size(ui->screen_ai_btn_week, 138, 16);

    //Write style for screen_ai_btn_week, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_ai_btn_week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_ai_btn_week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_btn_week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_ai_btn_week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_ai_btn_week, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_ai_btn_week, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_ai_btn_week, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_ai_btn_week, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_ai_btn_week, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_ai_btn_week, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_ai_btn_week, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_ai_btn_week, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_ai_btn_week, 11, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_ai_btn_week, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_ai_btn_week, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_ai_btn_week, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_ai_btn_week, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_ai_btn_week, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_ai_btn_week, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_ai_btn_week, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_ai_btn_week, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_ai_img_v10
    ui->screen_ai_img_v10 = lv_img_create(ui->screen_ai_ai_cont);
    lv_obj_add_flag(ui->screen_ai_img_v10, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_ai_img_v10, &_v10_alpha_305x140);
    lv_img_set_pivot(ui->screen_ai_img_v10, 50,50);
    lv_img_set_angle(ui->screen_ai_img_v10, 0);
    lv_obj_set_pos(ui->screen_ai_img_v10, 8, 6);
    lv_obj_set_size(ui->screen_ai_img_v10, 305, 140);
    lv_obj_add_flag(ui->screen_ai_img_v10, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_ai_img_v10, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_ai_img_v10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_ai_img_v10, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_img_v10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_ai_img_v10, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_ai_img_v4
    ui->screen_ai_img_v4 = lv_img_create(ui->screen_ai_ai_cont);
    lv_obj_add_flag(ui->screen_ai_img_v4, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_ai_img_v4, &_v4_alpha_304x126);
    lv_img_set_pivot(ui->screen_ai_img_v4, 50,50);
    lv_img_set_angle(ui->screen_ai_img_v4, 0);
    lv_obj_set_pos(ui->screen_ai_img_v4, 9, 14);
    lv_obj_set_size(ui->screen_ai_img_v4, 304, 126);
    lv_obj_add_flag(ui->screen_ai_img_v4, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_ai_img_v4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_ai_img_v4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_ai_img_v4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_img_v4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_ai_img_v4, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_ai_img_v5
    ui->screen_ai_img_v5 = lv_img_create(ui->screen_ai_ai_cont);
    lv_obj_add_flag(ui->screen_ai_img_v5, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_ai_img_v5, &_v5_alpha_304x126);
    lv_img_set_pivot(ui->screen_ai_img_v5, 50,50);
    lv_img_set_angle(ui->screen_ai_img_v5, 0);
    lv_obj_set_pos(ui->screen_ai_img_v5, 9, 14);
    lv_obj_set_size(ui->screen_ai_img_v5, 304, 126);
    lv_obj_add_flag(ui->screen_ai_img_v5, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_ai_img_v5, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_ai_img_v5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_ai_img_v5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_img_v5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_ai_img_v5, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_ai_img_v6
    ui->screen_ai_img_v6 = lv_img_create(ui->screen_ai_ai_cont);
    lv_obj_add_flag(ui->screen_ai_img_v6, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_ai_img_v6, &_v6_alpha_304x126);
    lv_img_set_pivot(ui->screen_ai_img_v6, 50,50);
    lv_img_set_angle(ui->screen_ai_img_v6, 0);
    lv_obj_set_pos(ui->screen_ai_img_v6, 9, 14);
    lv_obj_set_size(ui->screen_ai_img_v6, 304, 126);
    lv_obj_add_flag(ui->screen_ai_img_v6, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_ai_img_v6, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_ai_img_v6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_ai_img_v6, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_img_v6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_ai_img_v6, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_ai_img_v3
    ui->screen_ai_img_v3 = lv_img_create(ui->screen_ai_ai_cont);
    lv_obj_add_flag(ui->screen_ai_img_v3, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_ai_img_v3, &_v3_alpha_304x126);
    lv_img_set_pivot(ui->screen_ai_img_v3, 50,50);
    lv_img_set_angle(ui->screen_ai_img_v3, 0);
    lv_obj_set_pos(ui->screen_ai_img_v3, 9, 14);
    lv_obj_set_size(ui->screen_ai_img_v3, 304, 126);
    lv_obj_add_flag(ui->screen_ai_img_v3, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_ai_img_v3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_ai_img_v3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_ai_img_v3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_img_v3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_ai_img_v3, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_ai_img_v7
    ui->screen_ai_img_v7 = lv_img_create(ui->screen_ai_ai_cont);
    lv_obj_add_flag(ui->screen_ai_img_v7, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_ai_img_v7, &_v7_alpha_304x126);
    lv_img_set_pivot(ui->screen_ai_img_v7, 50,50);
    lv_img_set_angle(ui->screen_ai_img_v7, 0);
    lv_obj_set_pos(ui->screen_ai_img_v7, 9, 14);
    lv_obj_set_size(ui->screen_ai_img_v7, 304, 126);
    lv_obj_add_flag(ui->screen_ai_img_v7, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_ai_img_v7, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_ai_img_v7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_ai_img_v7, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_img_v7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_ai_img_v7, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_ai_img_v8
    ui->screen_ai_img_v8 = lv_img_create(ui->screen_ai_ai_cont);
    lv_obj_add_flag(ui->screen_ai_img_v8, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_ai_img_v8, &_v8_alpha_304x126);
    lv_img_set_pivot(ui->screen_ai_img_v8, 50,50);
    lv_img_set_angle(ui->screen_ai_img_v8, 0);
    lv_obj_set_pos(ui->screen_ai_img_v8, 9, 14);
    lv_obj_set_size(ui->screen_ai_img_v8, 304, 126);
    lv_obj_add_flag(ui->screen_ai_img_v8, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_ai_img_v8, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_ai_img_v8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_ai_img_v8, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_img_v8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_ai_img_v8, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_ai_img_v2
    ui->screen_ai_img_v2 = lv_img_create(ui->screen_ai_ai_cont);
    lv_obj_add_flag(ui->screen_ai_img_v2, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_ai_img_v2, &_v2_alpha_304x126);
    lv_img_set_pivot(ui->screen_ai_img_v2, 50,50);
    lv_img_set_angle(ui->screen_ai_img_v2, 0);
    lv_obj_set_pos(ui->screen_ai_img_v2, 9, 14);
    lv_obj_set_size(ui->screen_ai_img_v2, 304, 126);
    lv_obj_add_flag(ui->screen_ai_img_v2, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_ai_img_v2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_ai_img_v2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_ai_img_v2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_img_v2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_ai_img_v2, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_ai_label_percent
    ui->screen_ai_label_percent = lv_label_create(ui->screen_ai_ai_cont);
    lv_label_set_text(ui->screen_ai_label_percent, "0min / 600min");
    lv_label_set_long_mode(ui->screen_ai_label_percent, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_ai_label_percent, 146, 85);
    lv_obj_set_size(ui->screen_ai_label_percent, 142, 16);

    //Write style for screen_ai_label_percent, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_ai_label_percent, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_label_percent, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_ai_label_percent, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_ai_label_percent, &lv_font_arial_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_ai_label_percent, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_ai_label_percent, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_ai_label_percent, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_ai_label_percent, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_ai_label_percent, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_ai_label_percent, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_ai_label_percent, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_ai_label_percent, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_ai_label_percent, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_ai_label_percent, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_ai_img_v9
    ui->screen_ai_img_v9 = lv_img_create(ui->screen_ai_ai_cont);
    lv_obj_add_flag(ui->screen_ai_img_v9, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_ai_img_v9, &_v9_alpha_304x126);
    lv_img_set_pivot(ui->screen_ai_img_v9, 50,50);
    lv_img_set_angle(ui->screen_ai_img_v9, 0);
    lv_obj_set_pos(ui->screen_ai_img_v9, 9, 14);
    lv_obj_set_size(ui->screen_ai_img_v9, 304, 126);
    lv_obj_add_flag(ui->screen_ai_img_v9, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_ai_img_v9, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_ai_img_v9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_ai_img_v9, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_img_v9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_ai_img_v9, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_ai_img_target_bg
    ui->screen_ai_img_target_bg = lv_img_create(ui->screen_ai_ai_cont);
    lv_obj_add_flag(ui->screen_ai_img_target_bg, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_ai_img_target_bg, &_ai_target_alpha_304x140);
    lv_img_set_pivot(ui->screen_ai_img_target_bg, 50,50);
    lv_img_set_angle(ui->screen_ai_img_target_bg, 0);
    lv_obj_set_pos(ui->screen_ai_img_target_bg, 7, 177);
    lv_obj_set_size(ui->screen_ai_img_target_bg, 304, 140);

    //Write style for screen_ai_img_target_bg, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_ai_img_target_bg, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_ai_img_target_bg, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_img_target_bg, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_ai_img_target_bg, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_ai_arc_1
    ui->screen_ai_arc_1 = lv_arc_create(ui->screen_ai_ai_cont);
    lv_arc_set_mode(ui->screen_ai_arc_1, LV_ARC_MODE_NORMAL);
    lv_arc_set_range(ui->screen_ai_arc_1, 0, 100);
    lv_arc_set_bg_angles(ui->screen_ai_arc_1, 135, 45);
    lv_arc_set_value(ui->screen_ai_arc_1, 70);
    lv_arc_set_rotation(ui->screen_ai_arc_1, 0);
    lv_obj_set_style_arc_rounded(ui->screen_ai_arc_1, 0,  LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_rounded(ui->screen_ai_arc_1, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(ui->screen_ai_arc_1, 5, 207);
    lv_obj_set_size(ui->screen_ai_arc_1, 110, 122);

    //Write style for screen_ai_arc_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_ai_arc_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_ai_arc_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui->screen_ai_arc_1, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui->screen_ai_arc_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui->screen_ai_arc_1, lv_color_hex(0xe6e6e6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_arc_1, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_ai_arc_1, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_ai_arc_1, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_ai_arc_1, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_ai_arc_1, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_ai_arc_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_ai_arc_1, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_arc_width(ui->screen_ai_arc_1, 6, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui->screen_ai_arc_1, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui->screen_ai_arc_1, lv_color_hex(0xb3d1ff), LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for screen_ai_arc_1, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_ai_arc_1, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_ai_arc_1, lv_color_hex(0x9fbde9), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_ai_arc_1, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui->screen_ai_arc_1, 3, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_ai_arc_2
    ui->screen_ai_arc_2 = lv_arc_create(ui->screen_ai_ai_cont);
    lv_arc_set_mode(ui->screen_ai_arc_2, LV_ARC_MODE_NORMAL);
    lv_arc_set_range(ui->screen_ai_arc_2, 0, 100);
    lv_arc_set_bg_angles(ui->screen_ai_arc_2, 135, 45);
    lv_arc_set_value(ui->screen_ai_arc_2, 70);
    lv_arc_set_rotation(ui->screen_ai_arc_2, 0);
    lv_obj_set_style_arc_rounded(ui->screen_ai_arc_2, 0,  LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_rounded(ui->screen_ai_arc_2, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(ui->screen_ai_arc_2, 106, 207);
    lv_obj_set_size(ui->screen_ai_arc_2, 110, 122);

    //Write style for screen_ai_arc_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_ai_arc_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_ai_arc_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui->screen_ai_arc_2, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui->screen_ai_arc_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui->screen_ai_arc_2, lv_color_hex(0xe6e6e6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_arc_2, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_ai_arc_2, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_ai_arc_2, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_ai_arc_2, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_ai_arc_2, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_ai_arc_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_ai_arc_2, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_arc_width(ui->screen_ai_arc_2, 6, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui->screen_ai_arc_2, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui->screen_ai_arc_2, lv_color_hex(0xd9fff2), LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for screen_ai_arc_2, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_ai_arc_2, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_ai_arc_2, lv_color_hex(0xc1e9db), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_ai_arc_2, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui->screen_ai_arc_2, 3, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_ai_arc_3
    ui->screen_ai_arc_3 = lv_arc_create(ui->screen_ai_ai_cont);
    lv_arc_set_mode(ui->screen_ai_arc_3, LV_ARC_MODE_NORMAL);
    lv_arc_set_range(ui->screen_ai_arc_3, 0, 100);
    lv_arc_set_bg_angles(ui->screen_ai_arc_3, 135, 45);
    lv_arc_set_value(ui->screen_ai_arc_3, 70);
    lv_arc_set_rotation(ui->screen_ai_arc_3, 0);
    lv_obj_set_style_arc_rounded(ui->screen_ai_arc_3, 0,  LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_rounded(ui->screen_ai_arc_3, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(ui->screen_ai_arc_3, 204, 207);
    lv_obj_set_size(ui->screen_ai_arc_3, 110, 122);

    //Write style for screen_ai_arc_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_ai_arc_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_ai_arc_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui->screen_ai_arc_3, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui->screen_ai_arc_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui->screen_ai_arc_3, lv_color_hex(0xe6e6e6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_arc_3, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_ai_arc_3, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_ai_arc_3, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_ai_arc_3, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_ai_arc_3, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_ai_arc_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_ai_arc_3, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_arc_width(ui->screen_ai_arc_3, 6, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui->screen_ai_arc_3, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui->screen_ai_arc_3, lv_color_hex(0xd7c8d9), LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for screen_ai_arc_3, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_ai_arc_3, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_ai_arc_3, lv_color_hex(0xbfacc1), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_ai_arc_3, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui->screen_ai_arc_3, 3, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_ai_label_cat2
    ui->screen_ai_label_cat2 = lv_label_create(ui->screen_ai_ai_cont);
    lv_label_set_text(ui->screen_ai_label_cat2, "项目进度");
    lv_label_set_long_mode(ui->screen_ai_label_cat2, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_ai_label_cat2, 117, 199);
    lv_obj_set_size(ui->screen_ai_label_cat2, 86, 18);

    //Write style for screen_ai_label_cat2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_ai_label_cat2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_label_cat2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_ai_label_cat2, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_ai_label_cat2, &lv_font_AdobeFangsongStdRegular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_ai_label_cat2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_ai_label_cat2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_ai_label_cat2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_ai_label_cat2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_ai_label_cat2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_ai_label_cat2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_ai_label_cat2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_ai_label_cat2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_ai_label_cat2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_ai_label_cat2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_ai_label_cat1
    ui->screen_ai_label_cat1 = lv_label_create(ui->screen_ai_ai_cont);
    lv_label_set_text(ui->screen_ai_label_cat1, "学习任务");
    lv_label_set_long_mode(ui->screen_ai_label_cat1, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_ai_label_cat1, 19, 199);
    lv_obj_set_size(ui->screen_ai_label_cat1, 86, 18);

    //Write style for screen_ai_label_cat1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_ai_label_cat1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_label_cat1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_ai_label_cat1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_ai_label_cat1, &lv_font_AdobeFangsongStdRegular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_ai_label_cat1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_ai_label_cat1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_ai_label_cat1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_ai_label_cat1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_ai_label_cat1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_ai_label_cat1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_ai_label_cat1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_ai_label_cat1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_ai_label_cat1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_ai_label_cat1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_ai_label_cat3_num
    ui->screen_ai_label_cat3_num = lv_label_create(ui->screen_ai_ai_cont);
    lv_label_set_text(ui->screen_ai_label_cat3_num, "25%");
    lv_label_set_long_mode(ui->screen_ai_label_cat3_num, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_ai_label_cat3_num, 229, 284);
    lv_obj_set_size(ui->screen_ai_label_cat3_num, 69, 15);

    //Write style for screen_ai_label_cat3_num, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_ai_label_cat3_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_label_cat3_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_ai_label_cat3_num, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_ai_label_cat3_num, &lv_font_arial_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_ai_label_cat3_num, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_ai_label_cat3_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_ai_label_cat3_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_ai_label_cat3_num, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_ai_label_cat3_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_ai_label_cat3_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_ai_label_cat3_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_ai_label_cat3_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_ai_label_cat3_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_ai_label_cat3_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_ai_label_cat2_num
    ui->screen_ai_label_cat2_num = lv_label_create(ui->screen_ai_ai_cont);
    lv_label_set_text(ui->screen_ai_label_cat2_num, "15%");
    lv_label_set_long_mode(ui->screen_ai_label_cat2_num, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_ai_label_cat2_num, 128, 284);
    lv_obj_set_size(ui->screen_ai_label_cat2_num, 69, 15);

    //Write style for screen_ai_label_cat2_num, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_ai_label_cat2_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_label_cat2_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_ai_label_cat2_num, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_ai_label_cat2_num, &lv_font_arial_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_ai_label_cat2_num, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_ai_label_cat2_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_ai_label_cat2_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_ai_label_cat2_num, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_ai_label_cat2_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_ai_label_cat2_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_ai_label_cat2_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_ai_label_cat2_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_ai_label_cat2_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_ai_label_cat2_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_ai_label_cat1_num
    ui->screen_ai_label_cat1_num = lv_label_create(ui->screen_ai_ai_cont);
    lv_label_set_text(ui->screen_ai_label_cat1_num, "60%");
    lv_label_set_long_mode(ui->screen_ai_label_cat1_num, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_ai_label_cat1_num, 29, 284);
    lv_obj_set_size(ui->screen_ai_label_cat1_num, 69, 15);

    //Write style for screen_ai_label_cat1_num, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_ai_label_cat1_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_label_cat1_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_ai_label_cat1_num, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_ai_label_cat1_num, &lv_font_arial_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_ai_label_cat1_num, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_ai_label_cat1_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_ai_label_cat1_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_ai_label_cat1_num, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_ai_label_cat1_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_ai_label_cat1_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_ai_label_cat1_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_ai_label_cat1_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_ai_label_cat1_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_ai_label_cat1_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_ai_label_cat3
    ui->screen_ai_label_cat3 = lv_label_create(ui->screen_ai_ai_cont);
    lv_label_set_text(ui->screen_ai_label_cat3, "生活日常");
    lv_label_set_long_mode(ui->screen_ai_label_cat3, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_ai_label_cat3, 215, 199);
    lv_obj_set_size(ui->screen_ai_label_cat3, 86, 18);

    //Write style for screen_ai_label_cat3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_ai_label_cat3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_label_cat3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_ai_label_cat3, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_ai_label_cat3, &lv_font_AdobeFangsongStdRegular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_ai_label_cat3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_ai_label_cat3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_ai_label_cat3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_ai_label_cat3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_ai_label_cat3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_ai_label_cat3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_ai_label_cat3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_ai_label_cat3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_ai_label_cat3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_ai_label_cat3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_ai_line_1
    ui->screen_ai_line_1 = lv_line_create(ui->screen_ai_ai_cont);
    static lv_point_t screen_ai_line_1[] = {{0, 0},{290, 0},};
    lv_line_set_points(ui->screen_ai_line_1, screen_ai_line_1, 2);
    lv_obj_set_pos(ui->screen_ai_line_1, 13, 168);
    lv_obj_set_size(ui->screen_ai_line_1, 290, 6);

    //Write style for screen_ai_line_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_line_width(ui->screen_ai_line_1, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui->screen_ai_line_1, lv_color_hex(0x8da6bb), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui->screen_ai_line_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_rounded(ui->screen_ai_line_1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_ai_line_2
    ui->screen_ai_line_2 = lv_line_create(ui->screen_ai_ai_cont);
    static lv_point_t screen_ai_line_2[] = {{0, 0},{290, 0},};
    lv_line_set_points(ui->screen_ai_line_2, screen_ai_line_2, 2);
    lv_obj_set_pos(ui->screen_ai_line_2, 13, 321);
    lv_obj_set_size(ui->screen_ai_line_2, 290, 6);

    //Write style for screen_ai_line_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_line_width(ui->screen_ai_line_2, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui->screen_ai_line_2, lv_color_hex(0x8da6bb), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui->screen_ai_line_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_rounded(ui->screen_ai_line_2, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_ai_img_gicon1
    ui->screen_ai_img_gicon1 = lv_img_create(ui->screen_ai_ai_cont);
    lv_obj_add_flag(ui->screen_ai_img_gicon1, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_ai_img_gicon1, &_gemini_alpha_15x15);
    lv_img_set_pivot(ui->screen_ai_img_gicon1, 50,50);
    lv_img_set_angle(ui->screen_ai_img_gicon1, 0);
    lv_obj_set_pos(ui->screen_ai_img_gicon1, 15, 185);
    lv_obj_set_size(ui->screen_ai_img_gicon1, 15, 15);

    //Write style for screen_ai_img_gicon1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_ai_img_gicon1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_ai_img_gicon1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_img_gicon1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_ai_img_gicon1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_ai_img_gicon2
    ui->screen_ai_img_gicon2 = lv_img_create(ui->screen_ai_ai_cont);
    lv_obj_add_flag(ui->screen_ai_img_gicon2, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_ai_img_gicon2, &_gemini_alpha_15x15);
    lv_img_set_pivot(ui->screen_ai_img_gicon2, 50,50);
    lv_img_set_angle(ui->screen_ai_img_gicon2, 0);
    lv_obj_set_pos(ui->screen_ai_img_gicon2, 115, 185);
    lv_obj_set_size(ui->screen_ai_img_gicon2, 15, 15);

    //Write style for screen_ai_img_gicon2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_ai_img_gicon2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_ai_img_gicon2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_img_gicon2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_ai_img_gicon2, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_ai_img_sum_bg
    ui->screen_ai_img_sum_bg = lv_img_create(ui->screen_ai_ai_cont);
    lv_obj_add_flag(ui->screen_ai_img_sum_bg, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_ai_img_sum_bg, &_ai_sum_bg_alpha_305x172);
    lv_img_set_pivot(ui->screen_ai_img_sum_bg, 50,50);
    lv_img_set_angle(ui->screen_ai_img_sum_bg, 0);
    lv_obj_set_pos(ui->screen_ai_img_sum_bg, 7, 332);
    lv_obj_set_size(ui->screen_ai_img_sum_bg, 305, 172);

    //Write style for screen_ai_img_sum_bg, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_ai_img_sum_bg, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_ai_img_sum_bg, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_img_sum_bg, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_ai_img_sum_bg, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_ai_img_gicon3
    ui->screen_ai_img_gicon3 = lv_img_create(ui->screen_ai_ai_cont);
    lv_obj_add_flag(ui->screen_ai_img_gicon3, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_ai_img_gicon3, &_gemini_alpha_15x15);
    lv_img_set_pivot(ui->screen_ai_img_gicon3, 50,50);
    lv_img_set_angle(ui->screen_ai_img_gicon3, 0);
    lv_obj_set_pos(ui->screen_ai_img_gicon3, 214, 185);
    lv_obj_set_size(ui->screen_ai_img_gicon3, 15, 15);

    //Write style for screen_ai_img_gicon3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_ai_img_gicon3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_ai_img_gicon3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_img_gicon3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_ai_img_gicon3, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_ai_label_quote
    ui->screen_ai_label_quote = lv_label_create(ui->screen_ai_ai_cont);
    lv_label_set_text(ui->screen_ai_label_quote, "Action conquers fear.");
    lv_label_set_long_mode(ui->screen_ai_label_quote, LV_LABEL_LONG_SCROLL);
    lv_obj_set_pos(ui->screen_ai_label_quote, 90, 463);
    lv_obj_set_size(ui->screen_ai_label_quote, 210, 19);

    //Write style for screen_ai_label_quote, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_ai_label_quote, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_label_quote, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_ai_label_quote, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_ai_label_quote, &lv_font_AdobeFangsongStdRegular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_ai_label_quote, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_ai_label_quote, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_ai_label_quote, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_ai_label_quote, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_ai_label_quote, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_ai_label_quote, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_ai_label_quote, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_ai_label_quote, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_ai_label_quote, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_ai_label_quote, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_ai_label_sum
    ui->screen_ai_label_sum = lv_label_create(ui->screen_ai_ai_cont);
    lv_label_set_text(ui->screen_ai_label_sum, "专注当下,每一步都塑造未来.建议优先完成临近截止的实验室整理与代码编写,为后续腾出空间.");
    lv_label_set_long_mode(ui->screen_ai_label_sum, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_ai_label_sum, 16, 394);
    lv_obj_set_size(ui->screen_ai_label_sum, 288, 63);

    //Write style for screen_ai_label_sum, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_ai_label_sum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_label_sum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_ai_label_sum, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_ai_label_sum, &lv_font_AdobeFangsongStdRegular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_ai_label_sum, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_ai_label_sum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_ai_label_sum, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_ai_label_sum, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_ai_label_sum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_ai_label_sum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_ai_label_sum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_ai_label_sum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_ai_label_sum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_ai_label_sum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_ai_btn_back
    ui->screen_ai_btn_back = lv_btn_create(ui->screen_ai_ai_cont);
    ui->screen_ai_btn_back_label = lv_label_create(ui->screen_ai_btn_back);
    lv_label_set_text(ui->screen_ai_btn_back_label, "<");
    lv_label_set_long_mode(ui->screen_ai_btn_back_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_ai_btn_back_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_ai_btn_back, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_ai_btn_back_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_ai_btn_back, 3, 0);
    lv_obj_set_size(ui->screen_ai_btn_back, 8, 13);

    //Write style for screen_ai_btn_back, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_ai_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_ai_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_btn_back, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_ai_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_ai_btn_back, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_ai_btn_back, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_ai_btn_back, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_ai_btn_back, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_ai_btn_back, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_ai_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_ai_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_ai_btn_back, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_ai_btn_back, 6, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_ai_btn_back, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_ai_btn_back, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_ai_btn_back, 2, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_ai_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_ai_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_ai_btn_back, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_ai_btn_back, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_ai_btn_back, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_ai_btn_sum
    ui->screen_ai_btn_sum = lv_btn_create(ui->screen_ai_ai_cont);
    ui->screen_ai_btn_sum_label = lv_label_create(ui->screen_ai_btn_sum);
    lv_label_set_text(ui->screen_ai_btn_sum_label, "");
    lv_label_set_long_mode(ui->screen_ai_btn_sum_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_ai_btn_sum_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_ai_btn_sum, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_ai_btn_sum_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_ai_btn_sum, 9, 499);
    lv_obj_set_size(ui->screen_ai_btn_sum, 299, 1);

    //Write style for screen_ai_btn_sum, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_ai_btn_sum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_ai_btn_sum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_btn_sum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_ai_btn_sum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_ai_btn_sum, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_ai_btn_sum, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_ai_btn_sum, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_ai_btn_sum, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_ai_btn_sum, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_ai_btn_sum, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_ai_btn_sum, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_ai_btn_sum, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_ai_btn_sum, 4, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_ai_btn_sum, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_ai_btn_sum, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_ai_btn_sum, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_ai_btn_sum, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_ai_btn_sum, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_ai_btn_sum, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_ai_btn_sum, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_ai_btn_sum, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write style for screen_ai_btn_sum, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_bg_opa(ui->screen_ai_btn_sum, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_border_width(ui->screen_ai_btn_sum, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_radius(ui->screen_ai_btn_sum, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->screen_ai_btn_sum, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->screen_ai_btn_sum, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->screen_ai_btn_sum, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->screen_ai_btn_sum, 255, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for screen_ai_btn_sum, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_bg_opa(ui->screen_ai_btn_sum, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_border_width(ui->screen_ai_btn_sum, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_radius(ui->screen_ai_btn_sum, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_ai_btn_sum, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_ai_btn_sum, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->screen_ai_btn_sum, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->screen_ai_btn_sum, 255, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for screen_ai_btn_sum, Part: LV_PART_MAIN, State: LV_STATE_DISABLED.
    lv_obj_set_style_bg_opa(ui->screen_ai_btn_sum, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_border_width(ui->screen_ai_btn_sum, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_radius(ui->screen_ai_btn_sum, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_shadow_width(ui->screen_ai_btn_sum, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_text_color(ui->screen_ai_btn_sum, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_text_font(ui->screen_ai_btn_sum, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_text_opa(ui->screen_ai_btn_sum, 255, LV_PART_MAIN|LV_STATE_DISABLED);

    //Write codes screen_ai_btn_cat
    ui->screen_ai_btn_cat = lv_btn_create(ui->screen_ai_ai_cont);
    ui->screen_ai_btn_cat_label = lv_label_create(ui->screen_ai_btn_cat);
    lv_label_set_text(ui->screen_ai_btn_cat_label, "");
    lv_label_set_long_mode(ui->screen_ai_btn_cat_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_ai_btn_cat_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_ai_btn_cat, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_ai_btn_cat_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_ai_btn_cat, 10, 327);
    lv_obj_set_size(ui->screen_ai_btn_cat, 296, 1);

    //Write style for screen_ai_btn_cat, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_ai_btn_cat, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_ai_btn_cat, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_ai_btn_cat, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_ai_btn_cat, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_ai_btn_cat, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_ai_btn_cat, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_ai_btn_cat, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_ai_btn_cat, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_ai_btn_cat, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_ai_btn_cat, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_ai_btn_cat, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_ai_btn_cat, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_ai_btn_cat, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_ai_btn_cat, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_ai_btn_cat, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_ai_btn_cat, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_ai_btn_cat, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_ai_btn_cat, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_ai_btn_cat, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_ai_btn_cat, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_ai_btn_cat, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write style for screen_ai_btn_cat, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_bg_opa(ui->screen_ai_btn_cat, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_border_width(ui->screen_ai_btn_cat, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_radius(ui->screen_ai_btn_cat, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->screen_ai_btn_cat, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->screen_ai_btn_cat, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->screen_ai_btn_cat, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->screen_ai_btn_cat, 255, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for screen_ai_btn_cat, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_bg_opa(ui->screen_ai_btn_cat, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_border_width(ui->screen_ai_btn_cat, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_radius(ui->screen_ai_btn_cat, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_ai_btn_cat, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_ai_btn_cat, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->screen_ai_btn_cat, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->screen_ai_btn_cat, 255, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for screen_ai_btn_cat, Part: LV_PART_MAIN, State: LV_STATE_DISABLED.
    lv_obj_set_style_bg_opa(ui->screen_ai_btn_cat, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_border_width(ui->screen_ai_btn_cat, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_radius(ui->screen_ai_btn_cat, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_shadow_width(ui->screen_ai_btn_cat, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_text_color(ui->screen_ai_btn_cat, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_text_font(ui->screen_ai_btn_cat, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_text_opa(ui->screen_ai_btn_cat, 255, LV_PART_MAIN|LV_STATE_DISABLED);

    //The custom code of screen_ai.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_ai);

    //Init events for screen.
    events_init_screen_ai(ui);
}
