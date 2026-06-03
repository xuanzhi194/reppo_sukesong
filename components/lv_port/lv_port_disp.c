/**
 * @file lv_port_disp.c
 *
 * LVGL 8.x  显示移植层
 *   - ST7789P3 8080-I8 + i80 DMA
 *   - 双缓冲、单点等待 DMA
 */
#if 1

#include "lv_port_disp.h"
#include "lcd.h"
#include "esp_attr.h"
#include "esp_lcd_panel_io.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdbool.h>

static const char *TAG_DISP = "lv_port_disp";

#ifndef MY_DISP_HOR_RES
#define MY_DISP_HOR_RES   320
#endif
#ifndef MY_DISP_VER_RES
#define MY_DISP_VER_RES   170
#endif

#ifndef MY_DISP_BUF_LINES
#define MY_DISP_BUF_LINES  LCD_LVGL_BUF_LINES
#endif

static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);

static lv_disp_drv_t s_disp_drv;

static lv_color_t s_buf_a[MY_DISP_HOR_RES * MY_DISP_BUF_LINES] DRAM_ATTR;
static lv_color_t s_buf_b[MY_DISP_HOR_RES * MY_DISP_BUF_LINES] DRAM_ATTR;

volatile bool disp_flush_enabled = true;
static volatile bool s_dma_done = false;

void disp_enable_update(void)  { disp_flush_enabled = true; }
void disp_disable_update(void) { disp_flush_enabled = false; }

static bool IRAM_ATTR on_lcd_dma_done(esp_lcd_panel_io_handle_t panel_io,
                                      esp_lcd_panel_io_event_data_t *edata,
                                      void *user_ctx)
{
    (void)panel_io;
    (void)edata;
    (void)user_ctx;
    s_dma_done = true;
    return false;
}

void lv_port_disp_init(void)
{
    lcd_init();

    static lv_disp_draw_buf_t draw_buf_dsc;
    lv_disp_draw_buf_init(&draw_buf_dsc, s_buf_a, s_buf_b,
                          MY_DISP_HOR_RES * MY_DISP_BUF_LINES);

    lv_disp_drv_init(&s_disp_drv);
    s_disp_drv.hor_res  = MY_DISP_HOR_RES;
    s_disp_drv.ver_res  = MY_DISP_VER_RES;
    s_disp_drv.flush_cb = disp_flush;
    s_disp_drv.draw_buf = &draw_buf_dsc;
    lv_disp_drv_register(&s_disp_drv);

    lcd_register_flush_done_cb(on_lcd_dma_done, &s_disp_drv);
}

static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    if (!disp_flush_enabled) {
        lv_disp_flush_ready(disp_drv);
        return;
    }

    s_dma_done = false;
    lcd_draw_bitmap_async((uint16_t)area->x1, (uint16_t)area->y1,
                          (uint16_t)area->x2, (uint16_t)area->y2,
                          (uint16_t *)color_p);

    const TickType_t tick0 = xTaskGetTickCount();
    while (!s_dma_done) {
        if ((xTaskGetTickCount() - tick0) > pdMS_TO_TICKS(100)) {
            ESP_LOGW(TAG_DISP, "flush DMA timeout");
            break;
        }
        taskYIELD();
    }

    lv_disp_flush_ready(disp_drv);
}

#else
typedef int keep_pedantic_happy;
#endif
