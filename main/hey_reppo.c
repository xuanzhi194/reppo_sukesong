/* Online wake/command coordinator. UI and navigation stay on the LVGL task. */
#include "hey_reppo.h"
#include "hey_reppo_intent.h"
#include "boss_service.h"
#include "boss_ui.h"
#include "boss_rules.h"
#include "voice_todo.h"
#include "inspiration_ui.h"
#include "game_machine.h"
#include "custom.h"
#include "gui_guider.h"
#include "ui_locale.h"
#include "wifi.h"
#include "nvs.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>

static atomic_bool s_enabled, s_accepting, s_owned, s_save_failed;
static atomic_uint s_pref_revision;
/* Only bounded inbox copies use a spinlock; the dialog belongs to the UI. */
static portMUX_TYPE s_inbox_lock=portMUX_INITIALIZER_UNLOCKED;
#define HEY_INBOX_CAP 8u
typedef struct { uint32_t at; bool final; char text[BOSS_SOURCE_CAP]; } hey_event_t;
typedef struct {
    hey_event_t events[HEY_INBOX_CAP], current;
    hey_parse_workspace_t parser;
} hey_inbox_t;
static hey_inbox_t *s_inbox;
static unsigned s_inbox_head, s_inbox_count;
static bool s_inbox_overflow, s_crash_recovery;
static hey_dialog_t s_dialog;
static lv_obj_t *s_button, *s_panel, *s_title, *s_heard, *s_hint, *s_indicator, *s_origin;
static lv_obj_t *s_detail, *s_detail_status, *s_toggle, *s_toggle_knob, *s_voice_bars[5];
static bool s_detail_focused, s_toggle_on, s_panel_shown;
static uint32_t s_wave_at;
static uint32_t s_retry_at, s_toast_until, s_command_at;
static bool s_was_awake, s_start_pending;
static unsigned s_start_minutes, s_failures;
static hey_intent_t s_command;
static char s_toast[192];
static boss_snapshot_t s_audio;

static uint32_t now_ms(void) { return (uint32_t)(esp_timer_get_time()/1000); }
static bool before(uint32_t now,uint32_t end) { return (int32_t)(now-end)<0; }
void hey_reppo_init(void)
{
    nvs_handle_t h;uint8_t enabled=0;
    if(nvs_open("Settings",NVS_READONLY,&h)==ESP_OK) {
        (void)nvs_get_u8(h,"hey_reppo",&enabled);nvs_close(h);
    }
    esp_reset_reason_t reason=esp_reset_reason();
    s_crash_recovery=enabled==1 && (reason==ESP_RST_PANIC||reason==ESP_RST_INT_WDT||
                                   reason==ESP_RST_TASK_WDT||reason==ESP_RST_WDT);
    atomic_store(&s_enabled,enabled==1&&!s_crash_recovery);
    ESP_LOGI("hey_reppo","boot reset_reason=%d saved_enabled=%u recovery=%d",
             reason,enabled,s_crash_recovery);
    /* A crash does not erase the preference; explicitly toggling resumes it. */
}
void hey_reppo_storage_poll(void)
{
    static unsigned saved;static uint32_t retry;
    unsigned revision=atomic_load(&s_pref_revision);
    uint32_t now=now_ms();if(revision==saved||before(now,retry)) return;
    nvs_handle_t h;esp_err_t err=nvs_open("Settings",NVS_READWRITE,&h);
    if(err==ESP_OK) {
        err=nvs_set_u8(h,"hey_reppo",atomic_load(&s_enabled)?1:0);
        if(err==ESP_OK) err=nvs_commit(h);
        nvs_close(h);
    }
    atomic_store(&s_save_failed,err!=ESP_OK);
    if(err==ESP_OK) saved=revision;
    else {retry=now+5000;ESP_LOGW("hey_reppo","setting save failed: %d",err);}
}
bool hey_reppo_owns_audio(void) {return atomic_load(&s_owned);}
static void reset_dialog(void)
{
    portENTER_CRITICAL(&s_inbox_lock);
    s_inbox_head=s_inbox_count=0;s_inbox_overflow=false;
    portEXIT_CRITICAL(&s_inbox_lock);
    hey_dialog_reset(&s_dialog);
    s_was_awake=false;s_origin=NULL;
}
static void release_audio(void)
{
    atomic_store(&s_accepting,false);
    if(atomic_load(&s_owned)) boss_service_stop();
}
void hey_reppo_yield(void)
{
    if(atomic_load(&s_owned)) release_audio();
    reset_dialog();s_command.action=HEY_NONE;s_start_pending=false;
}
static bool inbox_prepare(void)
{
    if(s_inbox) return true;
    s_inbox=heap_caps_calloc(1,sizeof(*s_inbox),MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT);
    return s_inbox!=NULL;
}
static void receive(const char *text,bool final)
{
    /* WebSocket callback: no parsing, heap allocation, LVGL or blocking wait. */
    uint32_t at=now_ms();
    portENTER_CRITICAL(&s_inbox_lock);
    if(s_inbox&&atomic_load(&s_enabled)&&atomic_load(&s_accepting)&&!s_inbox_overflow) {
        if(s_inbox_count==HEY_INBOX_CAP) s_inbox_overflow=true;
        else {
            hey_event_t *event=&s_inbox->events[(s_inbox_head+s_inbox_count)%HEY_INBOX_CAP];
            event->at=at;event->final=final;
            boss_text_copy(event->text,sizeof(event->text),text);++s_inbox_count;
        }
    }
    portEXIT_CRITICAL(&s_inbox_lock);
}
static void receive_poll(uint32_t now)
{
    if(!s_inbox) return;
    /* Fixed work budget, preserving partial-before-final order. Overflow drops
     * the whole turn so a partial command can never become an accidental action. */
    for(unsigned i=0;i<HEY_INBOX_CAP;i++) {
        portENTER_CRITICAL(&s_inbox_lock);
        bool overflow=s_inbox_overflow, available=s_inbox_count!=0;
        if(overflow) {s_inbox_head=s_inbox_count=0;s_inbox_overflow=false;}
        else if(available) {
            s_inbox->current=s_inbox->events[s_inbox_head];
            s_inbox_head=(s_inbox_head+1)%HEY_INBOX_CAP;--s_inbox_count;
        }
        portEXIT_CRITICAL(&s_inbox_lock);
        if(overflow) {
            hey_dialog_reset(&s_dialog);
            ESP_LOGW("hey_reppo","inbox overflow: turn discarded; say wake phrase again");
            return;
        }
        if(!available) return;
        if(!atomic_load(&s_enabled)||!atomic_load(&s_accepting)) continue;
        hey_event_t *event=&s_inbox->current;
        if((int32_t)(now-event->at)>2000) {hey_dialog_reset(&s_dialog);continue;}
        bool was_awake=s_dialog.awake, was_pending=s_dialog.pending;
        hey_dialog_receive_with_workspace(&s_dialog,event->text,event->final,now,&s_inbox->parser);
        if(!was_awake&&s_dialog.awake) ESP_LOGI("hey_reppo","wake_detected final=%d ui_stack_free=%u",event->final,(unsigned)uxTaskGetStackHighWaterMark(NULL));
        if(!was_pending&&s_dialog.pending) ESP_LOGI("hey_reppo","command_ready action=%u",(unsigned)s_dialog.intent.action);
    }
}
static void toast(const char *text,unsigned ms)
{
    boss_text_copy(s_toast,sizeof(s_toast),text);s_toast_until=now_ms()+ms;
}
static void set_enabled(bool enabled)
{
    s_crash_recovery=false;
    atomic_store(&s_enabled,enabled);atomic_fetch_add(&s_pref_revision,1);
    if(!enabled) hey_reppo_yield();
    s_retry_at=0;s_failures=0;
}
/* Reppo's settings palette and right-hand detail area: no modal on focus. */
#define HEY_INK 0x274A6E
#define HEY_MUTED 0x768794
#define HEY_LINE 0xE1E7EC
#define HEY_ACCENT 0x72AE9B
static lv_obj_t *make_label(lv_obj_t *parent,int x,int y,int w,const lv_font_t *font,uint32_t color)
{
    lv_obj_t *l=lv_label_create(parent);lv_obj_set_pos(l,x,y);lv_obj_set_width(l,w);
    lv_obj_set_style_text_font(l,font,0);lv_obj_set_style_text_color(l,lv_color_hex(color),0);
    lv_obj_set_style_text_line_space(l,2,0);lv_obj_clear_flag(l,LV_OBJ_FLAG_CLICKABLE);
    lv_label_set_text(l,"");return l;
}
static lv_obj_t *surface(lv_obj_t *parent,int x,int y,int w,int h,uint32_t color,int radius)
{
    lv_obj_t *o=lv_obj_create(parent);lv_obj_remove_style_all(o);
    lv_obj_set_pos(o,x,y);lv_obj_set_size(o,w,h);
    lv_obj_clear_flag(o,LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(o,lv_color_hex(color),0);lv_obj_set_style_bg_opa(o,LV_OPA_COVER,0);
    lv_obj_set_style_radius(o,radius,0);return o;
}
static void text_if_changed(lv_obj_t *label,const char *text)
{
    if(strcmp(lv_label_get_text(label),text)) lv_label_set_text(label,text);
}
static void slide_y(void *obj,int32_t value) {lv_obj_set_style_translate_y(obj,value,0);}
static void slide_x(void *obj,int32_t value) {lv_obj_set_style_translate_x(obj,value,0);}
static void knob_x(void *obj,int32_t value) {lv_obj_set_x(obj,value);}
static void motion(lv_obj_t *obj,lv_anim_exec_xcb_t exec,int from,int to,unsigned ms)
{
    lv_anim_del(obj,exec);
    lv_anim_t a;lv_anim_init(&a);lv_anim_set_var(&a,obj);lv_anim_set_exec_cb(&a,exec);
    lv_anim_set_values(&a,from,to);lv_anim_set_time(&a,ms);lv_anim_set_path_cb(&a,lv_anim_path_ease_out);
    exec(obj,from);lv_anim_start(&a);
}
static void detail_deleted(lv_event_t *e)
{
    if(lv_event_get_target(e)!=s_detail) return;
    s_detail=s_detail_status=s_toggle=s_toggle_knob=NULL;s_detail_focused=false;
}
static void detail_create(void)
{
    if(s_detail) return;
    s_detail=surface(guider_ui.screen_setting,135,31,170,126,0xFFFFFF,10);
    lv_obj_set_style_border_width(s_detail,1,0);lv_obj_set_style_border_color(s_detail,lv_color_hex(HEY_LINE),0);
    lv_obj_add_event_cb(s_detail,detail_deleted,LV_EVENT_DELETE,NULL);
    lv_obj_t *heading=make_label(s_detail,10,9,102,ui_font_14(),HEY_INK);lv_label_set_text(heading,"语音唤醒");
    s_toggle=surface(s_detail,116,8,42,24,0xE5E9ED,LV_RADIUS_CIRCLE);
    s_toggle_knob=surface(s_toggle,3,3,18,18,0xFFFFFF,LV_RADIUS_CIRCLE);
    s_detail_status=make_label(s_detail,10,33,150,ui_font_12(),HEY_MUTED);
    surface(s_detail,10,53,148,1,HEY_LINE,0);
    lv_obj_t *guide=make_label(s_detail,10,61,150,ui_font_12(),HEY_INK);
    lv_label_set_text(guide,"说 Hi 或“嗨”\n再说你想做的事");
    lv_obj_t *notice=make_label(s_detail,10,94,150,ui_font_12(),HEY_MUTED);
    lv_label_set_text(notice,"开启后持续上传语音\n需 Wi-Fi · 使用语音额度");
    s_toggle_on=atomic_load(&s_enabled);
    lv_obj_set_x(s_toggle_knob,s_toggle_on?21:3);
    lv_obj_set_style_bg_color(s_toggle,lv_color_hex(s_toggle_on?HEY_ACCENT:0xE5E9ED),0);
    lv_obj_add_flag(s_detail,LV_OBJ_FLAG_HIDDEN);
}
static void detail_refresh(void)
{
    if(!s_detail) return;
    bool enabled=atomic_load(&s_enabled);
    if(s_toggle_on!=enabled) {
        s_toggle_on=enabled;
        motion(s_toggle_knob,knob_x,lv_obj_get_x(s_toggle_knob),enabled?21:3,180);
        lv_obj_set_style_bg_color(s_toggle,lv_color_hex(enabled?HEY_ACCENT:0xE5E9ED),0);
    }
    const char *state=atomic_load(&s_save_failed)?"设置未保存 · 正在重试":!enabled?(s_crash_recovery?"异常重启 · 单击恢复":"已关闭 · 单击开启"):
        !is_wifi_connected()?"等待联网 · 单击关闭":s_failures?"连接重试中 · 单击关闭":
        atomic_load(&s_owned)&&strstr(s_audio.status,"缓冲")?"语音延迟 · 单击关闭":"已开启 · 单击关闭";
    text_if_changed(s_detail_status,state);
}
static void detail_show(void)
{
    detail_create();detail_refresh();
    if(!s_detail_focused) {
        s_detail_focused=true;lv_obj_clear_flag(s_detail,LV_OBJ_FLAG_HIDDEN);
        lv_obj_move_foreground(s_detail);motion(s_detail,slide_x,5,0,180);
    }
}
static void setting_event(lv_event_t *e)
{
    lv_event_code_t code=lv_event_get_code(e);
    if(code==LV_EVENT_DELETE) {s_button=NULL;s_detail_focused=false;return;}
    if(code==LV_EVENT_FOCUSED) detail_show();
    if(code==LV_EVENT_DEFOCUSED) {
        s_detail_focused=false;
        if(s_detail) {lv_anim_del(s_detail,slide_x);lv_obj_add_flag(s_detail,LV_OBJ_FLAG_HIDDEN);}
    }
    if(code==LV_EVENT_CLICKED) {
        set_enabled(!atomic_load(&s_enabled));detail_show();
    }
}
static void settings_poll(void)
{
    /* Guider's *_del flag means "recreate on next load", not "not alive":
     * generated setup leaves it true on a visible page. The custom list helper
     * checks live ownership before adding a row, including stale address reuse. */
    if(lv_scr_act()!=guider_ui.screen_setting) return;
    if(!s_button) {
        s_button=screen_setting_list_add_btn("Hey Reppo");if(!s_button) return;
        lv_obj_add_event_cb(s_button,setting_event,LV_EVENT_ALL,NULL);
        /* Inherit the same font, width and lift animation as the other rows. */
    }
    detail_refresh();
}
static void ensure_ui(void)
{
    if(s_panel) return;
    s_panel=surface(lv_layer_top(),8,64,304,100,0xFFFFFF,16);
    lv_obj_set_style_border_width(s_panel,1,0);lv_obj_set_style_border_color(s_panel,lv_color_hex(HEY_LINE),0);
    s_title=make_label(s_panel,14,9,230,ui_font_16(),HEY_INK);
    s_heard=make_label(s_panel,14,34,276,ui_font_14(),0x354F61);
    lv_obj_set_height(s_heard,20);lv_label_set_long_mode(s_heard,LV_LABEL_LONG_DOT);
    s_hint=make_label(s_panel,14,59,276,ui_font_12(),HEY_MUTED);lv_obj_set_height(s_hint,34);
    for(unsigned i=0;i<5;i++) s_voice_bars[i]=surface(s_panel,256+i*6,16,3,4,HEY_ACCENT,2);
    s_indicator=make_label(lv_layer_top(),207,1,110,ui_font_12(),HEY_MUTED);
    lv_obj_set_style_bg_color(s_indicator,lv_color_hex(0xFFFFFF),0);
    lv_obj_set_style_bg_opa(s_indicator,LV_OPA_COVER,0);lv_obj_set_style_radius(s_indicator,8,0);
    lv_obj_set_style_border_color(s_indicator,lv_color_hex(HEY_LINE),0);lv_obj_set_style_border_width(s_indicator,1,0);
    lv_obj_set_style_pad_left(s_indicator,5,0);lv_obj_set_style_pad_top(s_indicator,1,0);
    lv_obj_set_height(s_indicator,19);lv_label_set_long_mode(s_indicator,LV_LABEL_LONG_CLIP);
    lv_obj_add_flag(s_panel,LV_OBJ_FLAG_HIDDEN);lv_obj_add_flag(s_indicator,LV_OBJ_FLAG_HIDDEN);
}
static void panel_hidden(lv_anim_t *a)
{
    if(!s_panel_shown) lv_obj_add_flag(a->var,LV_OBJ_FLAG_HIDDEN);
}
static void panel_show(bool show)
{
    if(s_panel_shown==show) return;
    s_panel_shown=show;lv_anim_del(s_panel,slide_y);
    if(show) {
        lv_obj_clear_flag(s_panel,LV_OBJ_FLAG_HIDDEN);motion(s_panel,slide_y,10,0,200);
    } else {
        lv_anim_t a;lv_anim_init(&a);lv_anim_set_var(&a,s_panel);lv_anim_set_exec_cb(&a,slide_y);
        lv_anim_set_values(&a,lv_obj_get_style_translate_y(s_panel,0),10);lv_anim_set_time(&a,130);
        lv_anim_set_path_cb(&a,lv_anim_path_ease_in);lv_anim_set_ready_cb(&a,panel_hidden);lv_anim_start(&a);
    }
}
static void wave_poll(bool awake,uint32_t now)
{
    if(now-s_wave_at<100) return;
    s_wave_at=now;
    static const uint8_t heights[]={5,9,15,11,6,4};
    for(unsigned i=0;i<5;i++) {
        unsigned h=awake?heights[(now/140+i)%6]:4;
        lv_obj_set_height(s_voice_bars[i],h);lv_obj_set_y(s_voice_bars[i],19-h/2);
    }
}
static bool eligible(void)
{
    if(boss_ui_is_open()||voice_todo_is_open()||voice_todo_audio_busy()||voice_todo_entry_pending()||
       inspiration_ui_is_open()||game_machine_is_open()||focus_timer_is_running()||
       countdown_start_label||stopwatch_start_label||countdown_timeout_pending) return false;
    lv_obj_t *screen=lv_scr_act();
    /* Never steal navigation from an editor, password entry, boot or modal. */
    return screen==guider_ui.screen_main||screen==guider_ui.screen_setting||
           screen==guider_ui.screen_todolist||screen==guider_ui.screen_donelist||
           screen==guider_ui.screen_pomodoro||screen==guider_ui.screen_keep||
           screen==guider_ui.screen_gash||screen==guider_ui.screen_book||
           screen==guider_ui.screen_picture||screen==guider_ui.screen_answer||screen==guider_ui.screen_timing;
}
static void load_screen(lv_obj_t **screen,bool *deleted,ui_setup_scr_t setup)
{
    if(!*deleted&&lv_scr_act()==*screen&&lv_obj_is_valid(*screen)) return;
    lv_obj_t *old=lv_scr_act();inspiration_ui_mark_guider_deleted(old);
    bool old_deleted=true;
    if(!*screen||!lv_obj_is_valid(*screen)) *deleted=true;
    ui_load_scr_animation(&guider_ui,screen,*deleted,&old_deleted,setup,
                         LV_SCR_LOAD_ANIM_FADE_ON,160,0,false,true);
}
static void execute(hey_intent_t command)
{
    /* Re-check at dispatch: a knob operation can change the page while the
     * network is closing. A running/paused timer must never be replaced. */
    if(!eligible()) {toast("当前操作尚未结束，请先用旋钮返回",2500);return;}
#define GO(name) load_screen(&guider_ui.name,&guider_ui.name##_del,setup_scr_##name)
    switch(command.action) {
    case HEY_HOME: GO(screen_main);break;
    case HEY_TODO: GO(screen_todolist);break;
    case HEY_DONE: GO(screen_donelist);break;
    case HEY_FOCUS_PAGE: GO(screen_pomodoro);break;
    case HEY_START_FOCUS:
        focus_en=false;focus_session_cancel();current_task_index=-1;
        GO(screen_pomodoro);s_start_pending=true;s_start_minutes=command.minutes;break;
    case HEY_KEEP: GO(screen_keep);break;
    case HEY_GASH: GO(screen_gash);break;
    case HEY_BOOK: GO(screen_book);break;
    case HEY_GAME: GO(screen_game);break;
    case HEY_SETTINGS: GO(screen_setting);break;
    case HEY_PHOTO: GO(screen_picture);break;
    case HEY_ANSWER: GO(screen_answer);break;
    case HEY_CLOCK: GO(screen_timing);break;
    case HEY_IDEAS: inspiration_ui_open_hub();break;
    case HEY_BOSS: boss_ui_request_enter();break;
    case HEY_DISABLE: set_enabled(false);toast("已关闭 Hey Reppo",2000);break;
    default: break;
    }
#undef GO
}
void hey_reppo_poll(void)
{
    uint32_t now=now_ms();bool transition=lv_disp_get_default()->scr_to_load!=NULL;
    if(!transition) settings_poll();
    bool enabled=atomic_load(&s_enabled), allowed=eligible();
    if(!enabled&&!s_panel&&!s_toast_until) return;
    ensure_ui();
    if(atomic_load(&s_owned)&&!boss_service_busy()) {
        /* A fast connection failure can finish between UI polls. Preserve
         * backoff even when there was no opportunity to observe BOSS_ERROR. */
        if(atomic_load(&s_accepting)) {
            reset_dialog();s_failures++;s_retry_at=now+(s_failures<3?5000:30000);
            toast("联网唤醒连接失败，稍后自动重试",2500);
        }
        atomic_store(&s_owned,false);atomic_store(&s_accepting,false);
    }
    if(s_start_pending&&!transition) {
        s_start_pending=false;
        if(lv_scr_act()==guider_ui.screen_pomodoro&&!countdown_start_label&&!stopwatch_start_label&&s_start_minutes>=1&&s_start_minutes<=180) {
            timer_all_num=(int16_t)s_start_minutes;focus_session_active=false;
            lv_label_set_text_fmt(guider_ui.screen_pomodoro_label_timer,"%02u:%02u:00",s_start_minutes/60,s_start_minutes%60);
            lv_event_send(guider_ui.screen_pomodoro_btn_start,LV_EVENT_CLICKED,NULL);
            ESP_LOGI("hey_reppo","focus started: %u min",s_start_minutes);
        }
    }
    if(s_command.action!=HEY_NONE) {
        if(before(now,s_command_at+8000)&&!transition&&lv_scr_act()==s_origin) {
            if(!boss_service_busy()) {
                hey_intent_t command=s_command;s_command.action=HEY_NONE;s_origin=NULL;
                execute(command);s_retry_at=now+2000;
            }
        } else if(!transition) {s_command.action=HEY_NONE;reset_dialog();toast("页面已变化或语音释放超时，请重新唤醒",2500);}
    }
    allowed=eligible();
    if(!enabled||!allowed||!is_wifi_connected()) {
        if(atomic_load(&s_accepting)) {release_audio();reset_dialog();}
    } else if(!transition&&s_command.action==HEY_NONE&&!s_start_pending&&!boss_service_busy()&& !before(now,s_retry_at)) {
        reset_dialog();atomic_store(&s_accepting,true);atomic_store(&s_owned,true);
        if(!inbox_prepare()||!boss_service_start_asr(receive)) {
            atomic_store(&s_owned,false);atomic_store(&s_accepting,false);
            s_failures++;s_retry_at=now+(s_failures<3?5000:30000);
        }
    }
    if(atomic_load(&s_owned)) {
        boss_service_snapshot(&s_audio);
        if(s_audio.state==BOSS_ERROR) {
            if(atomic_load(&s_accepting)) {
                release_audio();reset_dialog();s_failures++;s_retry_at=now+(s_failures<3?5000:30000);
                toast("联网唤醒连接失败，稍后自动重试",2500);
            }
        } else if(s_audio.state==BOSS_LISTENING) s_failures=0;
    }
    hey_dialog_t dialog;
    receive_poll(now);hey_dialog_tick(&s_dialog,now);dialog=s_dialog;
    if(dialog.awake&&!s_was_awake) {s_origin=lv_scr_act();idle_sleep_request_wake();}
    if(dialog.awake&&s_was_awake&&!transition&&lv_scr_act()!=s_origin) {reset_dialog();dialog.awake=dialog.pending=false;}
    s_was_awake=dialog.awake;
    if(dialog.awake) {
        idle_sleep_note_activity();s_toast_until=0;
        text_if_changed(s_title,"Hey Reppo · 我在听");text_if_changed(s_heard,dialog.heard);text_if_changed(s_hint,dialog.hint);
        if(dialog.pending&&s_command.action==HEY_NONE) {
            s_command=dialog.intent;s_command_at=now;s_origin=lv_scr_act();
            release_audio();
            reset_dialog();s_origin=lv_scr_act();
            s_was_awake=false;toast("收到，正在为你打开…",2500);
        }
    } else if(before(now,s_toast_until)) {
        text_if_changed(s_title,"Hey Reppo");text_if_changed(s_heard,"");text_if_changed(s_hint,s_toast);
    }
    bool settings_detail=s_detail_focused&&lv_scr_act()==guider_ui.screen_setting;
    bool show=dialog.awake||(before(now,s_toast_until)&&!settings_detail);
    panel_show(show);wave_poll(dialog.awake,now);
    bool indicator=enabled&&allowed&&!dialog.awake&&!show&&!settings_detail;
    if(indicator) {
        const char *hint=atomic_load(&s_save_failed)?"Hey · 保存失败":!is_wifi_connected()?"Hey · 离线":
            atomic_load(&s_accepting)&&s_audio.state==BOSS_LISTENING?
            (strstr(s_audio.status,"缓冲")?"Hey · 语音延迟":"Hey · 待唤醒"):"Hey · 连接中";
        text_if_changed(s_indicator,hint);lv_obj_clear_flag(s_indicator,LV_OBJ_FLAG_HIDDEN);
    } else lv_obj_add_flag(s_indicator,LV_OBJ_FLAG_HIDDEN);
}
