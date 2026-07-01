#include "badge_storage.h"

#include "esp_crc.h"
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_vfs_fat.h"
#include "wear_levelling.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static const char *TAG = "badge_storage";
static const char *NVS_NS = "badge";

static wl_handle_t s_wl_handle = WL_INVALID_HANDLE;
static bool s_mounted = false;
static bool s_has_image = false;

static bool s_rx_active = false;
static uint32_t s_rx_total = 0;
static uint32_t s_rx_crc_expected = 0;
static uint32_t s_rx_received_max = 0;
static FILE *s_rx_fp = NULL;

static esp_err_t badge_nvs_load_meta(void)
{
    nvs_handle_t h;
    esp_err_t err = nvs_open(NVS_NS, NVS_READONLY, &h);
    if (err != ESP_OK) {
        s_has_image = false;
        return err;
    }

    uint8_t valid = 0;
    err = nvs_get_u8(h, "valid", &valid);
    if (err == ESP_OK && valid != 0) {
        struct stat st;
        if (stat(BADGE_IMG_PATH, &st) == 0 && st.st_size > 0) {
            s_has_image = true;
        } else {
            s_has_image = false;
        }
    } else {
        s_has_image = false;
    }

    nvs_close(h);
    return ESP_OK;
}

static esp_err_t badge_nvs_save_meta(uint32_t size, uint32_t crc32)
{
    nvs_handle_t h;
    esp_err_t err = nvs_open(NVS_NS, NVS_READWRITE, &h);
    if (err != ESP_OK) {
        return err;
    }

    err = nvs_set_u8(h, "valid", 1);
    if (err == ESP_OK) {
        err = nvs_set_u32(h, "size", size);
    }
    if (err == ESP_OK) {
        err = nvs_set_u32(h, "crc32", crc32);
    }
    if (err == ESP_OK) {
        err = nvs_commit(h);
    }
    nvs_close(h);
    return err;
}

static esp_err_t badge_nvs_clear_meta(void)
{
    nvs_handle_t h;
    esp_err_t err = nvs_open(NVS_NS, NVS_READWRITE, &h);
    if (err != ESP_OK) {
        return err;
    }
    nvs_erase_key(h, "valid");
    nvs_erase_key(h, "size");
    nvs_erase_key(h, "crc32");
    err = nvs_commit(h);
    nvs_close(h);
    return err;
}

static uint32_t badge_file_crc32(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        return 0;
    }

    uint32_t crc = 0;
    uint8_t buf[1024];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), f)) > 0) {
        crc = esp_crc32_le(crc, buf, n);
    }
    fclose(f);
    return crc;
}

static void badge_rx_close_file(void)
{
    if (s_rx_fp != NULL) {
        fclose(s_rx_fp);
        s_rx_fp = NULL;
    }
}

esp_err_t badge_storage_init(void)
{
    if (s_mounted) {
        return ESP_OK;
    }

    const esp_vfs_fat_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 4,
        .allocation_unit_size = CONFIG_WL_SECTOR_SIZE,
    };

    esp_err_t err = esp_vfs_fat_spiflash_mount_rw_wl(
        BADGE_MOUNT_POINT, "badge", &mount_config, &s_wl_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "FAT mount failed: %s", esp_err_to_name(err));
        return err;
    }

    s_mounted = true;
    badge_nvs_load_meta();
    ESP_LOGI(TAG, "mounted %s  has_image=%d", BADGE_MOUNT_POINT, (int)s_has_image);
    return ESP_OK;
}

bool badge_storage_is_mounted(void)
{
    return s_mounted;
}

bool badge_storage_has_image(void)
{
    return s_has_image;
}

bool badge_storage_rx_in_progress(void)
{
    return s_rx_active;
}

void badge_storage_rx_abort(void)
{
    badge_rx_close_file();
    if (s_rx_active) {
        unlink(BADGE_TMP_PATH);
    }
    s_rx_active = false;
    s_rx_total = 0;
    s_rx_crc_expected = 0;
    s_rx_received_max = 0;
    ESP_LOGW(TAG, "RX aborted");
}

esp_err_t badge_storage_rx_begin(uint32_t total_size, uint32_t crc32, uint8_t format)
{
    if (!s_mounted) {
        return ESP_ERR_INVALID_STATE;
    }
    if (format != BADGE_FMT_JPEG) {
        ESP_LOGW(TAG, "unsupported format %u", (unsigned)format);
        return ESP_ERR_NOT_SUPPORTED;
    }
    if (total_size == 0 || total_size > BADGE_MAX_IMAGE_BYTES) {
        ESP_LOGW(TAG, "invalid size %lu", (unsigned long)total_size);
        return ESP_ERR_INVALID_SIZE;
    }

    badge_storage_rx_abort();

    s_rx_fp = fopen(BADGE_TMP_PATH, "wb");
    if (s_rx_fp == NULL) {
        ESP_LOGE(TAG, "open tmp failed");
        return ESP_FAIL;
    }

    s_rx_active = true;
    s_rx_total = total_size;
    s_rx_crc_expected = crc32;
    s_rx_received_max = 0;
    ESP_LOGI(TAG, "RX begin size=%lu crc=0x%08lX", (unsigned long)total_size,
             (unsigned long)crc32);
    return ESP_OK;
}

esp_err_t badge_storage_rx_write(uint32_t offset, const uint8_t *data, size_t len)
{
    if (!s_rx_active || s_rx_fp == NULL || data == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    if (len == 0) {
        return ESP_OK;
    }
    if (offset + len > s_rx_total) {
        ESP_LOGW(TAG, "write overflow off=%lu len=%u total=%lu",
                 (unsigned long)offset, (unsigned)len, (unsigned long)s_rx_total);
        return ESP_ERR_INVALID_SIZE;
    }

    if (fseek(s_rx_fp, (long)offset, SEEK_SET) != 0) {
        ESP_LOGE(TAG, "fseek failed off=%lu", (unsigned long)offset);
        return ESP_FAIL;
    }
    if (fwrite(data, 1, len, s_rx_fp) != len) {
        ESP_LOGE(TAG, "fwrite failed off=%lu len=%u", (unsigned long)offset, (unsigned)len);
        return ESP_FAIL;
    }

    uint32_t end = offset + (uint32_t)len;
    if (end > s_rx_received_max) {
        s_rx_received_max = end;
    }
    return ESP_OK;
}

esp_err_t badge_storage_rx_commit(void)
{
    if (!s_rx_active || s_rx_fp == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    fflush(s_rx_fp);
    badge_rx_close_file();

    struct stat st;
    if (stat(BADGE_TMP_PATH, &st) != 0 || (uint32_t)st.st_size != s_rx_total) {
        ESP_LOGE(TAG, "commit size mismatch got=%ld expect=%lu",
                 (long)st.st_size, (unsigned long)s_rx_total);
        unlink(BADGE_TMP_PATH);
        s_rx_active = false;
        return ESP_ERR_INVALID_SIZE;
    }

    if (s_rx_received_max < s_rx_total) {
        ESP_LOGE(TAG, "commit incomplete received=%lu total=%lu",
                 (unsigned long)s_rx_received_max, (unsigned long)s_rx_total);
        unlink(BADGE_TMP_PATH);
        s_rx_active = false;
        return ESP_ERR_INVALID_SIZE;
    }

    uint32_t crc = badge_file_crc32(BADGE_TMP_PATH);
    if (crc != s_rx_crc_expected) {
        ESP_LOGE(TAG, "CRC mismatch got=0x%08lX expect=0x%08lX",
                 (unsigned long)crc, (unsigned long)s_rx_crc_expected);
        unlink(BADGE_TMP_PATH);
        s_rx_active = false;
        return ESP_ERR_INVALID_CRC;
    }

    unlink(BADGE_IMG_PATH);
    if (rename(BADGE_TMP_PATH, BADGE_IMG_PATH) != 0) {
        ESP_LOGE(TAG, "rename tmp->jpg failed");
        unlink(BADGE_TMP_PATH);
        s_rx_active = false;
        return ESP_FAIL;
    }

    esp_err_t err = badge_nvs_save_meta(s_rx_total, s_rx_crc_expected);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "NVS save failed: %s", esp_err_to_name(err));
    }

    s_has_image = true;
    s_rx_active = false;
    s_rx_total = 0;
    s_rx_crc_expected = 0;
    s_rx_received_max = 0;
    ESP_LOGI(TAG, "RX commit OK (%lu bytes)", (unsigned long)st.st_size);
    return ESP_OK;
}
