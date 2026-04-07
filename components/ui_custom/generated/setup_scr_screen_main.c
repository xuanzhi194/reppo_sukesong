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
    lv_obj_set_style_bg_img_src(ui->screen_main, &_Reppo_bg1_320x170, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_main, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_main, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_img_wifi
    ui->screen_main_img_wifi = lv_img_create(ui->screen_main);
    lv_obj_add_flag(ui->screen_main_img_wifi, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_main_img_wifi, &_wifi_log_2_alpha_24x24);
    lv_img_set_pivot(ui->screen_main_img_wifi, 50,50);
    lv_img_set_angle(ui->screen_main_img_wifi, 0);
    lv_obj_set_pos(ui->screen_main_img_wifi, 236, 6);
    lv_obj_set_size(ui->screen_main_img_wifi, 24, 24);

    //Write style for screen_main_img_wifi, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_main_img_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_main_img_wifi, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_img_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_main_img_wifi, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_img_battery
    ui->screen_main_img_battery = lv_img_create(ui->screen_main);
    lv_obj_add_flag(ui->screen_main_img_battery, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_main_img_battery, &_battery_label_v3_alpha_18x18);
    lv_img_set_pivot(ui->screen_main_img_battery, 50,50);
    lv_img_set_angle(ui->screen_main_img_battery, 0);
    lv_obj_set_pos(ui->screen_main_img_battery, 272, 10);
    lv_obj_set_size(ui->screen_main_img_battery, 18, 18);

    //Write style for screen_main_img_battery, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_main_img_battery, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_main_img_battery, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_img_battery, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_main_img_battery, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_img_ble
    ui->screen_main_img_ble = lv_img_create(ui->screen_main);
    lv_obj_add_flag(ui->screen_main_img_ble, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_main_img_ble, &_ble_label_v3_alpha_14x14);
    lv_img_set_pivot(ui->screen_main_img_ble, 50,50);
    lv_img_set_angle(ui->screen_main_img_ble, 0);
    lv_obj_set_pos(ui->screen_main_img_ble, 256, 11);
    lv_obj_set_size(ui->screen_main_img_ble, 14, 14);

    //Write style for screen_main_img_ble, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_main_img_ble, 215, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(ui->screen_main_img_ble, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_main_img_ble, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_img_ble, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_main_img_ble, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_img_todolist
    ui->screen_main_img_todolist = lv_img_create(ui->screen_main);
    lv_obj_add_flag(ui->screen_main_img_todolist, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_main_img_todolist, &_A_todolist_BG1_alpha_60x80);
    lv_img_set_pivot(ui->screen_main_img_todolist, 50,50);
    lv_img_set_angle(ui->screen_main_img_todolist, 0);
    lv_obj_set_pos(ui->screen_main_img_todolist, 18, 45);
    lv_obj_set_size(ui->screen_main_img_todolist, 60, 80);

    //Write style for screen_main_img_todolist, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_main_img_todolist, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_main_img_todolist, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_img_todolist, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_main_img_todolist, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_img_word
    ui->screen_main_img_word = lv_img_create(ui->screen_main);
    lv_obj_add_flag(ui->screen_main_img_word, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_main_img_word, &_A_word_BG1_alpha_60x80);
    lv_img_set_pivot(ui->screen_main_img_word, 50,50);
    lv_img_set_angle(ui->screen_main_img_word, 0);
    lv_obj_set_pos(ui->screen_main_img_word, 93, 45);
    lv_obj_set_size(ui->screen_main_img_word, 60, 80);

    //Write style for screen_main_img_word, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_main_img_word, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_main_img_word, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_img_word, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_main_img_word, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_img_timer
    ui->screen_main_img_timer = lv_img_create(ui->screen_main);
    lv_obj_add_flag(ui->screen_main_img_timer, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_main_img_timer, &_A_time_BG1_alpha_60x80);
    lv_img_set_pivot(ui->screen_main_img_timer, 50,50);
    lv_img_set_angle(ui->screen_main_img_timer, 0);
    lv_obj_set_pos(ui->screen_main_img_timer, 170, 45);
    lv_obj_set_size(ui->screen_main_img_timer, 60, 80);

    //Write style for screen_main_img_timer, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_main_img_timer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_main_img_timer, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_img_timer, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_main_img_timer, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_img_setting
    ui->screen_main_img_setting = lv_img_create(ui->screen_main);
    lv_obj_add_flag(ui->screen_main_img_setting, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_main_img_setting, &_A_setting_BG1_alpha_60x80);
    lv_img_set_pivot(ui->screen_main_img_setting, 50,50);
    lv_img_set_angle(ui->screen_main_img_setting, 0);
    lv_obj_set_pos(ui->screen_main_img_setting, 246, 45);
    lv_obj_set_size(ui->screen_main_img_setting, 60, 80);

    //Write style for screen_main_img_setting, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_main_img_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_main_img_setting, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_img_setting, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_main_img_setting, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_btn_word
    ui->screen_main_btn_word = lv_btn_create(ui->screen_main);
    ui->screen_main_btn_word_label = lv_label_create(ui->screen_main_btn_word);
    lv_label_set_text(ui->screen_main_btn_word_label, "");
    lv_label_set_long_mode(ui->screen_main_btn_word_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_main_btn_word_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_main_btn_word, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_main_btn_word_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_main_btn_word, 95, 45);
    lv_obj_set_size(ui->screen_main_btn_word, 60, 80);

    //Write style for screen_main_btn_word, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_word, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_main_btn_word, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_btn_word, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_word, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_main_btn_word, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_btn_word, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_main_btn_word, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_main_btn_word, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_main_btn_word, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_word, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_main_btn_word, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_main_btn_word, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_word, 10, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_main_btn_word, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_main_btn_word, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_main_btn_word, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_main_btn_word, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_main_btn_word, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_main_btn_word, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_main_btn_word, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_main_btn_word, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_main_btn_timer
    ui->screen_main_btn_timer = lv_btn_create(ui->screen_main);
    ui->screen_main_btn_timer_label = lv_label_create(ui->screen_main_btn_timer);
    lv_label_set_text(ui->screen_main_btn_timer_label, "");
    lv_label_set_long_mode(ui->screen_main_btn_timer_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_main_btn_timer_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_main_btn_timer, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_main_btn_timer_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_main_btn_timer, 172, 45);
    lv_obj_set_size(ui->screen_main_btn_timer, 60, 80);

    //Write style for screen_main_btn_timer, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_timer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_main_btn_timer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_btn_timer, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_timer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_main_btn_timer, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_btn_timer, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_main_btn_timer, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_main_btn_timer, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_main_btn_timer, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_timer, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_main_btn_timer, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_main_btn_timer, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_timer, 10, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_main_btn_timer, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_main_btn_timer, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_main_btn_timer, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_main_btn_timer, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_main_btn_timer, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_main_btn_timer, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_main_btn_timer, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_main_btn_timer, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_main_btn_setting
    ui->screen_main_btn_setting = lv_btn_create(ui->screen_main);
    ui->screen_main_btn_setting_label = lv_label_create(ui->screen_main_btn_setting);
    lv_label_set_text(ui->screen_main_btn_setting_label, "");
    lv_label_set_long_mode(ui->screen_main_btn_setting_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_main_btn_setting_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_main_btn_setting, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_main_btn_setting_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_main_btn_setting, 248, 45);
    lv_obj_set_size(ui->screen_main_btn_setting, 60, 80);

    //Write style for screen_main_btn_setting, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_main_btn_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_btn_setting, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_main_btn_setting, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_btn_setting, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_main_btn_setting, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_main_btn_setting, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_main_btn_setting, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_setting, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_main_btn_setting, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_main_btn_setting, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_setting, 10, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_main_btn_setting, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_main_btn_setting, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_main_btn_setting, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_main_btn_setting, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_main_btn_setting, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_main_btn_setting, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_main_btn_setting, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_main_btn_setting, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_main_img_uptext
    ui->screen_main_img_uptext = lv_img_create(ui->screen_main);
    lv_obj_add_flag(ui->screen_main_img_uptext, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_main_img_uptext, &_probg4_alpha_160x15);
    lv_img_set_pivot(ui->screen_main_img_uptext, 50,50);
    lv_img_set_angle(ui->screen_main_img_uptext, 0);
    lv_obj_set_pos(ui->screen_main_img_uptext, 74, 10);
    lv_obj_set_size(ui->screen_main_img_uptext, 160, 15);

    //Write style for screen_main_img_uptext, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_main_img_uptext, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_main_img_uptext, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_img_uptext, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_main_img_uptext, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_label_uptext
    ui->screen_main_label_uptext = lv_label_create(ui->screen_main);
    lv_label_set_text_static(ui->screen_main_label_uptext, "Find your tempo in repose");
    lv_label_set_long_mode(ui->screen_main_label_uptext, LV_LABEL_LONG_SCROLL);
    lv_obj_set_pos(ui->screen_main_label_uptext, 77, 9);
    lv_obj_set_size(ui->screen_main_label_uptext, 155, 20);

    //Write style for screen_main_label_uptext, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_main_label_uptext, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_label_uptext, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_main_label_uptext, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_label_uptext, &lv_font_AlexBrush_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_main_label_uptext, 215, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_main_label_uptext, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_main_label_uptext, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_main_label_uptext, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_main_label_uptext, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_main_label_uptext, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_main_label_uptext, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_main_label_uptext, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_main_label_uptext, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_label_uptext, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_img_plane
    ui->screen_main_img_plane = lv_img_create(ui->screen_main);
    lv_obj_add_flag(ui->screen_main_img_plane, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_main_img_plane, &_plane_alpha_35x28);
    lv_img_set_pivot(ui->screen_main_img_plane, 0,0);
    lv_img_set_angle(ui->screen_main_img_plane, 150);
    lv_obj_set_pos(ui->screen_main_img_plane, 281, 131);
    lv_obj_set_size(ui->screen_main_img_plane, 35, 28);

    //Write style for screen_main_img_plane, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_main_img_plane, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(ui->screen_main_img_plane, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_main_img_plane, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_img_plane, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_main_img_plane, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_btn_plane
    ui->screen_main_btn_plane = lv_btn_create(ui->screen_main);
    ui->screen_main_btn_plane_label = lv_label_create(ui->screen_main_btn_plane);
    lv_label_set_text(ui->screen_main_btn_plane_label, "");
    lv_label_set_long_mode(ui->screen_main_btn_plane_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_main_btn_plane_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_main_btn_plane, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_main_btn_plane_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_main_btn_plane, 273, 134);
    lv_obj_set_size(ui->screen_main_btn_plane, 40, 29);

    //Write style for screen_main_btn_plane, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_plane, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_main_btn_plane, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_btn_plane, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_plane, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_main_btn_plane, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_btn_plane, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_main_btn_plane, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_main_btn_plane, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_main_btn_plane, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_plane, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_main_btn_plane, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_main_btn_plane, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_plane, 10, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_main_btn_plane, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_main_btn_plane, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_main_btn_plane, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_main_btn_plane, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_main_btn_plane, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_main_btn_plane, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_main_btn_plane, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_main_btn_plane, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_main_img_probg1
    ui->screen_main_img_probg1 = lv_img_create(ui->screen_main);
    lv_obj_add_flag(ui->screen_main_img_probg1, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_main_img_probg1, &_progress_bg2_alpha_76x18);
    lv_img_set_pivot(ui->screen_main_img_probg1, 50,50);
    lv_img_set_angle(ui->screen_main_img_probg1, 0);
    lv_obj_set_pos(ui->screen_main_img_probg1, 20, 149);
    lv_obj_set_size(ui->screen_main_img_probg1, 76, 18);

    //Write style for screen_main_img_probg1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_main_img_probg1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_main_img_probg1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_img_probg1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_main_img_probg1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_label_done
    ui->screen_main_label_done = lv_label_create(ui->screen_main);
    lv_label_set_text(ui->screen_main_label_done, "Done");
    lv_label_set_long_mode(ui->screen_main_label_done, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_main_label_done, 24, 131);
    lv_obj_set_size(ui->screen_main_label_done, 32, 12);

    //Write style for screen_main_label_done, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_main_label_done, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_label_done, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_main_label_done, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_label_done, &lv_font_Alatsi_Regular_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_main_label_done, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_main_label_done, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_main_label_done, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_main_label_done, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_main_label_done, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_main_label_done, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_main_label_done, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_main_label_done, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_main_label_done, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_label_done, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_label_donenum
    ui->screen_main_label_donenum = lv_label_create(ui->screen_main);
    lv_label_set_text(ui->screen_main_label_donenum, "5");
    lv_label_set_long_mode(ui->screen_main_label_donenum, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_main_label_donenum, 74, 129);
    lv_obj_set_size(ui->screen_main_label_donenum, 17, 13);

    //Write style for screen_main_label_donenum, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_main_label_donenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_label_donenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_main_label_donenum, lv_color_hex(0xb0d7ff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_label_donenum, &lv_font_Acme_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
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

    //Write codes screen_main_img_runpace
    ui->screen_main_img_runpace = lv_img_create(ui->screen_main);
    lv_obj_add_flag(ui->screen_main_img_runpace, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_main_img_runpace, &_progress_bg3_alpha_206x20);
    lv_img_set_pivot(ui->screen_main_img_runpace, 50,50);
    lv_img_set_angle(ui->screen_main_img_runpace, 0);
    lv_obj_set_pos(ui->screen_main_img_runpace, 100, 138);
    lv_obj_set_size(ui->screen_main_img_runpace, 206, 20);

    //Write style for screen_main_img_runpace, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_main_img_runpace, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(ui->screen_main_img_runpace, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_main_img_runpace, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_img_runpace, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_main_img_runpace, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_label_nodone
    ui->screen_main_label_nodone = lv_label_create(ui->screen_main);
    lv_label_set_text(ui->screen_main_label_nodone, "ToDo");
    lv_label_set_long_mode(ui->screen_main_label_nodone, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_main_label_nodone, 20, 152);
    lv_obj_set_size(ui->screen_main_label_nodone, 38, 12);

    //Write style for screen_main_label_nodone, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_main_label_nodone, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_label_nodone, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_main_label_nodone, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_label_nodone, &lv_font_Alatsi_Regular_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_main_label_nodone, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_main_label_nodone, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_main_label_nodone, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_main_label_nodone, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_main_label_nodone, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_main_label_nodone, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_main_label_nodone, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_main_label_nodone, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_main_label_nodone, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_label_nodone, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_img_probg2
    ui->screen_main_img_probg2 = lv_img_create(ui->screen_main);
    lv_obj_add_flag(ui->screen_main_img_probg2, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_main_img_probg2, &_progress_bg2_alpha_76x18);
    lv_img_set_pivot(ui->screen_main_img_probg2, 50,50);
    lv_img_set_angle(ui->screen_main_img_probg2, 0);
    lv_obj_set_pos(ui->screen_main_img_probg2, 20, 128);
    lv_obj_set_size(ui->screen_main_img_probg2, 76, 18);

    //Write style for screen_main_img_probg2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_main_img_probg2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_main_img_probg2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_img_probg2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_main_img_probg2, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_label_todonum
    ui->screen_main_label_todonum = lv_label_create(ui->screen_main);
    lv_label_set_text(ui->screen_main_label_todonum, "4");
    lv_label_set_long_mode(ui->screen_main_label_todonum, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_main_label_todonum, 74, 150);
    lv_obj_set_size(ui->screen_main_label_todonum, 18, 14);

    //Write style for screen_main_label_todonum, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_main_label_todonum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_label_todonum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_main_label_todonum, lv_color_hex(0xffceaa), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_label_todonum, &lv_font_Acme_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
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

    //Write codes screen_main_btn_todolist
    ui->screen_main_btn_todolist = lv_btn_create(ui->screen_main);
    ui->screen_main_btn_todolist_label = lv_label_create(ui->screen_main_btn_todolist);
    lv_label_set_text(ui->screen_main_btn_todolist_label, "");
    lv_label_set_long_mode(ui->screen_main_btn_todolist_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_main_btn_todolist_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_main_btn_todolist, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_main_btn_todolist_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_main_btn_todolist, 20, 45);
    lv_obj_set_size(ui->screen_main_btn_todolist, 60, 80);

    //Write style for screen_main_btn_todolist, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_todolist, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_main_btn_todolist, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_btn_todolist, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_todolist, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_main_btn_todolist, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_btn_todolist, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_main_btn_todolist, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_main_btn_todolist, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_main_btn_todolist, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_main_btn_todolist, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_main_btn_todolist, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_main_btn_todolist, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_main_btn_todolist, 10, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_main_btn_todolist, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_main_btn_todolist, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_main_btn_todolist, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_main_btn_todolist, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_main_btn_todolist, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_main_btn_todolist, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_main_btn_todolist, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_main_btn_todolist, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_main_label_battery
    ui->screen_main_label_battery = lv_label_create(ui->screen_main);
    lv_label_set_text(ui->screen_main_label_battery, "100%");
    lv_label_set_long_mode(ui->screen_main_label_battery, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_main_label_battery, 289, 13);
    lv_obj_set_size(ui->screen_main_label_battery, 31, 9);

    //Write style for screen_main_label_battery, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_main_label_battery, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_label_battery, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_main_label_battery, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_main_label_battery, &lv_font_arial_11, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_main_label_battery, 177, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_main_label_battery, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_main_label_battery, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_main_label_battery, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_main_label_battery, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_main_label_battery, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_main_label_battery, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_main_label_battery, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_main_label_battery, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_main_label_battery, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_main_img_gp1
    ui->screen_main_img_gp1 = lv_img_create(ui->screen_main);
    lv_obj_add_flag(ui->screen_main_img_gp1, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_main_img_gp1, &_reppo_main_gp1_alpha_75x16);
    lv_img_set_pivot(ui->screen_main_img_gp1, 50,50);
    lv_img_set_angle(ui->screen_main_img_gp1, 0);
    lv_obj_set_pos(ui->screen_main_img_gp1, 5, 9);
    lv_obj_set_size(ui->screen_main_img_gp1, 75, 16);

    //Write style for screen_main_img_gp1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_main_img_gp1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_main_img_gp1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_main_img_gp1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_main_img_gp1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_main.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_main);

    //Init events for screen.
    events_init_screen_main(ui);
}
