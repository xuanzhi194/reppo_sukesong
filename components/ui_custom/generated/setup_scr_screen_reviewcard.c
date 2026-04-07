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



void setup_scr_screen_reviewcard(lv_ui *ui)
{
    //Write codes screen_reviewcard
    ui->screen_reviewcard = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_reviewcard, 320, 170);
    lv_obj_set_scrollbar_mode(ui->screen_reviewcard, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_reviewcard, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_reviewcard, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_reviewcard, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_reviewcard, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_reviewcard, &_word_BG2_320x170, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_reviewcard, 137, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_reviewcard, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_reviewcard_img_wordrect
    ui->screen_reviewcard_img_wordrect = lv_img_create(ui->screen_reviewcard);
    lv_obj_add_flag(ui->screen_reviewcard_img_wordrect, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_reviewcard_img_wordrect, &_word_rectangle_alpha_264x136);
    lv_img_set_pivot(ui->screen_reviewcard_img_wordrect, 50,50);
    lv_img_set_angle(ui->screen_reviewcard_img_wordrect, 0);
    lv_obj_set_pos(ui->screen_reviewcard_img_wordrect, 28, 25);
    lv_obj_set_size(ui->screen_reviewcard_img_wordrect, 264, 136);

    //Write style for screen_reviewcard_img_wordrect, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_reviewcard_img_wordrect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_reviewcard_img_wordrect, 152, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_reviewcard_img_wordrect, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_reviewcard_img_wordrect, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_reviewcard_btn_back
    ui->screen_reviewcard_btn_back = lv_btn_create(ui->screen_reviewcard);
    ui->screen_reviewcard_btn_back_label = lv_label_create(ui->screen_reviewcard_btn_back);
    lv_label_set_text(ui->screen_reviewcard_btn_back_label, "<");
    lv_label_set_long_mode(ui->screen_reviewcard_btn_back_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_reviewcard_btn_back_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_reviewcard_btn_back, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_reviewcard_btn_back_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_reviewcard_btn_back, 5, 5);
    lv_obj_set_size(ui->screen_reviewcard_btn_back, 15, 15);

    //Write style for screen_reviewcard_btn_back, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_reviewcard_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_reviewcard_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_reviewcard_btn_back, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_reviewcard_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_reviewcard_btn_back, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_reviewcard_btn_back, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_reviewcard_btn_back, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_reviewcard_btn_back, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_reviewcard_btn_back, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_reviewcard_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_reviewcard_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_reviewcard_btn_back, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_reviewcard_btn_back, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_reviewcard_btn_back, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_reviewcard_btn_back, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_reviewcard_btn_back, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_reviewcard_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_reviewcard_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_reviewcard_btn_back, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_reviewcard_btn_back, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_reviewcard_btn_back, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_reviewcard_btn_left
    ui->screen_reviewcard_btn_left = lv_btn_create(ui->screen_reviewcard);
    ui->screen_reviewcard_btn_left_label = lv_label_create(ui->screen_reviewcard_btn_left);
    lv_label_set_text(ui->screen_reviewcard_btn_left_label, "");
    lv_label_set_long_mode(ui->screen_reviewcard_btn_left_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_reviewcard_btn_left_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_reviewcard_btn_left, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_reviewcard_btn_left_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_reviewcard_btn_left, 5, 79);
    lv_obj_set_size(ui->screen_reviewcard_btn_left, 20, 20);

    //Write style for screen_reviewcard_btn_left, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_reviewcard_btn_left, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_reviewcard_btn_left, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_reviewcard_btn_left, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_reviewcard_btn_left, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_reviewcard_btn_left, &_leftarrow_20x20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_reviewcard_btn_left, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_reviewcard_btn_left, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_reviewcard_btn_left, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_reviewcard_btn_left, &lv_font_Acme_Regular_22, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_reviewcard_btn_left, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_reviewcard_btn_left, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_reviewcard_btn_left, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_reviewcard_btn_left, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_reviewcard_btn_left, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_reviewcard_btn_left, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_reviewcard_btn_left, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_reviewcard_btn_left, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_reviewcard_btn_left, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_reviewcard_btn_left, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_reviewcard_btn_left, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_reviewcard_btn_left, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_src(ui->screen_reviewcard_btn_left, &_leftarrow_20x20, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_opa(ui->screen_reviewcard_btn_left, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_reviewcard_btn_left, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_reviewcard_btn_left, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_reviewcard_btn_left, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_reviewcard_btn_left, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_reviewcard_label_index
    ui->screen_reviewcard_label_index = lv_label_create(ui->screen_reviewcard);
    lv_label_set_text(ui->screen_reviewcard_label_index, "0/200");
    lv_label_set_long_mode(ui->screen_reviewcard_label_index, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_reviewcard_label_index, 219, 10);
    lv_obj_set_size(ui->screen_reviewcard_label_index, 68, 11);

    //Write style for screen_reviewcard_label_index, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_reviewcard_label_index, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_reviewcard_label_index, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_reviewcard_label_index, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_reviewcard_label_index, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_reviewcard_label_index, 207, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_reviewcard_label_index, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_reviewcard_label_index, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_reviewcard_label_index, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_reviewcard_label_index, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_reviewcard_label_index, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_reviewcard_label_index, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_reviewcard_label_index, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_reviewcard_label_index, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_reviewcard_label_index, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_reviewcard_btn_right
    ui->screen_reviewcard_btn_right = lv_btn_create(ui->screen_reviewcard);
    ui->screen_reviewcard_btn_right_label = lv_label_create(ui->screen_reviewcard_btn_right);
    lv_label_set_text(ui->screen_reviewcard_btn_right_label, "");
    lv_label_set_long_mode(ui->screen_reviewcard_btn_right_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_reviewcard_btn_right_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_reviewcard_btn_right, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_reviewcard_btn_right_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_reviewcard_btn_right, 296, 78);
    lv_obj_set_size(ui->screen_reviewcard_btn_right, 18, 18);

    //Write style for screen_reviewcard_btn_right, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_reviewcard_btn_right, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_reviewcard_btn_right, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_reviewcard_btn_right, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_reviewcard_btn_right, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_reviewcard_btn_right, &_rightarrow_18x18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_reviewcard_btn_right, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_reviewcard_btn_right, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_reviewcard_btn_right, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_reviewcard_btn_right, &lv_font_Acme_Regular_22, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_reviewcard_btn_right, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_reviewcard_btn_right, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_reviewcard_btn_right, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_reviewcard_btn_right, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_reviewcard_btn_right, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_reviewcard_btn_right, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_reviewcard_btn_right, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_reviewcard_btn_right, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_reviewcard_btn_right, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_reviewcard_btn_right, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_reviewcard_btn_right, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_reviewcard_btn_right, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_src(ui->screen_reviewcard_btn_right, &_rightarrow_18x18, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_opa(ui->screen_reviewcard_btn_right, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_reviewcard_btn_right, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_reviewcard_btn_right, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_reviewcard_btn_right, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_reviewcard_btn_right, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_reviewcard_label_spelling
    ui->screen_reviewcard_label_spelling = lv_label_create(ui->screen_reviewcard);
    lv_label_set_text(ui->screen_reviewcard_label_spelling, "Acquire");
    lv_label_set_long_mode(ui->screen_reviewcard_label_spelling, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_pos(ui->screen_reviewcard_label_spelling, 32, 28);
    lv_obj_set_size(ui->screen_reviewcard_label_spelling, 256, 41);

    //Write style for screen_reviewcard_label_spelling, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_reviewcard_label_spelling, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_reviewcard_label_spelling, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_reviewcard_label_spelling, lv_color_hex(0xefcbd1), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_reviewcard_label_spelling, &lv_font_Acme_Regular_30, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_reviewcard_label_spelling, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_reviewcard_label_spelling, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_reviewcard_label_spelling, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_reviewcard_label_spelling, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_reviewcard_label_spelling, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_reviewcard_label_spelling, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_reviewcard_label_spelling, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_reviewcard_label_spelling, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_reviewcard_label_spelling, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_reviewcard_label_spelling, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_reviewcard_label_phonetic
    ui->screen_reviewcard_label_phonetic = lv_label_create(ui->screen_reviewcard);
    lv_label_set_text(ui->screen_reviewcard_label_phonetic, "[ə'kwfaɪr]");
    lv_label_set_long_mode(ui->screen_reviewcard_label_phonetic, LV_LABEL_LONG_CLIP);
    lv_obj_set_pos(ui->screen_reviewcard_label_phonetic, 32, 69);
    lv_obj_set_size(ui->screen_reviewcard_label_phonetic, 255, 16);

    //Write style for screen_reviewcard_label_phonetic, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_reviewcard_label_phonetic, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_reviewcard_label_phonetic, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_reviewcard_label_phonetic, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_reviewcard_label_phonetic, &lv_font_arial_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_reviewcard_label_phonetic, 143, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_reviewcard_label_phonetic, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_reviewcard_label_phonetic, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_reviewcard_label_phonetic, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_reviewcard_label_phonetic, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_reviewcard_label_phonetic, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_reviewcard_label_phonetic, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_reviewcard_label_phonetic, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_reviewcard_label_phonetic, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_reviewcard_label_phonetic, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_reviewcard_label_meaning
    ui->screen_reviewcard_label_meaning = lv_label_create(ui->screen_reviewcard);
    lv_label_set_text(ui->screen_reviewcard_label_meaning, "v.获得;学到");
    lv_label_set_long_mode(ui->screen_reviewcard_label_meaning, LV_LABEL_LONG_CLIP);
    lv_obj_set_pos(ui->screen_reviewcard_label_meaning, 32, 91);
    lv_obj_set_size(ui->screen_reviewcard_label_meaning, 255, 18);

    //Write style for screen_reviewcard_label_meaning, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_reviewcard_label_meaning, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_reviewcard_label_meaning, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_reviewcard_label_meaning, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_reviewcard_label_meaning, &lv_font_AdobeFangsongStdRegular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_reviewcard_label_meaning, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_reviewcard_label_meaning, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_reviewcard_label_meaning, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_reviewcard_label_meaning, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_reviewcard_label_meaning, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_reviewcard_label_meaning, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_reviewcard_label_meaning, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_reviewcard_label_meaning, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_reviewcard_label_meaning, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_reviewcard_label_meaning, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_reviewcard_img_dui
    ui->screen_reviewcard_img_dui = lv_img_create(ui->screen_reviewcard);
    lv_obj_add_flag(ui->screen_reviewcard_img_dui, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_reviewcard_img_dui, &_dui_1_alpha_20x20);
    lv_img_set_pivot(ui->screen_reviewcard_img_dui, 50,50);
    lv_img_set_angle(ui->screen_reviewcard_img_dui, 0);
    lv_obj_set_pos(ui->screen_reviewcard_img_dui, 103, 119);
    lv_obj_set_size(ui->screen_reviewcard_img_dui, 20, 20);

    //Write style for screen_reviewcard_img_dui, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_reviewcard_img_dui, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_reviewcard_img_dui, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_reviewcard_img_dui, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_reviewcard_img_dui, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_reviewcard_label_gotit
    ui->screen_reviewcard_label_gotit = lv_label_create(ui->screen_reviewcard);
    lv_label_set_text(ui->screen_reviewcard_label_gotit, "Got it");
    lv_label_set_long_mode(ui->screen_reviewcard_label_gotit, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_reviewcard_label_gotit, 92, 145);
    lv_obj_set_size(ui->screen_reviewcard_label_gotit, 40, 12);

    //Write style for screen_reviewcard_label_gotit, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_reviewcard_label_gotit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_reviewcard_label_gotit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_reviewcard_label_gotit, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_reviewcard_label_gotit, &lv_font_montserratMedium_10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_reviewcard_label_gotit, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_reviewcard_label_gotit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_reviewcard_label_gotit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_reviewcard_label_gotit, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_reviewcard_label_gotit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_reviewcard_label_gotit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_reviewcard_label_gotit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_reviewcard_label_gotit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_reviewcard_label_gotit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_reviewcard_label_gotit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_reviewcard_btn_dui
    ui->screen_reviewcard_btn_dui = lv_btn_create(ui->screen_reviewcard);
    ui->screen_reviewcard_btn_dui_label = lv_label_create(ui->screen_reviewcard_btn_dui);
    lv_label_set_text(ui->screen_reviewcard_btn_dui_label, "");
    lv_label_set_long_mode(ui->screen_reviewcard_btn_dui_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_reviewcard_btn_dui_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_reviewcard_btn_dui, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_reviewcard_btn_dui_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_reviewcard_btn_dui, 103, 119);
    lv_obj_set_size(ui->screen_reviewcard_btn_dui, 20, 20);

    //Write style for screen_reviewcard_btn_dui, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_reviewcard_btn_dui, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_reviewcard_btn_dui, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_reviewcard_btn_dui, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_reviewcard_btn_dui, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_reviewcard_btn_dui, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_reviewcard_btn_dui, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_reviewcard_btn_dui, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_reviewcard_btn_dui, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_reviewcard_btn_dui, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_reviewcard_btn_dui, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_reviewcard_btn_dui, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_reviewcard_btn_dui, 20, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_reviewcard_btn_dui, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_reviewcard_btn_dui, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_reviewcard_btn_dui, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_reviewcard_btn_dui, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_reviewcard_btn_dui, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_reviewcard_btn_dui, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_reviewcard_btn_dui, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_reviewcard_btn_dui, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_reviewcard_btn_dui, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write style for screen_reviewcard_btn_dui, Part: LV_PART_MAIN, State: LV_STATE_DISABLED.
    lv_obj_set_style_bg_opa(ui->screen_reviewcard_btn_dui, 169, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_bg_color(ui->screen_reviewcard_btn_dui, lv_color_hex(0x7b7b7b), LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_bg_grad_dir(ui->screen_reviewcard_btn_dui, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_border_width(ui->screen_reviewcard_btn_dui, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_radius(ui->screen_reviewcard_btn_dui, 20, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_shadow_width(ui->screen_reviewcard_btn_dui, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_text_color(ui->screen_reviewcard_btn_dui, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_text_font(ui->screen_reviewcard_btn_dui, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_text_opa(ui->screen_reviewcard_btn_dui, 255, LV_PART_MAIN|LV_STATE_DISABLED);

    //Write codes screen_reviewcard_btn_home
    ui->screen_reviewcard_btn_home = lv_btn_create(ui->screen_reviewcard);
    ui->screen_reviewcard_btn_home_label = lv_label_create(ui->screen_reviewcard_btn_home);
    lv_label_set_text(ui->screen_reviewcard_btn_home_label, "");
    lv_label_set_long_mode(ui->screen_reviewcard_btn_home_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_reviewcard_btn_home_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_reviewcard_btn_home, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_reviewcard_btn_home_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_reviewcard_btn_home, 300, 5);
    lv_obj_set_size(ui->screen_reviewcard_btn_home, 15, 15);

    //Write style for screen_reviewcard_btn_home, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_reviewcard_btn_home, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_reviewcard_btn_home, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_reviewcard_btn_home, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_reviewcard_btn_home, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_reviewcard_btn_home, &_home_15x15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_reviewcard_btn_home, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_reviewcard_btn_home, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_reviewcard_btn_home, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_reviewcard_btn_home, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_reviewcard_btn_home, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_reviewcard_btn_home, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_reviewcard_btn_home, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_reviewcard_btn_home, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_reviewcard_btn_home, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_reviewcard_btn_home, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_reviewcard_btn_home, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_reviewcard_btn_home, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_reviewcard_btn_home, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_reviewcard_btn_home, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_reviewcard_btn_home, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_reviewcard_btn_home, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_src(ui->screen_reviewcard_btn_home, &_home_15x15, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_opa(ui->screen_reviewcard_btn_home, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_reviewcard_btn_home, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_reviewcard_btn_home, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_reviewcard_btn_home, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_reviewcard_btn_home, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_reviewcard_label_review
    ui->screen_reviewcard_label_review = lv_label_create(ui->screen_reviewcard);
    lv_label_set_text(ui->screen_reviewcard_label_review, "Reivew");
    lv_label_set_long_mode(ui->screen_reviewcard_label_review, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_reviewcard_label_review, 171, 145);
    lv_obj_set_size(ui->screen_reviewcard_label_review, 53, 11);

    //Write style for screen_reviewcard_label_review, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_reviewcard_label_review, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_reviewcard_label_review, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_reviewcard_label_review, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_reviewcard_label_review, &lv_font_montserratMedium_10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_reviewcard_label_review, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_reviewcard_label_review, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_reviewcard_label_review, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_reviewcard_label_review, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_reviewcard_label_review, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_reviewcard_label_review, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_reviewcard_label_review, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_reviewcard_label_review, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_reviewcard_label_review, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_reviewcard_label_review, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_reviewcard_img_cuo
    ui->screen_reviewcard_img_cuo = lv_img_create(ui->screen_reviewcard);
    lv_obj_add_flag(ui->screen_reviewcard_img_cuo, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_reviewcard_img_cuo, &_cuo_1_alpha_20x20);
    lv_img_set_pivot(ui->screen_reviewcard_img_cuo, 50,50);
    lv_img_set_angle(ui->screen_reviewcard_img_cuo, 0);
    lv_obj_set_pos(ui->screen_reviewcard_img_cuo, 188, 119);
    lv_obj_set_size(ui->screen_reviewcard_img_cuo, 20, 20);

    //Write style for screen_reviewcard_img_cuo, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_reviewcard_img_cuo, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_reviewcard_img_cuo, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_reviewcard_img_cuo, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_reviewcard_img_cuo, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_reviewcard_btn_cuo
    ui->screen_reviewcard_btn_cuo = lv_btn_create(ui->screen_reviewcard);
    ui->screen_reviewcard_btn_cuo_label = lv_label_create(ui->screen_reviewcard_btn_cuo);
    lv_label_set_text(ui->screen_reviewcard_btn_cuo_label, "");
    lv_label_set_long_mode(ui->screen_reviewcard_btn_cuo_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_reviewcard_btn_cuo_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_reviewcard_btn_cuo, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_reviewcard_btn_cuo_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_reviewcard_btn_cuo, 188, 119);
    lv_obj_set_size(ui->screen_reviewcard_btn_cuo, 20, 20);

    //Write style for screen_reviewcard_btn_cuo, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_reviewcard_btn_cuo, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_reviewcard_btn_cuo, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_reviewcard_btn_cuo, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_reviewcard_btn_cuo, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_reviewcard_btn_cuo, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_reviewcard_btn_cuo, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_reviewcard_btn_cuo, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_reviewcard_btn_cuo, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_reviewcard_btn_cuo, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_reviewcard_btn_cuo, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_reviewcard_btn_cuo, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_reviewcard_btn_cuo, 20, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_reviewcard_btn_cuo, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_reviewcard_btn_cuo, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_reviewcard_btn_cuo, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_reviewcard_btn_cuo, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_reviewcard_btn_cuo, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_reviewcard_btn_cuo, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_reviewcard_btn_cuo, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_reviewcard_btn_cuo, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_reviewcard_btn_cuo, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write style for screen_reviewcard_btn_cuo, Part: LV_PART_MAIN, State: LV_STATE_DISABLED.
    lv_obj_set_style_bg_opa(ui->screen_reviewcard_btn_cuo, 172, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_bg_color(ui->screen_reviewcard_btn_cuo, lv_color_hex(0x555555), LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_bg_grad_dir(ui->screen_reviewcard_btn_cuo, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_border_width(ui->screen_reviewcard_btn_cuo, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_radius(ui->screen_reviewcard_btn_cuo, 20, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_shadow_width(ui->screen_reviewcard_btn_cuo, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_text_color(ui->screen_reviewcard_btn_cuo, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_text_font(ui->screen_reviewcard_btn_cuo, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_text_opa(ui->screen_reviewcard_btn_cuo, 255, LV_PART_MAIN|LV_STATE_DISABLED);

    //The custom code of screen_reviewcard.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_reviewcard);

    //Init events for screen.
    events_init_screen_reviewcard(ui);
}
