#pragma once
#include <stddef.h>
extern size_t test_lv_bytes, test_lv_blocks, test_lv_peak;
void *test_lv_malloc(size_t size);
void *test_lv_realloc(void *ptr, size_t size);
void test_lv_free(void *ptr);
