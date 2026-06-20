#include "bsp_button.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

esp_err_t bsp_button_init(const bsp_button_config_t *config)
{
    if (config == NULL || config->gpio_num < 0) {
        return ESP_ERR_INVALID_ARG;
    }

    gpio_config_t io = {
        .pin_bit_mask = 1ULL << config->gpio_num,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = config->pull_up_en,
        .pull_down_en = config->pull_down_en,
        .intr_type = GPIO_INTR_DISABLE,
    };
    return gpio_config(&io);
}

bool bsp_button_is_pressed(gpio_num_t gpio_num, bool active_low)
{
    int level = gpio_get_level(gpio_num);
    return active_low ? (level == 0) : (level != 0);
}

bool bsp_button_read_debounced(const bsp_button_config_t *config)
{
    if (config == NULL) {
        return false;
    }

    bool first = bsp_button_is_pressed(config->gpio_num, config->active_low);
    if (!first || config->debounce_ms == 0) {
        return first;
    }
    vTaskDelay(pdMS_TO_TICKS(config->debounce_ms));
    return bsp_button_is_pressed(config->gpio_num, config->active_low);
}

