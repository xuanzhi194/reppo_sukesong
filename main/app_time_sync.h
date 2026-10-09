/**
 * @file app_time_sync.h
 * @brief WiFi 连上后用 SNTP 取北京时间，写入 RX8130 供后续走时
 */
#ifndef APP_TIME_SYNC_H
#define APP_TIME_SYNC_H

/** 主循环调用（不持有 LVGL 锁）。 */
void app_time_sync_poll(void);

#endif
