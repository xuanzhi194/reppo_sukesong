#include "bsp_battery_adc.h"

#include "esp_log.h"
#include <string.h>

static const char *TAG = "bsp_battery_adc";

static adc_oneshot_unit_handle_t s_adc;
static adc_cali_handle_t s_cali;
static bsp_battery_adc_config_t s_cfg;
static bool s_initialized;

esp_err_t bsp_battery_adc_init(const bsp_battery_adc_config_t *config)
{
    if (config == NULL || config->sample_count == 0 ||
        config->voltage_divider_ratio <= 0.0f || config->max_mv <= config->min_mv) {
        return ESP_ERR_INVALID_ARG;
    }
    memcpy(&s_cfg, config, sizeof(s_cfg));

    adc_oneshot_unit_init_cfg_t unit_cfg = {
        .unit_id = s_cfg.unit_id,
    };
    esp_err_t err = adc_oneshot_new_unit(&unit_cfg, &s_adc);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        return err;
    }

    adc_oneshot_chan_cfg_t chan_cfg = {
        .bitwidth = s_cfg.bitwidth,
        .atten = s_cfg.atten,
    };
    err = adc_oneshot_config_channel(s_adc, s_cfg.channel, &chan_cfg);
    if (err != ESP_OK) {
        return err;
    }

    if (s_cfg.use_curve_fitting_calibration) {
        adc_cali_curve_fitting_config_t cali_cfg = {
            .unit_id = s_cfg.unit_id,
            .chan = s_cfg.channel,
            .atten = s_cfg.atten,
            .bitwidth = s_cfg.bitwidth,
        };
        err = adc_cali_create_scheme_curve_fitting(&cali_cfg, &s_cali);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "ADC calibration unavailable: %s", esp_err_to_name(err));
            s_cali = NULL;
        }
    }

    s_initialized = true;
    return ESP_OK;
}

esp_err_t bsp_battery_adc_deinit(void)
{
    if (s_cali) {
        adc_cali_delete_scheme_curve_fitting(s_cali);
        s_cali = NULL;
    }
    if (s_adc) {
        adc_oneshot_del_unit(s_adc);
        s_adc = NULL;
    }
    s_initialized = false;
    return ESP_OK;
}

esp_err_t bsp_battery_adc_read_raw(int *raw)
{
    if (!s_initialized || raw == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    int sum = 0;
    for (uint8_t i = 0; i < s_cfg.sample_count; i++) {
        int sample = 0;
        esp_err_t err = adc_oneshot_read(s_adc, s_cfg.channel, &sample);
        if (err != ESP_OK) {
            return err;
        }
        sum += sample;
    }
    *raw = sum / s_cfg.sample_count;
    return ESP_OK;
}

esp_err_t bsp_battery_adc_read_pin_mv(int *pin_mv)
{
    if (pin_mv == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    int raw = 0;
    esp_err_t err = bsp_battery_adc_read_raw(&raw);
    if (err != ESP_OK) {
        return err;
    }

    if (s_cali) {
        return adc_cali_raw_to_voltage(s_cali, raw, pin_mv);
    }

    *pin_mv = raw * 3100 / 4095;
    return ESP_OK;
}

esp_err_t bsp_battery_adc_read_battery_mv(int *battery_mv)
{
    if (battery_mv == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    int pin_mv = 0;
    esp_err_t err = bsp_battery_adc_read_pin_mv(&pin_mv);
    if (err != ESP_OK) {
        return err;
    }
    *battery_mv = (int)(pin_mv * s_cfg.voltage_divider_ratio + 0.5f);
    return ESP_OK;
}

esp_err_t bsp_battery_adc_read_percent(int *percent)
{
    if (percent == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    int mv = 0;
    esp_err_t err = bsp_battery_adc_read_battery_mv(&mv);
    if (err != ESP_OK) {
        return err;
    }

    int value = (mv - s_cfg.min_mv) * 100 / (s_cfg.max_mv - s_cfg.min_mv);
    if (value < 0) {
        value = 0;
    } else if (value > 100) {
        value = 100;
    }
    *percent = value;
    return ESP_OK;
}

