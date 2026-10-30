/**
 * @file imu_motion.h
 * @brief 低采样 IMU：息屏时识别拿起，番茄钟闹钟时识别晃动。
 */
#ifndef IMU_MOTION_H
#define IMU_MOTION_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    float accel_g[3];
    float gyro_dps[3];
    bool valid;
    uint32_t sequence;     /* Incremented once per successful hardware read. */
    uint32_t timestamp_ms; /* FreeRTOS monotonic milliseconds, wraps naturally. */
} imu_sample_t;

/** 芯片已就绪时创建轮询任务。未贴 IMU 的旧板直接返回。 */
void imu_motion_start(void);

/** 最近一次采样的一致副本。未就绪/超过250ms未更新时 valid=false。不碰 I2C。 */
void imu_motion_latest(imu_sample_t *out);

#endif /* IMU_MOTION_H */
