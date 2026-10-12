#pragma once
#include "inspiration_storage.h"
/* All entry points are owned by the main LVGL task. */
void inspiration_ui_open_hub(void);
void inspiration_ui_preview(const inspiration_record_t *record);
void inspiration_ui_poll(void);
bool inspiration_ui_is_open(void);
void inspiration_ui_mark_guider_deleted(void *screen);
