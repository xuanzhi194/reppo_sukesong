"""Exercise actual capture lifecycle while cloud setup is pending or failing."""
from pathlib import Path
import os
import subprocess
import tempfile
import unittest

class BossCaptureTests(unittest.TestCase):
    def test_prime_upload_and_teardown(self):
        root = Path(__file__).resolve().parents[2]
        source = (root / 'main/boss_service.c').read_text()
        worker = source[source.index('static void capture_worker'):source.index('static bool send_text')]
        code = r'''
#include <assert.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define CHUNK 1600
#include "boss_audio_config.h"
#define ESP_LOGW(...) ((void)0)
#define ESP_LOGI(...) ((void)0)
#define ESP_OK 0
#define ESP_ERR_NO_MEM 257
#define ZTS6672_SLOT_UNKNOWN 0
#define ZTS6672_SLOT_LEFT 1
#define BOSS_LISTENING 2
#define portENTER_CRITICAL(x) ((void)(x))
#define portEXIT_CRITICAL(x) ((void)(x))
typedef int esp_err_t;
typedef struct { atomic_bool failed,ready,mic_ready; void *audio; } session_t;
static atomic_bool s_run,s_capture;
static int s_lock;
static struct {unsigned level;} s_view;
static unsigned init_count,deinit_count,allocs,reads,sends,listening,errors;
static int scenario;
static unsigned buffered, recovered, ring_used, ring_read, ring_write, consumed;
static uint8_t ring[BOSS_AUDIO_BUFFER_BYTES];
static bool saw_buffering;
static void consume_frames(unsigned count) {
    while (count-- && ring_used >= CHUNK*2) {
        for(unsigned i=0;i<CHUNK;i++) {
            int16_t value;
            uint8_t bytes[2];
            bytes[0]=ring[ring_read++ % sizeof(ring)];
            bytes[1]=ring[ring_read++ % sizeof(ring)];
            memcpy(&value,bytes,2);assert(value==(int16_t)(consumed+1));
        }
        ring_used-=CHUNK*2;consumed++;
    }
}
static session_t *active;
static void log_memory(const char *s) {(void)s;}
static void *audio_alloc(size_t n,const char *s) {(void)s;allocs++;return calloc(1,n);}
static void tracked_free(void *p) { if(p) {assert(allocs);allocs--;free(p);} }
#define free tracked_free
static void fail(session_t *ctx,const char *s) {(void)s;errors++;ctx->failed=true;}
static void status(int state,const char *s) {
    assert(state==BOSS_LISTENING);assert(active->ready);listening++;
    if(strstr(s,"缓冲中")) {buffered++;saw_buffering=true;}
    else if(saw_buffering) {recovered++;saw_buffering=false;}
}
static int zts6672_init_streaming(unsigned rate) {assert(rate==16000);init_count++;return scenario==1?ESP_ERR_NO_MEM:ESP_OK;}
static void zts6672_deinit(void) {deinit_count++;}
static int zts6672_detect_slot(int t,void*a,void*b) {(void)t;(void)a;(void)b;return 0;}
static int zts6672_get_slot(void) {return 0;}
static void zts6672_set_slot(int slot) {assert(slot==1);}
static int zts6672_read(int32_t *raw,size_t n,size_t *got,unsigned timeout) {
    assert(n==CHUNK&&timeout==200);reads++;*got=CHUNK;
    for(unsigned i=0;i<n;i++) raw[i]=i%2?40000:-40000;
    if(scenario>=5) {
        active->ready=true;
        for(unsigned i=0;i<n;i++) raw[i]=reads;
        if(scenario==5) {
            if((reads-1)%80>=30) consume_frames(2); // Repeated 3s stalls, then faster drain.
            if(reads==241) {consume_frames(100);s_run=false;}
        }
    }
    if(scenario==2) return -1; // Read failure must deinit.
    if(scenario==3) active->failed=true; // Connection failed while priming.
    if(scenario==0&&reads==2) active->ready=true;
    if(scenario==0&&reads==3) s_run=false;
    return ESP_OK;
}
static size_t xStreamBufferSend(void *q,void *data,size_t n,int wait) {
    (void)q;(void)wait;assert(active->ready);assert(n==CHUNK*2);
    if(scenario>=5) {
        if(ring_used+n>sizeof(ring)) return 0;
        for(unsigned i=0;i<n;i++) ring[ring_write++ % sizeof(ring)]=((uint8_t*)data)[i];
        ring_used+=n;
    } else { assert(reads==2);int16_t *pcm=data;assert(pcm[0]==-32768&&pcm[1]==32767); }
    sends++;return n;
}
static size_t xStreamBufferBytesAvailable(void *q) {(void)q;return scenario>=5?ring_used:CHUNK*2;}
static void vTaskDelete(void *p) {(void)p;}
''' + worker + r'''
int main(void) {
    for(scenario=0;scenario<7;scenario++) {
        session_t ctx={0};active=&ctx;s_run=scenario!=4;s_capture=true;
        init_count=deinit_count=allocs=reads=sends=listening=errors=0;
        buffered=recovered=ring_used=ring_read=ring_write=consumed=0;saw_buffering=false;
        capture_worker(&ctx);assert(!s_capture&&!allocs);
        if(scenario==0) {assert(ctx.mic_ready&&reads==3&&sends==1&&listening==1&&deinit_count==1);}
        if(scenario==1) {assert(errors==1&&!ctx.mic_ready&&deinit_count==0);}
        if(scenario==2) {assert(errors==1&&deinit_count==1&&!sends);}
        if(scenario==3) {assert(deinit_count==1&&!sends&&!listening);}
        if(scenario==4) {assert(!init_count&&!deinit_count&&!sends);}
        if(scenario==5) {assert(!errors&&consumed==240&&sends==240&&!ring_used&&buffered==3&&recovered==3);}
        if(scenario==6) {assert(errors==1&&sends==60&&reads==61&&deinit_count==1&&buffered==1&&!recovered);}
    }
    puts("PASS mic init failure, priming without upload, ready gating, PCM clipping, read failure, disconnect, cancellation, three 3s stalls with ordered recovery and bounded sustained outage");
}
'''
        with tempfile.TemporaryDirectory() as temp:
            path=Path(temp)
            (path/'test.c').write_text(code)
            subprocess.run(['cc','-std=c11','-fsanitize=address,undefined','-I',str(root/'main'),str(path/'test.c'),'-o',str(path/'test')],check=True)
            subprocess.run([str(path/'test')],check=True,env=dict(os.environ,ASAN_OPTIONS='detect_leaks=0:halt_on_error=1'))

if __name__=='__main__':
    unittest.main()
