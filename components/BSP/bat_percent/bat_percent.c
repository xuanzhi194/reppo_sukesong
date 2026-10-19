/**
 * @file bat_percent.c
 * @brief CW2015 电量计 + TP4056 充电状态。
 *
 * 电压和百分比来自 CW2015CHBD（出厂锂聚合物模型）。GPIO8 是 ALRT#，
 * 只作低电告警输入，不再做 ADC。充电图标仍看 TP4056 的 CHRG / STDBY。
 */
#include "bat_percent.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

static const char *TAG = "BAT_MONITOR";

#define CW_REG_VERSION          0x00
#define CW_REG_VCELL            0x02
#define CW_REG_SOC              0x04
#define CW_REG_RRT_ALERT        0x06
#define CW_REG_CONFIG           0x08
#define CW_REG_MODE             0x0A

#define CW_MODE_SLEEP           0xC0
#define CW_CONFIG_UFG           0x02
#define CW_ATHD_PERCENT         10
#define CW_I2C_TIMEOUT_MS       50

#define BAT_POLL_PERIOD_MS        200
#define BAT_GAUGE_PERIOD_TICKS    10     /* 2s 读一次电量计 */
#define BAT_GPIO_VOTE_SAMPLES     7
#define BAT_CHARGE_DEBOUNCE_CNT   15
#define BAT_PLUGGED_ENTER_CNT     3
#define BAT_PLUGGED_EXIT_CNT      50
#define BAT_PLUGGED_UI_ENTER_CNT  3
#define BAT_PLUGGED_UI_EXIT_CNT   5
#define BAT_FULL_STABLE_MS        15000
#define BAT_TRACK_MS              2000
#define BAT_LOG_PERIOD_TICKS      25     /* 5s 打一行，便于串口确认 */

/* 芯片 SOC 还没算出来时，用端电压粗略兜底。CW2015 量的是电芯电压。 */
static const struct { int mv; int pct; } s_soc_table[] = {
    {4150, 100}, {4070, 95}, {4010, 90}, {3960, 85}, {3920, 80},
    {3885, 75},  {3855, 70}, {3830, 65}, {3805, 60}, {3780, 55},
    {3770, 50},  {3745, 45}, {3720, 40}, {3690, 35}, {3655, 30},
    {3615, 25},  {3570, 20}, {3520, 15}, {3450, 10}, {3350, 5},  {3200, 0},
};
#define SOC_TABLE_LEN (sizeof(s_soc_table) / sizeof(s_soc_table[0]))

static portMUX_TYPE s_info_mux = portMUX_INITIALIZER_UNLOCKED;
static battery_info_t s_info = {
    .voltage_mv = 0,
    .estimated_percent = 50,
    .display_percent = 50,
    .charging = false,
    .full = false,
    .plugged = false,
    .plugged_ui = false,
    .state = BATTERY_STATE_DISCHARGING,
};
static bool s_gauge_ready = false;

static int clamp_percent(int pct)
{
    if (pct < 0) {
        return 0;
    }
    if (pct > 100) {
        return 100;
    }
    return pct;
}

static int voltage_to_percent(int mv)
{
    if (mv >= s_soc_table[0].mv) {
        return 100;
    }
    if (mv <= s_soc_table[SOC_TABLE_LEN - 1].mv) {
        return 0;
    }
    for (size_t i = 1; i < SOC_TABLE_LEN; i++) {
        if (mv >= s_soc_table[i].mv) {
            int v_hi = s_soc_table[i - 1].mv, p_hi = s_soc_table[i - 1].pct;
            int v_lo = s_soc_table[i].mv,     p_lo = s_soc_table[i].pct;
            return clamp_percent(p_lo + (mv - v_lo) * (p_hi - p_lo) / (v_hi - v_lo));
        }
    }
    return 0;
}

static esp_err_t cw_read(uint8_t reg, uint8_t *buf, size_t len)
{
    return i2c_master_write_read_device(CW2015_I2C_PORT, CW2015_I2C_ADDR,
                                        &reg, 1, buf, len,
                                        pdMS_TO_TICKS(CW_I2C_TIMEOUT_MS));
}

static esp_err_t cw_write(uint8_t reg, uint8_t val)
{
    uint8_t buf[2] = { reg, val };
    return i2c_master_write_to_device(CW2015_I2C_PORT, CW2015_I2C_ADDR,
                                      buf, sizeof(buf),
                                      pdMS_TO_TICKS(CW_I2C_TIMEOUT_MS));
}

/** 退出出厂休眠，并把低电告警阈值设到 10%。CHBD 自带锂聚合物模型。 */
static esp_err_t cw_wake(void)
{
    uint8_t mode = 0;
    uint8_t version = 0;
    uint8_t config = 0;
    esp_err_t err = cw_read(CW_REG_MODE, &mode, 1);
    if (err != ESP_OK) {
        return err;
    }
    if ((mode & CW_MODE_SLEEP) == CW_MODE_SLEEP) {
        err = cw_write(CW_REG_MODE, (uint8_t)(mode & (uint8_t)~CW_MODE_SLEEP));
        if (err != ESP_OK) {
            return err;
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
    err = cw_read(CW_REG_VERSION, &version, 1);
    if (err != ESP_OK) {
        return err;
    }
    if (version != CW2015_VERSION_EXPECT) {
        ESP_LOGW(TAG, "CW2015 version 0x%02X, expect 0x%02X",
                 version, CW2015_VERSION_EXPECT);
        return ESP_ERR_INVALID_RESPONSE;
    }
    err = cw_read(CW_REG_CONFIG, &config, 1);
    if (err != ESP_OK) {
        return err;
    }
    {
        uint8_t next = (uint8_t)((CW_ATHD_PERCENT << 3) | (config & CW_CONFIG_UFG));
        if (next != config) {
            err = cw_write(CW_REG_CONFIG, next);
            if (err != ESP_OK) {
                return err;
            }
        }
    }
    ESP_LOGI(TAG, "CW2015 ready, version 0x%02X", version);
    return ESP_OK;
}

static esp_err_t cw_read_sample(int *mv, int *soc, bool *soc_valid)
{
    uint8_t raw[2];
    uint16_t word;
    esp_err_t err = cw_read(CW_REG_VCELL, raw, 2);
    if (err != ESP_OK) {
        return err;
    }
    word = (uint16_t)((raw[0] << 8) | raw[1]);
    *mv = (int)((uint32_t)word * 305u / 1000u);

    err = cw_read(CW_REG_SOC, raw, 2);
    if (err != ESP_OK) {
        return err;
    }
    *soc_valid = raw[0] <= 100;
    *soc = *soc_valid ? raw[0] : -1;

    err = cw_read(CW_REG_RRT_ALERT, raw, 1);
    if (err == ESP_OK && (raw[0] & 0x80)) {
        (void)cw_write(CW_REG_RRT_ALERT, (uint8_t)(raw[0] & 0x7F));
    }
    return ESP_OK;
}

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

static bool tp4056_usb_present_raw(void)
{
    return charge_pin_active() || full_pin_active();
}

static void battery_gpio_init(void)
{
    gpio_config_t charge = {
        .pin_bit_mask = (1ULL << BAT_CHARGE_GPIO) | (1ULL << BAT_FULL_GPIO),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    gpio_config_t alert = {
        .pin_bit_mask = (1ULL << BAT_ALERT_GPIO),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    esp_err_t err = gpio_config(&charge);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "charge-status gpio_config failed: %s", esp_err_to_name(err));
    }
    err = gpio_config(&alert);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "alert gpio_config failed: %s", esp_err_to_name(err));
    }
}

static void battery_monitor_task(void *arg)
{
    (void)arg;

    int charge_cnt = 0, no_charge_cnt = 0;
    int full_cnt = 0, no_full_cnt = 0;
    int plugged_enter_cnt = 0, plugged_exit_cnt = 0;
    int plugged_ui_enter_cnt = 0, plugged_ui_exit_cnt = 0;
    bool charge_stable = false;
    bool plugged_latched = false;
    bool plugged_ui_latched = false;
    bool full_raw_stable = false;
    TickType_t full_since = 0;
    bool full_latched = false;

    battery_state_t state = BATTERY_STATE_DISCHARGING;
    TickType_t last_track = 0;
    int smoothed_mv = 0;
    int last_soc = -1;
    int display = -1;
    uint32_t tick = 0;
    const int full_stable_ticks = BAT_FULL_STABLE_MS / BAT_POLL_PERIOD_MS;

    while (1) {
        TickType_t now = xTaskGetTickCount();

        if (!s_gauge_ready && (tick % BAT_GAUGE_PERIOD_TICKS) == 0) {
            if (cw_wake() == ESP_OK) {
                s_gauge_ready = true;
            }
        }

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

        if (tp4056_usb_present_raw()) {
            plugged_ui_enter_cnt++;
            plugged_ui_exit_cnt = 0;
        } else {
            plugged_ui_exit_cnt++;
            plugged_ui_enter_cnt = 0;
        }
        if (plugged_ui_enter_cnt >= BAT_PLUGGED_UI_ENTER_CNT) {
            plugged_ui_latched = true;
        } else if (plugged_ui_exit_cnt >= BAT_PLUGGED_UI_EXIT_CNT) {
            plugged_ui_latched = false;
        }

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
            full_raw_stable = false;
            full_latched = false;
        }
        if (full_raw_stable && (now - full_since >= pdMS_TO_TICKS(BAT_FULL_STABLE_MS))) {
            full_latched = true;
        }

        int estimated = -1;
        if (s_gauge_ready && (tick % BAT_GAUGE_PERIOD_TICKS) == 0) {
            int mv = 0;
            int soc = -1;
            bool soc_valid = false;
            if (cw_read_sample(&mv, &soc, &soc_valid) == ESP_OK && mv > 0) {
                smoothed_mv = (smoothed_mv == 0) ? mv : (smoothed_mv * 3 + mv) / 4;
                if (soc_valid) {
                    last_soc = soc;
                }
                if (display < 0) {
                    display = soc_valid ? soc : voltage_to_percent(smoothed_mv);
                    last_track = now;
                    ESP_LOGI(TAG, "first sample %dmV soc=%d", mv, soc);
                }
            }
        }
        if (last_soc >= 0) {
            estimated = last_soc;
        } else if (smoothed_mv > 0) {
            estimated = voltage_to_percent(smoothed_mv);
        }

        battery_state_t prev = state;
        if (full_latched) {
            state = BATTERY_STATE_FULL;
        } else if (charge_stable || (plugged_latched && !full_latched)) {
            state = BATTERY_STATE_CHARGING;
        } else if (prev == BATTERY_STATE_CHARGING || prev == BATTERY_STATE_FULL ||
                   prev == BATTERY_STATE_RECOVERY) {
            state = BATTERY_STATE_DISCHARGING;
        }
        if (state != prev) {
            ESP_LOGI(TAG, "state %d -> %d (plug=%d chg=%d full=%d v=%dmV)",
                     prev, state, plugged_latched, charge_stable, full_latched, smoothed_mv);
        }

        if (display >= 0 && estimated >= 0) {
            if (state == BATTERY_STATE_FULL) {
                display = 100;
            } else if (now - last_track >= pdMS_TO_TICKS(BAT_TRACK_MS)) {
                if (estimated > display) {
                    display++;
                } else if (estimated < display) {
                    display--;
                }
                last_track = now;
            }
            display = clamp_percent(display);
        }

        taskENTER_CRITICAL(&s_info_mux);
        s_info.voltage_mv = smoothed_mv;
        if (estimated >= 0) {
            s_info.estimated_percent = estimated;
        }
        if (display >= 0) {
            s_info.display_percent = display;
        }
        s_info.charging = charge_stable && !full_latched;
        s_info.full = full_latched;
        s_info.plugged = plugged_latched;
        s_info.plugged_ui = plugged_ui_latched;
        s_info.state = state;
        taskEXIT_CRITICAL(&s_info_mux);

        if (smoothed_mv > 0 && (tick % BAT_LOG_PERIOD_TICKS) == 0) {
            ESP_LOGI(TAG, "v=%dmV soc=%d show=%d alert=%d chg=%d full=%d plug=%d",
                     smoothed_mv, last_soc, display,
                     gpio_get_level(BAT_ALERT_GPIO),
                     charge_stable, full_latched, plugged_latched);
        }

        tick++;
        vTaskDelay(pdMS_TO_TICKS(BAT_POLL_PERIOD_MS));
    }
}

void battery_adc_init(void)
{
    battery_gpio_init();
    xTaskCreatePinnedToCore(battery_monitor_task, "bat_monitor",
                            4096, NULL, 5, NULL, 0);
}

int get_battery_percentage(void)
{
    taskENTER_CRITICAL(&s_info_mux);
    int pct = s_info.display_percent;
    taskEXIT_CRITICAL(&s_info_mux);
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
