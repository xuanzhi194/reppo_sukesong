/**
 * @file app_main_loop.h
 * @brief app_main 主循环：事件轮询 + LVGL 刷新
 */
#ifndef APP_MAIN_LOOP_H
#define APP_MAIN_LOOP_H

/** 处理一轮异步事件（答案之书、超时、AI、BLE、donelist 等） */
void app_main_loop_once(void);

/** 在 lvgl_mutex 下执行 lv_task_handler */
void app_main_loop_lvgl_tick(void);

#endif /* APP_MAIN_LOOP_H */
