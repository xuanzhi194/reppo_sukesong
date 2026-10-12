#ifndef LV_CONF_H
#define LV_CONF_H
#include <stdlib.h>
#include <stdio.h>
#include <execinfo.h>
#define LV_COLOR_DEPTH 16
#define LV_MEM_CUSTOM 1
#define LV_USE_THEME_DEFAULT 0
#define LV_USE_THEME_BASIC 0
#define LV_USE_THEME_MONO 0
#define LV_USE_LOG 1
#define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
#define LV_LOG_PRINTF 1
#define LV_USE_ASSERT_NULL 1
#define LV_USE_ASSERT_MALLOC 1
#define LV_USE_ASSERT_OBJ 1
#define LV_ASSERT_HANDLER do { void *frames[32]; int n = backtrace(frames, 32); backtrace_symbols_fd(frames, n, 2); fprintf(stderr, "LVGL assertion: %s:%d\n", __FILE__, __LINE__); abort(); } while(0);
#endif
