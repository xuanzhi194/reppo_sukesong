"""Run production stream/start cleanup paths with deterministic RTOS/transport faults.
This checks task ownership and cleanup, not a physical ESP heap fragmentation model.
"""
from pathlib import Path
import os
import subprocess
import tempfile
import unittest

class BossNetworkTests(unittest.TestCase):
    def test_external_stack_and_all_exit_paths(self):
        root=Path(__file__).resolve().parents[2]
        source=(root/'main/boss_service.c').read_text()
        state=source[source.index('#define CHUNK'):source.index('static void status')]
        worker=source[source.index('static bool send_text'):]
        code=r'''
#include <assert.h>
#include <stdint.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "boss_service.h"
#include "boss_rules.h"
#include "boss_audio_config.h"
typedef int portMUX_TYPE;
typedef void *QueueHandle_t;
typedef void *StreamBufferHandle_t;
typedef int StaticStreamBuffer_t;
#define portMUX_INITIALIZER_UNLOCKED 0
#define portENTER_CRITICAL(x) ((void)(x))
#define portEXIT_CRITICAL(x) ((void)(x))
#define ESP_LOGI(...) ((void)0)
#define ESP_LOGW(...) ((void)0)
#define MALLOC_CAP_SPIRAM 1
#define MALLOC_CAP_8BIT 2
#define MALLOC_CAP_INTERNAL 4
#define ESP_OK 0
#define pdPASS 1
#define WEBSOCKET_EVENT_ANY 0
#define pdMS_TO_TICKS(x) (x)
#define esp_crt_bundle_attach NULL
''' + state + r'''
enum {OK_RUN, EARLY_STOP, STREAM_TASK_FAIL, ALLOC_FAIL, MIC_TASK_FAIL, MIC_TIMEOUT,
      WS_INIT_FAIL, WS_START_FAIL, CONNECT_TIMEOUT, CONFIG_FAIL, READY_TIMEOUT, AUDIO_FAIL};
static unsigned alloc_attempt,fail_alloc_at;
static int scenario,allocations,external_tasks,external_deletes,ws_stops,ws_destroys,reads;
static int64_t clock_us;
static unsigned packet_bytes=3200;
static bool slow_link;
static unsigned queued_bytes, uploaded_pcm;
static session_t *active;
static void (*scheduled)(void*);
static void *tracked_alloc(size_t n) {if(++alloc_attempt==fail_alloc_at)return NULL;void *p=calloc(1,n);if(p)allocations++;return p;}
static void tracked_free(void *p) {if(p){assert(allocations>0);allocations--;free(p);}}
#define free tracked_free
static void *heap_caps_calloc(size_t n,size_t size,int caps) {assert(caps==(MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT));return tracked_alloc(n*size);}
static void *audio_alloc(size_t n,const char *stage) {(void)stage;return scenario==ALLOC_FAIL?NULL:tracked_alloc(n);}
static bool audio_buffer_init(session_t *ctx) {ctx->audio_storage=tracked_alloc(100);ctx->audio=ctx->audio_storage;return ctx->audio!=NULL;}
static void audio_buffer_free(session_t *ctx) {free(ctx->audio_storage);ctx->audio=NULL;}
static void log_memory(const char *s) {(void)s;}
static void log_network(void) {}
static bool is_wifi_connected(void) {return true;}
static bool voice_ai_auth_header(char *s,size_t n) {snprintf(s,n,"test-only");return true;}
static void status(boss_state_t state,const char *text) {(void)text;s_view.state=state;}
static void fail(session_t *ctx,const char *text) {(void)text;ctx->failed=true;s_view.state=BOSS_ERROR;}
static void ai_worker(void *arg) {(void)arg;}
static void capture_worker(void *arg) {(void)arg;}
static int64_t esp_timer_get_time(void) {return clock_us;}
static void vTaskDelay(unsigned ms) {clock_us+=ms*1000;if(active&&(active->failed||!s_run))s_capture=false;}
static void vTaskDeleteWithCaps(void *task) {
 assert(!task&&!s_run&&!s_busy&&!s_capture&&allocations==0);external_deletes++;active=NULL;
}
static void *xQueueCreate(unsigned n,unsigned size) {(void)size;assert(n==1);return (void*)1;}
static void vQueueDelete(void *queue) {(void)queue;}
static int xTaskCreatePinnedToCoreWithCaps(void(*fn)(void*),const char *name,unsigned stack,void *arg,unsigned priority,void *handle,int core,unsigned caps) {
 (void)priority;(void)handle;assert(core==0&&caps==(MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT));
 if(fn==ai_worker){assert(stack==12288);return pdPASS;}
 assert(!strcmp(name,"boss_stream")&&stack==8192);
 if(scenario==STREAM_TASK_FAIL)return 0;
 scheduled=fn;active=arg;external_tasks++;return pdPASS;
}
static int xTaskCreatePinnedToCore(void(*fn)(void*),const char *name,unsigned stack,void *arg,unsigned priority,void *handle,int core) {
 (void)name;(void)priority;(void)handle;assert(fn==capture_worker&&stack==4096&&core==0&&arg==active);
 if(scenario==MIC_TASK_FAIL)return 0;
 if(scenario!=MIC_TIMEOUT)active->mic_ready=true;return pdPASS;
}
typedef void *esp_websocket_client_handle_t;
typedef struct {const char *uri,*headers;void *crt_bundle_attach;bool disable_auto_reconnect;unsigned network_timeout_ms,task_stack,buffer_size;} esp_websocket_client_config_t;
static void websocket_event(void) {}
static void *esp_websocket_client_init(const esp_websocket_client_config_t *cfg) {
 assert(cfg->task_stack==8192&&cfg->buffer_size==20480&&active->mic_ready);
 return scenario==WS_INIT_FAIL?NULL:(void*)2;
}
static int esp_websocket_register_events(void *ws,int event,void(*cb)(void),void *ctx) {(void)ws;(void)event;(void)cb;assert(ctx==active);return 0;}
static int esp_websocket_client_start(void *ws) {(void)ws;if(scenario==WS_START_FAIL)return -1;if(scenario!=CONNECT_TIMEOUT)active->connected=true;return 0;}
static int esp_websocket_client_send_text(void *ws,const char *text,int n,unsigned timeout) {
 (void)ws;assert(timeout==2000);
 if(strstr(text,"session.update")) {
  if(scenario==CONFIG_FAIL)return -1;
  if(scenario!=READY_TIMEOUT)active->ready=true;
 } else if(strstr(text,"session.finish"))active->finished=true;
 else {
  /* 100/200/300ms PCM stays bounded and fits one WS frame. */
  assert(n>4096&&n<=WEBSOCKET_BUFFER_BYTES);
  assert((unsigned)n>4*((packet_bytes+2)/3));
  assert((unsigned)n<4*((packet_bytes+2)/3)+200);
  if(scenario==AUDIO_FAIL)return -1;
  if(slow_link) {
   clock_us+=250000;queued_bytes+=8000; // 250ms capture while WS send blocks.
   assert(queued_bytes<=SEND_PCM_BYTES); // Old fixed 100ms sends accumulate here.
  }
 }
 return n;
}
static void esp_websocket_client_stop(void *ws) {(void)ws;ws_stops++;assert(!s_capture);}
static void esp_websocket_client_destroy(void *ws) {(void)ws;ws_destroys++;assert(!s_capture);}
static size_t xStreamBufferReceive(void *stream,void *pcm,size_t n,unsigned timeout) {
 (void)stream;assert(timeout==200&&n==SEND_PCM_BYTES);
 if(slow_link) {
  if(reads++==60){s_run=false;return 0;}
  packet_bytes=queued_bytes<n?queued_bytes:n;
  queued_bytes-=packet_bytes;uploaded_pcm+=packet_bytes;
 } else if(reads++){s_run=false;return 0;}
 assert(packet_bytes<=n);memset(pcm,0,packet_bytes);return packet_bytes;
}
static size_t xStreamBufferBytesAvailable(void *stream) {(void)stream;return 0;}
static int mbedtls_base64_encode(unsigned char *out,size_t cap,size_t *written,const unsigned char *in,size_t n) {
 (void)in;*written=4*((n+2)/3);assert(cap>*written);memset(out,'A',*written);return 0;
}
''' + worker + r'''
static void callback(const char *s,bool final) {(void)s;(void)final;}
int main(void) {
 for(unsigned round=0;round<1000;round++)for(scenario=OK_RUN;scenario<=AUDIO_FAIL;scenario++) {
  packet_bytes=3200*(1+round%3);
  reads=ws_stops=ws_destroys=0;clock_us=0;scheduled=NULL;s_view.state=BOSS_OFF;
  int tasks=external_tasks,deletes=external_deletes;
  bool started=(round%2)?boss_service_start():boss_service_start_asr(callback);
  if(scenario==STREAM_TASK_FAIL){assert(!started&&!s_busy&&!s_run&&allocations==0);continue;}
  assert(started&&s_busy&&scheduled&&external_tasks==tasks+1);
  if(scenario==EARLY_STOP)s_run=false;
  scheduled(active);
  assert(!s_busy&&!s_run&&!s_capture&&allocations==0&&external_deletes==deletes+1);
  if(scenario>=WS_START_FAIL)assert(ws_destroys==1);
  if(scenario>=CONNECT_TIMEOUT||scenario==OK_RUN)assert(ws_stops==1);
 }
 scenario=OK_RUN;
 for(unsigned fault=1;fault<=8;fault++) {
  fail_alloc_at=fault;alloc_attempt=0;reads=0;clock_us=0;scheduled=NULL;
  bool started=boss_service_start_asr(callback);
  if(started) scheduled(active);
  assert(!s_busy&&!s_run&&!s_capture&&allocations==0);
 }
 fail_alloc_at=0;slow_link=true;queued_bytes=3200;uploaded_pcm=reads=0;clock_us=0;
 assert(boss_service_start_asr(callback));scheduled(active);
 assert(uploaded_pcm==3200+59*8000&&queued_bytes==8000&&allocations==0);
 puts("PASS 60 production-worker sends with 250ms simulated WS delay: bounded 250ms backlog, no PCM skipped");
 puts("PASS 12000 stream lifecycles + each allocation failure: PSRAM stack, internal mic/WS stack, early stop, startup/allocation/mic/WS/config/audio faults, full cleanup");
}
'''
        with tempfile.TemporaryDirectory() as temp:
            path=Path(temp);(path/'test.c').write_text(code)
            subprocess.run(['cc','-std=c11','-g','-fsanitize=address,undefined','-I',str(root/'main'),str(path/'test.c'),str(root/'main/boss_rules.c'),'-o',str(path/'test')],check=True)
            subprocess.run([str(path/'test')],check=True,env=dict(os.environ,ASAN_OPTIONS='detect_leaks=0:halt_on_error=1'))
