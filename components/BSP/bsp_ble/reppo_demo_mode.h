#ifndef REPPO_DEMO_MODE_H
#define REPPO_DEMO_MODE_H

#include "driver/gpio.h"
/**
 * 宣传视频临时模式开关
 * 1 = 上电先 BLE 待命，电脑发 BLE_CMD_DEMO_BOOT_START(0x30) 后才正常开机
 * 0 = 正式版（上电即正常流程）
 */
#define REPPO_DEMO_BLE_BOOT_GATE  0

#define BLE_CMD_DEMO_BOOT_START   0x30

/** 宣传待命时按 ENTER（确认键）也可触发开机，与 lv_port 一致 */
#define REPPO_DEMO_BOOT_ENTER_GPIO  GPIO_NUM_5

#endif /* REPPO_DEMO_MODE_H */
