/**
 * @file app_led.h
 * @brief RGB 流水灯：番茄钟超时、AI 等待/结果指示
 */
#ifndef APP_LED_H
#define APP_LED_H

#include <stdbool.h>
#include <stdint.h>

/** 启动 AI 分析等待流水灯（蓝色） */
void app_led_ai_wait_start(void);

/** 停止 AI 等待流水灯 */
void app_led_ai_wait_stop(void);

/**
 * @brief 显示 AI / 操作结果灯效
 * @param success     true=绿色，false=红色
 * @param retry_count 闪烁次数
 */
void app_led_ai_result_show(bool success, uint8_t retry_count);

/** AI 总结冷却中：黄色流水灯（与红色 WiFi 失败、绿色成功区分） */
void app_led_ai_cooldown_show(uint8_t retry_count);

/** 创建番茄钟超时蜂鸣+流水灯任务（由主循环在 flag 置位时调用） */
void app_led_start_buzzer_timeout_task(void);

#endif /* APP_LED_H */
