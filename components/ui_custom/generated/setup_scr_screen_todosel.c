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



void setup_scr_screen_todosel(lv_ui *ui)
{
    //Write codes screen_todosel
    ui->screen_todosel = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_todosel, 320, 170);
    lv_obj_set_scrollbar_mode(ui->screen_todosel, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_todosel, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_todosel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_todosel, &_AAA_todosel_bg_320x170, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_todosel, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_todosel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_todosel_btn_complete
    ui->screen_todosel_btn_complete = lv_btn_create(ui->screen_todosel);
    ui->screen_todosel_btn_complete_label = lv_label_create(ui->screen_todosel_btn_complete);
    lv_label_set_text(ui->screen_todosel_btn_complete_label, "");
    lv_label_set_long_mode(ui->screen_todosel_btn_complete_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_todosel_btn_complete_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_todosel_btn_complete, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_todosel_btn_complete_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_todosel_btn_complete, 18, 85);
    lv_obj_set_size(ui->screen_todosel_btn_complete, 137, 28);

    //Write style for screen_todosel_btn_complete, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_todosel_btn_complete, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_todosel_btn_complete, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_todosel_btn_complete, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_todosel_btn_complete, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_todosel_btn_complete, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_todosel_btn_complete, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_todosel_btn_complete, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_todosel_btn_complete, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_todosel_btn_complete, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_todosel_btn_complete, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_todosel_btn_complete, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_todosel_btn_complete, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_todosel_btn_complete, 10, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_todosel_btn_complete, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_todosel_btn_complete, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_todosel_btn_complete, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_todosel_btn_complete, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_todosel_btn_complete, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_todosel_btn_complete, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_todosel_btn_complete, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_todosel_btn_complete, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_todosel_btn_focus
    ui->screen_todosel_btn_focus = lv_btn_create(ui->screen_todosel);
    ui->screen_todosel_btn_focus_label = lv_label_create(ui->screen_todosel_btn_focus);
    lv_label_set_text(ui->screen_todosel_btn_focus_label, "");
    lv_label_set_long_mode(ui->screen_todosel_btn_focus_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_todosel_btn_focus_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_todosel_btn_focus, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_todosel_btn_focus_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_todosel_btn_focus, 162, 86);
    lv_obj_set_size(ui->screen_todosel_btn_focus, 138, 26);

    //Write style for screen_todosel_btn_focus, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_todosel_btn_focus, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_todosel_btn_focus, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_todosel_btn_focus, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_todosel_btn_focus, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_todosel_btn_focus, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_todosel_btn_focus, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_todosel_btn_focus, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_todosel_btn_focus, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_todosel_btn_focus, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_todosel_btn_focus, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_todosel_btn_focus, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_todosel_btn_focus, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_todosel_btn_focus, 10, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_todosel_btn_focus, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_todosel_btn_focus, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_todosel_btn_focus, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_todosel_btn_focus, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_todosel_btn_focus, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_todosel_btn_focus, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_todosel_btn_focus, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_todosel_btn_focus, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_todosel_btn_cancel
    ui->screen_todosel_btn_cancel = lv_btn_create(ui->screen_todosel);
    ui->screen_todosel_btn_cancel_label = lv_label_create(ui->screen_todosel_btn_cancel);
    lv_label_set_text(ui->screen_todosel_btn_cancel_label, "");
    lv_label_set_long_mode(ui->screen_todosel_btn_cancel_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_todosel_btn_cancel_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_todosel_btn_cancel, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_todosel_btn_cancel_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_todosel_btn_cancel, 137, 128);
    lv_obj_set_size(ui->screen_todosel_btn_cancel, 43, 15);

    //Write style for screen_todosel_btn_cancel, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_todosel_btn_cancel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_todosel_btn_cancel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_todosel_btn_cancel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_todosel_btn_cancel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_todosel_btn_cancel, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_todosel_btn_cancel, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_todosel_btn_cancel, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_todosel_btn_cancel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_todosel_btn_cancel, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
    lv_obj_set_style_bg_opa(ui->screen_todosel_btn_cancel, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->screen_todosel_btn_cancel, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_radius(ui->screen_todosel_btn_cancel, 5, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_width(ui->screen_todosel_btn_cancel, 10, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(ui->screen_todosel_btn_cancel, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(ui->screen_todosel_btn_cancel, 255, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_spread(ui->screen_todosel_btn_cancel, 1, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_x(ui->screen_todosel_btn_cancel, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_ofs_y(ui->screen_todosel_btn_cancel, 0, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(ui->screen_todosel_btn_cancel, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(ui->screen_todosel_btn_cancel, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(ui->screen_todosel_btn_cancel, 255, LV_PART_MAIN|LV_STATE_FOCUSED);

    //Write codes screen_todosel_label_task
    ui->screen_todosel_label_task = lv_label_create(ui->screen_todosel);
    lv_label_set_text(ui->screen_todosel_label_task, "Reading for one hour");
    lv_label_set_long_mode(ui->screen_todosel_label_task, LV_LABEL_LONG_SCROLL);
    lv_obj_set_pos(ui->screen_todosel_label_task, 84, 59);
    lv_obj_set_size(ui->screen_todosel_label_task, 150, 16);

    //Write style for screen_todosel_label_task, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_todosel_label_task, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_todosel_label_task, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_todosel_label_task, lv_color_hex(0x274a6e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_todosel_label_task, &lv_font_SourceHanSansSC_Medium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_todosel_label_task, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_todosel_label_task, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_todosel_label_task, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_todosel_label_task, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_todosel_label_task, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_todosel_label_task, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_todosel_label_task, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_todosel_label_task, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_todosel_label_task, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_todosel_label_task, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_todosel.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_todosel);

    //Init events for screen.
    events_init_screen_todosel(ui);
}
