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



void setup_scr_screen_pomodoro(lv_ui *ui)
{
    //Write codes screen_pomodoro
    ui->screen_pomodoro = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_pomodoro, 320, 170);
    lv_obj_set_scrollbar_mode(ui->screen_pomodoro, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_pomodoro, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_pomodoro, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_pomodoro, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_pomodoro, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_pomodoro, &_pomodoro_bg_320x170, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_pomodoro, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_pomodoro, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_pomodoro_label_addnum
    ui->screen_pomodoro_label_addnum = lv_label_create(ui->screen_pomodoro);
    lv_label_set_text(ui->screen_pomodoro_label_addnum, "1");
    lv_label_set_long_mode(ui->screen_pomodoro_label_addnum, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_pomodoro_label_addnum, 134, 113);
    lv_obj_set_size(ui->screen_pomodoro_label_addnum, 56, 16);

    //Write style for screen_pomodoro_label_addnum, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_pomodoro_label_addnum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_pomodoro_label_addnum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_pomodoro_label_addnum, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_pomodoro_label_addnum, &lv_font_AdobeFangsongStdRegular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_pomodoro_label_addnum, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_pomodoro_label_addnum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_pomodoro_label_addnum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_pomodoro_label_addnum, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_pomodoro_label_addnum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_pomodoro_label_addnum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_pomodoro_label_addnum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_pomodoro_label_addnum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_pomodoro_label_addnum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_pomodoro_label_addnum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_pomodoro_img_pomorect1
    ui->screen_pomodoro_img_pomorect1 = lv_img_create(ui->screen_pomodoro);
    lv_obj_add_flag(ui->screen_pomodoro_img_pomorect1, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_pomodoro_img_pomorect1, &_pomorect_alpha_257x52);
    lv_img_set_pivot(ui->screen_pomodoro_img_pomorect1, 50,50);
    lv_img_set_angle(ui->screen_pomodoro_img_pomorect1, 0);
    lv_obj_set_pos(ui->screen_pomodoro_img_pomorect1, 37, 29);
    lv_obj_set_size(ui->screen_pomodoro_img_pomorect1, 257, 52);

    //Write style for screen_pomodoro_img_pomorect1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_pomodoro_img_pomorect1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_pomodoro_img_pomorect1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_pomodoro_img_pomorect1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_pomodoro_img_pomorect1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_pomodoro_label_timer
    ui->screen_pomodoro_label_timer = lv_label_create(ui->screen_pomodoro);
    lv_label_set_text(ui->screen_pomodoro_label_timer, "00:25:00");
    lv_label_set_long_mode(ui->screen_pomodoro_label_timer, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_pomodoro_label_timer, 94, 36);
    lv_obj_set_size(ui->screen_pomodoro_label_timer, 141, 27);

    //Write style for screen_pomodoro_label_timer, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_pomodoro_label_timer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_pomodoro_label_timer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_pomodoro_label_timer, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_pomodoro_label_timer, &lv_font_Abel_regular_30, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_pomodoro_label_timer, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_pomodoro_label_timer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_pomodoro_label_timer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_pomodoro_label_timer, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_pomodoro_label_timer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_pomodoro_label_timer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_pomodoro_label_timer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_pomodoro_label_timer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_pomodoro_label_timer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_pomodoro_label_timer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_pomodoro_label_funcstate
    ui->screen_pomodoro_label_funcstate = lv_label_create(ui->screen_pomodoro);
    lv_label_set_text(ui->screen_pomodoro_label_funcstate, "CountDown");
    lv_label_set_long_mode(ui->screen_pomodoro_label_funcstate, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_pomodoro_label_funcstate, 104, 66);
    lv_obj_set_size(ui->screen_pomodoro_label_funcstate, 119, 15);

    //Write style for screen_pomodoro_label_funcstate, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_pomodoro_label_funcstate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_pomodoro_label_funcstate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_pomodoro_label_funcstate, lv_color_hex(0xb586c1), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_pomodoro_label_funcstate, &lv_font_AdobeFangsongStdRegular_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_pomodoro_label_funcstate, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_pomodoro_label_funcstate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_pomodoro_label_funcstate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_pomodoro_label_funcstate, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_pomodoro_label_funcstate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_pomodoro_label_funcstate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_pomodoro_label_funcstate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_pomodoro_label_funcstate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_pomodoro_label_funcstate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_pomodoro_label_funcstate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_pomodoro_label_pomodoro
    ui->screen_pomodoro_label_pomodoro = lv_label_create(ui->screen_pomodoro);
    lv_label_set_text(ui->screen_pomodoro_label_pomodoro, "Pomodoro Focus");
    lv_label_set_long_mode(ui->screen_pomodoro_label_pomodoro, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_pomodoro_label_pomodoro, 60, 5);
    lv_obj_set_size(ui->screen_pomodoro_label_pomodoro, 210, 17);

    //Write style for screen_pomodoro_label_pomodoro, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_pomodoro_label_pomodoro, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_pomodoro_label_pomodoro, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_pomodoro_label_pomodoro, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_pomodoro_label_pomodoro, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_pomodoro_label_pomodoro, 229, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_pomodoro_label_pomodoro, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_pomodoro_label_pomodoro, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_pomodoro_label_pomodoro, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_pomodoro_label_pomodoro, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_pomodoro_label_pomodoro, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_pomodoro_label_pomodoro, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_pomodoro_label_pomodoro, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_pomodoro_label_pomodoro, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_pomodoro_label_pomodoro, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_pomodoro_btn_stopwatch
    ui->screen_pomodoro_btn_stopwatch = lv_btn_create(ui->screen_pomodoro);
    ui->screen_pomodoro_btn_stopwatch_label = lv_label_create(ui->screen_pomodoro_btn_stopwatch);
    lv_label_set_text(ui->screen_pomodoro_btn_stopwatch_label, "StopWatch");
    lv_label_set_long_mode(ui->screen_pomodoro_btn_stopwatch_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_pomodoro_btn_stopwatch_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_pomodoro_btn_stopwatch, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_pomodoro_btn_stopwatch_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_pomodoro_btn_stopwatch, 37, 87);
    lv_obj_set_size(ui->screen_pomodoro_btn_stopwatch, 125, 15);

    //Write style for screen_pomodoro_btn_stopwatch, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_pomodoro_btn_stopwatch, 40, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_pomodoro_btn_stopwatch, lv_color_hex(0xc3b5b5), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_pomodoro_btn_stopwatch, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_pomodoro_btn_stopwatch, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_pomodoro_btn_stopwatch, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_pomodoro_btn_stopwatch, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_pomodoro_btn_stopwatch, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_pomodoro_btn_stopwatch, &lv_font_AdobeFangsongStdRegular_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_pomodoro_btn_stopwatch, 221, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_pomodoro_btn_stopwatch, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_pomodoro_btn_stopwatch, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_pomodoro_btn_stopwatch, 28, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(ui->screen_pomodoro_btn_stopwatch, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_grad_dir(ui->screen_pomodoro_btn_stopwatch, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_pomodoro_btn_stopwatch, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_pomodoro_btn_stopwatch, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_pomodoro_btn_stopwatch, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_pomodoro_btn_stopwatch, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_pomodoro_btn_stopwatch, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_pomodoro_btn_stopwatch, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_pomodoro_btn_stopwatch, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_pomodoro_btn_stopwatch, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_pomodoro_btn_stopwatch, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_pomodoro_btn_stopwatch, &lv_font_SourceHanSerifSC_Regular_13, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_pomodoro_btn_stopwatch, 122, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_pomodoro_btn_countdown
    ui->screen_pomodoro_btn_countdown = lv_btn_create(ui->screen_pomodoro);
    ui->screen_pomodoro_btn_countdown_label = lv_label_create(ui->screen_pomodoro_btn_countdown);
    lv_label_set_text(ui->screen_pomodoro_btn_countdown_label, "CountDown");
    lv_label_set_long_mode(ui->screen_pomodoro_btn_countdown_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_pomodoro_btn_countdown_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_pomodoro_btn_countdown, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_pomodoro_btn_countdown_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_pomodoro_btn_countdown, 167, 87);
    lv_obj_set_size(ui->screen_pomodoro_btn_countdown, 125, 15);

    //Write style for screen_pomodoro_btn_countdown, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_pomodoro_btn_countdown, 38, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_pomodoro_btn_countdown, lv_color_hex(0xbebdbd), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_pomodoro_btn_countdown, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_pomodoro_btn_countdown, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_pomodoro_btn_countdown, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_pomodoro_btn_countdown, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_pomodoro_btn_countdown, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_pomodoro_btn_countdown, &lv_font_AdobeFangsongStdRegular_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_pomodoro_btn_countdown, 226, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_pomodoro_btn_countdown, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_pomodoro_btn_countdown, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_pomodoro_btn_countdown, 19, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(ui->screen_pomodoro_btn_countdown, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_grad_dir(ui->screen_pomodoro_btn_countdown, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_pomodoro_btn_countdown, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_pomodoro_btn_countdown, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_pomodoro_btn_countdown, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_pomodoro_btn_countdown, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_pomodoro_btn_countdown, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_pomodoro_btn_countdown, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_pomodoro_btn_countdown, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_pomodoro_btn_countdown, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_pomodoro_btn_countdown, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_pomodoro_btn_countdown, &lv_font_SourceHanSerifSC_Regular_13, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_pomodoro_btn_countdown, 129, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_pomodoro_btn_back
    ui->screen_pomodoro_btn_back = lv_btn_create(ui->screen_pomodoro);
    ui->screen_pomodoro_btn_back_label = lv_label_create(ui->screen_pomodoro_btn_back);
    lv_label_set_text(ui->screen_pomodoro_btn_back_label, "<");
    lv_label_set_long_mode(ui->screen_pomodoro_btn_back_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_pomodoro_btn_back_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_pomodoro_btn_back, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_pomodoro_btn_back_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_pomodoro_btn_back, 4, 3);
    lv_obj_set_size(ui->screen_pomodoro_btn_back, 20, 20);

    //Write style for screen_pomodoro_btn_back, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_pomodoro_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_pomodoro_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_pomodoro_btn_back, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_pomodoro_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_pomodoro_btn_back, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_pomodoro_btn_back, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_pomodoro_btn_back, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_pomodoro_btn_back, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_pomodoro_btn_back, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_pomodoro_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_pomodoro_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_pomodoro_btn_back, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_pomodoro_btn_back, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_pomodoro_btn_back, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_pomodoro_btn_back, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_pomodoro_btn_back, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_pomodoro_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_pomodoro_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_pomodoro_btn_back, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_pomodoro_btn_back, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_pomodoro_btn_back, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_pomodoro_btn_start
    ui->screen_pomodoro_btn_start = lv_btn_create(ui->screen_pomodoro);
    ui->screen_pomodoro_btn_start_label = lv_label_create(ui->screen_pomodoro_btn_start);
    lv_label_set_text(ui->screen_pomodoro_btn_start_label, "Start");
    lv_label_set_long_mode(ui->screen_pomodoro_btn_start_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_pomodoro_btn_start_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_pomodoro_btn_start, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_pomodoro_btn_start_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_pomodoro_btn_start, 94, 145);
    lv_obj_set_size(ui->screen_pomodoro_btn_start, 66, 16);

    //Write style for screen_pomodoro_btn_start, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_pomodoro_btn_start, 34, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_pomodoro_btn_start, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_pomodoro_btn_start, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_pomodoro_btn_start, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_pomodoro_btn_start, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_pomodoro_btn_start, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_pomodoro_btn_start, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_pomodoro_btn_start, &lv_font_AdobeFangsongStdRegular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_pomodoro_btn_start, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_pomodoro_btn_start, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_pomodoro_btn_start, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_pomodoro_btn_start, 34, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(ui->screen_pomodoro_btn_start, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_grad_dir(ui->screen_pomodoro_btn_start, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_pomodoro_btn_start, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_pomodoro_btn_start, 10, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_pomodoro_btn_start, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_pomodoro_btn_start, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_pomodoro_btn_start, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_pomodoro_btn_start, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_pomodoro_btn_start, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_pomodoro_btn_start, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_pomodoro_btn_start, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_pomodoro_btn_start, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_pomodoro_btn_start, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_pomodoro_btn_reset
    ui->screen_pomodoro_btn_reset = lv_btn_create(ui->screen_pomodoro);
    ui->screen_pomodoro_btn_reset_label = lv_label_create(ui->screen_pomodoro_btn_reset);
    lv_label_set_text(ui->screen_pomodoro_btn_reset_label, "Reset");
    lv_label_set_long_mode(ui->screen_pomodoro_btn_reset_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_pomodoro_btn_reset_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_pomodoro_btn_reset, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_pomodoro_btn_reset_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_pomodoro_btn_reset, 170, 145);
    lv_obj_set_size(ui->screen_pomodoro_btn_reset, 66, 16);

    //Write style for screen_pomodoro_btn_reset, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_pomodoro_btn_reset, 34, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_pomodoro_btn_reset, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_pomodoro_btn_reset, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_pomodoro_btn_reset, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_pomodoro_btn_reset, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_pomodoro_btn_reset, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_pomodoro_btn_reset, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_pomodoro_btn_reset, &lv_font_AdobeFangsongStdRegular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_pomodoro_btn_reset, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_pomodoro_btn_reset, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_pomodoro_btn_reset, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_pomodoro_btn_reset, 43, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(ui->screen_pomodoro_btn_reset, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_grad_dir(ui->screen_pomodoro_btn_reset, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_pomodoro_btn_reset, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_pomodoro_btn_reset, 10, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_pomodoro_btn_reset, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_pomodoro_btn_reset, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_pomodoro_btn_reset, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_pomodoro_btn_reset, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_pomodoro_btn_reset, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_pomodoro_btn_reset, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_pomodoro_btn_reset, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_pomodoro_btn_reset, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_pomodoro_btn_reset, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_pomodoro_btn_addnum
    ui->screen_pomodoro_btn_addnum = lv_btn_create(ui->screen_pomodoro);
    ui->screen_pomodoro_btn_addnum_label = lv_label_create(ui->screen_pomodoro_btn_addnum);
    lv_label_set_text(ui->screen_pomodoro_btn_addnum_label, "");
    lv_label_set_long_mode(ui->screen_pomodoro_btn_addnum_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_pomodoro_btn_addnum_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_pomodoro_btn_addnum, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_pomodoro_btn_addnum_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_pomodoro_btn_addnum, 129, 109);
    lv_obj_set_size(ui->screen_pomodoro_btn_addnum, 64, 25);

    //Write style for screen_pomodoro_btn_addnum, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_pomodoro_btn_addnum, 25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_pomodoro_btn_addnum, lv_color_hex(0xe2ecf4), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_pomodoro_btn_addnum, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_pomodoro_btn_addnum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_pomodoro_btn_addnum, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_pomodoro_btn_addnum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_pomodoro_btn_addnum, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_pomodoro_btn_addnum, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_pomodoro_btn_addnum, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_pomodoro_btn_addnum, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_pomodoro_btn_addnum, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_pomodoro_btn_addnum, 29, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(ui->screen_pomodoro_btn_addnum, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_grad_dir(ui->screen_pomodoro_btn_addnum, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_pomodoro_btn_addnum, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_pomodoro_btn_addnum, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_pomodoro_btn_addnum, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_pomodoro_btn_addnum, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_pomodoro_btn_addnum, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_pomodoro_btn_addnum, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_pomodoro_btn_addnum, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_pomodoro_btn_addnum, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_pomodoro_btn_addnum, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_pomodoro_btn_addnum, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_pomodoro_btn_addnum, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_pomodoro_btn_minus
    ui->screen_pomodoro_btn_minus = lv_btn_create(ui->screen_pomodoro);
    ui->screen_pomodoro_btn_minus_label = lv_label_create(ui->screen_pomodoro_btn_minus);
    lv_label_set_text(ui->screen_pomodoro_btn_minus_label, "-");
    lv_label_set_long_mode(ui->screen_pomodoro_btn_minus_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_pomodoro_btn_minus_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_pomodoro_btn_minus, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_pomodoro_btn_minus_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_pomodoro_btn_minus, 94, 112);
    lv_obj_set_size(ui->screen_pomodoro_btn_minus, 24, 18);

    //Write style for screen_pomodoro_btn_minus, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_pomodoro_btn_minus, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_pomodoro_btn_minus, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_pomodoro_btn_minus, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_pomodoro_btn_minus, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_pomodoro_btn_minus, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_pomodoro_btn_minus, &lv_font_montserratMedium_25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_pomodoro_btn_minus, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_pomodoro_btn_minus, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_pomodoro_btn_minus, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_pomodoro_btn_minus, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_pomodoro_btn_minus, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_pomodoro_btn_minus, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_pomodoro_btn_minus, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_pomodoro_btn_minus, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_pomodoro_btn_minus, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_pomodoro_btn_minus, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_pomodoro_btn_minus, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_pomodoro_btn_minus, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_pomodoro_btn_minus, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_pomodoro_btn_minus, &lv_font_montserratMedium_25, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_pomodoro_btn_minus, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_pomodoro_btn_plus
    ui->screen_pomodoro_btn_plus = lv_btn_create(ui->screen_pomodoro);
    ui->screen_pomodoro_btn_plus_label = lv_label_create(ui->screen_pomodoro_btn_plus);
    lv_label_set_text(ui->screen_pomodoro_btn_plus_label, "+");
    lv_label_set_long_mode(ui->screen_pomodoro_btn_plus_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_pomodoro_btn_plus_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_pomodoro_btn_plus, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_pomodoro_btn_plus_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_pomodoro_btn_plus, 205, 112);
    lv_obj_set_size(ui->screen_pomodoro_btn_plus, 24, 18);

    //Write style for screen_pomodoro_btn_plus, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_pomodoro_btn_plus, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_pomodoro_btn_plus, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_pomodoro_btn_plus, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_pomodoro_btn_plus, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_pomodoro_btn_plus, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_pomodoro_btn_plus, &lv_font_montserratMedium_25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_pomodoro_btn_plus, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_pomodoro_btn_plus, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_pomodoro_btn_plus, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_pomodoro_btn_plus, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_pomodoro_btn_plus, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_pomodoro_btn_plus, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_pomodoro_btn_plus, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_pomodoro_btn_plus, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_pomodoro_btn_plus, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_pomodoro_btn_plus, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_pomodoro_btn_plus, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_pomodoro_btn_plus, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_pomodoro_btn_plus, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_pomodoro_btn_plus, &lv_font_montserratMedium_25, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_pomodoro_btn_plus, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_pomodoro_img_timeout
    ui->screen_pomodoro_img_timeout = lv_img_create(ui->screen_pomodoro);
    lv_obj_add_flag(ui->screen_pomodoro_img_timeout, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_pomodoro_img_timeout, &_AAA_TIME_BG1_alpha_296x133);
    lv_img_set_pivot(ui->screen_pomodoro_img_timeout, 50,50);
    lv_img_set_angle(ui->screen_pomodoro_img_timeout, 0);
    lv_obj_set_pos(ui->screen_pomodoro_img_timeout, 18, 22);
    lv_obj_set_size(ui->screen_pomodoro_img_timeout, 296, 133);

    //Write style for screen_pomodoro_img_timeout, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_pomodoro_img_timeout, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(ui->screen_pomodoro_img_timeout, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_pomodoro_img_timeout, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_pomodoro_img_timeout, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_pomodoro_img_timeout, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_pomodoro_btn_complete
    ui->screen_pomodoro_btn_complete = lv_btn_create(ui->screen_pomodoro);
    ui->screen_pomodoro_btn_complete_label = lv_label_create(ui->screen_pomodoro_btn_complete);
    lv_label_set_text(ui->screen_pomodoro_btn_complete_label, "");
    lv_label_set_long_mode(ui->screen_pomodoro_btn_complete_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_pomodoro_btn_complete_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_pomodoro_btn_complete, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_pomodoro_btn_complete_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_pomodoro_btn_complete, 37, 113);
    lv_obj_set_size(ui->screen_pomodoro_btn_complete, 121, 18);

    //Write style for screen_pomodoro_btn_complete, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_pomodoro_btn_complete, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_pomodoro_btn_complete, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_pomodoro_btn_complete, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_pomodoro_btn_complete, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_pomodoro_btn_complete, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_pomodoro_btn_complete, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_pomodoro_btn_complete, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_pomodoro_btn_complete, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_pomodoro_btn_complete, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_pomodoro_btn_complete, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_pomodoro_btn_complete, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_pomodoro_btn_complete, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_pomodoro_btn_complete, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_pomodoro_btn_complete, lv_color_hex(0xaad7ef), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_pomodoro_btn_complete, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_pomodoro_btn_complete, 3, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_pomodoro_btn_complete, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_pomodoro_btn_complete, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_pomodoro_btn_complete, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_pomodoro_btn_complete, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_pomodoro_btn_complete, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write style for screen_pomodoro_btn_complete, Part: LV_PART_MAIN, State: LV_STATE_DISABLED.
    lv_obj_set_style_bg_opa(ui->screen_pomodoro_btn_complete, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_border_width(ui->screen_pomodoro_btn_complete, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_radius(ui->screen_pomodoro_btn_complete, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_shadow_width(ui->screen_pomodoro_btn_complete, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_text_color(ui->screen_pomodoro_btn_complete, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_text_font(ui->screen_pomodoro_btn_complete, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_text_opa(ui->screen_pomodoro_btn_complete, 255, LV_PART_MAIN|LV_STATE_DISABLED);

    //Write codes screen_pomodoro_btn_continue
    ui->screen_pomodoro_btn_continue = lv_btn_create(ui->screen_pomodoro);
    ui->screen_pomodoro_btn_continue_label = lv_label_create(ui->screen_pomodoro_btn_continue);
    lv_label_set_text(ui->screen_pomodoro_btn_continue_label, "");
    lv_label_set_long_mode(ui->screen_pomodoro_btn_continue_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_pomodoro_btn_continue_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_pomodoro_btn_continue, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_pomodoro_btn_continue_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_pomodoro_btn_continue, 167, 113);
    lv_obj_set_size(ui->screen_pomodoro_btn_continue, 121, 18);

    //Write style for screen_pomodoro_btn_continue, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_pomodoro_btn_continue, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_pomodoro_btn_continue, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_pomodoro_btn_continue, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_pomodoro_btn_continue, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_pomodoro_btn_continue, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_pomodoro_btn_continue, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_pomodoro_btn_continue, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_pomodoro_btn_continue, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_pomodoro_btn_continue, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_pomodoro_btn_continue, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_pomodoro_btn_continue, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_pomodoro_btn_continue, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_pomodoro_btn_continue, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_pomodoro_btn_continue, lv_color_hex(0xaad7ef), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_pomodoro_btn_continue, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_pomodoro_btn_continue, 3, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_pomodoro_btn_continue, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_pomodoro_btn_continue, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_pomodoro_btn_continue, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_pomodoro_btn_continue, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_pomodoro_btn_continue, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write style for screen_pomodoro_btn_continue, Part: LV_PART_MAIN, State: LV_STATE_DISABLED.
    lv_obj_set_style_bg_opa(ui->screen_pomodoro_btn_continue, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_border_width(ui->screen_pomodoro_btn_continue, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_radius(ui->screen_pomodoro_btn_continue, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_shadow_width(ui->screen_pomodoro_btn_continue, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_text_color(ui->screen_pomodoro_btn_continue, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_text_font(ui->screen_pomodoro_btn_continue, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_text_opa(ui->screen_pomodoro_btn_continue, 255, LV_PART_MAIN|LV_STATE_DISABLED);

    //The custom code of screen_pomodoro.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_pomodoro);

    //Init events for screen.
    events_init_screen_pomodoro(ui);
}
