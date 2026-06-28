/*
 * ============================================================================
 *  factory_test_main.c
 *  Reppo R1 PCB 出厂焊接功能检测程序  (LCD UI 进度显示版)
 * ============================================================================
 *
 * 测试项目（顺序执行）:
 *  [01] PSRAM      — 外部 PSRAM 容量 & 读写验证（期望 >= 1 MB）
 *  [02] WS2812B    — 10 颗 LED 依次呈现红 / 绿 / 蓝 / 白 / 关
 *  [03] 蜂鸣器      — 低 / 中 / 高频各鸣响一次
 *  [04] 按键        — 等待 GPIO5 按下（限时 10 s，超时失败）
 *  [05] 编码器      — 等待 EC06 旋转任一方向（限时 15 s，超时失败）
 *  [06] 电池 ADC   — 读取电池电压 & TP4056 充电状态引脚电平
 *  [07] RTC        — I2C 探测 RX8130，读取时间寄存器
 *  [08] LCD        — ST7789P3 全屏纯色 + 八色彩条视觉测试
 *  [09] WiFi       — 扫描无线 AP（发现 >= 1 个即通过）
 *
 * LCD UI:
 *  屏幕实时显示每项测试的 WAIT / TESTING / PASS / FAIL 状态
 *  全部通过时标题栏变绿，有失败时变红
 *
 * 最终反馈：
 *  全部通过 -> WS2812B 常绿 + 蜂鸣器双响
 *  有失败项 -> WS2812B 常红 + 蜂鸣器三响
 * ============================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_heap_caps.h"
#include "esp_attr.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

/* BSP */
#include "ws2812b.h"
#include "buzzer.h"
#include "bat_percent.h"
#include "rx8130.h"
#include "ec06.h"
#include "keypad.h"
#include "wifi.h"
#include "lcd.h"

/* ============================================================================
 *  宏定义
 * ============================================================================ */

#define TAG               "FACTORY"
#define DELAY_MS(ms)      vTaskDelay(pdMS_TO_TICKS(ms))

#define BTN_TIMEOUT_MS    10000
#define ENC_TIMEOUT_MS    15000
#define POLL_INTERVAL_MS  50

#define BAT_VOLTAGE_MIN   2500
#define BAT_VOLTAGE_MAX   4500

#define PSRAM_MIN_BYTES   (1 * 1024 * 1024)

#define TEST_COUNT  9

/* ============================================================================
 *  测试名称 & 结果数组
 * ============================================================================ */

static const char *s_test_names[TEST_COUNT] = {
    "PSRAM 读写  ",
    "WS2812B LED",
    "蜂鸣器      ",
    "按键 GPIO5  ",
    "编码器 EC06 ",
    "电池 ADC    ",
    "RTC RX8130  ",
    "LCD ST7789  ",
    "WiFi 扫描   ",
};

static bool s_results[TEST_COUNT];

/* ============================================================================
 *  辅助：LED 颜色与蜂鸣器快捷操作
 * ============================================================================ */

static inline void led_off(void)   { set_flow_light(0, 0, 0); }
static inline void led_blue(void)  { set_flow_light(0, 0, 40); }
static inline void led_green(void) { set_flow_light(0, 50, 0); }
static inline void led_red(void)   { set_flow_light(60, 0, 0); }

static void buzzer_tone(uint32_t freq_hz, uint32_t dur_ms)
{
    buzzer_set_sound(freq_hz, 70);
    DELAY_MS(dur_ms);
    buzzer_set_sound(freq_hz, 0);
}

/* ============================================================================
 *  LCD UI — 8x8 位图字体 (ASCII 0x20~0x7E, 95 字符)
 *  每字符 8 字节，每字节为一像素行，bit7 = 最左像素
 * ============================================================================ */

static const uint8_t s_font8x8[95][8] = {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, /* 0x20 ' ' */
    {0x18,0x3C,0x3C,0x18,0x18,0x00,0x18,0x00}, /* 0x21 '!' */
    {0x36,0x36,0x12,0x00,0x00,0x00,0x00,0x00}, /* 0x22 '"' */
    {0x36,0x36,0x7F,0x36,0x7F,0x36,0x36,0x00}, /* 0x23 '#' */
    {0x0C,0x3E,0x03,0x1E,0x30,0x1F,0x0C,0x00}, /* 0x24 '$' */
    {0x00,0x63,0x33,0x18,0x0C,0x66,0x63,0x00}, /* 0x25 '%' */
    {0x1C,0x36,0x1C,0x6E,0x3B,0x33,0x6E,0x00}, /* 0x26 '&' */
    {0x06,0x06,0x03,0x00,0x00,0x00,0x00,0x00}, /* 0x27 '\'' */
    {0x18,0x0C,0x06,0x06,0x06,0x0C,0x18,0x00}, /* 0x28 '(' */
    {0x06,0x0C,0x18,0x18,0x18,0x0C,0x06,0x00}, /* 0x29 ')' */
    {0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00}, /* 0x2A '*' */
    {0x00,0x0C,0x0C,0x3F,0x0C,0x0C,0x00,0x00}, /* 0x2B '+' */
    {0x00,0x00,0x00,0x00,0x0C,0x0C,0x06,0x00}, /* 0x2C ',' */
    {0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00}, /* 0x2D '-' */
    {0x00,0x00,0x00,0x00,0x00,0x0C,0x0C,0x00}, /* 0x2E '.' */
    {0x60,0x30,0x18,0x0C,0x06,0x03,0x01,0x00}, /* 0x2F '/' */
    {0x3E,0x63,0x73,0x7B,0x6F,0x67,0x3E,0x00}, /* 0x30 '0' */
    {0x0C,0x0E,0x0C,0x0C,0x0C,0x0C,0x3F,0x00}, /* 0x31 '1' */
    {0x1E,0x33,0x30,0x1C,0x06,0x33,0x3F,0x00}, /* 0x32 '2' */
    {0x1E,0x33,0x30,0x1C,0x30,0x33,0x1E,0x00}, /* 0x33 '3' */
    {0x38,0x3C,0x36,0x33,0x7F,0x30,0x78,0x00}, /* 0x34 '4' */
    {0x3F,0x03,0x1F,0x30,0x30,0x33,0x1E,0x00}, /* 0x35 '5' */
    {0x1C,0x06,0x03,0x1F,0x33,0x33,0x1E,0x00}, /* 0x36 '6' */
    {0x3F,0x33,0x30,0x18,0x0C,0x0C,0x0C,0x00}, /* 0x37 '7' */
    {0x1E,0x33,0x33,0x1E,0x33,0x33,0x1E,0x00}, /* 0x38 '8' */
    {0x1E,0x33,0x33,0x3E,0x30,0x18,0x0E,0x00}, /* 0x39 '9' */
    {0x00,0x0C,0x0C,0x00,0x00,0x0C,0x0C,0x00}, /* 0x3A ':' */
    {0x00,0x0C,0x0C,0x00,0x0C,0x0C,0x06,0x00}, /* 0x3B ';' */
    {0x18,0x0C,0x06,0x03,0x06,0x0C,0x18,0x00}, /* 0x3C '<' */
    {0x00,0x00,0x3F,0x00,0x00,0x3F,0x00,0x00}, /* 0x3D '=' */
    {0x06,0x0C,0x18,0x30,0x18,0x0C,0x06,0x00}, /* 0x3E '>' */
    {0x1E,0x33,0x30,0x18,0x0C,0x00,0x0C,0x00}, /* 0x3F '?' */
    {0x3E,0x63,0x7B,0x7B,0x7B,0x03,0x1E,0x00}, /* 0x40 '@' */
    {0x0C,0x1E,0x33,0x33,0x3F,0x33,0x33,0x00}, /* 0x41 'A' */
    {0x3F,0x66,0x66,0x3E,0x66,0x66,0x3F,0x00}, /* 0x42 'B' */
    {0x3C,0x66,0x03,0x03,0x03,0x66,0x3C,0x00}, /* 0x43 'C' */
    {0x1F,0x36,0x66,0x66,0x66,0x36,0x1F,0x00}, /* 0x44 'D' */
    {0x7F,0x46,0x16,0x1E,0x16,0x46,0x7F,0x00}, /* 0x45 'E' */
    {0x7F,0x46,0x16,0x1E,0x16,0x06,0x0F,0x00}, /* 0x46 'F' */
    {0x3C,0x66,0x03,0x03,0x73,0x66,0x7C,0x00}, /* 0x47 'G' */
    {0x33,0x33,0x33,0x3F,0x33,0x33,0x33,0x00}, /* 0x48 'H' */
    {0x1E,0x0C,0x0C,0x0C,0x0C,0x0C,0x1E,0x00}, /* 0x49 'I' */
    {0x78,0x30,0x30,0x30,0x33,0x33,0x1E,0x00}, /* 0x4A 'J' */
    {0x67,0x66,0x36,0x1E,0x36,0x66,0x67,0x00}, /* 0x4B 'K' */
    {0x0F,0x06,0x06,0x06,0x46,0x66,0x7F,0x00}, /* 0x4C 'L' */
    {0x63,0x77,0x7F,0x7F,0x6B,0x63,0x63,0x00}, /* 0x4D 'M' */
    {0x63,0x67,0x6F,0x7B,0x73,0x63,0x63,0x00}, /* 0x4E 'N' */
    {0x1C,0x36,0x63,0x63,0x63,0x36,0x1C,0x00}, /* 0x4F 'O' */
    {0x3F,0x66,0x66,0x3E,0x06,0x06,0x0F,0x00}, /* 0x50 'P' */
    {0x1E,0x33,0x33,0x33,0x3B,0x1E,0x38,0x00}, /* 0x51 'Q' */
    {0x3F,0x66,0x66,0x3E,0x36,0x66,0x67,0x00}, /* 0x52 'R' */
    {0x1E,0x33,0x07,0x0E,0x38,0x33,0x1E,0x00}, /* 0x53 'S' */
    {0x3F,0x2D,0x0C,0x0C,0x0C,0x0C,0x1E,0x00}, /* 0x54 'T' */
    {0x33,0x33,0x33,0x33,0x33,0x33,0x3F,0x00}, /* 0x55 'U' */
    {0x33,0x33,0x33,0x33,0x33,0x1E,0x0C,0x00}, /* 0x56 'V' */
    {0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00}, /* 0x57 'W' */
    {0x63,0x63,0x36,0x1C,0x1C,0x36,0x63,0x00}, /* 0x58 'X' */
    {0x33,0x33,0x33,0x1E,0x0C,0x0C,0x1E,0x00}, /* 0x59 'Y' */
    {0x7F,0x63,0x31,0x18,0x4C,0x66,0x7F,0x00}, /* 0x5A 'Z' */
    {0x1E,0x06,0x06,0x06,0x06,0x06,0x1E,0x00}, /* 0x5B '[' */
    {0x03,0x06,0x0C,0x18,0x30,0x60,0x40,0x00}, /* 0x5C '\\' */
    {0x1E,0x18,0x18,0x18,0x18,0x18,0x1E,0x00}, /* 0x5D ']' */
    {0x08,0x1C,0x36,0x63,0x00,0x00,0x00,0x00}, /* 0x5E '^' */
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF}, /* 0x5F '_' */
    {0x0C,0x0C,0x18,0x00,0x00,0x00,0x00,0x00}, /* 0x60 '`' */
    {0x00,0x00,0x1E,0x30,0x3E,0x33,0x6E,0x00}, /* 0x61 'a' */
    {0x07,0x06,0x06,0x3E,0x66,0x66,0x3B,0x00}, /* 0x62 'b' */
    {0x00,0x00,0x1E,0x33,0x03,0x33,0x1E,0x00}, /* 0x63 'c' */
    {0x38,0x30,0x30,0x3E,0x33,0x33,0x6E,0x00}, /* 0x64 'd' */
    {0x00,0x00,0x1E,0x33,0x3F,0x03,0x1E,0x00}, /* 0x65 'e' */
    {0x1C,0x36,0x06,0x0F,0x06,0x06,0x0F,0x00}, /* 0x66 'f' */
    {0x00,0x00,0x6E,0x33,0x33,0x3E,0x30,0x1F}, /* 0x67 'g' */
    {0x07,0x06,0x36,0x6E,0x66,0x66,0x67,0x00}, /* 0x68 'h' */
    {0x0C,0x00,0x0E,0x0C,0x0C,0x0C,0x1E,0x00}, /* 0x69 'i' */
    {0x30,0x00,0x30,0x30,0x30,0x33,0x33,0x1E}, /* 0x6A 'j' */
    {0x07,0x06,0x66,0x36,0x1E,0x36,0x67,0x00}, /* 0x6B 'k' */
    {0x0E,0x0C,0x0C,0x0C,0x0C,0x0C,0x1E,0x00}, /* 0x6C 'l' */
    {0x00,0x00,0x33,0x7F,0x7F,0x6B,0x63,0x00}, /* 0x6D 'm' */
    {0x00,0x00,0x1F,0x33,0x33,0x33,0x33,0x00}, /* 0x6E 'n' */
    {0x00,0x00,0x1E,0x33,0x33,0x33,0x1E,0x00}, /* 0x6F 'o' */
    {0x00,0x00,0x3B,0x66,0x66,0x3E,0x06,0x0F}, /* 0x70 'p' */
    {0x00,0x00,0x6E,0x33,0x33,0x3E,0x30,0x78}, /* 0x71 'q' */
    {0x00,0x00,0x3B,0x6E,0x66,0x06,0x0F,0x00}, /* 0x72 'r' */
    {0x00,0x00,0x3E,0x03,0x1E,0x30,0x1F,0x00}, /* 0x73 's' */
    {0x08,0x0C,0x3E,0x0C,0x0C,0x2C,0x18,0x00}, /* 0x74 't' */
    {0x00,0x00,0x33,0x33,0x33,0x33,0x6E,0x00}, /* 0x75 'u' */
    {0x00,0x00,0x33,0x33,0x33,0x1E,0x0C,0x00}, /* 0x76 'v' */
    {0x00,0x00,0x63,0x6B,0x7F,0x7F,0x36,0x00}, /* 0x77 'w' */
    {0x00,0x00,0x63,0x36,0x1C,0x36,0x63,0x00}, /* 0x78 'x' */
    {0x00,0x00,0x33,0x33,0x33,0x3E,0x30,0x1F}, /* 0x79 'y' */
    {0x00,0x00,0x3F,0x19,0x0C,0x26,0x3F,0x00}, /* 0x7A 'z' */
    {0x38,0x0C,0x0C,0x07,0x0C,0x0C,0x38,0x00}, /* 0x7B '{' */
    {0x18,0x18,0x18,0x00,0x18,0x18,0x18,0x00}, /* 0x7C '|' */
    {0x07,0x0C,0x0C,0x38,0x0C,0x0C,0x07,0x00}, /* 0x7D '}' */
    {0x6E,0x3B,0x00,0x00,0x00,0x00,0x00,0x00}, /* 0x7E '~' */
};

/* ============================================================================
 *  LCD UI — 布局常量 & 颜色
 *
 *  屏幕 320x170 横屏布局（swap_color_bytes=true，RGB565 标准格式）：
 *   y=0..15  (16px)  标题栏
 *   y=16     ( 1px)  青色分隔线
 *   y=17..169(153px) 9 行测试项，每行 17px（4 顶+8 字+5 底）
 *
 *  每行水平布局：
 *   x=4..19   序号 "01"（白色）
 *   x=24..119 测试名（浅灰，12字符）
 *   x=199     左边界（深灰竖线）
 *   x=200..319 状态徽标（彩色背景+9字符状态文字，居中于 x=224）
 * ============================================================================ */

#define UI_TITLE_H   16
#define UI_SEP_H      1
#define UI_ROW_H     17
#define UI_ROW_Y0    (UI_TITLE_H + UI_SEP_H)   /* = 17 */
#define UI_TEXT_DY    4    /* 行内字体顶部偏移 */

/* 产测 UI 用 MADCTL：MV 横屏，去掉 MX 避免文字左右镜像 (0x60 含 MX) */
#define FACTORY_LCD_MADCTL  0x20

/* RGB565 颜色（swap_color_bytes=true 下与 lcd.h 宏一致） */
#define UI_C_TITLE_BG  0x000F
#define UI_C_TITLE_FG  0xFFFF
#define UI_C_SEP       0x07FF
#define UI_C_BG_EVEN   0x0000
#define UI_C_BG_ODD    0x10A2
#define UI_C_NUM_FG    0xFFFF
#define UI_C_NAME_FG   0xC618
/* 状态徽标 */
#define UI_C_WAIT_BG   0x2945
#define UI_C_WAIT_FG   0x7BEF
#define UI_C_TEST_BG   0x38A0
#define UI_C_TEST_FG   0xFFE0
#define UI_C_PASS_BG   0x0320
#define UI_C_PASS_FG   0x07E0
#define UI_C_FAIL_BG   0x6000
#define UI_C_FAIL_FG   0xF800

/* 状态字符串（9字符对齐） */
#define STAT_WAIT     "  WAIT   "
#define STAT_TESTING  "[TESTING]"
#define STAT_PASS     " [PASS]  "
#define STAT_FAIL     " [FAIL]  "

/* ============================================================================
 *  LCD UI — 全局状态 & 缓冲
 * ============================================================================ */

typedef enum {
    STATE_WAIT = 0,
    STATE_TESTING,
    STATE_PASS,
    STATE_FAIL,
} test_state_t;

static test_state_t s_test_states[TEST_COUNT];

/* 行像素缓冲，需在内部 DRAM（DMA 可访问） */
static DRAM_ATTR uint16_t s_row_buf[TFT_WIDTH];

/* LCD 显示用 ASCII 短名（12字符对齐） */
static const char * const s_ui_names[TEST_COUNT] = {
    "PSRAM 8MB   ",
    "WS2812B LED ",
    "BUZZER      ",
    "BUTTON GPIO5",
    "ENCODER EC06",
    "BATTERY ADC ",
    "RTC RX8130  ",
    "LCD ST7789  ",
    "WIFI SCAN   ",
};

/* ============================================================================
 *  LCD UI — 像素行渲染工具
 * ============================================================================ */

/* 在 s_row_buf[x] 渲染 ASCII 字符 c 的第 fr 行（0..7）像素 */
static void row_char(int x, int fr, char c, uint16_t fg, uint16_t bg)
{
    if ((uint8_t)c < 0x20 || (uint8_t)c > 0x7E) c = ' ';
    uint8_t bits = s_font8x8[(uint8_t)c - 0x20][fr];
    for (int col = 0; col < 8; col++) {
        int px = x + col;
        if (px >= 0 && px < TFT_WIDTH) {
            s_row_buf[px] = (bits & (0x80u >> col)) ? fg : bg;
        }
    }
}

/* 在 s_row_buf 渲染字符串第 fr 行像素 */
static void row_str(int x, int fr, const char *s, uint16_t fg, uint16_t bg)
{
    while (*s) {
        row_char(x, fr, *s++, fg, bg);
        x += 8;
    }
}

/* 将 s_row_buf 整行发送到 LCD 第 y 行 */
static void ui_flush(int y)
{
    lcd_draw_bitmap(0, (uint16_t)y, TFT_WIDTH - 1, (uint16_t)y, s_row_buf);
    lcd_wait_flush_done();
}

/* ============================================================================
 *  LCD UI — 界面绘制函数
 * ============================================================================ */

/* 绘制标题栏（y=0..15）及青色分隔线（y=16） */
static void ui_draw_title(const char *text, uint16_t bg, uint16_t fg)
{
    int tx = ((int)TFT_WIDTH - (int)strlen(text) * 8) / 2;
    if (tx < 0) tx = 0;

    for (int r = 0; r < UI_TITLE_H; r++) {
        for (int x = 0; x < TFT_WIDTH; x++) s_row_buf[x] = bg;
        int fr = r - UI_TEXT_DY;
        if (fr >= 0 && fr < 8) row_str(tx, fr, text, fg, bg);
        ui_flush(r);
    }
    /* 青色分隔线 */
    for (int x = 0; x < TFT_WIDTH; x++) s_row_buf[x] = UI_C_SEP;
    ui_flush(UI_TITLE_H);
}

/* 根据 s_test_states[idx] 绘制第 idx 行测试条目 */
static void ui_draw_test_row(int idx)
{
    int y0      = UI_ROW_Y0 + idx * UI_ROW_H;
    uint16_t bg = (idx % 2 == 0) ? UI_C_BG_EVEN : UI_C_BG_ODD;

    const char *stat_str;
    uint16_t    badge_bg, badge_fg;
    switch (s_test_states[idx]) {
        case STATE_TESTING:
            stat_str = STAT_TESTING; badge_bg = UI_C_TEST_BG; badge_fg = UI_C_TEST_FG; break;
        case STATE_PASS:
            stat_str = STAT_PASS;    badge_bg = UI_C_PASS_BG; badge_fg = UI_C_PASS_FG; break;
        case STATE_FAIL:
            stat_str = STAT_FAIL;    badge_bg = UI_C_FAIL_BG; badge_fg = UI_C_FAIL_FG; break;
        default:
            stat_str = STAT_WAIT;    badge_bg = UI_C_WAIT_BG; badge_fg = UI_C_WAIT_FG; break;
    }

    char num_str[4];
    snprintf(num_str, sizeof(num_str), "%02d", (idx + 1) % 100);

    for (int r = 0; r < UI_ROW_H; r++) {
        /* 填充行背景 */
        for (int x = 0; x < TFT_WIDTH; x++) s_row_buf[x] = bg;
        /* 徽标背景区 x=200..319 */
        for (int x = 200; x < TFT_WIDTH; x++) s_row_buf[x] = badge_bg;
        /* 徽标左边界（深灰竖线） */
        s_row_buf[199] = 0x4208;

        int fr = r - UI_TEXT_DY;
        if (fr >= 0 && fr < 8) {
            row_str(4,   fr, num_str,         UI_C_NUM_FG,  bg);       /* 序号 */
            row_str(24,  fr, s_ui_names[idx], UI_C_NAME_FG, bg);       /* 名称 */
            row_str(224, fr, stat_str,        badge_fg,     badge_bg); /* 状态 */
        }
        ui_flush(y0 + r);
    }
}

/* 初始化 LCD 并绘制全部测试行（WAIT 状态） */
static void ui_init_screen(void)
{
    for (int i = 0; i < TEST_COUNT; i++) s_test_states[i] = STATE_WAIT;
    lcd_init_with_options(true);
    lcd_set_madctl(FACTORY_LCD_MADCTL);
    lcd_on();
    ui_draw_title("REPPO FACTORY TEST v1.0", UI_C_TITLE_BG, UI_C_TITLE_FG);
    for (int i = 0; i < TEST_COUNT; i++) ui_draw_test_row(i);
}

/* 重绘整个界面（LCD 测试结束后恢复显示用） */
static void ui_redraw_all(void)
{
    ui_draw_title("REPPO FACTORY TEST v1.0", UI_C_TITLE_BG, UI_C_TITLE_FG);
    for (int i = 0; i < TEST_COUNT; i++) ui_draw_test_row(i);
}

/* 测试全部完成后更新标题栏颜色 */
static void ui_show_summary(bool all_pass)
{
    if (all_pass) {
        ui_draw_title(" ALL TESTS PASSED  ", 0x07E0, 0x0000);  /* 绿底黑字 */
    } else {
        ui_draw_title(" SOME TESTS FAILED ", 0xF800, 0xFFFF);  /* 红底白字 */
    }
}

/* ============================================================================
 *  [01] PSRAM 检测
 * ============================================================================ */

static bool test_psram(void)
{
    size_t total   = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
    size_t free_sz = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);

    ESP_LOGI(TAG, "PSRAM total=%u KB  free=%u KB", total / 1024, free_sz / 1024);

    if (total < PSRAM_MIN_BYTES) {
        ESP_LOGE(TAG, "PSRAM 未检测到或容量不足 (%u KB < %u KB)",
                 total / 1024, PSRAM_MIN_BYTES / 1024);
        return false;
    }

    uint8_t *buf = heap_caps_malloc(PSRAM_MIN_BYTES, MALLOC_CAP_SPIRAM);
    if (!buf) {
        ESP_LOGE(TAG, "PSRAM malloc(1MB) 失败");
        return false;
    }
    const uint8_t PATTERN = 0xA5;
    memset(buf, PATTERN, PSRAM_MIN_BYTES);
    bool ok = (buf[0] == PATTERN) && (buf[PSRAM_MIN_BYTES - 1] == PATTERN);
    heap_caps_free(buf);

    if (!ok) {
        ESP_LOGE(TAG, "PSRAM 读写校验失败");
        return false;
    }

    ESP_LOGI(TAG, "PSRAM %.1f MB  OK", total / 1024.0f / 1024.0f);
    return true;
}

/* ============================================================================
 *  [02] WS2812B LED 测试
 * ============================================================================ */

static bool test_leds(void)
{
    const struct { uint32_t r, g, b; const char *name; } seq[] = {
        {80,  0,  0, "红"},
        { 0, 80,  0, "绿"},
        { 0,  0, 80, "蓝"},
        {50, 50, 50, "白"},
        { 0,  0,  0, "关"},
    };

    for (int i = 0; i < (int)(sizeof(seq)/sizeof(seq[0])); i++) {
        ESP_LOGI(TAG, "LED -> %s", seq[i].name);
        set_flow_light(seq[i].r, seq[i].g, seq[i].b);
        DELAY_MS(700);
    }
    return true;
}

/* ============================================================================
 *  [03] 蜂鸣器测试
 * ============================================================================ */

static bool test_buzzer(void)
{
    const struct { uint32_t freq; const char *desc; } tones[] = {
        { 500, "500 Hz (低频)"},
        {2000, "2000 Hz (中频)"},
        {4000, "4000 Hz (高频)"},
    };

    for (int i = 0; i < (int)(sizeof(tones)/sizeof(tones[0])); i++) {
        ESP_LOGI(TAG, "蜂鸣器: %s", tones[i].desc);
        buzzer_tone(tones[i].freq, 350);
        DELAY_MS(200);
    }
    return true;
}

/* ============================================================================
 *  [04] 按键测试（GPIO5，主动低）
 * ============================================================================ */

static bool test_button(void)
{
    keypad_io_cfg(GPIO_NUM_5);

    ESP_LOGI(TAG, ">>> 请在 %d 秒内按下 GPIO5 按键 <<<", BTN_TIMEOUT_MS / 1000);

    int elapsed = 0;
    while (elapsed < BTN_TIMEOUT_MS) {
        if (keypad_get_key_value(GPIO_NUM_5)) {
            ESP_LOGI(TAG, "按键: 检测到按下！(已等待 %d ms)", elapsed);
            DELAY_MS(200);
            return true;
        }
        DELAY_MS(POLL_INTERVAL_MS);
        elapsed += POLL_INTERVAL_MS;

        if (elapsed % 2000 < POLL_INTERVAL_MS) {
            ESP_LOGI(TAG, "按键: 等待中... 剩余 %d s", (BTN_TIMEOUT_MS - elapsed) / 1000);
        }
    }

    ESP_LOGE(TAG, "按键: 超时，未检测到按下！");
    return false;
}

/* ============================================================================
 *  [05] 编码器测试（EC06，GPIO1/GPIO2）
 * ============================================================================ */

static bool test_encoder(void)
{
    ESP_LOGI(TAG, ">>> 请在 %d 秒内向任意方向旋转 EC06 编码器 <<<", ENC_TIMEOUT_MS / 1000);

    int elapsed = 0;
    while (elapsed < ENC_TIMEOUT_MS) {
        uint8_t dir = check_encoder_direction();
        if (dir != 0) {
            ESP_LOGI(TAG, "编码器: 检测到旋转！方向=%s  (已等待 %d ms)",
                     (dir == 1) ? "顺时针 CW" : "逆时针 CCW", elapsed);
            return true;
        }
        DELAY_MS(POLL_INTERVAL_MS);
        elapsed += POLL_INTERVAL_MS;

        if (elapsed % 3000 < POLL_INTERVAL_MS) {
            ESP_LOGI(TAG, "编码器: 等待旋转... 剩余 %d s", (ENC_TIMEOUT_MS - elapsed) / 1000);
        }
    }

    ESP_LOGE(TAG, "编码器: 超时，未检测到旋转！");
    return false;
}

/* ============================================================================
 *  [06] 电池 ADC & TP4056 充电状态检测
 * ============================================================================ */

static bool test_battery(void)
{
    DELAY_MS(300);

    battery_info_t info;
    battery_monitor_get(&info);

    ESP_LOGI(TAG, "电池: 电压=%d mV  估算=%d%%  显示=%d%%",
             info.voltage_mv, info.estimated_percent, info.display_percent);
    ESP_LOGI(TAG, "电池: 充电=%s  已满=%s  已插入=%s",
             info.charging ? "是" : "否",
             info.full     ? "是" : "否",
             info.plugged  ? "是" : "否");

    int chrg = gpio_get_level(BAT_CHARGE_GPIO);
    int full = gpio_get_level(BAT_FULL_GPIO);
    ESP_LOGI(TAG, "TP4056: CHRG(GPIO%d)=%d  STDBY(GPIO%d)=%d",
             BAT_CHARGE_GPIO, chrg, BAT_FULL_GPIO, full);

    if (info.voltage_mv < BAT_VOLTAGE_MIN || info.voltage_mv > BAT_VOLTAGE_MAX) {
        ESP_LOGE(TAG, "电池电压异常: %d mV (期望 %d ~ %d mV)",
                 info.voltage_mv, BAT_VOLTAGE_MIN, BAT_VOLTAGE_MAX);
        return false;
    }

    return true;
}

/* ============================================================================
 *  [07] RTC RX8130 I2C 检测
 * ============================================================================ */

static bool test_rtc(void)
{
    esp_err_t ret = rx8130_probe(RX8130_I2C_PORT);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "RTC: 未检测到 RX8130 (I2C addr=0x%02X)  err=%s",
                 RX8130_ADDR, esp_err_to_name(ret));
        return false;
    }

    rx8130_time_t t;
    ret = rx8130_get_time(RX8130_I2C_PORT, &t);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "RTC: 读取时间失败  err=%s", esp_err_to_name(ret));
        return false;
    }

    ESP_LOGI(TAG, "RTC: %04d-%02d-%02d  %02d:%02d:%02d",
             t.year, t.month, t.date, t.hour, t.minute, t.second);

    if (t.year < 2020 || t.year > 2099) {
        ESP_LOGW(TAG, "RTC: 年份异常(%d)，晶振可能未起振或时间未同步（但器件通信 OK）", t.year);
    }

    return true;
}

/* ============================================================================
 *  [08] LCD ST7789P3 视觉测试（纯色 + 八色彩条）
 *  LCD 已在 ui_init_screen 初始化，此处仅做视觉检验，不重初始化 I80 总线
 * ============================================================================ */

static void lcd_draw_color_bars(void)
{
    static const uint16_t bar_colors[8] = {
        WHITE, YELLOW, CYAN, GREEN, MAGENTA, RED, BLUE, BLACK
    };
    const int bar_w = TFT_WIDTH / 8;

    uint16_t row[TFT_WIDTH];
    for (int x = 0; x < TFT_WIDTH; x++) {
        int idx = x / bar_w;
        row[x] = bar_colors[(idx < 8) ? idx : 7];
    }

    for (int y = 0; y < TFT_HEIGHT; y++) {
        lcd_draw_bitmap(0, y, TFT_WIDTH - 1, y, row);
        lcd_wait_flush_done();
    }
}

static bool test_lcd(void)
{
    ESP_LOGI(TAG, "LCD: 全屏纯色 + 八色彩条视觉测试...");
    lcd_set_madctl(FACTORY_LCD_MADCTL);
    DELAY_MS(100);

    const struct { uint16_t color; const char *name; } fills[] = {
        {RED,    "红"},
        {GREEN,  "绿"},
        {BLUE,   "蓝"},
        {WHITE,  "白"},
        {YELLOW, "黄"},
        {CYAN,   "青"},
        {BLACK,  "黑"},
    };
    for (int i = 0; i < (int)(sizeof(fills)/sizeof(fills[0])); i++) {
        ESP_LOGI(TAG, "LCD: 全屏 -> %s", fills[i].name);
        lcd_clear(fills[i].color);
        DELAY_MS(500);
    }

    ESP_LOGI(TAG, "LCD: 绘制八色彩条...");
    lcd_draw_color_bars();
    DELAY_MS(2000);

    lcd_clear(BLACK);
    return true;
}

/* ============================================================================
 *  [09] WiFi 扫描测试
 * ============================================================================ */

static bool test_wifi(void)
{
    ESP_LOGI(TAG, "WiFi: 初始化 STA 模式...");
    init_wifi();
    DELAY_MS(500);

    ESP_LOGI(TAG, "WiFi: 开始扫描 AP...");
    wifi_scan_top_5();

    const int SCAN_TIMEOUT_MS = 6000;
    int wait = 0;
    while (g_scan_ap_num == 0 && wait < SCAN_TIMEOUT_MS) {
        DELAY_MS(300);
        wait += 300;
    }

    ESP_LOGI(TAG, "WiFi: 扫描完成，发现 %d 个 AP:", g_scan_ap_num);
    for (int i = 0; i < g_scan_ap_num; i++) {
        ESP_LOGI(TAG, "  [%d] %-32s  RSSI=%d dBm",
                 i + 1, g_scan_results[i].ssid, g_scan_results[i].rssi);
    }

    if (g_scan_ap_num == 0) {
        ESP_LOGE(TAG, "WiFi: 未扫描到任何 AP，请确认天线已焊接且周围有无线信号");
        return false;
    }

    return true;
}

/* ============================================================================
 *  汇总输出（串口）
 * ============================================================================ */

static void print_summary(bool all_pass)
{
    printf("\n");
    printf("+--------------------------------------------+\n");
    printf("|    Reppo R1 PCB 出厂功能测试结果汇总        |\n");
    printf("+----+---------------+----------------------+\n");
    printf("| 序  | 测试项目       | 结果                 |\n");
    printf("+----+---------------+----------------------+\n");
    for (int i = 0; i < TEST_COUNT; i++) {
        printf("| %02d | %s | %-20s |\n",
               i + 1,
               s_test_names[i],
               s_results[i] ? "PASS  (通过)" : "FAIL  (失败)");
    }
    printf("+----+---------------+----------------------+\n");
    printf("|  总体: %-36s |\n",
           all_pass ? "全部通过 >>> PASS <<<" : "有失败项 >>> FAIL <<<");
    printf("+--------------------------------------------+\n\n");

    if (all_pass) {
        led_green();
        buzzer_tone(2000, 200);
        DELAY_MS(100);
        buzzer_tone(2000, 200);
        ESP_LOGI(TAG, ">>> 测试完成：全部通过 <<<");
    } else {
        led_red();
        for (int i = 0; i < 3; i++) {
            buzzer_tone(500, 200);
            DELAY_MS(100);
        }
        ESP_LOGE(TAG, ">>> 测试完成：有失败项，请检查上方日志 <<<");
    }
}

/* ============================================================================
 *  主函数
 * ============================================================================ */

void app_main(void)
{
    esp_chip_info_t chip;
    esp_chip_info(&chip);
    uint32_t flash_mb = 0;
    esp_flash_get_size(NULL, &flash_mb);
    flash_mb /= (1024 * 1024);

    printf("\n");
    printf("========================================\n");
    printf("  Reppo R1  PCB 出厂功能测试  v1.1\n");
    printf("  芯片: ESP32-S3  核心: %d  Rev: %d\n", chip.cores, chip.revision);
    printf("  Flash: %lu MB   PSRAM: %lu MB\n",
           (unsigned long)flash_mb,
           (unsigned long)(heap_caps_get_total_size(MALLOC_CAP_SPIRAM) / 1024 / 1024));
    printf("========================================\n\n");

    /* NVS 初始化 */
    esp_err_t nvs_ret = nvs_flash_init();
    if (nvs_ret == ESP_ERR_NVS_NO_FREE_PAGES || nvs_ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    /* 早期硬件初始化 */
    init_led_strip();
    buzzer_init();
    battery_adc_init();
    init_encoder_direction();
    rx8130_init(RX8130_I2C_PORT, RX8130_I2C_SDA_PIN, RX8130_I2C_SCL_PIN);

    /* 初始化 LCD，绘制测试进度界面（全部 WAIT） */
    ui_init_screen();

    /* 蓝灯闪烁 3 次，提示即将开始 */
    for (int i = 0; i < 3; i++) {
        led_blue(); DELAY_MS(300);
        led_off();  DELAY_MS(200);
    }
    DELAY_MS(500);

    /* ----------------------------------------------------------------
     *  逐项测试
     * ---------------------------------------------------------------- */
    typedef bool (*test_fn_t)(void);
    const test_fn_t tests[TEST_COUNT] = {
        test_psram,
        test_leds,
        test_buzzer,
        test_button,
        test_encoder,
        test_battery,
        test_rtc,
        test_lcd,
        test_wifi,
    };

    bool all_pass = true;

    for (int i = 0; i < TEST_COUNT; i++) {
        printf("\n----- [%02d/%02d] %s -----\n", i + 1, TEST_COUNT, s_test_names[i]);
        led_blue();

        /* UI: 标记为测试中 */
        s_test_states[i] = STATE_TESTING;
        ui_draw_test_row(i);

        s_results[i] = tests[i]();
        all_pass = all_pass && s_results[i];

        /* UI: 更新结果 */
        s_test_states[i] = s_results[i] ? STATE_PASS : STATE_FAIL;

        if (i == 7) {
            /* LCD 测试会清屏，完成后重绘整个界面 */
            ui_redraw_all();
        } else {
            ui_draw_test_row(i);
        }

        if (s_results[i]) {
            printf("  --> PASS (通过)\n");
            led_green();
        } else {
            printf("  --> FAIL (失败)\n");
            led_red();
        }
        DELAY_MS(600);
    }

    /* ----------------------------------------------------------------
     *  结果汇总
     * ---------------------------------------------------------------- */
    print_summary(all_pass);
    ui_show_summary(all_pass);  /* 标题栏变色显示总体结果 */

    /* 保持结果灯常亮，循环等待 */
    while (1) {
        if (!all_pass) {
            led_red();   DELAY_MS(1500);
            led_off();   DELAY_MS(500);
        } else {
            DELAY_MS(2000);
        }
    }
}
