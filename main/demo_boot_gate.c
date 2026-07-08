/**
 * @file demo_boot_gate.c
 * @brief 宣传视频：BLE 0x30 或 ENTER 键触发正常开机
 */
#include "demo_boot_gate.h"
#include "bsp_ble.h"
#include "reppo_demo_mode.h"
#include "keypad.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "demo_gate";

#if REPPO_DEMO_BLE_BOOT_GATE
static bool demo_enter_pressed(void)
{
    if (!keypad_get_key_value(REPPO_DEMO_BOOT_ENTER_GPIO)) {
        return false;
    }
    vTaskDelay(pdMS_TO_TICKS(35));
    return keypad_get_key_value(REPPO_DEMO_BOOT_ENTER_GPIO);
}

void demo_boot_gate_wait_ble_or_button(void)
{
    keypad_io_cfg(REPPO_DEMO_BOOT_ENTER_GPIO);
    ESP_LOGI(TAG, "Press ENTER or send BLE 0x30 to start normal boot");

    while (!bsp_ble_demo_boot_gate_is_released()) {
        if (demo_enter_pressed()) {
            ESP_LOGI(TAG, "ENTER pressed — release normal boot");
            bsp_ble_demo_boot_gate_release();
            while (keypad_get_key_value(REPPO_DEMO_BOOT_ENTER_GPIO)) {
                vTaskDelay(pdMS_TO_TICKS(20));
            }
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(30));
    }
}
#endif
