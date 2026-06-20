/**
 * @file app_globals.h
 * @brief 应用级全局状态（跨 main 子模块与 UI 层共享）
 */
#ifndef APP_GLOBALS_H
#define APP_GLOBALS_H

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "gui_guider.h"
#include "lvgl.h"

/** 星期名称，供 timing 屏显示 */
extern const char *week_days[7];

/** GUI Guider 生成的 UI 对象树（定义在 app_globals.c） */
extern lv_ui guider_ui;

/** 保护所有 LVGL API 调用的互斥锁 */
extern SemaphoreHandle_t lvgl_mutex;

/** 主屏 / timing 屏电量定时刷新 */
extern lv_timer_t *bat_timer;

/** SNTP 是否已初始化（AI 任务会按需触发） */
extern bool init_ntp;

/** 番茄钟超时蜂鸣任务是否正在运行 */
extern bool buzzer_timeout_task_running;

/** AI 分析请求是否进行中（用于流水灯与超时判定） */
extern volatile bool ai_request_pending;
extern TickType_t ai_request_deadline;

#endif /* APP_GLOBALS_H */
