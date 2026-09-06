#pragma once

/** Restore last care-activity day from NVS. */
void keep_machine_init(void);

/** Record a focus or completed-todo day so the olive tree stays healthy. */
void keep_note_activity(void);

/** Refresh today's date off the LVGL lock (I2C). Call from the main loop. */
void keep_machine_tick(void);

/** Re-read RTC into the cached calendar day after BLE phone-time sync. */
void keep_machine_refresh_today(void);

/** After setup_scr_screen_keep, bind widgets if Guider cleaned the main page first. */
void keep_machine_poll(void);
