/**
 * @file app_globals.c
 * @brief 应用级全局变量定义
 */
#include "app_globals.h"

const char *week_days[7] = {
    "Sunday",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
};

lv_ui guider_ui;
SemaphoreHandle_t lvgl_mutex = NULL;
lv_timer_t *bat_timer = NULL;
bool init_ntp = false;
bool buzzer_timeout_task_running = false;
bool ai_request_pending = false;
TickType_t ai_request_deadline = 0;
