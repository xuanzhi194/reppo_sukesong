"""Verify external audio allocation and cleanup under internal-RAM pressure."""
from pathlib import Path
import os
import subprocess
import tempfile
import unittest

class BossMemoryTests(unittest.TestCase):
    def test_external_ring_allocation_and_failure_cleanup(self):
        root = Path(__file__).resolve().parents[2]
        source = (root / 'main/boss_service.c').read_text()
        helpers = source[source.index('static void log_memory'):source.index('static void log_network')] + source[source.index('static void *audio_alloc'):source.index('static void capture_worker')]
        code = r'''
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define CHUNK 1600
#include "boss_audio_config.h"
#define AUDIO_CAP BOSS_AUDIO_BUFFER_BYTES
#define MALLOC_CAP_INTERNAL 1
#define MALLOC_CAP_SPIRAM 2
#define MALLOC_CAP_8BIT 4
#define ESP_LOGI(...) ((void)0)
#define ESP_LOGW(...) ((void)0)
static bool fail_alloc, fail_create;
static unsigned allocated, deleted;
static size_t requested;
static void *storage;
typedef struct { size_t size, trigger; uint8_t *data; } StaticStreamBuffer_t;
typedef StaticStreamBuffer_t *StreamBufferHandle_t;
typedef struct { StreamBufferHandle_t audio; StaticStreamBuffer_t audio_control; uint8_t *audio_storage; } session_t;
static size_t heap_caps_get_free_size(int caps) { return caps & MALLOC_CAP_INTERNAL ? 512 : 1024*1024; }
static size_t heap_caps_get_largest_free_block(int caps) { return heap_caps_get_free_size(caps); }
static void *heap_caps_malloc(size_t n,int caps) {
    assert(caps == (MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
    requested=n;if(fail_alloc) return NULL;
    assert(!storage);storage=malloc(n);assert(storage);allocated++;return storage;
}
static StreamBufferHandle_t xStreamBufferCreateStatic(size_t n,size_t trigger,uint8_t *data,StaticStreamBuffer_t *ctrl) {
    assert(n==requested&&data==storage&&trigger==CHUNK*2);
    if(fail_create) return NULL;
    ctrl->size=n;ctrl->trigger=trigger;ctrl->data=data;return ctrl;
}
static void vStreamBufferDelete(StreamBufferHandle_t stream) { assert(stream&&stream->data==storage);deleted++; }
static void tracked_free(void *p) { if(p) { assert(p==storage);free(p);storage=NULL;allocated--; } }
#define free tracked_free
''' + helpers + r'''
int main(void) {
    session_t ctx={0};
    /* Only 512 bytes of internal RAM are available. The 192KB payload still fits
     * externally, and the static control requires no dynamic internal block. */
    for(unsigned i=0;i<100;i++) {
        assert(audio_buffer_init(&ctx));assert(ctx.audio->size-1==AUDIO_CAP);
        assert(ctx.audio->trigger==3200);assert(allocated==1);
        audio_buffer_free(&ctx);assert(!ctx.audio&&!ctx.audio_storage&&!allocated);
        audio_buffer_free(&ctx); // Idempotent cleanup.
    }
    assert(deleted==100);
    fail_alloc=true;assert(!audio_buffer_init(&ctx));audio_buffer_free(&ctx);assert(!allocated);
    fail_alloc=false;fail_create=true;assert(!audio_buffer_init(&ctx));assert(allocated==1);
    audio_buffer_free(&ctx);assert(!allocated&&!ctx.audio&&!ctx.audio_storage);
    puts("PASS 512-byte internal budget, six-second external ring, 100 retries and allocation/init failure cleanup");
}
'''
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp)
            (path / 'test.c').write_text(code)
            subprocess.run(['cc', '-std=c11', '-g', '-Werror=implicit-function-declaration',
                            '-fsanitize=address,undefined', '-I', str(root / 'main'), str(path / 'test.c'), '-o', str(path / 'test')], check=True)
            subprocess.run([str(path / 'test')], check=True,
                           env=dict(os.environ, ASAN_OPTIONS='detect_leaks=0:halt_on_error=1'))

if __name__ == '__main__':
    unittest.main()
