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



void setup_scr_screen_getanswer(lv_ui *ui)
{
    //Write codes screen_getanswer
    ui->screen_getanswer = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_getanswer, 320, 170);
    lv_obj_set_scrollbar_mode(ui->screen_getanswer, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_getanswer, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_getanswer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_getanswer, &_AAA__answer_end_bg_320x170, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_getanswer, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_getanswer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_getanswer_btn_answer
    ui->screen_getanswer_btn_answer = lv_btn_create(ui->screen_getanswer);
    ui->screen_getanswer_btn_answer_label = lv_label_create(ui->screen_getanswer_btn_answer);
    lv_label_set_text(ui->screen_getanswer_btn_answer_label, "\"最好的事情正要发生\"");
    lv_label_set_long_mode(ui->screen_getanswer_btn_answer_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_getanswer_btn_answer_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_getanswer_btn_answer, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_getanswer_btn_answer_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_getanswer_btn_answer, 26, 69);
    lv_obj_set_size(ui->screen_getanswer_btn_answer, 271, 32);

    //Write style for screen_getanswer_btn_answer, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_getanswer_btn_answer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_getanswer_btn_answer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_getanswer_btn_answer, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_getanswer_btn_answer, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_getanswer_btn_answer, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_getanswer_btn_answer, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_getanswer_btn_answer, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_getanswer_btn_answer, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_getanswer_btn_answer, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_getanswer_btn_answer, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_getanswer_btn_answer, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_getanswer_btn_answer, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_getanswer_btn_answer, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_getanswer_btn_answer, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_getanswer_btn_answer, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_getanswer_btn_answer, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write style for screen_getanswer_btn_answer, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_bg_opa(ui->screen_getanswer_btn_answer, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_border_width(ui->screen_getanswer_btn_answer, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_radius(ui->screen_getanswer_btn_answer, 5, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->screen_getanswer_btn_answer, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->screen_getanswer_btn_answer, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->screen_getanswer_btn_answer, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->screen_getanswer_btn_answer, 255, LV_PART_MAIN|LV_STATE_PRESSED);

    //The custom code of screen_getanswer.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_getanswer);

    //Init events for screen.
    events_init_screen_getanswer(ui);
}
