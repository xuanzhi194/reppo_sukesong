"""Exercise the production Keep scheduler/storage/network split with fault injection."""
from pathlib import Path
import os
import subprocess
import tempfile
import unittest

class KeepWorkerTests(unittest.TestCase):
    def test_storage_handoff_and_failure_cleanup(self):
        root=Path(__file__).resolve().parents[2]
        idf=Path(os.environ.get('IDF_PATH',Path.home()/'.espressif/v5.3.5/esp-idf'))
        cjson=idf/'components/json/cJSON'
        source=(root/'main/keep_ai.c').read_text()
        pure=source[source.index('const char *keep_ai_system_prompt'):source.index('#ifdef ESP_PLATFORM')]
        worker=source[source.index('typedef struct {',source.index('#ifdef ESP_PLATFORM')):source.rindex('#endif')]
        todos=(root/'components/BSP/todo_storage/todo_storage.h').read_text()
        todos=todos[todos.index('#ifndef TODO_MAX_ITEMS'):todos.index('#include "todo_label.h"')]
        code=r'''
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "keep_ai.h"
#include "inspiration_service.h"
#define MALLOC_CAP_SPIRAM 1
#define MALLOC_CAP_8BIT 2
#define ESP_OK 0
#define pdTRUE 1
#define pdPASS 1
#define pdMS_TO_TICKS(x) (x)
#define ESP_LOGI(...) ((void)0)
typedef uint32_t TickType_t;
typedef struct {void *data;unsigned size;bool ready;} StaticQueue_t;
typedef StaticQueue_t *QueueHandle_t;
static unsigned live,attempt,fail_at,clock_ms,requests,deletes,ai_calls;
static bool external,online=true;
static void (*scheduled)(void*);static void *scheduled_arg;
enum {OK_RUN,TASK_FAIL,ARCHIVE_FAIL,IDEA_TIMEOUT,IDEA_FAIL,AI_FAIL};
static int fault;
static void *tracked_alloc(size_t n) {if(++attempt==fail_at)return NULL;void *p=malloc(n?n:1);if(p)live++;return p;}
static void tracked_free(void *p) {if(p){assert(live);live--;free(p);}}
#define free tracked_free
static void *heap_caps_malloc(size_t n,unsigned caps) {assert(caps==(MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT));return tracked_alloc(n);}
static void *heap_caps_calloc(size_t n,size_t size,unsigned caps) {void *p=heap_caps_malloc(n*size,caps);if(p)memset(p,0,n*size);return p;}
static uint32_t lv_tick_get(void) {return clock_ms;}
static uint32_t xTaskGetTickCount(void) {return clock_ms;}
static void vTaskDelay(unsigned n) {clock_ms+=n;}
static bool voice_todo_is_open(void) {return false;}
static bool inspiration_ui_is_open(void) {return false;}
static bool is_wifi_connected(void) {return online;}
static QueueHandle_t xQueueCreateStatic(unsigned count,unsigned size,uint8_t *data,StaticQueue_t *q) {assert(count==1);q->data=data;q->size=size;return q;}
static int xQueueOverwrite(QueueHandle_t q,const void *item) {assert(q);memcpy(q->data,item,q->size);q->ready=true;return 1;}
static int xQueueReceive(QueueHandle_t q,void *item,unsigned timeout) {(void)timeout;if(!q->ready)return 0;memcpy(item,q->data,q->size);q->ready=false;return 1;}
static int xTaskCreatePinnedToCoreWithCaps(void(*fn)(void*),const char *name,unsigned stack,void *arg,unsigned priority,void *handle,int core,unsigned caps) {
 (void)priority;(void)handle;assert(!external&&stack==16384&&core==0&&caps==(MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT)&&!strcmp(name,"keep_ai"));
 if(fault==TASK_FAIL)return 0;assert(!scheduled);scheduled=fn;scheduled_arg=arg;requests++;return pdPASS;
}
static void vTaskDeleteWithCaps(void *p) {assert(external&&!p&&!live);deletes++;}
static int todo_storage_export_done_json(char *out,size_t size) {assert(!external&&size==65536);strcpy(out,"[]");return fault==ARCHIVE_FAIL?-1:0;}
static unsigned token;
bool inspiration_service_summary_request(uint32_t t) {assert(external);token=t;return fault!=IDEA_TIMEOUT;}
bool inspiration_service_summary_receive(inspiration_reply_t *r) {memset(r,0,sizeof(*r));r->token=token;r->status=fault==IDEA_FAIL?INSPIRATION_IO:INSPIRATION_OK;return true;}
static bool voice_ai_complete(const char *system,const char *data,char *out,size_t capacity) {
 assert(external&&system&&data&&capacity==2048);ai_calls++;if(fault==AI_FAIL)return false;
 strcpy(out,"{\"lines\":[\"待办排队中。\",\"灵感正在发芽。\",\"番茄钟准备开工。\"]}");return true;
}
''' + todos + '\nstatic todo_list_t ui_todo_list;static int all_focus_time;\n' + pure + worker + r'''
static void finish_job(void) {
 keep_ai_storage_poll();
 if(scheduled) {void(*fn)(void*)=scheduled;void *arg=scheduled_arg;scheduled=NULL;external=true;fn(arg);external=false;}
 (void)keep_ai_poll();assert(!s_busy&&!atomic_load(&s_pending_job)&&!live);
}
int main(void) {
 cJSON_Hooks hooks={tracked_alloc,tracked_free};cJSON_InitHooks(&hooks);
 for(unsigned round=0;round<200;round++)for(fault=OK_RUN;fault<=AI_FAIL;fault++) {
  clock_ms+=31000;attempt=0;unsigned before=requests;
  keep_ai_refresh(20261031,true);assert(s_busy&&atomic_load(&s_pending_job));
  job_t *pending=atomic_load(&s_pending_job);
  for(unsigned click=0;click<20;click++)keep_ai_refresh(20261031,true);
  assert(atomic_load(&s_pending_job)==pending&&requests==before);finish_job();
  assert(s_status==(fault==OK_RUN?AI_READY:AI_FAILED));
 }
 fault=OK_RUN;attempt=0;keep_ai_refresh(20261031,true);finish_job();unsigned allocations=attempt;
 for(unsigned round=0;round<10;round++)for(unsigned n=1;n<=allocations;n++) {
  attempt=0;fail_at=n;keep_ai_refresh(20261031,true);finish_job();
 }
 fail_at=0;keep_ai_refresh(20261031,true);finish_job();
 clock_ms+=31000;unsigned before=ai_calls;keep_ai_request(20261031,true);finish_job();assert(ai_calls==before&&s_status==AI_READY);
 online=false;keep_ai_refresh(20261031,true);assert(s_status==AI_OFFLINE&&!s_busy&&!live);
 printf("PASS 1200 Keep jobs + %u allocation-fault jobs, burst refresh coalescing, internal storage/PSRAM network, cache/offline, zero retained allocations\n",10*allocations);
}
'''
        with tempfile.TemporaryDirectory() as temp:
            p=Path(temp);(p/'test.c').write_text(code)
            subprocess.run(['cc','-std=c11','-O1','-g','-Wno-deprecated-declarations','-fsanitize=address,undefined','-I',str(root/'main'),'-I',str(cjson),str(p/'test.c'),str(cjson/'cJSON.c'),'-o',str(p/'test')],check=True)
            r=subprocess.run([str(p/'test')],capture_output=True,text=True,timeout=45,env=dict(os.environ,ASAN_OPTIONS='detect_leaks=0:halt_on_error=1'))
            self.assertEqual(r.returncode,0,r.stdout+r.stderr);print(r.stdout,end='')

if __name__=='__main__':unittest.main()
