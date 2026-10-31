"""Repeated manual ASR/AI worker startup/failure/exit, using production lifecycle code."""
from pathlib import Path
import os
import subprocess
import tempfile
import unittest

class VoiceWorkerTests(unittest.TestCase):
    def test_external_stack_lifecycle(self):
        root=Path(__file__).resolve().parents[2]
        source=(root/'main/voice_todo.c').read_text()
        worker=source[source.index('static void voice_job(void *param)'):source.index('void voice_todo_poll(void)')]
        code=r'''
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inspiration_storage.h"
#define TODO_NAME_LEN 128
#define VOICE_RATE 16000
#define VOICE_MAX_MS 1000
#define VOICE_TODO_MS 1000
#define MALLOC_CAP_SPIRAM 1
#define MALLOC_CAP_8BIT 2
#define pdPASS 1
#define ESP_LOGI(...) ((void)0)
#define ESP_LOGE(...) ((void)0)
enum {VS_LISTEN,VS_FAIL};
enum {OK_RUN,TASK_FAIL,ALLOC_FAIL,RECORD_FAIL,ASR_FAIL,REFINE_FAIL};
static int fault,state;
static unsigned live,created,deleted;
static bool external,s_job_running,s_job_done,s_job_ok,s_job_recorded,s_job_inspiration,s_inspiration;
static uint32_t s_audio_ms,s_audio_level,s_feedback_at,s_feedback_seconds;
static uint16_t s_job_minutes,s_minutes;
static char s_job_text[TODO_NAME_LEN],s_shown[256];
static inspiration_record_t s_job_idea;
static void *s_record_bar;
static void (*scheduled)(void*);
static void *heap_caps_malloc(size_t n,unsigned caps) {assert(external&&caps==(MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT));if(fault==ALLOC_FAIL)return NULL;void *p=malloc(n);if(p)live++;return p;}
static void tracked_free(void *p) {if(p){assert(live);live--;free(p);}}
#define free tracked_free
static void vTaskDeleteWithCaps(void *p) {assert(!p&&external&&!live&&!s_job_running&&s_job_done);deleted++;}
static size_t voice_record(int16_t *samples,size_t max) {assert(external&&samples&&max);return fault==RECORD_FAIL?0:100;}
static void voice_wav_header(uint8_t *wav,uint32_t n) {assert(wav&&n==200);}
static bool voice_recognize_qwen(const uint8_t *data,size_t n,char *out,size_t cap) {(void)data;(void)n;(void)out;(void)cap;return true;}
static bool voice_asr_recognize(const uint8_t *wav,size_t n,char *out,size_t cap,bool(*fn)(const uint8_t*,size_t,char*,size_t)) {
 assert(external&&wav&&n==244&&fn==voice_recognize_qwen);if(fault==ASR_FAIL)return false;snprintf(out,cap,"准备答辩25分钟");return true;
}
static void voice_trim_copy(char *out,size_t cap,const char *in,unsigned chars) {(void)chars;snprintf(out,cap,"%s",in);}
void inspiration_text_copy(char *out,size_t cap,const char *in) {snprintf(out,cap,"%s",in);}
static bool voice_refine_idea(const char *text,inspiration_record_t *idea) {if(fault==REFINE_FAIL)return false;snprintf(idea->text,sizeof(idea->text),"%s",text);snprintf(idea->title,sizeof(idea->title),"idea");return true;}
static bool voice_refine(const char *text,char *out,size_t cap,uint16_t *minutes) {if(fault==REFINE_FAIL)return false;snprintf(out,cap,"%s",text);*minutes=25;return true;}
static unsigned voice_parse_minutes(const char *text) {(void)text;return 25;}
static bool voice_duration_cue(const char *text) {(void)text;return true;}
static bool is_wifi_connected(void) {return true;}
static void voice_show_idle(void) {}
static uint32_t lv_tick_get(void) {return 1000;}
static void lv_bar_set_range(void *bar,unsigned min,unsigned max) {(void)bar;assert(min==0&&max==1000);}
static void voice_apply(int s) {state=s;}
static const char *ui_tr(const char *en,const char *zh) {(void)en;return zh;}
static int xTaskCreatePinnedToCoreWithCaps(void(*fn)(void*),const char *name,unsigned bytes,void *arg,unsigned priority,void *handle,int core,unsigned caps) {
 assert(!external&&!arg&&!handle&&bytes==20480&&priority==5&&core==0&&caps==(MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT)&&!strcmp(name,"voice_job"));
 if(fault==TASK_FAIL)return 0;assert(!scheduled);scheduled=fn;created++;return pdPASS;
}
''' + worker + r'''
int main(void) {
 for(unsigned round=0;round<500;round++)for(fault=OK_RUN;fault<=REFINE_FAIL;fault++) {
  s_inspiration=round%2;unsigned before=created;voice_start_listen();
  if(fault==TASK_FAIL) {assert(!s_job_running&&state==VS_FAIL&&!live);continue;}
  assert(s_job_running&&scheduled&&created==before+1);voice_start_listen();assert(created==before+1);
  void(*fn)(void*)=scheduled;scheduled=NULL;external=true;fn(NULL);external=false;
  assert(s_job_done&&!s_job_running&&!live);
  assert(s_job_ok==(fault==OK_RUN||fault==REFINE_FAIL));
  if(s_job_ok&&s_inspiration)assert(s_job_idea.text[0]&&s_job_idea.original[0]);
  if(s_job_ok&&!s_inspiration)assert(s_job_text[0]&&s_job_minutes==25);
 }
 assert(created==deleted);printf("PASS 3000 manual voice lifecycle attempts, %u PSRAM task exits, ASR/AI fallback and zero retained PCM allocations\n",deleted);
}
'''
        with tempfile.TemporaryDirectory() as temp:
            p=Path(temp);(p/'test.c').write_text(code)
            subprocess.run(['cc','-std=c11','-O1','-g','-fsanitize=address,undefined','-I',str(root/'main'),str(p/'test.c'),'-o',str(p/'test')],check=True)
            r=subprocess.run([str(p/'test')],capture_output=True,text=True,timeout=45,env=dict(os.environ,ASAN_OPTIONS='detect_leaks=0:halt_on_error=1'))
            self.assertEqual(r.returncode,0,r.stdout+r.stderr);print(r.stdout,end='')

if __name__=='__main__':unittest.main()
