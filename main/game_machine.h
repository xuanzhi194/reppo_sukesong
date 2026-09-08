#pragma once

/* Called after lv_task_handler(), with lvgl_mutex already held. */
void game_machine_poll(void);

/* Called from the same main task, outside lvgl_mutex; never touches widgets. */
void game_machine_led_poll(void);

/* Same main task, outside lvgl_mutex. Defers Flash I/O until gameplay stops. */
void game_machine_storage_poll(void);
