/**
 * @file imu_motion.h
 * @brief 低采样 IMU：息屏时识别拿起，番茄钟闹钟时识别晃动。
 */
#ifndef IMU_MOTION_H
#define IMU_MOTION_H

/** 芯片已就绪时创建轮询任务。未贴 IMU 的旧板直接返回。 */
void imu_motion_start(void);

#endif /* IMU_MOTION_H */
