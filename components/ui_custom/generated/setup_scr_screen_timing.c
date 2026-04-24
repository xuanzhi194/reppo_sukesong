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



void setup_scr_screen_timing(lv_ui *ui)
{
    //Write codes screen_timing
    ui->screen_timing = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_timing, 320, 170);
    lv_obj_set_scrollbar_mode(ui->screen_timing, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_timing, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_timing, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_timing, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_timing, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_timing, &_A_timing_BG_320x170, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_timing, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_timing, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_timing_btn_main
    ui->screen_timing_btn_main = lv_btn_create(ui->screen_timing);
    ui->screen_timing_btn_main_label = lv_label_create(ui->screen_timing_btn_main);
    lv_label_set_text(ui->screen_timing_btn_main_label, "");
    lv_label_set_long_mode(ui->screen_timing_btn_main_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_timing_btn_main_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_timing_btn_main, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_timing_btn_main_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_timing_btn_main, 0, 0);
    lv_obj_set_size(ui->screen_timing_btn_main, 320, 170);

    //Write style for screen_timing_btn_main, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_timing_btn_main, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_timing_btn_main, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_timing_btn_main, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_timing_btn_main, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_timing_btn_main, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_timing_btn_main, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_timing_btn_main, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_timing_btn_main, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_timing_btn_main, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_timing_btn_main, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_timing_btn_main, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_timing_btn_main, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_timing_btn_main, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_timing_btn_main, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_timing_btn_main, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_timing_btn_main, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_timing_label_realtime
    ui->screen_timing_label_realtime = lv_label_create(ui->screen_timing);
    lv_label_set_text(ui->screen_timing_label_realtime, "10:32");
    lv_label_set_long_mode(ui->screen_timing_label_realtime, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_timing_label_realtime, 63, 38);
    lv_obj_set_size(ui->screen_timing_label_realtime, 192, 76);

    //Write style for screen_timing_label_realtime, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_timing_label_realtime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_timing_label_realtime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_timing_label_realtime, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_timing_label_realtime, &lv_font_Abel_regular_83, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_timing_label_realtime, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_timing_label_realtime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_timing_label_realtime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_timing_label_realtime, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_timing_label_realtime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_timing_label_realtime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_timing_label_realtime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_timing_label_realtime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_timing_label_realtime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_timing_label_realtime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_timing_label_day
    ui->screen_timing_label_day = lv_label_create(ui->screen_timing);
    lv_label_set_text(ui->screen_timing_label_day, "Saturday");
    lv_label_set_long_mode(ui->screen_timing_label_day, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_timing_label_day, 173, 137);
    lv_obj_set_size(ui->screen_timing_label_day, 60, 16);

    //Write style for screen_timing_label_day, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_timing_label_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_timing_label_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_timing_label_day, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_timing_label_day, &lv_font_Abel_regular_15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_timing_label_day, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_timing_label_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_timing_label_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_timing_label_day, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_timing_label_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_timing_label_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_timing_label_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_timing_label_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_timing_label_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_timing_label_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_timing_label_date
    ui->screen_timing_label_date = lv_label_create(ui->screen_timing);
    lv_label_set_text(ui->screen_timing_label_date, "2025/12/25");
    lv_label_set_long_mode(ui->screen_timing_label_date, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_timing_label_date, 95, 137);
    lv_obj_set_size(ui->screen_timing_label_date, 82, 16);

    //Write style for screen_timing_label_date, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_timing_label_date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_timing_label_date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_timing_label_date, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_timing_label_date, &lv_font_Abel_regular_15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_timing_label_date, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_timing_label_date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_timing_label_date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_timing_label_date, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_timing_label_date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_timing_label_date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_timing_label_date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_timing_label_date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_timing_label_date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_timing_label_date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_timing_img_night
    ui->screen_timing_img_night = lv_img_create(ui->screen_timing);
    lv_obj_add_flag(ui->screen_timing_img_night, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_timing_img_night, &_night_alpha_25x25);
    lv_img_set_pivot(ui->screen_timing_img_night, 50,50);
    lv_img_set_angle(ui->screen_timing_img_night, 0);
    lv_obj_set_pos(ui->screen_timing_img_night, 280, 130);
    lv_obj_set_size(ui->screen_timing_img_night, 25, 25);
    lv_obj_add_flag(ui->screen_timing_img_night, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_timing_img_night, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_timing_img_night, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_timing_img_night, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_timing_img_night, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_timing_img_night, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_timing_img_night, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_timing_img_morning
    ui->screen_timing_img_morning = lv_img_create(ui->screen_timing);
    lv_obj_add_flag(ui->screen_timing_img_morning, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_timing_img_morning, &_morning_alpha_30x30);
    lv_img_set_pivot(ui->screen_timing_img_morning, 50,50);
    lv_img_set_angle(ui->screen_timing_img_morning, 0);
    lv_obj_set_pos(ui->screen_timing_img_morning, 278, 130);
    lv_obj_set_size(ui->screen_timing_img_morning, 30, 30);

    //Write style for screen_timing_img_morning, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_timing_img_morning, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_timing_img_morning, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_timing_img_morning, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_timing_img_morning, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_timing.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_timing);

    //Init events for screen.
    events_init_screen_timing(ui);
}
