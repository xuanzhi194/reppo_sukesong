/**
 * @file voice_todo.h
 * @brief 两下晃动进入的语音待办页。
 */
#ifndef VOICE_TODO_H
#define VOICE_TODO_H

#include <stdbool.h>

/** IMU 任务在识别到两下晃动时调用。页面在 LVGL 线程里打开。 */
void voice_todo_request_enter(void);
/** 语音页开着，或正从语音页去连 WiFi。 */
bool voice_todo_is_open(void);
/** 在已持有 lvgl_mutex 时调用。 */
void voice_todo_poll(void);

#endif /* VOICE_TODO_H */
