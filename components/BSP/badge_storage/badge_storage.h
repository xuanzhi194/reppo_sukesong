#ifndef BADGE_STORAGE_H
#define BADGE_STORAGE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"

/** VFS mount point (badge FAT partition) */
#define BADGE_MOUNT_POINT       "/badge"
/** Persisted JPEG path */
#define BADGE_IMG_PATH          BADGE_MOUNT_POINT "/badge.jpg"
/** Staging file while BLE transfer is in progress */
#define BADGE_TMP_PATH          BADGE_MOUNT_POINT "/badge.tmp"
/** LVGL SJPG / POSIX FS path (letter B -> /badge); 路径需带 / 前缀 */
#define BADGE_LVGL_PATH         "B:/badge.jpg"

/** Max single image size (bytes); badge partition is 1 MB */
#define BADGE_MAX_IMAGE_BYTES   (768 * 1024)

/** BLE command bytes (WeChat mini program -> device) */
#define BLE_CMD_IMG_INFO        0x20
#define BLE_CMD_IMG_DATA        0x21
#define BLE_CMD_IMG_COMMIT      0x22
/** Device -> mini program status notify */
#define BLE_CMD_IMG_STATUS      0x23

/** BLE_CMD_IMG_STATUS payload[1] values */
#define BADGE_STATUS_OK         0
#define BADGE_STATUS_ERR        1
#define BADGE_STATUS_BUSY       2

/** Image format byte in 0x20 INFO */
#define BADGE_FMT_JPEG          1

esp_err_t badge_storage_init(void);
bool badge_storage_is_mounted(void);
bool badge_storage_has_image(void);
bool badge_storage_rx_in_progress(void);

esp_err_t badge_storage_rx_begin(uint32_t total_size, uint32_t crc32, uint8_t format);
esp_err_t badge_storage_rx_write(uint32_t offset, const uint8_t *data, size_t len);
esp_err_t badge_storage_rx_commit(void);
void badge_storage_rx_abort(void);

#endif /* BADGE_STORAGE_H */
