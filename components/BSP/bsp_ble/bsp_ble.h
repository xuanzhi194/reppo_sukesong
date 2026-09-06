#ifndef BSP_BLE_H
#define BSP_BLE_H

#include <stdbool.h>
#include "reppo_demo_mode.h"

/** 每次连接后，App 开启 notify，再下发本地墙钟（不做时区换算）。
 * payload: year(u16 LE, 2000..2099)+month+date+hour+min+sec[+wday]。
 * 一次只发送一条，收到 TIME_STATUS OK 后再开始 Todo 等同步。 */
#define BLE_CMD_TIME_SET      0x10
/** Notify：payload[0] = BLE_TIME_STATUS_* */
#define BLE_CMD_TIME_STATUS   0x11
#define BLE_TIME_STATUS_OK    0
#define BLE_TIME_STATUS_ERR   1

void bsp_ble_init(void);
void bsp_ble_deinit(void);
bool bsp_ble_is_active(void);
bool bsp_ble_is_connected(void);
void bsp_ble_disconnect(void);
/** 将排队的手机时间写入 RX8130，成功并恢复走时后返回 true。
 * 须在主循环、非 BLE 回调里调用；结果由 BLE host 发出 notify。 */
bool bsp_ble_time_poll(void);

#if REPPO_DEMO_BLE_BOOT_GATE
/** 阻塞直到收到 BLE 0x30 或 demo_boot_gate_wait 检测到按键 */
void bsp_ble_demo_boot_gate_wait(void);
void bsp_ble_demo_boot_gate_release(void);
bool bsp_ble_demo_boot_gate_is_released(void);
#endif

#endif /* BSP_BLE_H */
