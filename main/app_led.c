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

static bool s_timeout_alarm_light_running = false;
static bool s_ai_wait_light_running = false;
static TaskHandle_t s_ai_wait_light_task_handle = NULL;

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
    s_timeout_alarm_light_running = true;
    xTaskCreatePinnedToCore(timeout_alarm_light_task, "timeout_alarm_light_task",
                            2048, NULL, 7, NULL, 1);
    buzzer_timeout();
    s_timeout_alarm_light_running = false;
    buzzer_timeout_task_running = false;
    vTaskDelete(NULL);
}

static void ai_wait_light_task(void *param)
{
    (void)param;
    while (s_ai_wait_light_running) {
        set_flow_light(30, 52, 83);
    }
    s_ai_wait_light_task_handle = NULL;
    vTaskDelete(NULL);
}

void app_led_ai_wait_start(void)
{
    if (s_ai_wait_light_running || s_ai_wait_light_task_handle != NULL) {
        return;
    }
    s_ai_wait_light_running = true;
    xTaskCreatePinnedToCore(ai_wait_light_task, "ai_wait_light_task",
                            2048, NULL, 7, &s_ai_wait_light_task_handle, 0);
}

void app_led_ai_wait_stop(void)
{
    s_ai_wait_light_running = false;
    for (int i = 0; i < 100 && s_ai_wait_light_task_handle != NULL; i++) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void app_led_ai_result_show(bool success, uint8_t retry_count)
{
    if (success) {
        for (int i = 0; i < retry_count; i++) {
            set_flow_light(34, 80, 32);
        }
    } else {
        for (int i = 0; i < retry_count; i++) {
            set_flow_light(80, 32, 33);
        }
    }
    set_flow_light(0, 0, 0);
}

void app_led_start_buzzer_timeout_task(void)
{
    buzzer_timeout_task_running = true;
    xTaskCreatePinnedToCore(buzzer_timeout_task, "buzzer_timeout_task",
                            2048, NULL, 8, NULL, 0);
}
