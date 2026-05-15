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



void setup_scr_screen_donelist(lv_ui *ui)
{
    //Write codes screen_donelist
    ui->screen_donelist = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_donelist, 320, 170);
    lv_obj_set_scrollbar_mode(ui->screen_donelist, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_donelist, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_donelist, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_donelist, &_AAA_done_bg_320x170, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_donelist, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_donelist, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_donelist_line_4
    ui->screen_donelist_line_4 = lv_line_create(ui->screen_donelist);
    static lv_point_t screen_donelist_line_4[] = {{0, 0},{320, 0},};
    lv_line_set_points(ui->screen_donelist_line_4, screen_donelist_line_4, 2);
    lv_obj_set_pos(ui->screen_donelist_line_4, 25, 125);
    lv_obj_set_size(ui->screen_donelist_line_4, 279, 1);

    //Write style for screen_donelist_line_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_line_width(ui->screen_donelist_line_4, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui->screen_donelist_line_4, lv_color_hex(0x7698bc), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui->screen_donelist_line_4, 130, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_rounded(ui->screen_donelist_line_4, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_donelist_line_3
    ui->screen_donelist_line_3 = lv_line_create(ui->screen_donelist);
    static lv_point_t screen_donelist_line_3[] = {{0, 0},{320, 0},};
    lv_line_set_points(ui->screen_donelist_line_3, screen_donelist_line_3, 2);
    lv_obj_set_pos(ui->screen_donelist_line_3, 25, 99);
    lv_obj_set_size(ui->screen_donelist_line_3, 279, 1);

    //Write style for screen_donelist_line_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_line_width(ui->screen_donelist_line_3, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui->screen_donelist_line_3, lv_color_hex(0x7698bc), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui->screen_donelist_line_3, 136, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_rounded(ui->screen_donelist_line_3, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_donelist_line_2
    ui->screen_donelist_line_2 = lv_line_create(ui->screen_donelist);
    static lv_point_t screen_donelist_line_2[] = {{0, 0},{320, 0},};
    lv_line_set_points(ui->screen_donelist_line_2, screen_donelist_line_2, 2);
    lv_obj_set_pos(ui->screen_donelist_line_2, 25, 72);
    lv_obj_set_size(ui->screen_donelist_line_2, 279, 1);

    //Write style for screen_donelist_line_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_line_width(ui->screen_donelist_line_2, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui->screen_donelist_line_2, lv_color_hex(0x7698bc), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui->screen_donelist_line_2, 130, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_rounded(ui->screen_donelist_line_2, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_donelist_line_1
    ui->screen_donelist_line_1 = lv_line_create(ui->screen_donelist);
    static lv_point_t screen_donelist_line_1[] = {{0, 0},{320, 0},};
    lv_line_set_points(ui->screen_donelist_line_1, screen_donelist_line_1, 2);
    lv_obj_set_pos(ui->screen_donelist_line_1, 25, 45);
    lv_obj_set_size(ui->screen_donelist_line_1, 279, 1);

    //Write style for screen_donelist_line_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_line_width(ui->screen_donelist_line_1, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui->screen_donelist_line_1, lv_color_hex(0x7698bc), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui->screen_donelist_line_1, 134, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_rounded(ui->screen_donelist_line_1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_donelist_label_donenum
    ui->screen_donelist_label_donenum = lv_label_create(ui->screen_donelist);
    lv_label_set_text(ui->screen_donelist_label_donenum, "100");
    lv_label_set_long_mode(ui->screen_donelist_label_donenum, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_donelist_label_donenum, 152, 9);
    lv_obj_set_size(ui->screen_donelist_label_donenum, 33, 13);

    //Write style for screen_donelist_label_donenum, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_donelist_label_donenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_donelist_label_donenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_donelist_label_donenum, lv_color_hex(0x0689a1), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_donelist_label_donenum, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_donelist_label_donenum, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_donelist_label_donenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_donelist_label_donenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_donelist_label_donenum, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_donelist_label_donenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_donelist_label_donenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_donelist_label_donenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_donelist_label_donenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_donelist_label_donenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_donelist_label_donenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_donelist_label_nosavenum
    ui->screen_donelist_label_nosavenum = lv_label_create(ui->screen_donelist);
    lv_label_set_text(ui->screen_donelist_label_nosavenum, "100");
    lv_label_set_long_mode(ui->screen_donelist_label_nosavenum, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_donelist_label_nosavenum, 252, 9);
    lv_obj_set_size(ui->screen_donelist_label_nosavenum, 33, 13);

    //Write style for screen_donelist_label_nosavenum, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_donelist_label_nosavenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_donelist_label_nosavenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_donelist_label_nosavenum, lv_color_hex(0x8b287d), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_donelist_label_nosavenum, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_donelist_label_nosavenum, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_donelist_label_nosavenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_donelist_label_nosavenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_donelist_label_nosavenum, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_donelist_label_nosavenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_donelist_label_nosavenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_donelist_label_nosavenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_donelist_label_nosavenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_donelist_label_nosavenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_donelist_label_nosavenum, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_donelist_label_date1
    ui->screen_donelist_label_date1 = lv_label_create(ui->screen_donelist);
    lv_label_set_text(ui->screen_donelist_label_date1, "2026.04.13");
    lv_label_set_long_mode(ui->screen_donelist_label_date1, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_donelist_label_date1, 205, 39);
    lv_obj_set_size(ui->screen_donelist_label_date1, 60, 13);

    //Write style for screen_donelist_label_date1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_donelist_label_date1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_donelist_label_date1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_donelist_label_date1, lv_color_hex(0x2f67a4), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_donelist_label_date1, &lv_font_SourceHanSansSC_Medium_11, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_donelist_label_date1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_donelist_label_date1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_donelist_label_date1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_donelist_label_date1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_donelist_label_date1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_donelist_label_date1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_donelist_label_date1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_donelist_label_date1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_donelist_label_date1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_donelist_label_date1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_donelist_label_done1
    ui->screen_donelist_label_done1 = lv_label_create(ui->screen_donelist);
    lv_label_set_text(ui->screen_donelist_label_done1, "跑步3公里");
    lv_label_set_long_mode(ui->screen_donelist_label_done1, LV_LABEL_LONG_DOT);
    lv_obj_set_pos(ui->screen_donelist_label_done1, 28, 38);
    lv_obj_set_size(ui->screen_donelist_label_done1, 165, 18);

    //Write style for screen_donelist_label_done1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_donelist_label_done1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_donelist_label_done1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_donelist_label_done1, lv_color_hex(0x2f76a4), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_donelist_label_done1, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_donelist_label_done1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_donelist_label_done1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_donelist_label_done1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_donelist_label_done1, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_donelist_label_done1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_donelist_label_done1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_donelist_label_done1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_donelist_label_done1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_donelist_label_done1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_donelist_label_done1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_donelist_label_done2
    ui->screen_donelist_label_done2 = lv_label_create(ui->screen_donelist);
    lv_label_set_text(ui->screen_donelist_label_done2, "考研英语一第二次模拟考试");
    lv_label_set_long_mode(ui->screen_donelist_label_done2, LV_LABEL_LONG_DOT);
    lv_obj_set_pos(ui->screen_donelist_label_done2, 28, 65);
    lv_obj_set_size(ui->screen_donelist_label_done2, 165, 19);

    //Write style for screen_donelist_label_done2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_donelist_label_done2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_donelist_label_done2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_donelist_label_done2, lv_color_hex(0x2f67a4), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_donelist_label_done2, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_donelist_label_done2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_donelist_label_done2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_donelist_label_done2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_donelist_label_done2, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_donelist_label_done2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_donelist_label_done2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_donelist_label_done2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_donelist_label_done2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_donelist_label_done2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_donelist_label_done2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_donelist_label_done3
    ui->screen_donelist_label_done3 = lv_label_create(ui->screen_donelist);
    lv_label_set_text(ui->screen_donelist_label_done3, "阅读专业书籍");
    lv_label_set_long_mode(ui->screen_donelist_label_done3, LV_LABEL_LONG_DOT);
    lv_obj_set_pos(ui->screen_donelist_label_done3, 28, 91);
    lv_obj_set_size(ui->screen_donelist_label_done3, 165, 18);

    //Write style for screen_donelist_label_done3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_donelist_label_done3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_donelist_label_done3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_donelist_label_done3, lv_color_hex(0x2f67a4), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_donelist_label_done3, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_donelist_label_done3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_donelist_label_done3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_donelist_label_done3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_donelist_label_done3, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_donelist_label_done3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_donelist_label_done3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_donelist_label_done3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_donelist_label_done3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_donelist_label_done3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_donelist_label_done3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_donelist_label_done4
    ui->screen_donelist_label_done4 = lv_label_create(ui->screen_donelist);
    lv_label_set_text(ui->screen_donelist_label_done4, "马克思主义论文");
    lv_label_set_long_mode(ui->screen_donelist_label_done4, LV_LABEL_LONG_DOT);
    lv_obj_set_pos(ui->screen_donelist_label_done4, 28, 118);
    lv_obj_set_size(ui->screen_donelist_label_done4, 165, 18);

    //Write style for screen_donelist_label_done4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_donelist_label_done4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_donelist_label_done4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_donelist_label_done4, lv_color_hex(0x2f67a4), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_donelist_label_done4, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_donelist_label_done4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_donelist_label_done4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_donelist_label_done4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_donelist_label_done4, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_donelist_label_done4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_donelist_label_done4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_donelist_label_done4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_donelist_label_done4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_donelist_label_done4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_donelist_label_done4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_donelist_label_date2
    ui->screen_donelist_label_date2 = lv_label_create(ui->screen_donelist);
    lv_label_set_text(ui->screen_donelist_label_date2, "2026.04.13");
    lv_label_set_long_mode(ui->screen_donelist_label_date2, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_donelist_label_date2, 205, 66);
    lv_obj_set_size(ui->screen_donelist_label_date2, 60, 13);

    //Write style for screen_donelist_label_date2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_donelist_label_date2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_donelist_label_date2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_donelist_label_date2, lv_color_hex(0x2f67a4), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_donelist_label_date2, &lv_font_SourceHanSansSC_Medium_11, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_donelist_label_date2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_donelist_label_date2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_donelist_label_date2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_donelist_label_date2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_donelist_label_date2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_donelist_label_date2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_donelist_label_date2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_donelist_label_date2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_donelist_label_date2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_donelist_label_date2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_donelist_label_date3
    ui->screen_donelist_label_date3 = lv_label_create(ui->screen_donelist);
    lv_label_set_text(ui->screen_donelist_label_date3, "2026.04.13");
    lv_label_set_long_mode(ui->screen_donelist_label_date3, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_donelist_label_date3, 205, 93);
    lv_obj_set_size(ui->screen_donelist_label_date3, 60, 13);

    //Write style for screen_donelist_label_date3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_donelist_label_date3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_donelist_label_date3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_donelist_label_date3, lv_color_hex(0x2f67a4), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_donelist_label_date3, &lv_font_SourceHanSansSC_Medium_11, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_donelist_label_date3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_donelist_label_date3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_donelist_label_date3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_donelist_label_date3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_donelist_label_date3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_donelist_label_date3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_donelist_label_date3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_donelist_label_date3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_donelist_label_date3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_donelist_label_date3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_donelist_label_date4
    ui->screen_donelist_label_date4 = lv_label_create(ui->screen_donelist);
    lv_label_set_text(ui->screen_donelist_label_date4, "2026.04.13");
    lv_label_set_long_mode(ui->screen_donelist_label_date4, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_donelist_label_date4, 205, 119);
    lv_obj_set_size(ui->screen_donelist_label_date4, 60, 13);

    //Write style for screen_donelist_label_date4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_donelist_label_date4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_donelist_label_date4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_donelist_label_date4, lv_color_hex(0x2f67a4), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_donelist_label_date4, &lv_font_SourceHanSansSC_Medium_11, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_donelist_label_date4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_donelist_label_date4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_donelist_label_date4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_donelist_label_date4, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_donelist_label_date4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_donelist_label_date4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_donelist_label_date4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_donelist_label_date4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_donelist_label_date4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_donelist_label_date4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_donelist_label_page
    ui->screen_donelist_label_page = lv_label_create(ui->screen_donelist);
    lv_label_set_text(ui->screen_donelist_label_page, "1/1");
    lv_label_set_long_mode(ui->screen_donelist_label_page, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_donelist_label_page, 117, 147);
    lv_obj_set_size(ui->screen_donelist_label_page, 90, 15);

    //Write style for screen_donelist_label_page, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_donelist_label_page, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_donelist_label_page, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_donelist_label_page, lv_color_hex(0x2f67a4), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_donelist_label_page, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_donelist_label_page, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_donelist_label_page, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_donelist_label_page, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_donelist_label_page, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_donelist_label_page, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_donelist_label_page, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_donelist_label_page, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_donelist_label_page, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_donelist_label_page, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_donelist_label_page, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_donelist_btn_save
    ui->screen_donelist_btn_save = lv_btn_create(ui->screen_donelist);
    ui->screen_donelist_btn_save_label = lv_label_create(ui->screen_donelist_btn_save);
    lv_label_set_text(ui->screen_donelist_btn_save_label, "");
    lv_label_set_long_mode(ui->screen_donelist_btn_save_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_donelist_btn_save_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_donelist_btn_save, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_donelist_btn_save_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_donelist_btn_save, 294, 7);
    lv_obj_set_size(ui->screen_donelist_btn_save, 18, 19);

    //Write style for screen_donelist_btn_save, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_donelist_btn_save, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_donelist_btn_save, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_donelist_btn_save, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_donelist_btn_save, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_donelist_btn_save, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_donelist_btn_save, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_donelist_btn_save, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_donelist_btn_save, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_donelist_btn_save, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_donelist_btn_save, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_donelist_btn_save, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_donelist_btn_save, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_donelist_btn_save, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_donelist_btn_save, lv_color_hex(0x2f67a4), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_donelist_btn_save, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_donelist_btn_save, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_donelist_btn_save, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_donelist_btn_save, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_donelist_btn_save, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_donelist_btn_save, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_donelist_btn_save, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_donelist_btn_nxtpage
    ui->screen_donelist_btn_nxtpage = lv_btn_create(ui->screen_donelist);
    ui->screen_donelist_btn_nxtpage_label = lv_label_create(ui->screen_donelist_btn_nxtpage);
    lv_label_set_text(ui->screen_donelist_btn_nxtpage_label, "");
    lv_label_set_long_mode(ui->screen_donelist_btn_nxtpage_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_donelist_btn_nxtpage_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_donelist_btn_nxtpage, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_donelist_btn_nxtpage_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_donelist_btn_nxtpage, 217, 145);
    lv_obj_set_size(ui->screen_donelist_btn_nxtpage, 97, 19);

    //Write style for screen_donelist_btn_nxtpage, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_donelist_btn_nxtpage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_donelist_btn_nxtpage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_donelist_btn_nxtpage, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_donelist_btn_nxtpage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_donelist_btn_nxtpage, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_donelist_btn_nxtpage, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_donelist_btn_nxtpage, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_donelist_btn_nxtpage, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_donelist_btn_nxtpage, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_donelist_btn_nxtpage, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_donelist_btn_nxtpage, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_donelist_btn_nxtpage, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_donelist_btn_nxtpage, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_donelist_btn_nxtpage, lv_color_hex(0x2f67a4), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_donelist_btn_nxtpage, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_donelist_btn_nxtpage, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_donelist_btn_nxtpage, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_donelist_btn_nxtpage, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_donelist_btn_nxtpage, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_donelist_btn_nxtpage, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_donelist_btn_nxtpage, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_donelist_btn_lastpage
    ui->screen_donelist_btn_lastpage = lv_btn_create(ui->screen_donelist);
    ui->screen_donelist_btn_lastpage_label = lv_label_create(ui->screen_donelist_btn_lastpage);
    lv_label_set_text(ui->screen_donelist_btn_lastpage_label, "");
    lv_label_set_long_mode(ui->screen_donelist_btn_lastpage_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_donelist_btn_lastpage_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_donelist_btn_lastpage, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_donelist_btn_lastpage_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_donelist_btn_lastpage, 5, 146);
    lv_obj_set_size(ui->screen_donelist_btn_lastpage, 94, 18);

    //Write style for screen_donelist_btn_lastpage, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_donelist_btn_lastpage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_donelist_btn_lastpage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_donelist_btn_lastpage, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_donelist_btn_lastpage, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_donelist_btn_lastpage, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_donelist_btn_lastpage, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_donelist_btn_lastpage, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_donelist_btn_lastpage, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_donelist_btn_lastpage, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_donelist_btn_lastpage, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_donelist_btn_lastpage, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_donelist_btn_lastpage, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_donelist_btn_lastpage, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_donelist_btn_lastpage, lv_color_hex(0x2f67a4), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_donelist_btn_lastpage, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_donelist_btn_lastpage, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_donelist_btn_lastpage, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_donelist_btn_lastpage, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_donelist_btn_lastpage, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_donelist_btn_lastpage, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_donelist_btn_lastpage, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_donelist_label_focus1
    ui->screen_donelist_label_focus1 = lv_label_create(ui->screen_donelist);
    lv_label_set_text(ui->screen_donelist_label_focus1, "20min");
    lv_label_set_long_mode(ui->screen_donelist_label_focus1, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_donelist_label_focus1, 268, 39);
    lv_obj_set_size(ui->screen_donelist_label_focus1, 40, 12);

    //Write style for screen_donelist_label_focus1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_donelist_label_focus1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_donelist_label_focus1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_donelist_label_focus1, lv_color_hex(0x2f67a4), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_donelist_label_focus1, &lv_font_SourceHanSansSC_Medium_11, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_donelist_label_focus1, 240, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_donelist_label_focus1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_donelist_label_focus1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_donelist_label_focus1, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_donelist_label_focus1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_donelist_label_focus1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_donelist_label_focus1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_donelist_label_focus1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_donelist_label_focus1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_donelist_label_focus1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_donelist_label_focus2
    ui->screen_donelist_label_focus2 = lv_label_create(ui->screen_donelist);
    lv_label_set_text(ui->screen_donelist_label_focus2, "200min");
    lv_label_set_long_mode(ui->screen_donelist_label_focus2, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_donelist_label_focus2, 268, 66);
    lv_obj_set_size(ui->screen_donelist_label_focus2, 40, 12);

    //Write style for screen_donelist_label_focus2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_donelist_label_focus2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_donelist_label_focus2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_donelist_label_focus2, lv_color_hex(0x2f67a4), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_donelist_label_focus2, &lv_font_SourceHanSansSC_Medium_11, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_donelist_label_focus2, 244, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_donelist_label_focus2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_donelist_label_focus2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_donelist_label_focus2, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_donelist_label_focus2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_donelist_label_focus2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_donelist_label_focus2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_donelist_label_focus2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_donelist_label_focus2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_donelist_label_focus2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_donelist_label_focus3
    ui->screen_donelist_label_focus3 = lv_label_create(ui->screen_donelist);
    lv_label_set_text(ui->screen_donelist_label_focus3, "20min");
    lv_label_set_long_mode(ui->screen_donelist_label_focus3, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_donelist_label_focus3, 268, 93);
    lv_obj_set_size(ui->screen_donelist_label_focus3, 40, 12);

    //Write style for screen_donelist_label_focus3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_donelist_label_focus3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_donelist_label_focus3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_donelist_label_focus3, lv_color_hex(0x2f67a4), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_donelist_label_focus3, &lv_font_SourceHanSansSC_Medium_11, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_donelist_label_focus3, 245, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_donelist_label_focus3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_donelist_label_focus3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_donelist_label_focus3, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_donelist_label_focus3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_donelist_label_focus3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_donelist_label_focus3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_donelist_label_focus3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_donelist_label_focus3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_donelist_label_focus3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_donelist_label_focus4
    ui->screen_donelist_label_focus4 = lv_label_create(ui->screen_donelist);
    lv_label_set_text(ui->screen_donelist_label_focus4, "80min");
    lv_label_set_long_mode(ui->screen_donelist_label_focus4, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_donelist_label_focus4, 268, 119);
    lv_obj_set_size(ui->screen_donelist_label_focus4, 40, 12);

    //Write style for screen_donelist_label_focus4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_donelist_label_focus4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_donelist_label_focus4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_donelist_label_focus4, lv_color_hex(0x2f67a4), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_donelist_label_focus4, &lv_font_SourceHanSansSC_Medium_11, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_donelist_label_focus4, 244, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_donelist_label_focus4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_donelist_label_focus4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_donelist_label_focus4, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_donelist_label_focus4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_donelist_label_focus4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_donelist_label_focus4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_donelist_label_focus4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_donelist_label_focus4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_donelist_label_focus4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_donelist_btn_back
    ui->screen_donelist_btn_back = lv_btn_create(ui->screen_donelist);
    ui->screen_donelist_btn_back_label = lv_label_create(ui->screen_donelist_btn_back);
    lv_label_set_text(ui->screen_donelist_btn_back_label, "<");
    lv_label_set_long_mode(ui->screen_donelist_btn_back_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_donelist_btn_back_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_donelist_btn_back, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_donelist_btn_back_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_donelist_btn_back, 7, 8);
    lv_obj_set_size(ui->screen_donelist_btn_back, 20, 17);

    //Write style for screen_donelist_btn_back, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_donelist_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_donelist_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_donelist_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_donelist_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_donelist_btn_back, lv_color_hex(0x2f67a4), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_donelist_btn_back, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_donelist_btn_back, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_donelist_btn_back, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_donelist_btn_back, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_donelist_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_donelist_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_donelist_btn_back, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_donelist_btn_back, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_donelist_btn_back, lv_color_hex(0x2f67a4), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_donelist_btn_back, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_donelist_btn_back, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_donelist_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_donelist_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_donelist_btn_back, lv_color_hex(0x2f67a4), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_donelist_btn_back, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_donelist_btn_back, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //The custom code of screen_donelist.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_donelist);

    //Init events for screen.
    events_init_screen_donelist(ui);
}
