/**
 * @file lsm6dsr.h
 * @brief LSM6DSRTR 6 轴 IMU 驱动（Reppo R2）
 *
 * 与 RX8130 共用 I2C_NUM_0（SDA GPIO15 / SCL GPIO16），沿用 legacy I2C 驱动；
 * 调用 lsm6dsr_init() 前必须先由 rx8130_init() 安装好 I2C 驱动。
 * INT1 -> GPIO4，INT2 -> GPIO18，推挽高有效。
 *
 * V3.7 原理图把 IMU_INT1/INT2 标到模组 IO36/IO35（脚 29/28）。N16R8 的
 * 八线 PSRAM 占用 GPIO35–37，且本工程打开了 CONFIG_SPIRAM_MODE_OCT，
 * 这两脚不能再当 GPIO。中断因此沿用已在 R2 上验证过的 GPIO4 / GPIO18，
 * 可用 -DLSM6DSR_INT1_GPIO / -DLSM6DSR_INT2_GPIO 覆盖。
 */
#ifndef LSM6DSR_H
#define LSM6DSR_H

#include <stdbool.h>
#include <stdint.h>
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_err.h"

#define LSM6DSR_ADDR_SA0_LOW    0x6A
#define LSM6DSR_ADDR_SA0_HIGH   0x6B
#define LSM6DSR_WHO_AM_I_VALUE  0x6B

#ifndef LSM6DSR_INT1_GPIO
#define LSM6DSR_INT1_GPIO       GPIO_NUM_4
#endif
#ifndef LSM6DSR_INT2_GPIO
#define LSM6DSR_INT2_GPIO       GPIO_NUM_18
#endif

#define LSM6DSR_REG_FUNC_CFG_ACCESS 0x01
#define LSM6DSR_REG_INT1_CTRL       0x0D
#define LSM6DSR_REG_INT2_CTRL       0x0E
#define LSM6DSR_REG_WHO_AM_I        0x0F
#define LSM6DSR_REG_CTRL1_XL        0x10
#define LSM6DSR_REG_CTRL2_G         0x11
#define LSM6DSR_REG_CTRL3_C         0x12
#define LSM6DSR_REG_CTRL4_C         0x13
#define LSM6DSR_REG_CTRL5_C         0x14
#define LSM6DSR_REG_CTRL9_XL        0x18
#define LSM6DSR_REG_STATUS          0x1E
#define LSM6DSR_REG_OUT_TEMP_L      0x20
#define LSM6DSR_REG_OUTX_L_G        0x22
#define LSM6DSR_REG_OUTX_L_A        0x28

#define LSM6DSR_STATUS_XLDA         0x01
#define LSM6DSR_STATUS_GDA          0x02
#define LSM6DSR_STATUS_TDA          0x04

/* INT1_CTRL / INT2_CTRL 路由位 */
#define LSM6DSR_INT_DRDY_XL         0x01
#define LSM6DSR_INT_DRDY_G          0x02

/* 自检判定窗口（|ST_ON - ST_OFF|，每轴） */
#define LSM6DSR_ST_XL_MIN_MG        40.0f
#define LSM6DSR_ST_XL_MAX_MG        1700.0f
#define LSM6DSR_ST_G_MIN_DPS        150.0f
#define LSM6DSR_ST_G_MAX_DPS        700.0f

typedef enum {
    LSM6DSR_ODR_OFF    = 0,
    LSM6DSR_ODR_12_5HZ = 1,
    LSM6DSR_ODR_26HZ   = 2,
    LSM6DSR_ODR_52HZ   = 3,
    LSM6DSR_ODR_104HZ  = 4,
    LSM6DSR_ODR_208HZ  = 5,
    LSM6DSR_ODR_416HZ  = 6,
    LSM6DSR_ODR_833HZ  = 7,
    LSM6DSR_ODR_1666HZ = 8,
} lsm6dsr_odr_t;

typedef enum {
    LSM6DSR_XL_FS_2G = 0,
    LSM6DSR_XL_FS_4G,
    LSM6DSR_XL_FS_8G,
    LSM6DSR_XL_FS_16G,
} lsm6dsr_xl_fs_t;

typedef enum {
    LSM6DSR_G_FS_250DPS = 0,
    LSM6DSR_G_FS_500DPS,
    LSM6DSR_G_FS_1000DPS,
    LSM6DSR_G_FS_2000DPS,
} lsm6dsr_g_fs_t;

typedef struct {
    lsm6dsr_odr_t   xl_odr;
    lsm6dsr_xl_fs_t xl_fs;
    lsm6dsr_odr_t   g_odr;
    lsm6dsr_g_fs_t  g_fs;
} lsm6dsr_config_t;

/* 104 Hz、±4 g、±500 dps */
#define LSM6DSR_DEFAULT_CONFIG() {          \
    .xl_odr = LSM6DSR_ODR_104HZ,            \
    .xl_fs  = LSM6DSR_XL_FS_4G,             \
    .g_odr  = LSM6DSR_ODR_104HZ,            \
    .g_fs   = LSM6DSR_G_FS_500DPS,          \
}

typedef struct {
    int16_t accel[3];
    int16_t gyro[3];
    int16_t temp;
} lsm6dsr_raw_t;

typedef struct {
    float accel_g[3];
    float gyro_dps[3];
    float temp_c;
} lsm6dsr_data_t;

typedef struct {
    float xl_delta_mg[3];
    float g_delta_dps[3];
    bool  xl_pass;
    bool  g_pass;
} lsm6dsr_self_test_t;

/** 在 0x6A / 0x6B 上查找器件，不改动寄存器。found_addr / who_am_i 可为 NULL。 */
esp_err_t lsm6dsr_probe(i2c_port_t port, uint8_t *found_addr, uint8_t *who_am_i);

/** 探测 + 软复位 + 按 cfg 配置（cfg 为 NULL 时用默认配置）。 */
esp_err_t lsm6dsr_init(i2c_port_t port, const lsm6dsr_config_t *cfg);
/** 已探测到芯片，且加速度计或陀螺仪至少有一个在采样。 */
bool      lsm6dsr_is_ready(void);
uint8_t   lsm6dsr_get_addr(void);

/** cfg 为 NULL 时用默认配置。会等到新的一组数据就绪（超时只告警）。 */
esp_err_t lsm6dsr_configure(const lsm6dsr_config_t *cfg);
/**
 * 关闭加速度计与陀螺仪（约 3 µA），并清除中断路由。
 * 之后 is_ready() 为 false，read() 返回 ESP_ERR_INVALID_STATE；
 * 芯片仍在总线上，lsm6dsr_configure() 可再次打开，不必重新 init。
 */
esp_err_t lsm6dsr_power_down(void);

esp_err_t lsm6dsr_read_reg(uint8_t reg, uint8_t *val);
esp_err_t lsm6dsr_write_reg(uint8_t reg, uint8_t val);
esp_err_t lsm6dsr_read_status(uint8_t *status);

esp_err_t lsm6dsr_read_raw(lsm6dsr_raw_t *raw);
esp_err_t lsm6dsr_read(lsm6dsr_data_t *out);

/** ST 官方流程的加速度计 + 陀螺仪自检，结束后恢复原配置。耗时约 1 s，期间保持静止。 */
esp_err_t lsm6dsr_self_test(lsm6dsr_self_test_t *out);

/** 把 INT1/INT2 对应 GPIO 配为输入（下拉，避免断线时误读高电平）。 */
esp_err_t lsm6dsr_int_gpio_init(void);
/** pin: 1 = INT1, 2 = INT2 */
int       lsm6dsr_int_level(int pin);
esp_err_t lsm6dsr_route_int(uint8_t int1_ctrl, uint8_t int2_ctrl);

#endif /* LSM6DSR_H */
