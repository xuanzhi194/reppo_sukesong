#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "driver/i2c.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BSP_RX8130_I2C_ADDR        0x32

typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t weekday;    // 0-6, Sunday-Saturday
    uint8_t day;
    uint8_t month;
    uint16_t year;      // Full year, for example 2026
} bsp_rx8130_time_t;

typedef struct {
    i2c_port_t port;
    int sda_gpio;
    int scl_gpio;
    uint32_t clk_speed_hz;
    bool enable_internal_pullups;
} bsp_rx8130_i2c_config_t;

#define BSP_RX8130_I2C_DEFAULT_CONFIG() { \
    .port = I2C_NUM_0, \
    .sda_gpio = 15, \
    .scl_gpio = 16, \
    .clk_speed_hz = 100000, \
    .enable_internal_pullups = true, \
}

esp_err_t bsp_rx8130_i2c_init(const bsp_rx8130_i2c_config_t *config);
esp_err_t bsp_rx8130_probe(i2c_port_t port);
esp_err_t bsp_rx8130_startup(i2c_port_t port, bool *lost_power);
esp_err_t bsp_rx8130_get_time(i2c_port_t port, bsp_rx8130_time_t *time);
esp_err_t bsp_rx8130_set_time(i2c_port_t port, const bsp_rx8130_time_t *time);
esp_err_t bsp_rx8130_sync_from_system_time(i2c_port_t port, int min_valid_year);
esp_err_t bsp_rx8130_read_reg(i2c_port_t port, uint8_t reg, uint8_t *value);
esp_err_t bsp_rx8130_write_reg(i2c_port_t port, uint8_t reg, uint8_t value);

#ifdef __cplusplus
}
#endif

