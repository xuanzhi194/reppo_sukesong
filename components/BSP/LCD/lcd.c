#include "lcd.h"
#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_commands.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

static const char *TAG = "lcd_i80";

static esp_lcd_i80_bus_handle_t  s_i80_bus   = NULL;
static esp_lcd_panel_io_handle_t s_io_handle = NULL;
static esp_lcd_panel_handle_t    s_panel     = NULL;

static lcd_flush_done_cb_t s_flush_done_cb       = NULL;
static void               *s_flush_done_user_ctx = NULL;
static volatile bool       s_trans_done          = false;
static uint32_t            s_dma_timeout_count   = 0;
static uint32_t            s_draw_count          = 0;
static bool                s_data_bus_reversed   = false;
static bool                s_dc_inverted         = false;
static bool                s_bgr_order           = false;

#define LCD_LINE_BUF_MAX  320  /* max(TFT_WIDTH, ST7789_RAM_H) */

/* DMA 完成 ISR 中触发. 转给上层 (lvgl flush_ready 等). */
static bool IRAM_ATTR on_color_trans_done(esp_lcd_panel_io_handle_t panel_io,
                                          esp_lcd_panel_io_event_data_t *edata,
                                          void *user_ctx)
{
    (void)user_ctx;
    s_trans_done = true;
    if (s_flush_done_cb) {
        return s_flush_done_cb(panel_io, edata, s_flush_done_user_ctx);
    }
    return false;
}

static void lcd_wait_trans_done(void)
{
    const TickType_t t0 = xTaskGetTickCount();
    while (!s_trans_done) {
        if ((xTaskGetTickCount() - t0) > pdMS_TO_TICKS(200)) {
            s_dma_timeout_count++;
            ESP_LOGW(TAG, "i80 DMA timeout (total=%lu)", (unsigned long)s_dma_timeout_count);
            s_trans_done = true;
            return;
        }
        taskYIELD();
    }
}

void lcd_register_flush_done_cb(lcd_flush_done_cb_t cb, void *user_ctx)
{
    s_flush_done_cb       = cb;
    s_flush_done_user_ctx = user_ctx;
}

/* ---------- 背光 ---------- */
void lcd_on(void)  { gpio_set_level(LCD_PIN_BLK, 1); }
void lcd_off(void) { gpio_set_level(LCD_PIN_BLK, 0); }

/* datasheet: 8080 只写时 RD(Pin9) 保持无效(高) */
static void lcd_gpio_8080_idle(void)
{
    const gpio_config_t out_cfg = {
        .mode         = GPIO_MODE_OUTPUT,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    gpio_config_t rd = out_cfg;
    rd.pin_bit_mask = 1ULL << LCD_PIN_RD;
    ESP_ERROR_CHECK(gpio_config(&rd));
    gpio_set_level(LCD_PIN_RD, 1);

    /* 手动复位脉冲 (便于示波器确认 RST 引脚) */
    gpio_config_t rst = out_cfg;
    rst.pin_bit_mask = 1ULL << LCD_PIN_RST;
    ESP_ERROR_CHECK(gpio_config(&rst));
    gpio_set_level(LCD_PIN_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(LCD_PIN_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(120));
}

static void lcd_panel_force_on(void)
{
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(s_io_handle, LCD_CMD_SLPOUT, NULL, 0));
    vTaskDelay(pdMS_TO_TICKS(120));
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(s_io_handle, LCD_CMD_NORON, NULL, 0));
    vTaskDelay(pdMS_TO_TICKS(20));
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(s_io_handle, LCD_CMD_DISPON, NULL, 0));
    vTaskDelay(pdMS_TO_TICKS(20));
}

/* ---------- 位图绘制 ---------- */
void lcd_draw_bitmap_async(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t *img)
{
    s_trans_done = false;
    s_draw_count++;
    esp_lcd_panel_draw_bitmap(s_panel, x1, y1, (int)x2 + 1, (int)y2 + 1, img);
}

void lcd_wait_flush_done(void)
{
    lcd_wait_trans_done();
}

void lcd_draw_bitmap(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t *img)
{
    lcd_draw_bitmap_async(x1, y1, x2, y2, img);
    lcd_wait_flush_done();
}

/* DMA 可访问的内部 RAM 行缓冲 */
static uint16_t s_line_buf[LCD_LINE_BUF_MAX] DRAM_ATTR;

static void lcd_clear_area(uint16_t w, uint16_t h, uint16_t color)
{
    if (w > LCD_LINE_BUF_MAX) {
        w = LCD_LINE_BUF_MAX;
    }
    for (int i = 0; i < w; i++) {
        s_line_buf[i] = color;
    }
    for (int y = 0; y < h; y++) {
        lcd_draw_bitmap(0, y, w - 1, y, s_line_buf);
    }
}

/* ---------- 清屏 (开机用) ---------- */
void lcd_clear(uint16_t color)
{
    lcd_clear_area(TFT_WIDTH, TFT_HEIGHT, color);
}

#if LCD_BAREMETAL_SCAN_PROFILES
static void lcd_bruteforce_one(uint8_t madctl, uint16_t w, uint16_t h, uint16_t color)
{
    ESP_ERROR_CHECK(esp_lcd_panel_set_gap(s_panel, 0, 0));
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(s_io_handle, LCD_CMD_MADCTL, &madctl, 1));
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(s_io_handle, LCD_CMD_INVON, NULL, 0));
    lcd_clear_area(w, h, color);
}

static void lcd_run_bruteforce_matrix(void)
{
    static const uint8_t madctls[] = {0x00, 0x60, 0x70, 0xA0, 0xC0, 0x68};
    static const struct {
        uint16_t w;
        uint16_t h;
        const char *tag;
    } sizes[] = {
        {ST7789_RAM_H, ST7789_RAM_V, "240x320"},
        {TFT_WIDTH, TFT_HEIGHT, "320x170"},
        {170, 320, "170x320"},
    };

    ESP_LOGW(TAG, "若全部仍黑屏: 用万用表测 FPC Pin3(IM2)/Pin4(IM1) 必须接地(8080模式)");
    ESP_LOGW(TAG, "并确认 WR/DC/CS/RST/DB 实焊与原理图一致");

    while (1) {
        for (int dc = 0; dc < 2; dc++) {
            for (int swap = 0; swap < 2; swap++) {
                for (int bgr = 0; bgr < 2; bgr++) {
                    for (int br = 0; br < 2; br++) {
                        s_data_bus_reversed = (br != 0);
                        lcd_deinit();
                        lcd_init_full((bool)swap, (bool)dc, (bool)bgr);
                        lcd_panel_force_on();
                        ESP_LOGI(TAG, "=== dc_inv=%d swap_bytes=%d bgr=%d bus_rev=%d dma_to=%lu ===",
                                 dc, swap, bgr, br, (unsigned long)s_dma_timeout_count);

                        for (int mi = 0; mi < (int)(sizeof(madctls) / sizeof(madctls[0])); mi++) {
                            for (int si = 0; si < (int)(sizeof(sizes) / sizeof(sizes[0])); si++) {
                                ESP_LOGI(TAG, ">>> MADCTL=0x%02X %s RED 3s",
                                         madctls[mi], sizes[si].tag);
                                lcd_bruteforce_one(madctls[mi], sizes[si].w, sizes[si].h, RED);
                                vTaskDelay(pdMS_TO_TICKS(3000));
                            }
                        }
                    }
                }
            }
        }
    }
}
#endif /* LCD_BAREMETAL_SCAN_PROFILES */

/* ---------- 裸机测试图案 ---------- */
static void lcd_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    if (w > LCD_LINE_BUF_MAX) {
        w = LCD_LINE_BUF_MAX;
    }
    for (int i = 0; i < w; i++) {
        s_line_buf[i] = color;
    }
    for (int row = 0; row < h; row++) {
        int yy = y + row;
        if (yy >= TFT_HEIGHT) {
            break;
        }
        lcd_draw_bitmap(x, yy, x + w - 1, yy, s_line_buf);
    }
}

static void lcd_test_color_bars(void)
{
    static const uint16_t colors[] = {RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, WHITE, BLACK};
    const int bar_h = TFT_HEIGHT / 8;
    if (bar_h < 1) {
        return;
    }
    ESP_LOGI(TAG, "Pattern: 8 color bars");
    for (int i = 0; i < 8; i++) {
        lcd_fill_rect(0, i * bar_h, TFT_WIDTH, bar_h, colors[i]);
    }
}

static void lcd_test_checkerboard(void)
{
    const int cell = 20;
    ESP_LOGI(TAG, "Pattern: checkerboard");
    for (int y = 0; y < TFT_HEIGHT; y++) {
        for (int x = 0; x < TFT_WIDTH; x++) {
            bool on = (((x / cell) + (y / cell)) & 1) != 0;
            s_line_buf[x] = on ? WHITE : BLACK;
        }
        lcd_draw_bitmap(0, y, TFT_WIDTH - 1, y, s_line_buf);
    }
}

static void lcd_test_cross(void)
{
    ESP_LOGI(TAG, "Pattern: red + white cross + green/blue blocks");
    lcd_clear(RED);
    lcd_fill_rect(TFT_WIDTH / 2 - 4, 0, 8, TFT_HEIGHT, WHITE);
    lcd_fill_rect(0, TFT_HEIGHT / 2 - 4, TFT_WIDTH, 8, WHITE);
    lcd_fill_rect(10, 10, 80, 60, GREEN);
    lcd_fill_rect(TFT_WIDTH - 90, TFT_HEIGHT - 70, 80, 60, BLUE);
}

static void lcd_test_solid(uint16_t color, const char *name, int hold_ms)
{
    ESP_LOGI(TAG, ">>> Fill %s (%d ms), dma_to=%lu", name, hold_ms,
             (unsigned long)s_dma_timeout_count);
    lcd_clear(color);
    vTaskDelay(pdMS_TO_TICKS(hold_ms));
}

static void lcd_test_backlight_blink(void)
{
    gpio_config_t bk = {
        .pin_bit_mask = 1ULL << LCD_PIN_BLK,
        .mode         = GPIO_MODE_OUTPUT,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&bk));
    ESP_LOGW(TAG, ">>> 背光测试: GPIO%d 闪烁 10 次 (高电平=开) <<<", LCD_PIN_BLK);
    for (int i = 0; i < 10; i++) {
        gpio_set_level(LCD_PIN_BLK, i & 1);
        ESP_LOGI(TAG, "  BLK %s", (i & 1) ? "HIGH(on)" : "LOW(off)");
        vTaskDelay(pdMS_TO_TICKS(400));
    }
    gpio_set_level(LCD_PIN_BLK, 1);
    ESP_LOGI(TAG, "Backlight held HIGH. 若背光无变化 -> 查 GPIO3/SS8050/LED 供电");
    vTaskDelay(pdMS_TO_TICKS(2000));
}

/* ========== GPIO 直驱 8080 (绕过 esp_lcd/DMA, 硬件排查) ========== */
#if LCD_TEST_USE_BITBANG

typedef struct {
    const char *name;
    gpio_num_t cs;
    gpio_num_t rst;
    gpio_num_t dc;
    gpio_num_t wr;
    gpio_num_t rd;
    gpio_num_t db[8];
} bb_pin_profile_t;

static bb_pin_profile_t s_bb;

static const bb_pin_profile_t s_bb_profiles[] = {
    {
        "schematic",
        LCD_PIN_CS, LCD_PIN_RST, LCD_PIN_DC, LCD_PIN_WR, LCD_PIN_RD,
        { LCD_PIN_DB0, LCD_PIN_DB1, LCD_PIN_DB2, LCD_PIN_DB3,
          LCD_PIN_DB4, LCD_PIN_DB5, LCD_PIN_DB6, LCD_PIN_DB7 },
    },
    {
        "legacy_spi_pins",
        GPIO_NUM_46, GPIO_NUM_3, GPIO_NUM_9, GPIO_NUM_10, GPIO_NUM_47,
        { GPIO_NUM_12, GPIO_NUM_13, GPIO_NUM_14, GPIO_NUM_21,
          GPIO_NUM_47, GPIO_NUM_48, GPIO_NUM_45, GPIO_NUM_38 },
    },
    {
        "schematic_cs39_rst40",
        GPIO_NUM_39, GPIO_NUM_40, GPIO_NUM_45, GPIO_NUM_48, GPIO_NUM_47,
        { GPIO_NUM_21, GPIO_NUM_14, GPIO_NUM_13, GPIO_NUM_12,
          GPIO_NUM_11, GPIO_NUM_10, GPIO_NUM_9, GPIO_NUM_46 },
    },
};

static gpio_num_t s_bb_db_rev[8];
static bool s_bb_bus_rev = false;
static bool s_bb_dc_inv = false;

static const gpio_num_t *bb_db_map(void)
{
    return s_bb_bus_rev ? s_bb_db_rev : s_bb.db;
}

static void bb_db_set_output(void)
{
    const gpio_num_t *db = bb_db_map();
    for (int i = 0; i < 8; i++) {
        gpio_set_direction(db[i], GPIO_MODE_OUTPUT);
    }
}

static void bb_db_set_input(void)
{
    const gpio_num_t *db = bb_db_map();
    for (int i = 0; i < 8; i++) {
        gpio_set_direction(db[i], GPIO_MODE_INPUT);
    }
}

static void bb_wr_pulse(void)
{
    /* ST7789P3 8080: WRX 空闲高, 上升沿锁存; 加长 setup/hold 便于排查虚焊 */
    esp_rom_delay_us(2);
    gpio_set_level(s_bb.wr, 0);
    esp_rom_delay_us(2);
    gpio_set_level(s_bb.wr, 1);
    esp_rom_delay_us(2);
}

static void bb_rd_pulse_read(uint8_t *out)
{
    gpio_set_level(s_bb.wr, 1);
    gpio_set_level(s_bb.rd, 1);
    esp_rom_delay_us(1);
    gpio_set_level(s_bb.rd, 0);
    esp_rom_delay_us(1);
    const gpio_num_t *db = bb_db_map();
    uint8_t v = 0;
    for (int i = 0; i < 8; i++) {
        if (gpio_get_level(db[i])) {
            v |= (uint8_t)(1U << i);
        }
    }
    gpio_set_level(s_bb.rd, 1);
    esp_rom_delay_us(1);
    if (out) {
        *out = v;
    }
}

static void bb_write_bus(uint8_t byte)
{
    const gpio_num_t *db = bb_db_map();
    for (int i = 0; i < 8; i++) {
        gpio_set_level(db[i], (byte >> i) & 1);
    }
    bb_wr_pulse();
}

/* CS 全程拉低, 符合 ST7789 8080 多字节写时序 */
static void bb_write_cmd_burst(uint8_t cmd)
{
    bb_db_set_output();
    gpio_set_level(s_bb.wr, 1);
    gpio_set_level(s_bb.rd, 1);
    gpio_set_level(s_bb.cs, 0);
    gpio_set_level(s_bb.dc, s_bb_dc_inv ? 1 : 0);
    bb_write_bus(cmd);
}

static void bb_write_data_burst(uint8_t data)
{
    gpio_set_level(s_bb.dc, s_bb_dc_inv ? 0 : 1);
    bb_write_bus(data);
}

static void bb_burst_end(void)
{
    gpio_set_level(s_bb.cs, 1);
}

static void bb_write_cmd(uint8_t cmd)
{
    bb_write_cmd_burst(cmd);
    bb_burst_end();
}

static void bb_write_cmd_params(uint8_t cmd, const uint8_t *params, size_t len)
{
    /* 部分模组要求 CS 在 cmd 与 param 之间拉高 */
    bb_write_cmd(cmd);
    if (len == 0) {
        return;
    }
    bb_db_set_output();
    gpio_set_level(s_bb.wr, 1);
    gpio_set_level(s_bb.rd, 1);
    gpio_set_level(s_bb.cs, 0);
    gpio_set_level(s_bb.dc, s_bb_dc_inv ? 0 : 1);
    for (size_t i = 0; i < len; i++) {
        bb_write_bus(params[i]);
    }
    bb_burst_end();
}

static bool bb_read_rddid(uint8_t id[3])
{
    bb_db_set_output();
    gpio_set_level(s_bb.wr, 1);
    gpio_set_level(s_bb.rd, 1);
    gpio_set_level(s_bb.cs, 0);
    gpio_set_level(s_bb.dc, s_bb_dc_inv ? 1 : 0);
    bb_write_bus(0x04);
    gpio_set_level(s_bb.dc, s_bb_dc_inv ? 0 : 1);

    bb_db_set_input();
    uint8_t dummy = 0;
    bb_rd_pulse_read(&dummy);
    (void)dummy;
    for (int i = 0; i < 3; i++) {
        bb_rd_pulse_read(&id[i]);
    }
    bb_db_set_output();
    bb_burst_end();
    return (id[0] != 0x00 && id[0] != 0xFF) ||
           (id[1] != 0x00 && id[1] != 0xFF) ||
           (id[2] != 0x00 && id[2] != 0xFF);
}

/* ST7789P3 8080-I8 读: 1 dummy + 常为 cmd 回显 + 真实数据 (RDDID 04h 除外) */
static bool bb_read_reg_raw(uint8_t cmd, uint8_t *raw, size_t raw_len)
{
    if (raw_len == 0) {
        return false;
    }
    bb_db_set_output();
    gpio_set_level(s_bb.wr, 1);
    gpio_set_level(s_bb.rd, 1);
    gpio_set_level(s_bb.cs, 0);
    gpio_set_level(s_bb.dc, s_bb_dc_inv ? 1 : 0);
    bb_write_bus(cmd);
    gpio_set_level(s_bb.dc, s_bb_dc_inv ? 0 : 1);

    bb_db_set_input();
    uint8_t dummy = 0;
    bb_rd_pulse_read(&dummy);
    (void)dummy;
    bool any = false;
    for (size_t i = 0; i < raw_len; i++) {
        bb_rd_pulse_read(&raw[i]);
        if (raw[i] != 0x00 && raw[i] != 0xFF) {
            any = true;
        }
    }
    bb_db_set_output();
    bb_burst_end();
    return any;
}

static bool bb_read_reg8(uint8_t cmd, uint8_t *val)
{
    uint8_t raw[6] = {0};
    bb_read_reg_raw(cmd, raw, sizeof(raw));
    for (size_t i = 0; i < sizeof(raw); i++) {
        if (raw[i] != 0x00 && raw[i] != 0xFF && raw[i] != cmd) {
            *val = raw[i];
            return true;
        }
    }
    *val = raw[0];
    return false;
}

static bool bb_read_reg_bytes(uint8_t cmd, uint8_t *buf, size_t len)
{
    if (len == 0) {
        return false;
    }
    uint8_t raw[8] = {0};
    const size_t n = len + 2;
    if (n > sizeof(raw)) {
        return false;
    }
    if (!bb_read_reg_raw(cmd, raw, n)) {
        memcpy(buf, raw, len);
        return false;
    }
    const size_t off = (raw[0] == cmd) ? 1 : 0;
    memcpy(buf, &raw[off], len);
    return true;
}

static void bb_log_chip_probe(void)
{
    uint8_t id[3] = {0};
    uint8_t id1 = 0, id2 = 0, id3 = 0;
    bool ok_rddid = bb_read_rddid(id);
    bool ok1 = bb_read_reg8(0xDA, &id1);
    bool ok2 = bb_read_reg8(0xDB, &id2);
    bool ok3 = bb_read_reg8(0xDC, &id3);

    ESP_LOGW(TAG, "ST7789P3 ID probe (dc_inv=%d bus_rev=%d):", (int)s_bb_dc_inv, (int)s_bb_bus_rev);
    ESP_LOGW(TAG, "  RDDID(04h): %02X %02X %02X %s",
             id[0], id[1], id[2], ok_rddid ? "OK" : "FAIL(all 0/FF?)");
    ESP_LOGW(TAG, "  RDID1/2/3: DA=%02X DB=%02X DC=%02X (P3 expect 81 81 B3)",
             id1, id2, id3);
    if (id[0] == 0x81 && id[1] == 0x81 && (id[2] == 0xB3 || id[2] == 0x83)) {
        ESP_LOGI(TAG, "  -> ST7789P3 ID match (81 81 %02X)", id[2]);
    } else if (!ok_rddid && !ok1 && !ok2 && !ok3) {
        ESP_LOGE(TAG, "  -> 8080 读无有效 ID: 查 RD(GPIO%d)焊接、DB方向、屏 VCI/IOVCC 供电",
                 (int)s_bb.rd);
    } else {
        ESP_LOGI(TAG, "  -> 屏控制器有响应, 问题在 init/刷屏参数");
    }
}

static void bb_apply_profile(const bb_pin_profile_t *p)
{
    s_bb = *p;
    for (int i = 0; i < 8; i++) {
        s_bb_db_rev[i] = s_bb.db[7 - i];
    }
}

static void bb_gpio_init(void)
{
    uint64_t mask = (1ULL << LCD_PIN_BLK);
    const gpio_num_t ctrls[] = {
        s_bb.cs, s_bb.rst, s_bb.dc, s_bb.wr, s_bb.rd,
        GPIO_NUM_3, GPIO_NUM_38, GPIO_NUM_39, GPIO_NUM_40, GPIO_NUM_46,
    };
    for (int i = 0; i < (int)(sizeof(ctrls) / sizeof(ctrls[0])); i++) {
        mask |= (1ULL << ctrls[i]);
    }
    for (int i = 0; i < 8; i++) {
        mask |= (1ULL << s_bb.db[i]);
    }
    gpio_config_t io = {
        .pin_bit_mask = mask,
        .mode         = GPIO_MODE_OUTPUT,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&io));
    gpio_set_level(s_bb.cs, 1);
    gpio_set_level(s_bb.rd, 1);
    gpio_set_level(s_bb.wr, 1);
    gpio_set_level(s_bb.rst, 1);
}

static void bb_hw_reset(void)
{
    gpio_set_level(s_bb.rst, 0);
    vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(s_bb.rst, 1);
    vTaskDelay(pdMS_TO_TICKS(120));
}

static void bb_vendor_init_ex(uint8_t madctl, uint8_t colmod, bool inv_on)
{
    bb_db_set_output();
    bb_write_cmd(0x11);
    vTaskDelay(pdMS_TO_TICKS(120));

    bb_write_cmd_params(0xB2, (uint8_t[]){0x0C, 0x0C, 0x00, 0x33, 0x33}, 5);
    bb_write_cmd_params(0x35, (uint8_t[]){0x00}, 1);
    bb_write_cmd_params(0x36, (uint8_t[]){madctl}, 1);
    bb_write_cmd_params(0x3A, (uint8_t[]){colmod}, 1);
    bb_write_cmd_params(0xB7, (uint8_t[]){0x35}, 1);
    bb_write_cmd_params(0xBB, (uint8_t[]){0x2D}, 1);
    bb_write_cmd_params(0xC0, (uint8_t[]){0x2C}, 1);
    bb_write_cmd_params(0xC2, (uint8_t[]){0x01}, 1);
    bb_write_cmd_params(0xC3, (uint8_t[]){0x15}, 1);
    bb_write_cmd_params(0xC4, (uint8_t[]){0x20}, 1);
    bb_write_cmd_params(0xC6, (uint8_t[]){0x0F}, 1);
    bb_write_cmd_params(0xD0, (uint8_t[]){0xA4, 0xA1}, 2);
    bb_write_cmd_params(0xD6, (uint8_t[]){0xA1}, 1);
    bb_write_cmd_params(0xE0,
        (uint8_t[]){0x70, 0x05, 0x0A, 0x0B, 0x0A, 0x27, 0x2F, 0x44,
                    0x47, 0x37, 0x14, 0x14, 0x29, 0x2F}, 14);
    bb_write_cmd_params(0xE1,
        (uint8_t[]){0x70, 0x07, 0x0C, 0x08, 0x08, 0x04, 0x2F, 0x33,
                    0x46, 0x18, 0x15, 0x15, 0x2B, 0x2D}, 14);
    bb_write_cmd(inv_on ? 0x21 : 0x20);
    bb_write_cmd(0x29);
}

static void bb_vendor_init(uint8_t madctl, uint8_t colmod)
{
    bb_vendor_init_ex(madctl, colmod, true);
}

static void bb_minimal_init(uint8_t madctl, uint8_t colmod)
{
    bb_write_cmd(0x11);
    vTaskDelay(pdMS_TO_TICKS(120));
    bb_write_cmd_params(0x3A, (uint8_t[]){colmod}, 1);
    bb_write_cmd_params(0x36, (uint8_t[]){madctl}, 1);
    bb_write_cmd(0x13);
    vTaskDelay(pdMS_TO_TICKS(10));
    bb_write_cmd(0x29);
}

static void bb_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    bb_write_cmd_burst(0x2A);
    bb_write_data_burst((uint8_t)(x0 >> 8));
    bb_write_data_burst((uint8_t)(x0 & 0xFF));
    bb_write_data_burst((uint8_t)(x1 >> 8));
    bb_write_data_burst((uint8_t)(x1 & 0xFF));
    bb_write_cmd_burst(0x2B);
    bb_write_data_burst((uint8_t)(y0 >> 8));
    bb_write_data_burst((uint8_t)(y0 & 0xFF));
    bb_write_data_burst((uint8_t)(y1 >> 8));
    bb_write_data_burst((uint8_t)(y1 & 0xFF));
    bb_write_cmd_burst(0x2C);
}

static void bb_fill_ex(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                     uint16_t color, bool swap_bytes)
{
    bb_db_set_output();
    bb_set_window(x, y, x + w - 1, y + h - 1);
    const uint8_t hi = (uint8_t)(color >> 8);
    const uint8_t lo = (uint8_t)(color & 0xFF);
    const uint8_t b0 = swap_bytes ? lo : hi;
    const uint8_t b1 = swap_bytes ? hi : lo;
    gpio_set_level(s_bb.dc, s_bb_dc_inv ? 0 : 1);
    const uint32_t total = (uint32_t)w * h;
    for (uint32_t i = 0; i < total; i++) {
        bb_write_bus(b0);
        bb_write_bus(b1);
        if ((i & 0x3FF) == 0) {
            taskYIELD();
        }
    }
    bb_burst_end();
}

static void bb_fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    bb_fill_ex(x, y, w, h, color, false);
}

static void bb_log_panel_status(uint8_t expect_madctl)
{
    uint8_t st[5] = {0};
    uint8_t pm = 0;
    uint8_t cm = 0;
    uint8_t md = 0;
    bb_read_reg_bytes(0x09, st, sizeof(st));
    bb_read_reg8(0x0A, &pm);
    bb_read_reg8(0x0C, &cm);
    bb_read_reg8(0x0B, &md);

    const bool discon = (st[0] & 0x04) != 0;
    const bool sleep_in = (pm & 0x10) != 0;
    const bool colmod_ok = ((cm & 0x07) == 0x05);
    const bool madctl_ok = (md == expect_madctl);

    ESP_LOGW(TAG, "  [init 状态] RDDST[0]=0x%02X RDDPM=0x%02X COLMOD=0x%02X MADCTL=0x%02X (expect 0x%02X)",
             st[0], pm, cm, md, expect_madctl);
    ESP_LOGW(TAG, "    DISPON=%s  SLPOUT=%s  COLMOD=0x05=%s  MADCTL=%s",
             discon ? "OK" : "FAIL",
             sleep_in ? "FAIL(sleep)" : "OK",
             colmod_ok ? "OK" : "FAIL",
             madctl_ok ? "OK" : "FAIL");
    if (discon && !sleep_in && colmod_ok && madctl_ok) {
        ESP_LOGI(TAG, "    => init 写命令 OK; 若仍全黑 -> WR 像素写 / 窗口偏移");
    } else if (!colmod_ok && !madctl_ok) {
        ESP_LOGE(TAG, "    => init 可能未写入; 查 WR 写时序");
    } else {
        ESP_LOGW(TAG, "    => 部分寄存器不符, 见上方逐项");
    }
}

static void lcd_bitbang_test_run(void)
{
    static const struct {
        uint8_t madctl;
        uint16_t x;
        uint16_t y;
        uint16_t w;
        uint16_t h;
        bool inv_on;
        bool swap_bytes;
        const char *tag;
    } cases[] = {
        {0x60, 0, 35, TFT_WIDTH, TFT_HEIGHT, true,  false, "land y35 INVON hi-lo"},
        {0x60, 0, 35, TFT_WIDTH, TFT_HEIGHT, true,  true,  "land y35 INVON lo-hi"},
        {0x60, 0, 35, TFT_WIDTH, TFT_HEIGHT, false, false, "land y35 INVOFF hi-lo"},
        {0x60, 35, 0, TFT_HEIGHT, TFT_WIDTH, true, false, "land x35 INVON"},
        {0x00, 35, 0, 170, 320, true, false, "port x35 INVON"},
        {0x00, 0, 0, ST7789_RAM_H, ST7789_RAM_V, true, false, "port full 240x320"},
        {0x60, 0, 0, ST7789_RAM_V, ST7789_RAM_H, true, false, "land full 320x240@0,0"},
    };

    ESP_LOGW(TAG, "=== 快速验证: init 状态寄存器 + 7 组红/绿 ===");
    ESP_LOGW(TAG, "请看串口 [init 状态] 与屏上 [0]~[6] 哪组有颜色");
    bb_apply_profile(&s_bb_profiles[0]);
    s_bb_dc_inv = false;
    s_bb_bus_rev = false;
    bb_gpio_init();
    gpio_set_level(LCD_PIN_BLK, 1);
    bb_hw_reset();
    bb_log_chip_probe();

    for (int i = 0; i < (int)(sizeof(cases) / sizeof(cases[0])); i++) {
        ESP_LOGI(TAG, "-- [%d] MADCTL=0x%02X %s: 红4s 绿2s --", i, cases[i].madctl, cases[i].tag);
        bb_hw_reset();
        bb_vendor_init_ex(cases[i].madctl, 0x05, cases[i].inv_on);
        bb_log_panel_status(cases[i].madctl);
        bb_fill_ex(cases[i].x, cases[i].y, cases[i].w, cases[i].h,
                   RED, cases[i].swap_bytes);
        vTaskDelay(pdMS_TO_TICKS(4000));
        bb_fill_ex(cases[i].x, cases[i].y, cases[i].w, cases[i].h,
                   GREEN, cases[i].swap_bytes);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }

    ESP_LOGW(TAG, "首轮结束; 若仍全黑请看 RDDST/RDDPM 与上方哪组有颜色");
    while (1) {
        bb_hw_reset();
        bb_vendor_init_ex(0x60, 0x05, true);
        bb_fill(0, 35, TFT_WIDTH, TFT_HEIGHT, RED);
        vTaskDelay(pdMS_TO_TICKS(2000));
        bb_fill(0, 35, TFT_WIDTH, TFT_HEIGHT, WHITE);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

#endif /* LCD_TEST_USE_BITBANG */

void lcd_baremetal_test_run(void)
{
    static const struct {
        uint16_t color;
        const char *name;
    } solids[] = {
        {RED, "RED"}, {GREEN, "GREEN"}, {BLUE, "BLUE"},
        {WHITE, "WHITE"}, {YELLOW, "YELLOW"}, {BLACK, "BLACK"},
    };

    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "  LCD bare-metal test (no LVGL/WiFi)");
    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "CS=%d RST=%d DC=%d WR=%d RD=%d BLK=%d",
             LCD_PIN_CS, LCD_PIN_RST, LCD_PIN_DC, LCD_PIN_WR, LCD_PIN_RD, LCD_PIN_BLK);
    ESP_LOGI(TAG, "DB0..7: %d %d %d %d %d %d %d %d",
             LCD_PIN_DB0, LCD_PIN_DB1, LCD_PIN_DB2, LCD_PIN_DB3,
             LCD_PIN_DB4, LCD_PIN_DB5, LCD_PIN_DB6, LCD_PIN_DB7);
    ESP_LOGI(TAG, "Resolution %dx%d  pclk=%d  madctl=0x%02X  gap=(%d,%d)",
             TFT_WIDTH, TFT_HEIGHT, (int)LCD_PIXEL_CLOCK_HZ,
             LCD_MADCTL_LANDSCAPE, TFT_X_OFFSET, TFT_Y_OFFSET);

    lcd_test_backlight_blink();

#if LCD_TEST_USE_BITBANG
    lcd_bitbang_test_run(); /* 不返回 */
#endif

#if LCD_BAREMETAL_ESP_LCD_PHASE
    ESP_LOGW(TAG, "=== esp_lcd I80 DMA 刷屏 (硬件并口, 替代 bitbang) ===");

#if LCD_BAREMETAL_SCAN_PROFILES
    lcd_run_bruteforce_matrix();
#else
    static const struct {
        bool swap;
        int16_t gap_x;
        int16_t gap_y;
        uint16_t w;
        uint16_t h;
        const char *tag;
    } esp_cases[] = {
        {false, TFT_X_OFFSET, TFT_Y_OFFSET, TFT_WIDTH, TFT_HEIGHT, "swap=0 gap(0,35) 320x170"},
        {true,  TFT_X_OFFSET, TFT_Y_OFFSET, TFT_WIDTH, TFT_HEIGHT, "swap=1 gap(0,35) 320x170"},
        {false, 35, 0, TFT_HEIGHT, TFT_WIDTH, "swap=0 gap(35,0) 170x320"},
        {false, 0, 0, ST7789_RAM_H, ST7789_RAM_V, "swap=0 gap(0,0) 240x320"},
        {true,  0, 0, ST7789_RAM_H, ST7789_RAM_V, "swap=1 gap(0,0) 240x320"},
    };

    for (int i = 0; i < (int)(sizeof(esp_cases) / sizeof(esp_cases[0])); i++) {
        lcd_deinit();
        lcd_init_full(esp_cases[i].swap, false, false);
        ESP_ERROR_CHECK(esp_lcd_panel_set_gap(s_panel, esp_cases[i].gap_x, esp_cases[i].gap_y));
        ESP_LOGI(TAG, "-- esp [%d] %s: RED 4s GREEN 2s dma_to=%lu --",
                 i, esp_cases[i].tag, (unsigned long)s_dma_timeout_count);
        lcd_clear_area(esp_cases[i].w, esp_cases[i].h, RED);
        vTaskDelay(pdMS_TO_TICKS(4000));
        lcd_clear_area(esp_cases[i].w, esp_cases[i].h, GREEN);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }

    lcd_deinit();
    lcd_init_full(false, false, false);

    while (1) {
        for (int i = 0; i < (int)(sizeof(solids) / sizeof(solids[0])); i++) {
            lcd_test_solid(solids[i].color, solids[i].name, 2000);
        }
        lcd_test_color_bars();
        vTaskDelay(pdMS_TO_TICKS(3000));
        lcd_test_checkerboard();
        vTaskDelay(pdMS_TO_TICKS(3000));
        lcd_test_cross();
        vTaskDelay(pdMS_TO_TICKS(3000));
        ESP_LOGI(TAG, "--- cycle done, dma_to=%lu ---", (unsigned long)s_dma_timeout_count);
    }
#endif /* LCD_BAREMETAL_SCAN_PROFILES */
#endif /* LCD_BAREMETAL_ESP_LCD_PHASE */
}

/* ---------- T190X7 1.9" ST7789 初始化 (与 lvgl_new SPI 版 lcd.c 一致, 8080 COLMOD=0x05) ---------- */
static void st7789_vendor_8080_init(esp_lcd_panel_io_handle_t io)
{
    esp_lcd_panel_io_tx_param(io, 0x11, NULL, 0);
    vTaskDelay(pdMS_TO_TICKS(120));

    esp_lcd_panel_io_tx_param(io, 0xB2, (uint8_t[]){0x0C, 0x0C, 0x00, 0x33, 0x33}, 5);
    esp_lcd_panel_io_tx_param(io, 0x35, (uint8_t[]){0x00}, 1);
    esp_lcd_panel_io_tx_param(io, 0x36, (uint8_t[]){LCD_MADCTL_LANDSCAPE}, 1);
    esp_lcd_panel_io_tx_param(io, 0x3A, (uint8_t[]){0x05}, 1);
    esp_lcd_panel_io_tx_param(io, 0xB7, (uint8_t[]){0x35}, 1);
    esp_lcd_panel_io_tx_param(io, 0xBB, (uint8_t[]){0x2D}, 1);
    esp_lcd_panel_io_tx_param(io, 0xC0, (uint8_t[]){0x2C}, 1);
    esp_lcd_panel_io_tx_param(io, 0xC2, (uint8_t[]){0x01}, 1);
    esp_lcd_panel_io_tx_param(io, 0xC3, (uint8_t[]){0x15}, 1);
    esp_lcd_panel_io_tx_param(io, 0xC4, (uint8_t[]){0x20}, 1);
    esp_lcd_panel_io_tx_param(io, 0xC6, (uint8_t[]){0x0F}, 1);
    esp_lcd_panel_io_tx_param(io, 0xD0, (uint8_t[]){0xA4, 0xA1}, 2);
    esp_lcd_panel_io_tx_param(io, 0xD6, (uint8_t[]){0xA1}, 1);
    esp_lcd_panel_io_tx_param(io, 0xE0,
        (uint8_t[]){0x70, 0x05, 0x0A, 0x0B, 0x0A, 0x27, 0x2F, 0x44,
                    0x47, 0x37, 0x14, 0x14, 0x29, 0x2F}, 14);
    esp_lcd_panel_io_tx_param(io, 0xE1,
        (uint8_t[]){0x70, 0x07, 0x0C, 0x08, 0x08, 0x04, 0x2F, 0x33,
                    0x46, 0x18, 0x15, 0x15, 0x2B, 0x2D}, 14);
    esp_lcd_panel_io_tx_param(io, 0x21, NULL, 0); /* INVON */
    esp_lcd_panel_io_tx_param(io, 0x29, NULL, 0); /* DISPON */
}

/* T190X7 1.9" 170x320 -> UI 横屏 320x170 */
static void lcd_apply_t190x7_layout(void)
{
    uint8_t madctl = LCD_MADCTL_LANDSCAPE;
    esp_lcd_panel_io_tx_param(s_io_handle, LCD_CMD_MADCTL, &madctl, 1);
    esp_lcd_panel_set_gap(s_panel, TFT_X_OFFSET, TFT_Y_OFFSET);
}

void lcd_deinit(void)
{
    if (s_panel) {
        esp_lcd_panel_del(s_panel);
        s_panel = NULL;
    }
    if (s_io_handle) {
        esp_lcd_panel_io_del(s_io_handle);
        s_io_handle = NULL;
    }
    if (s_i80_bus) {
        esp_lcd_del_i80_bus(s_i80_bus);
        s_i80_bus = NULL;
    }
    s_flush_done_cb = NULL;
    s_flush_done_user_ctx = NULL;
}

void lcd_init_full(bool swap_color_bytes, bool dc_inverted, bool bgr_order)
{
    static bool s_blk_gpio_done = false;

    s_dc_inverted = dc_inverted;
    s_bgr_order   = bgr_order;

    lcd_gpio_8080_idle();

    if (!s_blk_gpio_done) {
        gpio_config_t bk_io = {
            .mode         = GPIO_MODE_OUTPUT,
            .pin_bit_mask = 1ULL << LCD_PIN_BLK,
        };
        ESP_ERROR_CHECK(gpio_config(&bk_io));
        gpio_set_drive_capability(LCD_PIN_BLK, GPIO_DRIVE_CAP_3);
        s_blk_gpio_done = true;
    }
    gpio_set_level(LCD_PIN_BLK, 1);

    const int data_normal[8] = {
        LCD_PIN_DB0, LCD_PIN_DB1, LCD_PIN_DB2, LCD_PIN_DB3,
        LCD_PIN_DB4, LCD_PIN_DB5, LCD_PIN_DB6, LCD_PIN_DB7,
    };
    const int data_reversed[8] = {
        LCD_PIN_DB7, LCD_PIN_DB6, LCD_PIN_DB5, LCD_PIN_DB4,
        LCD_PIN_DB3, LCD_PIN_DB2, LCD_PIN_DB1, LCD_PIN_DB0,
    };

    esp_lcd_i80_bus_config_t bus_cfg = {
        .clk_src            = LCD_CLK_SRC_DEFAULT,
        .dc_gpio_num        = LCD_PIN_DC,
        .wr_gpio_num        = LCD_PIN_WR,
        .bus_width          = 8,
        .max_transfer_bytes = TFT_WIDTH * LCD_LVGL_BUF_LINES * sizeof(uint16_t) + 2048,
        .dma_burst_size     = 64,
    };
    for (int i = 0; i < 8; i++) {
        bus_cfg.data_gpio_nums[i] = s_data_bus_reversed ? data_reversed[i] : data_normal[i];
    }
    ESP_ERROR_CHECK(esp_lcd_new_i80_bus(&bus_cfg, &s_i80_bus));

    esp_lcd_panel_io_i80_config_t io_cfg = {
        .cs_gpio_num         = LCD_PIN_CS,
        .pclk_hz             = LCD_PIXEL_CLOCK_HZ,
        .trans_queue_depth   = 10,
        .on_color_trans_done = on_color_trans_done,
        .user_ctx            = NULL,
        .lcd_cmd_bits        = 8,
        .lcd_param_bits      = 8,
        .dc_levels = {
            .dc_idle_level  = dc_inverted ? 1 : 0,
            .dc_cmd_level   = dc_inverted ? 1 : 0,
            .dc_dummy_level = dc_inverted ? 1 : 0,
            .dc_data_level  = dc_inverted ? 0 : 1,
        },
        .flags = {
            /* IDF 示例: swap_color_bytes = !LV_COLOR_16_SWAP (lv_conf.h 里为 1) */
            .swap_color_bytes = swap_color_bytes,
        },
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i80(s_i80_bus, &io_cfg, &s_io_handle));

    esp_lcd_panel_dev_config_t panel_cfg = {
        .reset_gpio_num = LCD_PIN_RST,
        .rgb_ele_order  = bgr_order ? LCD_RGB_ELEMENT_ORDER_BGR : LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(s_io_handle, &panel_cfg, &s_panel));

    ESP_ERROR_CHECK(esp_lcd_panel_reset(s_panel));
    vTaskDelay(pdMS_TO_TICKS(120));
    /* ST7789P3 8080-I8: 不用 esp_lcd_panel_init (会发 COLMOD=0x55/SPI 格式),
     * 改走模组厂方 8080 序列 (COLMOD=0x05 = 并口 16bit) */
    st7789_vendor_8080_init(s_io_handle);
    lcd_apply_t190x7_layout();

    lcd_clear(BLACK);
    vTaskDelay(pdMS_TO_TICKS(10));
    lcd_on();

    ESP_LOGI(TAG,
        "ST7789P3 8080 init done: pclk=%d swap=%d dc_inv=%d bgr=%d bus_rev=%d madctl=0x%02X gap=(%d,%d)",
        (int)LCD_PIXEL_CLOCK_HZ, (int)swap_color_bytes, (int)dc_inverted,
        (int)bgr_order, (int)s_data_bus_reversed, LCD_MADCTL_LANDSCAPE,
        TFT_X_OFFSET, TFT_Y_OFFSET);
}

void lcd_init_with_options(bool swap_color_bytes)
{
    lcd_init_full(swap_color_bytes, s_dc_inverted, s_bgr_order);
}

void lcd_init(void)
{
    s_data_bus_reversed = false;
    lcd_init_full(LCD_I80_SWAP_COLOR_BYTES_FOR_LVGL, false, false);
}
