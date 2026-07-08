#ifndef BSP_BLE_H
#define BSP_BLE_H

#include <stdbool.h>
#include "reppo_demo_mode.h"

void bsp_ble_init(void);
void bsp_ble_deinit(void);
bool bsp_ble_is_active(void);
bool bsp_ble_is_connected(void);
void bsp_ble_disconnect(void);

#if REPPO_DEMO_BLE_BOOT_GATE
/** 阻塞直到收到 BLE 0x30 或 demo_boot_gate_wait 检测到按键 */
void bsp_ble_demo_boot_gate_wait(void);
void bsp_ble_demo_boot_gate_release(void);
bool bsp_ble_demo_boot_gate_is_released(void);
#endif

#endif /* BSP_BLE_H */