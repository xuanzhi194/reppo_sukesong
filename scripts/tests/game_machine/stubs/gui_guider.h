#pragma once
#include "lvgl.h"
typedef struct {
    lv_obj_t *screen_main;
    bool screen_main_del;
    lv_obj_t *screen_game;
    bool screen_game_del;
} lv_ui;
extern lv_ui guider_ui;
typedef void (*ui_setup_scr_t)(lv_ui *);
void setup_scr_screen_main(lv_ui *ui);
void ui_load_scr_animation(lv_ui *, lv_obj_t **, bool, bool *, ui_setup_scr_t,
                           lv_scr_load_anim_t, uint32_t, uint32_t, bool, bool);
