#pragma once
#include <stdbool.h>
void hey_reppo_init(void);             /* After NVS init, before workers. */
void hey_reppo_storage_poll(void);     /* Internal-stack NVS worker only. */
void hey_reppo_poll(void);             /* LVGL mutex held. */
bool hey_reppo_owns_audio(void);        /* Atomic, safe for IMU. */
void hey_reppo_yield(void);            /* LVGL thread: let a manual recorder win. */
