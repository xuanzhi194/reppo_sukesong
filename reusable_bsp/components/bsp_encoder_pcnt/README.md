# bsp_encoder_pcnt

Reusable quadrature rotary encoder driver based on ESP-IDF PCNT.

## Hardware

- EC06 / EC11 / compatible mechanical quadrature encoder
- Two phase outputs A/B connected to GPIOs with pull-ups
- Default Reppo pins: A GPIO1, B GPIO2

## Basic Usage

```c
bsp_encoder_pcnt_config_t enc = BSP_ENCODER_PCNT_DEFAULT_CONFIG();
enc.gpio_a = GPIO_NUM_1;
enc.gpio_b = GPIO_NUM_2;
bsp_encoder_pcnt_init(&enc);

while (1) {
    switch (bsp_encoder_pcnt_poll()) {
    case BSP_ENCODER_EVENT_CW:
        // next
        break;
    case BSP_ENCODER_EVENT_CCW:
        // previous
        break;
    default:
        break;
    }
    vTaskDelay(pdMS_TO_TICKS(2));
}
```

## API

- `bsp_encoder_pcnt_init(config)`: configure PCNT 4x quadrature decoding.
- `bsp_encoder_pcnt_poll()`: polling API that returns one event per detent-like
  movement.
- `bsp_encoder_pcnt_reset()`: clear accumulated movement and PCNT count.

## Tuning

- `step_cw` / `step_ccw`: hardware counts per logical event.
- `event_gap_us`: minimum gap between emitted UI events.
- `glitch_filter_ns`: hardware pulse filter.
- `reverse_noise`, `reverse_budget`, `dir_lock_us`: suppress contact bounce
  that briefly reports the opposite direction.
- `invert_direction`: flip CW/CCW if the encoder wiring is reversed.

## Porting Notes

Poll this driver from a UI/task loop.  For LVGL focus navigation, translate
`CW/CCW` to next/previous key events in your input driver.

