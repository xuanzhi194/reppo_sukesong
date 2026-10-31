"""Drive the production ASR receive functions with fragmented/stale server events."""
from pathlib import Path
import os
import subprocess
import tempfile
import unittest

class BossProtocolTests(unittest.TestCase):
    def test_stream_frames_and_session_isolation(self):
        root = Path(__file__).resolve().parents[2]
        idf = Path(os.environ.get('IDF_PATH', '/Users/sean/.espressif/v5.3.5/esp-idf'))
        cjson = idf / 'components/json/cJSON'
        self.assertTrue((cjson / 'cJSON.c').exists())
        source = (root / 'main/boss_service.c').read_text()
        # Compile exact production receive/state code, replacing only RTOS/network types.
        prelude = source[source.index('#define CHUNK'):source.index('static void ai_worker')]
        receive = source[source.index('static const char *str('):source.index('/* Bulk data')]
        code = r'''
#include <assert.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "boss_service.h"
#include "boss_rules.h"
#include "boss_audio_config.h"
#include "cJSON.h"
typedef int portMUX_TYPE;
#define portMUX_INITIALIZER_UNLOCKED 0
#define portENTER_CRITICAL(x) ((void)(x))
#define portEXIT_CRITICAL(x) ((void)(x))
#define ESP_LOGW(...) ((void)0)
#define ESP_LOGI(...) ((void)0)
typedef void *QueueHandle_t;
typedef void *StreamBufferHandle_t;
typedef int StaticStreamBuffer_t;
typedef const char *esp_event_base_t;
#define ESP_ERR_ESP_TLS_CANNOT_RESOLVE_HOSTNAME 202
#define ESP_ERR_NO_MEM 257
enum {WEBSOCKET_EVENT_CONNECTED,WEBSOCKET_EVENT_DISCONNECTED,WEBSOCKET_EVENT_ERROR,WEBSOCKET_EVENT_DATA};
typedef struct {int op_code,payload_offset,data_len,payload_len;bool fin;const char *data_ptr;struct { int esp_tls_last_esp_err, esp_ws_handshake_status_code, esp_transport_sock_errno, esp_tls_cert_verify_flags; } error_handle;} esp_websocket_event_data_t;
static int64_t now;
static int64_t esp_timer_get_time(void) {return now;}
static unsigned assistant_partials,assistant_finals;
static char assistant_text[BOSS_SOURCE_CAP];
static void assistant_receive(const char *text,bool final) {assert(text[0]);boss_text_copy(assistant_text,sizeof(assistant_text),text);if(final)assistant_finals++;else assistant_partials++;}
static unsigned jobs;
static unsigned queued_generation,queued_utterance;
static void xQueueOverwrite(void *q,const void *job);
''' + prelude + r'''
static void xQueueOverwrite(void *q,const void *ptr) { (void)q; const ai_job_t *j=ptr;jobs++;queued_generation=j->generation;queued_utterance=j->utterance; }
''' + receive + r'''
static void frame(session_t *ctx,const char *s,int opcode,int offset,int total,bool fin) {
 esp_websocket_event_data_t e={.op_code=opcode,.payload_offset=offset,.data_len=(int)strlen(s),.payload_len=total,.fin=fin,.data_ptr=s};
 websocket_event(ctx,NULL,WEBSOCKET_EVENT_DATA,&e);
}
static void complete(session_t *ctx,const char *json) { frame(ctx,json,1,0,strlen(json),true); }
int main(void) {
 session_t ctx={.generation=1};ctx.rx=calloc(1,RX_CAP);s_generation=1;s_view.state=BOSS_LISTENING;atomic_store(&s_run,true);s_ai_queue=(void*)1;
 complete(&ctx,"{\"type\":\"session.updated\"}");assert(ctx.ready);
 const char *a="{\"type\":\"conversation.item.input_audio_transcription.text\",\"item_id\":\"one\",";
 const char *b="\"text\":\"我们\",\"stash\":\"复盘一下\"}";
 frame(&ctx,a,1,0,strlen(a)+strlen(b),true);assert(!s_view.source[0]);
 frame(&ctx,b,1,strlen(a),strlen(a)+strlen(b),true);assert(!strcmp(s_view.source,"我们复盘一下")&&s_view.partial);
 complete(&ctx,"{\"type\":\"conversation.item.input_audio_transcription.completed\",\"item_id\":\"one\",\"transcript\":\"我们复盘一下。\"}");
 assert(!s_view.partial&&!strcmp(s_view.reply,"请各位打开背锅二维码。")&&jobs==0);
 // New partials/finals during the result hold cannot replace the visible joke.
 unsigned hold_revision=s_view.revision;
 complete(&ctx,"{\"type\":\"conversation.item.input_audio_transcription.text\",\"item_id\":\"blocked\",\"text\":\"下一句\"}");
 assert(s_view.revision==hold_revision&&s_view.turn==BOSS_TURN_SHOW);
 now=SHOW_TIME_US-1;boss_snapshot_t snap;boss_service_snapshot(&snap);assert(snap.turn==BOSS_TURN_SHOW);
 now++;boss_service_snapshot(&snap);assert(snap.turn==BOSS_TURN_LISTEN&&strstr(snap.status,"下一句"));
 // A blocked sentence stays blocked even when its final arrives after reopening.
 complete(&ctx,"{\"type\":\"conversation.item.input_audio_transcription.completed\",\"item_id\":\"blocked\",\"transcript\":\"下一句话\"}");
 assert(jobs==0&&!strcmp(s_view.reply,"请各位打开背锅二维码。"));
 // A fragmented WebSocket message with an intervening ping remains intact.
 frame(&ctx,a,1,0,strlen(a),false);frame(&ctx,"ping",9,0,4,true);frame(&ctx,b,0,0,strlen(b),true);
 assert(!strcmp(s_view.source,"我们复盘一下"));
 complete(&ctx,"{\"type\":\"conversation.item.input_audio_transcription.completed\",\"item_id\":\"two\",\"transcript\":\"明天早上交\"}");assert(jobs==1);
 ai_job_t job={.generation=queued_generation,.utterance=queued_utterance};
 unsigned old_utterance=queued_utterance;
 complete(&ctx,"{\"type\":\"conversation.item.input_audio_transcription.text\",\"item_id\":\"three\",\"text\":\"你好\",\"stash\":\"老板\"}");
 assert(s_utterance==old_utterance&&s_view.turn==BOSS_TURN_THINK);
 assert(!strcmp(s_view.source,"明天早上交")); // Slow translation is protected.
 now+=2000000;translation_done(&job,true,"工作量已经充值到账。","余额不足");
 assert(s_view.turn==BOSS_TURN_SHOW&&s_view.translated);
 now+=SHOW_TIME_US-1;boss_service_snapshot(&snap);assert(snap.turn==BOSS_TURN_SHOW);
 now++;boss_service_snapshot(&snap);assert(snap.turn==BOSS_TURN_LISTEN);
 complete(&ctx,"{\"type\":\"conversation.item.input_audio_transcription.completed\",\"item_id\":\"three\",\"transcript\":\"你好老板\"}");assert(jobs==1);
 complete(&ctx,"{\"type\":\"conversation.item.input_audio_transcription.completed\",\"item_id\":\"four\",\"transcript\":\"另一个陌生句子\"}");assert(jobs==2);
 job.utterance=queued_utterance;
 now+=THINK_TIMEOUT_US;boss_service_snapshot(&snap);assert(snap.turn==BOSS_TURN_SHOW&&!snap.translated);
 unsigned timeout_revision=s_view.revision;
 translation_done(&job,true,"迟到的答案","不能覆盖");assert(s_view.revision==timeout_revision);
 now+=SHOW_TIME_US;boss_service_snapshot(&snap);assert(snap.turn==BOSS_TURN_LISTEN);
 complete(&ctx,"{\"type\":\"conversation.item.input_audio_transcription.completed\",\"item_id\":\"five\",\"transcript\":\"新的句子\"}");assert(jobs==3);
 job.utterance=queued_utterance;translation_done(&job,false,"","");assert(s_view.turn==BOSS_TURN_SHOW&&!s_view.translated);
 now+=SHOW_TIME_US;boss_service_snapshot(&snap);
 complete(&ctx,"{\"type\":\"conversation.item.input_audio_transcription.completed\",\"item_id\":\"six\",\"transcript\":\"其他句子\"}");
 job.utterance=queued_utterance;
 translation_done(&job,true,"这是一句特别长的翻译需要翻页才能看到完整内容所以必须多给观众一点阅读时间。","阅读中");
 now+=SHOW_TIME_US;boss_service_snapshot(&snap);assert(snap.turn==BOSS_TURN_SHOW);
 now+=LONG_SHOW_TIME_US-SHOW_TIME_US;boss_service_snapshot(&snap);assert(snap.turn==BOSS_TURN_LISTEN);
 boss_service_stop();unsigned rev=s_view.revision;
 translation_done(&job,true,"旧回复","");assert(s_view.revision==rev);
 complete(&ctx,"{\"type\":\"conversation.item.input_audio_transcription.completed\",\"transcript\":\"大局观\"}");assert(s_view.revision==rev);
 atomic_store(&s_run,true);s_generation++;complete(&ctx,"{\"type\":\"conversation.item.input_audio_transcription.completed\",\"transcript\":\"抓手\"}");assert(s_view.revision==rev);
 ctx.generation=s_generation;ctx.used=RX_CAP-1;ctx.assembling=true;
 frame(&ctx,"xx",0,0,2,true);assert(ctx.failed&&s_view.state==BOSS_ERROR);
 unsigned failed_rev=s_view.revision;fail(&ctx,"later disconnect");assert(s_view.revision==failed_rev); // Keep useful first error.
 ctx.failed=false;ctx.generation=s_generation;
 esp_websocket_event_data_t error={0};error.error_handle.esp_tls_last_esp_err=ESP_ERR_ESP_TLS_CANNOT_RESOLVE_HOSTNAME;
 websocket_event(&ctx,NULL,WEBSOCKET_EVENT_ERROR,&error);assert(strstr(s_view.status,"DNS失败"));
 websocket_event(&ctx,NULL,WEBSOCKET_EVENT_DISCONNECTED,NULL);assert(strstr(s_view.status,"DNS失败"));
 status(BOSS_LISTENING,"网络波动");assert(s_view.state==BOSS_ERROR&&strstr(s_view.status,"DNS失败"));
 ctx.failed=false;error.error_handle.esp_tls_last_esp_err=0;error.error_handle.esp_ws_handshake_status_code=403;
 websocket_event(&ctx,NULL,WEBSOCKET_EVENT_ERROR,&error);assert(strstr(s_view.status,"鉴权失败"));
 // Shared assistant mode delivers every event, bypassing Boss jokes/hold/AI.
 ctx.failed=false;ctx.callback=assistant_receive;s_view.turn=BOSS_TURN_SHOW;
 unsigned old_jobs=jobs;unsigned old_revision=s_view.revision;
 complete(&ctx,"{\"type\":\"conversation.item.input_audio_transcription.text\",\"text\":\"Hey Reppo\"}");
 complete(&ctx,"{\"type\":\"conversation.item.input_audio_transcription.completed\",\"transcript\":\"打开领导黑话翻译\"}");
 assert(assistant_partials==1&&assistant_finals==1&&jobs==old_jobs&&s_view.revision==old_revision);

 // Join large UTF-8 partial/stash fields without a 1.5KiB temporary stack array.
 char long_text[2048];long_text[0]=0;
 for(unsigned i=0;i<500;i++)strcat(long_text,"啊");
 cJSON *large=cJSON_CreateObject();cJSON_AddStringToObject(large,"text",long_text);
 cJSON_AddStringToObject(large,"stash","尾巴");transcript(&ctx,large,false);
 assert(strlen(assistant_text)==765&&strlen(assistant_text)%3==0);
 cJSON_ReplaceItemInObject(large,"text",cJSON_CreateString("Hey Reppo，"));
 cJSON_ReplaceItemInObject(large,"stash",cJSON_CreateString("打开领导黑话翻译"));
 transcript(&ctx,large,false);assert(!strcmp(assistant_text,"Hey Reppo，打开领导黑话翻译"));cJSON_Delete(large);
 boss_service_stop();complete(&ctx,"{\"type\":\"conversation.item.input_audio_transcription.completed\",\"transcript\":\"迟到命令\"}");assert(assistant_finals==1);
 free(ctx.rx);puts("PASS chunked/fragmented events, text+stash, local match, AI handoff, protected turns, read holds, AI timeout/failure, stale session/utterance and bounded overflow");
}
'''
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp)
            (path / 'test.c').write_text(code)
            subprocess.run(['cc', '-std=c11', '-g', '-Wno-deprecated-declarations', '-fsanitize=address,undefined',
                            '-I', str(root / 'main'), '-I', str(cjson), str(path / 'test.c'),
                            str(root / 'main/boss_rules.c'), str(cjson / 'cJSON.c'), '-o', str(path / 'test')], check=True)
            subprocess.run([str(path / 'test')], check=True,
                           env=dict(os.environ, ASAN_OPTIONS='detect_leaks=0:halt_on_error=1'))

if __name__ == '__main__':
    unittest.main()
