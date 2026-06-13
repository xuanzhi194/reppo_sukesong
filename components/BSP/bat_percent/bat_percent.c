/**
 * @file bat_percent.c
 * @brief 电池监测状态机：电压估算 + 充电状态判断 + 显示平滑
 *
 * 设计要点：
 *  - VBAT_ADC 只做"估算输入"，UI 显示用 display_percent，两者解耦；
 *  - BAT_CHARGE / BAT_FULL 经软件去抖后驱动状态机；
 *  - 充电时显示值冻结在插线前的水平，只允许 +1%/45s 缓慢上涨，避免
 *    充电器抬高电池端电压导致百分比瞬间跳变；
 *  - BAT_FULL 连续稳定 15s 才认满电，之后锁定 100%；
 *  - 拔线后进入 60s 恢复期，期间不相信 ADC，恢复期结束再向曲线估算值收敛。
 */
#include "bat_percent.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

static const char *TAG = "BAT_MONITOR";

/* ---------------- 调度参数 ---------------- */
#define BAT_POLL_PERIOD_MS        200    /* 状态机轮询周期 */
#define BAT_ADC_PERIOD_TICKS      10     /* 每 10 次轮询采一次 ADC（2s） */
#define BAT_GPIO_VOTE_SAMPLES     7      /* 每次轮询对 GPIO 做多数表决 */
#define BAT_CHARGE_DEBOUNCE_CNT   15     /* CHRG 去抖：连续 15 次（3s） */
#define BAT_PLUGGED_ENTER_CNT     3      /* 插线认定：CHRG|STDBY 任低 连续 3 次（0.6s） */
#define BAT_PLUGGED_EXIT_CNT      50     /* 拔线认定：两脚都高 连续 50 次（10s） */
#define BAT_FULL_STABLE_MS        15000  /* FULL 稳定 15s 才认满电 */
#define BAT_RECOVERY_MS           60000  /* 拔线后电压恢复期 60s */
#define BAT_CHARGE_RAMP_MS        45000  /* 充电时显示值上涨速率：+1%/45s */
#define BAT_DISCHG_STEP_MS        5000   /* 放电收敛速率：1%/5s */
#define BAT_DISCHG_HYST_PCT       2      /* 放电收敛迟滞，差值≤2% 不动 */

/* ---------------- ADC 采样参数 ---------------- */
#define BAT_DUMMY_SAMPLES         2      /* 丢弃前 2 次采样 */
#define BAT_SAMPLE_COUNT          32     /* 有效采样次数 */
#define BAT_TRIM_COUNT            8      /* 排序后两端各去掉 8 个极值 */

static adc_oneshot_unit_handle_t s_adc_handle;
static adc_cali_handle_t s_cali_handle = NULL;

static portMUX_TYPE s_info_mux = portMUX_INITIALIZER_UNLOCKED;
static battery_info_t s_info = {
    .voltage_mv = 0,
    .estimated_percent = 50,
    .display_percent = 50,
    .charging = false,
    .full = false,
    .plugged = false,
    .state = BATTERY_STATE_DISCHARGING,
};
static bool s_has_sample = false;

/* ---------------- 403450 锂电池近似 OCV 显示曲线 ---------------- */
static const struct { int mv; int pct; } s_ocv_table[] = {
    {4200, 100}, {4160, 95}, {4110, 90}, {4070, 85}, {4030, 80},
    {3980, 75},  {3930, 70}, {3890, 60}, {3850, 50}, {3810, 40},
    {3760, 30},  {3710, 20}, {3650, 10}, {3550, 5},  {3400, 0},
};
#define OCV_TABLE_LEN (sizeof(s_ocv_table) / sizeof(s_ocv_table[0]))

static int ocv_voltage_to_percent(int mv)
{
    if (mv >= s_ocv_table[0].mv) {
        return 100;
    }
    if (mv <= s_ocv_table[OCV_TABLE_LEN - 1].mv) {
        return 0;
    }
    for (size_t i = 1; i < OCV_TABLE_LEN; i++) {
        if (mv >= s_ocv_table[i].mv) {
            int v_hi = s_ocv_table[i - 1].mv, p_hi = s_ocv_table[i - 1].pct;
            int v_lo = s_ocv_table[i].mv,     p_lo = s_ocv_table[i].pct;
            return p_lo + (mv - v_lo) * (p_hi - p_lo) / (v_hi - v_lo);
        }
    }
    return 0;
}

/* ---------------- ADC：丢弃 dummy + 去极值平均 ---------------- */
static esp_err_t battery_read_voltage_mv(int *out_mv)
{
    int raw;
    for (int i = 0; i < BAT_DUMMY_SAMPLES; i++) {
        (void)adc_oneshot_read(s_adc_handle, BAT_ADC_CHAN, &raw);
    }

    int samples[BAT_SAMPLE_COUNT];
    int got = 0;
    for (int i = 0; i < BAT_SAMPLE_COUNT; i++) {
        if (adc_oneshot_read(s_adc_handle, BAT_ADC_CHAN, &raw) == ESP_OK) {
            samples[got++] = raw;
        }
    }
    if (got < BAT_SAMPLE_COUNT / 2) {
        return ESP_FAIL;
    }

    /* 插入排序后去掉两端极值再平均 */
    for (int i = 1; i < got; i++) {
        int key = samples[i], j = i - 1;
        while (j >= 0 && samples[j] > key) {
            samples[j + 1] = samples[j];
            j--;
        }
        samples[j + 1] = key;
    }
    int trim = (got > 2 * BAT_TRIM_COUNT + 4) ? BAT_TRIM_COUNT : got / 4;
    int64_t sum = 0;
    int cnt = 0;
    for (int i = trim; i < got - trim; i++) {
        sum += samples[i];
        cnt++;
    }
    int adc_avg = (int)(sum / cnt);

    int pad_mv;
    if (s_cali_handle != NULL &&
        adc_cali_raw_to_voltage(s_cali_handle, adc_avg, &pad_mv) == ESP_OK) {
        /* 校准成功 */
    } else {
        pad_mv = adc_avg * 3100 / 4095;  /* 12bit, 12dB ≈3.1V 满量程 */
    }

    *out_mv = pad_mv * BAT_DIVIDER_RATIO_X1000 / 1000;
    return ESP_OK;
}

/* ---------------- 充电状态 GPIO（多数表决抗 GPIO19/20 抖动） ---------------- */
static bool gpio_active_majority(gpio_num_t pin, int active_level)
{
    int active = 0;
    for (int i = 0; i < BAT_GPIO_VOTE_SAMPLES; i++) {
        if (gpio_get_level(pin) == active_level) {
            active++;
        }
    }
    return active > BAT_GPIO_VOTE_SAMPLES / 2;
}

static bool charge_pin_active(void)
{
    return gpio_active_majority(BAT_CHARGE_GPIO, BAT_CHARGE_ACTIVE_LEVEL);
}

static bool full_pin_active(void)
{
    return gpio_active_majority(BAT_FULL_GPIO, BAT_FULL_ACTIVE_LEVEL);
}

/* TP4056 插线：CHRG 或 STDBY 任一为低（充电中或已满） */
static bool tp4056_usb_present_raw(void)
{
    return charge_pin_active() || full_pin_active();
}

static void battery_gpio_init(void)
{
    gpio_config_t io = {
        .pin_bit_mask = (1ULL << BAT_CHARGE_GPIO) | (1ULL << BAT_FULL_GPIO),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,   /* 充电芯片多为开漏输出 */
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    esp_err_t err = gpio_config(&io);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "charge-status gpio_config failed: %s", esp_err_to_name(err));
    }
}

/* ---------------- 状态机任务 ---------------- */
static void battery_monitor_task(void *arg)
{
    (void)arg;

    /* 去抖计数 */
    int charge_cnt = 0, no_charge_cnt = 0;
    int full_cnt = 0, no_full_cnt = 0;
    int plugged_enter_cnt = 0, plugged_exit_cnt = 0;
    bool charge_stable = false;
    bool plugged_latched = false;
    bool full_raw_stable = false;
    TickType_t full_since = 0;
    bool full_latched = false;

    battery_state_t state = BATTERY_STATE_DISCHARGING;
    TickType_t recovery_end = 0;
    TickType_t last_ramp = 0;
    TickType_t last_dischg_step = 0;

    int smoothed_mv = 0;
    int display = -1;   /* 首次 ADC 后再初始化 */
    uint32_t tick = 0;

    const int full_stable_ticks = BAT_FULL_STABLE_MS / BAT_POLL_PERIOD_MS;

    while (1) {
        TickType_t now = xTaskGetTickCount();

        /* --- 1. 插线锁存（快入慢出，专供 UI 绿色/闪电图标） --- */
        if (tp4056_usb_present_raw()) {
            plugged_enter_cnt++;
            plugged_exit_cnt = 0;
        } else {
            plugged_exit_cnt++;
            plugged_enter_cnt = 0;
        }
        if (plugged_enter_cnt >= BAT_PLUGGED_ENTER_CNT) {
            plugged_latched = true;
        } else if (plugged_exit_cnt >= BAT_PLUGGED_EXIT_CNT) {
            plugged_latched = false;
        }

        /* --- 2. CHRG / STDBY 各自去抖（驱动百分比状态机） --- */
        if (charge_pin_active()) {
            charge_cnt++;
            no_charge_cnt = 0;
        } else {
            no_charge_cnt++;
            charge_cnt = 0;
        }
        if (charge_cnt >= BAT_CHARGE_DEBOUNCE_CNT) {
            charge_stable = true;
        } else if (no_charge_cnt >= BAT_CHARGE_DEBOUNCE_CNT) {
            charge_stable = false;
        }

        if (full_pin_active()) {
            full_cnt++;
            no_full_cnt = 0;
        } else {
            no_full_cnt++;
            full_cnt = 0;
        }
        if (full_cnt >= BAT_CHARGE_DEBOUNCE_CNT && !full_raw_stable) {
            full_raw_stable = true;
            full_since = now;
        } else if (no_full_cnt >= full_stable_ticks) {
            /* 满电撤销需要更长时间，避免 100% 频繁回落 */
            full_raw_stable = false;
            full_latched = false;
        }
        bool full_confirmed = full_raw_stable &&
            (now - full_since >= pdMS_TO_TICKS(BAT_FULL_STABLE_MS));
        if (full_confirmed) {
            full_latched = true;
        }

        /* --- 3. ADC 周期采样 + 电压平滑 --- */
        if (tick % BAT_ADC_PERIOD_TICKS == 0) {
            int mv;
            if (battery_read_voltage_mv(&mv) == ESP_OK) {
                smoothed_mv = (smoothed_mv == 0) ? mv : (smoothed_mv * 3 + mv) / 4;
                if (display < 0) {
                    display = ocv_voltage_to_percent(smoothed_mv);
                    last_ramp = now;
                    last_dischg_step = now;
                }
            }
        }

        /* --- 4. 状态迁移（拔线用 plugged_latched 慢释放，避免 CHRG 毛刺误触发恢复期） --- */
        battery_state_t prev = state;
        if (full_latched) {
            state = BATTERY_STATE_FULL;
        } else if (charge_stable || (plugged_latched && !full_latched)) {
            /* plugged 但 CHRG 短暂抖动时仍保持 CHARGING */
            state = BATTERY_STATE_CHARGING;
        } else if (!plugged_latched &&
                   (prev == BATTERY_STATE_CHARGING || prev == BATTERY_STATE_FULL)) {
            state = BATTERY_STATE_RECOVERY;
            recovery_end = now + pdMS_TO_TICKS(BAT_RECOVERY_MS);
        } else if (prev == BATTERY_STATE_RECOVERY && now >= recovery_end) {
            state = BATTERY_STATE_DISCHARGING;
        }
        if (state != prev) {
            ESP_LOGI(TAG, "state %d -> %d (plug=%d chg=%d full=%d v=%dmV)",
                     prev, state, plugged_latched, charge_stable, full_latched, smoothed_mv);
            if (state == BATTERY_STATE_CHARGING) {
                last_ramp = now;   /* 重置上涨计时，插线瞬间不 +1 */
            }
        }

        /* --- 5. 显示平滑 --- */
        int estimated = (smoothed_mv > 0) ? ocv_voltage_to_percent(smoothed_mv) : -1;
        if (display >= 0) {
            switch (state) {
            case BATTERY_STATE_CHARGING:
                /* 只允许慢慢上涨，封顶 99%（满电由 FULL 信号决定） */
                if (now - last_ramp >= pdMS_TO_TICKS(BAT_CHARGE_RAMP_MS)) {
                    last_ramp = now;
                    if (display < 99) {
                        display++;
                    }
                }
                break;
            case BATTERY_STATE_FULL:
                display = 100;
                break;
            case BATTERY_STATE_RECOVERY:
                /* 恢复期保持显示值不变 */
                break;
            case BATTERY_STATE_DISCHARGING:
            default:
                /* 向估算值限速收敛：1%/5s，带迟滞避免抖动 */
                if (estimated >= 0 &&
                    now - last_dischg_step >= pdMS_TO_TICKS(BAT_DISCHG_STEP_MS)) {
                    if (estimated < display - BAT_DISCHG_HYST_PCT) {
                        display--;
                        last_dischg_step = now;
                    } else if (estimated > display + BAT_DISCHG_HYST_PCT) {
                        display++;
                        last_dischg_step = now;
                    }
                }
                break;
            }
        }

        /* --- 6. 发布快照 --- */
        taskENTER_CRITICAL(&s_info_mux);
        s_info.voltage_mv = smoothed_mv;
        s_info.estimated_percent = (estimated >= 0) ? estimated : s_info.estimated_percent;
        if (display >= 0) {
            s_info.display_percent = display;
            s_has_sample = true;
        }
        s_info.charging = charge_stable && !full_latched;
        s_info.full = full_latched;
        s_info.plugged = plugged_latched;
        s_info.state = state;
        taskEXIT_CRITICAL(&s_info_mux);

        tick++;
        vTaskDelay(pdMS_TO_TICKS(BAT_POLL_PERIOD_MS));
    }
}

/* ---------------- 公共 API ---------------- */
void battery_adc_init(void)
{
    adc_oneshot_unit_init_cfg_t init_cfg = {
        .unit_id = ADC_UNIT_1,
    };
    esp_err_t err = adc_oneshot_new_unit(&init_cfg, &s_adc_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "adc_oneshot_new_unit failed: %s", esp_err_to_name(err));
        return;
    }

    adc_oneshot_chan_cfg_t chan_cfg = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = BAT_ADC_ATTEN,
    };
    err = adc_oneshot_config_channel(s_adc_handle, BAT_ADC_CHAN, &chan_cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "adc channel config failed: %s", esp_err_to_name(err));
    }

    adc_cali_curve_fitting_config_t cali_cfg = {
        .unit_id = ADC_UNIT_1,
        .chan = BAT_ADC_CHAN,
        .atten = BAT_ADC_ATTEN,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    if (adc_cali_create_scheme_curve_fitting(&cali_cfg, &s_cali_handle) != ESP_OK) {
        ESP_LOGW(TAG, "ADC calibration unavailable, using approximate conversion");
        s_cali_handle = NULL;
    }

    battery_gpio_init();

    xTaskCreatePinnedToCore(battery_monitor_task, "bat_monitor",
                            3072, NULL, 5, NULL, 0);
}

int get_battery_percentage(void)
{
    taskENTER_CRITICAL(&s_info_mux);
    bool valid = s_has_sample;
    int pct = s_info.display_percent;
    taskEXIT_CRITICAL(&s_info_mux);
    if (valid) {
        return pct;
    }
    /* 任务尚未完成首次采样时的兜底：同步估算一次 */
    int mv;
    if (battery_read_voltage_mv(&mv) == ESP_OK) {
        return ocv_voltage_to_percent(mv);
    }
    return pct;
}

void battery_monitor_get(battery_info_t *out)
{
    if (out == NULL) {
        return;
    }
    taskENTER_CRITICAL(&s_info_mux);
    *out = s_info;
    taskEXIT_CRITICAL(&s_info_mux);
}
