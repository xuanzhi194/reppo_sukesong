/*
 * ============================================================================
 *  factory_test_main.c
 *  Reppo R1 PCB 出厂焊接功能检测程序
 * ============================================================================
 *
 * 测试项目（顺序执行）:
 *  [01] PSRAM      — 外部 PSRAM 容量 & 读写验证（期望 ≥ 1 MB）
 *  [02] WS2812B    — 10 颗 LED 依次呈现红 / 绿 / 蓝 / 白 / 关
 *  [03] 蜂鸣器      — 低 / 中 / 高频各鸣响一次
 *  [04] 按键        — 等待 GPIO5 按下（限时 10 s，超时失败）
 *  [05] 编码器      — 等待 EC06 旋转任一方向（限时 15 s，超时失败）
 *  [06] 电池 ADC   — 读取电池电压 & TP4056 充电状态引脚电平
 *  [07] RTC        — I²C 探测 RX8130，读取时间寄存器
 *  [08] LCD        — ST7789P3 全屏纯色 + 八色彩条视觉测试
 *  [09] WiFi       — 扫描无线 AP（发现 ≥ 1 个即通过）
 *
 * 最终反馈：
 *  全部通过 → WS2812B 常绿 + 蜂鸣器双响
 *  有失败项 → WS2812B 常红 + 蜂鸣器三响
 *
 * 烧录方式：
 *  cd factory_test
 *  idf.py set-target esp32s3
 *  idf.py -p /dev/ttyUSB0 flash monitor
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
#include "nvs_flash.h"
#include "driver/gpio.h"

/* BSP 模块头文件 */
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

#define TAG             "FACTORY"
#define DELAY_MS(ms)    vTaskDelay(pdMS_TO_TICKS(ms))

/* 交互式测试超时 */
#define BTN_TIMEOUT_MS      10000   /* 按键等待上限 10 s */
#define ENC_TIMEOUT_MS      15000   /* 编码器等待上限 15 s */
#define POLL_INTERVAL_MS    50      /* 轮询间隔 */

/* 电池电压合法范围 (mV) */
#define BAT_VOLTAGE_MIN     2500
#define BAT_VOLTAGE_MAX     4500

/* PSRAM 最小可接受容量 */
#define PSRAM_MIN_BYTES     (1 * 1024 * 1024)   /* 1 MB */

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

static inline void led_off(void)         { set_flow_light(0, 0, 0); }
static inline void led_blue(void)        { set_flow_light(0, 0, 40); }
static inline void led_green(void)       { set_flow_light(0, 50, 0); }
static inline void led_red(void)         { set_flow_light(60, 0, 0); }

static void buzzer_tone(uint32_t freq_hz, uint32_t dur_ms)
{
    buzzer_set_sound(freq_hz, 70);
    DELAY_MS(dur_ms);
    buzzer_set_sound(freq_hz, 0);
}

/* ============================================================================
 *  [01] PSRAM 检测
 * ============================================================================ */

static bool test_psram(void)
{
    size_t total = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
    size_t free_sz = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);

    ESP_LOGI(TAG, "PSRAM total=%u KB  free=%u KB", total / 1024, free_sz / 1024);

    if (total < PSRAM_MIN_BYTES) {
        ESP_LOGE(TAG, "PSRAM 未检测到或容量不足 (%u KB < %u KB)",
                 total / 1024, PSRAM_MIN_BYTES / 1024);
        return false;
    }

    /* 分配 1 MB，写入并读回校验 */
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
        ESP_LOGI(TAG, "LED → %s", seq[i].name);
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
            DELAY_MS(200);  /* 去抖 */
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
    /* battery_adc_init() 已在 app_main 里调用，后台任务已跑了一段时间 */
    DELAY_MS(300);  /* 确保后台任务至少完成一次采样 */

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
 *  [07] RTC RX8130 I²C 检测
 * ============================================================================ */

static bool test_rtc(void)
{
    /* rx8130_init 已在 app_main 调用，仅重新探测 + 读时间 */
    esp_err_t ret = rx8130_probe(RX8130_I2C_PORT);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "RTC: 未检测到 RX8130 (I²C addr=0x%02X)  err=%s",
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
        /* 器件通信正常，时间未设置不算硬件故障，仅警告 */
        ESP_LOGW(TAG, "RTC: 年份异常(%d)，晶振可能未起振或时间未同步（但器件通信 OK）", t.year);
    }

    return true;
}

/* ============================================================================
 *  [08] LCD ST7789P3 视觉测试（纯色 + 八色彩条）
 * ============================================================================ */

static void lcd_draw_color_bars(void)
{
    /* 八色彩条：经典广播制式顺序 */
    static const uint16_t bar_colors[8] = {
        WHITE, YELLOW, CYAN, GREEN, MAGENTA, RED, BLUE, BLACK
    };
    const int bar_w = TFT_WIDTH / 8;   /* 40 像素/条 */

    /* 一行像素缓冲（320×2 = 640 字节，栈上安全） */
    uint16_t row[TFT_WIDTH];
    for (int x = 0; x < TFT_WIDTH; x++) {
        int idx = x / bar_w;
        row[x] = bar_colors[(idx < 8) ? idx : 7];
    }

    /* 逐行刷屏（320 行，每行 DMA 传输） */
    for (int y = 0; y < TFT_HEIGHT; y++) {
        lcd_draw_bitmap(0, y, TFT_WIDTH - 1, y, row);
        lcd_wait_flush_done();
    }
}

static bool test_lcd(void)
{
    ESP_LOGI(TAG, "LCD: 初始化 ST7789P3 (8080 I80)...");
    lcd_init_with_options(true);  /* swap_color_bytes=true，裸机填色 */
    lcd_on();
    DELAY_MS(200);

    /* 全屏纯色循环 */
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
        ESP_LOGI(TAG, "LCD: 全屏 → %s", fills[i].name);
        lcd_clear(fills[i].color);
        DELAY_MS(500);
    }

    /* 八色彩条 */
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

    /* 轮询等待扫描结果，最长 6 s */
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
 *  汇总输出
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

    /* LED + 蜂鸣器提示 */
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
    /* 打印芯片信息 */
    esp_chip_info_t chip;
    esp_chip_info(&chip);
    uint32_t flash_mb = 0;
    esp_flash_get_size(NULL, &flash_mb);
    flash_mb /= (1024 * 1024);

    printf("\n");
    printf("========================================\n");
    printf("  Reppo R1  PCB 出厂功能测试  v1.0\n");
    printf("  芯片: ESP32-S3  核心: %d  Rev: %d\n", chip.cores, chip.revision);
    printf("  Flash: %lu MB   PSRAM: %lu MB\n",
           (unsigned long)flash_mb,
           (unsigned long)(heap_caps_get_total_size(MALLOC_CAP_SPIRAM) / 1024 / 1024));
    printf("========================================\n\n");

    /* NVS 初始化（WiFi 依赖） */
    esp_err_t nvs_ret = nvs_flash_init();
    if (nvs_ret == ESP_ERR_NVS_NO_FREE_PAGES || nvs_ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    /* 早期硬件初始化 */
    init_led_strip();
    buzzer_init();
    battery_adc_init();    /* 启动后台采样任务 */
    init_encoder_direction();
    rx8130_init(RX8130_I2C_PORT, RX8130_I2C_SDA_PIN, RX8130_I2C_SCL_PIN);

    /* 开始前闪烁蓝色表示测试即将开始 */
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
        led_blue();  /* 测试中：蓝色 */

        s_results[i] = tests[i]();
        all_pass = all_pass && s_results[i];

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
     *  结果汇总 & 反馈
     * ---------------------------------------------------------------- */
    print_summary(all_pass);

    /* 保持结果灯常亮，循环等待 */
    while (1) {
        if (!all_pass) {
            /* 有失败：红灯慢闪提醒 */
            led_red();   DELAY_MS(1500);
            led_off();   DELAY_MS(500);
        } else {
            DELAY_MS(2000);
        }
    }
}
