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



void setup_scr_screen_book(lv_ui *ui)
{
    //Write codes screen_book
    ui->screen_book = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_book, 320, 170);
    lv_obj_set_scrollbar_mode(ui->screen_book, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_book, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_book, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_book, &_AAA_ebook_BG_320x170, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_book, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_book, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_book_label_author
    ui->screen_book_label_author = lv_label_create(ui->screen_book);
    lv_label_set_text(ui->screen_book_label_author, "- Hermann Hesse");
    lv_label_set_long_mode(ui->screen_book_label_author, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_book_label_author, 157, 10);
    lv_obj_set_size(ui->screen_book_label_author, 158, 16);
    lv_obj_add_flag(ui->screen_book_label_author, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_book_label_author, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_book_label_author, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_book_label_author, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_book_label_author, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_book_label_author, &lv_font_AlexBrush_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_book_label_author, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_book_label_author, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_book_label_author, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_book_label_author, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_book_label_author, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_book_label_author, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_book_label_author, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_book_label_author, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_book_label_author, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_book_label_author, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_book_label_content
    ui->screen_book_label_content = lv_label_create(ui->screen_book);
    lv_label_set_text(ui->screen_book_label_content, "A small creek ran behind the old mill, quiet except when the wind moved the reeds. I sat on a flat stone and watched the water carry leaves downstream. Nothing urgent waited for me. For a few minutes the day felt wide enough, and that was enough.For a few minutes the day felt wide enough, and that was enough.Nothing urgent waited for me. For a few minutes the day felt.");
    lv_label_set_long_mode(ui->screen_book_label_content, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_book_label_content, 5, 0);
    lv_obj_set_size(ui->screen_book_label_content, 310, 151);
    lv_obj_add_flag(ui->screen_book_label_content, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_book_label_content, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_book_label_content, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_book_label_content, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_book_label_content, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_book_label_content, &lv_font_arial_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_book_label_content, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_book_label_content, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_book_label_content, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_book_label_content, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_book_label_content, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_book_label_content, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_book_label_content, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_book_label_content, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_book_label_content, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_book_label_content, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_book_label_pageindex
    ui->screen_book_label_pageindex = lv_label_create(ui->screen_book);
    lv_label_set_text(ui->screen_book_label_pageindex, "1");
    lv_label_set_long_mode(ui->screen_book_label_pageindex, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_book_label_pageindex, 235, 156);
    lv_obj_set_size(ui->screen_book_label_pageindex, 80, 11);
    lv_obj_add_flag(ui->screen_book_label_pageindex, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_book_label_pageindex, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_book_label_pageindex, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_book_label_pageindex, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_book_label_pageindex, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_book_label_pageindex, &lv_font_ArchitectsDaughter_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_book_label_pageindex, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_book_label_pageindex, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_book_label_pageindex, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_book_label_pageindex, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_book_label_pageindex, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_book_label_pageindex, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_book_label_pageindex, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_book_label_pageindex, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_book_label_pageindex, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_book_label_pageindex, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_book_label_partname
    ui->screen_book_label_partname = lv_label_create(ui->screen_book);
    lv_label_set_text(ui->screen_book_label_partname, "The son of the Brahman");
    lv_label_set_long_mode(ui->screen_book_label_partname, LV_LABEL_LONG_SCROLL);
    lv_obj_set_pos(ui->screen_book_label_partname, 12, 111);
    lv_obj_set_size(ui->screen_book_label_partname, 210, 21);

    //Write style for screen_book_label_partname, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_book_label_partname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_book_label_partname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_book_label_partname, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_book_label_partname, &lv_font_SourceHanSansSC_Medium_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_book_label_partname, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_book_label_partname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_book_label_partname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_book_label_partname, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_book_label_partname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_book_label_partname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_book_label_partname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_book_label_partname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_book_label_partname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_book_label_partname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_book_label_partindex
    ui->screen_book_label_partindex = lv_label_create(ui->screen_book);
    lv_label_set_text(ui->screen_book_label_partindex, "Part 1");
    lv_label_set_long_mode(ui->screen_book_label_partindex, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_book_label_partindex, 12, 82);
    lv_obj_set_size(ui->screen_book_label_partindex, 87, 21);

    //Write style for screen_book_label_partindex, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_book_label_partindex, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_book_label_partindex, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_book_label_partindex, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_book_label_partindex, &lv_font_SourceHanSansSC_Medium_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_book_label_partindex, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_book_label_partindex, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_book_label_partindex, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_book_label_partindex, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_book_label_partindex, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_book_label_partindex, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_book_label_partindex, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_book_label_partindex, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_book_label_partindex, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_book_label_partindex, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_book_label_bookname
    ui->screen_book_label_bookname = lv_label_create(ui->screen_book);
    lv_label_set_text(ui->screen_book_label_bookname, "Siddhartha");
    lv_label_set_long_mode(ui->screen_book_label_bookname, LV_LABEL_LONG_SCROLL);
    lv_obj_set_pos(ui->screen_book_label_bookname, 12, 10);
    lv_obj_set_size(ui->screen_book_label_bookname, 125, 20);

    //Write style for screen_book_label_bookname, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_book_label_bookname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_book_label_bookname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_book_label_bookname, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_book_label_bookname, &lv_font_SourceHanSansSC_Normal_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_book_label_bookname, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_book_label_bookname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_book_label_bookname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_book_label_bookname, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_book_label_bookname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_book_label_bookname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_book_label_bookname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_book_label_bookname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_book_label_bookname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_book_label_bookname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_book_img_content_bg
    ui->screen_book_img_content_bg = lv_img_create(ui->screen_book);
    lv_obj_add_flag(ui->screen_book_img_content_bg, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_book_img_content_bg, &_AAA_ebook_contents_BG_alpha_136x170);
    lv_img_set_pivot(ui->screen_book_img_content_bg, 50,50);
    lv_img_set_angle(ui->screen_book_img_content_bg, 0);
    lv_obj_set_pos(ui->screen_book_img_content_bg, 184, 0);
    lv_obj_set_size(ui->screen_book_img_content_bg, 136, 170);

    //Write style for screen_book_img_content_bg, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_book_img_content_bg, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_book_img_content_bg, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_book_img_content_bg, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_book_img_content_bg, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_book_list_part
    ui->screen_book_list_part = lv_list_create(ui->screen_book);
    ui->screen_book_list_part_item0 = lv_list_add_btn(ui->screen_book_list_part, LV_SYMBOL_BARS, "PART_1");
    ui->screen_book_list_part_item1 = lv_list_add_btn(ui->screen_book_list_part, LV_SYMBOL_BARS, "PART_2");
    ui->screen_book_list_part_item2 = lv_list_add_btn(ui->screen_book_list_part, LV_SYMBOL_BARS, "PART_3");
    ui->screen_book_list_part_item3 = lv_list_add_btn(ui->screen_book_list_part, LV_SYMBOL_BARS, "PART_4");
    ui->screen_book_list_part_item4 = lv_list_add_btn(ui->screen_book_list_part, LV_SYMBOL_BARS, "PART_5");
    ui->screen_book_list_part_item5 = lv_list_add_btn(ui->screen_book_list_part, LV_SYMBOL_BARS, "PART_6");
    lv_obj_set_pos(ui->screen_book_list_part, 195, 26);
    lv_obj_set_size(ui->screen_book_list_part, 122, 138);
    lv_obj_set_scrollbar_mode(ui->screen_book_list_part, LV_SCROLLBAR_MODE_OFF);

    //Write style state: LV_STATE_DEFAULT for &style_screen_book_list_part_main_main_default
    static lv_style_t style_screen_book_list_part_main_main_default;
    ui_init_style(&style_screen_book_list_part_main_main_default);

    lv_style_set_pad_top(&style_screen_book_list_part_main_main_default, 5);
    lv_style_set_pad_left(&style_screen_book_list_part_main_main_default, 5);
    lv_style_set_pad_right(&style_screen_book_list_part_main_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_book_list_part_main_main_default, 5);
    lv_style_set_bg_opa(&style_screen_book_list_part_main_main_default, 255);
    lv_style_set_bg_color(&style_screen_book_list_part_main_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_book_list_part_main_main_default, LV_GRAD_DIR_NONE);
    lv_style_set_border_width(&style_screen_book_list_part_main_main_default, 1);
    lv_style_set_border_opa(&style_screen_book_list_part_main_main_default, 255);
    lv_style_set_border_color(&style_screen_book_list_part_main_main_default, lv_color_hex(0xe1e6ee));
    lv_style_set_border_side(&style_screen_book_list_part_main_main_default, LV_BORDER_SIDE_FULL);
    lv_style_set_radius(&style_screen_book_list_part_main_main_default, 3);
    lv_style_set_shadow_width(&style_screen_book_list_part_main_main_default, 0);
    lv_obj_add_style(ui->screen_book_list_part, &style_screen_book_list_part_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_book_list_part_main_scrollbar_default
    static lv_style_t style_screen_book_list_part_main_scrollbar_default;
    ui_init_style(&style_screen_book_list_part_main_scrollbar_default);

    lv_style_set_radius(&style_screen_book_list_part_main_scrollbar_default, 3);
    lv_style_set_bg_opa(&style_screen_book_list_part_main_scrollbar_default, 255);
    lv_style_set_bg_color(&style_screen_book_list_part_main_scrollbar_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_book_list_part_main_scrollbar_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_book_list_part, &style_screen_book_list_part_main_scrollbar_default, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_book_list_part_extra_btns_main_default
    static lv_style_t style_screen_book_list_part_extra_btns_main_default;
    ui_init_style(&style_screen_book_list_part_extra_btns_main_default);

    lv_style_set_pad_top(&style_screen_book_list_part_extra_btns_main_default, 5);
    lv_style_set_pad_left(&style_screen_book_list_part_extra_btns_main_default, 5);
    lv_style_set_pad_right(&style_screen_book_list_part_extra_btns_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_book_list_part_extra_btns_main_default, 5);
    lv_style_set_border_width(&style_screen_book_list_part_extra_btns_main_default, 0);
    lv_style_set_text_color(&style_screen_book_list_part_extra_btns_main_default, lv_color_hex(0x0D3055));
    lv_style_set_text_font(&style_screen_book_list_part_extra_btns_main_default, &lv_font_montserratMedium_12);
    lv_style_set_text_opa(&style_screen_book_list_part_extra_btns_main_default, 255);
    lv_style_set_radius(&style_screen_book_list_part_extra_btns_main_default, 3);
    lv_style_set_bg_opa(&style_screen_book_list_part_extra_btns_main_default, 255);
    lv_style_set_bg_color(&style_screen_book_list_part_extra_btns_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_book_list_part_extra_btns_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_book_list_part_item5, &style_screen_book_list_part_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_book_list_part_item4, &style_screen_book_list_part_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_book_list_part_item3, &style_screen_book_list_part_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_book_list_part_item2, &style_screen_book_list_part_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_book_list_part_item1, &style_screen_book_list_part_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_book_list_part_item0, &style_screen_book_list_part_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_book_list_part_extra_texts_main_default
    static lv_style_t style_screen_book_list_part_extra_texts_main_default;
    ui_init_style(&style_screen_book_list_part_extra_texts_main_default);

    lv_style_set_pad_top(&style_screen_book_list_part_extra_texts_main_default, 5);
    lv_style_set_pad_left(&style_screen_book_list_part_extra_texts_main_default, 5);
    lv_style_set_pad_right(&style_screen_book_list_part_extra_texts_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_book_list_part_extra_texts_main_default, 5);
    lv_style_set_border_width(&style_screen_book_list_part_extra_texts_main_default, 0);
    lv_style_set_text_color(&style_screen_book_list_part_extra_texts_main_default, lv_color_hex(0x0D3055));
    lv_style_set_text_font(&style_screen_book_list_part_extra_texts_main_default, &lv_font_montserratMedium_12);
    lv_style_set_text_opa(&style_screen_book_list_part_extra_texts_main_default, 255);
    lv_style_set_radius(&style_screen_book_list_part_extra_texts_main_default, 3);
    lv_style_set_transform_width(&style_screen_book_list_part_extra_texts_main_default, 0);
    lv_style_set_bg_opa(&style_screen_book_list_part_extra_texts_main_default, 255);
    lv_style_set_bg_color(&style_screen_book_list_part_extra_texts_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_book_list_part_extra_texts_main_default, LV_GRAD_DIR_NONE);

    //Write codes screen_book_btn_back
    ui->screen_book_btn_back = lv_btn_create(ui->screen_book);
    ui->screen_book_btn_back_label = lv_label_create(ui->screen_book_btn_back);
    lv_label_set_text(ui->screen_book_btn_back_label, "");
    lv_label_set_long_mode(ui->screen_book_btn_back_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_book_btn_back_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_book_btn_back, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_book_btn_back_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_book_btn_back, 199, 7);
    lv_obj_set_size(ui->screen_book_btn_back, 12, 14);

    //Write style for screen_book_btn_back, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_book_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_book_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_book_btn_back, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_book_btn_back, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_book_btn_back, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_book_btn_back, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_book_btn_back, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_book_btn_back, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_book_btn_back, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_book_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_book_btn_back, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_opa(ui->screen_book_btn_back, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_color(ui->screen_book_btn_back, lv_color_hex(0x676767), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_side(ui->screen_book_btn_back, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_book_btn_back, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_book_btn_back, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_book_btn_back, lv_color_hex(0x454545), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_book_btn_back, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_book_btn_back, 2, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_book_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_book_btn_back, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_book_btn_back, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_book_btn_back, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_book_btn_back, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //The custom code of screen_book.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_book);

}
