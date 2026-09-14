#ifndef LSM6DSR_H
#define LSM6DSR_H

#include <stddef.h>
#include <stdint.h>
#include "driver/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Schematic v3.6: shared I2C with RX8130. SA0/SDO = GND → 0x6A. */
#define LSM6DSR_I2C_ADDR        0x6A
#define LSM6DSR_WHO_AM_I_VAL    0x6B

#define LSM6DSR_I2C_PORT        I2C_NUM_0
#define LSM6DSR_I2C_SDA_PIN     15
#define LSM6DSR_I2C_SCL_PIN     16
#define LSM6DSR_INT1_GPIO       36
#define LSM6DSR_INT2_GPIO       35

#define LSM6DSR_REG_WHO_AM_I    0x0F
#define LSM6DSR_REG_CTRL1_XL    0x10
#define LSM6DSR_REG_CTRL2_G     0x11
#define LSM6DSR_REG_CTRL3_C     0x12
#define LSM6DSR_REG_STATUS      0x1E
#define LSM6DSR_REG_OUTX_L_A    0x28

#define LSM6DSR_CTRL3_SW_RESET  (1U << 0)
#define LSM6DSR_CTRL3_IF_INC    (1U << 2)
#define LSM6DSR_CTRL3_BDU       (1U << 6)

#define LSM6DSR_STATUS_XLDA     (1U << 0)

/* CTRL1_XL: ODR 12.5 Hz, ±2 g */
#define LSM6DSR_CTRL1_XL_12HZ5_2G  0x10

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} lsm6dsr_raw3_t;

/** Remember the I2C port. Does not install the bus; call after rx8130_init(). */
esp_err_t lsm6dsr_init(i2c_port_t port);

esp_err_t lsm6dsr_read_reg(uint8_t reg, uint8_t *val);
esp_err_t lsm6dsr_write_reg(uint8_t reg, uint8_t val);
esp_err_t lsm6dsr_read_regs(uint8_t reg, uint8_t *buf, size_t len);

/** Read WHO_AM_I. ESP_OK even if the value is wrong; check *id. */
esp_err_t lsm6dsr_who_am_i(uint8_t *id);

/** I2C ACK and WHO_AM_I == 0x6B. */
esp_err_t lsm6dsr_probe(void);

esp_err_t lsm6dsr_soft_reset(void);
esp_err_t lsm6dsr_read_accel_raw(lsm6dsr_raw3_t *out);

/**
 * Bring-up solder check: probe WHO_AM_I, enable the accelerometer,
 * log one sample. ESP_OK only if the chip ID matches.
 */
esp_err_t lsm6dsr_solder_test(void);

#ifdef __cplusplus
}
#endif

#endif
