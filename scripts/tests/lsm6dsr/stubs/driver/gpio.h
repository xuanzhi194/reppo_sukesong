#ifndef LSM6DSR_TEST_GPIO_H
#define LSM6DSR_TEST_GPIO_H
#include <stdint.h>

typedef int gpio_num_t;
typedef struct {
    uint64_t pin_bit_mask;
    int mode;
    int pull_up_en;
    int pull_down_en;
    int intr_type;
} gpio_config_t;

#define GPIO_NUM_4              4
#define GPIO_NUM_18             18
#define GPIO_MODE_INPUT         1
#define GPIO_PULLUP_DISABLE     0
#define GPIO_PULLDOWN_ENABLE    1
#define GPIO_INTR_DISABLE       0

int gpio_config(const gpio_config_t *cfg);
int gpio_get_level(gpio_num_t pin);
#endif
