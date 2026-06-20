#include "bsp_rx8130.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <sys/time.h>
#include <time.h>

static const char *TAG = "bsp_rx8130";

#define RX8130_REG_SEC       0x10
#define RX8130_REG_MIN       0x11
#define RX8130_REG_HOUR      0x12
#define RX8130_REG_WEEK      0x13
#define RX8130_REG_DAY       0x14
#define RX8130_REG_MONTH     0x15
#define RX8130_REG_YEAR      0x16
#define RX8130_REG_FLAG      0x1D
#define RX8130_REG_CTRL0     0x1E
#define RX8130_REG_CTRL1     0x1F

#define RX8130_CTRL0_STOP    (1U << 6)
#define RX8130_CTRL0_TEST    (1U << 7)
#define RX8130_FLAG_VLF      (1U << 1)
#define RX8130_FLAG_VBFF     (1U << 0)
#define RX8130_CTRL1_INIEN   (1U << 4)
#define RX8130_CTRL1_CHGEN   (1U << 5)

static uint8_t bcd_to_dec(uint8_t value)
{
    return (uint8_t)(((value >> 4) * 10) + (value & 0x0F));
}

static uint8_t dec_to_bcd(uint8_t value)
{
    return (uint8_t)(((value / 10) << 4) + (value % 10));
}

static uint8_t weekday_to_reg(uint8_t weekday)
{
    if (weekday > 6) {
        weekday = 0;
    }
    return (uint8_t)(1U << weekday);
}

static uint8_t reg_to_weekday(uint8_t reg)
{
    for (uint8_t i = 0; i < 7; i++) {
        if (reg & (1U << i)) {
            return i;
        }
    }
    return 0;
}

esp_err_t bsp_rx8130_i2c_init(const bsp_rx8130_i2c_config_t *config)
{
    if (config == NULL || config->sda_gpio < 0 || config->scl_gpio < 0 ||
        config->clk_speed_hz == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = config->sda_gpio,
        .scl_io_num = config->scl_gpio,
        .sda_pullup_en = config->enable_internal_pullups ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE,
        .scl_pullup_en = config->enable_internal_pullups ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE,
        .master.clk_speed = config->clk_speed_hz,
    };

    esp_err_t err = i2c_param_config(config->port, &conf);
    if (err != ESP_OK) {
        return err;
    }

    err = i2c_driver_install(config->port, I2C_MODE_MASTER, 0, 0, 0);
    return (err == ESP_ERR_INVALID_STATE) ? ESP_OK : err;
}

esp_err_t bsp_rx8130_read_reg(i2c_port_t port, uint8_t reg, uint8_t *value)
{
    if (value == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    return i2c_master_write_read_device(
        port, BSP_RX8130_I2C_ADDR, &reg, 1, value, 1, pdMS_TO_TICKS(200));
}

esp_err_t bsp_rx8130_write_reg(i2c_port_t port, uint8_t reg, uint8_t value)
{
    uint8_t buf[2] = {reg, value};
    return i2c_master_write_to_device(
        port, BSP_RX8130_I2C_ADDR, buf, sizeof(buf), pdMS_TO_TICKS(200));
}

esp_err_t bsp_rx8130_probe(i2c_port_t port)
{
    uint8_t sec = 0;
    return bsp_rx8130_read_reg(port, RX8130_REG_SEC, &sec);
}

static esp_err_t ensure_running(i2c_port_t port)
{
    uint8_t ctrl0 = 0;
    esp_err_t err = bsp_rx8130_read_reg(port, RX8130_REG_CTRL0, &ctrl0);
    if (err != ESP_OK) {
        return err;
    }
    if (ctrl0 & RX8130_CTRL0_STOP) {
        ctrl0 &= (uint8_t)~RX8130_CTRL0_STOP;
        err = bsp_rx8130_write_reg(port, RX8130_REG_CTRL0, ctrl0);
        if (err == ESP_OK) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
    return err;
}

esp_err_t bsp_rx8130_startup(i2c_port_t port, bool *lost_power)
{
    vTaskDelay(pdMS_TO_TICKS(30));

    uint8_t flag = 0;
    uint8_t ctrl0 = 0;
    uint8_t ctrl1 = 0;
    esp_err_t err = bsp_rx8130_read_reg(port, RX8130_REG_FLAG, &flag);
    if (err != ESP_OK) {
        return err;
    }
    err = bsp_rx8130_read_reg(port, RX8130_REG_CTRL0, &ctrl0);
    if (err != ESP_OK) {
        return err;
    }
    err = bsp_rx8130_read_reg(port, RX8130_REG_CTRL1, &ctrl1);
    if (err != ESP_OK) {
        return err;
    }

    bool vlf = (flag & RX8130_FLAG_VLF) != 0;
    bool stop = (ctrl0 & RX8130_CTRL0_STOP) != 0;
    bool test = (ctrl0 & RX8130_CTRL0_TEST) != 0;
    if (lost_power) {
        *lost_power = vlf;
    }

    if (vlf) {
        ESP_LOGW(TAG, "VLF=1; RTC time may be invalid");
    }
    if (flag & RX8130_FLAG_VBFF) {
        ESP_LOGW(TAG, "VBFF=1; backup battery may be low");
    }

    if (vlf || stop || test || (flag & 0xFC)) {
        err = bsp_rx8130_write_reg(port, RX8130_REG_FLAG, 0x00);
        if (err != ESP_OK) {
            return err;
        }
        err = bsp_rx8130_write_reg(port, RX8130_REG_CTRL0, 0x00);
        if (err != ESP_OK) {
            return err;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    err = ensure_running(port);
    if (err != ESP_OK) {
        return err;
    }

    ctrl1 |= RX8130_CTRL1_INIEN;
    ctrl1 &= (uint8_t)~RX8130_CTRL1_CHGEN;
    return bsp_rx8130_write_reg(port, RX8130_REG_CTRL1, ctrl1);
}

esp_err_t bsp_rx8130_get_time(i2c_port_t port, bsp_rx8130_time_t *time)
{
    if (time == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t reg = RX8130_REG_SEC;
    uint8_t data[7] = {0};
    esp_err_t err = i2c_master_write_read_device(
        port, BSP_RX8130_I2C_ADDR, &reg, 1, data, sizeof(data), pdMS_TO_TICKS(1000));
    if (err != ESP_OK) {
        return err;
    }

    time->second = bcd_to_dec(data[0] & 0x7F);
    time->minute = bcd_to_dec(data[1] & 0x7F);
    time->hour = bcd_to_dec(data[2] & 0x3F);
    time->weekday = reg_to_weekday(data[3] & 0x7F);
    time->day = bcd_to_dec(data[4] & 0x3F);
    time->month = bcd_to_dec(data[5] & 0x1F);
    time->year = (uint16_t)(2000 + bcd_to_dec(data[6]));
    return ESP_OK;
}

esp_err_t bsp_rx8130_set_time(i2c_port_t port, const bsp_rx8130_time_t *time)
{
    if (time == NULL || time->second > 59 || time->minute > 59 || time->hour > 23 ||
        time->weekday > 6 || time->day < 1 || time->day > 31 ||
        time->month < 1 || time->month > 12 || time->year < 2000) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t ctrl0 = 0;
    esp_err_t err = bsp_rx8130_read_reg(port, RX8130_REG_CTRL0, &ctrl0);
    if (err != ESP_OK) {
        return err;
    }

    err = bsp_rx8130_write_reg(port, RX8130_REG_CTRL0, (uint8_t)(ctrl0 | RX8130_CTRL0_STOP));
    if (err != ESP_OK) {
        return err;
    }

    uint8_t data[8] = {
        RX8130_REG_SEC,
        dec_to_bcd(time->second),
        dec_to_bcd(time->minute),
        dec_to_bcd((uint8_t)(time->hour & 0x3F)),
        weekday_to_reg(time->weekday),
        dec_to_bcd(time->day),
        dec_to_bcd(time->month),
        dec_to_bcd((uint8_t)(time->year % 100)),
    };

    err = i2c_master_write_to_device(
        port, BSP_RX8130_I2C_ADDR, data, sizeof(data), pdMS_TO_TICKS(1000));
    if (err != ESP_OK) {
        return err;
    }

    err = bsp_rx8130_read_reg(port, RX8130_REG_CTRL0, &ctrl0);
    if (err != ESP_OK) {
        return err;
    }
    return bsp_rx8130_write_reg(port, RX8130_REG_CTRL0, (uint8_t)(ctrl0 & ~RX8130_CTRL0_STOP));
}

esp_err_t bsp_rx8130_sync_from_system_time(i2c_port_t port, int min_valid_year)
{
    time_t now = 0;
    struct tm timeinfo = {0};
    time(&now);
    localtime_r(&now, &timeinfo);

    int year = timeinfo.tm_year + 1900;
    if (year < min_valid_year) {
        return ESP_ERR_INVALID_STATE;
    }

    bsp_rx8130_time_t rtc_time = {
        .second = (uint8_t)timeinfo.tm_sec,
        .minute = (uint8_t)timeinfo.tm_min,
        .hour = (uint8_t)timeinfo.tm_hour,
        .weekday = (uint8_t)timeinfo.tm_wday,
        .day = (uint8_t)timeinfo.tm_mday,
        .month = (uint8_t)(timeinfo.tm_mon + 1),
        .year = (uint16_t)year,
    };
    return bsp_rx8130_set_time(port, &rtc_time);
}

