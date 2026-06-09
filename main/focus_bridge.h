#pragma once

/** 从 NVS 恢复 all_focus_time（Timing 命名空间） */
void focus_time_init(void);

/** 主循环：番茄钟自然倒计时结束且处于任务专注会话时处理 done + 时长 */
void focus_bridge_on_countdown_complete(void);
