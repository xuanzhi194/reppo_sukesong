#ifndef __BUZZER_H_
#define __BUZZER_H_

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/ledc.h"

// 定义配置参数
#define BUZZER_IO           (7)               // 使用 IO7
#define BUZZER_LEDC_TIMER   LEDC_TIMER_0
#define BUZZER_LEDC_MODE    LEDC_LOW_SPEED_MODE
#define BUZZER_LEDC_CHANNEL LEDC_CHANNEL_0
#define BUZZER_DUTY_RES     LEDC_TIMER_13_BIT // 13位分辨率
#define BUZZER_INITIAL_FREQ (2000)            // 初始频率 2kHz

/**
 * 初始化 PWM 配置
 */
void buzzer_init();
void buzzer_bind_mute_check(bool (*fn)(void));
void buzzer_set_sound(uint32_t freq, uint8_t volume_percent);
void buzzer_beep(void);
void buzzer_beep_confirm(void);
void buzzer_timeout();

/** 让正在播放的 buzzer_timeout() 在下一次 20 ms 节拍退出并静音。下一次超时任务开始前要先 clear。 */
void buzzer_alarm_abort(void);
void buzzer_alarm_abort_clear(void);
bool buzzer_alarm_is_aborted(void);
#endif