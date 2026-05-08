#include "ec06.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include <stdbool.h>
#include <stdlib.h>

static const char *TAG = "ENCODER";

// =================【用户配置区】=================

// 1. 引脚定义
#define ENCODER_A_GPIO 1
#define ENCODER_B_GPIO 2

// 2. 软件步进阈值。提高阈值可降低灵敏度、减少误触：
//    8 约等于两格合并一次 UI 事件，手感更稳。
#define EC_STEP_PER_DETENT 8

// 3. 两次 UI 步进的最短间隔（微秒）。硬件滤波已经处理触点毛刺，
//    这里只限制同一次机械抖动造成的重复触发，同时允许快速连续旋转。
#define EC_EVENT_GAP_US 30000

// 4. 停顿超过该时间后，丢弃未达到一格的残余脉冲，避免下次转动被旧抖动影响。
#define EC_IDLE_RESET_US 180000

// 5. 异常情况下的累加上限。快速旋转时保留多个待发送事件，但不让残值无限堆积。
#define EC_ACCUM_LIMIT (EC_STEP_PER_DETENT * 8)

// ===============================================

static pcnt_unit_handle_t pcnt_unit = NULL;
static int accumulated_count = 0;     // 软件累加器
static int64_t last_trigger_time = -1; // 首次不经过冷却（-1 表示尚未有过有效步进）
static int64_t last_motion_time = -1;  // 最近一次读到硬件脉冲的时间
static bool release_pending = false;   // keypad 模式下，每个 NEXT/PREV 后需要插入一次释放

static int clamp_accumulated_count(int value)
{
    if (value > EC_ACCUM_LIMIT) {
        return EC_ACCUM_LIMIT;
    }
    if (value < -EC_ACCUM_LIMIT) {
        return -EC_ACCUM_LIMIT;
    }
    return value;
}

/**
 * @brief 初始化编码器驱动
 * 使用双通道 4X 正交解码模式，抗干扰能力最强
 */
void init_encoder_direction() {
    ESP_LOGI(TAG, "Initializing EC06 Encoder...");

    // 1. 强制开启 GPIO 内部上拉 (机械触点必须要有上拉，否则悬空会乱跳)
    gpio_set_pull_mode(ENCODER_A_GPIO, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(ENCODER_B_GPIO, GPIO_PULLUP_ONLY);

    // 2. 创建 PCNT 单元
    //    设置较大的限值，防止溢出，虽然我们会定期清零
    pcnt_unit_config_t unit_config = {
        .high_limit = 1000,
        .low_limit = -1000,
    };
    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_unit));

    // 3. 硬件毛刺滤波（纳秒→内部计数，见 pcnt_unit_set_glitch_filter，须使 thres ≤ 1023）
    //    在常见 80MHz APB 下，约 12.7µs 为上限；取 12µs 作为较严滤抖仍合法的最大值之一。
    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 12000,
    };
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));

    // 4. 配置通道 A
    pcnt_chan_config_t chan_a_config = {
        .edge_gpio_num = ENCODER_A_GPIO,
        .level_gpio_num = ENCODER_B_GPIO,
    };
    pcnt_channel_handle_t pcnt_chan_a = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_a_config, &pcnt_chan_a));

    // 5. 配置通道 B
    pcnt_chan_config_t chan_b_config = {
        .edge_gpio_num = ENCODER_B_GPIO,
        .level_gpio_num = ENCODER_A_GPIO,
    };
    pcnt_channel_handle_t pcnt_chan_b = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_b_config, &pcnt_chan_b));

    // =========================================================
    // 6. 配置 4X 正交解码逻辑 (核心)
    //    逻辑：利用 A/B 相位差，实现正转 +1，反转 -1
    // =========================================================
    
    // 通道 A 动作：
    // 上升沿：若 B 高电平->减，若 B 低电平->加
    // 下降沿：若 B 高电平->加，若 B 低电平->减
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));

    // 通道 B 动作 (与 A 形成互补)：
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_b, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));

    // 7. 启动计数器
    ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit));
    
    ESP_LOGI(TAG, "Encoder initialized successfully (4X Mode)");
}

/**
 * @brief 检测编码器旋转事件
 * @return 0: 无动作, 1: 顺时针(NEXT, lv_port 映射键值 1), 2: 逆时针(PREV, 键值 2)
 */
uint8_t check_encoder_direction() {
    int curr_hw_count = 0;
    int64_t now = esp_timer_get_time();
    
    // 1. 读取硬件计数器当前的增量
    ESP_ERROR_CHECK(pcnt_unit_get_count(pcnt_unit, &curr_hw_count));
    
    // 2. 停顿后只清掉不足一格的残余脉冲，完整步进会继续派发给 UI。
    if (abs(accumulated_count) < EC_STEP_PER_DETENT && last_motion_time >= 0 && (now - last_motion_time) > EC_IDLE_RESET_US) {
        accumulated_count = 0;
    }

    // 3. 始终代数相加，保留快速旋转产生的多格事件，方向由累加值符号决定。
    if (curr_hw_count != 0) {
        last_motion_time = now;
        accumulated_count += curr_hw_count;
        ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
        accumulated_count = clamp_accumulated_count(accumulated_count);
    }

    // 4. 当前输入驱动把旋钮映射为 keypad。连续两次返回 NEXT/PREV 会被 LVGL 视为
    //    同一次按住，所以每个旋转事件后先返回一次 0，形成 PRESS -> RELEASE。
    if (release_pending) {
        release_pending = false;
        return 0;
    }

    if (accumulated_count == 0) {
        return 0;
    }

    // 5. 达到一格才触发；间隔期内保留累加值，下一次 read 继续派发，快速旋转不丢步。
    if (abs(accumulated_count) < EC_STEP_PER_DETENT) {
        return 0;
    }

    if (last_trigger_time >= 0 && (now - last_trigger_time) < EC_EVENT_GAP_US) {
        return 0;
    }

    last_trigger_time = now;
    release_pending = true;
    int sign = (accumulated_count > 0) ? 1 : -1;
    accumulated_count -= sign * EC_STEP_PER_DETENT;

    if (sign > 0) {
        ESP_LOGD(TAG, "Action: NEXT >>");
        return 1;
    }
    ESP_LOGD(TAG, "Action: << PREV");
    return 2;
}