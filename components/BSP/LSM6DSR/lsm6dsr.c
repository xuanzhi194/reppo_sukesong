#include "lsm6dsr.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "LSM6DSR";
static i2c_port_t s_port = LSM6DSR_I2C_PORT;

#define LSM6DSR_I2C_TIMEOUT_MS  200
#define LSM6DSR_MG_PER_LSB_2G   61  /* 0.061 mg/LSB, used as milli-mg for integer math */

esp_err_t lsm6dsr_init(i2c_port_t port)
{
    s_port = port;
    return ESP_OK;
}

esp_err_t lsm6dsr_read_reg(uint8_t reg, uint8_t *val)
{
    if (val == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    return i2c_master_write_read_device(s_port, LSM6DSR_I2C_ADDR, &reg, 1, val, 1,
                                        pdMS_TO_TICKS(LSM6DSR_I2C_TIMEOUT_MS));
}

esp_err_t lsm6dsr_write_reg(uint8_t reg, uint8_t val)
{
    uint8_t buf[2] = {reg, val};
    return i2c_master_write_to_device(s_port, LSM6DSR_I2C_ADDR, buf, 2,
                                      pdMS_TO_TICKS(LSM6DSR_I2C_TIMEOUT_MS));
}

esp_err_t lsm6dsr_read_regs(uint8_t reg, uint8_t *buf, size_t len)
{
    if (buf == NULL || len == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    return i2c_master_write_read_device(s_port, LSM6DSR_I2C_ADDR, &reg, 1, buf, len,
                                        pdMS_TO_TICKS(LSM6DSR_I2C_TIMEOUT_MS));
}

esp_err_t lsm6dsr_who_am_i(uint8_t *id)
{
    return lsm6dsr_read_reg(LSM6DSR_REG_WHO_AM_I, id);
}

esp_err_t lsm6dsr_probe(void)
{
    uint8_t id = 0;
    esp_err_t err = lsm6dsr_who_am_i(&id);
    if (err != ESP_OK) {
        return err;
    }
    if (id != LSM6DSR_WHO_AM_I_VAL) {
        ESP_LOGW(TAG, "WHO_AM_I=0x%02X (expect 0x%02X)", id, LSM6DSR_WHO_AM_I_VAL);
        return ESP_ERR_NOT_FOUND;
    }
    return ESP_OK;
}

esp_err_t lsm6dsr_soft_reset(void)
{
    esp_err_t err = lsm6dsr_write_reg(LSM6DSR_REG_CTRL3_C, LSM6DSR_CTRL3_SW_RESET);
    if (err != ESP_OK) {
        return err;
    }

    for (int i = 0; i < 20; i++) {
        uint8_t ctrl3 = 0;
        vTaskDelay(pdMS_TO_TICKS(5));
        err = lsm6dsr_read_reg(LSM6DSR_REG_CTRL3_C, &ctrl3);
        if (err != ESP_OK) {
            return err;
        }
        if ((ctrl3 & LSM6DSR_CTRL3_SW_RESET) == 0) {
            return ESP_OK;
        }
    }
    ESP_LOGW(TAG, "soft reset bit still set");
    return ESP_ERR_TIMEOUT;
}

esp_err_t lsm6dsr_read_accel_raw(lsm6dsr_raw3_t *out)
{
    uint8_t buf[6];
    esp_err_t err;

    if (out == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    err = lsm6dsr_read_regs(LSM6DSR_REG_OUTX_L_A, buf, sizeof(buf));
    if (err != ESP_OK) {
        return err;
    }
    out->x = (int16_t)((uint16_t)buf[1] << 8 | buf[0]);
    out->y = (int16_t)((uint16_t)buf[3] << 8 | buf[2]);
    out->z = (int16_t)((uint16_t)buf[5] << 8 | buf[4]);
    return ESP_OK;
}

static int accel_raw_to_mg(int16_t raw)
{
    return (int)raw * LSM6DSR_MG_PER_LSB_2G / 1000;
}

esp_err_t lsm6dsr_solder_test(void)
{
    uint8_t id = 0;
    uint8_t status = 0;
    lsm6dsr_raw3_t a = {0, 0, 0};
    esp_err_t err = lsm6dsr_who_am_i(&id);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "solder test FAIL: no ACK at 0x%02X (%s)",
                 LSM6DSR_I2C_ADDR, esp_err_to_name(err));
        return err;
    }
    if (id != LSM6DSR_WHO_AM_I_VAL) {
        ESP_LOGE(TAG, "solder test FAIL: WHO_AM_I=0x%02X expect 0x%02X",
                 id, LSM6DSR_WHO_AM_I_VAL);
        return ESP_ERR_NOT_FOUND;
    }
    ESP_LOGI(TAG, "WHO_AM_I=0x%02X — I2C solder OK (INT1=GPIO%d INT2=GPIO%d)",
             id, LSM6DSR_INT1_GPIO, LSM6DSR_INT2_GPIO);

    err = lsm6dsr_soft_reset();
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "soft reset failed (%s); ID was good", esp_err_to_name(err));
        return ESP_OK;
    }
    err = lsm6dsr_write_reg(LSM6DSR_REG_CTRL3_C, LSM6DSR_CTRL3_BDU | LSM6DSR_CTRL3_IF_INC);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "CTRL3 write failed (%s)", esp_err_to_name(err));
        return ESP_OK;
    }
    err = lsm6dsr_write_reg(LSM6DSR_REG_CTRL2_G, 0x00);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "CTRL2 write failed (%s)", esp_err_to_name(err));
        return ESP_OK;
    }
    err = lsm6dsr_write_reg(LSM6DSR_REG_CTRL1_XL, LSM6DSR_CTRL1_XL_12HZ5_2G);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "CTRL1_XL write failed (%s)", esp_err_to_name(err));
        return ESP_OK;
    }

    /* 12.5 Hz → first sample in ~80 ms */
    vTaskDelay(pdMS_TO_TICKS(120));
    (void)lsm6dsr_read_reg(LSM6DSR_REG_STATUS, &status);
    err = lsm6dsr_read_accel_raw(&a);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "accel read failed (%s); WHO_AM_I was OK", esp_err_to_name(err));
        return ESP_OK;
    }

    ESP_LOGI(TAG, "accel XLDA=%u  raw x=%d y=%d z=%d  (~%d %d %d mg)",
             (unsigned)((status & LSM6DSR_STATUS_XLDA) != 0),
             (int)a.x, (int)a.y, (int)a.z,
             accel_raw_to_mg(a.x), accel_raw_to_mg(a.y), accel_raw_to_mg(a.z));
    if ((status & LSM6DSR_STATUS_XLDA) == 0 && a.x == 0 && a.y == 0 && a.z == 0) {
        ESP_LOGW(TAG, "accel still zero — I2C is up, MEMS may need a longer wait");
    }
    return ESP_OK;
}
