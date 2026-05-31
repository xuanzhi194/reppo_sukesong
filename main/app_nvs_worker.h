/**
 * @file app_nvs_worker.h
 * @brief 后台 NVS / Todo / BLE / 单词卡 异步任务
 */
#ifndef APP_NVS_WORKER_H
#define APP_NVS_WORKER_H

/** FreeRTOS 任务入口：处理 NVS 写入、归档、BLE、单词卡 UI 刷新 */
void app_nvs_worker_task(void *param);

#endif /* APP_NVS_WORKER_H */
