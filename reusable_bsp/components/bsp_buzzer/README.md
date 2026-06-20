# bsp_buzzer

Reusable LEDC PWM driver for a passive buzzer.

## Hardware

- Passive buzzer or piezo element connected to one GPIO.
- If the buzzer current is high, drive it through a transistor instead of
  directly from the ESP32 GPIO.

## Basic Usage

```c
#include "bsp_buzzer.h"

void app_main(void)
{
    bsp_buzzer_config_t cfg = BSP_BUZZER_DEFAULT_CONFIG();
    cfg.gpio_num = 7;
    bsp_buzzer_init(&cfg);

    bsp_buzzer_beep(2400, 70, 120);
}
```

## API

- `bsp_buzzer_init(config)`: configure LEDC timer/channel and GPIO.
- `bsp_buzzer_set_sound(freq_hz, volume_percent)`: start or update PWM.
- `bsp_buzzer_stop()`: set duty to zero.
- `bsp_buzzer_beep(...)`: blocking one-shot beep.
- `bsp_buzzer_alarm(...)`: blocking grouped beep pattern.
- `bsp_buzzer_set_mute_cb(cb, ctx)`: optional runtime mute policy.

## Porting Notes

Change only `gpio_num`, LEDC timer/channel, and resolution.  If your project
already uses LEDC, avoid channel/timer conflicts.

