/**
 * @file voice_todo.c
 * @brief 敲击进入共享语音入口：待办、灵感与本地回看。
 *
 * 麦克风只在这一次录音里打开。思必驰凭证由后端保存，通义沿用原配置。
 */
#include "voice_todo.h"
#include "voice_asr.h"
#include "ui_locale.h"
#include "inspiration_ui.h"
#include "inspiration_storage.h"
#include "cJSON.h"
#include "esp_attr.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "lvgl.h"
#include "gui_guider.h"
#include "custom.h"
#include "buzzer.h"
#include "wifi.h"
#include "zts6672_mic.h"
#include "todo_storage.h"
#include "rx8130.h"
#include "esp_crt_bundle.h"
#include "esp_heap_caps.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <time.h>

extern const lv_font_t voice_todo_font;
extern const lv_font_t voice_todo_title_font;

static const char *TAG = "VOICE";

/* 调试阶段直接放在固件里。 */
static const char *DASHSCOPE_KEY =
    "sk-ws-H.PEHIPMI.CcDs.MEUCIQDFSutV7iRYepTCeinfRLWN88HmUKuPtaguXytiEkSdrQIgFL0Wq8rrmV3GIuygw0CX9NtTNtiGVFZGXviUIDhCvhQ";

#define ASR_URL     "https://dashscope.aliyuncs.com/api/v1/services/aigc/multimodal-generation/generation"
#define REFINE_URL  "https://dashscope.aliyuncs.com/compatible-mode/v1/chat/completions"
#define REFINE_MODEL "qwen-flash"

#define VOICE_RATE          16000
#define VOICE_MAX_MS        15000
#define VOICE_TODO_MS       4000
#define VOICE_MAX_SAMPLES   (VOICE_RATE * VOICE_MAX_MS / 1000)
#define VOICE_CHUNK         (VOICE_RATE / 10)
#define VOICE_CLICK_MS      450
#define VOICE_HTTP_MS       25000
#define VOICE_RESP_MAX      8192
typedef enum {
    VS_WIFI = 0,
    VS_READY,
    VS_LISTEN,
    VS_THINK,
    VS_RESULT,
    VS_FAIL,
    VS_SAVED,
} voice_state_t;

static volatile bool s_enter_req;
static bool s_capture_req, s_inspiration;
static bool s_job_inspiration;
static volatile uint32_t s_audio_ms, s_audio_level;
static inspiration_record_t s_job_idea EXT_RAM_BSS_ATTR;
static volatile bool s_wificfg_return;
static volatile bool s_open;
static volatile bool s_job_running;
static volatile bool s_job_recorded;
static volatile bool s_job_done;
static volatile bool s_job_ok;
static volatile uint16_t s_job_minutes;
static char s_job_text[TODO_NAME_LEN];
static uint16_t s_minutes;
static uint32_t s_feedback_at, s_feedback_seconds;

#define VOICE_BARS 7

static lv_obj_t *s_scr;
static lv_obj_t *s_card;
static lv_obj_t *s_title;
static lv_obj_t *s_chip;
static lv_obj_t *s_chip_icon;
static lv_obj_t *s_chip_text;
static lv_obj_t *s_orb;
static lv_obj_t *s_orb_icon;
static lv_obj_t *s_ring[2];
static lv_obj_t *s_spin;
static lv_obj_t *s_body;
static lv_obj_t *s_wave;
static lv_obj_t *s_record_bar, *s_record_time;
static lv_obj_t *s_bar[VOICE_BARS];
static lv_obj_t *s_ticket;
static lv_obj_t *s_ticket_text;
static lv_obj_t *s_hint;
static lv_obj_t *s_sel;
static lv_obj_t *s_btn_main;
static lv_obj_t *s_btn_main_label;
static lv_obj_t *s_btn_add;
static lv_obj_t *s_btn_drop;
static lv_timer_t *s_click_timer;
static lv_timer_t *s_saved_timer;
static voice_state_t s_state;
static bool s_click_armed;
static uint32_t s_click_tick;
static int s_pending_act;
static bool s_wifi_seen;
static bool s_sel_placed;
static char s_shown[TODO_NAME_LEN];
static bool s_voice_del_flag;

static void voice_apply(voice_state_t state);
static void voice_show_obj(lv_obj_t *obj, bool show);
static void voice_go_main(void);
static void voice_open_wifi(void);
static void voice_start_listen(void);
static void voice_click_timer_cb(lv_timer_t *timer);

void voice_todo_request_enter(void)
{
    s_enter_req = true;
}

void voice_todo_start_mode(bool inspiration)
{
    if (s_job_running) return;
    s_inspiration = inspiration;
    s_capture_req = true;
}

bool voice_todo_is_open(void)
{
    return s_open || s_wificfg_return || inspiration_ui_is_open();
}

bool voice_todo_wants_wificfg_return(void)
{
    return s_wificfg_return;
}

/* ------------------------------------------------------------------ */
/* 视觉：沿用设置页的浅底、白卡片、藏青 0x274A6E。                       */
/* ------------------------------------------------------------------ */

#define VOICE_NAVY      0x274A6E
#define VOICE_INK       0x3D5670
#define VOICE_MUTED     0x8A9BB0
#define VOICE_TINT      0xEEF3F8
#define VOICE_WARN      0xA65A5A
#define VOICE_WARN_TINT 0xF6EAEA

/* 卡片内：麦克风圆心 (52, 72)，用居中对齐偏移表示，改尺寸时圆心不动。 */
#define ORB_DX          (-96)
#define ORB_DY          (-3)
#define ORB_SIZE        54
#define BTN_Y           108
#define BTN_H           30

static void fx_opa(void *obj, int32_t v)
{
    lv_obj_set_style_opa((lv_obj_t *)obj, (lv_opa_t)v, LV_PART_MAIN);
}

static void fx_ty(void *obj, int32_t v)
{
    lv_obj_set_style_translate_y((lv_obj_t *)obj, (lv_coord_t)v, LV_PART_MAIN);
}

static void fx_tx(void *obj, int32_t v)
{
    lv_obj_set_style_translate_x((lv_obj_t *)obj, (lv_coord_t)v, LV_PART_MAIN);
}

static void fx_y(void *obj, int32_t v)
{
    lv_obj_set_y((lv_obj_t *)obj, (lv_coord_t)v);
}

static void fx_x(void *obj, int32_t v)
{
    lv_obj_set_x((lv_obj_t *)obj, (lv_coord_t)v);
}

static void fx_w(void *obj, int32_t v)
{
    lv_obj_set_width((lv_obj_t *)obj, (lv_coord_t)v);
}

static void fx_h(void *obj, int32_t v)
{
    lv_obj_set_height((lv_obj_t *)obj, (lv_coord_t)v);
}

static void fx_size(void *obj, int32_t v)
{
    lv_obj_set_size((lv_obj_t *)obj, (lv_coord_t)v, (lv_coord_t)v);
}

static void fx_ring(void *obj, int32_t v)
{
    /* v: 0..100，圈从球边扩散到 1.9 倍，同时淡出。 */
    lv_coord_t size = (lv_coord_t)(ORB_SIZE + (ORB_SIZE * 9 / 10) * v / 100);
    lv_obj_set_size((lv_obj_t *)obj, size, size);
    lv_obj_set_style_border_opa((lv_obj_t *)obj, (lv_opa_t)(170 - 170 * v / 100), LV_PART_MAIN);
}

static void fx_glow(void *obj, int32_t v)
{
    lv_obj_set_style_shadow_spread((lv_obj_t *)obj, (lv_coord_t)v, LV_PART_MAIN);
    lv_obj_set_style_shadow_opa((lv_obj_t *)obj, (lv_opa_t)(60 + v * 10), LV_PART_MAIN);
}

static void fx_run(lv_obj_t *obj, lv_anim_exec_xcb_t cb, int32_t from, int32_t to,
                   uint32_t time_ms, uint32_t delay_ms, lv_anim_path_cb_t path,
                   uint32_t playback_ms, uint16_t repeat)
{
    lv_anim_t a;
    if (obj == NULL) {
        return;
    }
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_exec_cb(&a, cb);
    lv_anim_set_values(&a, from, to);
    lv_anim_set_time(&a, time_ms);
    lv_anim_set_delay(&a, delay_ms);
    lv_anim_set_path_cb(&a, path);
    if (playback_ms > 0) {
        lv_anim_set_playback_time(&a, playback_ms);
    }
    lv_anim_set_repeat_count(&a, repeat);
    lv_anim_start(&a);
    /* 延迟期间先停在起点，避免先闪出终态再跳回。 */
    if (delay_ms > 0) {
        cb(obj, from);
    }
}

/** 淡入 + 上浮。用于入场和每次状态切换时的内容替换。 */
static void fx_rise(lv_obj_t *obj, uint32_t delay_ms, lv_coord_t dist)
{
    if (obj == NULL) {
        return;
    }
    lv_anim_del(obj, fx_opa);
    lv_anim_del(obj, fx_ty);
    fx_run(obj, fx_opa, LV_OPA_TRANSP, LV_OPA_COVER, 240, delay_ms,
           lv_anim_path_ease_out, 0, 0);
    fx_run(obj, fx_ty, dist, 0, 320, delay_ms, lv_anim_path_ease_out, 0, 0);
}

/** 淡入 + 从左滑入。 */
static void fx_slide(lv_obj_t *obj, uint32_t delay_ms, lv_coord_t dist)
{
    if (obj == NULL) {
        return;
    }
    lv_anim_del(obj, fx_opa);
    lv_anim_del(obj, fx_tx);
    fx_run(obj, fx_opa, LV_OPA_TRANSP, LV_OPA_COVER, 240, delay_ms,
           lv_anim_path_ease_out, 0, 0);
    fx_run(obj, fx_tx, dist, 0, 360, delay_ms, lv_anim_path_ease_out, 0, 0);
}

static void voice_show_obj(lv_obj_t *obj, bool show)
{
    if (obj == NULL) {
        return;
    }
    if (show) {
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
}

static void voice_fx_stop(void)
{
    int i;
    if (s_orb != NULL) {
        lv_anim_del(s_orb, fx_glow);
        lv_anim_del(s_orb, fx_size);
        lv_obj_set_size(s_orb, ORB_SIZE, ORB_SIZE);
        fx_glow(s_orb, 0);
    }
    for (i = 0; i < 2; i++) {
        if (s_ring[i] != NULL) {
            lv_anim_del(s_ring[i], NULL);
            voice_show_obj(s_ring[i], false);
        }
    }
    for (i = 0; i < VOICE_BARS; i++) {
        if (s_bar[i] != NULL) {
            lv_anim_del(s_bar[i], NULL);
            lv_obj_set_height(s_bar[i], 6);
        }
    }
}

static void voice_fx_rings(uint32_t period_ms, uint16_t repeat)
{
    int i;
    for (i = 0; i < 2; i++) {
        if (s_ring[i] == NULL) {
            continue;
        }
        voice_show_obj(s_ring[i], true);
        fx_run(s_ring[i], fx_ring, 0, 100, period_ms, (uint32_t)i * (period_ms / 2),
               lv_anim_path_ease_out, 0, repeat);
    }
}

static void voice_fx_bars(bool listening)
{
    static const uint16_t peak[VOICE_BARS] = { 14, 24, 30, 20, 28, 18, 12 };
    static const uint16_t speed[VOICE_BARS] = { 260, 210, 300, 180, 240, 280, 220 };
    int i;
    for (i = 0; i < VOICE_BARS; i++) {
        if (s_bar[i] == NULL) {
            continue;
        }
        if (listening) {
            fx_run(s_bar[i], fx_h, 6, peak[i], speed[i], (uint32_t)i * 40U,
                   lv_anim_path_ease_in_out, speed[i], LV_ANIM_REPEAT_INFINITE);
        } else {
            /* 整理中：一道波从左走到右。 */
            fx_run(s_bar[i], fx_h, 6, 16, 240, (uint32_t)i * 90U,
                   lv_anim_path_ease_in_out, 240, LV_ANIM_REPEAT_INFINITE);
        }
    }
}

static void voice_fx_set(voice_state_t state)
{
    voice_fx_stop();
    voice_show_obj(s_spin, state == VS_THINK);
    voice_show_obj(s_wave, state == VS_LISTEN || state == VS_THINK);
    if (s_wave) { lv_obj_set_y(s_wave, state == VS_THINK ? 87 : 64); lv_obj_set_height(s_wave, state == VS_THINK ? 18 : 32); }
    switch (state) {
    case VS_READY:
        fx_run(s_orb, fx_glow, 0, 6, 900, 0, lv_anim_path_ease_in_out, 900,
               LV_ANIM_REPEAT_INFINITE);
        break;
    case VS_LISTEN:
        voice_fx_rings(1200, LV_ANIM_REPEAT_INFINITE);
        /* The main poll paints bars from measured microphone energy. */
        break;
    case VS_THINK:
        voice_fx_bars(false);
        break;
    case VS_SAVED:
        fx_run(s_orb, fx_size, 18, ORB_SIZE, 420, 0, lv_anim_path_overshoot, 0, 0);
        voice_fx_rings(700, 1);
        break;
    default:
        break;
    }
}

/* ------------------------------------------------------------------ */
/* 焦点：一块藏青滑块垫在当前按钮下面，旋钮转动时滑过去。               */
/* ------------------------------------------------------------------ */

static void voice_sel_to(lv_obj_t *btn, bool animate)
{
    lv_coord_t x;
    lv_coord_t w;
    if (s_sel == NULL || btn == NULL || lv_obj_has_flag(btn, LV_OBJ_FLAG_HIDDEN)) {
        return;
    }
    x = lv_obj_get_x(btn);
    w = lv_obj_get_width(btn);
    voice_show_obj(s_sel, true);
    lv_anim_del(s_sel, fx_x);
    lv_anim_del(s_sel, fx_w);
    if (!animate || !s_sel_placed) {
        lv_obj_set_pos(s_sel, x, BTN_Y);
        lv_obj_set_width(s_sel, w);
        s_sel_placed = true;
        return;
    }
    fx_run(s_sel, fx_x, lv_obj_get_x(s_sel), x, 220, 0, lv_anim_path_ease_out, 0, 0);
    fx_run(s_sel, fx_w, lv_obj_get_width(s_sel), w, 220, 0, lv_anim_path_ease_out, 0, 0);
}

static void voice_btn_focus(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_FOCUSED) {
        voice_sel_to(lv_event_get_target(e), true);
    }
}

static void voice_style_btn(lv_obj_t *btn)
{
    clear_focus_box(btn);
    lv_obj_set_style_radius(btn, BTN_H / 2, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(btn, 0, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_outline_width(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    /* 未选中：细描边 + 藏青字；选中：滑块垫底 + 白字。 */
    lv_obj_set_style_border_width(btn, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(btn, lv_color_hex(VOICE_NAVY), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(btn, 90, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(btn, lv_color_hex(VOICE_NAVY), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(btn, 170, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(btn, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_text_opa(btn, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(btn, &voice_todo_font, LV_PART_MAIN);
    lv_obj_add_event_cb(btn, voice_btn_focus, LV_EVENT_FOCUSED, NULL);
}

static void voice_group_set(lv_obj_t *a, lv_obj_t *b)
{
    lv_indev_t *indev = NULL;

    if (g == NULL) {
        g = lv_group_create();
    } else {
        lv_group_remove_all_objs(g);
        lv_group_set_edge_cb(g, NULL);
        lv_group_set_wrap(g, true);
    }
    lv_group_set_default(g);
    while ((indev = lv_indev_get_next(indev)) != NULL) {
        if (lv_indev_get_type(indev) == LV_INDEV_TYPE_KEYPAD) {
            lv_indev_set_group(indev, g);
        }
    }
    if (a != NULL) {
        lv_group_add_obj(g, a);
    }
    if (b != NULL) {
        lv_group_add_obj(g, b);
    }
    if (a != NULL) {
        group_focus_obj(a);
        voice_sel_to(a, true);
    } else {
        voice_show_obj(s_sel, false);
    }
}

static void voice_label_style(lv_obj_t *label, const lv_font_t *font, uint32_t color,
                              lv_text_align_t align)
{
    lv_obj_set_style_text_font(label, font, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, lv_color_hex(color), LV_PART_MAIN);
    lv_obj_set_style_text_align(label, align, LV_PART_MAIN);
}

static void voice_chip_update(void)
{
    bool on = is_wifi_connected();
    if (s_chip == NULL) {
        return;
    }
    lv_obj_set_style_bg_color(s_chip, lv_color_hex(on ? VOICE_TINT : VOICE_WARN_TINT), LV_PART_MAIN);
    lv_obj_set_style_text_color(s_chip_icon, lv_color_hex(on ? VOICE_NAVY : VOICE_WARN), LV_PART_MAIN);
    lv_obj_set_style_text_color(s_chip_text, lv_color_hex(on ? VOICE_NAVY : VOICE_WARN), LV_PART_MAIN);
    lv_label_set_text(s_chip_text, on ? ui_tr("On", "已连接") : ui_tr("Off", "未连接"));
}

static void voice_orb_icon(const char *symbol, bool solid)
{
    if (s_orb == NULL) {
        return;
    }
    lv_label_set_text(s_orb_icon, symbol);
    lv_obj_set_style_bg_color(s_orb, lv_color_hex(solid ? VOICE_NAVY : VOICE_TINT), LV_PART_MAIN);
    lv_obj_set_style_text_color(s_orb_icon, lv_color_hex(solid ? 0xffffff : VOICE_NAVY), LV_PART_MAIN);
}

static void voice_hint_at(const char *text, bool left)
{
    lv_label_set_text(s_hint, text);
    lv_obj_set_pos(s_hint, left ? 18 : 164, BTN_Y + 6);
}

static void voice_apply(voice_state_t state)
{
    bool center = state != VS_RESULT;
    voice_state_t prev = s_state;

    s_state = state;
    s_click_armed = false;
    s_pending_act = 0;
    if (s_title == NULL || s_scr == NULL || !lv_obj_is_valid(s_scr)) {
        return;
    }

    voice_show_obj(s_btn_add, false);
    voice_show_obj(s_btn_drop, false);
    voice_show_obj(s_btn_main, false);
    voice_show_obj(s_ticket, !center);
    voice_show_obj(s_orb, center);
    voice_show_obj(s_body, center);
    voice_show_obj(s_hint, true);
    voice_show_obj(s_record_bar, state == VS_LISTEN);
    voice_show_obj(s_record_time, state == VS_LISTEN);
    voice_chip_update();
    lv_obj_set_y(s_body, 46);

    switch (state) {
    case VS_WIFI:
        lv_label_set_text(s_title, ui_tr("WiFi first", "先连 WiFi"));
        lv_label_set_text(s_body, ui_tr(s_inspiration ? "Connect to record\nRead your ideas offline" : "Connect, then I can\nturn speech into a todo",
                                        s_inspiration ? "联网后才能识别\n回看灵感不需要联网" : "联网后才能把\n说的话变成待办"));
        voice_orb_icon(LV_SYMBOL_WIFI, false);
        lv_label_set_text(s_btn_main_label, ui_tr("Connect", "去连接"));
        voice_show_obj(s_btn_main, true);
        voice_hint_at(ui_tr("2 taps: back", "双击返回"), false);
        voice_group_set(s_btn_main, NULL);
        break;
    case VS_READY:
        lv_label_set_text(s_title, s_inspiration ? ui_tr("Inspiration", "灵感闪记") : ui_tr("Voice todo", "语音待办"));
        lv_label_set_text(s_body, ui_tr(s_inspiration ? "Say your thought\nI'll keep it for you" : "Say one thing to do\nI'll write it down",
                                        s_inspiration ? "说出突然冒出的想法\n我帮你整理并留住" : "说一句想做的事\n我帮你记下来"));
        voice_orb_icon(LV_SYMBOL_AUDIO, true);
        lv_label_set_text(s_btn_main_label, ui_tr("Speak", "开始说"));
        voice_show_obj(s_btn_main, true);
        voice_hint_at(ui_tr("2 taps: back", "双击返回"), false);
        voice_group_set(s_btn_main, NULL);
        break;
    case VS_LISTEN:
        lv_label_set_text(s_title, ui_tr("Listening", "正在听"));
        lv_label_set_text(s_body, ui_tr("Speak after the beep", "听到提示音后说话"));
        lv_obj_set_y(s_body, 40);
        voice_orb_icon(LV_SYMBOL_AUDIO, true);
        voice_hint_at(ui_tr("Pause when you finish", "说完停一下就好"), true);
        voice_group_set(NULL, NULL);
        break;
    case VS_THINK:
        lv_label_set_text(s_title, ui_tr("Working", "整理中"));
        lv_label_set_text(s_body, s_inspiration ? ui_tr("Keeping your meaning", "保留原意\n让想法更清楚") : ui_tr("Making it one line", "正在收成一句"));
        lv_obj_set_y(s_body, 40);
        voice_orb_icon(LV_SYMBOL_AUDIO, false);
        voice_hint_at(ui_tr("One moment", "稍等一下"), true);
        voice_group_set(NULL, NULL);
        break;
    case VS_RESULT:
        lv_label_set_text(s_title, ui_tr("Add it?", "加入待办？"));
        if (s_btn_add != NULL) {
            lv_label_set_text(lv_obj_get_child(s_btn_add, 0), ui_tr("Add", "加入"));
        }
        if (s_btn_drop != NULL) {
            lv_label_set_text(lv_obj_get_child(s_btn_drop, 0), ui_tr("Skip", "放弃"));
        }
        lv_label_set_text(s_ticket_text, s_shown);
        voice_show_obj(s_hint, false);
        voice_show_obj(s_btn_add, true);
        voice_show_obj(s_btn_drop, true);
        s_sel_placed = false;
        voice_group_set(s_btn_add, s_btn_drop);
        break;
    case VS_FAIL:
        lv_label_set_text(s_title, ui_tr("Try again", "再试一次"));
        lv_label_set_text(s_body, s_shown[0] ? s_shown : ui_tr("Didn't catch that", "没听清"));
        voice_orb_icon(LV_SYMBOL_REFRESH, false);
        lv_label_set_text(s_btn_main_label, ui_tr("Again", "重新说"));
        voice_show_obj(s_btn_main, true);
        voice_hint_at(ui_tr("2 taps: back", "双击返回"), false);
        voice_group_set(s_btn_main, NULL);
        break;
    case VS_SAVED:
        lv_label_set_text(s_title, ui_tr("Added", "已加入"));
        lv_label_set_text(s_body, s_shown);
        voice_orb_icon(LV_SYMBOL_OK, true);
        voice_hint_at("", true);
        voice_group_set(NULL, NULL);
        break;
    }
    voice_fx_set(state);

    /* 状态切换时内容分层浮现；首次入场由 voice_show 统一编排。 */
    if (prev != state && s_card != NULL && lv_obj_get_style_opa(s_card, LV_PART_MAIN) == LV_OPA_COVER) {
        fx_slide(s_title, 0, -14);
        if (center) {
            fx_rise(s_body, 60, 8);
            fx_rise(s_wave, 100, 8);
        } else {
            fx_slide(s_ticket, 40, 36);
        }
        fx_rise(s_btn_main, 120, 12);
        fx_rise(s_btn_add, 120, 14);
        fx_rise(s_btn_drop, 190, 14);
        fx_rise(s_sel, 120, 14);
        fx_rise(s_hint, 160, 6);
    }
}

static void voice_show_idle(void)
{
    s_wifi_seen = is_wifi_connected();
    voice_apply(s_wifi_seen ? VS_READY : VS_WIFI);
}

static void voice_mark_deleted(lv_obj_t *scr)
{
    if (scr == NULL) {
        return;
    }
    if (scr == guider_ui.screen_reppo) guider_ui.screen_reppo_del = true;
    else if (scr == guider_ui.screen_main) guider_ui.screen_main_del = true;
    else if (scr == guider_ui.screen_todolist) guider_ui.screen_todolist_del = true;
    else if (scr == guider_ui.screen_todosel) guider_ui.screen_todosel_del = true;
    else if (scr == guider_ui.screen_todoback) guider_ui.screen_todoback_del = true;
    else if (scr == guider_ui.screen_timing) guider_ui.screen_timing_del = true;
    else if (scr == guider_ui.screen_pomodoro) guider_ui.screen_pomodoro_del = true;
    else if (scr == guider_ui.screen_setting) guider_ui.screen_setting_del = true;
    else if (scr == guider_ui.screen_wificfg) guider_ui.screen_wificfg_del = true;
    else if (scr == guider_ui.screen_donelist) guider_ui.screen_donelist_del = true;
    else if (scr == guider_ui.screen_answer) guider_ui.screen_answer_del = true;
    else if (scr == guider_ui.screen_loading) guider_ui.screen_loading_del = true;
    else if (scr == guider_ui.screen_getanswer) guider_ui.screen_getanswer_del = true;
    else if (scr == guider_ui.screen_picture) guider_ui.screen_picture_del = true;
    else if (scr == guider_ui.screen_gash) guider_ui.screen_gash_del = true;
    else if (scr == guider_ui.screen_game) guider_ui.screen_game_del = true;
    else if (scr == guider_ui.screen_keep) guider_ui.screen_keep_del = true;
    else if (scr == guider_ui.screen_book) guider_ui.screen_book_del = true;
}

static void voice_on_delete(lv_event_t *e)
{
    int i;
    if (lv_event_get_target(e) != s_scr) {
        return;
    }
    voice_fx_stop();
    if (s_click_timer != NULL) {
        lv_timer_del(s_click_timer);
        s_click_timer = NULL;
    }
    if (s_saved_timer != NULL) {
        lv_timer_del(s_saved_timer);
        s_saved_timer = NULL;
    }
    s_scr = NULL;
    s_card = NULL;
    s_title = NULL;
    s_chip = NULL;
    s_chip_icon = NULL;
    s_chip_text = NULL;
    s_orb = NULL;
    s_orb_icon = NULL;
    s_ring[0] = s_ring[1] = NULL;
    s_spin = NULL;
    s_body = NULL;
    s_wave = NULL;
    s_record_bar = s_record_time = NULL;
    for (i = 0; i < VOICE_BARS; i++) {
        s_bar[i] = NULL;
    }
    s_ticket = NULL;
    s_ticket_text = NULL;
    s_hint = NULL;
    s_sel = NULL;
    s_btn_main = NULL;
    s_btn_main_label = NULL;
    s_btn_add = NULL;
    s_btn_drop = NULL;
    s_open = false;
    s_click_armed = false;
    s_pending_act = 0;
    s_sel_placed = false;
}

static lv_obj_t *voice_plain(lv_obj_t *parent)
{
    lv_obj_t *o = lv_obj_create(parent);
    lv_obj_remove_style_all(o);
    lv_obj_clear_flag(o, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    return o;
}

static lv_obj_t *voice_make_btn(lv_obj_t *parent, const char *text, lv_obj_t **label_out,
                                lv_coord_t x, lv_coord_t w)
{
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, text);
    lv_obj_center(label);
    lv_obj_set_pos(btn, x, BTN_Y);
    lv_obj_set_size(btn, w, BTN_H);
    voice_style_btn(btn);
    if (label_out != NULL) {
        *label_out = label;
    }
    return btn;
}

static void voice_btn_event(lv_event_t *e);

static void voice_build(void)
{
    int i;

    s_scr = lv_obj_create(NULL);
    lv_obj_set_size(s_scr, 320, 170);
    lv_obj_set_scrollbar_mode(s_scr, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(s_scr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(s_scr, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(s_scr, lv_color_hex(VOICE_TINT), LV_PART_MAIN);
    lv_obj_add_event_cb(s_scr, voice_on_delete, LV_EVENT_DELETE, NULL);

    s_card = voice_plain(s_scr);
    lv_obj_set_size(s_card, 296, 150);
    lv_obj_set_pos(s_card, 12, 10);
    lv_obj_set_style_radius(s_card, 16, LV_PART_MAIN);
    lv_obj_set_style_bg_color(s_card, lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(s_card, 228, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(s_card, 18, LV_PART_MAIN);
    lv_obj_set_style_shadow_ofs_y(s_card, 4, LV_PART_MAIN);
    lv_obj_set_style_shadow_color(s_card, lv_color_hex(VOICE_NAVY), LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(s_card, 45, LV_PART_MAIN);

    s_title = lv_label_create(s_card);
    voice_label_style(s_title, s_inspiration ? ui_font_16() : &voice_todo_title_font, VOICE_NAVY, LV_TEXT_ALIGN_LEFT);
    lv_obj_set_pos(s_title, 18, 10);

    s_chip = voice_plain(s_card);
    lv_obj_set_size(s_chip, LV_SIZE_CONTENT, 22);
    lv_obj_set_style_radius(s_chip, 11, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(s_chip, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_pad_hor(s_chip, 9, LV_PART_MAIN);
    lv_obj_set_style_pad_column(s_chip, 5, LV_PART_MAIN);
    lv_obj_set_flex_flow(s_chip, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(s_chip, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_align(s_chip, LV_ALIGN_TOP_RIGHT, -14, 13);
    s_chip_icon = lv_label_create(s_chip);
    lv_obj_set_style_text_font(s_chip_icon, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_label_set_text(s_chip_icon, LV_SYMBOL_WIFI);
    s_chip_text = lv_label_create(s_chip);
    lv_obj_set_style_text_font(s_chip_text, &voice_todo_font, LV_PART_MAIN);

    for (i = 0; i < 2; i++) {
        s_ring[i] = voice_plain(s_card);
        lv_obj_set_size(s_ring[i], ORB_SIZE, ORB_SIZE);
        lv_obj_align(s_ring[i], LV_ALIGN_CENTER, ORB_DX, ORB_DY);
        lv_obj_set_style_radius(s_ring[i], LV_RADIUS_CIRCLE, LV_PART_MAIN);
        lv_obj_set_style_border_width(s_ring[i], 2, LV_PART_MAIN);
        lv_obj_set_style_border_color(s_ring[i], lv_color_hex(VOICE_NAVY), LV_PART_MAIN);
        lv_obj_add_flag(s_ring[i], LV_OBJ_FLAG_HIDDEN);
    }

    s_spin = lv_spinner_create(s_card, 900, 70);
    lv_obj_set_size(s_spin, ORB_SIZE + 14, ORB_SIZE + 14);
    lv_obj_align(s_spin, LV_ALIGN_CENTER, ORB_DX, ORB_DY);
    lv_obj_clear_flag(s_spin, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_width(s_spin, 3, LV_PART_MAIN);
    lv_obj_set_style_arc_color(s_spin, lv_color_hex(VOICE_NAVY), LV_PART_MAIN);
    lv_obj_set_style_arc_opa(s_spin, 40, LV_PART_MAIN);
    lv_obj_set_style_arc_width(s_spin, 3, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(s_spin, lv_color_hex(VOICE_NAVY), LV_PART_INDICATOR);
    lv_obj_add_flag(s_spin, LV_OBJ_FLAG_HIDDEN);

    s_orb = voice_plain(s_card);
    lv_obj_set_size(s_orb, ORB_SIZE, ORB_SIZE);
    lv_obj_align(s_orb, LV_ALIGN_CENTER, ORB_DX, ORB_DY);
    lv_obj_set_style_radius(s_orb, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(s_orb, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(s_orb, 16, LV_PART_MAIN);
    lv_obj_set_style_shadow_color(s_orb, lv_color_hex(VOICE_NAVY), LV_PART_MAIN);
    s_orb_icon = lv_label_create(s_orb);
    lv_obj_set_style_text_font(s_orb_icon, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_center(s_orb_icon);

    s_body = lv_label_create(s_card);
    lv_obj_set_width(s_body, 186);
    lv_label_set_long_mode(s_body, LV_LABEL_LONG_WRAP);
    voice_label_style(s_body, ui_font_14(), VOICE_INK, LV_TEXT_ALIGN_LEFT);
    lv_obj_set_style_text_line_space(s_body, 3, LV_PART_MAIN);
    lv_obj_set_pos(s_body, 96, 46);

    s_wave = voice_plain(s_card);
    lv_obj_set_size(s_wave, 120, 32);
    lv_obj_set_pos(s_wave, 96, 64);
    lv_obj_set_flex_flow(s_wave, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(s_wave, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(s_wave, 6, LV_PART_MAIN);
    for (i = 0; i < VOICE_BARS; i++) {
        s_bar[i] = voice_plain(s_wave);
        lv_obj_set_size(s_bar[i], 5, 6);
        lv_obj_set_style_radius(s_bar[i], 3, LV_PART_MAIN);
        lv_obj_set_style_bg_color(s_bar[i], lv_color_hex(VOICE_NAVY), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(s_bar[i], (lv_opa_t)(150 + i * 15), LV_PART_MAIN);
    }
    lv_obj_add_flag(s_wave, LV_OBJ_FLAG_HIDDEN);
    s_record_bar = lv_bar_create(s_card);
    lv_obj_set_pos(s_record_bar, 96, 96);
    lv_obj_set_size(s_record_bar, 174, 3);
    lv_bar_set_range(s_record_bar, 0, VOICE_MAX_MS);
    lv_obj_set_style_bg_color(s_record_bar, lv_color_hex(0xDCE7ED), LV_PART_MAIN);
    lv_obj_set_style_bg_color(s_record_bar, lv_color_hex(s_inspiration ? 0xC49A57 : 0x5E9C92), LV_PART_INDICATOR);
    s_record_time = lv_label_create(s_card);
    lv_obj_set_pos(s_record_time, 96, 101);
    lv_obj_set_style_text_font(s_record_time, ui_font_12(), LV_PART_MAIN);
    lv_obj_set_style_text_color(s_record_time, lv_color_hex(VOICE_MUTED), LV_PART_MAIN);


    s_ticket = voice_plain(s_card);
    lv_obj_set_size(s_ticket, 260, 54);
    lv_obj_set_pos(s_ticket, 18, 44);
    lv_obj_set_style_radius(s_ticket, 10, LV_PART_MAIN);
    lv_obj_set_style_bg_color(s_ticket, lv_color_hex(VOICE_TINT), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(s_ticket, LV_OPA_COVER, LV_PART_MAIN);
    {
        lv_obj_t *accent = voice_plain(s_ticket);
        lv_obj_set_size(accent, 4, 34);
        lv_obj_align(accent, LV_ALIGN_LEFT_MID, 10, 0);
        lv_obj_set_style_radius(accent, 2, LV_PART_MAIN);
        lv_obj_set_style_bg_color(accent, lv_color_hex(VOICE_NAVY), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(accent, LV_OPA_COVER, LV_PART_MAIN);
    }
    s_ticket_text = lv_label_create(s_ticket);
    lv_obj_set_width(s_ticket_text, 226);
    lv_label_set_long_mode(s_ticket_text, LV_LABEL_LONG_WRAP);
    voice_label_style(s_ticket_text, ui_font_14(), VOICE_NAVY, LV_TEXT_ALIGN_LEFT);
    lv_obj_align(s_ticket_text, LV_ALIGN_LEFT_MID, 24, 0);
    lv_obj_add_flag(s_ticket, LV_OBJ_FLAG_HIDDEN);

    s_hint = lv_label_create(s_card);
    voice_label_style(s_hint, &voice_todo_font, VOICE_MUTED, LV_TEXT_ALIGN_LEFT);

    s_sel = voice_plain(s_card);
    lv_obj_set_size(s_sel, 132, BTN_H);
    lv_obj_set_pos(s_sel, 18, BTN_Y);
    lv_obj_set_style_radius(s_sel, BTN_H / 2, LV_PART_MAIN);
    lv_obj_set_style_bg_color(s_sel, lv_color_hex(VOICE_NAVY), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(s_sel, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(s_sel, 14, LV_PART_MAIN);
    lv_obj_set_style_shadow_ofs_y(s_sel, 3, LV_PART_MAIN);
    lv_obj_set_style_shadow_color(s_sel, lv_color_hex(VOICE_NAVY), LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(s_sel, 110, LV_PART_MAIN);
    lv_obj_add_flag(s_sel, LV_OBJ_FLAG_HIDDEN);

    s_btn_main = voice_make_btn(s_card, "开始说", &s_btn_main_label, 18, 132);
    s_btn_add = voice_make_btn(s_card, "加入", NULL, 18, 124);
    s_btn_drop = voice_make_btn(s_card, "放弃", NULL, 154, 124);
    lv_obj_add_event_cb(s_btn_main, voice_btn_event, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(s_btn_add, voice_btn_event, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(s_btn_drop, voice_btn_event, LV_EVENT_CLICKED, NULL);
}

/** 入场：卡片带回弹升起，标题从左滑入，麦克风弹出，正文和按钮依次浮现。 */
static void voice_intro(void)
{
    fx_run(s_card, fx_opa, LV_OPA_TRANSP, LV_OPA_COVER, 260, 0, lv_anim_path_ease_out, 0, 0);
    fx_run(s_card, fx_y, 46, 10, 460, 0, lv_anim_path_overshoot, 0, 0);
    fx_slide(s_title, 180, -20);
    fx_rise(s_chip, 240, 6);
    if (!lv_obj_has_flag(s_orb, LV_OBJ_FLAG_HIDDEN)) {
        lv_anim_del(s_orb, fx_glow);
        fx_run(s_orb, fx_size, 8, ORB_SIZE, 460, 220, lv_anim_path_overshoot, 0, 0);
        if (s_state == VS_READY) {
            fx_run(s_orb, fx_glow, 0, 6, 900, 700, lv_anim_path_ease_in_out, 900,
                   LV_ANIM_REPEAT_INFINITE);
        }
    }
    fx_rise(s_body, 300, 10);
    fx_rise(s_sel, 380, 16);
    fx_rise(s_btn_main, 380, 16);
    fx_rise(s_hint, 460, 8);
}

static void voice_show(void)
{
    lv_obj_t *old = lv_scr_act();

    if (s_open && s_scr != NULL && lv_obj_is_valid(s_scr)) {
        return;
    }
    s_open = true;
    s_shown[0] = '\0';
    s_sel_placed = false;
    s_state = VS_WIFI;
    voice_build();

    if (s_click_timer == NULL) {
        s_click_timer = lv_timer_create(voice_click_timer_cb, VOICE_CLICK_MS, NULL);
    }
    /* 先把卡片设为透明，voice_apply 就不会叠一层状态切换动画。 */
    lv_obj_set_style_opa(s_card, LV_OPA_TRANSP, LV_PART_MAIN);
    voice_show_idle();
    voice_intro();
    voice_mark_deleted(old);
    lv_scr_load_anim(s_scr, LV_SCR_LOAD_ANIM_FADE_ON, 160, 0, true);
}

void voice_todo_return_from_wifi(void)
{
    voice_show();
    s_wificfg_return = false;
}

static void voice_go_main(void)
{
    s_wificfg_return = false;
    if (guider_ui.screen_main == NULL || !lv_obj_is_valid(guider_ui.screen_main)) {
        guider_ui.screen_main_del = true;
    }
    ui_load_scr_animation(&guider_ui, &guider_ui.screen_main, guider_ui.screen_main_del,
                          &s_voice_del_flag, setup_scr_screen_main,
                          LV_SCR_LOAD_ANIM_FADE_ON, 160, 0, false, true);
}

static void voice_open_wifi(void)
{
    s_wificfg_return = true;
    if (guider_ui.screen_wificfg == NULL || !lv_obj_is_valid(guider_ui.screen_wificfg)) {
        guider_ui.screen_wificfg_del = true;
    }
    ui_load_scr_animation(&guider_ui, &guider_ui.screen_wificfg, guider_ui.screen_wificfg_del,
                          &s_voice_del_flag, setup_scr_screen_wificfg,
                          LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, false, true);
}

static void voice_click_timer_cb(lv_timer_t *timer)
{
    int act;
    (void)timer;
    if (!s_click_armed || !s_open) {
        s_click_armed = false;
        return;
    }
    s_click_armed = false;
    act = s_pending_act;
    s_pending_act = 0;
    if (act == 1) {
        voice_start_listen();
    } else if (act == 2) {
        voice_open_wifi();
    } else if (act == 3) {
        voice_show_idle();
    }
}

/** 第一下先记住。窗口内再按一下是双击，回主页；否则这一下生效。 */
static void voice_arm_click(int act)
{
    uint32_t now = lv_tick_get();
    if (s_click_armed && (uint32_t)(now - s_click_tick) < VOICE_CLICK_MS) {
        s_click_armed = false;
        s_pending_act = 0;
        voice_go_main();
        return;
    }
    s_click_armed = true;
    s_click_tick = now;
    s_pending_act = act;
    if (s_click_timer != NULL) {
        lv_timer_reset(s_click_timer);
    }
    /* 按下即有反馈：滑块轻轻一沉。 */
    if (s_sel != NULL) {
        fx_run(s_sel, fx_ty, 0, 2, 90, 0, lv_anim_path_ease_out, 90, 0);
    }
}

static void voice_recording_feedback(void)
{
    if (lv_tick_elaps(s_feedback_at) < 50) return;
    s_feedback_at = lv_tick_get();
    uint32_t level = s_audio_level;
    static const uint8_t weights[VOICE_BARS] = {4, 6, 9, 10, 8, 5, 3};
    for (int i = 0; i < VOICE_BARS; ++i) {
        unsigned h = 5 + level * weights[i] / 400;
        if (h > 30) h = 30;
        lv_obj_set_height(s_bar[i], h);
    }
    lv_bar_set_value(s_record_bar, s_audio_ms, LV_ANIM_OFF);
    uint32_t seconds = s_audio_ms / 1000;
    if (seconds != s_feedback_seconds) {
        s_feedback_seconds = seconds;
        char progress[24];
        snprintf(progress, sizeof(progress), "%02u / %02u s", (unsigned)seconds,
                 (unsigned)((s_inspiration ? VOICE_MAX_MS : VOICE_TODO_MS) / 1000));
        lv_label_set_text(s_record_time, progress);
    }
}

static void voice_write_date(char *dst, size_t cap, unsigned year, unsigned month, unsigned day)
{
    if (cap < 11 || year < 2024 || year > 9999 || month < 1 || month > 12 || day < 1 || day > 31) {
        return;
    }
    snprintf(dst, cap, "%04u-%02u-%02u", year % 10000u, month % 100u, day % 100u);
}

static void voice_today(char *dst, size_t cap)
{
    rx8130_time_t now;
    time_t sec;
    struct tm tm;

    dst[0] = '\0';
    if (rx8130_get_time(I2C_NUM_0, &now) == ESP_OK) {
        voice_write_date(dst, cap, now.year, now.month, now.date);
        if (dst[0] != '\0') {
            return;
        }
    }
    sec = time(NULL);
    localtime_r(&sec, &tm);
    if (tm.tm_year + 1900 > 0) {
        voice_write_date(dst, cap,
                         (unsigned)(tm.tm_year + 1900),
                         (unsigned)(tm.tm_mon + 1),
                         (unsigned)tm.tm_mday);
    }
}

static bool voice_save_todo(const char *name)
{
    uint8_t idx;
    uint8_t i;

    if (name == NULL || name[0] == '\0') {
        return false;
    }
    if (ui_todo_list.count >= TODO_MAX_ITEMS) {
        return false;
    }
    idx = ui_todo_list.count;
    memset(&ui_todo_list.items[idx], 0, sizeof(ui_todo_list.items[idx]));
    strncpy(ui_todo_list.items[idx].name, name, TODO_NAME_LEN - 1);
    todo_storage_sanitize_task_name(ui_todo_list.items[idx].name, TODO_NAME_LEN);
    if (ui_todo_list.items[idx].name[0] == '\0') {
        return false;
    }
    /* 没说到时长就留空。说到了才写入。日期用当前 RTC 的今天。 */
    ui_todo_list.items[idx].duration_min = s_minutes;
    voice_today(ui_todo_list.items[idx].planned_date,
                sizeof(ui_todo_list.items[idx].planned_date));
    ui_todo_list.items[idx].done = 0;
    ui_todo_list.items[idx].label = TODO_LABEL_NONE;
    ui_todo_list.count++;
    done_sum = 0;
    for (i = 0; i < ui_todo_list.count; i++) {
        if (ui_todo_list.items[i].done) {
            done_sum++;
        }
    }
    todo_sum = (int8_t)(ui_todo_list.count - done_sum);
    if (!todo_storage_lock(pdMS_TO_TICKS(500))) {
        return true;
    }
    g_todo_list = ui_todo_list;
    (void)todo_storage_flush_locked();
    todo_storage_unlock();
    return true;
}

static void voice_saved_timer_cb(lv_timer_t *timer)
{
    (void)timer;
    s_saved_timer = NULL;
    voice_go_main();
}

static void voice_commit_add(void)
{
    if (!voice_save_todo(s_shown)) {
        strncpy(s_shown, ui_tr("List is full", "待办已满"), sizeof(s_shown) - 1);
        s_shown[sizeof(s_shown) - 1] = '\0';
        voice_apply(VS_FAIL);
        return;
    }
    buzzer_beep_confirm();
    voice_apply(VS_SAVED);
    if (s_saved_timer == NULL) {
        s_saved_timer = lv_timer_create(voice_saved_timer_cb, 1100, NULL);
        lv_timer_set_repeat_count(s_saved_timer, 1);
    }
}

static void voice_btn_event(lv_event_t *e)
{
    lv_obj_t *target;

    if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }
    target = lv_event_get_target(e);
    if (s_state == VS_LISTEN || s_state == VS_THINK || s_state == VS_SAVED) {
        return;
    }
    if (s_state == VS_RESULT) {
        if (target == s_btn_add) {
            voice_commit_add();
        } else if (target == s_btn_drop) {
            voice_go_main();
        }
        return;
    }
    if (s_state == VS_WIFI && target == s_btn_main) {
        voice_arm_click(2);
        return;
    }
    if (s_state == VS_FAIL && target == s_btn_main) {
        voice_arm_click(is_wifi_connected() ? 1 : 3);
        return;
    }
    if (s_state == VS_READY && target == s_btn_main) {
        voice_arm_click(1);
    }
}

static const cJSON *voice_find_string(const cJSON *node, const char *key)
{
    if (!node) return NULL;
    if (cJSON_IsObject(node)) {
        const cJSON *value = cJSON_GetObjectItemCaseSensitive(node, key);
        if (cJSON_IsString(value)) return value;
    }
    const cJSON *child;
    cJSON_ArrayForEach(child, node) {
        const cJSON *value = voice_find_string(child, key);
        if (value) return value;
    }
    return NULL;
}

static bool voice_json_pick(const char *json, const char *key, char *out, size_t cap)
{
    cJSON *root = cJSON_Parse(json);
    const cJSON *value = voice_find_string(root, key);
    bool ok = value && value->valuestring && value->valuestring[0];
    if (ok) inspiration_text_copy(out, cap, value->valuestring);
    cJSON_Delete(root);
    return ok;
}

static void voice_trim_copy(char *dst, size_t cap, const char *src, int max_chars)
{
    const char *s = src;
    size_t n = 0;
    int chars = 0;

    while (*s != '\0' && isspace((unsigned char)*s)) {
        s++;
    }
    while (*s == '"' || *s == '\'') {
        s++;
    }
    while (*s != '\0' && chars < max_chars && n + 1 < cap) {
        unsigned char c = (unsigned char)*s;
        size_t step = 1;
        if ((c & 0x80) == 0) step = 1;
        else if ((c & 0xE0) == 0xC0) step = 2;
        else if ((c & 0xF0) == 0xE0) step = 3;
        else if ((c & 0xF8) == 0xF0) step = 4;
        if (s[step - 1] == '\0') {
            break;
        }
        if (n + step >= cap) {
            break;
        }
        memcpy(dst + n, s, step);
        n += step;
        s += step;
        chars++;
    }
    while (n > 0 && isspace((unsigned char)dst[n - 1])) {
        n--;
    }
    dst[n] = '\0';
}

static void voice_put_le32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v);
    p[1] = (uint8_t)(v >> 8);
    p[2] = (uint8_t)(v >> 16);
    p[3] = (uint8_t)(v >> 24);
}

static void voice_wav_header(uint8_t *h, uint32_t pcm_bytes)
{
    memcpy(h, "RIFF", 4);
    voice_put_le32(h + 4, 36 + pcm_bytes);
    memcpy(h + 8, "WAVEfmt ", 8);
    voice_put_le32(h + 16, 16);
    h[20] = 1;
    h[21] = 0;
    h[22] = 1;
    h[23] = 0;
    voice_put_le32(h + 24, VOICE_RATE);
    voice_put_le32(h + 28, VOICE_RATE * 2);
    h[32] = 2;
    h[33] = 0;
    h[34] = 16;
    h[35] = 0;
    memcpy(h + 36, "data", 4);
    voice_put_le32(h + 40, pcm_bytes);
}

static size_t voice_b64(const uint8_t *in, size_t in_len, char *out)
{
    static const char tab[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t i = 0;
    size_t o = 0;
    while (i < in_len) {
        uint32_t n = ((uint32_t)in[i]) << 16;
        int rem = (int)(in_len - i);
        if (rem > 1) n |= ((uint32_t)in[i + 1]) << 8;
        if (rem > 2) n |= in[i + 2];
        out[o++] = tab[(n >> 18) & 63];
        out[o++] = tab[(n >> 12) & 63];
        out[o++] = rem > 1 ? tab[(n >> 6) & 63] : '=';
        out[o++] = rem > 2 ? tab[n & 63] : '=';
        i += 3;
    }
    out[o] = '\0';
    return o;
}

static bool voice_http_post(const char *url, const char *body, size_t body_len,
                            char *resp, size_t resp_max)
{
    char auth[220];
    esp_http_client_config_t cfg = {
        .url = url,
        .method = HTTP_METHOD_POST,
        .timeout_ms = VOICE_HTTP_MS,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .buffer_size = 4096,
        .buffer_size_tx = 1024,
    };
    esp_http_client_handle_t client;
    int status;
    int n;
    size_t total = 0;
    size_t sent = 0;

    snprintf(auth, sizeof(auth), "Bearer %s", DASHSCOPE_KEY);
    client = esp_http_client_init(&cfg);
    if (client == NULL) {
        return false;
    }
    esp_http_client_set_header(client, "Authorization", auth);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_header(client, "X-DashScope-SSE", "disable");
    if (esp_http_client_open(client, (int)body_len) != ESP_OK) {
        esp_http_client_cleanup(client);
        return false;
    }
    while (sent < body_len) {
        int w = esp_http_client_write(client, body + sent, (int)(body_len - sent));
        if (w <= 0) {
            esp_http_client_close(client);
            esp_http_client_cleanup(client);
            return false;
        }
        sent += (size_t)w;
    }
    if (esp_http_client_fetch_headers(client) < 0) {
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        return false;
    }
    status = esp_http_client_get_status_code(client);
    while (total + 1 < resp_max &&
           (n = esp_http_client_read(client, resp + total, (int)(resp_max - 1 - total))) > 0) {
        total += (size_t)n;
    }
    resp[total] = '\0';
    bool complete = esp_http_client_is_complete_data_received(client);
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
    if (status != 200) {
        ESP_LOGW(TAG, "HTTP %d", status);
        return false;
    }
    return complete;
}

bool voice_ai_complete(const char *system, const char *data, char *out, size_t capacity)
{
    bool ok = false;
    cJSON *root = cJSON_CreateObject();
    cJSON *messages = root ? cJSON_AddArrayToObject(root, "messages") : NULL;
    char *body = NULL, *response = NULL;
    if (!out || !capacity || !system || !data || !messages) goto done;
    out[0] = '\0';
    if (!cJSON_AddStringToObject(root, "model", REFINE_MODEL) ||
        !cJSON_AddNumberToObject(root, "max_tokens", 400)) goto done;
    const char *roles[] = {"system", "user"};
    const char *texts[] = {system, data};
    for (unsigned i = 0; i < 2; ++i) {
        cJSON *message = cJSON_CreateObject();
        if (!message) goto done;
        cJSON_AddItemToArray(messages, message);
        if (!cJSON_AddStringToObject(message, "role", roles[i]) ||
            !cJSON_AddStringToObject(message, "content", texts[i])) goto done;
    }
    body = cJSON_PrintUnformatted(root);
    response = heap_caps_malloc(VOICE_RESP_MAX, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!body || !response || !voice_http_post(REFINE_URL, body, strlen(body), response, VOICE_RESP_MAX)) goto done;
    cJSON *reply = cJSON_Parse(response);
    const cJSON *choice = cJSON_GetArrayItem(cJSON_GetObjectItemCaseSensitive(reply, "choices"), 0);
    const cJSON *content = cJSON_GetObjectItemCaseSensitive(
        cJSON_GetObjectItemCaseSensitive(choice, "message"), "content");
    if (cJSON_IsString(content) && content->valuestring[0] && strlen(content->valuestring) < capacity) {
        memcpy(out, content->valuestring, strlen(content->valuestring) + 1);
        ok = true;
    }
    cJSON_Delete(reply);
done:
    cJSON_Delete(root); cJSON_free(body); free(response);
    return ok;
}

static void voice_json_escape(const char *src, char *dst, size_t cap)
{
    size_t n = 0;
    while (*src != '\0' && n + 2 < cap) {
        unsigned char c = (unsigned char)*src;
        if (c < 0x20) {
            src++;
            continue;
        }
        if (*src == '"' || *src == '\\') {
            dst[n++] = '\\';
        }
        dst[n++] = *src++;
    }
    dst[n] = '\0';
}

static uint32_t voice_utf8_next(const char **ps)
{
    const unsigned char *s = (const unsigned char *)*ps;
    uint32_t cp;

    if (*s == 0) {
        return 0;
    }
    if (*s < 0x80) {
        *ps += 1;
        return *s;
    }
    if ((*s & 0xE0) == 0xC0 && s[1] != 0) {
        cp = ((uint32_t)(s[0] & 0x1F) << 6) | (s[1] & 0x3F);
        *ps += 2;
        return cp;
    }
    if ((*s & 0xF0) == 0xE0 && s[1] != 0 && s[2] != 0) {
        cp = ((uint32_t)(s[0] & 0x0F) << 12) |
             ((uint32_t)(s[1] & 0x3F) << 6) | (s[2] & 0x3F);
        *ps += 3;
        return cp;
    }
    if ((*s & 0xF8) == 0xF0 && s[1] != 0 && s[2] != 0 && s[3] != 0) {
        cp = ((uint32_t)(s[0] & 0x07) << 18) |
             ((uint32_t)(s[1] & 0x3F) << 12) |
             ((uint32_t)(s[2] & 0x3F) << 6) | (s[3] & 0x3F);
        *ps += 4;
        return cp;
    }
    *ps += 1;
    return *s;
}

static int voice_cn_digit(uint32_t cp)
{
    switch (cp) {
    case 0x96F6: return 0;
    case 0x4E00: return 1;
    case 0x4E8C: case 0x4E24: return 2;
    case 0x4E09: return 3;
    case 0x56DB: return 4;
    case 0x4E94: return 5;
    case 0x516D: return 6;
    case 0x4E03: return 7;
    case 0x516B: return 8;
    case 0x4E5D: return 9;
    default: return -1;
    }
}

static bool voice_parse_cn(const char **ps, int *out)
{
    const char *s = *ps;
    int val = 0;
    int cur = -1;
    bool any = false;

    for (;;) {
        const char *save = s;
        uint32_t cp = voice_utf8_next(&s);
        int d;

        if (cp == 0) {
            s = save;
            break;
        }
        if (cp == 0x5341) {
            val += (cur < 0 ? 1 : cur) * 10;
            cur = -1;
            any = true;
            continue;
        }
        if (cp == 0x767E) {
            val += (cur < 0 ? 1 : cur) * 100;
            cur = -1;
            any = true;
            continue;
        }
        d = voice_cn_digit(cp);
        if (d >= 0) {
            cur = d;
            any = true;
            continue;
        }
        s = save;
        break;
    }
    if (!any) {
        return false;
    }
    *out = val + (cur < 0 ? 0 : cur);
    *ps = s;
    return true;
}

static bool voice_starts(const char *s, const char *lit)
{
    return strncmp(s, lit, strlen(lit)) == 0;
}

static void voice_skip_ascii_unit_space(const char **ps)
{
    const char *p = *ps;
    while (*p == ' ') {
        p++;
    }
    *ps = p;
}

/** 从原话里取第一个明确时长。没说到就返回 0。 */
static uint16_t voice_parse_minutes(const char *text)
{
    const char *p = text;

    if (text == NULL) {
        return 0;
    }
    while (*p != '\0') {
        const char *q = p;
        int num = 0;
        bool half = false;
        bool saw = false;
        int minutes = 0;

        if (voice_starts(q, "半个小时")) {
            return 30;
        }
        if (voice_starts(q, "半小时") || voice_starts(q, "半钟头")) {
            return 30;
        }
        if (strncasecmp(q, "half an hour", 12) == 0 || strncasecmp(q, "half hour", 9) == 0) {
            return 30;
        }
        if (strncasecmp(q, "an hour", 7) == 0) {
            return 60;
        }
        if (*q >= '0' && *q <= '9') {
            while (*q >= '0' && *q <= '9') {
                num = num * 10 + (*q - '0');
                q++;
                saw = true;
            }
            if (*q == '.' && q[1] == '5') {
                half = true;
                q += 2;
            }
        } else if (voice_parse_cn(&q, &num)) {
            saw = true;
        }
        if (saw && num >= 0 && num <= 24 * 60) {
            voice_skip_ascii_unit_space(&q);
            if (voice_starts(q, "个半")) {
                half = true;
                q += strlen("个半");
            } else if (voice_starts(q, "个")) {
                q += strlen("个");
            }
            if (voice_starts(q, "半")) {
                half = true;
                q += strlen("半");
            }
            voice_skip_ascii_unit_space(&q);
            if (voice_starts(q, "小时") || voice_starts(q, "钟头")) {
                q += voice_starts(q, "小时") ? strlen("小时") : strlen("钟头");
                minutes = num * 60 + (half ? 30 : 0);
            } else if (strncasecmp(q, "hours", 5) == 0) {
                q += 5;
                minutes = num * 60 + (half ? 30 : 0);
            } else if (strncasecmp(q, "hour", 4) == 0) {
                q += 4;
                minutes = num * 60 + (half ? 30 : 0);
            } else if ((*q == 'h' || *q == 'H') && !isalpha((unsigned char)q[1])) {
                q += 1;
                minutes = num * 60 + (half ? 30 : 0);
            } else if (voice_starts(q, "分钟")) {
                q += strlen("分钟");
                minutes = num;
            } else if (voice_starts(q, "分") && !voice_starts(q, "分享") && !voice_starts(q, "分别")) {
                unsigned char next = (unsigned char)q[strlen("分")];
                /* 「十分重要」这种不是时长。裸「分」只认结尾或标点。 */
                if (next < 0x80) {
                    q += strlen("分");
                    minutes = num;
                }
            } else if (voice_starts(q, "刻钟")) {
                q += strlen("刻钟");
                minutes = num * 15;
            } else if (strncasecmp(q, "minutes", 7) == 0) {
                q += 7;
                minutes = num;
            } else if (strncasecmp(q, "minute", 6) == 0) {
                q += 6;
                minutes = num;
            } else if (strncasecmp(q, "mins", 4) == 0) {
                q += 4;
                minutes = num;
            } else if (strncasecmp(q, "min", 3) == 0 && !isalpha((unsigned char)q[3])) {
                q += 3;
                minutes = num;
            }
            if (minutes > 0 && minutes <= 24 * 60 && q > p) {
                return (uint16_t)minutes;
            }
        }
        if (((unsigned char)*p & 0x80) == 0) {
            p++;
        } else if (((unsigned char)*p & 0xE0) == 0xC0) {
            p += 2;
        } else if (((unsigned char)*p & 0xF0) == 0xE0) {
            p += 3;
        } else if (((unsigned char)*p & 0xF8) == 0xF0) {
            p += 4;
        } else {
            p++;
        }
    }
    return 0;
}

static bool voice_has_ci(const char *text, const char *lit)
{
    size_t n = strlen(lit);
    const char *s = text;

    if (n == 0) {
        return false;
    }
    while (*s != '\0') {
        if (strncasecmp(s, lit, n) == 0) {
            return true;
        }
        s++;
    }
    return false;
}

static bool voice_duration_cue(const char *text)
{
    if (text == NULL) {
        return false;
    }
    return strstr(text, "分钟") != NULL || strstr(text, "小时") != NULL ||
           strstr(text, "钟头") != NULL || strstr(text, "刻钟") != NULL ||
           voice_has_ci(text, "hour") || voice_has_ci(text, "min");
}

static bool voice_refine(const char *transcript, char *out, size_t cap, uint16_t *minutes)
{
    char escaped[TODO_NAME_LEN * 2];
    char body[1200];
    char *resp;
    char raw[TODO_NAME_LEN];
    int n;
    bool ok = false;

    if (minutes != NULL) {
        *minutes = 0;
    }
    voice_json_escape(transcript, escaped, sizeof(escaped));
    n = snprintf(body, sizeof(body),
                 "{\"model\":\"%s\",\"messages\":["
                 "{\"role\":\"system\",\"content\":\""
                 "把用户的话收成一条待办。只输出一行：标题|分钟。"
                 "标题不超过20个字，不要引号，标题里不要再写时长。"
                 "原话明确说了要花多久才把分钟写成整数：半小时=30，一小时=60，一个半小时=90，一刻钟=15。"
                 "没提到时长就写0。例如：读书半小时->读书|30。去超市->去超市|0。"
                 "\"},"
                 "{\"role\":\"user\",\"content\":\"%s\"}]}",
                 REFINE_MODEL, escaped);
    if (n <= 0 || (size_t)n >= sizeof(body)) {
        return false;
    }
    resp = heap_caps_malloc(VOICE_RESP_MAX, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (resp == NULL) {
        return false;
    }
    resp[0] = '\0';
    if (!voice_http_post(REFINE_URL, body, (size_t)n, resp, VOICE_RESP_MAX)) {
        free(resp);
        return false;
    }
    if (voice_json_pick(resp, "content", raw, sizeof(raw))) {
        char *bar = strrchr(raw, '|');
        if (bar == NULL) {
            bar = strstr(raw, "｜");
        }
        if (bar != NULL && minutes != NULL) {
            int v = atoi(bar + (*bar == '|' ? 1 : 3));
            *bar = '\0';
            if (v > 0 && v <= 24 * 60) {
                *minutes = (uint16_t)v;
            }
        }
        voice_trim_copy(out, cap, raw, 20);
        ok = out[0] != '\0';
    } else {
        ESP_LOGW(TAG, "refine parse %.180s", resp);
    }
    free(resp);
    return ok;
}

static bool voice_refine_idea(const char *transcript, inspiration_record_t *record)
{
    cJSON *root = cJSON_CreateObject();
    cJSON *messages = cJSON_AddArrayToObject(root, "messages");
    cJSON *system = cJSON_CreateObject();
    cJSON *user = cJSON_CreateObject();
    if (!root || !messages || !system || !user) {
        cJSON_Delete(system); cJSON_Delete(user); cJSON_Delete(root); return false;
    }
    cJSON_AddItemToArray(messages, system);
    cJSON_AddItemToArray(messages, user);
    cJSON_AddStringToObject(root, "model", REFINE_MODEL);
    cJSON_AddNumberToObject(root, "max_tokens", 600);
    cJSON_AddStringToObject(system, "role", "system");
    cJSON_AddStringToObject(system, "content",
        "你在整理用户突然想到的灵感。保留原意、语气和具体细节，只修正口语、断句和明显识别错误。"
        "不要扩写，不要编造事实，不要变成待办，不要补建议。保持原话语言。"
        "只输出JSON对象，字段title和text。title不超过18个字，text不超过240个字。不要代码块。");
    cJSON_AddStringToObject(user, "role", "user");
    cJSON_AddStringToObject(user, "content", transcript);
    char *body = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    char *resp = heap_caps_malloc(VOICE_RESP_MAX, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    char *content = heap_caps_malloc(VOICE_RESP_MAX, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    bool ok = false;
    if (!body || !resp || !content) goto done;
    if (!voice_http_post(REFINE_URL, body, strlen(body), resp, VOICE_RESP_MAX) ||
        !voice_json_pick(resp, "content", content, VOICE_RESP_MAX)) goto done;
    cJSON *result = cJSON_Parse(content);
    const cJSON *title = cJSON_GetObjectItemCaseSensitive(result, "title");
    const cJSON *text = cJSON_GetObjectItemCaseSensitive(result, "text");
    if (cJSON_IsString(title) && cJSON_IsString(text) && title->valuestring[0] && text->valuestring[0]) {
        inspiration_text_copy(record->title, sizeof(record->title), title->valuestring);
        inspiration_text_copy(record->text, sizeof(record->text), text->valuestring);
        ok = true;
    }
    cJSON_Delete(result);
done:
    free(body); free(resp); free(content);
    return ok;
}

static bool voice_recognize_qwen(const uint8_t *wav, size_t wav_len, char *out, size_t cap)
{
    static const char prefix[] =
        "{\"model\":\"qwen3-asr-flash\",\"input\":{\"messages\":[{\"role\":\"user\",\"content\":"
        "[{\"audio\":\"data:audio/wav;base64,";
    static const char suffix[] =
        "\"}]}]},\"parameters\":{\"asr_options\":{\"enable_itn\":true}}}";
    size_t b64_len = 4 * ((wav_len + 2) / 3);
    size_t json_len = strlen(prefix) + b64_len + strlen(suffix);
    char *json = heap_caps_malloc(json_len + 1, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    char *resp = heap_caps_malloc(VOICE_RESP_MAX, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    char raw[INSPIRATION_TEXT_BYTES];
    bool ok = false;

    if (json == NULL || resp == NULL) {
        ESP_LOGE(TAG, "asr buffer alloc failed");
        goto done;
    }
    memcpy(json, prefix, strlen(prefix));
    voice_b64(wav, wav_len, json + strlen(prefix));
    memcpy(json + strlen(prefix) + b64_len, suffix, strlen(suffix) + 1);
    if (!voice_http_post(ASR_URL, json, json_len, resp, VOICE_RESP_MAX)) {
        goto done;
    }
    if (!voice_json_pick(resp, "text", raw, sizeof(raw))) {
        ESP_LOGW(TAG, "asr parse %.180s", resp);
        goto done;
    }
    voice_trim_copy(out, cap, raw, s_job_inspiration ? 240 : 40);
    ok = out[0] != '\0';
done:
    free(json);
    free(resp);
    return ok;
}

static int voice_mean_abs(const int16_t *s, size_t n)
{
    int64_t acc = 0;
    size_t i;
    if (n == 0) {
        return 0;
    }
    for (i = 0; i < n; i++) {
        acc += s[i] < 0 ? -s[i] : s[i];
    }
    return (int)(acc / (int64_t)n);
}

static size_t voice_record(int16_t *samples, size_t max_samples)
{
    size_t filled = 0;
    int noise = 80;
    int peak = 0;
    int hot_chunks = 0;
    int quiet_chunks = 0;
    bool heard = false;
    int chunks = 0;

    if (zts6672_init(VOICE_RATE) != ESP_OK) {
        ESP_LOGW(TAG, "mic init failed");
        return 0;
    }
    (void)zts6672_detect_slot(200, NULL, NULL);
    if (zts6672_get_slot() == ZTS6672_SLOT_UNKNOWN) {
        zts6672_set_slot(ZTS6672_SLOT_LEFT);
    }
    /* 预热完再响。嘀声会灌进麦，先丢掉这几百毫秒，再量安静底噪。 */
    buzzer_beep();
    static int32_t raw[VOICE_CHUNK];
    {
        size_t dumped = 0;
        while (dumped < VOICE_CHUNK * 2) {
            size_t got = 0;
            if (zts6672_read(raw, VOICE_CHUNK, &got, 400) != ESP_OK || got == 0) {
                break;
            }
            dumped += got;
        }
    }

    while (filled + VOICE_CHUNK <= max_samples) {
        size_t got = 0;
        size_t i;
        int level;
        int gate;
        if (zts6672_read(raw, VOICE_CHUNK, &got, 400) != ESP_OK || got == 0) {
            break;
        }
        for (i = 0; i < got && filled < max_samples; i++) {
            /* zts6672_read 已经把 24 位收成低 16 位量程，这里不能再 >> 8。 */
            int32_t v = raw[i];
            if (v > 32767) v = 32767;
            if (v < -32768) v = -32768;
            samples[filled++] = (int16_t)v;
        }
        level = voice_mean_abs(samples + filled - got, got);
        s_audio_ms = (uint32_t)(filled * 1000 / VOICE_RATE);
        s_audio_level = (uint32_t)level;
        if (level > peak) {
            peak = level;
        }
        chunks++;
        if (chunks <= 1) {
            noise = level > 40 ? level : 40;
            continue;
        }
        gate = noise + noise / 2;
        if (gate < noise + 80) {
            gate = noise + 80;
        }
        if (!heard) {
            if (level > gate) {
                heard = true;
                hot_chunks++;
            }
            continue;
        }
        hot_chunks++;
        if (level < noise + (gate - noise) / 2) {
            quiet_chunks++;
            if (quiet_chunks >= (s_job_inspiration ? 12 : 6) && hot_chunks >= 2) {
                break;
            }
        } else {
            quiet_chunks = 0;
        }
    }
    zts6672_deinit();
    ESP_LOGI(TAG, "rec noise=%d peak=%d heard=%d samples=%u",
             noise, peak, (int)heard, (unsigned)filled);
    if (!heard || filled < VOICE_RATE / 5) {
        return 0;
    }
    return filled;
}

static void voice_job(void *param)
{
    uint8_t *wav = NULL;
    int16_t *samples;
    size_t n = 0;
    char transcript[INSPIRATION_TEXT_BYTES];
    char title[TODO_NAME_LEN];
    uint16_t model_minutes = 0;
    uint16_t heard_minutes;

    (void)param;
    transcript[0] = '\0';
    title[0] = '\0';
    s_job_minutes = 0;
    size_t max_samples = VOICE_RATE * (s_job_inspiration ? VOICE_MAX_MS : VOICE_TODO_MS) / 1000;
    ESP_LOGI(TAG, "record %s: internal free=%u largest=%u, PSRAM free=%u largest=%u",
             s_job_inspiration ? "idea" : "todo",
             (unsigned)heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT),
             (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT),
             (unsigned)heap_caps_get_free_size(MALLOC_CAP_SPIRAM),
             (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM));
    wav = heap_caps_malloc(44 + max_samples * sizeof(int16_t),
                           MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (wav == NULL) {
        ESP_LOGE(TAG, "pcm alloc failed");
        goto finish;
    }
    samples = (int16_t *)(wav + 44);
    n = voice_record(samples, max_samples);
    if (n == 0) {
        goto finish;
    }
    s_job_recorded = true;
    voice_wav_header(wav, (uint32_t)(n * sizeof(int16_t)));
    if (!voice_asr_recognize(wav, 44 + n * sizeof(int16_t), transcript, sizeof(transcript),
                             voice_recognize_qwen)) {
        goto finish;
    }
    /* Apply the same mode-specific limits to either provider. */
    char limited[INSPIRATION_TEXT_BYTES];
    voice_trim_copy(limited, sizeof(limited), transcript, s_job_inspiration ? 240 : 40);
    memcpy(transcript, limited, strlen(limited) + 1);
    free(wav);
    wav = NULL; /* ASR no longer needs PCM; release it before the polishing request. */
    if (s_job_inspiration) {
        inspiration_text_copy(s_job_idea.original, sizeof(s_job_idea.original), transcript);
        if (!voice_refine_idea(transcript, &s_job_idea)) {
            /* Recognition is already valuable: retain it when polishing fails. */
            inspiration_text_copy(s_job_idea.text, sizeof(s_job_idea.text), transcript);
            voice_trim_copy(s_job_idea.title, sizeof(s_job_idea.title), transcript, 18);
        }
        s_job_ok = true;
        goto finish;
    }
    ESP_LOGI(TAG, "asr: %s", transcript);
    if (!voice_refine(transcript, title, sizeof(title), &model_minutes)) {
        voice_trim_copy(title, sizeof(title), transcript, 20);
        model_minutes = 0;
    }
    if (title[0] == '\0') {
        goto finish;
    }
    heard_minutes = voice_parse_minutes(transcript);
    if (heard_minutes == 0 && model_minutes > 0 && voice_duration_cue(transcript)) {
        heard_minutes = model_minutes;
    }
    s_job_minutes = heard_minutes;
    ESP_LOGI(TAG, "todo minutes=%u", (unsigned)s_job_minutes);
    strncpy(s_job_text, title, sizeof(s_job_text) - 1);
    s_job_text[sizeof(s_job_text) - 1] = '\0';
    s_job_ok = true;
finish:
    free(wav);
    if (!s_job_ok) {
        s_job_text[0] = '\0';
        s_job_minutes = 0;
    }
    s_job_running = false;
    s_job_done = true;
    vTaskDelete(NULL);
}

static void voice_start_listen(void)
{
    if (s_job_running || !is_wifi_connected()) {
        voice_show_idle();
        return;
    }
    s_job_done = false;
    s_job_ok = false;
    s_job_recorded = false;
    s_job_text[0] = '\0';
    s_job_minutes = 0;
    s_minutes = 0;
    s_job_inspiration = s_inspiration;
    s_audio_ms = s_audio_level = 0;
    s_feedback_at = lv_tick_get() - 50;
    s_feedback_seconds = UINT32_MAX;
    memset(&s_job_idea, 0, sizeof(s_job_idea));
    lv_bar_set_range(s_record_bar, 0, s_inspiration ? VOICE_MAX_MS : VOICE_TODO_MS);
    s_job_running = true;
    voice_apply(VS_LISTEN);
    if (xTaskCreatePinnedToCore(voice_job, "voice_job", 20480, NULL, 5, NULL, 0) != pdPASS) {
        s_job_running = false;
        strncpy(s_shown, ui_tr("Didn't catch that", "没听清"), sizeof(s_shown) - 1);
        voice_apply(VS_FAIL);
    }
}

void voice_todo_poll(void)
{
    if (s_job_running) idle_sleep_note_activity();
    if (s_enter_req) {
        s_enter_req = false;
        if (!s_open && !s_wificfg_return && !inspiration_ui_is_open() && !focus_timer_is_running()) {
            inspiration_ui_open_hub();
        }
    }
    if (s_capture_req && lv_disp_get_default()->scr_to_load == NULL) {
        s_capture_req = false;
        voice_show();
    }
    if (!s_open || s_scr == NULL || !lv_obj_is_valid(s_scr)) {
        if (s_job_done) {
            s_job_done = false;
        }
        return;
    }
    if (s_job_done) {
        s_job_done = false;
        if (s_job_ok && s_job_inspiration) {
            rx8130_time_t now;
            if (rx8130_get_time(I2C_NUM_0, &now) == ESP_OK && now.year >= 2024 && now.year <= 2099 &&
                now.month >= 1 && now.month <= 12 && now.date >= 1 && now.date <= 31 &&
                now.hour <= 23 && now.minute <= 59) {
                snprintf(s_job_idea.time, sizeof(s_job_idea.time), "%04u-%02u-%02u %02u:%02u",
                         (unsigned)now.year % 10000u, (unsigned)now.month % 100u, (unsigned)now.date % 100u,
                         (unsigned)now.hour % 100u, (unsigned)now.minute % 100u);
            }
            inspiration_ui_preview(&s_job_idea);
        } else if (s_job_ok && s_job_text[0] != '\0') {
            strncpy(s_shown, s_job_text, sizeof(s_shown) - 1);
            s_shown[sizeof(s_shown) - 1] = '\0';
            s_minutes = s_job_minutes;
            voice_apply(VS_RESULT);
        } else {
            strncpy(s_shown, ui_tr("Didn't catch that\nCome a bit closer",
                                    "刚才没听清\n靠近一点再说一次"),
                    sizeof(s_shown) - 1);
            s_shown[sizeof(s_shown) - 1] = '\0';
            voice_apply(VS_FAIL);
        }
    } else if (s_state == VS_LISTEN && !s_job_recorded) {
        voice_recording_feedback();
    } else if (s_state == VS_LISTEN && s_job_recorded) {
        voice_apply(VS_THINK);
    } else if ((s_state == VS_WIFI || s_state == VS_READY) && !s_job_running) {
        bool wifi = is_wifi_connected();
        if (wifi != s_wifi_seen) {
            voice_show_idle();
        }
    }
}
