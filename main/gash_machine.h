#pragma once

/** 主循环在 lv_task_handler 之后调用：setup_scr 一创建 screen_gash 就初始化。 */
void gash_machine_poll(void);
