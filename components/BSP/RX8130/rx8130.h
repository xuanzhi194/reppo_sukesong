#ifndef RX8130_H
#define RX8130_H

#include "driver/i2c.h"

#define RX8130_ADDR             0x32

#define RX8130_REG_SEC          0x10
#define RX8130_REG_MIN          0x11
#define RX8130_REG_HOUR         0x12
#define RX8130_REG_WEEK         0x13
#define RX8130_REG_DAY          0x14
#define RX8130_REG_MONTH        0x15
#define RX8130_REG_YEAR         0x16

#define RX8130_REG_FLAG         0x1D
#define RX8130_REG_CTRL0        0x1E
#define RX8130_REG_CTRL1        0x1F

#define RX8130_CTRL0_STOP       (1 << 6)
#define RX8130_CTRL0_TEST       (1 << 7)

#define RX8130_FLAG_VLF         (1 << 1)
#define RX8130_FLAG_VBFF        (1 << 0)

#define RX8130_CTRL1_INIEN      (1 << 4)  /* VDD 掉电时切换到 VBAT */
#define RX8130_CTRL1_CHGEN      (1 << 5)  /* 可充电电池; 纽扣电池保持 0 */

#define RX8130_I2C_SDA_PIN      15
#define RX8130_I2C_SCL_PIN      16
#define RX8130_I2C_PORT         I2C_NUM_0

typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;    /* 0-6: Sunday-Saturday */
    uint8_t date;
    uint8_t month;
    uint16_t year;  /* full year, e.g. 2026 */
} rx8130_time_t;

esp_err_t rx8130_init(i2c_port_t port, int sda_pin, int scl_pin);
esp_err_t rx8130_probe(i2c_port_t port);
/** 上电后调用: 等待晶振、清 VLF/STOP、恢复走时. lost_power 可选输出是否发生过掉电(VLF). */
esp_err_t rx8130_startup(i2c_port_t port, bool *lost_power);
/** 用 ESP32 系统时间 (NTP 已同步后) 写入 RX8130 */
esp_err_t rx8130_sync_from_system(i2c_port_t port);
esp_err_t rx8130_set_time(i2c_port_t port, rx8130_time_t *time);
esp_err_t rx8130_get_time(i2c_port_t port, rx8130_time_t *time);
esp_err_t rx8130_read_reg(i2c_port_t port, uint8_t reg, uint8_t *val);
esp_err_t rx8130_write_reg(i2c_port_t port, uint8_t reg, uint8_t val);
void rx8130_test_run(i2c_port_t port, int sda_pin, int scl_pin);

#endif
