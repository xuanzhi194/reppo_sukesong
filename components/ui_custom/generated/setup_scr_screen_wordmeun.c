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



void setup_scr_screen_wordmeun(lv_ui *ui)
{
    //Write codes screen_wordmeun
    ui->screen_wordmeun = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_wordmeun, 320, 170);
    lv_obj_set_scrollbar_mode(ui->screen_wordmeun, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_wordmeun, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_wordmeun, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_wordmeun, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_wordmeun, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_wordmeun, &_word_BG_320x170, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_wordmeun, 128, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_wordmeun, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wordmeun_img_learn
    ui->screen_wordmeun_img_learn = lv_img_create(ui->screen_wordmeun);
    lv_obj_add_flag(ui->screen_wordmeun_img_learn, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_wordmeun_img_learn, &_word_rectangle_alpha_129x21);
    lv_img_set_pivot(ui->screen_wordmeun_img_learn, 50,50);
    lv_img_set_angle(ui->screen_wordmeun_img_learn, 0);
    lv_obj_set_pos(ui->screen_wordmeun_img_learn, 29, 135);
    lv_obj_set_size(ui->screen_wordmeun_img_learn, 129, 21);

    //Write style for screen_wordmeun_img_learn, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_wordmeun_img_learn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_wordmeun_img_learn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wordmeun_img_learn, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_wordmeun_img_learn, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wordmeun_img_wordrect
    ui->screen_wordmeun_img_wordrect = lv_img_create(ui->screen_wordmeun);
    lv_obj_add_flag(ui->screen_wordmeun_img_wordrect, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_wordmeun_img_wordrect, &_word_rectangle_alpha_264x94);
    lv_img_set_pivot(ui->screen_wordmeun_img_wordrect, 50,50);
    lv_img_set_angle(ui->screen_wordmeun_img_wordrect, 0);
    lv_obj_set_pos(ui->screen_wordmeun_img_wordrect, 28, 33);
    lv_obj_set_size(ui->screen_wordmeun_img_wordrect, 264, 94);

    //Write style for screen_wordmeun_img_wordrect, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_wordmeun_img_wordrect, 134, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(ui->screen_wordmeun_img_wordrect, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_wordmeun_img_wordrect, 160, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wordmeun_img_wordrect, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_wordmeun_img_wordrect, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wordmeun_btn_back
    ui->screen_wordmeun_btn_back = lv_btn_create(ui->screen_wordmeun);
    ui->screen_wordmeun_btn_back_label = lv_label_create(ui->screen_wordmeun_btn_back);
    lv_label_set_text(ui->screen_wordmeun_btn_back_label, "<");
    lv_label_set_long_mode(ui->screen_wordmeun_btn_back_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_wordmeun_btn_back_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_wordmeun_btn_back, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_wordmeun_btn_back_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_wordmeun_btn_back, 4, 5);
    lv_obj_set_size(ui->screen_wordmeun_btn_back, 15, 15);

    //Write style for screen_wordmeun_btn_back, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_wordmeun_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_wordmeun_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wordmeun_btn_back, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wordmeun_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wordmeun_btn_back, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wordmeun_btn_back, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wordmeun_btn_back, 165, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wordmeun_btn_back, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_wordmeun_btn_back, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_wordmeun_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_wordmeun_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_wordmeun_btn_back, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_wordmeun_btn_back, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_wordmeun_btn_back, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_wordmeun_btn_back, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_wordmeun_btn_back, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_wordmeun_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_wordmeun_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_wordmeun_btn_back, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_wordmeun_btn_back, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_wordmeun_btn_back, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_wordmeun_bar_process
    ui->screen_wordmeun_bar_process = lv_bar_create(ui->screen_wordmeun);
    lv_obj_set_style_anim_time(ui->screen_wordmeun_bar_process, 1000, 0);
    lv_bar_set_mode(ui->screen_wordmeun_bar_process, LV_BAR_MODE_NORMAL);
    lv_bar_set_range(ui->screen_wordmeun_bar_process, 0, 100);
    lv_bar_set_value(ui->screen_wordmeun_bar_process, 100, LV_ANIM_OFF);
    lv_obj_set_pos(ui->screen_wordmeun_bar_process, 84, 91);
    lv_obj_set_size(ui->screen_wordmeun_bar_process, 203, 5);

    //Write style for screen_wordmeun_bar_process, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_wordmeun_bar_process, 134, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_wordmeun_bar_process, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_wordmeun_bar_process, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wordmeun_bar_process, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wordmeun_bar_process, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_wordmeun_bar_process, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_wordmeun_bar_process, 90, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_wordmeun_bar_process, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_wordmeun_bar_process, LV_GRAD_DIR_HOR, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui->screen_wordmeun_bar_process, lv_color_hex(0x00ff40), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_main_stop(ui->screen_wordmeun_bar_process, 0, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_stop(ui->screen_wordmeun_bar_process, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wordmeun_bar_process, 10, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write codes screen_wordmeun_label_booksel
    ui->screen_wordmeun_label_booksel = lv_label_create(ui->screen_wordmeun);
    lv_label_set_text(ui->screen_wordmeun_label_booksel, "Select Vocab Book");
    lv_label_set_long_mode(ui->screen_wordmeun_label_booksel, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_wordmeun_label_booksel, 58, 8);
    lv_obj_set_size(ui->screen_wordmeun_label_booksel, 204, 19);

    //Write style for screen_wordmeun_label_booksel, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_wordmeun_label_booksel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wordmeun_label_booksel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wordmeun_label_booksel, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wordmeun_label_booksel, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wordmeun_label_booksel, 163, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_wordmeun_label_booksel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_wordmeun_label_booksel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wordmeun_label_booksel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_wordmeun_label_booksel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_wordmeun_label_booksel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_wordmeun_label_booksel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_wordmeun_label_booksel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_wordmeun_label_booksel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wordmeun_label_booksel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wordmeun_img_book
    ui->screen_wordmeun_img_book = lv_img_create(ui->screen_wordmeun);
    lv_obj_add_flag(ui->screen_wordmeun_img_book, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_wordmeun_img_book, &_book_alpha_39x46);
    lv_img_set_pivot(ui->screen_wordmeun_img_book, 50,50);
    lv_img_set_angle(ui->screen_wordmeun_img_book, 0);
    lv_obj_set_pos(ui->screen_wordmeun_img_book, 34, 58);
    lv_obj_set_size(ui->screen_wordmeun_img_book, 39, 46);

    //Write style for screen_wordmeun_img_book, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_wordmeun_img_book, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_wordmeun_img_book, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wordmeun_img_book, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_wordmeun_img_book, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wordmeun_label_bookname
    ui->screen_wordmeun_label_bookname = lv_label_create(ui->screen_wordmeun);
    lv_label_set_text(ui->screen_wordmeun_label_bookname, "CET4");
    lv_label_set_long_mode(ui->screen_wordmeun_label_bookname, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_wordmeun_label_bookname, 84, 43);
    lv_obj_set_size(ui->screen_wordmeun_label_bookname, 202, 18);

    //Write style for screen_wordmeun_label_bookname, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_wordmeun_label_bookname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wordmeun_label_bookname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wordmeun_label_bookname, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wordmeun_label_bookname, &lv_font_Alatsi_Regular_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wordmeun_label_bookname, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_wordmeun_label_bookname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_wordmeun_label_bookname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wordmeun_label_bookname, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_wordmeun_label_bookname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_wordmeun_label_bookname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_wordmeun_label_bookname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_wordmeun_label_bookname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_wordmeun_label_bookname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wordmeun_label_bookname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wordmeun_label_divdernum
    ui->screen_wordmeun_label_divdernum = lv_label_create(ui->screen_wordmeun);
    lv_label_set_text(ui->screen_wordmeun_label_divdernum, "500/2000 words (25%)");
    lv_label_set_long_mode(ui->screen_wordmeun_label_divdernum, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_wordmeun_label_divdernum, 84, 71);
    lv_obj_set_size(ui->screen_wordmeun_label_divdernum, 202, 13);

    //Write style for screen_wordmeun_label_divdernum, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_wordmeun_label_divdernum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wordmeun_label_divdernum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wordmeun_label_divdernum, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wordmeun_label_divdernum, &lv_font_Alatsi_Regular_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wordmeun_label_divdernum, 180, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_wordmeun_label_divdernum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_wordmeun_label_divdernum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wordmeun_label_divdernum, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_wordmeun_label_divdernum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_wordmeun_label_divdernum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_wordmeun_label_divdernum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_wordmeun_label_divdernum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_wordmeun_label_divdernum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wordmeun_label_divdernum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wordmeun_label_wordindicate
    ui->screen_wordmeun_label_wordindicate = lv_label_create(ui->screen_wordmeun);
    lv_label_set_text(ui->screen_wordmeun_label_wordindicate, "Learn: 500 Review: 100");
    lv_label_set_long_mode(ui->screen_wordmeun_label_wordindicate, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_wordmeun_label_wordindicate, 85, 101);
    lv_obj_set_size(ui->screen_wordmeun_label_wordindicate, 202, 16);

    //Write style for screen_wordmeun_label_wordindicate, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_wordmeun_label_wordindicate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wordmeun_label_wordindicate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wordmeun_label_wordindicate, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wordmeun_label_wordindicate, &lv_font_Alatsi_Regular_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wordmeun_label_wordindicate, 186, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_wordmeun_label_wordindicate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_wordmeun_label_wordindicate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wordmeun_label_wordindicate, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_wordmeun_label_wordindicate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_wordmeun_label_wordindicate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_wordmeun_label_wordindicate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_wordmeun_label_wordindicate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_wordmeun_label_wordindicate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wordmeun_label_wordindicate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wordmeun_btn_bookleft
    ui->screen_wordmeun_btn_bookleft = lv_btn_create(ui->screen_wordmeun);
    ui->screen_wordmeun_btn_bookleft_label = lv_label_create(ui->screen_wordmeun_btn_bookleft);
    lv_label_set_text(ui->screen_wordmeun_btn_bookleft_label, "<<");
    lv_label_set_long_mode(ui->screen_wordmeun_btn_bookleft_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_wordmeun_btn_bookleft_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_wordmeun_btn_bookleft, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_wordmeun_btn_bookleft_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_wordmeun_btn_bookleft, 4, 75);
    lv_obj_set_size(ui->screen_wordmeun_btn_bookleft, 20, 20);

    //Write style for screen_wordmeun_btn_bookleft, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_wordmeun_btn_bookleft, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_wordmeun_btn_bookleft, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wordmeun_btn_bookleft, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wordmeun_btn_bookleft, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wordmeun_btn_bookleft, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wordmeun_btn_bookleft, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wordmeun_btn_bookleft, 170, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wordmeun_btn_bookleft, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_wordmeun_btn_bookleft, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_wordmeun_btn_bookleft, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_wordmeun_btn_bookleft, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_wordmeun_btn_bookleft, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_wordmeun_btn_bookleft, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_wordmeun_btn_bookleft, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_wordmeun_btn_bookleft, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_wordmeun_btn_bookleft, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_wordmeun_btn_bookleft, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_wordmeun_btn_bookleft, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_wordmeun_btn_bookleft, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_wordmeun_btn_bookleft, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_wordmeun_btn_bookleft, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_wordmeun_btn_bookright
    ui->screen_wordmeun_btn_bookright = lv_btn_create(ui->screen_wordmeun);
    ui->screen_wordmeun_btn_bookright_label = lv_label_create(ui->screen_wordmeun_btn_bookright);
    lv_label_set_text(ui->screen_wordmeun_btn_bookright_label, ">>");
    lv_label_set_long_mode(ui->screen_wordmeun_btn_bookright_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_wordmeun_btn_bookright_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_wordmeun_btn_bookright, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_wordmeun_btn_bookright_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_wordmeun_btn_bookright, 295, 75);
    lv_obj_set_size(ui->screen_wordmeun_btn_bookright, 20, 20);

    //Write style for screen_wordmeun_btn_bookright, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_wordmeun_btn_bookright, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_wordmeun_btn_bookright, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wordmeun_btn_bookright, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wordmeun_btn_bookright, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wordmeun_btn_bookright, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wordmeun_btn_bookright, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wordmeun_btn_bookright, 171, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wordmeun_btn_bookright, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_wordmeun_btn_bookright, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_wordmeun_btn_bookright, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_wordmeun_btn_bookright, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_wordmeun_btn_bookright, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_wordmeun_btn_bookright, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_wordmeun_btn_bookright, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_wordmeun_btn_bookright, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_wordmeun_btn_bookright, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_wordmeun_btn_bookright, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_wordmeun_btn_bookright, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_wordmeun_btn_bookright, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_wordmeun_btn_bookright, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_wordmeun_btn_bookright, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_wordmeun_label_learn
    ui->screen_wordmeun_label_learn = lv_label_create(ui->screen_wordmeun);
    lv_label_set_text(ui->screen_wordmeun_label_learn, "Learn");
    lv_label_set_long_mode(ui->screen_wordmeun_label_learn, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_wordmeun_label_learn, 62, 135);
    lv_obj_set_size(ui->screen_wordmeun_label_learn, 62, 15);

    //Write style for screen_wordmeun_label_learn, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_wordmeun_label_learn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wordmeun_label_learn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wordmeun_label_learn, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wordmeun_label_learn, &lv_font_Alatsi_Regular_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wordmeun_label_learn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_wordmeun_label_learn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_wordmeun_label_learn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wordmeun_label_learn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_wordmeun_label_learn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_wordmeun_label_learn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_wordmeun_label_learn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_wordmeun_label_learn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_wordmeun_label_learn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wordmeun_label_learn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wordmeun_btn_learn
    ui->screen_wordmeun_btn_learn = lv_btn_create(ui->screen_wordmeun);
    ui->screen_wordmeun_btn_learn_label = lv_label_create(ui->screen_wordmeun_btn_learn);
    lv_label_set_text(ui->screen_wordmeun_btn_learn_label, "");
    lv_label_set_long_mode(ui->screen_wordmeun_btn_learn_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_wordmeun_btn_learn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_wordmeun_btn_learn, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_wordmeun_btn_learn_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_wordmeun_btn_learn, 29, 136);
    lv_obj_set_size(ui->screen_wordmeun_btn_learn, 127, 19);

    //Write style for screen_wordmeun_btn_learn, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_wordmeun_btn_learn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_wordmeun_btn_learn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wordmeun_btn_learn, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wordmeun_btn_learn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wordmeun_btn_learn, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wordmeun_btn_learn, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wordmeun_btn_learn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wordmeun_btn_learn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_wordmeun_btn_learn, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_wordmeun_btn_learn, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_wordmeun_btn_learn, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_wordmeun_btn_learn, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_wordmeun_btn_learn, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_wordmeun_btn_learn, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_wordmeun_btn_learn, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_wordmeun_btn_learn, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_wordmeun_btn_learn, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_wordmeun_btn_learn, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_wordmeun_btn_learn, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_wordmeun_btn_learn, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_wordmeun_btn_learn, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_wordmeun_img_review
    ui->screen_wordmeun_img_review = lv_img_create(ui->screen_wordmeun);
    lv_obj_add_flag(ui->screen_wordmeun_img_review, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_wordmeun_img_review, &_word_rectangle_alpha_129x21);
    lv_img_set_pivot(ui->screen_wordmeun_img_review, 50,50);
    lv_img_set_angle(ui->screen_wordmeun_img_review, 0);
    lv_obj_set_pos(ui->screen_wordmeun_img_review, 164, 135);
    lv_obj_set_size(ui->screen_wordmeun_img_review, 129, 21);

    //Write style for screen_wordmeun_img_review, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_wordmeun_img_review, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_wordmeun_img_review, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wordmeun_img_review, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_wordmeun_img_review, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wordmeun_label_Review
    ui->screen_wordmeun_label_Review = lv_label_create(ui->screen_wordmeun);
    lv_label_set_text(ui->screen_wordmeun_label_Review, "Review");
    lv_label_set_long_mode(ui->screen_wordmeun_label_Review, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_wordmeun_label_Review, 194, 136);
    lv_obj_set_size(ui->screen_wordmeun_label_Review, 69, 16);

    //Write style for screen_wordmeun_label_Review, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_wordmeun_label_Review, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wordmeun_label_Review, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wordmeun_label_Review, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wordmeun_label_Review, &lv_font_Alatsi_Regular_17, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wordmeun_label_Review, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_wordmeun_label_Review, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_wordmeun_label_Review, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wordmeun_label_Review, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_wordmeun_label_Review, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_wordmeun_label_Review, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_wordmeun_label_Review, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_wordmeun_label_Review, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_wordmeun_label_Review, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wordmeun_label_Review, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wordmeun_btn_review
    ui->screen_wordmeun_btn_review = lv_btn_create(ui->screen_wordmeun);
    ui->screen_wordmeun_btn_review_label = lv_label_create(ui->screen_wordmeun_btn_review);
    lv_label_set_text(ui->screen_wordmeun_btn_review_label, "");
    lv_label_set_long_mode(ui->screen_wordmeun_btn_review_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_wordmeun_btn_review_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_wordmeun_btn_review, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_wordmeun_btn_review_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_wordmeun_btn_review, 164, 136);
    lv_obj_set_size(ui->screen_wordmeun_btn_review, 127, 19);

    //Write style for screen_wordmeun_btn_review, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_wordmeun_btn_review, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_wordmeun_btn_review, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wordmeun_btn_review, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wordmeun_btn_review, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wordmeun_btn_review, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wordmeun_btn_review, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wordmeun_btn_review, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wordmeun_btn_review, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_wordmeun_btn_review, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_wordmeun_btn_review, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_wordmeun_btn_review, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_wordmeun_btn_review, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_wordmeun_btn_review, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_wordmeun_btn_review, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_wordmeun_btn_review, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_wordmeun_btn_review, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_wordmeun_btn_review, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_wordmeun_btn_review, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_wordmeun_btn_review, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_wordmeun_btn_review, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_wordmeun_btn_review, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //The custom code of screen_wordmeun.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_wordmeun);

    //Init events for screen.
    events_init_screen_wordmeun(ui);
}
