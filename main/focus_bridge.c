/**
 * 任务专注（todosel → pomodoro）完成与累计专注时长离线存储
 */
#include "focus_bridge.h"
#include "custom.h"
#include "todo_storage.h"
#include "nvs.h"

#define FOCUS_NVS_NS   "Timing"
#define FOCUS_NVS_KEY  "focus_min"

void focus_time_init(void)
{
    nvs_handle_t h;
    if (nvs_open(FOCUS_NVS_NS, NVS_READONLY, &h) != ESP_OK) {
        return;
    }
    int32_t stored = 0;
    if (nvs_get_i32(h, FOCUS_NVS_KEY, &stored) == ESP_OK && stored >= 0) {
        all_focus_time = (int)stored;
    }
    nvs_close(h);
}

static void focus_time_save(void)
{
    nvs_handle_t h;
    if (nvs_open(FOCUS_NVS_NS, NVS_READWRITE, &h) != ESP_OK) {
        return;
    }
    nvs_set_i32(h, FOCUS_NVS_KEY, (int32_t)all_focus_time);
    nvs_commit(h);
    nvs_close(h);
}

void focus_bridge_on_countdown_complete(void)
{
    if (!focus_complete_pending) {
        return;
    }
    focus_complete_pending = 0;

    /* 任务专注会话：标记 todo 为完成，并用番茄钟实际时长覆盖预设时长 */
    if (focus_session_active) {
        focus_session_active = 0;
        if (current_task_index >= 0 &&
            current_task_index < (int)ui_todo_list.count &&
            ui_todo_list.items[current_task_index].done == 0) {
            ui_todo_list.items[current_task_index].done = 1;
            if (focus_session_minutes > 0) {
                uint16_t actual_min = (focus_session_minutes > 65535)
                                          ? 65535
                                          : (uint16_t)focus_session_minutes;
                ui_todo_list.items[current_task_index].duration_min = actual_min;
            }
            done_sum += 1;
            todo_sum -= 1;
            (void)todo_storage_apply_ui_done_flags(&ui_todo_list);
        }
    }

    /* 任意倒计时完成均累积专注时长 */
    if (focus_session_minutes > 0) {
        all_focus_time += focus_session_minutes;
        focus_time_save();
    }
    focus_session_minutes = 0;
}

void focus_session_cancel_impl(void)
{
    focus_session_active = 0;
    focus_session_minutes = 0;
    focus_complete_pending = 0;
}
