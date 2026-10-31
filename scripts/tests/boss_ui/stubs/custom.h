#pragma once
#include "gui_guider.h"
extern lv_group_t *g;
void idle_sleep_note_activity(void);
bool focus_timer_is_running(void);
lv_obj_t *screen_setting_list_add_btn(const char *text);
