#ifndef __MYLCD_H_
#define __MYLCD_H_

#include <stdint.h>
#include <stdbool.h>
#include "driver/gpio.h"
#include "esp_lcd_types.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"

/* =========================================================================
 *  ST7789P3  1.9inch  170x320  30pin 8080-I8 并口屏 (T190X7 等)
 *  - 8080 并口 COLMOD=0x05 (16bit); SPI 驱动常用 0x55, 二者不可混用
 *  - 内部 RAM 240x320, 可视 170x320, 横屏 UI 320x170, Y 偏移 35
 *  - IM[3:0]=0000 -> 8080 8bit (IM0/IM1/IM2/IM3 均 GND, 见手册 8.1)
 *  - 8080 COLMOD=0x05 (16bit); SPI 才用 0x55 (见手册 8.7.3)
 *  - RDDID(04h) 默认: 81 81 B3 (见手册 9.1.3)
 *  - WRX 上升沿锁存数据; RDX 只写时接 VDDI(高)
 * ========================================================================= */

/* 与原理图 ESP32-S3 <-> FPC1 一致 (8080-I8 写模式, RDX 屏侧未用) */
#define LCD_PIN_RST    GPIO_NUM_39   /* TFT_RESX */
#define LCD_PIN_CS     GPIO_NUM_38   /* TFT_CSX  */
#define LCD_PIN_DC     GPIO_NUM_45   /* TFT_DCX  */
#define LCD_PIN_WR     GPIO_NUM_48   /* TFT_WRX  */
#define LCD_PIN_BLK    GPIO_NUM_3    /* TFT_BLK  */

#define LCD_PIN_DB0    GPIO_NUM_21
#define LCD_PIN_DB1    GPIO_NUM_14
#define LCD_PIN_DB2    GPIO_NUM_13
#define LCD_PIN_DB3    GPIO_NUM_12
#define LCD_PIN_DB4    GPIO_NUM_11
#define LCD_PIN_DB5    GPIO_NUM_10
#define LCD_PIN_DB6    GPIO_NUM_9
#define LCD_PIN_DB7    GPIO_NUM_46
#define LCD_PIN_RD     GPIO_NUM_47   /* TFT_RDX: 8080 只写时保持高 */

/* ST7789 内部 RAM (大于模组可视 170x320, 用于 brute-force 刷屏) */
#define ST7789_RAM_H   240
#define ST7789_RAM_V   320

#define TFT_WIDTH      320
#define TFT_HEIGHT     170
#define TFT_X_OFFSET   0
#define TFT_Y_OFFSET   35

/* 8080 像素时钟 (Hz). 模组稳定后可试 20M; 16M 为性能/稳定性折中.
 * 12M: 降低总线翻转瞬时电流/地弹, 缓解大面积重绘时背光被电源纹波带闪 */
#ifndef LCD_PIXEL_CLOCK_HZ
#define LCD_PIXEL_CLOCK_HZ   (12 * 1000 * 1000)
#endif

/* 8080 数据/控制脚驱动强度: 0~3, 默认 2(20mA).
 * 降到 1(10mA) 可明显减小同步翻转的电流尖峰与地弹(12MHz 下时序裕量充足);
 * 若出现花屏/条纹请改回 GPIO_DRIVE_CAP_2 */
#ifndef LCD_BUS_DRIVE_CAP
#define LCD_BUS_DRIVE_CAP    GPIO_DRIVE_CAP_1
#endif

/* 与 lv_port MY_DISP_BUF_LINES 一致, 用于 I80 max_transfer_bytes */
/* 受内部 DRAM 限制 (85 行会链接溢出); 50 行≈62KB 双缓冲, 全屏约 4 次 flush */
#ifndef LCD_LVGL_BUF_LINES
#define LCD_LVGL_BUF_LINES   50
#endif

/* 裸机测试: 1=GPIO bit-bang 8080 (不经过 esp_lcd/DMA) */
#ifndef LCD_TEST_USE_BITBANG
#define LCD_TEST_USE_BITBANG  0   /* 0=用 esp_lcd I80; 1=GPIO bitbang(仅排查) */
#endif

/* 裸机测试: 1=跑 esp_lcd I80 DMA 刷屏 */
#ifndef LCD_BAREMETAL_ESP_LCD_PHASE
#define LCD_BAREMETAL_ESP_LCD_PHASE  1
#endif

/* 裸机测试: 1=轮询多组 esp_lcd 参数 */
#ifndef LCD_BAREMETAL_SCAN_PROFILES
#define LCD_BAREMETAL_SCAN_PROFILES  0
#endif

/* 若原理图 RST 为 GPIO40 而非 39, 在编译选项里 -DLCD_PIN_RST=GPIO_NUM_40 */

/* 1.9" 320x170 横屏: 厂方 init 后 MADCTL (0x60=MV, 与旧 SPI 一致) */
#ifndef LCD_MADCTL_LANDSCAPE
#define LCD_MADCTL_LANDSCAPE  0x60
#endif

/* 8080 像素须高字节先发 (同旧 SPI lcd_draw_bitmap: hi=>>8, lo=&0xFF).
 * LVGL 缓冲按 uint16 数值组织, I80 DMA 须 swap=1 才与 SPI 线序一致. */
#ifndef LCD_I80_SWAP_COLOR_BYTES_FOR_LVGL
#define LCD_I80_SWAP_COLOR_BYTES_FOR_LVGL  1
#endif

/* 颜色宏 (RGB565, 兼容老代码) */
#define WHITE      0xFFFF
#define BLACK      0x0000
#define RED        0xF800
#define GREEN      0x07E0
#define BLUE       0x001F
#define MAGENTA    0xF81F
#define YELLOW     0xFFE0
#define CYAN       0x07FF
#define BROWN      0xBC40
#define BRRED      0xFC07
#define GRAY       0x8430
#define DARKBLUE   0x01CF
#define LIGHTBLUE  0x7D7C
#define GRAYBLUE   0x5458
#define LIGHTGREEN 0x841F
#define LGRAY      0xC618
#define LGRAYBLUE  0xA651
#define LBBLUE     0x2B12

/* ---------- 公共 API ---------- */

void lcd_init(void);
void lcd_deinit(void);
void lcd_set_madctl(uint8_t madctl);
/* swap_color_bytes: 裸机 RGB565 填色通常用 1; LVGL 且 LV_COLOR_16_SWAP=1 时用 0 */
void lcd_init_with_options(bool swap_color_bytes);
void lcd_init_full(bool swap_color_bytes, bool dc_inverted, bool bgr_order);
#ifndef LCD_BACKLIGHT_LEVEL_COUNT
#define LCD_BACKLIGHT_LEVEL_COUNT  5
#endif

/* 观感大致按 sqrt(duty) 拉开，低端更密：夜灯 / 暗室 / 室内 / 明亮 / 最大。
 * 旧三档 30/65/100 对应新档 2/3/4。 */
#ifndef LCD_BACKLIGHT_LEVEL0_PERCENT
#define LCD_BACKLIGHT_LEVEL0_PERCENT  5
#endif
#ifndef LCD_BACKLIGHT_LEVEL1_PERCENT
#define LCD_BACKLIGHT_LEVEL1_PERCENT  15
#endif
#ifndef LCD_BACKLIGHT_LEVEL2_PERCENT
#define LCD_BACKLIGHT_LEVEL2_PERCENT  35
#endif
#ifndef LCD_BACKLIGHT_LEVEL3_PERCENT
#define LCD_BACKLIGHT_LEVEL3_PERCENT  65
#endif
#ifndef LCD_BACKLIGHT_LEVEL4_PERCENT
#define LCD_BACKLIGHT_LEVEL4_PERCENT  100
#endif

void lcd_backlight_init(void);
void lcd_backlight_set_level(uint8_t level);
/** 仅缓存亮度百分比，供 lcd_backlight_init 启动时使用（不操作 GPIO）。 */
void lcd_backlight_prefetch_level(uint8_t level);
uint8_t lcd_backlight_level_to_percent(uint8_t level);
void lcd_on(void);
void lcd_off(void);
void lcd_clear(uint16_t color);
/* 不经过 LVGL，循环刷纯色/色条/棋盘格，用于硬件排查 */
void lcd_baremetal_test_run(void);

/* 绘制位图. 入参 (x1,y1,x2,y2) 为闭区间 [x1,x2] x [y1,y2],
 * 与原 SPI 驱动一致. 内部使用 DMA 异步提交,
 * 传输完成后会调用通过 lcd_register_flush_done_cb 注册的回调. */
void lcd_draw_bitmap(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t *img);
/* 仅提交 DMA, 不等待完成 (LVGL flush 路径用, 由上层等待一次) */
void lcd_draw_bitmap_async(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t *img);
void lcd_wait_flush_done(void);

/* DMA 传输完成回调原型 (与 esp_lcd_panel_io_color_trans_done_cb_t 一致) */
typedef bool (*lcd_flush_done_cb_t)(esp_lcd_panel_io_handle_t panel_io,
                                    esp_lcd_panel_io_event_data_t *edata,
                                    void *user_ctx);

void lcd_register_flush_done_cb(lcd_flush_done_cb_t cb, void *user_ctx);

#endif /* __MYLCD_H_ */
