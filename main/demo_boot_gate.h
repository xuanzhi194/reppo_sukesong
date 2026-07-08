#ifndef DEMO_BOOT_GATE_H
#define DEMO_BOOT_GATE_H

#include "reppo_demo_mode.h"

#if REPPO_DEMO_BLE_BOOT_GATE
void demo_boot_gate_wait_ble_or_button(void);
#endif

#endif /* DEMO_BOOT_GATE_H */
