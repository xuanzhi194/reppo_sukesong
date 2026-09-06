/**
 * @file app_wifi_worker.h
 * @brief WiFi 扫描、连接，以及 timing 屏从 RTC 刷新时间
 */
#ifndef APP_WIFI_WORKER_H
#define APP_WIFI_WORKER_H

/** FreeRTOS 任务入口：实时钟、WiFi 扫描/连接/断开 */
void app_wifi_worker_task(void *arg);

/** 注册 WiFi 断连回调（在 init_wifi 之后调用） */
void app_wifi_register_callbacks(void);

#endif /* APP_WIFI_WORKER_H */
