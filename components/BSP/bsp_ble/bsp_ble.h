#ifndef BSP_BLE_H
#define BSP_BLE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"
#include "reppo_demo_mode.h"

/** 每次连接后，App 开启 notify，再下发本地墙钟（不做时区换算）。
 * payload: year(u16 LE, 2000..2099)+month+date+hour+min+sec[+wday]。
 * 一次只发送一条，收到 TIME_STATUS OK 后再开始 Todo 等同步。 */
#define BLE_CMD_TIME_SET      0x10
/** Notify：payload[0] = BLE_TIME_STATUS_* */
#define BLE_CMD_TIME_STATUS   0x11
#define BLE_TIME_STATUS_OK    0
#define BLE_TIME_STATUS_ERR   1

/** Todo list over the same GATT char.
 * Write JSON array with 0x02 (single packet) or 0x03/0x04/0x05 (start/chunk/end).
 * Pull with 0x06; device notifies {"todo":[...],"taskDone":[...]}.
 *
 * Each todo object:
 *   content      string  required
 *   durationStr  string  e.g. "25MIN" (leading integer is minutes)
 *   done         bool
 *   plannedDate  "YYYY-MM-DD" optional
 *   label        optional category, same four buckets as the claw machine:
 *                "work" | "study" | "life" | "fun"
 *                (aliases: 工作/学习/生活/娱乐, or number 1..4)
 *                omit / "" / null = unlabeled
 * Also accepted: "category"/"tag" as the same string, or
 * "categoryIndex" 0..3 matching gash Work/Study/Life/Fun.
 */
#define BLE_CMD_TODO_JSON_ONE    0x02
#define BLE_CMD_TODO_JSON_START  0x03
#define BLE_CMD_TODO_JSON_CHUNK  0x04
#define BLE_CMD_TODO_JSON_END    0x05
#define BLE_CMD_TODO_PULL        0x06

/** Ebook UTF-8 text upload (same GATT char 0xFF01 as todo/badge).
 * INFO  payload = total_size(u32 LE) + crc32(u32 LE) + format(u8=1)
 * DATA  payload = offset(u32 LE) + chunk  (write packet <= 256: chunk <= 251)
 * COMMIT payload = empty
 * STATUS notify payload = status(u8) + progress(u8)
 * CRC = ESP-IDF esp_crc32_le / zlib.crc32 IEEE. */
#define BLE_CMD_BOOK_INFO     0x50
#define BLE_CMD_BOOK_DATA     0x51
#define BLE_CMD_BOOK_COMMIT   0x52
#define BLE_CMD_BOOK_STATUS   0x53
#define BLE_BOOK_FMT_TXT      1
#define BLE_BOOK_STATUS_OK    0
#define BLE_BOOK_STATUS_ERR   1
#define BLE_BOOK_STATUS_BUSY  2

typedef struct {
    esp_err_t (*begin)(uint32_t total_size, uint32_t crc32, uint8_t format);
    esp_err_t (*write)(uint32_t offset, const uint8_t *data, size_t len);
    esp_err_t (*commit)(void);
    void (*abort)(void);
} bsp_ble_book_rx_t;

void bsp_ble_register_book_rx(const bsp_ble_book_rx_t *rx);

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
