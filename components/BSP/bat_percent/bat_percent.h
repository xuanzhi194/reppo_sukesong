#ifndef __BAT_PERCENT_H_
#define __BAT_PERCENT_H_

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------- 硬件配置 ---------------- */
#define BAT_ADC_CHAN            ADC_CHANNEL_7   /* GPIO8 = ADC1_CH7 */
#define BAT_ADC_ATTEN           ADC_ATTEN_DB_12 /* 0~3.1V */
/* 100k+100k 分压，理论 x2。实测电阻有误差时微调此值（x1000 定点） */
#define BAT_DIVIDER_RATIO_X1000 2000

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
    BATTERY_STATE_DISCHARGING = 0,  /* 未充电，正常用电压曲线估算 */
    BATTERY_STATE_CHARGING,         /* 充电中，显示值只允许缓慢上涨 */
    BATTERY_STATE_FULL,             /* 已充满，锁定 100% */
    BATTERY_STATE_RECOVERY,         /* 刚拔线，电压恢复期，显示值保持 */
} battery_state_t;

typedef struct {
    int  voltage_mv;          /* 电池端电压（分压还原后） */
    int  estimated_percent;   /* OCV 曲线瞬时估算值 */
    int  display_percent;     /* 平滑后的显示值，UI 只用这个 */
    bool charging;            /* CHRG 去抖后：正在充电 */
    bool full;                /* STDBY 去抖且稳定后：已充满 */
    bool plugged;             /* Type-C 已插入（CHRG|STDBY 任一为低，慢释放锁存） */
    battery_state_t state;
} battery_info_t;

/**
 * @brief 初始化 ADC + 充电状态 GPIO，并启动后台监测任务。
 *        保持旧函数名以兼容现有调用点（app_init.c）。
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
