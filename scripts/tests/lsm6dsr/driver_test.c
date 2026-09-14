#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "lsm6dsr.h"
#include "freertos/task.h"

static uint8_t registers[256];
static esp_err_t next_xfer = ESP_OK;
static unsigned delay_count;

esp_err_t i2c_master_write_read_device(i2c_port_t port, uint8_t addr,
                                      const uint8_t *write_data, size_t write_len,
                                      uint8_t *read_data, size_t read_len, TickType_t timeout)
{
    assert(port == I2C_NUM_0 && addr == LSM6DSR_I2C_ADDR && timeout > 0);
    assert(write_len == 1 && write_data != NULL && read_data != NULL);
    if (next_xfer != ESP_OK) {
        return next_xfer;
    }
    memcpy(read_data, &registers[write_data[0]], read_len);
    return ESP_OK;
}

esp_err_t i2c_master_write_to_device(i2c_port_t port, uint8_t addr,
                                    const uint8_t *data, size_t len, TickType_t timeout)
{
    assert(port == I2C_NUM_0 && addr == LSM6DSR_I2C_ADDR && timeout > 0);
    assert(len >= 2 && data != NULL);
    if (next_xfer != ESP_OK) {
        return next_xfer;
    }
    memcpy(&registers[data[0]], &data[1], len - 1);
    if (data[0] == LSM6DSR_REG_CTRL3_C && (data[1] & LSM6DSR_CTRL3_SW_RESET)) {
        registers[LSM6DSR_REG_CTRL3_C] = (uint8_t)(data[1] & (uint8_t)~LSM6DSR_CTRL3_SW_RESET);
    }
    return ESP_OK;
}

void vTaskDelay(TickType_t ticks)
{
    (void)ticks;
    delay_count++;
}

static void reset_bus(void)
{
    memset(registers, 0, sizeof(registers));
    registers[LSM6DSR_REG_WHO_AM_I] = LSM6DSR_WHO_AM_I_VAL;
    next_xfer = ESP_OK;
    delay_count = 0;
    assert(lsm6dsr_init(I2C_NUM_0) == ESP_OK);
}

static void test_probe_ok(void)
{
    reset_bus();
    assert(lsm6dsr_probe() == ESP_OK);
}

static void test_probe_wrong_id(void)
{
    reset_bus();
    registers[LSM6DSR_REG_WHO_AM_I] = 0x6A;
    assert(lsm6dsr_probe() == ESP_ERR_NOT_FOUND);
}

static void test_probe_nack(void)
{
    reset_bus();
    next_xfer = ESP_FAIL;
    assert(lsm6dsr_probe() == ESP_FAIL);
}

static void test_accel_unpack(void)
{
    lsm6dsr_raw3_t a;
    reset_bus();
    registers[0x28] = 0x00;
    registers[0x29] = 0x10; /* 4096 */
    registers[0x2A] = 0xFF;
    registers[0x2B] = 0xFF; /* -1 */
    registers[0x2C] = 0x00;
    registers[0x2D] = 0x40; /* 16384 ~ 1 g at ±2g */
    assert(lsm6dsr_read_accel_raw(&a) == ESP_OK);
    assert(a.x == 4096);
    assert(a.y == -1);
    assert(a.z == 16384);
}

static void test_solder_ok(void)
{
    reset_bus();
    registers[LSM6DSR_REG_STATUS] = LSM6DSR_STATUS_XLDA;
    registers[0x2C] = 0x00;
    registers[0x2D] = 0x40;
    assert(lsm6dsr_solder_test() == ESP_OK);
    assert(registers[LSM6DSR_REG_CTRL1_XL] == LSM6DSR_CTRL1_XL_12HZ5_2G);
    assert(delay_count > 0);
}

int main(void)
{
    test_probe_ok();
    test_probe_wrong_id();
    test_probe_nack();
    test_accel_unpack();
    test_solder_ok();
    puts("PASS");
    return 0;
}
