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



void setup_scr_screen_loading(lv_ui *ui)
{
    //Write codes screen_loading
    ui->screen_loading = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_loading, 320, 170);
    lv_obj_set_scrollbar_mode(ui->screen_loading, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_loading, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_loading, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_loading, &_AAA_answer_wait_bg_320x170, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_loading, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_loading, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_loading_img_icon
    ui->screen_loading_img_icon = lv_img_create(ui->screen_loading);
    lv_obj_add_flag(ui->screen_loading_img_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_loading_img_icon, &_AAA_answer_icon_bg_alpha_58x58);
    lv_img_set_pivot(ui->screen_loading_img_icon, 50,50);
    lv_img_set_angle(ui->screen_loading_img_icon, 0);
    lv_obj_set_pos(ui->screen_loading_img_icon, 128, 48);
    lv_obj_set_size(ui->screen_loading_img_icon, 58, 58);

    //Write style for screen_loading_img_icon, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->screen_loading_img_icon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_loading_img_icon, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_loading_img_icon, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_loading_img_icon, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_loading.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_loading);

    //Init events for screen.
    events_init_screen_loading(ui);
}
