/**
 * @file app_ui.h
 * @brief LVGL 屏级刷新、定时器回调（电量 / 时钟 / timing 状态）
 */
#ifndef APP_UI_H
#define APP_UI_H

#include "lvgl.h"
#include <stdbool.h>

/** 更新 timing 屏 WiFi 图标与电量条（调用前建议已持有 lvgl_mutex） */
void app_ui_timing_status_update(bool wifi_connected);

/** 更新 timing 屏日期/时间/星期标签（调用前须已持有 lvgl_mutex） */
void app_ui_timing_labels_update(const char *date_str, const char *time_str, const char *day_str);

/** 主屏 arc / timing 屏 bar 电量定时刷新（LVGL timer 回调） */
void app_ui_battery_timer_cb(lv_timer_t *timer);

/** 主屏 RTC 时间标签定时刷新（LVGL timer 回调） */
void app_ui_realtime_clock_timer_cb(lv_timer_t *timer);
void app_ui_init_clock_label_static(void);

/** 从 SPIFFS 统计初始化单词卡计数（app_init 调用） */
void app_ui_init_word_counters(void);

/** 处理 wordcard_change 标志（需在 lvgl_mutex 内调用） */
void app_ui_wordcard_process_change(void);

/** 处理 reviewcard_change 标志（需在 lvgl_mutex 内调用） */
void app_ui_reviewcard_process_change(void);

/** timing / wificfg 屏是否仍为当前活动屏 */
bool app_ui_timing_screen_is_alive(void);
bool app_ui_wificfg_screen_is_alive(void);

#endif /* APP_UI_H */
