# lcd_i80_st7789p3_template

This is a migration note for the current Reppo LCD driver, not a cleaned source
component yet.

The existing project driver targets a very specific 1.9 inch ST7789P3 panel in
8080-I8 parallel mode.  It contains useful bring-up knowledge, but also many
board-specific pins, LVGL buffer assumptions, and diagnostic profiles.  Extract
it as a reusable driver only after your next board keeps the same panel/bus.

## Current Hardware Assumptions

- Controller: ST7789P3
- Interface: 8080 8-bit parallel write mode
- Logical UI: 320 x 170 landscape
- Internal GRAM: 240 x 320
- Visible offset: Y offset 35
- Pixel format: RGB565
- ESP-IDF driver: `esp_lcd_panel_io_i80`

## Current Reppo Pin Map

| Signal | GPIO |
| --- | --- |
| RST | GPIO39 |
| CS | GPIO38 |
| DC | GPIO45 |
| WR | GPIO48 |
| RD | GPIO47, held high for write-only mode |
| BLK | GPIO3 |
| DB0 | GPIO21 |
| DB1 | GPIO14 |
| DB2 | GPIO13 |
| DB3 | GPIO12 |
| DB4 | GPIO11 |
| DB5 | GPIO10 |
| DB6 | GPIO9 |
| DB7 | GPIO46 |

## Recommended Reusable API Shape

When you clean this driver, prefer this interface instead of hard-coded macros:

```c
typedef struct {
    gpio_num_t rst;
    gpio_num_t cs;
    gpio_num_t dc;
    gpio_num_t wr;
    gpio_num_t rd;
    gpio_num_t backlight;
    gpio_num_t data[8];
    uint16_t width;
    uint16_t height;
    uint16_t x_gap;
    uint16_t y_gap;
    uint32_t pixel_clock_hz;
    bool swap_color_bytes;
    bool bgr_order;
    bool dc_inverted;
} bsp_lcd_i80_st7789p3_config_t;
```

Suggested APIs:

- `bsp_lcd_i80_st7789p3_init(config)`
- `bsp_lcd_i80_st7789p3_deinit()`
- `bsp_lcd_i80_st7789p3_backlight_on/off()`
- `bsp_lcd_i80_st7789p3_draw_bitmap(x1, y1, x2, y2, pixels)`
- `bsp_lcd_i80_st7789p3_register_flush_done_cb(cb, ctx)`
- `bsp_lcd_i80_st7789p3_wait_flush_done()`

## LVGL Porting Checklist

1. Match `width`, `height`, `x_gap`, and `y_gap` to the physical panel.
2. Confirm `MADCTL` orientation and BGR/RGB order with color bars.
3. Decide one place to do RGB565 byte swap: either LVGL config or LCD driver,
   not both.
4. Keep DMA buffers in memory accessible by the LCD peripheral.
5. Register the LCD flush-done callback and call `lv_disp_flush_ready()` there
   or after `wait_flush_done()`.

## Why It Is Not Source-Cleaned Yet

The current file mixes three responsibilities:

- Production LCD flush path.
- Hardware bring-up tests such as color bars, checkerboard, and backlight blink.
- Multiple experimental pin/timing profiles.

For a reusable BSP, split those into:

- `bsp_lcd_i80_st7789p3.c`: production driver only.
- `bsp_lcd_i80_st7789p3_test.c`: optional diagnostics.
- Board config in the application or Kconfig.

