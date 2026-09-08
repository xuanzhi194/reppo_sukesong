#pragma once
#include "FreeRTOS.h"

typedef void *TaskHandle_t;
TickType_t xTaskGetTickCount(void);
void vTaskDelay(TickType_t ticks);
void vTaskDelete(TaskHandle_t task);
BaseType_t xTaskCreatePinnedToCore(void (*function)(void *), const char *name,
                                 unsigned stack_size, void *param,
                                 unsigned priority, TaskHandle_t *handle, int core);
