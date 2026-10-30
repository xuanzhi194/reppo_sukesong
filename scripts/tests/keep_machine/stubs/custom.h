#pragma once
#include "gui_guider.h"
extern lv_group_t *g;
extern int all_focus_time;
void group_focus_obj(lv_obj_t *obj);
void idle_sleep_note_activity(void);
bool idle_sleep_is_asleep(void);
