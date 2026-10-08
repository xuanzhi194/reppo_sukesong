/**
 * @file zts6672_mic.h
 * @brief ZTS6672 I2S MEMS 麦克风驱动（Reppo R2）
 *
 * BCLK GPIO42 / WS GPIO41 / DATA GPIO40，ESP32-S3 为 I2S 主机、仅 RX。
 * 这三脚在 V3.7 板上未被其它外设占用。不要在 app 启动时调用 zts6672_init()，
 * 需要采集时再初始化（内部会占用 I2S，并丢弃约 300 ms 上电数据）。
 * 已初始化后再用不同采样率调用 init() 会先关掉再重开。同一时间只应有一个调用者。
 * 麦克风 CONFIG 脚须悬空（I2S 模式）；SELECT 低 = 左声道，高 = 右声道。
 * 驱动按立体声 32 bit 槽采集，自动识别麦克风实际输出所在的声道。
 * 输出样本为 24 bit 有符号数（存放在 int32_t 低 24 位，已符号扩展），
 * 并经过 ZTS6672_HPF_CUTOFF_HZ 一阶高通去除直流与上电低频漂移。
 */
#ifndef ZTS6672_MIC_H
#define ZTS6672_MIC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "driver/gpio.h"
#include "esp_err.h"

#ifndef ZTS6672_I2S_SCK_GPIO
#define ZTS6672_I2S_SCK_GPIO    GPIO_NUM_42
#endif
#ifndef ZTS6672_I2S_WS_GPIO
#define ZTS6672_I2S_WS_GPIO     GPIO_NUM_41
#endif
#ifndef ZTS6672_I2S_SD_GPIO
#define ZTS6672_I2S_SD_GPIO     GPIO_NUM_40
#endif

/* BCLK = 64 × fs，须落在 0.768 ~ 4.8 MHz，低于 0.9 MHz 麦克风会进入睡眠 */
#define ZTS6672_SAMPLE_RATE_MIN     16000
#define ZTS6672_SAMPLE_RATE_MAX     48000
#define ZTS6672_DEFAULT_SAMPLE_RATE 16000

#define ZTS6672_FULL_SCALE          8388608.0f   /* 2^23 */

#ifndef ZTS6672_HPF_CUTOFF_HZ
#define ZTS6672_HPF_CUTOFF_HZ       40.0f
#endif

typedef enum {
    ZTS6672_SLOT_UNKNOWN = 0,
    ZTS6672_SLOT_LEFT,
    ZTS6672_SLOT_RIGHT,
} zts6672_slot_t;

typedef struct {
    uint32_t samples;
    int32_t  mean;        /* 直流分量 */
    int32_t  min;
    int32_t  max;
    int32_t  peak;        /* max |x - mean| */
    float    rms;         /* 去直流后的 RMS（24 bit LSB） */
    float    rms_dbfs;
    float    peak_dbfs;
} zts6672_stats_t;

esp_err_t zts6672_init(uint32_t sample_rate_hz);
esp_err_t zts6672_deinit(void);
bool      zts6672_is_ready(void);
uint32_t  zts6672_get_sample_rate(void);

/**
 * 采集 duration_ms 双声道数据，选出有信号的声道作为后续 read/measure 的来源。
 * left / right 可为 NULL。两个声道都没有有效数据时返回 ESP_ERR_NOT_FOUND。
 */
esp_err_t zts6672_detect_slot(uint32_t duration_ms, zts6672_stats_t *left, zts6672_stats_t *right);
zts6672_slot_t zts6672_get_slot(void);
void      zts6672_set_slot(zts6672_slot_t slot);

/** 读取单声道样本，阻塞直到读满 max_samples 或超时。 */
esp_err_t zts6672_read(int32_t *pcm, size_t max_samples, size_t *out_samples, uint32_t timeout_ms);

/** 连续采集 duration_ms 并统计电平，不保存样本。 */
esp_err_t zts6672_measure(uint32_t duration_ms, zts6672_stats_t *out);

void      zts6672_compute_stats(const int32_t *pcm, size_t n, zts6672_stats_t *out);
float     zts6672_to_dbfs(float level);

#endif /* ZTS6672_MIC_H */
