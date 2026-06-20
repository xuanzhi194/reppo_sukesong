# bsp_button

Small reusable GPIO button helper.

## Hardware

- Button to GND with internal pull-up: use default `active_low = true`.
- Button to VCC with internal pull-down: set `active_low = false`, disable
  pull-up and enable pull-down.

## Basic Usage

```c
bsp_button_config_t btn = BSP_BUTTON_DEFAULT_CONFIG(GPIO_NUM_0);
bsp_button_init(&btn);

if (bsp_button_read_debounced(&btn)) {
    // pressed
}
```

## Porting Notes

This is a polling helper.  If you need wake-up or low-power behavior, configure
GPIO interrupts or light-sleep wake sources in your application.

