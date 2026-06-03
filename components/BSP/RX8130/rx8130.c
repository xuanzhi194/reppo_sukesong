#include "rx8130.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

static const char *TAG = "rx8130";

static const char *s_week_names[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat",
};

static esp_err_t rx8130_ensure_running(i2c_port_t port);
static void rx8130_print_time(const rx8130_time_t *t);

static uint8_t bcd2dec(uint8_t val) {
    return ((val >> 4) * 10) + (val & 0x0F);
}

static uint8_t dec2bcd(uint8_t val) {
    return ((val / 10) << 4) + (val % 10);
}

/* RX8130 WEEK register is one-hot bitmap: bit0..bit6 => Sunday..Saturday */
static uint8_t weekday_to_reg(uint8_t day_idx) {
    if (day_idx > 6) {
        day_idx = 0;
    }
    return (uint8_t)(1U << day_idx);
}

static uint8_t reg_to_weekday(uint8_t week_reg) {
    for (uint8_t i = 0; i < 7; i++) {
        if (week_reg & (1U << i)) {
            return i;
        }
    }
    return 0;
}

static void i2c_scan_bus(i2c_port_t port)
{
    ESP_LOGW(TAG, "I2C scan on port %d (0x08..0x77)", (int)port);
    int found = 0;
    for (uint8_t addr = 0x08; addr <= 0x77; addr++) {
        esp_err_t err = i2c_master_write_to_device(port, addr, NULL, 0, pdMS_TO_TICKS(30));
        if (err == ESP_OK) {
            ESP_LOGW(TAG, "  device at 0x%02X%s", addr,
                     (addr == RX8130_ADDR) ? "  <-- RX8130 expected" : "");
            found++;
        }
    }
    if (found == 0) {
        ESP_LOGE(TAG, "  no I2C device found (check SDA/SCL/power/pull-up)");
    }
    ESP_LOGW(TAG, "I2C scan done, %d device(s)", found);
}

esp_err_t rx8130_init(i2c_port_t port, int sda_pin, int scl_pin) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda_pin,
        .scl_io_num = scl_pin,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
    };

    esp_err_t err = i2c_param_config(port, &conf);
    if (err != ESP_OK) {
        return err;
    }

    err = i2c_driver_install(port, conf.mode, 0, 0, 0);
    if (err == ESP_ERR_INVALID_STATE) {
        return ESP_OK;
    }
    return err;
}

esp_err_t rx8130_read_reg(i2c_port_t port, uint8_t reg, uint8_t *val)
{
    return i2c_master_write_read_device(
        port, RX8130_ADDR, &reg, 1, val, 1, pdMS_TO_TICKS(200));
}

esp_err_t rx8130_write_reg(i2c_port_t port, uint8_t reg, uint8_t val)
{
    uint8_t buf[2] = {reg, val};
    return i2c_master_write_to_device(port, RX8130_ADDR, buf, 2, pdMS_TO_TICKS(200));
}

esp_err_t rx8130_probe(i2c_port_t port) {
    uint8_t sec = 0;
    return rx8130_read_reg(port, RX8130_REG_SEC, &sec);
}

esp_err_t rx8130_startup(i2c_port_t port, bool *lost_power)
{
    /* 手册: 上电后至少 30ms 再读 VLF */
    vTaskDelay(pdMS_TO_TICKS(30));

    uint8_t flag = 0;
    uint8_t ctrl0 = 0;
    uint8_t ctrl1 = 0;
    esp_err_t err = rx8130_read_reg(port, RX8130_REG_FLAG, &flag);
    if (err != ESP_OK) {
        return err;
    }
    err = rx8130_read_reg(port, RX8130_REG_CTRL0, &ctrl0);
    if (err != ESP_OK) {
        return err;
    }
    err = rx8130_read_reg(port, RX8130_REG_CTRL1, &ctrl1);
    if (err != ESP_OK) {
        return err;
    }
    const bool vlf = (flag & RX8130_FLAG_VLF) != 0;
    const bool stop = (ctrl0 & RX8130_CTRL0_STOP) != 0;
    const bool test = (ctrl0 & RX8130_CTRL0_TEST) != 0;

    if (lost_power) {
        *lost_power = vlf;
    }

    if (vlf) {
        ESP_LOGW(TAG,
                 "VLF=1: power-loss or first boot — calendar may be invalid, need NTP/WiFi sync");
    }
    if (flag & RX8130_FLAG_VBFF) {
        ESP_LOGW(TAG, "VBFF=1: backup battery voltage low — check VBAT/coin cell");
    }
    if (stop) {
        ESP_LOGW(TAG, "CTRL0.STOP=1: clock was halted");
    }
    if (test) {
        ESP_LOGW(TAG, "CTRL0.TEST=1: must be 0 for normal operation");
    }

    /* 手册: VLF=1 或标志异常时清 FLAG/CTRL0, 再确保 STOP=0 */
    if (vlf || stop || test || (flag & 0xFC)) {
        err = rx8130_write_reg(port, RX8130_REG_FLAG, 0x00);
        if (err != ESP_OK) {
            return err;
        }
        ctrl0 = 0x00;
        err = rx8130_write_reg(port, RX8130_REG_CTRL0, ctrl0);
        if (err != ESP_OK) {
            return err;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    err = rx8130_ensure_running(port);
    if (err != ESP_OK) {
        return err;
    }

    /* 手册 EX3: 纽扣电池备份需 INIEN=1, CHGEN=0 才能在 VDD 掉电后由 VBAT 供电 */
    ctrl1 |= RX8130_CTRL1_INIEN;
    ctrl1 &= (uint8_t)~RX8130_CTRL1_CHGEN;
    err = rx8130_write_reg(port, RX8130_REG_CTRL1, ctrl1);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "CTRL1 backup config write failed");
    } else {
        ESP_LOGI(TAG, "CTRL1: INIEN=1 CHGEN=0 (coin-cell backup switchover)");
    }

    rx8130_time_t boot_now = {0};
    if (rx8130_get_time(port, &boot_now) == ESP_OK) {
        ESP_LOGI(TAG, "RTC after startup:");
        rx8130_print_time(&boot_now);
    }
    return ESP_OK;
}

esp_err_t rx8130_sync_from_system(i2c_port_t port)
{
    time_t now = 0;
    struct tm timeinfo = {0};
    time(&now);
    localtime_r(&now, &timeinfo);

    if (timeinfo.tm_year + 1900 < 2026) {
        ESP_LOGW(TAG, "system time not valid yet, skip RTC sync");
        return ESP_ERR_INVALID_STATE;
    }

    rx8130_time_t set_time = {
        .second = (uint8_t)timeinfo.tm_sec,
        .minute = (uint8_t)timeinfo.tm_min,
        .hour   = (uint8_t)timeinfo.tm_hour,
        .day    = (uint8_t)timeinfo.tm_wday,
        .date   = (uint8_t)timeinfo.tm_mday,
        .month  = (uint8_t)(timeinfo.tm_mon + 1),
        .year   = (uint16_t)(timeinfo.tm_year + 1900),
    };

    esp_err_t err = rx8130_set_time(port, &set_time);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "RTC synced from system (NTP):");
        rx8130_print_time(&set_time);
    }
    return err;
}

static esp_err_t rx8130_dump_regs(i2c_port_t port)
{
    static const struct {
        uint8_t reg;
        const char *name;
    } regs[] = {
        {RX8130_REG_SEC,   "SEC  "},
        {RX8130_REG_MIN,   "MIN  "},
        {RX8130_REG_HOUR,  "HOUR "},
        {RX8130_REG_WEEK,  "WEEK "},
        {RX8130_REG_DAY,   "DAY  "},
        {RX8130_REG_MONTH, "MONTH"},
        {RX8130_REG_YEAR,  "YEAR "},
        {RX8130_REG_FLAG,  "FLAG "},
        {RX8130_REG_CTRL0, "CTRL0"},
        {RX8130_REG_CTRL1, "CTRL1"},
    };

    ESP_LOGI(TAG, "Register dump (addr=0x%02X):", RX8130_ADDR);
    for (int i = 0; i < (int)(sizeof(regs) / sizeof(regs[0])); i++) {
        uint8_t v = 0;
        esp_err_t err = rx8130_read_reg(port, regs[i].reg, &v);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "  0x%02X %s: read fail (%s)",
                     regs[i].reg, regs[i].name, esp_err_to_name(err));
            return err;
        }
        ESP_LOGI(TAG, "  0x%02X %s = 0x%02X", regs[i].reg, regs[i].name, v);
    }
    return ESP_OK;
}

static esp_err_t rx8130_ensure_running(i2c_port_t port)
{
    uint8_t ctrl0 = 0;
    esp_err_t err = rx8130_read_reg(port, RX8130_REG_CTRL0, &ctrl0);
    if (err != ESP_OK) {
        return err;
    }
    if (ctrl0 & RX8130_CTRL0_STOP) {
        ESP_LOGW(TAG, "CTRL0.STOP=1, clock halted -> clearing STOP bit");
        ctrl0 &= (uint8_t)~RX8130_CTRL0_STOP;
        err = rx8130_write_reg(port, RX8130_REG_CTRL0, ctrl0);
        if (err != ESP_OK) {
            return err;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    } else {
        ESP_LOGI(TAG, "CTRL0.STOP=0, clock should be running");
    }
    return ESP_OK;
}

static void rx8130_print_time(const rx8130_time_t *t)
{
    const char *wday = (t->day <= 6) ? s_week_names[t->day] : "?";
    ESP_LOGI(TAG, "  %04u-%02u-%02u %s %02u:%02u:%02u",
             (unsigned)t->year, (unsigned)t->month, (unsigned)t->date,
             wday, (unsigned)t->hour, (unsigned)t->minute, (unsigned)t->second);
}

esp_err_t rx8130_set_time(i2c_port_t port, rx8130_time_t *time) {
    uint8_t ctrl0 = 0;
    esp_err_t err = rx8130_read_reg(port, RX8130_REG_CTRL0, &ctrl0);
    if (err != ESP_OK) {
        return err;
    }
    /* datasheet: set STOP before changing calendar */
    err = rx8130_write_reg(port, RX8130_REG_CTRL0, (uint8_t)(ctrl0 | RX8130_CTRL0_STOP));
    if (err != ESP_OK) {
        return err;
    }

    uint8_t data[8];
    data[0] = RX8130_REG_SEC;
    data[1] = dec2bcd(time->second);
    data[2] = dec2bcd(time->minute);
    data[3] = dec2bcd((uint8_t)(time->hour & 0x3F));
    data[4] = weekday_to_reg(time->day);
    data[5] = dec2bcd(time->date);
    data[6] = dec2bcd(time->month);
    data[7] = dec2bcd((uint8_t)(time->year % 100));

    err = i2c_master_write_to_device(port, RX8130_ADDR, data, sizeof(data), pdMS_TO_TICKS(1000));
    if (err != ESP_OK) {
        return err;
    }

    /* clear STOP to resume ticking (must succeed or clock stays frozen) */
    err = rx8130_read_reg(port, RX8130_REG_CTRL0, &ctrl0);
    if (err != ESP_OK) {
        return err;
    }
    err = rx8130_write_reg(port, RX8130_REG_CTRL0, (uint8_t)(ctrl0 & ~RX8130_CTRL0_STOP));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "set_time: failed to clear STOP, clock may be halted");
    }
    return err;
}

esp_err_t rx8130_get_time(i2c_port_t port, rx8130_time_t *time) {
    uint8_t reg_addr = RX8130_REG_SEC;
    uint8_t data[7];

    esp_err_t err = i2c_master_write_read_device(
        port, RX8130_ADDR, &reg_addr, 1, data, sizeof(data), pdMS_TO_TICKS(1000));
    if (err != ESP_OK) {
        return err;
    }

    time->second = bcd2dec(data[0] & 0x7F);
    time->minute = bcd2dec(data[1] & 0x7F);
    time->hour   = bcd2dec(data[2] & 0x3F);
    time->day    = reg_to_weekday(data[3] & 0x7F);
    time->date   = bcd2dec(data[4] & 0x3F);
    time->month  = bcd2dec(data[5] & 0x1F);
    time->year   = (uint16_t)(2000 + bcd2dec(data[6]));

    return ESP_OK;
}

void rx8130_test_run(i2c_port_t port, int sda_pin, int scl_pin)
{
    ESP_LOGI(TAG, "======== RX8130 test (no LCD/LVGL) ========");
    ESP_LOGI(TAG, "I2C port=%d SDA=GPIO%d SCL=GPIO%d addr=0x%02X",
             (int)port, sda_pin, scl_pin, RX8130_ADDR);

    esp_err_t err = rx8130_init(port, sda_pin, scl_pin);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C init failed: %s", esp_err_to_name(err));
        return;
    }
    ESP_LOGI(TAG, "I2C init OK (100kHz)");

    err = rx8130_probe(port);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "RX8130 probe FAILED: %s", esp_err_to_name(err));
        ESP_LOGE(TAG, "Expected device at 0x%02X on GPIO%d/GPIO%d",
                 RX8130_ADDR, sda_pin, scl_pin);
        i2c_scan_bus(port);
        return;
    }
    ESP_LOGI(TAG, "RX8130 probe OK");

    if (rx8130_dump_regs(port) != ESP_OK) {
        return;
    }
    if (rx8130_ensure_running(port) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to clear STOP bit");
        return;
    }

    rx8130_time_t t0 = {0};
    if (rx8130_get_time(port, &t0) == ESP_OK) {
        ESP_LOGI(TAG, "Current RTC time:");
        rx8130_print_time(&t0);
    }

    /* 秒针走动测试: 连续读 5 次, 间隔 1s */
    ESP_LOGI(TAG, "--- Tick test (5 reads, 1s apart) ---");
    uint8_t prev_sec = 0xFF;
    int tick_ok = 0;
    for (int i = 0; i < 5; i++) {
        rx8130_time_t now = {0};
        err = rx8130_get_time(port, &now);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "[%d] read time failed: %s", i, esp_err_to_name(err));
        } else {
            ESP_LOGI(TAG, "[%d]", i);
            rx8130_print_time(&now);
            if (prev_sec != 0xFF) {
                int diff = (int)now.second - (int)prev_sec;
                if (diff < 0) {
                    diff += 60;
                }
                if (diff >= 1 && diff <= 2) {
                    tick_ok++;
                } else {
                    ESP_LOGW(TAG, "  second delta=%d (expect 1)", diff);
                }
            }
            prev_sec = now.second;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    if (tick_ok >= 3) {
        ESP_LOGI(TAG, "Tick test PASS (%d/4 intervals OK)", tick_ok);
    } else {
        ESP_LOGW(TAG, "Tick test FAIL (%d/4 intervals OK) - check crystal/STOP/VBAT", tick_ok);
    }

    /* 写时间再读回 */
    ESP_LOGI(TAG, "--- Set-time test (2026-05-28 12:00:00 Wed) ---");
    rx8130_time_t set_t = {
        .second = 0,
        .minute = 0,
        .hour   = 12,
        .day    = 3,   /* Wednesday */
        .date   = 28,
        .month  = 5,
        .year   = 2026,
    };
    err = rx8130_set_time(port, &set_t);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "set_time failed: %s", esp_err_to_name(err));
    } else {
        vTaskDelay(pdMS_TO_TICKS(50));
        rx8130_time_t readback = {0};
        if (rx8130_get_time(port, &readback) == ESP_OK) {
            ESP_LOGI(TAG, "Read back after set:");
            rx8130_print_time(&readback);
            if (readback.hour == 12 && readback.minute == 0 && readback.second <= 2 &&
                readback.date == 28 && readback.month == 5 && readback.year == 2026) {
                ESP_LOGI(TAG, "Set-time test PASS");
            } else {
                ESP_LOGW(TAG, "Set-time test FAIL (values mismatch)");
            }
        }
    }

    ESP_LOGI(TAG, "--- Continuous monitor (Ctrl+C to stop) ---");
    while (1) {
        rx8130_time_t now = {0};
        if (rx8130_get_time(port, &now) == ESP_OK) {
            rx8130_print_time(&now);
        } else {
            ESP_LOGE(TAG, "read time failed");
        }
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}
