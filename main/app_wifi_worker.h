/**
 * @file app_wifi_worker.h
 * @brief WiFi 扫描、连接、NTP/RTC 时间同步后台任务
 */
#ifndef APP_WIFI_WORKER_H
#define APP_WIFI_WORKER_H

/** FreeRTOS 任务入口：实时钟、WiFi 扫描/连接/断开 */
void app_wifi_worker_task(void *arg);

#endif /* APP_WIFI_WORKER_H */
