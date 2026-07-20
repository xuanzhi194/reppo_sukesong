#include "ec06.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include <stdbool.h>
#include <stdlib.h>

static const char *TAG = "ENCODER";

// =================【用户配置区】=================
// EC06 4X：约 80 计数/圈（20 档×4）。step≈4 时约 1 档对应 1 次 UI 步进。

#define ENCODER_A_GPIO 1
#define ENCODER_B_GPIO 2

/* 每触发一次 NEXT/PREV 需要的硬件计数（越小越灵） */
/* EC06 4X 解码：4 counts ≈ 1 物理档；略降低步进阈值以提高 UI 响应灵敏度 */
#define EC_STEP_CW   6
#define EC_STEP_CCW  6

/* 连续两次 UI 步进间隔；过大会转很久才动，过小易连跳 */
#define EC_EVENT_GAP_US    40000

#define EC_IDLE_RESET_US   150000

/* 慢速旋转时，对向单次硬件计数 <= 此值 视为机械触点抖动产生的噪声，先丢弃 */
#define EC_REVERSE_NOISE      1

/* 反向噪声"预算"：累计被丢弃的反向计数达到该阈值，才认为是真实反向。
 * 同方向运动会把这个预算清零，所以只对真正混入抖动的慢速场景生效 */
#define EC_REVERSE_BUDGET     5

/* 触发一次 NEXT/PREV 之后，此时间内继续把反向小计数当噪声丢弃，
 * 防止刚切完一次又被几个抖动带回去 */
#define EC_DIR_LOCK_US        350000

/* 触发前要求主方向“净优势”至少达到该计数，抑制慢速时偶发反向毛刺 */
#define EC_DOMINANCE_MARGIN   1

#define EC_GLITCH_FILTER_NS  12000

#define EC_INVERT_DIRECTION  0
// ===============================================

static pcnt_unit_handle_t pcnt_unit = NULL;
static int32_t accumulated_count = 0;
static int64_t last_emit_time = -1;
static int64_t last_motion_time = -1;
static bool release_pending = false;
static int8_t locked_dir = 0;        /* +1 / -1 / 0；emit 后短窗口内有效 */
static int32_t reverse_budget = 0;   /* 当前已被丢弃的反向噪声累计量 */
static int32_t dir_balance = 0;      /* 方向净计数：同向增加、反向减少 */

static int ec_step_for_sign(int sign)
{
    if (sign > 0) {
        return EC_STEP_CW;
    }
    if (sign < 0) {
        return EC_STEP_CCW;
    }
    return EC_STEP_CW;
}

/* 最多只保留“一格”脉冲，多出来的直接丢掉，避免停转后连跳多项 */
static void ec_cap_accum_to_one_step(void)
{
    if (accumulated_count == 0) {
        return;
    }
    int sign = (accumulated_count > 0) ? 1 : -1;
    int step = ec_step_for_sign(sign);
    if (abs((int)accumulated_count) > step) {
        accumulated_count = sign * step;
    }
}

static void encoder_reset_idle_state(void)
{
    accumulated_count = 0;
    release_pending = false;
    locked_dir = 0;
    reverse_budget = 0;
    dir_balance = 0;
    if (pcnt_unit) {
        pcnt_unit_clear_count(pcnt_unit);
    }
}

void init_encoder_direction(void)
{
    ESP_LOGI(TAG, "Initializing EC06 Encoder (4X + software detent)...");

    gpio_set_pull_mode(ENCODER_A_GPIO, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(ENCODER_B_GPIO, GPIO_PULLUP_ONLY);

    pcnt_unit_config_t unit_config = {
        .high_limit = 1000,
        .low_limit = -1000,
    };
    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_unit));

    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = EC_GLITCH_FILTER_NS,
    };
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));

    pcnt_chan_config_t chan_a_config = {
        .edge_gpio_num = ENCODER_A_GPIO,
        .level_gpio_num = ENCODER_B_GPIO,
    };
    pcnt_channel_handle_t pcnt_chan_a = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_a_config, &pcnt_chan_a));

    pcnt_chan_config_t chan_b_config = {
        .edge_gpio_num = ENCODER_B_GPIO,
        .level_gpio_num = ENCODER_A_GPIO,
    };
    pcnt_channel_handle_t pcnt_chan_b = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_b_config, &pcnt_chan_b));

    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_a,
        PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_a,
        PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));

    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_b,
        PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_b,
        PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));

    ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit));

    encoder_reset_idle_state();
    ESP_LOGI(TAG, "Encoder ready: step=%d/%d gap=%dus (1-step cap)",
             EC_STEP_CW, EC_STEP_CCW, EC_EVENT_GAP_US);
}

uint8_t check_encoder_direction(void)
{
    int curr_hw_count = 0;
    int64_t now = esp_timer_get_time();

    ESP_ERROR_CHECK(pcnt_unit_get_count(pcnt_unit, &curr_hw_count));

    /* 长时间无动作 → 全清，包括方向锁与噪声预算 */
    if (last_motion_time >= 0 && (now - last_motion_time) > EC_IDLE_RESET_US) {
        accumulated_count = 0;
        release_pending = false;
        last_emit_time = -1;
        locked_dir = 0;
        reverse_budget = 0;
        dir_balance = 0;
        if (curr_hw_count == 0) {
            pcnt_unit_clear_count(pcnt_unit);
            return 0;
        }
        pcnt_unit_clear_count(pcnt_unit);
    }

    /* emit 后的方向锁定窗口超时 → 解锁 */
    if (locked_dir != 0 && last_emit_time >= 0 &&
        (now - last_emit_time) > EC_DIR_LOCK_US) {
        locked_dir = 0;
    }

    if (curr_hw_count != 0) {
        last_motion_time = now;

        int new_sign = (curr_hw_count > 0) ? 1 : -1;

        /* 已建立方向 = 锁定方向优先；否则用累积值的符号 */
        int est_dir = locked_dir;
        if (est_dir == 0 && accumulated_count != 0) {
            est_dir = (accumulated_count > 0) ? 1 : -1;
        }

        bool consumed = false;
        if (est_dir != 0 && new_sign != est_dir &&
            abs((int)curr_hw_count) <= EC_REVERSE_NOISE) {
            /* 反向小计数 → 视为抖动噪声，记入预算，先不动累积值 */
            reverse_budget += abs((int)curr_hw_count);
            consumed = true;

            if (reverse_budget >= EC_REVERSE_BUDGET) {
                /* 噪声累计太多 → 大概率是真实反向，接受 */
                accumulated_count = 0;
                locked_dir = 0;
                reverse_budget = 0;
                dir_balance = 0;
                accumulated_count += curr_hw_count;
                dir_balance += curr_hw_count;
                ec_cap_accum_to_one_step();
            }
        }

        if (!consumed) {
            /* 大反向计数：丢弃旧累计 + 释放方向锁 */
            if (est_dir != 0 && new_sign != est_dir) {
                accumulated_count = 0;
                locked_dir = 0;
                dir_balance = 0;
            }
            /* 真正的同向运动会清空噪声预算 */
            if (new_sign == est_dir || est_dir == 0) {
                reverse_budget = 0;
            }
            accumulated_count += curr_hw_count;
            dir_balance += curr_hw_count;
            int active_step = ec_step_for_sign((dir_balance >= 0) ? 1 : -1);
            /* 方向净计数只保留 1-step 范围，避免停转后残余影响下一次方向判定 */
            if (dir_balance > active_step) dir_balance = active_step;
            if (dir_balance < -active_step) dir_balance = -active_step;
            ec_cap_accum_to_one_step();
        }

        ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
    }

    if (release_pending) {
        release_pending = false;
        return 0;
    }

    if (accumulated_count == 0) {
        return 0;
    }

    int sign = (accumulated_count > 0) ? 1 : -1;
    int step = ec_step_for_sign(sign);
    if (abs((int)accumulated_count) < step) {
        return 0;
    }

    /* 到达步进阈值后，再做一次主方向优势判定，避免右转夹杂少量反向毛刺触发 prev */
    if (sign > 0) {
        if (dir_balance < EC_DOMINANCE_MARGIN) {
            return 0;
        }
    } else {
        if (dir_balance > -EC_DOMINANCE_MARGIN) {
            return 0;
        }
    }

    if (last_emit_time >= 0 && (now - last_emit_time) < EC_EVENT_GAP_US) {
        return 0;
    }

    last_emit_time = now;
    release_pending = true;
    accumulated_count = 0;
    reverse_budget = 0;
    dir_balance = 0;
    locked_dir = sign;   /* 锁定本次方向，抑制紧随的反向抖动 */

    if (EC_INVERT_DIRECTION) {
        sign = -sign;
    }

    if (sign > 0) {
        return 1;
    }
    return 2;
}
