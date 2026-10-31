"""Fault-inject the production AI/HTTP functions, without credentials or network.
Every cJSON/response/client allocation is counted; successful and failed jobs
must return to zero live allocations and zero HTTP clients.
"""
from pathlib import Path
import os
import subprocess
import tempfile
import unittest

class VoiceAIFaultTests(unittest.TestCase):
    def test_http_and_allocation_failures(self):
        root=Path(__file__).resolve().parents[2]
        idf=Path(os.environ.get('IDF_PATH',Path.home()/'.espressif/v5.3.5/esp-idf'))
        cjson=idf/'components/json/cJSON'
        source=(root/'main/voice_todo.c').read_text()
        functions=source[source.index('static bool voice_http_post('):source.index('static void voice_json_escape(')]
        code=r'''
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#define VOICE_RESP_MAX 8192
#define VOICE_HTTP_MS 20000
#define DASHSCOPE_KEY "test-only-never-a-real-key"
#define REFINE_URL "https://test.invalid"
#define REFINE_MODEL "test-model"
#define ESP_OK 0
#define ESP_LOGW(...) ((void)0)
#define HTTP_METHOD_POST 1
#define esp_crt_bundle_attach ((void*)1)
#define MALLOC_CAP_SPIRAM 1
#define MALLOC_CAP_8BIT 2
static unsigned live,attempt,fail_at,clients,peak;
static void *tracked_alloc(size_t n) {if(++attempt==fail_at)return NULL;void *p=malloc(n?n:1);if(p){live++;if(live>peak)peak=live;}return p;}
static void tracked_free(void *p) {if(p){assert(live);live--;free(p);}}
static void *heap_caps_malloc(size_t n,int caps) {assert(caps==(MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT));return tracked_alloc(n);}
#define free tracked_free
typedef struct {const char *url;int method,timeout_ms,buffer_size,buffer_size_tx;void *crt_bundle_attach;} esp_http_client_config_t;
typedef struct {bool opened,closed;size_t sent,expected,pos;} http_t;
typedef http_t *esp_http_client_handle_t;
enum {OK_RUN,INIT_FAIL,HEADER_FAIL,OPEN_FAIL,WRITE_FAIL,WRITE_ZERO,FETCH_FAIL,READ_FAIL,INCOMPLETE,UNAUTHORIZED,RATE_LIMIT,SERVER_ERROR,BAD_JSON,MISSING_CONTENT,LONG_CONTENT,BODY_OVERFLOW};
static int fault;
static char large[9000];
static const char *payload(void) {
 if(fault==BAD_JSON)return "not JSON";
 if(fault==MISSING_CONTENT)return "{\"choices\":[{\"message\":{}}]}";
 if(fault==LONG_CONTENT)return "{\"choices\":[{\"message\":{\"content\":\"This reply exceeds a deliberately small destination buffer\"}}]}";
 if(fault==BODY_OVERFLOW)return large;
 return "{\"choices\":[{\"message\":{\"content\":\"测试回复\"}}]}";
}
static http_t *esp_http_client_init(const esp_http_client_config_t *cfg) {
 assert(cfg->crt_bundle_attach&&cfg->timeout_ms>0);if(fault==INIT_FAIL)return NULL;
 http_t *h=tracked_alloc(sizeof(*h));if(h){memset(h,0,sizeof(*h));clients++;}return h;
}
static int esp_http_client_set_header(http_t *h,const char *key,const char *value) {(void)h;(void)key;(void)value;return fault==HEADER_FAIL?-1:0;}
static int esp_http_client_open(http_t *h,int n) {if(fault==OPEN_FAIL)return -1;h->opened=true;h->expected=n;return 0;}
static int esp_http_client_write(http_t *h,const char *text,int n) {
 assert(h->opened&&text&&n>0);if(fault==WRITE_FAIL)return -1;if(fault==WRITE_ZERO)return 0;
 int sent=n>7?7:n;h->sent+=sent;return sent; // Force partial writes on every call.
}
static int esp_http_client_fetch_headers(http_t *h) {assert(h->sent==h->expected);return fault==FETCH_FAIL?-1:(int)strlen(payload());}
static int esp_http_client_get_status_code(http_t *h) {(void)h;return fault==UNAUTHORIZED?401:fault==RATE_LIMIT?429:fault==SERVER_ERROR?503:200;}
static int esp_http_client_read(http_t *h,char *out,int cap) {
 if(fault==READ_FAIL)return -1;
 size_t n=strlen(payload())-h->pos;if(n>(size_t)cap)n=cap;if(n>11)n=11;
 memcpy(out,payload()+h->pos,n);h->pos+=n;return (int)n;
}
static bool esp_http_client_is_complete_data_received(http_t *h) {return fault!=INCOMPLETE&&fault!=READ_FAIL&&h->pos==strlen(payload());}
static int esp_http_client_close(http_t *h) {assert(!h->closed);h->closed=true;return 0;}
static int esp_http_client_cleanup(http_t *h) {assert(!h->opened||h->closed);clients--;free(h);return 0;}
''' + functions + r'''
static void run_job(bool expected) {
 struct {unsigned guard1;char out[32];unsigned guard2;} dst={.guard1=0x12345678,.guard2=0x87654321};
 bool ok=voice_ai_complete("system","untrusted user input",dst.out,sizeof(dst.out));
 assert(ok==expected);assert(dst.guard1==0x12345678&&dst.guard2==0x87654321);
 if(ok)assert(!strcmp(dst.out,"测试回复"));assert(live==0&&clients==0);
}
int main(void) {
 memset(large,'x',sizeof(large)-1);large[sizeof(large)-1]=0;
 cJSON_Hooks hooks={tracked_alloc,tracked_free};cJSON_InitHooks(&hooks);
 for(unsigned round=0;round<200;round++)for(fault=OK_RUN;fault<=BODY_OVERFLOW;fault++){attempt=0;run_job(fault==OK_RUN);}
 fault=OK_RUN;attempt=0;run_job(true);unsigned allocations=attempt;
 for(unsigned round=0;round<20;round++)for(unsigned n=1;n<=allocations;n++) {
  attempt=0;fail_at=n;
  char out[32]={0};(void)voice_ai_complete("system","input",out,sizeof(out));
  assert(live==0&&clients==0);
 }
 fail_at=0;run_job(true);
 assert(!voice_ai_complete(NULL,"input",NULL,0)&&!live&&!clients);
 printf("PASS 3200 HTTP/AI lifecycle jobs + %u allocation-fault jobs; zero retained allocations/clients; peak allocations=%u\n",20*allocations,peak);
}
'''
        with tempfile.TemporaryDirectory() as temp:
            p=Path(temp);(p/'test.c').write_text(code)
            subprocess.run(['cc','-std=c11','-O1','-g','-Wno-deprecated-declarations','-fsanitize=address,undefined','-I',str(cjson),str(p/'test.c'),str(cjson/'cJSON.c'),'-o',str(p/'test')],check=True)
            result=subprocess.run([str(p/'test')],text=True,capture_output=True,timeout=45,
                env=dict(os.environ,ASAN_OPTIONS='detect_leaks=0:halt_on_error=1'))
            self.assertEqual(result.returncode,0,result.stdout+result.stderr)
            print(result.stdout,end='')

if __name__=='__main__':unittest.main()
