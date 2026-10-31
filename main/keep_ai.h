#pragma once
#include <stdbool.h>
#include <stddef.h>
#define KEEP_AI_LINES 3
#define KEEP_AI_LINE_BYTES 160
typedef struct { char lines[KEEP_AI_LINES][KEEP_AI_LINE_BYTES]; } keep_ai_result_t;

/* UI-thread only. Snapshot inputs under LVGL lock; network/storage run in worker. */
/* Existing NVS/storage worker only: prepares Flash data before PSRAM task. */
void keep_ai_storage_poll(void);
void keep_ai_request(int today_ymd, bool chinese);
/* Manual refresh bypasses both cooldown and cached generation. One job at a time. */
void keep_ai_refresh(int today_ymd, bool chinese);
bool keep_ai_poll(void);
const char *keep_ai_text(unsigned index, bool chinese);

/* Shared, host-testable output validation and model instructions. */
const char *keep_ai_system_prompt(void);
bool keep_ai_parse(const char *json, keep_ai_result_t *out);
