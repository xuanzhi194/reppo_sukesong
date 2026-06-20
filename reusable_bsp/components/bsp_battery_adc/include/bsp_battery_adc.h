#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    adc_unit_t unit_id;
    adc_channel_t channel;
    adc_atten_t atten;
    adc_bitwidth_t bitwidth;
    float voltage_divider_ratio;
    int min_mv;
    int max_mv;
    uint8_t sample_count;
    bool use_curve_fitting_calibration;
} bsp_battery_adc_config_t;

#define BSP_BATTERY_ADC_DEFAULT_CONFIG() { \
    .unit_id = ADC_UNIT_1, \
    .channel = ADC_CHANNEL_7, \
    .atten = ADC_ATTEN_DB_12, \
    .bitwidth = ADC_BITWIDTH_DEFAULT, \
    .voltage_divider_ratio = 2.0f, \
    .min_mv = 3100, \
    .max_mv = 4200, \
    .sample_count = 16, \
    .use_curve_fitting_calibration = true, \
}

esp_err_t bsp_battery_adc_init(const bsp_battery_adc_config_t *config);
esp_err_t bsp_battery_adc_deinit(void);
esp_err_t bsp_battery_adc_read_raw(int *raw);
esp_err_t bsp_battery_adc_read_pin_mv(int *pin_mv);
esp_err_t bsp_battery_adc_read_battery_mv(int *battery_mv);
esp_err_t bsp_battery_adc_read_percent(int *percent);

#ifdef __cplusplus
}
#endif
