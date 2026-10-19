#ifndef __BAT_PERCENT_H_
#define __BAT_PERCENT_H_

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
#include "driver/gpio.h"
#include "driver/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * CW2015CHBD，挂在 RX8130 / LSM6DSR 同一条 I2C0（SDA GPIO15，SCL GPIO16）。
 * ALRT# 开漏、低有效，接到 GPIO8。电芯电压和百分比由芯片给出。
 */
#define CW2015_I2C_PORT          I2C_NUM_0
#define CW2015_I2C_ADDR          0x62
#define CW2015_VERSION_EXPECT    0x73
#define BAT_ALERT_GPIO           GPIO_NUM_8

/*
 * 充电状态输入：TP4056 的 CHRG / STDBY，开漏输出、低电平有效。
 *   CHRG  低 = 正在充电；STDBY 低 = 充电完成（待机）。
 *   不插 USB 时两脚高阻，由内部上拉读为高 = 不活动。
 * 注意：GPIO19/20 是 ESP32-S3 原生 USB D-/D+。当前控制台走 UART0，
 * 重配置这两脚仅使 USB-Serial-JTAG 副控制台失效（不影响 UART 日志）。
 */
#define BAT_CHARGE_GPIO          GPIO_NUM_19   /* TP4056 CHRG  */
#define BAT_FULL_GPIO            GPIO_NUM_20   /* TP4056 STDBY */
#define BAT_CHARGE_ACTIVE_LEVEL  0
#define BAT_FULL_ACTIVE_LEVEL    0

/* ---------------- 状态机 ---------------- */
typedef enum {
    BATTERY_STATE_DISCHARGING = 0,
    BATTERY_STATE_CHARGING,
    BATTERY_STATE_FULL,
    BATTERY_STATE_RECOVERY,
} battery_state_t;

typedef struct {
    int  voltage_mv;          /* CW2015 测到的电芯电压 */
    int  estimated_percent;   /* 芯片 SOC；尚未就绪时用电压曲线兜底 */
    int  display_percent;     /* 略作平滑后的显示值，UI 只用这个 */
    bool charging;            /* CHRG 去抖后：正在充电 */
    bool full;                /* STDBY 去抖且稳定后：已充满 */
    bool plugged;             /* Type-C 已插入（慢释放锁存，供百分比状态机） */
    bool plugged_ui;          /* Type-C 已插入（快释放锁存，供 UI 颜色/图标） */
    battery_state_t state;
} battery_info_t;

/**
 * @brief 配置告警脚和充电状态脚，并启动后台监测任务。
 *        I2C 驱动需已由 rx8130_init() 装好；任务会等到总线可用再读 CW2015。
 *        保持旧函数名以兼容现有调用点。
 */
void battery_adc_init(void);

/**
 * @brief 返回平滑后的显示百分比（0~100）。UI 直接用。
 */
int get_battery_percentage(void);

/**
 * @brief 获取完整电池状态快照（电压/估算值/显示值/充电状态）。
 */
void battery_monitor_get(battery_info_t *out);

#ifdef __cplusplus
}
#endif

#endif /* __BAT_PERCENT_H_ */
