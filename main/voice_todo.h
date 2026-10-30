/**
 * @file voice_todo.h
 * @brief 敲击进入的语音待办、灵感入口。
 */
#ifndef VOICE_TODO_H
#define VOICE_TODO_H

#include <stdbool.h>
#include <stddef.h>

/** IMU 任务在识别到一次大于 3g 的敲击时调用。页面在 LVGL 线程里打开。 */
void voice_todo_request_enter(void);
/** UI 主任务选择模式后调用；下一次 poll 创建录音页。 */
void voice_todo_start_mode(bool inspiration);
/** 语音页开着，或正从语音页去连 WiFi。 */
bool voice_todo_is_open(void);
/** 在已持有 lvgl_mutex 时调用。 */
void voice_todo_poll(void);

/** Background-only text completion using the existing Qwen credentials/model. */
bool voice_ai_complete(const char *system, const char *data, char *out, size_t capacity);

#endif /* VOICE_TODO_H */
