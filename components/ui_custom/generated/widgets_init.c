/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include "gui_guider.h"
#include "widgets_init.h"
#include <stdlib.h>
#include <string.h>


__attribute__((unused)) void kb_event_cb (lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *kb = lv_event_get_target(e);
    if(code == LV_EVENT_READY || code == LV_EVENT_CANCEL) {
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
}

__attribute__((unused)) void ta_event_cb (lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
#if LV_USE_KEYBOARD || LV_USE_ZH_KEYBOARD
    lv_obj_t *ta = lv_event_get_target(e);
#endif
    lv_obj_t *kb = lv_event_get_user_data(e);
    if (code == LV_EVENT_FOCUSED || code == LV_EVENT_CLICKED)
    {
#if LV_USE_ZH_KEYBOARD != 0
        lv_zh_keyboard_set_textarea(kb, ta);
#endif
#if LV_USE_KEYBOARD != 0
        lv_keyboard_set_textarea(kb, ta);
#endif
        lv_obj_move_foreground(kb);
        lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
    if (code == LV_EVENT_CANCEL || code == LV_EVENT_DEFOCUSED)
    {

#if LV_USE_ZH_KEYBOARD != 0
        lv_zh_keyboard_set_textarea(kb, ta);
#endif
#if LV_USE_KEYBOARD != 0
        lv_keyboard_set_textarea(kb, ta);
#endif
        lv_obj_move_background(kb);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
}

#if LV_USE_ANALOGCLOCK != 0
void clock_count(int *hour, int *min, int *sec)
{
    (*sec)++;
    if(*sec == 60)
    {
        *sec = 0;
        (*min)++;
    }
    if(*min == 60)
    {
        *min = 0;
        if(*hour < 12)
        {
            (*hour)++;
        } else {
            (*hour)++;
            *hour = *hour %12;
        }
    }
}
#endif


const lv_img_dsc_t * screen_reppo_reppo_gif_imgs[24] = {
    &screen_reppo_reppo_gifAAA_reppo_start_00,
    &screen_reppo_reppo_gifAAA_reppo_start_01,
    &screen_reppo_reppo_gifAAA_reppo_start_02,
    &screen_reppo_reppo_gifAAA_reppo_start_03,
    &screen_reppo_reppo_gifAAA_reppo_start_04,
    &screen_reppo_reppo_gifAAA_reppo_start_05,
    &screen_reppo_reppo_gifAAA_reppo_start_06,
    &screen_reppo_reppo_gifAAA_reppo_start_07,
    &screen_reppo_reppo_gifAAA_reppo_start_08,
    &screen_reppo_reppo_gifAAA_reppo_start_09,
    &screen_reppo_reppo_gifAAA_reppo_start_10,
    &screen_reppo_reppo_gifAAA_reppo_start_11,
    &screen_reppo_reppo_gifAAA_reppo_start_12,
    &screen_reppo_reppo_gifAAA_reppo_start_13,
    &screen_reppo_reppo_gifAAA_reppo_start_14,
    &screen_reppo_reppo_gifAAA_reppo_start_15,
    &screen_reppo_reppo_gifAAA_reppo_start_16,
    &screen_reppo_reppo_gifAAA_reppo_start_17,
    &screen_reppo_reppo_gifAAA_reppo_start_18,
    &screen_reppo_reppo_gifAAA_reppo_start_19,
    &screen_reppo_reppo_gifAAA_reppo_start_20,
    &screen_reppo_reppo_gifAAA_reppo_start_21,
    &screen_reppo_reppo_gifAAA_reppo_start_22,
    &screen_reppo_reppo_gifAAA_reppo_start_23,
};
const lv_img_dsc_t * screen_main_cat_left_imgs[30] = {
    &screen_main_cat_leftAAA_yuexin_cat_left_00,
    &screen_main_cat_leftAAA_yuexin_cat_left_01,
    &screen_main_cat_leftAAA_yuexin_cat_left_02,
    &screen_main_cat_leftAAA_yuexin_cat_left_03,
    &screen_main_cat_leftAAA_yuexin_cat_left_04,
    &screen_main_cat_leftAAA_yuexin_cat_left_05,
    &screen_main_cat_leftAAA_yuexin_cat_left_06,
    &screen_main_cat_leftAAA_yuexin_cat_left_07,
    &screen_main_cat_leftAAA_yuexin_cat_left_08,
    &screen_main_cat_leftAAA_yuexin_cat_left_09,
    &screen_main_cat_leftAAA_yuexin_cat_left_10,
    &screen_main_cat_leftAAA_yuexin_cat_left_11,
    &screen_main_cat_leftAAA_yuexin_cat_left_12,
    &screen_main_cat_leftAAA_yuexin_cat_left_13,
    &screen_main_cat_leftAAA_yuexin_cat_left_14,
    &screen_main_cat_leftAAA_yuexin_cat_left_15,
    &screen_main_cat_leftAAA_yuexin_cat_left_16,
    &screen_main_cat_leftAAA_yuexin_cat_left_17,
    &screen_main_cat_leftAAA_yuexin_cat_left_18,
    &screen_main_cat_leftAAA_yuexin_cat_left_19,
    &screen_main_cat_leftAAA_yuexin_cat_left_20,
    &screen_main_cat_leftAAA_yuexin_cat_left_21,
    &screen_main_cat_leftAAA_yuexin_cat_left_22,
    &screen_main_cat_leftAAA_yuexin_cat_left_23,
    &screen_main_cat_leftAAA_yuexin_cat_left_24,
    &screen_main_cat_leftAAA_yuexin_cat_left_25,
    &screen_main_cat_leftAAA_yuexin_cat_left_26,
    &screen_main_cat_leftAAA_yuexin_cat_left_27,
    &screen_main_cat_leftAAA_yuexin_cat_left_28,
    &screen_main_cat_leftAAA_yuexin_cat_left_29,
};
const lv_img_dsc_t * screen_main_cat_right_imgs[30] = {
    &screen_main_cat_rightAAA_yuexin_cat_right_00,
    &screen_main_cat_rightAAA_yuexin_cat_right_01,
    &screen_main_cat_rightAAA_yuexin_cat_right_02,
    &screen_main_cat_rightAAA_yuexin_cat_right_03,
    &screen_main_cat_rightAAA_yuexin_cat_right_04,
    &screen_main_cat_rightAAA_yuexin_cat_right_05,
    &screen_main_cat_rightAAA_yuexin_cat_right_06,
    &screen_main_cat_rightAAA_yuexin_cat_right_07,
    &screen_main_cat_rightAAA_yuexin_cat_right_08,
    &screen_main_cat_rightAAA_yuexin_cat_right_09,
    &screen_main_cat_rightAAA_yuexin_cat_right_10,
    &screen_main_cat_rightAAA_yuexin_cat_right_11,
    &screen_main_cat_rightAAA_yuexin_cat_right_12,
    &screen_main_cat_rightAAA_yuexin_cat_right_13,
    &screen_main_cat_rightAAA_yuexin_cat_right_14,
    &screen_main_cat_rightAAA_yuexin_cat_right_15,
    &screen_main_cat_rightAAA_yuexin_cat_right_16,
    &screen_main_cat_rightAAA_yuexin_cat_right_17,
    &screen_main_cat_rightAAA_yuexin_cat_right_18,
    &screen_main_cat_rightAAA_yuexin_cat_right_19,
    &screen_main_cat_rightAAA_yuexin_cat_right_20,
    &screen_main_cat_rightAAA_yuexin_cat_right_21,
    &screen_main_cat_rightAAA_yuexin_cat_right_22,
    &screen_main_cat_rightAAA_yuexin_cat_right_23,
    &screen_main_cat_rightAAA_yuexin_cat_right_24,
    &screen_main_cat_rightAAA_yuexin_cat_right_25,
    &screen_main_cat_rightAAA_yuexin_cat_right_26,
    &screen_main_cat_rightAAA_yuexin_cat_right_27,
    &screen_main_cat_rightAAA_yuexin_cat_right_28,
    &screen_main_cat_rightAAA_yuexin_cat_right_29,
};
const lv_img_dsc_t * screen_wordcard_dog_cuo_imgs[6] = {
    &screen_wordcard_dog_cuoAAA_dog_shake_0,
    &screen_wordcard_dog_cuoAAA_dog_shake_1,
    &screen_wordcard_dog_cuoAAA_dog_shake_2,
    &screen_wordcard_dog_cuoAAA_dog_shake_3,
    &screen_wordcard_dog_cuoAAA_dog_shake_4,
    &screen_wordcard_dog_cuoAAA_dog_shake_5,
};
const lv_img_dsc_t * screen_wordcard_dog_dui_imgs[3] = {
    &screen_wordcard_dog_duiAAA_dog_love_0,
    &screen_wordcard_dog_duiAAA_dog_love_1,
    &screen_wordcard_dog_duiAAA_dog_love_2,
};
const lv_img_dsc_t * screen_reviewcard_dog_dance_imgs[3] = {
    &screen_reviewcard_dog_danceAAA_dog_dance_0,
    &screen_reviewcard_dog_danceAAA_dog_dance_1,
    &screen_reviewcard_dog_danceAAA_dog_dance_2,
};
const lv_img_dsc_t * screen_reviewcard_dog_dui_imgs[3] = {
    &screen_reviewcard_dog_duiAAA_dog_love_2_0,
    &screen_reviewcard_dog_duiAAA_dog_love_2_1,
    &screen_reviewcard_dog_duiAAA_dog_love_2_2,
};
