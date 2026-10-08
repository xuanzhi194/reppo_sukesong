#include "lsm6dsr.h"

#include <math.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "lsm6dsr";

#define LSM6DSR_I2C_TIMEOUT_MS  50

#define CTRL3_C_BDU      0x40
#define CTRL3_C_IF_INC   0x04
#define CTRL3_C_SW_RESET 0x01
#define CTRL9_XL_I3C_DISABLE 0x02

static i2c_port_t       s_port  = I2C_NUM_0;
static uint8_t          s_addr  = 0;
static bool             s_ready = false;
static lsm6dsr_config_t s_cfg   = LSM6DSR_DEFAULT_CONFIG();
static float            s_xl_mg_per_lsb   = 0.122f;
static float            s_g_mdps_per_lsb  = 17.5f;

static esp_err_t wait_data_ready(uint8_t mask, int timeout_ms);

static int odr_settle_ms(lsm6dsr_odr_t odr)
{
    switch (odr) {
    case LSM6DSR_ODR_12_5HZ: return 200;
    case LSM6DSR_ODR_26HZ:   return 100;
    case LSM6DSR_ODR_52HZ:   return 60;
    case LSM6DSR_ODR_104HZ:  return 40;
    case LSM6DSR_ODR_OFF:    return 0;
    default:                 return 20;
    }
}

static const uint8_t s_xl_fs_bits[]   = { 0x00, 0x08, 0x0C, 0x04 };  /* 2/4/8/16 g */
static const float   s_xl_sens_mg[]   = { 0.061f, 0.122f, 0.244f, 0.488f };
static const uint8_t s_g_fs_bits[]    = { 0x00, 0x04, 0x08, 0x0C };  /* 250/500/1000/2000 dps */
static const float   s_g_sens_mdps[]  = { 8.75f, 17.5f, 35.0f, 70.0f };

static esp_err_t reg_read_at(i2c_port_t port, uint8_t addr, uint8_t reg, uint8_t *buf, size_t len)
{
    return i2c_master_write_read_device(port, addr, &reg, 1, buf, len,
                                        pdMS_TO_TICKS(LSM6DSR_I2C_TIMEOUT_MS));
}

static esp_err_t reg_read(uint8_t reg, uint8_t *buf, size_t len)
{
    return reg_read_at(s_port, s_addr, reg, buf, len);
}

static esp_err_t reg_write(uint8_t reg, uint8_t val)
{
    uint8_t buf[2] = { reg, val };
    return i2c_master_write_to_device(s_port, s_addr, buf, sizeof(buf),
                                      pdMS_TO_TICKS(LSM6DSR_I2C_TIMEOUT_MS));
}

static inline int16_t le16(const uint8_t *p)
{
    return (int16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

esp_err_t lsm6dsr_probe(i2c_port_t port, uint8_t *found_addr, uint8_t *who_am_i)
{
    static const uint8_t addrs[] = { LSM6DSR_ADDR_SA0_LOW, LSM6DSR_ADDR_SA0_HIGH };
    esp_err_t result = ESP_ERR_NOT_FOUND;

    for (size_t i = 0; i < sizeof(addrs); i++) {
        uint8_t id = 0;
        if (reg_read_at(port, addrs[i], LSM6DSR_REG_WHO_AM_I, &id, 1) != ESP_OK) {
            continue;
        }
        if (found_addr) *found_addr = addrs[i];
        if (who_am_i)   *who_am_i   = id;
        if (id == LSM6DSR_WHO_AM_I_VALUE) {
            return ESP_OK;
        }
        ESP_LOGW(TAG, "device at 0x%02X answered WHO_AM_I=0x%02X (expect 0x%02X)",
                 addrs[i], id, LSM6DSR_WHO_AM_I_VALUE);
        result = ESP_ERR_INVALID_RESPONSE;
    }
    return result;
}

esp_err_t lsm6dsr_configure(const lsm6dsr_config_t *cfg)
{
    lsm6dsr_config_t def = LSM6DSR_DEFAULT_CONFIG();
    if (!cfg) cfg = &def;
    if (!s_addr) return ESP_ERR_INVALID_STATE;
    if (cfg->xl_fs > LSM6DSR_XL_FS_16G || cfg->g_fs > LSM6DSR_G_FS_2000DPS ||
        cfg->xl_odr > LSM6DSR_ODR_1666HZ || cfg->g_odr > LSM6DSR_ODR_1666HZ) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = reg_write(LSM6DSR_REG_CTRL1_XL,
                              (uint8_t)((cfg->xl_odr << 4) | s_xl_fs_bits[cfg->xl_fs]));
    if (err != ESP_OK) return err;
    err = reg_write(LSM6DSR_REG_CTRL2_G,
                    (uint8_t)((cfg->g_odr << 4) | s_g_fs_bits[cfg->g_fs]));
    if (err != ESP_OK) return err;

    s_cfg = *cfg;
    s_xl_mg_per_lsb  = s_xl_sens_mg[cfg->xl_fs];
    s_g_mdps_per_lsb = s_g_sens_mdps[cfg->g_fs];
    s_ready = (cfg->xl_odr != LSM6DSR_ODR_OFF) || (cfg->g_odr != LSM6DSR_ODR_OFF);
    if (!s_ready) return ESP_OK;

    uint8_t mask = 0;
    int wait_ms = 0;
    if (cfg->xl_odr != LSM6DSR_ODR_OFF) {
        mask |= LSM6DSR_STATUS_XLDA;
        if (odr_settle_ms(cfg->xl_odr) > wait_ms) wait_ms = odr_settle_ms(cfg->xl_odr);
    }
    if (cfg->g_odr != LSM6DSR_ODR_OFF) {
        mask |= LSM6DSR_STATUS_GDA;
        if (odr_settle_ms(cfg->g_odr) > wait_ms) wait_ms = odr_settle_ms(cfg->g_odr);
    }
    err = wait_data_ready(mask, wait_ms);
    if (err == ESP_ERR_TIMEOUT) {
        ESP_LOGW(TAG, "data not ready within %d ms", wait_ms);
        return ESP_OK;
    }
    if (err != ESP_OK) s_ready = false;
    return err;
}

esp_err_t lsm6dsr_init(i2c_port_t port, const lsm6dsr_config_t *cfg)
{
    s_ready = false;
    s_port  = port;

    uint8_t addr = 0, id = 0;
    esp_err_t err = lsm6dsr_probe(port, &addr, &id);
    if (err != ESP_OK) {
        s_addr = 0;
        ESP_LOGE(TAG, "probe failed: %s", esp_err_to_name(err));
        return err;
    }
    s_addr = addr;

    err = reg_write(LSM6DSR_REG_CTRL3_C, CTRL3_C_SW_RESET);
    if (err != ESP_OK) return err;
    uint8_t c3 = CTRL3_C_SW_RESET;
    for (int i = 0; i < 20 && (c3 & CTRL3_C_SW_RESET); i++) {
        vTaskDelay(pdMS_TO_TICKS(1));
        err = reg_read(LSM6DSR_REG_CTRL3_C, &c3, 1);
        if (err != ESP_OK) return err;
    }
    if (c3 & CTRL3_C_SW_RESET) {
        ESP_LOGE(TAG, "software reset timeout");
        return ESP_ERR_TIMEOUT;
    }

    err = reg_write(LSM6DSR_REG_CTRL3_C, CTRL3_C_BDU | CTRL3_C_IF_INC);
    if (err != ESP_OK) return err;

    uint8_t c9 = 0;
    err = reg_read(LSM6DSR_REG_CTRL9_XL, &c9, 1);
    if (err == ESP_OK) {
        err = reg_write(LSM6DSR_REG_CTRL9_XL, c9 | CTRL9_XL_I3C_DISABLE);
    }
    if (err != ESP_OK) return err;

    err = lsm6dsr_configure(cfg);
    if (err != ESP_OK) return err;

    ESP_LOGI(TAG, "%s at 0x%02X (SA0=%d)",
             s_ready ? "ready" : "probed, sensors off",
             s_addr, s_addr == LSM6DSR_ADDR_SA0_HIGH);
    return ESP_OK;
}

bool lsm6dsr_is_ready(void)
{
    return s_ready;
}

uint8_t lsm6dsr_get_addr(void)
{
    return s_addr;
}

esp_err_t lsm6dsr_power_down(void)
{
    if (!s_addr) return ESP_ERR_INVALID_STATE;
    esp_err_t err = lsm6dsr_route_int(0, 0);
    if (err == ESP_OK) err = reg_write(LSM6DSR_REG_CTRL1_XL, 0);
    if (err == ESP_OK) err = reg_write(LSM6DSR_REG_CTRL2_G, 0);
    if (err == ESP_OK) {
        s_cfg.xl_odr = LSM6DSR_ODR_OFF;
        s_cfg.g_odr  = LSM6DSR_ODR_OFF;
        s_ready = false;
    }
    return err;
}

esp_err_t lsm6dsr_read_reg(uint8_t reg, uint8_t *val)
{
    if (!s_addr) return ESP_ERR_INVALID_STATE;
    return reg_read(reg, val, 1);
}

esp_err_t lsm6dsr_write_reg(uint8_t reg, uint8_t val)
{
    if (!s_addr) return ESP_ERR_INVALID_STATE;
    return reg_write(reg, val);
}

esp_err_t lsm6dsr_read_status(uint8_t *status)
{
    return lsm6dsr_read_reg(LSM6DSR_REG_STATUS, status);
}

esp_err_t lsm6dsr_read_raw(lsm6dsr_raw_t *raw)
{
    if (!raw) return ESP_ERR_INVALID_ARG;
    if (!s_ready) return ESP_ERR_INVALID_STATE;
    uint8_t buf[14];
    esp_err_t err = reg_read(LSM6DSR_REG_OUT_TEMP_L, buf, sizeof(buf));
    if (err != ESP_OK) return err;

    raw->temp = le16(&buf[0]);
    for (int i = 0; i < 3; i++) {
        raw->gyro[i]  = le16(&buf[2 + i * 2]);
        raw->accel[i] = le16(&buf[8 + i * 2]);
    }
    return ESP_OK;
}

esp_err_t lsm6dsr_read(lsm6dsr_data_t *out)
{
    if (!out) return ESP_ERR_INVALID_ARG;
    lsm6dsr_raw_t raw;
    esp_err_t err = lsm6dsr_read_raw(&raw);
    if (err != ESP_OK) return err;

    for (int i = 0; i < 3; i++) {
        out->accel_g[i]  = raw.accel[i] * s_xl_mg_per_lsb / 1000.0f;
        out->gyro_dps[i] = raw.gyro[i] * s_g_mdps_per_lsb / 1000.0f;
    }
    out->temp_c = 25.0f + raw.temp / 256.0f;
    return ESP_OK;
}

/* ---------------------------------------------------------------------------
 *  自检
 * ------------------------------------------------------------------------- */

static esp_err_t wait_data_ready(uint8_t mask, int timeout_ms)
{
    for (int t = 0; t < timeout_ms; t += 2) {
        uint8_t st = 0;
        esp_err_t err = reg_read(LSM6DSR_REG_STATUS, &st, 1);
        if (err != ESP_OK) return err;
        if (mask && (st & mask) == mask) return ESP_OK;
        vTaskDelay(pdMS_TO_TICKS(2));
    }
    return ESP_ERR_TIMEOUT;
}

/* 丢弃 1 组后对 5 组取平均，单位为 sens（mg 或 mdps）每 LSB 换算后的物理量 */
static esp_err_t average_axes(uint8_t out_reg, uint8_t drdy_mask, float sens, float avg[3])
{
    uint8_t buf[6];
    esp_err_t err = wait_data_ready(drdy_mask, 100);
    if (err == ESP_OK) err = reg_read(out_reg, buf, sizeof(buf));
    if (err != ESP_OK) return err;

    float sum[3] = { 0 };
    const int n = 5;
    for (int k = 0; k < n; k++) {
        err = wait_data_ready(drdy_mask, 100);
        if (err == ESP_OK) err = reg_read(out_reg, buf, sizeof(buf));
        if (err != ESP_OK) return err;
        for (int i = 0; i < 3; i++) sum[i] += le16(&buf[i * 2]) * sens;
    }
    for (int i = 0; i < 3; i++) avg[i] = sum[i] / n;
    return ESP_OK;
}

static esp_err_t self_test_run(lsm6dsr_self_test_t *out)
{
    float off[3], on[3];
    esp_err_t err;

    /* 加速度计：52 Hz / ±4 g，正向自检 */
    if ((err = reg_write(LSM6DSR_REG_CTRL2_G, 0x00)) != ESP_OK) return err;
    if ((err = reg_write(LSM6DSR_REG_CTRL1_XL, 0x38)) != ESP_OK) return err;
    vTaskDelay(pdMS_TO_TICKS(100));
    if ((err = average_axes(LSM6DSR_REG_OUTX_L_A, LSM6DSR_STATUS_XLDA, 0.122f, off)) != ESP_OK) return err;
    if ((err = reg_write(LSM6DSR_REG_CTRL5_C, 0x01)) != ESP_OK) return err;
    vTaskDelay(pdMS_TO_TICKS(100));
    err = average_axes(LSM6DSR_REG_OUTX_L_A, LSM6DSR_STATUS_XLDA, 0.122f, on);
    reg_write(LSM6DSR_REG_CTRL5_C, 0x00);
    reg_write(LSM6DSR_REG_CTRL1_XL, 0x00);
    if (err != ESP_OK) return err;

    out->xl_pass = true;
    for (int i = 0; i < 3; i++) {
        out->xl_delta_mg[i] = fabsf(on[i] - off[i]);
        if (out->xl_delta_mg[i] < LSM6DSR_ST_XL_MIN_MG || out->xl_delta_mg[i] > LSM6DSR_ST_XL_MAX_MG) {
            out->xl_pass = false;
        }
    }

    /* 陀螺仪：208 Hz / ±2000 dps，正向自检 */
    if ((err = reg_write(LSM6DSR_REG_CTRL2_G, 0x5C)) != ESP_OK) return err;
    vTaskDelay(pdMS_TO_TICKS(100));
    if ((err = average_axes(LSM6DSR_REG_OUTX_L_G, LSM6DSR_STATUS_GDA, 70.0f, off)) != ESP_OK) return err;
    if ((err = reg_write(LSM6DSR_REG_CTRL5_C, 0x04)) != ESP_OK) return err;
    vTaskDelay(pdMS_TO_TICKS(100));
    err = average_axes(LSM6DSR_REG_OUTX_L_G, LSM6DSR_STATUS_GDA, 70.0f, on);
    reg_write(LSM6DSR_REG_CTRL5_C, 0x00);
    reg_write(LSM6DSR_REG_CTRL2_G, 0x00);
    if (err != ESP_OK) return err;

    out->g_pass = true;
    for (int i = 0; i < 3; i++) {
        out->g_delta_dps[i] = fabsf(on[i] - off[i]) / 1000.0f;
        if (out->g_delta_dps[i] < LSM6DSR_ST_G_MIN_DPS || out->g_delta_dps[i] > LSM6DSR_ST_G_MAX_DPS) {
            out->g_pass = false;
        }
    }
    return ESP_OK;
}

esp_err_t lsm6dsr_self_test(lsm6dsr_self_test_t *out)
{
    if (!out) return ESP_ERR_INVALID_ARG;
    if (!s_ready) return ESP_ERR_INVALID_STATE;
    memset(out, 0, sizeof(*out));

    uint8_t int1 = 0, int2 = 0;
    reg_read(LSM6DSR_REG_INT1_CTRL, &int1, 1);
    reg_read(LSM6DSR_REG_INT2_CTRL, &int2, 1);
    lsm6dsr_route_int(0, 0);

    lsm6dsr_config_t saved = s_cfg;
    esp_err_t err = self_test_run(out);

    esp_err_t rerr = lsm6dsr_configure(&saved);
    if (rerr == ESP_OK) rerr = lsm6dsr_route_int(int1, int2);
    if (err == ESP_OK) err = rerr;

    ESP_LOGI(TAG, "self-test XL d=[%.0f %.0f %.0f] mg %s, G d=[%.0f %.0f %.0f] dps %s",
             out->xl_delta_mg[0], out->xl_delta_mg[1], out->xl_delta_mg[2],
             out->xl_pass ? "PASS" : "FAIL",
             out->g_delta_dps[0], out->g_delta_dps[1], out->g_delta_dps[2],
             out->g_pass ? "PASS" : "FAIL");
    return err;
}

/* ---------------------------------------------------------------------------
 *  中断引脚
 * ------------------------------------------------------------------------- */

esp_err_t lsm6dsr_int_gpio_init(void)
{
    gpio_config_t io = {
        .pin_bit_mask = (1ULL << LSM6DSR_INT1_GPIO) | (1ULL << LSM6DSR_INT2_GPIO),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    return gpio_config(&io);
}

int lsm6dsr_int_level(int pin)
{
    return gpio_get_level(pin == 2 ? LSM6DSR_INT2_GPIO : LSM6DSR_INT1_GPIO);
}

esp_err_t lsm6dsr_route_int(uint8_t int1_ctrl, uint8_t int2_ctrl)
{
    if (!s_addr) return ESP_ERR_INVALID_STATE;
    esp_err_t err = reg_write(LSM6DSR_REG_INT1_CTRL, int1_ctrl);
    if (err == ESP_OK) err = reg_write(LSM6DSR_REG_INT2_CTRL, int2_ctrl);
    return err;
}
