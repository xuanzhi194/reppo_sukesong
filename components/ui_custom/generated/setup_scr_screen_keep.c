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



void setup_scr_screen_keep(lv_ui *ui)
{
    //Write codes screen_keep
    ui->screen_keep = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_keep, 320, 170);
    lv_obj_set_scrollbar_mode(ui->screen_keep, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_keep, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_keep, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_keep, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_keep, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_keep_img_bg
    ui->screen_keep_img_bg = lv_img_create(ui->screen_keep);
    lv_obj_add_flag(ui->screen_keep_img_bg, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_keep_img_bg, &_AAA_keep_bg_alpha_312x137);
    lv_img_set_pivot(ui->screen_keep_img_bg, 50,50);
    lv_img_set_angle(ui->screen_keep_img_bg, 0);
    lv_obj_set_pos(ui->screen_keep_img_bg, 4, 22);
    lv_obj_set_size(ui->screen_keep_img_bg, 312, 137);

    //Write style for screen_keep_img_bg, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_keep_img_bg, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_keep_img_bg, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_keep_img_bg, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_keep_img_bg, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_keep_img_lv10_wither
    ui->screen_keep_img_lv10_wither = lv_img_create(ui->screen_keep);
    lv_obj_add_flag(ui->screen_keep_img_lv10_wither, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_keep_img_lv10_wither, &_AAA_keep_LV10_wither_alpha_200x150);
    lv_img_set_pivot(ui->screen_keep_img_lv10_wither, 50,50);
    lv_img_set_angle(ui->screen_keep_img_lv10_wither, 0);
    lv_obj_set_pos(ui->screen_keep_img_lv10_wither, 0, 0);
    lv_obj_set_size(ui->screen_keep_img_lv10_wither, 200, 150);
    lv_obj_add_flag(ui->screen_keep_img_lv10_wither, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_keep_img_lv10_wither, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_keep_img_lv10_wither, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_keep_img_lv10_wither, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_keep_img_lv10_wither, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_keep_img_lv10_wither, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_keep_img_lv10
    ui->screen_keep_img_lv10 = lv_img_create(ui->screen_keep);
    lv_obj_add_flag(ui->screen_keep_img_lv10, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_keep_img_lv10, &_AAA_keep_LV10_alpha_184x160);
    lv_img_set_pivot(ui->screen_keep_img_lv10, 50,50);
    lv_img_set_angle(ui->screen_keep_img_lv10, 0);
    lv_obj_set_pos(ui->screen_keep_img_lv10, 0, 0);
    lv_obj_set_size(ui->screen_keep_img_lv10, 184, 160);
    lv_obj_add_flag(ui->screen_keep_img_lv10, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_keep_img_lv10, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_keep_img_lv10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_keep_img_lv10, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_keep_img_lv10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_keep_img_lv10, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_keep_img_lv9_wither
    ui->screen_keep_img_lv9_wither = lv_img_create(ui->screen_keep);
    lv_obj_add_flag(ui->screen_keep_img_lv9_wither, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_keep_img_lv9_wither, &_AAA_keep_LV9_wither_alpha_170x155);
    lv_img_set_pivot(ui->screen_keep_img_lv9_wither, 50,50);
    lv_img_set_angle(ui->screen_keep_img_lv9_wither, 0);
    lv_obj_set_pos(ui->screen_keep_img_lv9_wither, 0, 1);
    lv_obj_set_size(ui->screen_keep_img_lv9_wither, 170, 155);
    lv_obj_add_flag(ui->screen_keep_img_lv9_wither, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_keep_img_lv9_wither, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_keep_img_lv9_wither, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_keep_img_lv9_wither, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_keep_img_lv9_wither, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_keep_img_lv9_wither, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_keep_img_lv9
    ui->screen_keep_img_lv9 = lv_img_create(ui->screen_keep);
    lv_obj_add_flag(ui->screen_keep_img_lv9, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_keep_img_lv9, &_AAA_keep_LV9_alpha_165x161);
    lv_img_set_pivot(ui->screen_keep_img_lv9, 50,50);
    lv_img_set_angle(ui->screen_keep_img_lv9, 0);
    lv_obj_set_pos(ui->screen_keep_img_lv9, 0, 0);
    lv_obj_set_size(ui->screen_keep_img_lv9, 165, 161);
    lv_obj_add_flag(ui->screen_keep_img_lv9, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_keep_img_lv9, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_keep_img_lv9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_keep_img_lv9, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_keep_img_lv9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_keep_img_lv9, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_keep_img_lv8_wither
    ui->screen_keep_img_lv8_wither = lv_img_create(ui->screen_keep);
    lv_obj_add_flag(ui->screen_keep_img_lv8_wither, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_keep_img_lv8_wither, &_AAA_keep_LV8_wither_alpha_154x170);
    lv_img_set_pivot(ui->screen_keep_img_lv8_wither, 50,50);
    lv_img_set_angle(ui->screen_keep_img_lv8_wither, 0);
    lv_obj_set_pos(ui->screen_keep_img_lv8_wither, 0, 0);
    lv_obj_set_size(ui->screen_keep_img_lv8_wither, 154, 170);
    lv_obj_add_flag(ui->screen_keep_img_lv8_wither, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_keep_img_lv8_wither, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_keep_img_lv8_wither, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_keep_img_lv8_wither, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_keep_img_lv8_wither, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_keep_img_lv8_wither, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_keep_img_lv8
    ui->screen_keep_img_lv8 = lv_img_create(ui->screen_keep);
    lv_obj_add_flag(ui->screen_keep_img_lv8, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_keep_img_lv8, &_AAA_keep_LV8_alpha_154x170);
    lv_img_set_pivot(ui->screen_keep_img_lv8, 50,50);
    lv_img_set_angle(ui->screen_keep_img_lv8, 0);
    lv_obj_set_pos(ui->screen_keep_img_lv8, 0, 0);
    lv_obj_set_size(ui->screen_keep_img_lv8, 154, 170);
    lv_obj_add_flag(ui->screen_keep_img_lv8, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_keep_img_lv8, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_keep_img_lv8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_keep_img_lv8, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_keep_img_lv8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_keep_img_lv8, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_keep_img_lv7_wither
    ui->screen_keep_img_lv7_wither = lv_img_create(ui->screen_keep);
    lv_obj_add_flag(ui->screen_keep_img_lv7_wither, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_keep_img_lv7_wither, &_AAA_keep_LV7_wither_alpha_186x170);
    lv_img_set_pivot(ui->screen_keep_img_lv7_wither, 50,50);
    lv_img_set_angle(ui->screen_keep_img_lv7_wither, 0);
    lv_obj_set_pos(ui->screen_keep_img_lv7_wither, 0, 0);
    lv_obj_set_size(ui->screen_keep_img_lv7_wither, 186, 170);
    lv_obj_add_flag(ui->screen_keep_img_lv7_wither, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_keep_img_lv7_wither, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_keep_img_lv7_wither, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_keep_img_lv7_wither, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_keep_img_lv7_wither, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_keep_img_lv7_wither, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_keep_img_lv7
    ui->screen_keep_img_lv7 = lv_img_create(ui->screen_keep);
    lv_obj_add_flag(ui->screen_keep_img_lv7, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_keep_img_lv7, &_AAA_keep_LV7_alpha_186x170);
    lv_img_set_pivot(ui->screen_keep_img_lv7, 50,50);
    lv_img_set_angle(ui->screen_keep_img_lv7, 0);
    lv_obj_set_pos(ui->screen_keep_img_lv7, 0, 0);
    lv_obj_set_size(ui->screen_keep_img_lv7, 186, 170);
    lv_obj_add_flag(ui->screen_keep_img_lv7, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_keep_img_lv7, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_keep_img_lv7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_keep_img_lv7, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_keep_img_lv7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_keep_img_lv7, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_keep_img_lv6_wither
    ui->screen_keep_img_lv6_wither = lv_img_create(ui->screen_keep);
    lv_obj_add_flag(ui->screen_keep_img_lv6_wither, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_keep_img_lv6_wither, &_AAA_keep_LV6_wither_alpha_180x160);
    lv_img_set_pivot(ui->screen_keep_img_lv6_wither, 50,50);
    lv_img_set_angle(ui->screen_keep_img_lv6_wither, 0);
    lv_obj_set_pos(ui->screen_keep_img_lv6_wither, 0, 5);
    lv_obj_set_size(ui->screen_keep_img_lv6_wither, 180, 160);
    lv_obj_add_flag(ui->screen_keep_img_lv6_wither, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_keep_img_lv6_wither, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_keep_img_lv6_wither, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_keep_img_lv6_wither, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_keep_img_lv6_wither, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_keep_img_lv6_wither, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_keep_img_lv6
    ui->screen_keep_img_lv6 = lv_img_create(ui->screen_keep);
    lv_obj_add_flag(ui->screen_keep_img_lv6, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_keep_img_lv6, &_AAA_keep_LV6_alpha_189x160);
    lv_img_set_pivot(ui->screen_keep_img_lv6, 50,50);
    lv_img_set_angle(ui->screen_keep_img_lv6, 0);
    lv_obj_set_pos(ui->screen_keep_img_lv6, 0, 5);
    lv_obj_set_size(ui->screen_keep_img_lv6, 189, 160);
    lv_obj_add_flag(ui->screen_keep_img_lv6, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_keep_img_lv6, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_keep_img_lv6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_keep_img_lv6, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_keep_img_lv6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_keep_img_lv6, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_keep_img_lv5_wither
    ui->screen_keep_img_lv5_wither = lv_img_create(ui->screen_keep);
    lv_obj_add_flag(ui->screen_keep_img_lv5_wither, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_keep_img_lv5_wither, &_AAA_keep_LV5_wither_alpha_170x158);
    lv_img_set_pivot(ui->screen_keep_img_lv5_wither, 50,50);
    lv_img_set_angle(ui->screen_keep_img_lv5_wither, 0);
    lv_obj_set_pos(ui->screen_keep_img_lv5_wither, 0, 5);
    lv_obj_set_size(ui->screen_keep_img_lv5_wither, 170, 158);
    lv_obj_add_flag(ui->screen_keep_img_lv5_wither, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_keep_img_lv5_wither, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_keep_img_lv5_wither, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_keep_img_lv5_wither, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_keep_img_lv5_wither, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_keep_img_lv5_wither, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_keep_img_lv5
    ui->screen_keep_img_lv5 = lv_img_create(ui->screen_keep);
    lv_obj_add_flag(ui->screen_keep_img_lv5, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_keep_img_lv5, &_AAA_keep_LV5_alpha_170x158);
    lv_img_set_pivot(ui->screen_keep_img_lv5, 50,50);
    lv_img_set_angle(ui->screen_keep_img_lv5, 0);
    lv_obj_set_pos(ui->screen_keep_img_lv5, 0, 5);
    lv_obj_set_size(ui->screen_keep_img_lv5, 170, 158);
    lv_obj_add_flag(ui->screen_keep_img_lv5, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_keep_img_lv5, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_keep_img_lv5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_keep_img_lv5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_keep_img_lv5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_keep_img_lv5, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_keep_img_lv4_wither
    ui->screen_keep_img_lv4_wither = lv_img_create(ui->screen_keep);
    lv_obj_add_flag(ui->screen_keep_img_lv4_wither, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_keep_img_lv4_wither, &_AAA_keep_LV4_wither_alpha_164x152);
    lv_img_set_pivot(ui->screen_keep_img_lv4_wither, 50,50);
    lv_img_set_angle(ui->screen_keep_img_lv4_wither, 0);
    lv_obj_set_pos(ui->screen_keep_img_lv4_wither, 0, 0);
    lv_obj_set_size(ui->screen_keep_img_lv4_wither, 164, 152);
    lv_obj_add_flag(ui->screen_keep_img_lv4_wither, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_keep_img_lv4_wither, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_keep_img_lv4_wither, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_keep_img_lv4_wither, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_keep_img_lv4_wither, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_keep_img_lv4_wither, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_keep_img_lv4
    ui->screen_keep_img_lv4 = lv_img_create(ui->screen_keep);
    lv_obj_add_flag(ui->screen_keep_img_lv4, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_keep_img_lv4, &_AAA_keep_LV4_alpha_164x149);
    lv_img_set_pivot(ui->screen_keep_img_lv4, 50,50);
    lv_img_set_angle(ui->screen_keep_img_lv4, 0);
    lv_obj_set_pos(ui->screen_keep_img_lv4, 0, 1);
    lv_obj_set_size(ui->screen_keep_img_lv4, 164, 149);
    lv_obj_add_flag(ui->screen_keep_img_lv4, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_keep_img_lv4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_keep_img_lv4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_keep_img_lv4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_keep_img_lv4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_keep_img_lv4, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_keep_img_lv3_wither
    ui->screen_keep_img_lv3_wither = lv_img_create(ui->screen_keep);
    lv_obj_add_flag(ui->screen_keep_img_lv3_wither, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_keep_img_lv3_wither, &_AAA_keep_LV3_wither_alpha_114x147);
    lv_img_set_pivot(ui->screen_keep_img_lv3_wither, 50,50);
    lv_img_set_angle(ui->screen_keep_img_lv3_wither, 0);
    lv_obj_set_pos(ui->screen_keep_img_lv3_wither, 9, 3);
    lv_obj_set_size(ui->screen_keep_img_lv3_wither, 114, 147);
    lv_obj_add_flag(ui->screen_keep_img_lv3_wither, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_keep_img_lv3_wither, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_keep_img_lv3_wither, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_keep_img_lv3_wither, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_keep_img_lv3_wither, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_keep_img_lv3_wither, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_keep_img_lv3
    ui->screen_keep_img_lv3 = lv_img_create(ui->screen_keep);
    lv_obj_add_flag(ui->screen_keep_img_lv3, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_keep_img_lv3, &_AAA_keep_LV3_alpha_114x147);
    lv_img_set_pivot(ui->screen_keep_img_lv3, 50,50);
    lv_img_set_angle(ui->screen_keep_img_lv3, 0);
    lv_obj_set_pos(ui->screen_keep_img_lv3, 10, 5);
    lv_obj_set_size(ui->screen_keep_img_lv3, 114, 147);
    lv_obj_add_flag(ui->screen_keep_img_lv3, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_keep_img_lv3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_keep_img_lv3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_keep_img_lv3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_keep_img_lv3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_keep_img_lv3, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_keep_img_lv2_wither
    ui->screen_keep_img_lv2_wither = lv_img_create(ui->screen_keep);
    lv_obj_add_flag(ui->screen_keep_img_lv2_wither, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_keep_img_lv2_wither, &_AAA_keep_LV2_wither_alpha_88x141);
    lv_img_set_pivot(ui->screen_keep_img_lv2_wither, 50,50);
    lv_img_set_angle(ui->screen_keep_img_lv2_wither, 0);
    lv_obj_set_pos(ui->screen_keep_img_lv2_wither, 16, 11);
    lv_obj_set_size(ui->screen_keep_img_lv2_wither, 88, 141);
    lv_obj_add_flag(ui->screen_keep_img_lv2_wither, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_keep_img_lv2_wither, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_keep_img_lv2_wither, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_keep_img_lv2_wither, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_keep_img_lv2_wither, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_keep_img_lv2_wither, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_keep_img_lv2
    ui->screen_keep_img_lv2 = lv_img_create(ui->screen_keep);
    lv_obj_add_flag(ui->screen_keep_img_lv2, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_keep_img_lv2, &_AAA_keep_LV2_alpha_88x141);
    lv_img_set_pivot(ui->screen_keep_img_lv2, 50,50);
    lv_img_set_angle(ui->screen_keep_img_lv2, 0);
    lv_obj_set_pos(ui->screen_keep_img_lv2, 16, 11);
    lv_obj_set_size(ui->screen_keep_img_lv2, 88, 141);
    lv_obj_add_flag(ui->screen_keep_img_lv2, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_keep_img_lv2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_keep_img_lv2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_keep_img_lv2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_keep_img_lv2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_keep_img_lv2, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_keep_img_lv1
    ui->screen_keep_img_lv1 = lv_img_create(ui->screen_keep);
    lv_obj_add_flag(ui->screen_keep_img_lv1, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_keep_img_lv1, &_AAA_keep_LV1_alpha_96x107);
    lv_img_set_pivot(ui->screen_keep_img_lv1, 50,50);
    lv_img_set_angle(ui->screen_keep_img_lv1, 0);
    lv_obj_set_pos(ui->screen_keep_img_lv1, 12, 44);
    lv_obj_set_size(ui->screen_keep_img_lv1, 96, 107);

    //Write style for screen_keep_img_lv1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_keep_img_lv1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_keep_img_lv1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_keep_img_lv1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_keep_img_lv1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_keep_img_lv1_wither
    ui->screen_keep_img_lv1_wither = lv_img_create(ui->screen_keep);
    lv_obj_add_flag(ui->screen_keep_img_lv1_wither, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_keep_img_lv1_wither, &_AAA_keep_LV1_wither_alpha_94x106);
    lv_img_set_pivot(ui->screen_keep_img_lv1_wither, 50,50);
    lv_img_set_angle(ui->screen_keep_img_lv1_wither, 0);
    lv_obj_set_pos(ui->screen_keep_img_lv1_wither, 12, 45);
    lv_obj_set_size(ui->screen_keep_img_lv1_wither, 94, 106);
    lv_obj_add_flag(ui->screen_keep_img_lv1_wither, LV_OBJ_FLAG_HIDDEN);

    //Write style for screen_keep_img_lv1_wither, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_keep_img_lv1_wither, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_keep_img_lv1_wither, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_keep_img_lv1_wither, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_keep_img_lv1_wither, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_keep_bar_focustime
    ui->screen_keep_bar_focustime = lv_bar_create(ui->screen_keep);
    lv_obj_set_style_anim_time(ui->screen_keep_bar_focustime, 1000, 0);
    lv_bar_set_mode(ui->screen_keep_bar_focustime, LV_BAR_MODE_NORMAL);
    lv_bar_set_range(ui->screen_keep_bar_focustime, 0, 100);
    lv_bar_set_value(ui->screen_keep_bar_focustime, 50, LV_ANIM_OFF);
    lv_obj_set_pos(ui->screen_keep_bar_focustime, 9, 150);
    lv_obj_set_size(ui->screen_keep_bar_focustime, 297, 4);

    //Write style for screen_keep_bar_focustime, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_keep_bar_focustime, 60, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_keep_bar_focustime, lv_color_hex(0x4d4e4e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_keep_bar_focustime, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_keep_bar_focustime, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_keep_bar_focustime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_keep_bar_focustime, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_keep_bar_focustime, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_keep_bar_focustime, lv_color_hex(0x555d62), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_keep_bar_focustime, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_keep_bar_focustime, 10, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write codes screen_keep_img_process_icon
    ui->screen_keep_img_process_icon = lv_img_create(ui->screen_keep);
    lv_obj_add_flag(ui->screen_keep_img_process_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_keep_img_process_icon, &_AAA_keep_icon_alpha_14x14);
    lv_img_set_pivot(ui->screen_keep_img_process_icon, 50,50);
    lv_img_set_angle(ui->screen_keep_img_process_icon, 0);
    lv_obj_set_pos(ui->screen_keep_img_process_icon, 297, 134);
    lv_obj_set_size(ui->screen_keep_img_process_icon, 14, 14);

    //Write style for screen_keep_img_process_icon, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_keep_img_process_icon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_keep_img_process_icon, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_keep_img_process_icon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_keep_img_process_icon, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_keep_img_dialog
    ui->screen_keep_img_dialog = lv_img_create(ui->screen_keep);
    lv_obj_add_flag(ui->screen_keep_img_dialog, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_keep_img_dialog, &_AAA_keep_dialogbox_alpha_204x78);
    lv_img_set_pivot(ui->screen_keep_img_dialog, 50,50);
    lv_img_set_angle(ui->screen_keep_img_dialog, 0);
    lv_obj_set_pos(ui->screen_keep_img_dialog, 102, 35);
    lv_obj_set_size(ui->screen_keep_img_dialog, 204, 78);

    //Write style for screen_keep_img_dialog, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_keep_img_dialog, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_keep_img_dialog, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_keep_img_dialog, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_keep_img_dialog, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_keep_label_state
    ui->screen_keep_label_state = lv_label_create(ui->screen_keep);
    lv_label_set_text(ui->screen_keep_label_state, "First Sprout");
    lv_label_set_long_mode(ui->screen_keep_label_state, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_keep_label_state, 200, 2);
    lv_obj_set_size(ui->screen_keep_label_state, 114, 18);

    //Write style for screen_keep_label_state, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_keep_label_state, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_keep_label_state, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_keep_label_state, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_keep_label_state, &lv_font_SourceHanSansSC_Medium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_keep_label_state, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_keep_label_state, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_keep_label_state, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_keep_label_state, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_keep_label_state, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_keep_label_state, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_keep_label_state, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_keep_label_state, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_keep_label_state, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_keep_label_state, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_keep_label_conversion
    ui->screen_keep_label_conversion = lv_label_create(ui->screen_keep);
    lv_label_set_text(ui->screen_keep_label_conversion, "You're doing a great job in taking care of me !");
    lv_label_set_long_mode(ui->screen_keep_label_conversion, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_keep_label_conversion, 116, 48);
    lv_obj_set_size(ui->screen_keep_label_conversion, 187, 44);

    //Write style for screen_keep_label_conversion, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_keep_label_conversion, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_keep_label_conversion, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_keep_label_conversion, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_keep_label_conversion, &lv_font_arial_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_keep_label_conversion, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_keep_label_conversion, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_keep_label_conversion, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_keep_label_conversion, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_keep_label_conversion, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_keep_label_conversion, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_keep_label_conversion, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_keep_label_conversion, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_keep_label_conversion, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_keep_label_conversion, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_keep_label_focustime
    ui->screen_keep_label_focustime = lv_label_create(ui->screen_keep);
    lv_label_set_text(ui->screen_keep_label_focustime, "0/180 MIN");
    lv_label_set_long_mode(ui->screen_keep_label_focustime, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_keep_label_focustime, 225, 156);
    lv_obj_set_size(ui->screen_keep_label_focustime, 80, 11);

    //Write style for screen_keep_label_focustime, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_keep_label_focustime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_keep_label_focustime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_keep_label_focustime, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_keep_label_focustime, &lv_font_SourceHanSansSC_Normal_10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_keep_label_focustime, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_keep_label_focustime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_keep_label_focustime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_keep_label_focustime, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_keep_label_focustime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_keep_label_focustime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_keep_label_focustime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_keep_label_focustime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_keep_label_focustime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_keep_label_focustime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_keep.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_keep);

}
