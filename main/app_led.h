/**
 * @file app_led.h
 * @brief RGB 流水灯：番茄钟超时、答案之书 loading
 */
#ifndef APP_LED_H
#define APP_LED_H

#include <stdbool.h>
#include <stdint.h>

/** 创建番茄钟超时蜂鸣+流水灯任务（由主循环在 flag 置位时调用） */
void app_led_start_buzzer_timeout_task(void);

/** 答案之书 loading 页：启动/停止流水灯（与旋转动画同周期） */
void app_led_answer_loading_start(void);
void app_led_answer_loading_stop(void);

#endif /* APP_LED_H */
