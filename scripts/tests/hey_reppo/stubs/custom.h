#pragma once
#include "gui_guider.h"
extern lv_ui guider_ui;
extern bool countdown_start_label,stopwatch_start_label,countdown_timeout_pending,focus_en,focus_session_active;
extern int current_task_index;
extern int16_t timer_all_num;
bool focus_timer_is_running(void);
void focus_session_cancel(void);
void idle_sleep_note_activity(void);
void idle_sleep_request_wake(void);
lv_obj_t *screen_setting_list_add_btn(const char *);
