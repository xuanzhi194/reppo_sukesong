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
    lv_obj_set_style_bg_img_src(ui->screen_timing, &_AAA_clock_bg_320x170, LV_PART_MAIN|LV_STATE_DEFAULT);
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
    lv_obj_set_pos(ui->screen_timing_label_realtime, 17, 43);
    lv_obj_set_size(ui->screen_timing_label_realtime, 156, 56);

    //Write style for screen_timing_label_realtime, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_timing_label_realtime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_timing_label_realtime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_timing_label_realtime, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_timing_label_realtime, &lv_font_SourceHanSansSC_Medium_52, LV_PART_MAIN|LV_STATE_DEFAULT);
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
    lv_obj_set_pos(ui->screen_timing_label_day, 198, 72);
    lv_obj_set_size(ui->screen_timing_label_day, 97, 19);

    //Write style for screen_timing_label_day, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_timing_label_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_timing_label_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_timing_label_day, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_timing_label_day, &lv_font_SourceHanSansSC_Medium_17, LV_PART_MAIN|LV_STATE_DEFAULT);
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
    lv_obj_set_pos(ui->screen_timing_label_date, 195, 49);
    lv_obj_set_size(ui->screen_timing_label_date, 97, 16);

    //Write style for screen_timing_label_date, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_timing_label_date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_timing_label_date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_timing_label_date, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_timing_label_date, &lv_font_SourceHanSansSC_Medium_17, LV_PART_MAIN|LV_STATE_DEFAULT);
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

    //Write codes screen_timing_bar_battery
    ui->screen_timing_bar_battery = lv_bar_create(ui->screen_timing);
    lv_obj_set_style_anim_time(ui->screen_timing_bar_battery, 1000, 0);
    lv_bar_set_mode(ui->screen_timing_bar_battery, LV_BAR_MODE_NORMAL);
    lv_bar_set_range(ui->screen_timing_bar_battery, 0, 100);
    lv_bar_set_value(ui->screen_timing_bar_battery, 50, LV_ANIM_OFF);
    lv_obj_set_pos(ui->screen_timing_bar_battery, 32, 127);
    lv_obj_set_size(ui->screen_timing_bar_battery, 257, 2);

    //Write style for screen_timing_bar_battery, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_timing_bar_battery, 98, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_timing_bar_battery, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_timing_bar_battery, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_timing_bar_battery, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_timing_bar_battery, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_timing_bar_battery, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_timing_bar_battery, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_timing_bar_battery, lv_color_hex(0x274a6e), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_timing_bar_battery, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_timing_bar_battery, 10, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write codes screen_timing_img_baticon
    ui->screen_timing_img_baticon = lv_img_create(ui->screen_timing);
    lv_obj_add_flag(ui->screen_timing_img_baticon, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_timing_img_baticon, &_AAA_shandian_icon_alpha_9x13);
    lv_img_set_pivot(ui->screen_timing_img_baticon, 50,50);
    lv_img_set_angle(ui->screen_timing_img_baticon, 0);
    lv_obj_set_pos(ui->screen_timing_img_baticon, 285, 109);
    lv_obj_set_size(ui->screen_timing_img_baticon, 9, 13);

    //Write style for screen_timing_img_baticon, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_timing_img_baticon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_timing_img_baticon, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_timing_img_baticon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_timing_img_baticon, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_timing_img_wificonnect
    ui->screen_timing_img_wificonnect = lv_img_create(ui->screen_timing);
    lv_obj_add_flag(ui->screen_timing_img_wificonnect, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_timing_img_wificonnect, &_AAA_clock_wifi_icon_alpha_14x14);
    lv_img_set_pivot(ui->screen_timing_img_wificonnect, 50,50);
    lv_img_set_angle(ui->screen_timing_img_wificonnect, 0);
    lv_obj_set_pos(ui->screen_timing_img_wificonnect, 288, 7);
    lv_obj_set_size(ui->screen_timing_img_wificonnect, 14, 14);

    //Write style for screen_timing_img_wificonnect, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_timing_img_wificonnect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_timing_img_wificonnect, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_timing_img_wificonnect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_timing_img_wificonnect, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_timing_img_wifidis
    ui->screen_timing_img_wifidis = lv_img_create(ui->screen_timing);
    lv_obj_add_flag(ui->screen_timing_img_wifidis, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_timing_img_wifidis, &_AAA_clock_wifiduan_icon_alpha_15x15);
    lv_img_set_pivot(ui->screen_timing_img_wifidis, 50,50);
    lv_img_set_angle(ui->screen_timing_img_wifidis, 0);
    lv_obj_set_pos(ui->screen_timing_img_wifidis, 288, 7);
    lv_obj_set_size(ui->screen_timing_img_wifidis, 15, 15);

    //Write style for screen_timing_img_wifidis, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_timing_img_wifidis, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_timing_img_wifidis, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_timing_img_wifidis, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_timing_img_wifidis, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_timing.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_timing);

    //Init events for screen.
    events_init_screen_timing(ui);
}
