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
    lv_obj_set_style_bg_img_src(ui->screen_wordmeun, &_AAA_cishu_320x170, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_wordmeun, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_wordmeun, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wordmeun_bar_process
    ui->screen_wordmeun_bar_process = lv_bar_create(ui->screen_wordmeun);
    lv_obj_set_style_anim_time(ui->screen_wordmeun_bar_process, 1000, 0);
    lv_bar_set_mode(ui->screen_wordmeun_bar_process, LV_BAR_MODE_NORMAL);
    lv_bar_set_range(ui->screen_wordmeun_bar_process, 0, 100);
    lv_bar_set_value(ui->screen_wordmeun_bar_process, 100, LV_ANIM_OFF);
    lv_obj_set_pos(ui->screen_wordmeun_bar_process, 84, 90);
    lv_obj_set_size(ui->screen_wordmeun_bar_process, 203, 5);

    //Write style for screen_wordmeun_bar_process, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_wordmeun_bar_process, 134, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_wordmeun_bar_process, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_wordmeun_bar_process, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wordmeun_bar_process, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wordmeun_bar_process, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_wordmeun_bar_process, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_wordmeun_bar_process, 152, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_wordmeun_bar_process, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_wordmeun_bar_process, LV_GRAD_DIR_HOR, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui->screen_wordmeun_bar_process, lv_color_hex(0x0a3763), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_main_stop(ui->screen_wordmeun_bar_process, 0, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_stop(ui->screen_wordmeun_bar_process, 0, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wordmeun_bar_process, 10, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write codes screen_wordmeun_label_wordpercent
    ui->screen_wordmeun_label_wordpercent = lv_label_create(ui->screen_wordmeun);
    lv_label_set_text(ui->screen_wordmeun_label_wordpercent, "25%");
    lv_label_set_long_mode(ui->screen_wordmeun_label_wordpercent, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_wordmeun_label_wordpercent, 252, 104);
    lv_obj_set_size(ui->screen_wordmeun_label_wordpercent, 36, 14);

    //Write style for screen_wordmeun_label_wordpercent, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_wordmeun_label_wordpercent, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wordmeun_label_wordpercent, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wordmeun_label_wordpercent, lv_color_hex(0x274a61), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wordmeun_label_wordpercent, &lv_font_SourceHanSansSC_Medium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wordmeun_label_wordpercent, 180, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_wordmeun_label_wordpercent, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_wordmeun_label_wordpercent, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wordmeun_label_wordpercent, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_wordmeun_label_wordpercent, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_wordmeun_label_wordpercent, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_wordmeun_label_wordpercent, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_wordmeun_label_wordpercent, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_wordmeun_label_wordpercent, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wordmeun_label_wordpercent, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wordmeun_label_reviewnum
    ui->screen_wordmeun_label_reviewnum = lv_label_create(ui->screen_wordmeun);
    lv_label_set_text(ui->screen_wordmeun_label_reviewnum, "Review: 100");
    lv_label_set_long_mode(ui->screen_wordmeun_label_reviewnum, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_wordmeun_label_reviewnum, 84, 104);
    lv_obj_set_size(ui->screen_wordmeun_label_reviewnum, 91, 15);

    //Write style for screen_wordmeun_label_reviewnum, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_wordmeun_label_reviewnum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wordmeun_label_reviewnum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wordmeun_label_reviewnum, lv_color_hex(0x274a61), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wordmeun_label_reviewnum, &lv_font_SourceHanSansSC_Medium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wordmeun_label_reviewnum, 186, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_wordmeun_label_reviewnum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_wordmeun_label_reviewnum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wordmeun_label_reviewnum, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_wordmeun_label_reviewnum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_wordmeun_label_reviewnum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_wordmeun_label_reviewnum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_wordmeun_label_reviewnum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_wordmeun_label_reviewnum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wordmeun_label_reviewnum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wordmeun_btn_learn
    ui->screen_wordmeun_btn_learn = lv_btn_create(ui->screen_wordmeun);
    ui->screen_wordmeun_btn_learn_label = lv_label_create(ui->screen_wordmeun_btn_learn);
    lv_label_set_text(ui->screen_wordmeun_btn_learn_label, "");
    lv_label_set_long_mode(ui->screen_wordmeun_btn_learn_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_wordmeun_btn_learn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_wordmeun_btn_learn, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_wordmeun_btn_learn_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_wordmeun_btn_learn, 29, 136);
    lv_obj_set_size(ui->screen_wordmeun_btn_learn, 127, 17);

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
    lv_obj_set_style_shadow_color(ui->screen_wordmeun_btn_learn, lv_color_hex(0x49607b), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_wordmeun_btn_learn, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_wordmeun_btn_learn, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_wordmeun_btn_learn, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_wordmeun_btn_learn, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_wordmeun_btn_learn, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_wordmeun_btn_learn, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_wordmeun_btn_learn, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_wordmeun_btn_review
    ui->screen_wordmeun_btn_review = lv_btn_create(ui->screen_wordmeun);
    ui->screen_wordmeun_btn_review_label = lv_label_create(ui->screen_wordmeun_btn_review);
    lv_label_set_text(ui->screen_wordmeun_btn_review_label, "");
    lv_label_set_long_mode(ui->screen_wordmeun_btn_review_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_wordmeun_btn_review_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_wordmeun_btn_review, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_wordmeun_btn_review_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_wordmeun_btn_review, 164, 136);
    lv_obj_set_size(ui->screen_wordmeun_btn_review, 126, 16);

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
    lv_obj_set_style_shadow_color(ui->screen_wordmeun_btn_review, lv_color_hex(0x49607b), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_wordmeun_btn_review, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_wordmeun_btn_review, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_wordmeun_btn_review, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_wordmeun_btn_review, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_wordmeun_btn_review, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_wordmeun_btn_review, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_wordmeun_btn_review, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_wordmeun_btn_back
    ui->screen_wordmeun_btn_back = lv_btn_create(ui->screen_wordmeun);
    ui->screen_wordmeun_btn_back_label = lv_label_create(ui->screen_wordmeun_btn_back);
    lv_label_set_text(ui->screen_wordmeun_btn_back_label, "");
    lv_label_set_long_mode(ui->screen_wordmeun_btn_back_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_wordmeun_btn_back_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_wordmeun_btn_back, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_wordmeun_btn_back_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_wordmeun_btn_back, 8, 10);
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
    lv_obj_set_style_shadow_color(ui->screen_wordmeun_btn_back, lv_color_hex(0x49607b), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_wordmeun_btn_back, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_wordmeun_btn_back, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_wordmeun_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_wordmeun_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_wordmeun_btn_back, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_wordmeun_btn_back, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_wordmeun_btn_back, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //The custom code of screen_wordmeun.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_wordmeun);

    //Init events for screen.
    events_init_screen_wordmeun(ui);
}
