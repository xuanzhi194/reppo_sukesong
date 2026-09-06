/**
 * @file app_led.c
 * @brief RGB 流水灯与蜂鸣器联动任务
 */
#include "app_led.h"
#include "app_globals.h"
#include "buzzer.h"
#include "ws2812b.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define POMODORO_ALARM_LIGHT_MS  3000

static bool s_timeout_alarm_light_running = false;
static bool s_answer_loading_light_running = false;
static TaskHandle_t s_answer_loading_light_task_handle = NULL;

static void timeout_alarm_light_task(void *param)
{
    (void)param;
    while (s_timeout_alarm_light_running) {
        set_flow_light(30, 52, 83);
    }
    vTaskDelete(NULL);
}

static void buzzer_timeout_task(void *param)
{
    (void)param;
    TickType_t light_end = xTaskGetTickCount() + pdMS_TO_TICKS(POMODORO_ALARM_LIGHT_MS);

    s_timeout_alarm_light_running = true;
    xTaskCreatePinnedToCore(timeout_alarm_light_task, "timeout_alarm_light_task",
                            2048, NULL, 7, NULL, 1);

    /* 蜂鸣与流水灯同时进行；静音时蜂鸣可立即返回，流水灯仍保持 POMODORO_ALARM_LIGHT_MS */
    buzzer_timeout();
    while (xTaskGetTickCount() < light_end) {
        vTaskDelay(pdMS_TO_TICKS(20));
    }

    s_timeout_alarm_light_running = false;
    buzzer_timeout_task_running = false;
    vTaskDelete(NULL);
}

static void answer_loading_light_task(void *param)
{
    (void)param;
    while (s_answer_loading_light_running) {
        set_flow_light_fast(40, 40, 40);
    }
    s_answer_loading_light_task_handle = NULL;
    vTaskDelete(NULL);
}

void app_led_answer_loading_start(void)
{
    if (s_answer_loading_light_running || s_answer_loading_light_task_handle != NULL) {
        return;
    }
    s_answer_loading_light_running = true;
    xTaskCreatePinnedToCore(answer_loading_light_task, "answer_load_light",
                            2048, NULL, 7, &s_answer_loading_light_task_handle, 0);
}

void app_led_answer_loading_stop(void)
{
    s_answer_loading_light_running = false;
}

void app_led_start_buzzer_timeout_task(void)
{
    buzzer_timeout_task_running = true;
    xTaskCreatePinnedToCore(buzzer_timeout_task, "buzzer_timeout_task",
                            2048, NULL, 8, NULL, 0);
}
