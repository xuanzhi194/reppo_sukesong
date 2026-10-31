#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "boss_ui.c"
#include "boss_rules.h"
lv_ui guider_ui;
lv_group_t *g;
static boss_snapshot_t fake;
static unsigned starts, stops;
static bool busy, online = true;
static lv_color_t framebuffer[320*170];
extern const lv_font_t ui_todo_cjk_font;
bool ui_lang_is_zh(void) { return true; }
const char *ui_tr(const char *en, const char *zh) { (void)en; return zh; }
const lv_font_t *ui_font_16(void) { static lv_font_t f; f=lv_font_SourceHanSansSC_Medium_16; f.fallback=&ui_todo_cjk_font; return &f; }
const lv_font_t *ui_font_12(void) { static lv_font_t f; f=lv_font_SourceHanSansSC_Medium_12; f.fallback=&ui_todo_cjk_font; return &f; }
const lv_font_t *ui_font_14(void) { static lv_font_t f; f=lv_font_SourceHanSansSC_Medium_14; f.fallback=&ui_todo_cjk_font; return &f; }
void idle_sleep_note_activity(void) {}
bool focus_timer_is_running(void) { return false; }
bool voice_todo_audio_busy(void) { return false; }
bool voice_todo_is_open(void) { return false; }
void inspiration_ui_mark_guider_deleted(void *s) { if (s==guider_ui.screen_setting) guider_ui.screen_setting_del=true; }
void boss_service_snapshot(boss_snapshot_t *out) { *out=fake; }
bool boss_service_busy(void) { return busy; }
bool boss_service_start(void) { starts++; busy=online; fake.state=online?BOSS_LISTENING:BOSS_ERROR; strcpy(fake.status,online?"正在聆听 · 实时转写":"请先连接 WiFi · 单击重试"); fake.revision++; return online; }
void boss_service_stop(void) { stops++; busy=false; fake.state=BOSS_OFF; strcpy(fake.status,"已暂停 · 麦克风关闭"); fake.revision++; }
lv_obj_t *screen_setting_list_add_btn(const char *text) { lv_obj_t *o=lv_btn_create(guider_ui.screen_setting); lv_obj_t *l=lv_label_create(o); lv_label_set_text(l,text); return o; }
void setup_scr_screen_main(lv_ui *ui) { ui->screen_main=lv_obj_create(NULL); }
void ui_load_scr_animation(lv_ui *ui, lv_obj_t **target, bool del, bool *old_del, ui_setup_scr_t setup, lv_scr_load_anim_t anim, uint32_t time, uint32_t delay, bool clean, bool auto_del) {
    (void)del;(void)old_del;(void)anim;(void)time;(void)delay;(void)clean;(void)auto_del;
    lv_obj_t *old=lv_scr_act(); if (!*target || !lv_obj_is_valid(*target)) setup(ui); lv_scr_load(*target); lv_obj_del(old);
}
static void flush(lv_disp_drv_t *d, const lv_area_t *a, lv_color_t *p) {
    for(int y=a->y1;y<=a->y2;y++) for(int x=a->x1;x<=a->x2;x++) { if(x>=0&&x<320&&y>=0&&y<170) framebuffer[y*320+x]=*p; p++; } lv_disp_flush_ready(d);
}
static void advance(unsigned ms) { for(unsigned i=0;i<ms;i+=10) { lv_tick_inc(10); lv_timer_handler(); boss_ui_poll(); } }
static void enter(void) { s_requested=true; advance(300); assert(boss_ui_is_open()); }
static void click(void) { lv_event_send(s_screen,LV_EVENT_SHORT_CLICKED,NULL); }
static void snapshot_named(const char *name) {
    const char *path=getenv("REPPO_BOSS_SNAPSHOT"); if(!path) return;
    char output[1024];snprintf(output,sizeof(output),"%s%s",path,name);
    FILE *f=fopen(output,"wb");assert(f);fprintf(f,"P6\n320 170\n255\n");
    for(unsigned i=0;i<320*170;i++) { lv_color32_t c={.full=lv_color_to32(framebuffer[i])}; unsigned char b[]={c.ch.red,c.ch.green,c.ch.blue};fwrite(b,1,3,f); } fclose(f);
}
int main(void) {
    lv_init(); static lv_color_t pixels[320*20]; static lv_disp_draw_buf_t buffer; static lv_disp_drv_t disp;
    lv_disp_draw_buf_init(&buffer,pixels,NULL,320*20);lv_disp_drv_init(&disp);disp.hor_res=320;disp.ver_res=170;disp.draw_buf=&buffer;disp.flush_cb=flush;lv_disp_drv_register(&disp);g=lv_group_create();
    setup_scr_screen_main(&guider_ui);lv_scr_load(guider_ui.screen_main);
    guider_ui.screen_setting=lv_obj_create(NULL);lv_scr_load(guider_ui.screen_setting);advance(100);assert(s_button);
    assert(!strcmp(lv_label_get_text(lv_obj_get_child(s_button,0)),"Boss Trans"));
    lv_event_send(s_button,LV_EVENT_CLICKED,NULL);advance(80);assert(starts==0);advance(220);assert(starts==1&&busy&&s_open);
    fake.turn=BOSS_TURN_THINK;fake.revision++;advance(100);
    assert(!strcmp(lv_label_get_text(s_status),"翻译中"));
    fake.turn=BOSS_TURN_SHOW;fake.revision++;advance(100);
    assert(!strcmp(lv_label_get_text(s_status),"看翻译"));
    fake.turn=BOSS_TURN_LISTEN;fake.revision++;advance(100);
    assert(!strcmp(lv_label_get_text(s_status),"请说话"));
    strcpy(fake.source,"我们复盘一下！");boss_rules_translate(fake.source,fake.reply,sizeof(fake.reply),fake.effect,sizeof(fake.effect));fake.translated=true;fake.utterance++;fake.revision++;fake.level=500;advance(200);
    assert(!strcmp(lv_label_get_text(s_reply),"请各位打开背锅二维码。"));
    assert(lv_label_get_long_mode(s_reply)==LV_LABEL_LONG_WRAP&&lv_label_get_long_mode(s_source)==LV_LABEL_LONG_WRAP);
    assert(s_effect_armed&&!s_effect_ready&&!s_banner_is_effect);
    advance(410);assert(s_effect_ready&&s_banner_is_effect);
    assert(!strcmp(lv_label_get_text(s_effect),"正在匹配责任人…"));snapshot_named("");
    uint32_t at=s_result_at;fake.revision++;advance(100);assert(s_result_at==at); // Status-only update never replays reveal.
    strcpy(fake.status,"网络波动 · 语音缓冲中");fake.revision++;advance(100);
    assert(!s_banner_is_effect&&strstr(lv_label_get_text(s_effect),"缓冲"));
    strcpy(fake.status,"正在聆听 · 实时转写");fake.revision++;advance(100);assert(s_banner_is_effect);
    const char *phrases[]={"公司就是你的家。","这个需求就改一点点。","还是第一版好！"};
    const char *names[]={".home.ppm",".request.ppm",".version.ppm"};
    for(unsigned i=0;i<3;i++) {
        strcpy(fake.source,phrases[i]);assert(boss_rules_translate(fake.source,fake.reply,sizeof(fake.reply),fake.effect,sizeof(fake.effect)));
        fake.utterance++;fake.revision++;advance(700);assert(s_banner_is_effect);snapshot_named(names[i]);
    }
    // A new partial sentence invalidates the previous punchline and delayed effect.
    fake.utterance++;fake.partial=true;fake.translated=false;strcpy(fake.source,"我们要");strcpy(fake.reply,"听完这句再翻译…");fake.effect[0]=0;fake.revision++;
    advance(700);assert(!s_effect_ready&&!s_effect_armed&&!s_banner_is_effect);
    // Long Unicode replies page without dropping/splitting bytes; both lines stay inside their card.
    fake.partial=false;fake.translated=true;fake.utterance++;
    strcpy(fake.reply,"领导说的话很长，但是每一页都要清清楚楚地显示，让每个字都能被看见。第二页也不能把中文切坏。再长一点也没有关系。");
    strcpy(fake.effect,"解锁技能：认真翻页");fake.revision++;advance(100);
    assert(s_reply_total>1);unsigned first_next=s_reply_next;assert(first_next>0);
    assert(((unsigned char)fake.reply[first_next]&0xc0)!=0x80);
    advance(PAGE_MS);assert(s_reply_page==2);advance(100);snapshot_named(".long.ppm");
    // Error/paused state wins over the effect and cancels pending reveal.
    fake.utterance++;fake.revision++;advance(100);assert(s_effect_armed);
    fake.state=BOSS_ERROR;strcpy(fake.status,"DNS失败 · 换近处热点重试");fake.revision++;advance(700);
    assert(!s_effect_armed&&!s_banner_is_effect&&strstr(lv_label_get_text(s_effect),"DNS"));
    fake.state=BOSS_LISTENING;strcpy(fake.status,"正在聆听 · 实时转写");fake.revision++;advance(100);
    click();advance(460);assert(stops==1&&!busy&&s_paused);advance(100);assert(!strcmp(lv_label_get_text(s_action_hint),"单击继续"));
    click();advance(460);assert(starts==2&&busy&&!s_paused);
    unsigned before=starts;click();advance(100);click();advance(250);assert(!s_open&&!busy&&starts==before&&stops==2);
    /* Deleted settings pointers must not remain valid after auto-delete. */
    assert(!s_settings&&!s_button);
    for(int i=0;i<20;i++) {enter();lv_obj_t *other=lv_obj_create(NULL);lv_scr_load(other);advance(30);assert(!s_open&&!busy);lv_obj_del(s_screen);}
    online=false;enter();assert(fake.state==BOSS_ERROR);click();advance(460);assert(fake.state==BOSS_ERROR);click();advance(100);click();advance(250);assert(!s_open);
    puts("PASS settings entry, pause/resume, double-click cancellation, foreign navigation, repeated teardown and offline retry");
    return 0;
}
