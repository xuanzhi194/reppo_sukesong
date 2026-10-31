#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hey_reppo.c"
lv_ui guider_ui;
bool countdown_start_label,stopwatch_start_label,countdown_timeout_pending,focus_en,focus_session_active;
int current_task_index;
int16_t timer_all_num;
static bool online=true,audio_busy,boss_open,voice_open,voice_pending,game_open;
static bool save_fail,heap_fail;
static unsigned inbox_allocs;
static esp_reset_reason_t reset_reason=ESP_RST_POWERON;
esp_reset_reason_t esp_reset_reason(void) {return reset_reason;}
void *heap_caps_calloc(size_t count,size_t size,unsigned caps) {
 assert(caps==(MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT));
 if(heap_fail) return NULL;
 inbox_allocs++;return calloc(count,size);
}
static uint8_t persisted;
static unsigned starts,stops,saves,focus_starts,wakes;
static uint32_t release_at;
static boss_asr_callback_t callback;
static boss_snapshot_t audio;
static lv_color_t framebuffer[320*170];
extern const lv_font_t ui_todo_cjk_font;
const lv_font_t *ui_font_16(void) {static lv_font_t f;f=lv_font_SourceHanSansSC_Medium_16;f.fallback=&ui_todo_cjk_font;return &f;}
const lv_font_t *ui_font_14(void) {static lv_font_t f;f=lv_font_SourceHanSansSC_Medium_14;f.fallback=&ui_todo_cjk_font;return &f;}
const lv_font_t *ui_font_12(void) {static lv_font_t f;f=lv_font_SourceHanSansSC_Medium_12;f.fallback=&ui_todo_cjk_font;return &f;}
int64_t esp_timer_get_time(void) {return (int64_t)lv_tick_get()*1000;}
bool is_wifi_connected(void) {return online;}
int nvs_open(const char *name,int mode,nvs_handle_t *h) {(void)name;(void)mode;*h=1;return 0;}
int nvs_get_u8(nvs_handle_t h,const char *key,uint8_t *v) {(void)h;(void)key;*v=persisted;return 0;}
int nvs_set_u8(nvs_handle_t h,const char *key,uint8_t v) {(void)h;(void)key;if(save_fail)return 1;persisted=v;return 0;}
int nvs_commit(nvs_handle_t h) {(void)h;saves++;return 0;}
void nvs_close(nvs_handle_t h) {(void)h;}
bool boss_service_start_asr(boss_asr_callback_t cb) {assert(!audio_busy&&!boss_open&&!voice_open&&!voice_pending);callback=cb;starts++;audio_busy=true;audio.state=BOSS_LISTENING;return true;}
bool boss_service_busy(void) {return audio_busy;}
void boss_service_stop(void) {stops++;audio.state=BOSS_OFF;if(!release_at) release_at=lv_tick_get()+200;}
void boss_service_snapshot(boss_snapshot_t *out) {*out=audio;}
bool boss_ui_is_open(void) {return boss_open;}
void boss_ui_request_enter(void) {assert(!audio_busy);boss_open=true;}
bool voice_todo_is_open(void) {return voice_open;}
bool voice_todo_audio_busy(void) {return voice_open;}
bool voice_todo_entry_pending(void) {return voice_pending;}
bool inspiration_ui_is_open(void) {return voice_open;}
void inspiration_ui_open_hub(void) {voice_open=true;}
bool game_machine_is_open(void) {return game_open;}
bool focus_timer_is_running(void) {return countdown_start_label||stopwatch_start_label;}
void focus_session_cancel(void) {focus_session_active=false;}
void idle_sleep_note_activity(void) {}
void idle_sleep_request_wake(void) {wakes++;}
void inspiration_ui_mark_guider_deleted(void *screen) {
#define MARK(n) if(screen==guider_ui.n) {guider_ui.n##_del=true;return;}
 MARK(screen_main) MARK(screen_setting) MARK(screen_todolist) MARK(screen_donelist) MARK(screen_pomodoro)
 MARK(screen_keep) MARK(screen_gash) MARK(screen_book) MARK(screen_game)
 MARK(screen_picture) MARK(screen_answer) MARK(screen_timing)
#undef MARK
}
static lv_obj_t *fixture_settings;
static void fixture_settings_deleted(lv_event_t *e) {
 (void)e;fixture_settings=NULL;
}
lv_obj_t *screen_setting_list_add_btn(const char *text) {
 // Model custom.c's live settings-list ownership check, not the Guider flag.
 if(!fixture_settings||fixture_settings!=guider_ui.screen_setting||!lv_obj_is_valid(fixture_settings)) return NULL;
 lv_obj_t *b=surface(guider_ui.screen_setting,19,110,95,18,0xFFFFFF,5);
 lv_obj_t *l=make_label(b,0,0,95,ui_font_14(),HEY_INK);lv_label_set_text(l,text);return b;
}
#define SETUP(n) void setup_scr_##n(lv_ui *ui) {ui->n=lv_obj_create(NULL);ui->n##_del=false;}
SETUP(screen_main) SETUP(screen_todolist) SETUP(screen_donelist)
SETUP(screen_keep) SETUP(screen_gash) SETUP(screen_book) SETUP(screen_game)
SETUP(screen_picture) SETUP(screen_answer) SETUP(screen_timing)
void setup_scr_screen_setting(lv_ui *ui) {
 // Real generated setup leaves screen_setting_del unchanged.
 ui->screen_setting=lv_obj_create(NULL);fixture_settings=ui->screen_setting;
 lv_obj_add_event_cb(fixture_settings,fixture_settings_deleted,LV_EVENT_DELETE,NULL);
 lv_obj_remove_style_all(ui->screen_setting);lv_obj_set_size(ui->screen_setting,320,170);
 lv_obj_set_style_bg_img_src(ui->screen_setting,&_AAA_reppo_setting_bg_320x170,0);
 lv_obj_set_style_bg_img_recolor(ui->screen_setting,lv_color_hex(0x7987a6),0);
 lv_obj_set_style_bg_img_recolor_opa(ui->screen_setting,21,0);
 const char *rows[]={"Sleep","Language","Boss Trans"};
 for(unsigned i=0;i<3;i++){lv_obj_t *l=make_label(ui->screen_setting,19,35+i*25,95,ui_font_14(),HEY_INK);lv_label_set_text(l,rows[i]);}
}
static void focus_start(lv_event_t *e) {(void)e;assert(!audio_busy);focus_starts++;countdown_start_label=true;}
void setup_scr_screen_pomodoro(lv_ui *ui) {
 ui->screen_pomodoro=lv_obj_create(NULL);ui->screen_pomodoro_del=false;
 ui->screen_pomodoro_label_timer=lv_label_create(ui->screen_pomodoro);
 ui->screen_pomodoro_btn_start=lv_btn_create(ui->screen_pomodoro);
 lv_obj_add_event_cb(ui->screen_pomodoro_btn_start,focus_start,LV_EVENT_CLICKED,NULL);
}
#include "guider_loader_under_test.h"
static void flush(lv_disp_drv_t *d,const lv_area_t *a,lv_color_t *p) {
 for(int y=a->y1;y<=a->y2;y++)for(int x=a->x1;x<=a->x2;x++){if(x>=0&&x<320&&y>=0&&y<170)framebuffer[y*320+x]=*p;p++;}lv_disp_flush_ready(d);
}
static void advance(unsigned ms) {
 for(unsigned i=0;i<ms;i+=10) {
  lv_tick_inc(10);if(release_at&&(int32_t)(lv_tick_get()-release_at)>=0){audio_busy=false;release_at=0;}
  lv_timer_handler();hey_reppo_poll();
 }
}
static void screenshot_named(const char *suffix) {
 const char *path=getenv("REPPO_HEY_SNAPSHOT");if(!path)return;
 char full[1024];snprintf(full,sizeof(full),"%s%s",path,suffix);
 FILE *f=fopen(full,"wb");assert(f);fprintf(f,"P6\n320 170\n255\n");
 for(unsigned i=0;i<320*170;i++){lv_color32_t c={.full=lv_color_to32(framebuffer[i])};unsigned char rgb[]={c.ch.red,c.ch.green,c.ch.blue};fwrite(rgb,1,3,f);}fclose(f);
}
static void home(void) {load_screen(&guider_ui.screen_main,&guider_ui.screen_main_del,setup_scr_screen_main);advance(2500);}
static void *burst_producer(void *unused) {
 (void)unused;
 for(unsigned i=0;i<10000;i++) receive("普通背景聊天",i%3==0);
 return NULL;
}
int main(void) {
 lv_init();static lv_color_t pixels[320*20];static lv_disp_draw_buf_t buf;static lv_disp_drv_t disp;
 lv_disp_draw_buf_init(&buf,pixels,NULL,320*20);lv_disp_drv_init(&disp);disp.hor_res=320;disp.ver_res=170;disp.draw_buf=&buf;disp.flush_cb=flush;lv_disp_drv_register(&disp);
 persisted=1;reset_reason=ESP_RST_PANIC;hey_reppo_init();assert(s_crash_recovery&&!s_enabled);
 hey_reppo_storage_poll();assert(persisted==1&&saves==0);
 reset_reason=ESP_RST_POWERON;hey_reppo_init();assert(s_enabled&&!s_crash_recovery);
 persisted=0;heap_fail=true;assert(!inbox_prepare());heap_fail=false;
 hey_reppo_init();setup_scr_screen_main(&guider_ui);lv_scr_load(guider_ui.screen_main);advance(100);assert(!starts&&!atomic_load(&s_enabled));
 // Knob navigation uses the real generated loader. Its recreate flag stays
 // true even while settings is visible, including after exit/re-entry.
 for(unsigned repeat=0;repeat<20;repeat++) {
  guider_ui.screen_setting_del=true;
  ui_load_scr_animation(&guider_ui,&guider_ui.screen_setting,true,&guider_ui.screen_main_del,
                       setup_scr_screen_setting,LV_SCR_LOAD_ANIM_FADE_ON,100,100,true,true);
  advance(400);assert(guider_ui.screen_setting_del&&s_button);
  assert(!strcmp(lv_label_get_text(lv_obj_get_child(s_button,0)),"Hey Reppo"));
  lv_obj_t *button=s_button;unsigned children=lv_obj_get_child_cnt(guider_ui.screen_setting);
  advance(500);assert(s_button==button&&lv_obj_get_child_cnt(guider_ui.screen_setting)==children);
  if(repeat<19) {home();assert(!s_button&&!s_detail&&!fixture_settings);}
 }
 puts("PASS 20 generated-loader settings entries with recreate flag set; no missing/duplicate button");
 assert(!strcmp(lv_label_get_text(lv_obj_get_child(s_button,0)),"Hey Reppo"));
 lv_event_send(s_button,LV_EVENT_FOCUSED,NULL);advance(250);
 assert(s_detail_focused&&s_detail&&!lv_obj_has_flag(s_detail,LV_OBJ_FLAG_HIDDEN));
 assert(!s_panel&&!starts&&strstr(lv_label_get_text(s_detail_status),"已关闭"));
 screenshot_named(".settings-off.ppm");
 lv_event_send(s_button,LV_EVENT_CLICKED,NULL);advance(250);assert(starts==1&&audio_busy);
 assert(!strcmp(lv_label_get_text(lv_obj_get_child(s_button,0)),"Hey Reppo"));
 assert(lv_obj_get_x(s_toggle_knob)==21&&lv_obj_has_flag(s_panel,LV_OBJ_FLAG_HIDDEN));
 screenshot_named(".settings-on.ppm");
 boss_text_copy(audio.status,sizeof(audio.status),"网络缓冲中");advance(150);
 assert(strstr(lv_label_get_text(s_detail_status),"语音延迟"));
 lv_event_send(s_button,LV_EVENT_DEFOCUSED,NULL);advance(150);
 assert(!lv_obj_has_flag(s_indicator,LV_OBJ_FLAG_HIDDEN)&&strstr(lv_label_get_text(s_indicator),"语音延迟"));
 boss_text_copy(audio.status,sizeof(audio.status),"请说话");advance(150);
 assert(strstr(lv_label_get_text(s_indicator),"待唤醒"));

 lv_event_send(s_button,LV_EVENT_DEFOCUSED,NULL);advance(150);assert(lv_obj_has_flag(s_detail,LV_OBJ_FLAG_HIDDEN));
 lv_event_send(s_button,LV_EVENT_FOCUSED,NULL);advance(250);assert(!lv_obj_has_flag(s_detail,LV_OBJ_FLAG_HIDDEN));
 hey_reppo_storage_poll();assert(persisted==1&&saves==1);hey_reppo_storage_poll();assert(saves==1);
 callback("打开游戏",true);advance(100);assert(!s_dialog.awake);
 callback("Hey Reppo",false);assert(!s_dialog.awake&&s_inbox_count==1);advance(100);assert(wakes&&s_dialog.awake&&!lv_obj_has_flag(s_panel,LV_OBJ_FLAG_HIDDEN));
 callback("Hey Reppo",true);callback("帮我开始二十五分钟专注",false);advance(250);screenshot_named("");assert(!focus_starts);
 callback("帮我开始二十五分钟专注",true);advance(100);assert(stops&&audio_busy&&!focus_starts);
 callback("打开游戏",true);assert(s_command.action==HEY_START_FOCUS); // Late event while releasing cannot replace it.
 advance(600);assert(lv_scr_act()==guider_ui.screen_pomodoro&&focus_starts==1&&timer_all_num==25&&!focus_session_active);
 assert(!s_button&&!s_detail); // Setting children/animations cannot survive their page.
 unsigned before_starts=starts;advance(3000);assert(starts==before_starts&&!audio_busy);
 countdown_start_label=false;home();assert(audio_busy);
 // Overflow never dispatches a truncated/stale command. Polling stays bounded.
 for(unsigned i=0;i<=HEY_INBOX_CAP;i++) callback("Hey Reppo，打开游戏",true);
 assert(s_inbox_overflow&&!s_dialog.pending);advance(100);
 assert(!s_dialog.awake&&s_command.action==HEY_NONE&&lv_scr_act()==guider_ui.screen_main);
 callback("Hey Reppo，打开游戏",true);lv_tick_inc(2100);advance(100);
 assert(!s_dialog.awake&&s_command.action==HEY_NONE&&lv_scr_act()==guider_ui.screen_main);
 // A producer event can be newer than the UI poll's captured clock.
 callback("Hey Reppo",false);receive_poll(now_ms()-1);assert(s_dialog.awake);
 reset_dialog();
 callback("Hey Reppo，打开游戏",true);hey_reppo_yield();advance(300);
 assert(s_command.action==HEY_NONE&&lv_scr_act()==guider_ui.screen_main);home();assert(audio_busy);
 unsigned allocs_before=inbox_allocs;
 // Real producer/consumer concurrency, no clock mutation while producer runs.
 pthread_t producer;assert(!pthread_create(&producer,NULL,burst_producer,NULL));
 for(unsigned i=0;i<10000;i++) receive_poll(now_ms());
 assert(!pthread_join(producer,NULL));receive_poll(now_ms());
 assert(!s_dialog.awake&&!s_dialog.pending&&inbox_allocs==allocs_before);
 // Retain the wake partial even when ASR's final contains only the command.
 callback("嘿，Ripple，进入",false);callback("进入领导黑话翻译功能",true);
 assert(!s_dialog.pending);advance(600);assert(boss_open&&!audio_busy);
 boss_open=false;home();assert(audio_busy);

 callback("Hey Reppo，进入领导黑话翻译",true);advance(600);assert(boss_open&&!audio_busy);
 before_starts=starts;advance(3000);assert(starts==before_starts);
 boss_open=false;home();assert(audio_busy);
 voice_pending=true;advance(100);assert(!atomic_load(&s_accepting));advance(300);assert(!audio_busy);
 voice_pending=false;voice_open=true;advance(1000);assert(!audio_busy);voice_open=false;advance(100);assert(audio_busy);
 online=false;advance(400);assert(!audio_busy);before_starts=starts;advance(1000);assert(starts==before_starts);
 online=true;advance(100);assert(audio_busy);
 callback("Hey Reppo",true);advance(100);assert(s_dialog.awake);
 load_screen(&guider_ui.screen_todolist,&guider_ui.screen_todolist_del,setup_scr_screen_todolist);advance(300);assert(!s_dialog.awake); // Knob navigation cancels.
 callback("Hey Reppo，关闭语音唤醒",true);advance(600);assert(!atomic_load(&s_enabled)&&!audio_busy);
 hey_reppo_storage_poll();assert(persisted==0);before_starts=starts;advance(15000);assert(starts==before_starts);
 set_enabled(true);save_fail=true;hey_reppo_storage_poll();assert(atomic_load(&s_save_failed));save_fail=false;advance(5100);hey_reppo_storage_poll();assert(!atomic_load(&s_save_failed)&&persisted==1);
 // Worker failure completed before the next UI poll still gets backoff.
 advance(100);assert(audio_busy);audio_busy=false;audio.state=BOSS_ERROR;
 before_starts=starts;advance(100);assert(starts==before_starts&&!atomic_load(&s_accepting));
 advance(4500);assert(starts==before_starts);advance(600);assert(starts==before_starts+1);

 // Repeated navigation exercises the real coordinator/LVGL lifecycle. Page
 // bodies here are fixtures; dedicated Keep/game/Boss tests cover their widgets.
 home();
 struct {const char *command;lv_obj_t **screen;} routes[]={
  {"打开待办",&guider_ui.screen_todolist},{"打开已完成",&guider_ui.screen_donelist},
  {"进入专注页面",&guider_ui.screen_pomodoro},{"打开Keep",&guider_ui.screen_keep},
  {"打开扭蛋",&guider_ui.screen_gash},{"打开阅读",&guider_ui.screen_book},
  {"打开游戏",&guider_ui.screen_game},{"打开设置",&guider_ui.screen_setting},
  {"打开相册",&guider_ui.screen_picture},{"打开答案之书",&guider_ui.screen_answer},
  {"查看时间",&guider_ui.screen_timing},{"返回主页",&guider_ui.screen_main}
 };
 size_t stable_bytes=0,stable_blocks=0;
 for(unsigned round=0;round<50;round++) {
  for(unsigned route=0;route<sizeof(routes)/sizeof(routes[0]);route++) {
   home();assert(audio_busy&&atomic_load(&s_accepting));
   callback(round%3==0?"Hi.":round%3==1?"嗨！":"Hey, Repo.",false);callback(routes[route].command,true);
   advance(700);assert(lv_scr_act()==*routes[route].screen&&lv_obj_is_valid(*routes[route].screen));
  }
  home();callback("Hey Reppo，进入领导黑话翻译",true);advance(700);assert(boss_open&&!audio_busy);
  boss_open=false;home();callback("Hey Reppo，打开灵感闪记",true);advance(700);assert(voice_open&&!audio_busy);
  voice_open=false;home();
  if(round%5==0) {online=false;advance(500);assert(!audio_busy);online=true;advance(100);assert(audio_busy);}
  advance(3000);
  if(round==1){stable_bytes=test_lv_bytes;stable_blocks=test_lv_blocks;}
  if(round>1){assert(test_lv_bytes==stable_bytes&&test_lv_blocks==stable_blocks);}
 }
 printf("PASS LVGL live allocation plateau: %zu bytes / %zu blocks, peak %zu bytes\n",stable_bytes,stable_blocks,test_lv_peak);
 // Recycled LVGL addresses must not be mistaken for a still-live target page.
 home();guider_ui.screen_setting=lv_scr_act();guider_ui.screen_setting_del=true;
 advance(100);assert(!s_button); // Stale address cannot attach to the main page.
 callback("Hey Reppo，打开设置",true);advance(700);
 assert(lv_scr_act()==guider_ui.screen_setting&&guider_ui.screen_setting_del&&s_button);
 puts("PASS 700 voice destinations/returns, disconnect recovery and recycled page identity");
 puts("PASS inbox overflow/staleness/concurrent producer, PSRAM workspace, crash recovery, callback deferral, default off, persisted switch, wake overlay, final-only focus, mic release before dispatch, Boss handoff, manual recorder, offline/recovery, navigation cancel and save retry");
}
