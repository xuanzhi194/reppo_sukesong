#ifndef RX8130_TEST_I2C_H
#define RX8130_TEST_I2C_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef int esp_err_t;
typedef int i2c_port_t;
typedef uint32_t TickType_t;
#define ESP_OK 0
#define ESP_ERR_INVALID_STATE 0x103
#define I2C_NUM_0 0
#define I2C_MODE_MASTER 1
#define GPIO_PULLUP_ENABLE 1

typedef struct {
    int mode;
    int sda_io_num;
    int scl_io_num;
    int sda_pullup_en;
    int scl_pullup_en;
    struct { unsigned clk_speed; } master;
} i2c_config_t;

esp_err_t i2c_param_config(i2c_port_t port, const i2c_config_t *config);
esp_err_t i2c_driver_install(i2c_port_t port, int mode, size_t rx, size_t tx, int flags);
esp_err_t i2c_master_write_to_device(i2c_port_t port, uint8_t addr,
                                    const uint8_t *data, size_t len, TickType_t timeout);
esp_err_t i2c_master_write_read_device(i2c_port_t port, uint8_t addr,
                                      const uint8_t *write_data, size_t write_len,
                                      uint8_t *read_data, size_t read_len, TickType_t timeout);
static inline const char *esp_err_to_name(esp_err_t err) { (void)err; return "test error"; }
#endif
