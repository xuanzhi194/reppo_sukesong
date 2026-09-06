/* Exercise the production driver with a register model and injected I2C errors. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "rx8130.h"
#include "freertos/task.h"

enum { READ_ERROR = 0x201, STOP_ERROR, CALENDAR_ERROR, RESUME_ERROR, RETRY_ERROR };
static uint8_t registers[256];
static esp_err_t read_result;
static esp_err_t write_results[4];
static bool failed_stop_was_applied;
static unsigned write_count;
static unsigned delay_count;
static unsigned call_count;
static struct {
    bool is_read;
    uint8_t bytes[8];
    size_t length;
} calls[8];

static void record_call(bool is_read, const uint8_t *data, size_t length)
{
    assert(call_count < sizeof(calls) / sizeof(calls[0]));
    assert(length <= sizeof(calls[0].bytes));
    calls[call_count].is_read = is_read;
    calls[call_count].length = length;
    memcpy(calls[call_count].bytes, data, length);
    call_count++;
}

esp_err_t i2c_master_write_read_device(i2c_port_t port, uint8_t addr,
                                      const uint8_t *write_data, size_t write_len,
                                      uint8_t *read_data, size_t read_len, TickType_t timeout)
{
    assert(port == I2C_NUM_0 && addr == RX8130_ADDR && timeout > 0);
    assert(write_len == 1 && read_len == 1 && write_data[0] == RX8130_REG_CTRL0);
    record_call(true, write_data, write_len);
    if (read_result == ESP_OK) {
        *read_data = registers[write_data[0]];
    }
    return read_result;
}

esp_err_t i2c_master_write_to_device(i2c_port_t port, uint8_t addr,
                                    const uint8_t *data, size_t len, TickType_t timeout)
{
    assert(port == I2C_NUM_0 && addr == RX8130_ADDR && timeout > 0);
    assert(len >= 2 && write_count < sizeof(write_results) / sizeof(write_results[0]));
    record_call(false, data, len);
    esp_err_t result = write_results[write_count++];
    /* A timeout may be reported even after the STOP register reached the chip. */
    if (result == ESP_OK || (write_count == 1 && failed_stop_was_applied)) {
        memcpy(&registers[data[0]], &data[1], len - 1);
    }
    return result;
}

esp_err_t i2c_param_config(i2c_port_t port, const i2c_config_t *config)
{
    (void)port; (void)config; return ESP_OK;
}

esp_err_t i2c_driver_install(i2c_port_t port, int mode, size_t rx, size_t tx, int flags)
{
    (void)port; (void)mode; (void)rx; (void)tx; (void)flags; return ESP_OK;
}

void vTaskDelay(TickType_t ticks)
{
    assert(ticks == pdMS_TO_TICKS(10));
    delay_count++;
}

static void assert_control_write(unsigned index, uint8_t expected)
{
    assert(index < call_count && !calls[index].is_read);
    assert(calls[index].length == 2);
    assert(calls[index].bytes[0] == RX8130_REG_CTRL0);
    assert(calls[index].bytes[1] == expected);
}

static void run_case(const char *name, uint8_t initial_control, esp_err_t read_error,
                     esp_err_t stop_error, esp_err_t calendar_error,
                     esp_err_t resume_error, esp_err_t retry_error,
                     bool partial_stop, esp_err_t expected_result)
{
    memset(registers, 0, sizeof(registers));
    memset(calls, 0, sizeof(calls));
    memset(write_results, 0, sizeof(write_results));
    registers[RX8130_REG_CTRL0] = initial_control;
    read_result = read_error;
    failed_stop_was_applied = partial_stop;
    write_count = delay_count = call_count = 0;

    write_results[0] = stop_error;
    unsigned resume_write = 1;
    if (stop_error == ESP_OK) {
        write_results[1] = calendar_error;
        resume_write = 2;
    }
    write_results[resume_write] = resume_error;
    write_results[resume_write + 1] = retry_error;

    rx8130_time_t phone = {
        .year = 2026, .month = 9, .date = 6, .day = 0,
        .hour = 23, .minute = 58, .second = 59,
    };
    assert(rx8130_set_time(I2C_NUM_0, &phone) == expected_result);
    assert(call_count >= 1 && calls[0].is_read);
    assert(calls[0].length == 1 && calls[0].bytes[0] == RX8130_REG_CTRL0);
    if (read_error != ESP_OK) {
        assert(call_count == 1 && write_count == 0 && delay_count == 0);
        assert(registers[RX8130_REG_CTRL0] == initial_control);
    } else {
        uint8_t running_control = (uint8_t)(initial_control & ~RX8130_CTRL0_STOP);
        assert_control_write(1, (uint8_t)(initial_control | RX8130_CTRL0_STOP));
        if (stop_error == ESP_OK) {
            const uint8_t calendar[] = {0x10, 0x59, 0x58, 0x23, 0x01, 0x06, 0x09, 0x26};
            assert(!calls[2].is_read && calls[2].length == sizeof(calendar));
            assert(memcmp(calls[2].bytes, calendar, sizeof(calendar)) == 0);
        }
        unsigned resume_call = resume_write + 1;
        assert_control_write(resume_call, running_control);
        if (resume_error != ESP_OK) {
            assert_control_write(resume_call + 1, running_control);
            assert(call_count == resume_call + 2 && delay_count == 1);
        } else {
            assert(call_count == resume_call + 1 && delay_count == 0);
        }
        if (resume_error == ESP_OK || retry_error == ESP_OK) {
            assert(registers[RX8130_REG_CTRL0] == running_control);
        } else {
            assert(registers[RX8130_REG_CTRL0] & RX8130_CTRL0_STOP);
        }
    }
    printf("PASS %s\n", name);
}

int main(void)
{
    run_case("normal sync keeps other control bits", 0x29, 0, 0, 0, 0, 0, false, ESP_OK);
    run_case("previously stopped clock resumes", 0x69, 0, 0, 0, 0, 0, false, ESP_OK);
    run_case("initial read failure leaves chip untouched", 0x29, READ_ERROR, 0, 0, 0, 0, false, READ_ERROR);
    run_case("calendar failure still resumes", 0x29, 0, 0, CALENDAR_ERROR, 0, 0, false, CALENDAR_ERROR);
    run_case("partial STOP failure still resumes", 0x29, 0, STOP_ERROR, 0, 0, 0, true, STOP_ERROR);
    run_case("STOP failure skips calendar", 0x29, 0, STOP_ERROR, 0, 0, 0, false, STOP_ERROR);
    run_case("resume retry succeeds but sync fails", 0x29, 0, 0, 0, RESUME_ERROR, 0, false, RESUME_ERROR);
    run_case("both resume writes fail", 0x29, 0, 0, 0, RESUME_ERROR, RETRY_ERROR, false, RESUME_ERROR);
    run_case("calendar error survives resume retry", 0x29, 0, 0, CALENDAR_ERROR, RESUME_ERROR, 0, false, CALENDAR_ERROR);
    run_case("partial STOP failure with resume retry", 0x29, 0, STOP_ERROR, 0, RESUME_ERROR, 0, true, STOP_ERROR);
    return 0;
}
