#include <assert.h>
#include <math.h>
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
    assert(port == I2C_NUM_0 && timeout > 0);
    assert(write_len == 1 && write_data != NULL && read_data != NULL);
    if (next_xfer != ESP_OK || addr != LSM6DSR_ADDR_SA0_LOW) {
        return ESP_FAIL;
    }
    memcpy(read_data, &registers[write_data[0]], read_len);
    return ESP_OK;
}

esp_err_t i2c_master_write_to_device(i2c_port_t port, uint8_t addr,
                                    const uint8_t *data, size_t len, TickType_t timeout)
{
    assert(port == I2C_NUM_0 && timeout > 0);
    assert(len >= 2 && data != NULL);
    if (next_xfer != ESP_OK || addr != LSM6DSR_ADDR_SA0_LOW) {
        return ESP_FAIL;
    }
    memcpy(&registers[data[0]], &data[1], len - 1);
    if (data[0] == LSM6DSR_REG_CTRL3_C && (data[1] & 0x01)) {
        registers[LSM6DSR_REG_CTRL3_C] = (uint8_t)(data[1] & (uint8_t)~0x01);
    }
    return ESP_OK;
}

int gpio_config(const gpio_config_t *cfg)
{
    assert(cfg != NULL);
    assert(cfg->pin_bit_mask & (1ULL << LSM6DSR_INT1_GPIO));
    assert(cfg->pin_bit_mask & (1ULL << LSM6DSR_INT2_GPIO));
    return ESP_OK;
}

int gpio_get_level(gpio_num_t pin)
{
    (void)pin;
    return 0;
}

void vTaskDelay(TickType_t ticks)
{
    (void)ticks;
    delay_count++;
}

static void reset_bus(void)
{
    memset(registers, 0, sizeof(registers));
    registers[LSM6DSR_REG_WHO_AM_I] = LSM6DSR_WHO_AM_I_VALUE;
    next_xfer = ESP_OK;
    delay_count = 0;
}

static void test_probe_ok(void)
{
    uint8_t addr = 0, id = 0;
    reset_bus();
    assert(lsm6dsr_probe(I2C_NUM_0, &addr, &id) == ESP_OK);
    assert(addr == LSM6DSR_ADDR_SA0_LOW);
    assert(id == LSM6DSR_WHO_AM_I_VALUE);
}

static void test_probe_wrong_id(void)
{
    uint8_t id = 0;
    reset_bus();
    registers[LSM6DSR_REG_WHO_AM_I] = 0x6A;
    assert(lsm6dsr_probe(I2C_NUM_0, NULL, &id) == ESP_ERR_INVALID_RESPONSE);
    assert(id == 0x6A);
}

static void test_probe_nack(void)
{
    reset_bus();
    next_xfer = ESP_FAIL;
    assert(lsm6dsr_probe(I2C_NUM_0, NULL, NULL) == ESP_ERR_NOT_FOUND);
}

static void test_init_default_config(void)
{
    reset_bus();
    assert(lsm6dsr_init(I2C_NUM_0, NULL) == ESP_OK);
    assert(lsm6dsr_is_ready());
    assert(lsm6dsr_get_addr() == LSM6DSR_ADDR_SA0_LOW);
    /* 104 Hz, ±4 g → CTRL1_XL = 0x48; 104 Hz, ±500 dps → CTRL2_G = 0x44 */
    assert(registers[LSM6DSR_REG_CTRL1_XL] == 0x48);
    assert(registers[LSM6DSR_REG_CTRL2_G] == 0x44);
    assert(registers[LSM6DSR_REG_CTRL3_C] == 0x44);
    assert(lsm6dsr_int_gpio_init() == ESP_OK);
    assert(delay_count > 0);
}

static void test_read_scale(void)
{
    lsm6dsr_data_t d;
    reset_bus();
    assert(lsm6dsr_init(I2C_NUM_0, NULL) == ESP_OK);
    /* temp raw 0 → 25 C; accel X = 1000 LSB at ±4 g (0.122 mg/LSB) → 0.122 g */
    registers[LSM6DSR_REG_OUTX_L_A] = 0xE8;
    registers[LSM6DSR_REG_OUTX_L_A + 1] = 0x03;
    assert(lsm6dsr_read(&d) == ESP_OK);
    assert(fabsf(d.temp_c - 25.0f) < 0.01f);
    assert(fabsf(d.accel_g[0] - 0.122f) < 0.001f);
    assert(d.accel_g[1] == 0.0f);
    assert(d.gyro_dps[0] == 0.0f);
    assert(lsm6dsr_read(NULL) == ESP_ERR_INVALID_ARG);
    assert(lsm6dsr_self_test(NULL) == ESP_ERR_INVALID_ARG);
}

static void test_power_down_and_wake(void)
{
    lsm6dsr_data_t d;
    reset_bus();
    assert(lsm6dsr_init(I2C_NUM_0, NULL) == ESP_OK);
    assert(lsm6dsr_power_down() == ESP_OK);
    assert(!lsm6dsr_is_ready());
    assert(registers[LSM6DSR_REG_CTRL1_XL] == 0);
    assert(registers[LSM6DSR_REG_CTRL2_G] == 0);
    assert(lsm6dsr_read(&d) == ESP_ERR_INVALID_STATE);

    assert(lsm6dsr_configure(NULL) == ESP_OK);
    assert(lsm6dsr_is_ready());
    assert(registers[LSM6DSR_REG_CTRL1_XL] == 0x48);
    assert(registers[LSM6DSR_REG_CTRL2_G] == 0x44);
}

int main(void)
{
    test_probe_ok();
    test_probe_wrong_id();
    test_probe_nack();
    test_init_default_config();
    test_read_scale();
    test_power_down_and_wake();
    puts("PASS");
    return 0;
}
