#include "zts6672_mic.h"

#include <math.h>
#include <string.h>
#include "driver/i2s_std.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "zts6672";

#define MIC_CHUNK_FRAMES     256
#define MIC_READ_TIMEOUT_MS  100
#define MIC_WARMUP_MS        300
/* 低于该 RMS（24 bit LSB）视为 DATA 线无有效信号；正常底噪约为数百 LSB */
#define MIC_DEAD_RMS_LSB     4.0f

static i2s_chan_handle_t s_rx;
static uint32_t          s_rate;
static zts6672_slot_t    s_slot = ZTS6672_SLOT_UNKNOWN;
static int32_t           s_frames[MIC_CHUNK_FRAMES * 2];

/* 一阶去直流高通：y[n] = x[n] - x[n-1] + R * y[n-1] */
typedef struct {
    bool  primed;
    float x1;
    float y1;
} dc_block_t;

static dc_block_t s_dc[2];
static float      s_dc_r;

static void dc_block_reset(void)
{
    memset(s_dc, 0, sizeof(s_dc));
}

static inline int32_t dc_block(dc_block_t *f, int32_t x)
{
    if (!f->primed) {
        f->primed = true;
        f->x1 = (float)x;
        f->y1 = 0.0f;
        return 0;
    }
    float y = (float)x - f->x1 + s_dc_r * f->y1;
    f->x1 = (float)x;
    f->y1 = y;
    return (int32_t)lrintf(y);
}

typedef struct {
    uint32_t n;
    int32_t  ref;
    int64_t  sum;
    double   sumsq;
    int32_t  min;
    int32_t  max;
} level_acc_t;

static void acc_reset(level_acc_t *a)
{
    memset(a, 0, sizeof(*a));
    a->min = INT32_MAX;
    a->max = INT32_MIN;
}

static inline void acc_add(level_acc_t *a, int32_t x)
{
    if (a->n == 0) a->ref = x;
    int32_t d = x - a->ref;
    a->sum   += d;
    a->sumsq += (double)d * d;
    if (x < a->min) a->min = x;
    if (x > a->max) a->max = x;
    a->n++;
}

static void acc_finish(const level_acc_t *a, zts6672_stats_t *out)
{
    memset(out, 0, sizeof(*out));
    out->samples = a->n;
    if (a->n == 0) {
        out->rms_dbfs = out->peak_dbfs = zts6672_to_dbfs(0);
        return;
    }
    double mean_d = (double)a->sum / a->n;
    double var    = a->sumsq / a->n - mean_d * mean_d;
    int32_t mean  = a->ref + (int32_t)lround(mean_d);

    out->mean = mean;
    out->min  = a->min;
    out->max  = a->max;
    int32_t up = a->max - mean, down = mean - a->min;
    out->peak = up > down ? up : down;
    out->rms  = var > 0 ? (float)sqrt(var) : 0.0f;
    out->rms_dbfs  = zts6672_to_dbfs(out->rms);
    out->peak_dbfs = zts6672_to_dbfs((float)out->peak);
}

float zts6672_to_dbfs(float level)
{
    if (level < 1.0f) return -144.0f;
    return 20.0f * log10f(level / ZTS6672_FULL_SCALE);
}

static inline int32_t raw_to_sample(int32_t raw)
{
    return raw >> 8;
}

/* 读取一块立体声帧，返回帧数 */
static esp_err_t read_frames(size_t max_frames, size_t *frames)
{
    if (max_frames > MIC_CHUNK_FRAMES) max_frames = MIC_CHUNK_FRAMES;
    size_t bytes = 0;
    esp_err_t err = i2s_channel_read(s_rx, s_frames, max_frames * 2 * sizeof(int32_t),
                                     &bytes, MIC_READ_TIMEOUT_MS);
    *frames = bytes / (2 * sizeof(int32_t));
    if (err == ESP_OK && *frames == 0) err = ESP_ERR_TIMEOUT;
    return err;
}

static inline size_t slot_index(void)
{
    return s_slot == ZTS6672_SLOT_RIGHT ? 1 : 0;
}

static esp_err_t discard_ms(uint32_t ms)
{
    size_t left = (size_t)s_rate * ms / 1000;
    while (left) {
        size_t got = 0;
        esp_err_t err = read_frames(left, &got);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "discard: %s got=%u left=%u", esp_err_to_name(err), (unsigned)got, (unsigned)left);
            return err;
        }
        for (size_t i = 0; i < got; i++) {
            dc_block(&s_dc[0], raw_to_sample(s_frames[i * 2]));
            dc_block(&s_dc[1], raw_to_sample(s_frames[i * 2 + 1]));
        }
        left -= got;
    }
    return ESP_OK;
}

esp_err_t zts6672_init(uint32_t sample_rate_hz)
{
    if (sample_rate_hz == 0) sample_rate_hz = ZTS6672_DEFAULT_SAMPLE_RATE;
    if (sample_rate_hz < ZTS6672_SAMPLE_RATE_MIN || sample_rate_hz > ZTS6672_SAMPLE_RATE_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_rx) {
        if (sample_rate_hz == s_rate) return ESP_OK;
        esp_err_t err = zts6672_deinit();
        if (err != ESP_OK) return err;
    }

    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    chan_cfg.dma_desc_num  = 4;
    chan_cfg.dma_frame_num = MIC_CHUNK_FRAMES;
    esp_err_t err = i2s_new_channel(&chan_cfg, NULL, &s_rx);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "i2s_new_channel: %s", esp_err_to_name(err));
        s_rx = NULL;
        return err;
    }

    i2s_std_config_t std_cfg = {
        .clk_cfg  = I2S_STD_CLK_DEFAULT_CONFIG(sample_rate_hz),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = ZTS6672_I2S_SCK_GPIO,
            .ws   = ZTS6672_I2S_WS_GPIO,
            .dout = I2S_GPIO_UNUSED,
            .din  = ZTS6672_I2S_SD_GPIO,
            .invert_flags = { 0 },
        },
    };
    err = i2s_channel_init_std_mode(s_rx, &std_cfg);
    if (err == ESP_OK) {
        /* 麦克风在非本声道时 DATA 为高阻，下拉让空声道稳定读到 0 */
        gpio_pulldown_en(ZTS6672_I2S_SD_GPIO);
        err = i2s_channel_enable(s_rx);
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "i2s std init: %s", esp_err_to_name(err));
        i2s_del_channel(s_rx);
        s_rx = NULL;
        return err;
    }

    s_rate = sample_rate_hz;
    s_dc_r = expf(-2.0f * (float)M_PI * ZTS6672_HPF_CUTOFF_HZ / (float)sample_rate_hz);
    dc_block_reset();
    err = discard_ms(MIC_WARMUP_MS);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "no I2S data after enable: %s", esp_err_to_name(err));
        zts6672_deinit();
        return err;
    }
    ESP_LOGI(TAG, "ready: %lu Hz, BCLK=%d WS=%d SD=%d", (unsigned long)s_rate,
             ZTS6672_I2S_SCK_GPIO, ZTS6672_I2S_WS_GPIO, ZTS6672_I2S_SD_GPIO);
    return ESP_OK;
}

esp_err_t zts6672_deinit(void)
{
    if (!s_rx) return ESP_OK;
    i2s_channel_disable(s_rx);
    esp_err_t err = i2s_del_channel(s_rx);
    s_rx = NULL;
    s_rate = 0;
    return err;
}

bool zts6672_is_ready(void)
{
    return s_rx != NULL;
}

uint32_t zts6672_get_sample_rate(void)
{
    return s_rate;
}

zts6672_slot_t zts6672_get_slot(void)
{
    return s_slot;
}

void zts6672_set_slot(zts6672_slot_t slot)
{
    s_slot = slot;
}

esp_err_t zts6672_detect_slot(uint32_t duration_ms, zts6672_stats_t *left, zts6672_stats_t *right)
{
    if (!s_rx) return ESP_ERR_INVALID_STATE;

    level_acc_t acc[2];
    acc_reset(&acc[0]);
    acc_reset(&acc[1]);

    size_t remain = (size_t)s_rate * duration_ms / 1000;
    while (remain) {
        size_t got = 0;
        esp_err_t err = read_frames(remain, &got);
        if (err != ESP_OK) return err;
        for (size_t i = 0; i < got; i++) {
            acc_add(&acc[0], dc_block(&s_dc[0], raw_to_sample(s_frames[i * 2])));
            acc_add(&acc[1], dc_block(&s_dc[1], raw_to_sample(s_frames[i * 2 + 1])));
        }
        remain -= got;
    }

    zts6672_stats_t st[2];
    acc_finish(&acc[0], &st[0]);
    acc_finish(&acc[1], &st[1]);
    if (left)  *left  = st[0];
    if (right) *right = st[1];

    bool l_ok = st[0].rms >= MIC_DEAD_RMS_LSB;
    bool r_ok = st[1].rms >= MIC_DEAD_RMS_LSB;
    if (!l_ok && !r_ok) {
        s_slot = ZTS6672_SLOT_UNKNOWN;
        return ESP_ERR_NOT_FOUND;
    }
    s_slot = (st[1].rms > st[0].rms) ? ZTS6672_SLOT_RIGHT : ZTS6672_SLOT_LEFT;
    ESP_LOGI(TAG, "slot=%s  L rms=%.1f(%.1f dBFS)  R rms=%.1f(%.1f dBFS)",
             s_slot == ZTS6672_SLOT_RIGHT ? "RIGHT" : "LEFT",
             st[0].rms, st[0].rms_dbfs, st[1].rms, st[1].rms_dbfs);
    return ESP_OK;
}

esp_err_t zts6672_read(int32_t *pcm, size_t max_samples, size_t *out_samples, uint32_t timeout_ms)
{
    if (out_samples) *out_samples = 0;
    if (!pcm || max_samples == 0) return ESP_ERR_INVALID_ARG;
    if (!s_rx) return ESP_ERR_INVALID_STATE;

    const size_t idx = slot_index();
    TickType_t start = xTaskGetTickCount();
    size_t total = 0;
    while (total < max_samples) {
        size_t got = 0;
        esp_err_t err = read_frames(max_samples - total, &got);
        if (err != ESP_OK && err != ESP_ERR_TIMEOUT) return err;
        for (size_t i = 0; i < got; i++) {
            pcm[total + i] = dc_block(&s_dc[idx], raw_to_sample(s_frames[i * 2 + idx]));
        }
        total += got;
        if (out_samples) *out_samples = total;
        if ((xTaskGetTickCount() - start) >= pdMS_TO_TICKS(timeout_ms)) {
            return total == max_samples ? ESP_OK : ESP_ERR_TIMEOUT;
        }
    }
    return ESP_OK;
}

esp_err_t zts6672_measure(uint32_t duration_ms, zts6672_stats_t *out)
{
    if (!out) return ESP_ERR_INVALID_ARG;
    if (!s_rx) return ESP_ERR_INVALID_STATE;

    const size_t idx = slot_index();
    level_acc_t acc;
    acc_reset(&acc);

    size_t remain = (size_t)s_rate * duration_ms / 1000;
    while (remain) {
        size_t got = 0;
        esp_err_t err = read_frames(remain, &got);
        if (err != ESP_OK) return err;
        for (size_t i = 0; i < got; i++) {
            acc_add(&acc, dc_block(&s_dc[idx], raw_to_sample(s_frames[i * 2 + idx])));
        }
        remain -= got;
    }
    acc_finish(&acc, out);
    return ESP_OK;
}

void zts6672_compute_stats(const int32_t *pcm, size_t n, zts6672_stats_t *out)
{
    if (!out) return;
    level_acc_t acc;
    acc_reset(&acc);
    if (pcm) {
        for (size_t i = 0; i < n; i++) acc_add(&acc, pcm[i]);
    }
    acc_finish(&acc, out);
}
