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



void setup_scr_screen_wificfg(lv_ui *ui)
{
    //Write codes screen_wificfg
    ui->screen_wificfg = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_wificfg, 320, 170);
    lv_obj_set_scrollbar_mode(ui->screen_wificfg, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_wificfg, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_wificfg, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_wificfg, &_AAA_wifi_lianjie_320x170, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_wificfg, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_wificfg, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wificfg_img_network
    ui->screen_wificfg_img_network = lv_img_create(ui->screen_wificfg);
    lv_obj_add_flag(ui->screen_wificfg_img_network, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_wificfg_img_network, &_pomorect_alpha_160x125);
    lv_img_set_pivot(ui->screen_wificfg_img_network, 50,50);
    lv_img_set_angle(ui->screen_wificfg_img_network, 0);
    lv_obj_set_pos(ui->screen_wificfg_img_network, 155, 33);
    lv_obj_set_size(ui->screen_wificfg_img_network, 160, 125);

    //Write style for screen_wificfg_img_network, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_wificfg_img_network, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_wificfg_img_network, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wificfg_img_network, 15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_wificfg_img_network, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wificfg_btn_back
    ui->screen_wificfg_btn_back = lv_btn_create(ui->screen_wificfg);
    ui->screen_wificfg_btn_back_label = lv_label_create(ui->screen_wificfg_btn_back);
    lv_label_set_text(ui->screen_wificfg_btn_back_label, "");
    lv_label_set_long_mode(ui->screen_wificfg_btn_back_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_wificfg_btn_back_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_wificfg_btn_back, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_wificfg_btn_back_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_wificfg_btn_back, 8, 9);
    lv_obj_set_size(ui->screen_wificfg_btn_back, 15, 16);

    //Write style for screen_wificfg_btn_back, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_wificfg_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_wificfg_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wificfg_btn_back, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wificfg_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wificfg_btn_back, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wificfg_btn_back, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wificfg_btn_back, 195, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wificfg_btn_back, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_wificfg_btn_back, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_wificfg_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_wificfg_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_wificfg_btn_back, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_wificfg_btn_back, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_wificfg_btn_back, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_wificfg_btn_back, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_wificfg_btn_back, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_wificfg_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_wificfg_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_wificfg_btn_back, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_wificfg_btn_back, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_wificfg_btn_back, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_wificfg_img_statusbg
    ui->screen_wificfg_img_statusbg = lv_img_create(ui->screen_wificfg);
    lv_obj_add_flag(ui->screen_wificfg_img_statusbg, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_wificfg_img_statusbg, &_AAA_wifi_kuang_alpha_145x125);
    lv_img_set_pivot(ui->screen_wificfg_img_statusbg, 50,50);
    lv_img_set_angle(ui->screen_wificfg_img_statusbg, 0);
    lv_obj_set_pos(ui->screen_wificfg_img_statusbg, 11, 33);
    lv_obj_set_size(ui->screen_wificfg_img_statusbg, 145, 125);

    //Write style for screen_wificfg_img_statusbg, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_wificfg_img_statusbg, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_wificfg_img_statusbg, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wificfg_img_statusbg, 15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_wificfg_img_statusbg, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wificfg_label_status
    ui->screen_wificfg_label_status = lv_label_create(ui->screen_wificfg);
    lv_label_set_text(ui->screen_wificfg_label_status, "Connection Status");
    lv_label_set_long_mode(ui->screen_wificfg_label_status, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_pos(ui->screen_wificfg_label_status, 25, 40);
    lv_obj_set_size(ui->screen_wificfg_label_status, 113, 14);

    //Write style for screen_wificfg_label_status, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_wificfg_label_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wificfg_label_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wificfg_label_status, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wificfg_label_status, &lv_font_SourceHanSansSC_Medium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wificfg_label_status, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_wificfg_label_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_wificfg_label_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wificfg_label_status, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_wificfg_label_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_wificfg_label_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_wificfg_label_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_wificfg_label_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_wificfg_label_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wificfg_label_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wificfg_img_wifiicon
    ui->screen_wificfg_img_wifiicon = lv_img_create(ui->screen_wificfg);
    lv_obj_add_flag(ui->screen_wificfg_img_wifiicon, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_wificfg_img_wifiicon, &_AAA_wifi_icons_alpha_110x110);
    lv_img_set_pivot(ui->screen_wificfg_img_wifiicon, 50,50);
    lv_img_set_angle(ui->screen_wificfg_img_wifiicon, 0);
    lv_obj_set_pos(ui->screen_wificfg_img_wifiicon, 28, 43);
    lv_obj_set_size(ui->screen_wificfg_img_wifiicon, 110, 110);

    //Write style for screen_wificfg_img_wifiicon, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_wificfg_img_wifiicon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_wificfg_img_wifiicon, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wificfg_img_wifiicon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_wificfg_img_wifiicon, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wificfg_label_status2
    ui->screen_wificfg_label_status2 = lv_label_create(ui->screen_wificfg);
    lv_label_set_text(ui->screen_wificfg_label_status2, "Disconnected");
    lv_label_set_long_mode(ui->screen_wificfg_label_status2, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_wificfg_label_status2, 38, 139);
    lv_obj_set_size(ui->screen_wificfg_label_status2, 91, 12);

    //Write style for screen_wificfg_label_status2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_wificfg_label_status2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wificfg_label_status2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wificfg_label_status2, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wificfg_label_status2, &lv_font_Alatsi_Regular_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wificfg_label_status2, 216, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_wificfg_label_status2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_wificfg_label_status2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wificfg_label_status2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_wificfg_label_status2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_wificfg_label_status2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_wificfg_label_status2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_wificfg_label_status2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_wificfg_label_status2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wificfg_label_status2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wificfg_img_wififalse
    ui->screen_wificfg_img_wififalse = lv_img_create(ui->screen_wificfg);
    lv_obj_add_flag(ui->screen_wificfg_img_wififalse, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_wificfg_img_wififalse, &_wifi_false_alpha_20x20);
    lv_img_set_pivot(ui->screen_wificfg_img_wififalse, 50,50);
    lv_img_set_angle(ui->screen_wificfg_img_wififalse, 0);
    lv_obj_set_pos(ui->screen_wificfg_img_wififalse, 111, 60);
    lv_obj_set_size(ui->screen_wificfg_img_wififalse, 20, 20);

    //Write style for screen_wificfg_img_wififalse, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_wificfg_img_wififalse, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_wificfg_img_wififalse, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wificfg_img_wififalse, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_wificfg_img_wififalse, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wificfg_list_1
    ui->screen_wificfg_list_1 = lv_list_create(ui->screen_wificfg);
    ui->screen_wificfg_list_1_item0 = lv_list_add_btn(ui->screen_wificfg_list_1, LV_SYMBOL_WIFI, "wifi1_dummy");
    ui->screen_wificfg_list_1_item1 = lv_list_add_btn(ui->screen_wificfg_list_1, LV_SYMBOL_WIFI, "wifi2_dummy");
    ui->screen_wificfg_list_1_item2 = lv_list_add_btn(ui->screen_wificfg_list_1, LV_SYMBOL_WIFI, "wifi3_dummy");
    ui->screen_wificfg_list_1_item3 = lv_list_add_btn(ui->screen_wificfg_list_1, LV_SYMBOL_WIFI, "wifi4_dummy");
    ui->screen_wificfg_list_1_item4 = lv_list_add_btn(ui->screen_wificfg_list_1, LV_SYMBOL_WIFI, "wifi5_dummy");
    lv_obj_set_pos(ui->screen_wificfg_list_1, 172, 64);
    lv_obj_set_size(ui->screen_wificfg_list_1, 131, 60);
    lv_obj_set_scrollbar_mode(ui->screen_wificfg_list_1, LV_SCROLLBAR_MODE_OFF);

    //Write style state: LV_STATE_DEFAULT for &style_screen_wificfg_list_1_main_main_default
    static lv_style_t style_screen_wificfg_list_1_main_main_default;
    ui_init_style(&style_screen_wificfg_list_1_main_main_default);

    lv_style_set_pad_top(&style_screen_wificfg_list_1_main_main_default, 0);
    lv_style_set_pad_left(&style_screen_wificfg_list_1_main_main_default, 0);
    lv_style_set_pad_right(&style_screen_wificfg_list_1_main_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_wificfg_list_1_main_main_default, 5);
    lv_style_set_bg_opa(&style_screen_wificfg_list_1_main_main_default, 0);
    lv_style_set_border_width(&style_screen_wificfg_list_1_main_main_default, 0);
    lv_style_set_radius(&style_screen_wificfg_list_1_main_main_default, 3);
    lv_style_set_shadow_width(&style_screen_wificfg_list_1_main_main_default, 0);
    lv_obj_add_style(ui->screen_wificfg_list_1, &style_screen_wificfg_list_1_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_FOCUSED for &style_screen_wificfg_list_1_main_main_focused
    static lv_style_t style_screen_wificfg_list_1_main_main_focused;
    ui_init_style(&style_screen_wificfg_list_1_main_main_focused);

    lv_style_set_pad_top(&style_screen_wificfg_list_1_main_main_focused, 5);
    lv_style_set_pad_left(&style_screen_wificfg_list_1_main_main_focused, 5);
    lv_style_set_pad_right(&style_screen_wificfg_list_1_main_main_focused, 5);
    lv_style_set_pad_bottom(&style_screen_wificfg_list_1_main_main_focused, 5);
    lv_style_set_bg_opa(&style_screen_wificfg_list_1_main_main_focused, 0);
    lv_style_set_border_width(&style_screen_wificfg_list_1_main_main_focused, 0);
    lv_style_set_radius(&style_screen_wificfg_list_1_main_main_focused, 3);
    lv_style_set_shadow_width(&style_screen_wificfg_list_1_main_main_focused, 0);
    lv_obj_add_style(ui->screen_wificfg_list_1, &style_screen_wificfg_list_1_main_main_focused, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write style state: LV_STATE_DEFAULT for &style_screen_wificfg_list_1_main_scrollbar_default
    static lv_style_t style_screen_wificfg_list_1_main_scrollbar_default;
    ui_init_style(&style_screen_wificfg_list_1_main_scrollbar_default);

    lv_style_set_radius(&style_screen_wificfg_list_1_main_scrollbar_default, 3);
    lv_style_set_bg_opa(&style_screen_wificfg_list_1_main_scrollbar_default, 255);
    lv_style_set_bg_color(&style_screen_wificfg_list_1_main_scrollbar_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_wificfg_list_1_main_scrollbar_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_wificfg_list_1, &style_screen_wificfg_list_1_main_scrollbar_default, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_wificfg_list_1_extra_btns_main_default
    static lv_style_t style_screen_wificfg_list_1_extra_btns_main_default;
    ui_init_style(&style_screen_wificfg_list_1_extra_btns_main_default);

    lv_style_set_pad_top(&style_screen_wificfg_list_1_extra_btns_main_default, 0);
    lv_style_set_pad_left(&style_screen_wificfg_list_1_extra_btns_main_default, 0);
    lv_style_set_pad_right(&style_screen_wificfg_list_1_extra_btns_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_wificfg_list_1_extra_btns_main_default, 4);
    lv_style_set_border_width(&style_screen_wificfg_list_1_extra_btns_main_default, 0);
    lv_style_set_text_color(&style_screen_wificfg_list_1_extra_btns_main_default, lv_color_hex(0x274a6e));
    lv_style_set_text_font(&style_screen_wificfg_list_1_extra_btns_main_default, &lv_font_SourceHanSansSC_Medium_11);
    lv_style_set_text_opa(&style_screen_wificfg_list_1_extra_btns_main_default, 255);
    lv_style_set_radius(&style_screen_wificfg_list_1_extra_btns_main_default, 3);
    lv_style_set_bg_opa(&style_screen_wificfg_list_1_extra_btns_main_default, 17);
    lv_style_set_bg_color(&style_screen_wificfg_list_1_extra_btns_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_wificfg_list_1_extra_btns_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_wificfg_list_1_item4, &style_screen_wificfg_list_1_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_wificfg_list_1_item3, &style_screen_wificfg_list_1_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_wificfg_list_1_item2, &style_screen_wificfg_list_1_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_wificfg_list_1_item1, &style_screen_wificfg_list_1_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_wificfg_list_1_item0, &style_screen_wificfg_list_1_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_PRESSED for &style_screen_wificfg_list_1_extra_btns_main_pressed
    static lv_style_t style_screen_wificfg_list_1_extra_btns_main_pressed;
    ui_init_style(&style_screen_wificfg_list_1_extra_btns_main_pressed);

    lv_style_set_pad_top(&style_screen_wificfg_list_1_extra_btns_main_pressed, 0);
    lv_style_set_pad_left(&style_screen_wificfg_list_1_extra_btns_main_pressed, 0);
    lv_style_set_pad_right(&style_screen_wificfg_list_1_extra_btns_main_pressed, 5);
    lv_style_set_pad_bottom(&style_screen_wificfg_list_1_extra_btns_main_pressed, 4);
    lv_style_set_border_width(&style_screen_wificfg_list_1_extra_btns_main_pressed, 0);
    lv_style_set_radius(&style_screen_wificfg_list_1_extra_btns_main_pressed, 3);
    lv_style_set_text_color(&style_screen_wificfg_list_1_extra_btns_main_pressed, lv_color_hex(0x274a6e));
    lv_style_set_text_font(&style_screen_wificfg_list_1_extra_btns_main_pressed, &lv_font_SourceHanSansSC_Medium_13);
    lv_style_set_text_opa(&style_screen_wificfg_list_1_extra_btns_main_pressed, 255);
    lv_style_set_bg_opa(&style_screen_wificfg_list_1_extra_btns_main_pressed, 0);
    lv_obj_add_style(ui->screen_wificfg_list_1_item4, &style_screen_wificfg_list_1_extra_btns_main_pressed, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_add_style(ui->screen_wificfg_list_1_item3, &style_screen_wificfg_list_1_extra_btns_main_pressed, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_add_style(ui->screen_wificfg_list_1_item2, &style_screen_wificfg_list_1_extra_btns_main_pressed, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_add_style(ui->screen_wificfg_list_1_item1, &style_screen_wificfg_list_1_extra_btns_main_pressed, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_add_style(ui->screen_wificfg_list_1_item0, &style_screen_wificfg_list_1_extra_btns_main_pressed, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style state: LV_STATE_FOCUSED for &style_screen_wificfg_list_1_extra_btns_main_focused
    static lv_style_t style_screen_wificfg_list_1_extra_btns_main_focused;
    ui_init_style(&style_screen_wificfg_list_1_extra_btns_main_focused);

    lv_style_set_pad_top(&style_screen_wificfg_list_1_extra_btns_main_focused, 0);
    lv_style_set_pad_left(&style_screen_wificfg_list_1_extra_btns_main_focused, 1);
    lv_style_set_pad_right(&style_screen_wificfg_list_1_extra_btns_main_focused, 5);
    lv_style_set_pad_bottom(&style_screen_wificfg_list_1_extra_btns_main_focused, 4);
    lv_style_set_border_width(&style_screen_wificfg_list_1_extra_btns_main_focused, 0);
    lv_style_set_radius(&style_screen_wificfg_list_1_extra_btns_main_focused, 3);
    lv_style_set_text_color(&style_screen_wificfg_list_1_extra_btns_main_focused, lv_color_hex(0x274a6e));
    lv_style_set_text_font(&style_screen_wificfg_list_1_extra_btns_main_focused, &lv_font_SourceHanSansSC_Medium_12);
    lv_style_set_text_opa(&style_screen_wificfg_list_1_extra_btns_main_focused, 255);
    lv_style_set_bg_opa(&style_screen_wificfg_list_1_extra_btns_main_focused, 19);
    lv_style_set_bg_color(&style_screen_wificfg_list_1_extra_btns_main_focused, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_wificfg_list_1_extra_btns_main_focused, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_wificfg_list_1_item4, &style_screen_wificfg_list_1_extra_btns_main_focused, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_add_style(ui->screen_wificfg_list_1_item3, &style_screen_wificfg_list_1_extra_btns_main_focused, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_add_style(ui->screen_wificfg_list_1_item2, &style_screen_wificfg_list_1_extra_btns_main_focused, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_add_style(ui->screen_wificfg_list_1_item1, &style_screen_wificfg_list_1_extra_btns_main_focused, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_add_style(ui->screen_wificfg_list_1_item0, &style_screen_wificfg_list_1_extra_btns_main_focused, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write style state: LV_STATE_DEFAULT for &style_screen_wificfg_list_1_extra_texts_main_default
    static lv_style_t style_screen_wificfg_list_1_extra_texts_main_default;
    ui_init_style(&style_screen_wificfg_list_1_extra_texts_main_default);

    lv_style_set_pad_top(&style_screen_wificfg_list_1_extra_texts_main_default, 5);
    lv_style_set_pad_left(&style_screen_wificfg_list_1_extra_texts_main_default, 5);
    lv_style_set_pad_right(&style_screen_wificfg_list_1_extra_texts_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_wificfg_list_1_extra_texts_main_default, 5);
    lv_style_set_border_width(&style_screen_wificfg_list_1_extra_texts_main_default, 0);
    lv_style_set_text_color(&style_screen_wificfg_list_1_extra_texts_main_default, lv_color_hex(0x274a6e));
    lv_style_set_text_font(&style_screen_wificfg_list_1_extra_texts_main_default, &lv_font_SourceHanSansSC_Medium_12);
    lv_style_set_text_opa(&style_screen_wificfg_list_1_extra_texts_main_default, 255);
    lv_style_set_radius(&style_screen_wificfg_list_1_extra_texts_main_default, 3);
    lv_style_set_transform_width(&style_screen_wificfg_list_1_extra_texts_main_default, 0);
    lv_style_set_bg_opa(&style_screen_wificfg_list_1_extra_texts_main_default, 255);
    lv_style_set_bg_color(&style_screen_wificfg_list_1_extra_texts_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_wificfg_list_1_extra_texts_main_default, LV_GRAD_DIR_NONE);

    //Write codes screen_wificfg_btn_scan
    ui->screen_wificfg_btn_scan = lv_btn_create(ui->screen_wificfg);
    ui->screen_wificfg_btn_scan_label = lv_label_create(ui->screen_wificfg_btn_scan);
    lv_label_set_text(ui->screen_wificfg_btn_scan_label, "");
    lv_label_set_long_mode(ui->screen_wificfg_btn_scan_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_wificfg_btn_scan_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_wificfg_btn_scan, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_wificfg_btn_scan_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_wificfg_btn_scan, 177, 135);
    lv_obj_set_size(ui->screen_wificfg_btn_scan, 41, 11);

    //Write style for screen_wificfg_btn_scan, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_wificfg_btn_scan, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_wificfg_btn_scan, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wificfg_btn_scan, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wificfg_btn_scan, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wificfg_btn_scan, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wificfg_btn_scan, &lv_font_Alatsi_Regular_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wificfg_btn_scan, 198, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wificfg_btn_scan, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_wificfg_btn_scan, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_wificfg_btn_scan, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_wificfg_btn_scan, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_wificfg_btn_scan, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_wificfg_btn_scan, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_wificfg_btn_scan, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_wificfg_btn_scan, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_wificfg_btn_scan, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_wificfg_btn_scan, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_wificfg_btn_scan, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_wificfg_btn_scan, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_wificfg_btn_scan, &lv_font_Alatsi_Regular_12, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_wificfg_btn_scan, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_wificfg_btn_disconnect
    ui->screen_wificfg_btn_disconnect = lv_btn_create(ui->screen_wificfg);
    ui->screen_wificfg_btn_disconnect_label = lv_label_create(ui->screen_wificfg_btn_disconnect);
    lv_label_set_text(ui->screen_wificfg_btn_disconnect_label, "");
    lv_label_set_long_mode(ui->screen_wificfg_btn_disconnect_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_wificfg_btn_disconnect_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_wificfg_btn_disconnect, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_wificfg_btn_disconnect_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_wificfg_btn_disconnect, 226, 135);
    lv_obj_set_size(ui->screen_wificfg_btn_disconnect, 76, 11);

    //Write style for screen_wificfg_btn_disconnect, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_wificfg_btn_disconnect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_wificfg_btn_disconnect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wificfg_btn_disconnect, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wificfg_btn_disconnect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wificfg_btn_disconnect, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wificfg_btn_disconnect, &lv_font_Alatsi_Regular_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wificfg_btn_disconnect, 193, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wificfg_btn_disconnect, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_wificfg_btn_disconnect, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_wificfg_btn_disconnect, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_wificfg_btn_disconnect, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_wificfg_btn_disconnect, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_wificfg_btn_disconnect, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_wificfg_btn_disconnect, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_wificfg_btn_disconnect, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_wificfg_btn_disconnect, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_wificfg_btn_disconnect, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_wificfg_btn_disconnect, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_wificfg_btn_disconnect, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_wificfg_btn_disconnect, &lv_font_Alatsi_Regular_12, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_wificfg_btn_disconnect, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_wificfg_btn_cancel
    ui->screen_wificfg_btn_cancel = lv_btn_create(ui->screen_wificfg);
    ui->screen_wificfg_btn_cancel_label = lv_label_create(ui->screen_wificfg_btn_cancel);
    lv_label_set_text(ui->screen_wificfg_btn_cancel_label, "Cancel");
    lv_label_set_long_mode(ui->screen_wificfg_btn_cancel_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_wificfg_btn_cancel_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_wificfg_btn_cancel, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_wificfg_btn_cancel_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_wificfg_btn_cancel, 23, 91);
    lv_obj_set_size(ui->screen_wificfg_btn_cancel, 45, 14);
    lv_obj_add_flag(ui->screen_wificfg_btn_cancel, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_wificfg_btn_cancel, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_wificfg_btn_cancel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_wificfg_btn_cancel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wificfg_btn_cancel, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wificfg_btn_cancel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wificfg_btn_cancel, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wificfg_btn_cancel, &lv_font_SourceHanSansSC_Medium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wificfg_btn_cancel, 198, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wificfg_btn_cancel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_wificfg_btn_cancel, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_wificfg_btn_cancel, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_wificfg_btn_cancel, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_wificfg_btn_cancel, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_wificfg_btn_cancel, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_wificfg_btn_cancel, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_wificfg_btn_cancel, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_wificfg_btn_cancel, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_wificfg_btn_cancel, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_wificfg_btn_cancel, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_wificfg_btn_cancel, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_wificfg_btn_cancel, &lv_font_SourceHanSansSC_Medium_12, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_wificfg_btn_cancel, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_wificfg_btn_connect
    ui->screen_wificfg_btn_connect = lv_btn_create(ui->screen_wificfg);
    ui->screen_wificfg_btn_connect_label = lv_label_create(ui->screen_wificfg_btn_connect);
    lv_label_set_text(ui->screen_wificfg_btn_connect_label, "Connect");
    lv_label_set_long_mode(ui->screen_wificfg_btn_connect_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_wificfg_btn_connect_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_wificfg_btn_connect, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_wificfg_btn_connect_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_wificfg_btn_connect, 95, 91);
    lv_obj_set_size(ui->screen_wificfg_btn_connect, 49, 14);
    lv_obj_add_flag(ui->screen_wificfg_btn_connect, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_wificfg_btn_connect, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_wificfg_btn_connect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_wificfg_btn_connect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wificfg_btn_connect, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wificfg_btn_connect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wificfg_btn_connect, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wificfg_btn_connect, &lv_font_SourceHanSansSC_Medium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wificfg_btn_connect, 198, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wificfg_btn_connect, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_wificfg_btn_connect, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_wificfg_btn_connect, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_wificfg_btn_connect, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_wificfg_btn_connect, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_wificfg_btn_connect, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_wificfg_btn_connect, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_wificfg_btn_connect, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_wificfg_btn_connect, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_wificfg_btn_connect, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_wificfg_btn_connect, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_wificfg_btn_connect, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_wificfg_btn_connect, &lv_font_SourceHanSansSC_Medium_12, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_wificfg_btn_connect, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_wificfg_ta_password
    ui->screen_wificfg_ta_password = lv_textarea_create(ui->screen_wificfg);
    lv_textarea_set_text(ui->screen_wificfg_ta_password, "");
    lv_textarea_set_placeholder_text(ui->screen_wificfg_ta_password, "");
    lv_textarea_set_password_bullet(ui->screen_wificfg_ta_password, "*");
    lv_textarea_set_password_mode(ui->screen_wificfg_ta_password, false);
    lv_textarea_set_one_line(ui->screen_wificfg_ta_password, true);
    lv_textarea_set_accepted_chars(ui->screen_wificfg_ta_password, "");
    lv_textarea_set_max_length(ui->screen_wificfg_ta_password, 32);

    lv_obj_set_pos(ui->screen_wificfg_ta_password, 15, 60);
    lv_obj_set_size(ui->screen_wificfg_ta_password, 135, 26);
    lv_obj_add_flag(ui->screen_wificfg_ta_password, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_wificfg_ta_password, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(ui->screen_wificfg_ta_password, LV_OBJ_FLAG_CLICK_FOCUSABLE);

    //Write style for screen_wificfg_ta_password, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_wificfg_ta_password, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wificfg_ta_password, &lv_font_SourceHanSansSC_Medium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wificfg_ta_password, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_wificfg_ta_password, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wificfg_ta_password, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_wificfg_ta_password, 70, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_wificfg_ta_password, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_wificfg_ta_password, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_wificfg_ta_password, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wificfg_ta_password, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_wificfg_ta_password, 7, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_wificfg_ta_password, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_wificfg_ta_password, 26, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wificfg_ta_password, 7, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_wificfg_ta_password, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_text_color(ui->screen_wificfg_ta_password, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_wificfg_ta_password, &lv_font_SourceHanSansSC_Medium_12, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_wificfg_ta_password, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_opa(ui->screen_wificfg_ta_password, 120, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(ui->screen_wificfg_ta_password, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_grad_dir(ui->screen_wificfg_ta_password, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_wificfg_ta_password, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_wificfg_ta_password, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_wificfg_ta_password, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_wificfg_ta_password, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_wificfg_ta_password, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_wificfg_ta_password, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_wificfg_ta_password, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_pad_top(ui->screen_wificfg_ta_password, 7, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_pad_right(ui->screen_wificfg_ta_password, 4, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_pad_left(ui->screen_wificfg_ta_password, 22, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_wificfg_ta_password, 4, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write style for screen_wificfg_ta_password, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_wificfg_ta_password, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wificfg_ta_password, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write codes screen_wificfg_img_passicon
    ui->screen_wificfg_img_passicon = lv_img_create(ui->screen_wificfg);
    lv_obj_add_flag(ui->screen_wificfg_img_passicon, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_wificfg_img_passicon, &_AAA_wifi_suo_icon_alpha_13x15);
    lv_img_set_pivot(ui->screen_wificfg_img_passicon, 50,50);
    lv_img_set_angle(ui->screen_wificfg_img_passicon, 0);
    lv_obj_set_pos(ui->screen_wificfg_img_passicon, 18, 65);
    lv_obj_set_size(ui->screen_wificfg_img_passicon, 13, 15);
    lv_obj_add_flag(ui->screen_wificfg_img_passicon, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_wificfg_img_passicon, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_wificfg_img_passicon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_wificfg_img_passicon, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wificfg_img_passicon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_wificfg_img_passicon, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wificfg_spinner_connect
    ui->screen_wificfg_spinner_connect = lv_spinner_create(ui->screen_wificfg, 1000, 60);
    lv_obj_set_pos(ui->screen_wificfg_spinner_connect, 63, 114);
    lv_obj_set_size(ui->screen_wificfg_spinner_connect, 40, 40);
    lv_obj_add_flag(ui->screen_wificfg_spinner_connect, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_wificfg_spinner_connect, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_pad_top(ui->screen_wificfg_spinner_connect, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_wificfg_spinner_connect, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_wificfg_spinner_connect, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_wificfg_spinner_connect, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_wificfg_spinner_connect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui->screen_wificfg_spinner_connect, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui->screen_wificfg_spinner_connect, 208, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui->screen_wificfg_spinner_connect, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wificfg_spinner_connect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_wificfg_spinner_connect, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_arc_width(ui->screen_wificfg_spinner_connect, 4, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui->screen_wificfg_spinner_connect, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui->screen_wificfg_spinner_connect, lv_color_hex(0x274a6e), LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //The custom code of screen_wificfg.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_wificfg);

    //Init events for screen.
    events_init_screen_wificfg(ui);
}
