# Reusable ESP-IDF BSP Modules

This folder contains reusable BSP modules extracted from the Reppo firmware.
The original project files are not modified.  Each module is intentionally
small, configurable, and free of Reppo application state.

## Target

- Framework: ESP-IDF 5.x
- MCU family: ESP32-S3 first, most modules also work on other ESP32 chips when
  the peripheral exists.
- Style: one driver per component, with a stable public header in `include/`.

## Modules

| Module | Component | Hardware | Notes |
| --- | --- | --- | --- |
| Buzzer | `bsp_buzzer` | Passive buzzer driven by LEDC PWM | Configurable GPIO, LEDC timer/channel, mute callback |
| Battery ADC | `bsp_battery_adc` | Li-ion battery through resistor divider | ADC oneshot + optional calibration + percent mapping |
| RX8130 RTC | `bsp_rx8130` | Epson RX8130 I2C RTC | Probe, startup, get/set time, sync from system time |
| WS2812 Strip | `bsp_ws2812_strip` | WS2812/WS2812B addressable LEDs | RMT backend, solid/fill/flow/async flow |
| PCNT Encoder | `bsp_encoder_pcnt` | EC06/EC11 style quadrature encoder | ESP-IDF PCNT, detent filtering, direction output |
| Button | `bsp_button` | GPIO button | Active-high/active-low, pull mode, debounce helper |
| Wi-Fi Credential Store | `bsp_wifi_cred` | NVS-backed SSID/password cache | Not a Wi-Fi driver; stores credentials only |
| LCD Template | `lcd_i80_st7789p3_template` | ST7789P3 8080-I8 LCD | Migration notes for the current LCD driver |

## How To Use In Another ESP-IDF Project

Copy only the components you need into the target project's `components/`
directory:

```text
your_project/
  components/
    bsp_buzzer/
    bsp_rx8130/
```

Then include the public header:

```c
#include "bsp_buzzer.h"
#include "bsp_rx8130.h"
```

If you keep this whole `reusable_bsp` directory as a subfolder, add the chosen
component paths to `EXTRA_COMPONENT_DIRS` in the target project's top-level
`CMakeLists.txt`.

```cmake
set(EXTRA_COMPONENT_DIRS
    reusable_bsp/components/bsp_buzzer
    reusable_bsp/components/bsp_rx8130
)
```

## Design Rules Used In This Extraction

- No dependency on `custom.h`, `gui_guider.h`, LVGL screens, app globals, or
  Reppo data models.
- Pins, channels, limits, and hardware options are passed through config
  structs.
- Public APIs return `esp_err_t` where initialization or hardware I/O can fail.
- Blocking helper effects, such as beep or LED flow, are explicit in the API
  names or documentation.
- Defaults match the current Reppo R1 board where useful, but are not hidden in
  the implementation.

## What Was Not Directly Extracted

`todo_storage`, `word_storage`, `ai_chat`, and `bsp_ble` are useful references
but are product/application modules rather than pure reusable hardware drivers.
They depend on Reppo-specific JSON formats, CSV schemas, AI prompt formats, or
UI refresh flags.  Reuse them by first defining a stable protocol boundary.

The LCD driver is hardware-specific and contains many bring-up diagnostics.  A
separate migration note is provided in `components/lcd_i80_st7789p3_template/`.

