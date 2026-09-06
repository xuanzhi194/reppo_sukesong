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



void setup_scr_screen_answer(lv_ui *ui)
{
    //Write codes screen_answer
    ui->screen_answer = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_answer, 320, 170);
    lv_obj_set_scrollbar_mode(ui->screen_answer, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_answer, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_answer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_answer, &_AAA_answer_1_0905_320x170, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_answer, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_answer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_answer_btn_start
    ui->screen_answer_btn_start = lv_btn_create(ui->screen_answer);
    ui->screen_answer_btn_start_label = lv_label_create(ui->screen_answer_btn_start);
    lv_label_set_text(ui->screen_answer_btn_start_label, "");
    lv_label_set_long_mode(ui->screen_answer_btn_start_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_answer_btn_start_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_answer_btn_start, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_answer_btn_start_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_answer_btn_start, 8, 155);
    lv_obj_set_size(ui->screen_answer_btn_start, 1, 9);

    //Write style for screen_answer_btn_start, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_answer_btn_start, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_answer_btn_start, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_answer_btn_start, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_answer_btn_start, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_answer_btn_start, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_answer_btn_start, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_answer_btn_start, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_answer_btn_start, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_answer_btn_start, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_answer_btn_start, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_answer_btn_start, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_answer_btn_start, 10, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_answer_btn_start, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_answer_btn_start, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_answer_btn_start, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_answer_btn_start, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write style for screen_answer_btn_start, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_bg_opa(ui->screen_answer_btn_start, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_border_width(ui->screen_answer_btn_start, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_radius(ui->screen_answer_btn_start, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->screen_answer_btn_start, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->screen_answer_btn_start, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->screen_answer_btn_start, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->screen_answer_btn_start, 255, LV_PART_MAIN|LV_STATE_PRESSED);

    //The custom code of screen_answer.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_answer);

    //Init events for screen.
    events_init_screen_answer(ui);
}
