#ifndef BSP_BLE_H
#define BSP_BLE_H

#include <stdbool.h>

void bsp_ble_init(void);
void bsp_ble_deinit(void);
bool bsp_ble_is_active(void);
bool bsp_ble_is_connected(void);
void bsp_ble_disconnect(void);

#endif /* BSP_BLE_H */