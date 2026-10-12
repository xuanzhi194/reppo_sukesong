#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define INSPIRATION_LIMIT 500u
#define INSPIRATION_TITLE_BYTES 64u
#define INSPIRATION_TEXT_BYTES 768u
#define INSPIRATION_RECORD_BYTES 1632u

typedef struct {
    char time[20];                 /* RTC wall clock; empty if not calibrated. */
    char title[INSPIRATION_TITLE_BYTES];
    char original[INSPIRATION_TEXT_BYTES];
    char text[INSPIRATION_TEXT_BYTES];
} inspiration_record_t;

typedef enum {
    INSPIRATION_OK = 0,
    INSPIRATION_IO,
    INSPIRATION_FULL,
    INSPIRATION_INVALID,
    INSPIRATION_NO_SPACE,
} inspiration_status_t;

/* Single storage worker owns these functions. No LVGL or heap allocations. */
inspiration_status_t inspiration_storage_init(const char *path);
unsigned inspiration_storage_count(void);
inspiration_status_t inspiration_storage_read(unsigned newest_index, inspiration_record_t *out);
inspiration_status_t inspiration_storage_add(const inspiration_record_t *record);
inspiration_status_t inspiration_storage_delete(unsigned newest_index);
/* Copy complete UTF-8 code points, never a partial Chinese character. */
void inspiration_text_copy(char *dst, size_t capacity, const char *src);
