# bsp_rx8130

Reusable I2C driver for Epson RX8130 real-time clock.

## Hardware

- Sensor/chip: Epson RX8130 RTC
- Bus: I2C, default address `0x32`
- Current Reppo default pins: SDA GPIO15, SCL GPIO16
- Use external I2C pull-ups when possible; internal pull-ups are acceptable for
  short bring-up wiring.

## Basic Usage

```c
#include "bsp_rx8130.h"

void app_main(void)
{
    bsp_rx8130_i2c_config_t i2c = BSP_RX8130_I2C_DEFAULT_CONFIG();
    bsp_rx8130_i2c_init(&i2c);

    bool lost_power = false;
    if (bsp_rx8130_probe(i2c.port) == ESP_OK) {
        bsp_rx8130_startup(i2c.port, &lost_power);
    }

    bsp_rx8130_time_t now;
    bsp_rx8130_get_time(i2c.port, &now);
}
```

## API

- `bsp_rx8130_i2c_init(config)`: optional helper to configure/install an I2C
  master driver.
- `bsp_rx8130_probe(port)`: check whether the device responds.
- `bsp_rx8130_startup(port, lost_power)`: clear STOP/test flags, configure
  backup switchover, report VLF power-loss status.
- `bsp_rx8130_get_time(port, time)`: read BCD calendar registers.
- `bsp_rx8130_set_time(port, time)`: stop clock, write calendar, resume clock.
- `bsp_rx8130_sync_from_system_time(port, min_valid_year)`: write ESP system
  time to RTC after SNTP or another time source is valid.
- `bsp_rx8130_read_reg/write_reg`: low-level register access.

## Porting Notes

If your project already owns the I2C bus, skip `bsp_rx8130_i2c_init()` and call
the device APIs with your existing `i2c_port_t`.  Keep the bus at 100 kHz until
your hardware has been validated.

