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



void setup_scr_screen_reppo(lv_ui *ui)
{
    //Write codes screen_reppo
    ui->screen_reppo = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_reppo, 320, 170);
    lv_obj_set_scrollbar_mode(ui->screen_reppo, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_reppo, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_reppo, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_reppo, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_reppo, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_reppo_reppo_gif
    ui->screen_reppo_reppo_gif = lv_animimg_create(ui->screen_reppo);
    lv_animimg_set_src(ui->screen_reppo_reppo_gif, (const void **) screen_reppo_reppo_gif_imgs, 24, false);
    lv_animimg_set_duration(ui->screen_reppo_reppo_gif, 50*24);
    lv_animimg_set_repeat_count(ui->screen_reppo_reppo_gif, 1);
    lv_animimg_start(ui->screen_reppo_reppo_gif);
    lv_obj_set_pos(ui->screen_reppo_reppo_gif, 0, 0);
    lv_obj_set_size(ui->screen_reppo_reppo_gif, 320, 170);

    //The custom code of screen_reppo.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_reppo);

    //Init events for screen.
    events_init_screen_reppo(ui);
}
