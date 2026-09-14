#ifndef LSM6DSR_TEST_I2C_H
#define LSM6DSR_TEST_I2C_H
#include <stddef.h>
#include <stdint.h>

typedef int esp_err_t;
typedef int i2c_port_t;
typedef uint32_t TickType_t;
#define ESP_OK 0
#define ESP_FAIL -1
#define ESP_ERR_INVALID_ARG 0x102
#define ESP_ERR_INVALID_STATE 0x103
#define ESP_ERR_NOT_FOUND 0x105
#define ESP_ERR_TIMEOUT 0x107
#define I2C_NUM_0 0

esp_err_t i2c_master_write_to_device(i2c_port_t port, uint8_t addr,
                                    const uint8_t *data, size_t len, TickType_t timeout);
esp_err_t i2c_master_write_read_device(i2c_port_t port, uint8_t addr,
                                      const uint8_t *write_data, size_t write_len,
                                      uint8_t *read_data, size_t read_len, TickType_t timeout);
static inline const char *esp_err_to_name(esp_err_t err)
{
    (void)err;
    return "test";
}
#endif
