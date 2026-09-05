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



void setup_scr_screen_gash(lv_ui *ui)
{
    //Write codes screen_gash
    ui->screen_gash = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_gash, 320, 170);
    lv_obj_set_scrollbar_mode(ui->screen_gash, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_gash, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_gash, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_gash, &_AAA_clawmachine_bg_0905_320x170, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_gash, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_gash, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_gash_img_stick
    ui->screen_gash_img_stick = lv_img_create(ui->screen_gash);
    lv_obj_add_flag(ui->screen_gash_img_stick, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_gash_img_stick, &_AAA_Handle_0905_alpha_10x16);
    lv_img_set_pivot(ui->screen_gash_img_stick, 50,50);
    lv_img_set_angle(ui->screen_gash_img_stick, 0);
    lv_obj_set_pos(ui->screen_gash_img_stick, 258, 134);
    lv_obj_set_size(ui->screen_gash_img_stick, 10, 16);

    //Write style for screen_gash_img_stick, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_gash_img_stick, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_gash_img_stick, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_gash_img_stick, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_gash_img_stick, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_gash_img_claw1
    ui->screen_gash_img_claw1 = lv_img_create(ui->screen_gash);
    lv_obj_add_flag(ui->screen_gash_img_claw1, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_gash_img_claw1, &_AAA_claw1_0905_alpha_15x42);
    lv_img_set_pivot(ui->screen_gash_img_claw1, 50,50);
    lv_img_set_angle(ui->screen_gash_img_claw1, 0);
    lv_obj_set_pos(ui->screen_gash_img_claw1, 41, 8);
    lv_obj_set_size(ui->screen_gash_img_claw1, 15, 42);

    //Write style for screen_gash_img_claw1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_gash_img_claw1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_gash_img_claw1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_gash_img_claw1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_gash_img_claw1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_gash_img_claw2
    ui->screen_gash_img_claw2 = lv_img_create(ui->screen_gash);
    lv_obj_add_flag(ui->screen_gash_img_claw2, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_gash_img_claw2, &_AAA_claw2_0905_alpha_15x58);
    lv_img_set_pivot(ui->screen_gash_img_claw2, 50,50);
    lv_img_set_angle(ui->screen_gash_img_claw2, 0);
    lv_obj_set_pos(ui->screen_gash_img_claw2, 41, 8);
    lv_obj_set_size(ui->screen_gash_img_claw2, 15, 58);

    //Write style for screen_gash_img_claw2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_gash_img_claw2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_gash_img_claw2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_gash_img_claw2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_gash_img_claw2, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_gash_img_claw3
    ui->screen_gash_img_claw3 = lv_img_create(ui->screen_gash);
    lv_obj_add_flag(ui->screen_gash_img_claw3, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_gash_img_claw3, &_AAA_claw3_0905_alpha_18x58);
    lv_img_set_pivot(ui->screen_gash_img_claw3, 50,50);
    lv_img_set_angle(ui->screen_gash_img_claw3, 0);
    lv_obj_set_pos(ui->screen_gash_img_claw3, 40, 8);
    lv_obj_set_size(ui->screen_gash_img_claw3, 18, 58);

    //Write style for screen_gash_img_claw3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_gash_img_claw3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_gash_img_claw3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_gash_img_claw3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_gash_img_claw3, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_gash_img_ball1
    ui->screen_gash_img_ball1 = lv_img_create(ui->screen_gash);
    lv_obj_add_flag(ui->screen_gash_img_ball1, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_gash_img_ball1, &_AAA_ball1_0905_alpha_26x26);
    lv_img_set_pivot(ui->screen_gash_img_ball1, 50,50);
    lv_img_set_angle(ui->screen_gash_img_ball1, 0);
    lv_obj_set_pos(ui->screen_gash_img_ball1, 109, 17);
    lv_obj_set_size(ui->screen_gash_img_ball1, 26, 26);

    //Write style for screen_gash_img_ball1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_gash_img_ball1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_gash_img_ball1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_gash_img_ball1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_gash_img_ball1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_gash_img_ball2
    ui->screen_gash_img_ball2 = lv_img_create(ui->screen_gash);
    lv_obj_add_flag(ui->screen_gash_img_ball2, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_gash_img_ball2, &_AAA_ball2_0905_alpha_26x26);
    lv_img_set_pivot(ui->screen_gash_img_ball2, 50,50);
    lv_img_set_angle(ui->screen_gash_img_ball2, 0);
    lv_obj_set_pos(ui->screen_gash_img_ball2, 149, 17);
    lv_obj_set_size(ui->screen_gash_img_ball2, 26, 26);

    //Write style for screen_gash_img_ball2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_gash_img_ball2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_gash_img_ball2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_gash_img_ball2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_gash_img_ball2, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_gash_img_ball3
    ui->screen_gash_img_ball3 = lv_img_create(ui->screen_gash);
    lv_obj_add_flag(ui->screen_gash_img_ball3, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_gash_img_ball3, &_AAA_ball3_0905_alpha_26x26);
    lv_img_set_pivot(ui->screen_gash_img_ball3, 50,50);
    lv_img_set_angle(ui->screen_gash_img_ball3, 0);
    lv_obj_set_pos(ui->screen_gash_img_ball3, 185, 17);
    lv_obj_set_size(ui->screen_gash_img_ball3, 26, 26);

    //Write style for screen_gash_img_ball3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_gash_img_ball3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_gash_img_ball3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_gash_img_ball3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_gash_img_ball3, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_gash_img_ball4
    ui->screen_gash_img_ball4 = lv_img_create(ui->screen_gash);
    lv_obj_add_flag(ui->screen_gash_img_ball4, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_gash_img_ball4, &_AAA_ball4_0905_alpha_26x26);
    lv_img_set_pivot(ui->screen_gash_img_ball4, 50,50);
    lv_img_set_angle(ui->screen_gash_img_ball4, 0);
    lv_obj_set_pos(ui->screen_gash_img_ball4, 227, 17);
    lv_obj_set_size(ui->screen_gash_img_ball4, 26, 26);

    //Write style for screen_gash_img_ball4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_gash_img_ball4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_gash_img_ball4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_gash_img_ball4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_gash_img_ball4, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_gash_img_randontask
    ui->screen_gash_img_randontask = lv_img_create(ui->screen_gash);
    lv_obj_add_flag(ui->screen_gash_img_randontask, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_gash_img_randontask, &_AAA_randomtask_0905_alpha_199x140);
    lv_img_set_pivot(ui->screen_gash_img_randontask, 50,50);
    lv_img_set_angle(ui->screen_gash_img_randontask, 0);
    lv_obj_set_pos(ui->screen_gash_img_randontask, 66, 17);
    lv_obj_set_size(ui->screen_gash_img_randontask, 199, 140);

    //Write style for screen_gash_img_randontask, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_gash_img_randontask, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_gash_img_randontask, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_gash_img_randontask, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_gash_img_randontask, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_gash_btn_addtodo
    ui->screen_gash_btn_addtodo = lv_btn_create(ui->screen_gash);
    ui->screen_gash_btn_addtodo_label = lv_label_create(ui->screen_gash_btn_addtodo);
    lv_label_set_text(ui->screen_gash_btn_addtodo_label, "Add todo");
    lv_label_set_long_mode(ui->screen_gash_btn_addtodo_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_gash_btn_addtodo_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_gash_btn_addtodo, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_gash_btn_addtodo_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_gash_btn_addtodo, 84, 122);
    lv_obj_set_size(ui->screen_gash_btn_addtodo, 77, 20);

    //Write style for screen_gash_btn_addtodo, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_gash_btn_addtodo, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_gash_btn_addtodo, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_gash_btn_addtodo, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_gash_btn_addtodo, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_gash_btn_addtodo, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_gash_btn_addtodo, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_gash_btn_addtodo, lv_color_hex(0x13407a), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_gash_btn_addtodo, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_gash_btn_addtodo, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_gash_btn_addtodo, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_gash_btn_addtodo, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_gash_btn_addtodo, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(ui->screen_gash_btn_addtodo, lv_color_hex(0x13407a), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_grad_dir(ui->screen_gash_btn_addtodo, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_gash_btn_addtodo, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_gash_btn_addtodo, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_gash_btn_addtodo, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_gash_btn_addtodo, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_gash_btn_addtodo, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_gash_btn_addtodo, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write style for screen_gash_btn_addtodo, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_bg_opa(ui->screen_gash_btn_addtodo, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(ui->screen_gash_btn_addtodo, lv_color_hex(0x13407a), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_bg_grad_dir(ui->screen_gash_btn_addtodo, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_border_width(ui->screen_gash_btn_addtodo, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_radius(ui->screen_gash_btn_addtodo, 5, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->screen_gash_btn_addtodo, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->screen_gash_btn_addtodo, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->screen_gash_btn_addtodo, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->screen_gash_btn_addtodo, 255, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write codes screen_gash_btn_again
    ui->screen_gash_btn_again = lv_btn_create(ui->screen_gash);
    ui->screen_gash_btn_again_label = lv_label_create(ui->screen_gash_btn_again);
    lv_label_set_text(ui->screen_gash_btn_again_label, "Once again");
    lv_label_set_long_mode(ui->screen_gash_btn_again_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_gash_btn_again_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_gash_btn_again, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_gash_btn_again_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_gash_btn_again, 170, 122);
    lv_obj_set_size(ui->screen_gash_btn_again, 77, 20);

    //Write style for screen_gash_btn_again, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_gash_btn_again, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_gash_btn_again, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_gash_btn_again, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_gash_btn_again, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_gash_btn_again, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_gash_btn_again, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_gash_btn_again, lv_color_hex(0x13407a), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_gash_btn_again, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_gash_btn_again, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_gash_btn_again, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_gash_btn_again, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_gash_btn_again, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(ui->screen_gash_btn_again, lv_color_hex(0x13407a), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_bg_grad_dir(ui->screen_gash_btn_again, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_gash_btn_again, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_gash_btn_again, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_gash_btn_again, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_gash_btn_again, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_gash_btn_again, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_gash_btn_again, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write style for screen_gash_btn_again, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_bg_opa(ui->screen_gash_btn_again, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(ui->screen_gash_btn_again, lv_color_hex(0x13407a), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_bg_grad_dir(ui->screen_gash_btn_again, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_border_width(ui->screen_gash_btn_again, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_radius(ui->screen_gash_btn_again, 5, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->screen_gash_btn_again, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->screen_gash_btn_again, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->screen_gash_btn_again, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->screen_gash_btn_again, 255, LV_PART_MAIN|LV_STATE_PRESSED);

    //The custom code of screen_gash.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_gash);

}
