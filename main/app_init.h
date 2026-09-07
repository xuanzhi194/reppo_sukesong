/**
 * @file app_init.h
 * @brief 应用分层初始化（硬件 → 存储 → LVGL → 后台任务）
 */
#ifndef APP_INIT_H
#define APP_INIT_H

/** 外设：RGB、编码器、蜂鸣器、电池 ADC、RTC、WiFi 栈前置 */
void app_init_hardware(void);

/** NVS、凭据、SPIFFS、待办缓存 */
void app_init_storage_and_data(void);

/** LVGL、GUI Guider、定时器、互斥锁 */
void app_init_lvgl_ui(void);

/** 创建 NVS 与 WiFi 后台任务 */
void app_init_start_workers(void);

#endif /* APP_INIT_H */
