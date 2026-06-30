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
    lv_obj_set_style_bg_img_src(ui->screen_reviewcard, &_AAA_flashcards_review_bg_320x170, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_reviewcard, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_reviewcard, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_reviewcard_label_index
    ui->screen_reviewcard_label_index = lv_label_create(ui->screen_reviewcard);
    lv_label_set_text(ui->screen_reviewcard_label_index, "1/100");
    lv_label_set_long_mode(ui->screen_reviewcard_label_index, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_reviewcard_label_index, 219, 7);
    lv_obj_set_size(ui->screen_reviewcard_label_index, 71, 14);

    //Write style for screen_reviewcard_label_index, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_reviewcard_label_index, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_reviewcard_label_index, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_reviewcard_label_index, lv_color_hex(0x274a61), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_reviewcard_label_index, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
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

    //Write codes screen_reviewcard_label_spelling
    ui->screen_reviewcard_label_spelling = lv_label_create(ui->screen_reviewcard);
    lv_label_set_text(ui->screen_reviewcard_label_spelling, "Abondon");
    lv_label_set_long_mode(ui->screen_reviewcard_label_spelling, LV_LABEL_LONG_SCROLL);
    lv_obj_set_pos(ui->screen_reviewcard_label_spelling, 32, 26);
    lv_obj_set_size(ui->screen_reviewcard_label_spelling, 257, 43);

    //Write style for screen_reviewcard_label_spelling, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_reviewcard_label_spelling, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_reviewcard_label_spelling, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_reviewcard_label_spelling, lv_color_hex(0x274a61), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_reviewcard_label_spelling, &lv_font_SourceHanSansSC_Medium_28, LV_PART_MAIN|LV_STATE_DEFAULT);
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
    lv_label_set_text(ui->screen_reviewcard_label_phonetic, "ɑ ɒ ɔ ə ɛ ɜ ɪ ʊ ʌ æ e ɪ a ɪ ɔ ɪ a ʊ ə ʊ ɪ ə e ə ʊ ə p t k f θ s ʃ h tʃ tr ts b d v ð z ʒ r dʒ dr dz m n ŋ l j w ˈ ˌ ː . [ ] / ( ) ɡ ，。ɚ ɝ̃");
    lv_label_set_long_mode(ui->screen_reviewcard_label_phonetic, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_reviewcard_label_phonetic, 32, 66);
    lv_obj_set_size(ui->screen_reviewcard_label_phonetic, 282, 15);

    //Write style for screen_reviewcard_label_phonetic, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_reviewcard_label_phonetic, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_reviewcard_label_phonetic, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_reviewcard_label_phonetic, lv_color_hex(0x274a61), LV_PART_MAIN|LV_STATE_DEFAULT);
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
    lv_label_set_text(ui->screen_reviewcard_label_meaning, "v.放弃;抛弃;放纵");
    lv_label_set_long_mode(ui->screen_reviewcard_label_meaning, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_reviewcard_label_meaning, 31, 92);
    lv_obj_set_size(ui->screen_reviewcard_label_meaning, 257, 15);

    //Write style for screen_reviewcard_label_meaning, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_reviewcard_label_meaning, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_reviewcard_label_meaning, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_reviewcard_label_meaning, lv_color_hex(0x274a61), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_reviewcard_label_meaning, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
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

    //Write codes screen_reviewcard_btn_dui
    ui->screen_reviewcard_btn_dui = lv_btn_create(ui->screen_reviewcard);
    ui->screen_reviewcard_btn_dui_label = lv_label_create(ui->screen_reviewcard_btn_dui);
    lv_label_set_text(ui->screen_reviewcard_btn_dui_label, "");
    lv_label_set_long_mode(ui->screen_reviewcard_btn_dui_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_reviewcard_btn_dui_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_reviewcard_btn_dui, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_reviewcard_btn_dui_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_reviewcard_btn_dui, 130, 123);
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
    lv_obj_set_style_shadow_width(ui->screen_reviewcard_btn_dui, 10, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_reviewcard_btn_dui, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_reviewcard_btn_dui, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_reviewcard_btn_dui, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_reviewcard_btn_dui, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_reviewcard_btn_dui, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_reviewcard_btn_dui, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_reviewcard_btn_dui, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_reviewcard_btn_dui, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write style for screen_reviewcard_btn_dui, Part: LV_PART_MAIN, State: LV_STATE_DISABLED.
    lv_obj_set_style_bg_opa(ui->screen_reviewcard_btn_dui, 131, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_bg_color(ui->screen_reviewcard_btn_dui, lv_color_hex(0x555555), LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_bg_grad_dir(ui->screen_reviewcard_btn_dui, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_border_width(ui->screen_reviewcard_btn_dui, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_radius(ui->screen_reviewcard_btn_dui, 20, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_shadow_width(ui->screen_reviewcard_btn_dui, 0, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_text_color(ui->screen_reviewcard_btn_dui, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_text_font(ui->screen_reviewcard_btn_dui, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DISABLED);
    lv_obj_set_style_text_opa(ui->screen_reviewcard_btn_dui, 255, LV_PART_MAIN|LV_STATE_DISABLED);

    //Write codes screen_reviewcard_btn_back
    ui->screen_reviewcard_btn_back = lv_btn_create(ui->screen_reviewcard);
    ui->screen_reviewcard_btn_back_label = lv_label_create(ui->screen_reviewcard_btn_back);
    lv_label_set_text(ui->screen_reviewcard_btn_back_label, "");
    lv_label_set_long_mode(ui->screen_reviewcard_btn_back_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_reviewcard_btn_back_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_reviewcard_btn_back, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_reviewcard_btn_back_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_reviewcard_btn_back, 8, 10);
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
    lv_obj_set_style_shadow_color(ui->screen_reviewcard_btn_back, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_reviewcard_btn_back, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_reviewcard_btn_back, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_reviewcard_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_reviewcard_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_reviewcard_btn_back, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_reviewcard_btn_back, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_reviewcard_btn_back, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_reviewcard_dog_dance
    ui->screen_reviewcard_dog_dance = lv_animimg_create(ui->screen_reviewcard);
    lv_animimg_set_src(ui->screen_reviewcard_dog_dance, (const void **) screen_reviewcard_dog_dance_imgs, 3, false);
    lv_animimg_set_duration(ui->screen_reviewcard_dog_dance, 100*3);
    lv_animimg_set_repeat_count(ui->screen_reviewcard_dog_dance, LV_ANIM_REPEAT_INFINITE);
    lv_animimg_start(ui->screen_reviewcard_dog_dance);
    lv_obj_set_pos(ui->screen_reviewcard_dog_dance, 245, 30);
    lv_obj_set_size(ui->screen_reviewcard_dog_dance, 34, 34);

    //The custom code of screen_reviewcard.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_reviewcard);

    //Init events for screen.
    events_init_screen_reviewcard(ui);
}
