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



void setup_scr_screen_setting(lv_ui *ui)
{
    //Write codes screen_setting
    ui->screen_setting = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_setting, 320, 170);
    lv_obj_set_scrollbar_mode(ui->screen_setting, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_setting, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_setting, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_setting, &_AAA_reppo_setting_bg_320x170, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_setting, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor(ui->screen_setting, lv_color_hex(0x7987a6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_setting, 21, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_setting_btn_back
    ui->screen_setting_btn_back = lv_btn_create(ui->screen_setting);
    ui->screen_setting_btn_back_label = lv_label_create(ui->screen_setting_btn_back);
    lv_label_set_text(ui->screen_setting_btn_back_label, "");
    lv_label_set_long_mode(ui->screen_setting_btn_back_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_setting_btn_back_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_setting_btn_back, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_setting_btn_back_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_setting_btn_back, 7, 8);
    lv_obj_set_size(ui->screen_setting_btn_back, 17, 17);

    //Write style for screen_setting_btn_back, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_setting_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_setting_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_setting_btn_back, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_setting_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_setting_btn_back, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_setting_btn_back, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_setting_btn_back, 192, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_setting_btn_back, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_setting_btn_back, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_setting_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_setting_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_setting_btn_back, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_setting_btn_back, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_setting_btn_back, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_setting_btn_back, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_setting_btn_back, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_setting_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_setting_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_setting_btn_back, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_setting_btn_back, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_setting_btn_back, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_setting_btn_general
    ui->screen_setting_btn_general = lv_btn_create(ui->screen_setting);
    ui->screen_setting_btn_general_label = lv_label_create(ui->screen_setting_btn_general);
    lv_label_set_text(ui->screen_setting_btn_general_label, "General");
    lv_label_set_long_mode(ui->screen_setting_btn_general_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_setting_btn_general_label, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_setting_btn_general, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_setting_btn_general_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_setting_btn_general, 19, 135);
    lv_obj_set_size(ui->screen_setting_btn_general, 95, 16);

    //Write style for screen_setting_btn_general, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_setting_btn_general, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_setting_btn_general, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_setting_btn_general, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_setting_btn_general, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_setting_btn_general, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_setting_btn_general, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_setting_btn_general, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_setting_btn_general, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_setting_btn_general, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_setting_btn_general, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(ui->screen_setting_btn_general, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_grad_dir(ui->screen_setting_btn_general, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_setting_btn_general, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_setting_btn_general, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_setting_btn_general, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_setting_btn_general, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_setting_btn_general, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_setting_btn_general, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_setting_btn_brightness
    ui->screen_setting_btn_brightness = lv_btn_create(ui->screen_setting);
    ui->screen_setting_btn_brightness_label = lv_label_create(ui->screen_setting_btn_brightness);
    lv_label_set_text(ui->screen_setting_btn_brightness_label, "Brightness");
    lv_label_set_long_mode(ui->screen_setting_btn_brightness_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_setting_btn_brightness_label, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_setting_btn_brightness, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_setting_btn_brightness_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_setting_btn_brightness, 19, 110);
    lv_obj_set_size(ui->screen_setting_btn_brightness, 95, 16);

    //Write style for screen_setting_btn_brightness, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_setting_btn_brightness, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_setting_btn_brightness, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_setting_btn_brightness, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_setting_btn_brightness, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_setting_btn_brightness, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_setting_btn_brightness, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_setting_btn_brightness, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_setting_btn_brightness, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_setting_btn_brightness, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_setting_btn_brightness, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(ui->screen_setting_btn_brightness, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_grad_dir(ui->screen_setting_btn_brightness, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_setting_btn_brightness, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_setting_btn_brightness, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_setting_btn_brightness, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_setting_btn_brightness, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_setting_btn_brightness, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_setting_btn_brightness, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_setting_btn_volume
    ui->screen_setting_btn_volume = lv_btn_create(ui->screen_setting);
    ui->screen_setting_btn_volume_label = lv_label_create(ui->screen_setting_btn_volume);
    lv_label_set_text(ui->screen_setting_btn_volume_label, "Volume");
    lv_label_set_long_mode(ui->screen_setting_btn_volume_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_setting_btn_volume_label, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_setting_btn_volume, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_setting_btn_volume_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_setting_btn_volume, 19, 85);
    lv_obj_set_size(ui->screen_setting_btn_volume, 95, 16);

    //Write style for screen_setting_btn_volume, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_setting_btn_volume, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_setting_btn_volume, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_setting_btn_volume, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_setting_btn_volume, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_setting_btn_volume, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_setting_btn_volume, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_setting_btn_volume, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_setting_btn_volume, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_setting_btn_volume, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_setting_btn_volume, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(ui->screen_setting_btn_volume, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_grad_dir(ui->screen_setting_btn_volume, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_setting_btn_volume, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_setting_btn_volume, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_setting_btn_volume, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_setting_btn_volume, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_setting_btn_volume, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_setting_btn_volume, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_setting_btn_ble
    ui->screen_setting_btn_ble = lv_btn_create(ui->screen_setting);
    ui->screen_setting_btn_ble_label = lv_label_create(ui->screen_setting_btn_ble);
    lv_label_set_text(ui->screen_setting_btn_ble_label, "BlueTooth");
    lv_label_set_long_mode(ui->screen_setting_btn_ble_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_setting_btn_ble_label, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_setting_btn_ble, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_setting_btn_ble_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_setting_btn_ble, 19, 60);
    lv_obj_set_size(ui->screen_setting_btn_ble, 95, 16);

    //Write style for screen_setting_btn_ble, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_setting_btn_ble, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_setting_btn_ble, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_setting_btn_ble, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_setting_btn_ble, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_setting_btn_ble, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_setting_btn_ble, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_setting_btn_ble, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_setting_btn_ble, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_setting_btn_ble, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_setting_btn_ble, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(ui->screen_setting_btn_ble, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_grad_dir(ui->screen_setting_btn_ble, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_setting_btn_ble, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_setting_btn_ble, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_setting_btn_ble, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_setting_btn_ble, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_setting_btn_ble, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_setting_btn_ble, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_setting_btn_wifi
    ui->screen_setting_btn_wifi = lv_btn_create(ui->screen_setting);
    ui->screen_setting_btn_wifi_label = lv_label_create(ui->screen_setting_btn_wifi);
    lv_label_set_text(ui->screen_setting_btn_wifi_label, "WIFI");
    lv_label_set_long_mode(ui->screen_setting_btn_wifi_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_setting_btn_wifi_label, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_setting_btn_wifi, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_setting_btn_wifi_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_setting_btn_wifi, 19, 35);
    lv_obj_set_size(ui->screen_setting_btn_wifi, 95, 16);

    //Write style for screen_setting_btn_wifi, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_setting_btn_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_setting_btn_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_setting_btn_wifi, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_setting_btn_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_setting_btn_wifi, lv_color_hex(0x274A6E), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_setting_btn_wifi, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_setting_btn_wifi, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_setting_btn_wifi, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_setting_btn_wifi, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_setting_btn_wifi, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(ui->screen_setting_btn_wifi, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_grad_dir(ui->screen_setting_btn_wifi, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_setting_btn_wifi, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_setting_btn_wifi, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_setting_btn_wifi, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_setting_btn_wifi, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_setting_btn_wifi, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_setting_btn_wifi, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write style for screen_setting_btn_wifi, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_bg_opa(ui->screen_setting_btn_wifi, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_border_width(ui->screen_setting_btn_wifi, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_radius(ui->screen_setting_btn_wifi, 5, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->screen_setting_btn_wifi, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->screen_setting_btn_wifi, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->screen_setting_btn_wifi, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->screen_setting_btn_wifi, 255, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write codes screen_setting_img_volume_open
    ui->screen_setting_img_volume_open = lv_img_create(ui->screen_setting);
    lv_obj_add_flag(ui->screen_setting_img_volume_open, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_setting_img_volume_open, &_AAA_reppo_volume_kai_icon_alpha_93x75);
    lv_img_set_pivot(ui->screen_setting_img_volume_open, 50,50);
    lv_img_set_angle(ui->screen_setting_img_volume_open, 0);
    lv_obj_set_pos(ui->screen_setting_img_volume_open, 173, 56);
    lv_obj_set_size(ui->screen_setting_img_volume_open, 93, 75);
    lv_obj_add_flag(ui->screen_setting_img_volume_open, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_setting_img_volume_open, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_setting_img_volume_open, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_setting_img_volume_open, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_setting_img_volume_open, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_setting_img_volume_open, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_setting_img_volume_close
    ui->screen_setting_img_volume_close = lv_img_create(ui->screen_setting);
    lv_obj_add_flag(ui->screen_setting_img_volume_close, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_setting_img_volume_close, &_AAA_reppo_volume_guan_icon_alpha_93x75);
    lv_img_set_pivot(ui->screen_setting_img_volume_close, 50,50);
    lv_img_set_angle(ui->screen_setting_img_volume_close, 0);
    lv_obj_set_pos(ui->screen_setting_img_volume_close, 173, 56);
    lv_obj_set_size(ui->screen_setting_img_volume_close, 93, 75);
    lv_obj_add_flag(ui->screen_setting_img_volume_close, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_setting_img_volume_close, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_setting_img_volume_close, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_setting_img_volume_close, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_setting_img_volume_close, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_setting_img_volume_close, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_setting_img_wifi_open
    ui->screen_setting_img_wifi_open = lv_img_create(ui->screen_setting);
    lv_obj_add_flag(ui->screen_setting_img_wifi_open, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_setting_img_wifi_open, &_AAA_clock_wifi_icon_alpha_100x100);
    lv_img_set_pivot(ui->screen_setting_img_wifi_open, 50,50);
    lv_img_set_angle(ui->screen_setting_img_wifi_open, 0);
    lv_obj_set_pos(ui->screen_setting_img_wifi_open, 173, 44);
    lv_obj_set_size(ui->screen_setting_img_wifi_open, 100, 100);
    lv_obj_add_flag(ui->screen_setting_img_wifi_open, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_setting_img_wifi_open, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_setting_img_wifi_open, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_setting_img_wifi_open, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_setting_img_wifi_open, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_setting_img_wifi_open, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_setting_img_wifi_close
    ui->screen_setting_img_wifi_close = lv_img_create(ui->screen_setting);
    lv_obj_add_flag(ui->screen_setting_img_wifi_close, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_setting_img_wifi_close, &_AAA_clock_wifiduan_icon_alpha_100x100);
    lv_img_set_pivot(ui->screen_setting_img_wifi_close, 50,50);
    lv_img_set_angle(ui->screen_setting_img_wifi_close, 0);
    lv_obj_set_pos(ui->screen_setting_img_wifi_close, 173, 41);
    lv_obj_set_size(ui->screen_setting_img_wifi_close, 100, 100);
    lv_obj_add_flag(ui->screen_setting_img_wifi_close, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_setting_img_wifi_close, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_setting_img_wifi_close, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_setting_img_wifi_close, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_setting_img_wifi_close, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_setting_img_wifi_close, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_setting_img_ble
    ui->screen_setting_img_ble = lv_img_create(ui->screen_setting);
    lv_obj_add_flag(ui->screen_setting_img_ble, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_setting_img_ble, &_AAA_reppo_bluetooth_icon_alpha_57x92);
    lv_img_set_pivot(ui->screen_setting_img_ble, 50,50);
    lv_img_set_angle(ui->screen_setting_img_ble, 0);
    lv_obj_set_pos(ui->screen_setting_img_ble, 191, 44);
    lv_obj_set_size(ui->screen_setting_img_ble, 57, 92);
    lv_obj_add_flag(ui->screen_setting_img_ble, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_setting_img_ble, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_setting_img_ble, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_setting_img_ble, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_setting_img_ble, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_setting_img_ble, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_setting_img_ble_dis
    ui->screen_setting_img_ble_dis = lv_img_create(ui->screen_setting);
    lv_obj_add_flag(ui->screen_setting_img_ble_dis, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_setting_img_ble_dis, &_AAA_reppo_ble_dis_alpha_100x100);
    lv_img_set_pivot(ui->screen_setting_img_ble_dis, 50,50);
    lv_img_set_angle(ui->screen_setting_img_ble_dis, 0);
    lv_obj_set_pos(ui->screen_setting_img_ble_dis, 173, 41);
    lv_obj_set_size(ui->screen_setting_img_ble_dis, 100, 100);
    lv_obj_add_flag(ui->screen_setting_img_ble_dis, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_setting_img_ble_dis, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_setting_img_ble_dis, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_setting_img_ble_dis, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_setting_img_ble_dis, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_setting_img_ble_dis, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_setting_img_brightness
    ui->screen_setting_img_brightness = lv_img_create(ui->screen_setting);
    lv_obj_add_flag(ui->screen_setting_img_brightness, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_setting_img_brightness, &_AAA_setting_brightness_alpha_105x105);
    lv_img_set_pivot(ui->screen_setting_img_brightness, 50,50);
    lv_img_set_angle(ui->screen_setting_img_brightness, 0);
    lv_obj_set_pos(ui->screen_setting_img_brightness, 173, 41);
    lv_obj_set_size(ui->screen_setting_img_brightness, 105, 105);
    lv_obj_add_flag(ui->screen_setting_img_brightness, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_setting_img_brightness, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_setting_img_brightness, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_setting_img_brightness, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_setting_img_brightness, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_setting_img_brightness, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_setting_label_about
    ui->screen_setting_label_about = lv_label_create(ui->screen_setting);
    lv_label_set_text(ui->screen_setting_label_about, "关于Reppo");
    lv_label_set_long_mode(ui->screen_setting_label_about, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_setting_label_about, 167, 35);
    lv_obj_set_size(ui->screen_setting_label_about, 109, 18);

    //Write style for screen_setting_label_about, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_setting_label_about, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_setting_label_about, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_setting_label_about, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_setting_label_about, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_setting_label_about, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_setting_label_about, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_setting_label_about, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_setting_label_about, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_setting_label_about, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_setting_label_about, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_setting_label_about, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_setting_label_about, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_setting_label_about, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_setting_label_about, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_setting_label_txt1
    ui->screen_setting_label_txt1 = lv_label_create(ui->screen_setting);
    lv_label_set_text(ui->screen_setting_label_txt1, "设备名称");
    lv_label_set_long_mode(ui->screen_setting_label_txt1, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_setting_label_txt1, 139, 60);
    lv_obj_set_size(ui->screen_setting_label_txt1, 60, 16);

    //Write style for screen_setting_label_txt1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_setting_label_txt1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_setting_label_txt1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_setting_label_txt1, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_setting_label_txt1, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_setting_label_txt1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_setting_label_txt1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_setting_label_txt1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_setting_label_txt1, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_setting_label_txt1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_setting_label_txt1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_setting_label_txt1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_setting_label_txt1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_setting_label_txt1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_setting_label_txt1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_setting_label_txt2
    ui->screen_setting_label_txt2 = lv_label_create(ui->screen_setting);
    lv_label_set_text(ui->screen_setting_label_txt2, "型号名称");
    lv_label_set_long_mode(ui->screen_setting_label_txt2, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_setting_label_txt2, 139, 85);
    lv_obj_set_size(ui->screen_setting_label_txt2, 60, 16);

    //Write style for screen_setting_label_txt2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_setting_label_txt2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_setting_label_txt2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_setting_label_txt2, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_setting_label_txt2, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_setting_label_txt2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_setting_label_txt2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_setting_label_txt2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_setting_label_txt2, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_setting_label_txt2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_setting_label_txt2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_setting_label_txt2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_setting_label_txt2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_setting_label_txt2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_setting_label_txt2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_setting_label_txt3
    ui->screen_setting_label_txt3 = lv_label_create(ui->screen_setting);
    lv_label_set_text(ui->screen_setting_label_txt3, "Reppo OS");
    lv_label_set_long_mode(ui->screen_setting_label_txt3, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_setting_label_txt3, 139, 110);
    lv_obj_set_size(ui->screen_setting_label_txt3, 72, 16);

    //Write style for screen_setting_label_txt3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_setting_label_txt3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_setting_label_txt3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_setting_label_txt3, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_setting_label_txt3, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_setting_label_txt3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_setting_label_txt3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_setting_label_txt3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_setting_label_txt3, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_setting_label_txt3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_setting_label_txt3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_setting_label_txt3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_setting_label_txt3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_setting_label_txt3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_setting_label_txt3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_setting_label_txt4
    ui->screen_setting_label_txt4 = lv_label_create(ui->screen_setting);
    lv_label_set_text(ui->screen_setting_label_txt4, "硬件版本");
    lv_label_set_long_mode(ui->screen_setting_label_txt4, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_setting_label_txt4, 139, 135);
    lv_obj_set_size(ui->screen_setting_label_txt4, 60, 16);

    //Write style for screen_setting_label_txt4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_setting_label_txt4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_setting_label_txt4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_setting_label_txt4, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_setting_label_txt4, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_setting_label_txt4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_setting_label_txt4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_setting_label_txt4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_setting_label_txt4, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_setting_label_txt4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_setting_label_txt4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_setting_label_txt4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_setting_label_txt4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_setting_label_txt4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_setting_label_txt4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_setting_label_info1
    ui->screen_setting_label_info1 = lv_label_create(ui->screen_setting);
    lv_label_set_text(ui->screen_setting_label_info1, "Reppo");
    lv_label_set_long_mode(ui->screen_setting_label_info1, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_setting_label_info1, 232, 60);
    lv_obj_set_size(ui->screen_setting_label_info1, 70, 16);

    //Write style for screen_setting_label_info1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_setting_label_info1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_setting_label_info1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_setting_label_info1, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_setting_label_info1, &lv_font_SourceHanSansSC_Medium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_setting_label_info1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_setting_label_info1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_setting_label_info1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_setting_label_info1, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_setting_label_info1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_setting_label_info1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_setting_label_info1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_setting_label_info1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_setting_label_info1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_setting_label_info1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_setting_label_info2
    ui->screen_setting_label_info2 = lv_label_create(ui->screen_setting);
    lv_label_set_text(ui->screen_setting_label_info2, "Reppo R1");
    lv_label_set_long_mode(ui->screen_setting_label_info2, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_setting_label_info2, 232, 85);
    lv_obj_set_size(ui->screen_setting_label_info2, 70, 16);

    //Write style for screen_setting_label_info2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_setting_label_info2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_setting_label_info2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_setting_label_info2, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_setting_label_info2, &lv_font_SourceHanSansSC_Medium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_setting_label_info2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_setting_label_info2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_setting_label_info2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_setting_label_info2, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_setting_label_info2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_setting_label_info2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_setting_label_info2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_setting_label_info2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_setting_label_info2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_setting_label_info2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_setting_label_info3
    ui->screen_setting_label_info3 = lv_label_create(ui->screen_setting);
    lv_label_set_text(ui->screen_setting_label_info3, "1.0");
    lv_label_set_long_mode(ui->screen_setting_label_info3, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_setting_label_info3, 232, 110);
    lv_obj_set_size(ui->screen_setting_label_info3, 70, 16);

    //Write style for screen_setting_label_info3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_setting_label_info3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_setting_label_info3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_setting_label_info3, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_setting_label_info3, &lv_font_SourceHanSansSC_Medium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_setting_label_info3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_setting_label_info3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_setting_label_info3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_setting_label_info3, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_setting_label_info3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_setting_label_info3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_setting_label_info3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_setting_label_info3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_setting_label_info3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_setting_label_info3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_setting_label_info4
    ui->screen_setting_label_info4 = lv_label_create(ui->screen_setting);
    lv_label_set_text(ui->screen_setting_label_info4, "3.5");
    lv_label_set_long_mode(ui->screen_setting_label_info4, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_setting_label_info4, 232, 135);
    lv_obj_set_size(ui->screen_setting_label_info4, 70, 16);

    //Write style for screen_setting_label_info4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_setting_label_info4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_setting_label_info4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_setting_label_info4, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_setting_label_info4, &lv_font_SourceHanSansSC_Medium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_setting_label_info4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_setting_label_info4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_setting_label_info4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_setting_label_info4, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_setting_label_info4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_setting_label_info4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_setting_label_info4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_setting_label_info4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_setting_label_info4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_setting_label_info4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_setting.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_setting);

    //Init events for screen.
    events_init_screen_setting(ui);
}
