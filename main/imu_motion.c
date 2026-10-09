/**
 * @file imu_motion.c
 * @brief 26 Hz 轮询 LSM6DSR。不在 LVGL 任务里读 I2C。
 *
 * 拿起：息屏后先记下静止重力方向，连续两帧角速度或倾角超过阈值才唤醒。
 * 晃动：闹钟响起约 400 ms 后，一帧很猛或连续两帧较快就停蜂鸣和流水灯。
 * 屏幕超时、扣下息屏都不在这里改。
 */
#include "imu_motion.h"
#include "lsm6dsr.h"
#include "custom.h"
#include "voice_todo.h"
#include "buzzer.h"
#include "app_globals.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <math.h>

static const char *TAG = "IMU";

#define IMU_POLL_MS             50
#define REST_GYRO_DPS           18.0f
#define PICKUP_GYRO_DPS         60.0f
#define PICKUP_TILT_COS         0.9063f   /* 约 25° */
#define SHAKE_GYRO_DPS          140.0f
#define SHAKE_GYRO_STRONG_DPS   220.0f
#define SHAKE_ARM_MS            400
#define VOICE_KNOCK_DPS         85.0f
#define VOICE_KNOCK_GAP_MIN_MS  90
#define VOICE_KNOCK_GAP_MAX_MS  800
#define VOICE_KNOCK_ACCEL       0.38f

static bool s_was_asleep;
static bool s_rest_valid;
static float s_rest[3];
static uint8_t s_pickup_hits;
static uint8_t s_shake_hits;
static bool s_alarm_was_running;
static TickType_t s_alarm_arm_tick;
static bool s_knock_hot;
static bool s_knock_have;
static bool s_knock_tick_on;
static float s_knock_peak;
static TickType_t s_knock_tick;
static TickType_t s_knock_cool;
static TickType_t s_knock_tick_at;

static float gyro_mag(const lsm6dsr_data_t *d)
{
    float x = d->gyro_dps[0];
    float y = d->gyro_dps[1];
    float z = d->gyro_dps[2];
    return sqrtf(x * x + y * y + z * z);
}

static void consider_pickup(const lsm6dsr_data_t *d, float gmag)
{
    float ax, ay, az, dot, mag, rmag;

    if (!idle_sleep_is_asleep()) {
        s_was_asleep = false;
        s_rest_valid = false;
        s_pickup_hits = 0;
        return;
    }
    if (!s_was_asleep) {
        s_was_asleep = true;
        s_rest_valid = false;
        s_pickup_hits = 0;
    }
    if (!s_rest_valid) {
        if (gmag < REST_GYRO_DPS) {
            s_rest[0] = d->accel_g[0];
            s_rest[1] = d->accel_g[1];
            s_rest[2] = d->accel_g[2];
            s_rest_valid = true;
            s_pickup_hits = 0;
        } else if (++s_pickup_hits >= 3) {
            /* 息屏瞬间人已经拿在手里，等不到静止基准，持续转动也算拿起。 */
            ESP_LOGI(TAG, "pickup before rest gyro %.0f dps, wake", gmag);
            idle_sleep_request_wake();
            s_pickup_hits = 0;
        }
        return;
    }

    ax = d->accel_g[0];
    ay = d->accel_g[1];
    az = d->accel_g[2];
    dot = ax * s_rest[0] + ay * s_rest[1] + az * s_rest[2];
    mag = sqrtf(ax * ax + ay * ay + az * az);
    rmag = sqrtf(s_rest[0] * s_rest[0] + s_rest[1] * s_rest[1] + s_rest[2] * s_rest[2]);
    if (gmag >= PICKUP_GYRO_DPS ||
        (mag > 0.5f && rmag > 0.5f && (dot / (mag * rmag)) < PICKUP_TILT_COS)) {
        if (++s_pickup_hits >= 2) {
            ESP_LOGI(TAG, "pickup gyro %.0f dps, wake", gmag);
            idle_sleep_request_wake();
            s_pickup_hits = 0;
        }
    } else {
        s_pickup_hits = 0;
    }
}

static void consider_shake(float gmag)
{
    bool running = buzzer_timeout_task_running;

    if (!running) {
        s_alarm_was_running = false;
        s_shake_hits = 0;
        return;
    }
    if (!s_alarm_was_running) {
        s_alarm_was_running = true;
        s_alarm_arm_tick = xTaskGetTickCount();
        s_shake_hits = 0;
    }
    if ((xTaskGetTickCount() - s_alarm_arm_tick) < pdMS_TO_TICKS(SHAKE_ARM_MS)) {
        return;
    }
    if (gmag >= SHAKE_GYRO_STRONG_DPS || gmag >= SHAKE_GYRO_DPS) {
        s_shake_hits++;
        if (gmag >= SHAKE_GYRO_STRONG_DPS || s_shake_hits >= 2) {
            ESP_LOGI(TAG, "shake %.0f dps, stop pomodoro alarm", gmag);
            pomodoro_alarm_dismiss();
            s_shake_hits = 0;
        }
    } else {
        s_shake_hits = 0;
    }
}

static void voice_knock_tick_off(void)
{
    if (!s_knock_tick_on) {
        return;
    }
    buzzer_set_sound(2300, 0);
    s_knock_tick_on = false;
}

static void voice_knock_tick_on(void)
{
    buzzer_set_sound(2300, 16);
    s_knock_tick_on = true;
    s_knock_tick_at = xTaskGetTickCount();
}

/** 两下晃动进语音页。第一下短促一响，第二下进入。峰值回落即可再计，不必完全静止。 */
static void consider_voice_knock(const lsm6dsr_data_t *d, float gmag)
{
    float ax = d->accel_g[0];
    float ay = d->accel_g[1];
    float az = d->accel_g[2];
    float amag = sqrtf(ax * ax + ay * ay + az * az);
    float adev = amag > 1.0f ? amag - 1.0f : 1.0f - amag;
    bool impulse = gmag >= VOICE_KNOCK_DPS || adev >= VOICE_KNOCK_ACCEL;
    TickType_t now = xTaskGetTickCount();

    if (s_knock_tick_on &&
        (now - s_knock_tick_at) >= pdMS_TO_TICKS(40)) {
        voice_knock_tick_off();
    }
    if (buzzer_timeout_task_running || focus_timer_is_running() ||
        voice_todo_is_open() || now < s_knock_cool) {
        s_knock_hot = false;
        s_knock_have = false;
        return;
    }
    if (!s_knock_hot) {
        if (!impulse) {
            if (s_knock_have &&
                (now - s_knock_tick) > pdMS_TO_TICKS(VOICE_KNOCK_GAP_MAX_MS)) {
                s_knock_have = false;
            }
            return;
        }
        s_knock_hot = true;
        s_knock_peak = gmag;
        if (s_knock_have) {
            uint32_t gap = (uint32_t)(now - s_knock_tick) * (uint32_t)portTICK_PERIOD_MS;
            if (gap >= VOICE_KNOCK_GAP_MIN_MS && gap <= VOICE_KNOCK_GAP_MAX_MS) {
                ESP_LOGI(TAG, "double knock %.0f dps gap %u, open voice", gmag, (unsigned)gap);
                voice_knock_tick_off();
                if (idle_sleep_is_asleep()) {
                    idle_sleep_request_wake();
                }
                voice_todo_request_enter();
                s_knock_have = false;
                s_knock_cool = now + pdMS_TO_TICKS(900);
                return;
            }
        }
        ESP_LOGI(TAG, "knock 1 %.0f dps", gmag);
        s_knock_tick = now;
        s_knock_have = true;
        voice_knock_tick_on();
        return;
    }
    if (gmag > s_knock_peak) {
        s_knock_peak = gmag;
    }
    {
        float release = s_knock_peak * 0.62f;
        if (release < 55.0f) {
            release = 55.0f;
        }
        if (gmag < release && adev < 0.22f) {
            s_knock_hot = false;
        }
    }
}

static void imu_motion_task(void *param)
{
    (void)param;
    for (;;) {
        lsm6dsr_data_t sample;
        float gmag;

        vTaskDelay(pdMS_TO_TICKS(IMU_POLL_MS));
        if (!lsm6dsr_is_ready() || lsm6dsr_read(&sample) != ESP_OK) {
            continue;
        }
        gmag = gyro_mag(&sample);
        consider_pickup(&sample, gmag);
        consider_shake(gmag);
        consider_voice_knock(&sample, gmag);
    }
}

void imu_motion_start(void)
{
    if (!lsm6dsr_is_ready()) {
        return;
    }
    if (xTaskCreatePinnedToCore(imu_motion_task, "imu_motion", 4096, NULL, 4,
                                NULL, 1) != pdPASS) {
        ESP_LOGW(TAG, "motion task create failed");
    }
}
