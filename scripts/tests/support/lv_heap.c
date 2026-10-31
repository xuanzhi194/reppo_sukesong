#include "lv_heap.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
typedef union {max_align_t alignment;struct {size_t size;uint32_t magic;} value;} header_t;
size_t test_lv_bytes,test_lv_blocks,test_lv_peak;
void *test_lv_malloc(size_t size) {
    header_t *h=malloc(sizeof(*h)+size);if(!h)return NULL;
    h->value.size=size;h->value.magic=0x51c0ffee;
    test_lv_bytes+=size;test_lv_blocks++;if(test_lv_bytes>test_lv_peak)test_lv_peak=test_lv_bytes;
    return h+1;
}
void test_lv_free(void *ptr) {
    if(!ptr)return;header_t *h=(header_t *)ptr-1;assert(h->value.magic==0x51c0ffee&&test_lv_blocks);
    test_lv_bytes-=h->value.size;test_lv_blocks--;h->value.magic=0;free(h);
}
void *test_lv_realloc(void *ptr,size_t size) {
    if(!ptr)return test_lv_malloc(size);if(!size){test_lv_free(ptr);return NULL;}
    header_t *h=(header_t *)ptr-1;assert(h->value.magic==0x51c0ffee);size_t old=h->value.size;
    h=realloc(h,sizeof(*h)+size);if(!h)return NULL;h->value.size=size;
    test_lv_bytes=test_lv_bytes-old+size;if(test_lv_bytes>test_lv_peak)test_lv_peak=test_lv_bytes;return h+1;
}
