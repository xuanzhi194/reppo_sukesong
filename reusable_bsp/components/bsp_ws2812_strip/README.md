# bsp_ws2812_strip

Reusable WS2812/WS2812B addressable LED strip driver using Espressif's
`led_strip` RMT backend.

## Hardware

- WS2812 / WS2812B / compatible single-wire RGB LED strip
- Default Reppo wiring: GPIO17, 10 LEDs, GRB order
- Use a suitable 5 V supply and level shifting when required by your strip.

## Basic Usage

```c
bsp_ws2812_strip_config_t cfg = BSP_WS2812_STRIP_DEFAULT_CONFIG();
cfg.gpio_num = 17;
cfg.led_count = 10;
bsp_ws2812_strip_init(&cfg);

bsp_ws2812_strip_fill(32, 0, 0);
bsp_ws2812_strip_clear();
```

## API

- `bsp_ws2812_strip_init(config)`: create RMT LED strip handle.
- `bsp_ws2812_strip_handle()`: access the raw `led_strip_handle_t` if needed.
- `bsp_ws2812_strip_set_pixel(index, r, g, b)`: set and refresh one LED.
- `bsp_ws2812_strip_fill(r, g, b)`: fill whole strip.
- `bsp_ws2812_strip_flow(...)`: blocking chase animation.
- `bsp_ws2812_strip_flow_async(...)`: run chase animation in a FreeRTOS task.

## Porting Notes

Change `gpio_num`, `led_count`, and `pixel_format`.  Most WS2812B strips use
GRB order, not RGB.

