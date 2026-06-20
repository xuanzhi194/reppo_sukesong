# bsp_battery_adc

Reusable ADC battery monitor for a single-cell Li-ion/LiPo battery measured
through a resistor divider.

## Hardware

Typical circuit:

```text
VBAT --- R1 --- ADC GPIO --- R2 --- GND
```

For equal resistors, `voltage_divider_ratio = 2.0`.

## Basic Usage

```c
#include "bsp_battery_adc.h"

void app_main(void)
{
    bsp_battery_adc_config_t cfg = BSP_BATTERY_ADC_DEFAULT_CONFIG();
    cfg.channel = ADC_CHANNEL_7;          // ESP32-S3 GPIO8
    cfg.voltage_divider_ratio = 2.0f;     // 100k + 100k
    bsp_battery_adc_init(&cfg);

    int percent = 0;
    bsp_battery_adc_read_percent(&percent);
}
```

## API

- `bsp_battery_adc_init(config)`: initialize ADC oneshot and optional
  curve-fitting calibration.
- `bsp_battery_adc_read_raw(raw)`: averaged raw ADC code.
- `bsp_battery_adc_read_pin_mv(pin_mv)`: voltage at ADC pin.
- `bsp_battery_adc_read_battery_mv(battery_mv)`: voltage before divider.
- `bsp_battery_adc_read_percent(percent)`: linear percent between `min_mv` and
  `max_mv`.

## Porting Notes

Set the ADC channel for your GPIO and update `voltage_divider_ratio`.
For non-linear battery gauges, keep this driver for voltage reading and do the
SOC curve mapping in your application.

