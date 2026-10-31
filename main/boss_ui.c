/* Code-built speech bubbles and punchline cards; no Guider-generated edits. */
#include "boss_ui.h"
#include "boss_service.h"
#include "boss_rules.h"
#include "voice_todo.h"
#include "inspiration_ui.h"
#include "custom.h"
#include "gui_guider.h"
#include "ui_locale.h"
#include <string.h>

#define PAPER 0xF5EFE3
#define INK 0x34423E
#define MUTED 0x867A68
#define CARD 0x293F3D
#define MINT 0xA5D5BE
#define PEACH 0xF3B783
#define TEXT_W 206
#define EFFECT_DELAY_MS 500
#define PAGE_MS 2800
LV_IMG_DECLARE(boss_sprite_0);
LV_IMG_DECLARE(boss_sprite_1);
LV_IMG_DECLARE(boss_sprite_2);
LV_IMG_DECLARE(boss_sprite_3);
static const lv_img_dsc_t *sprites[] = {&boss_sprite_0, &boss_sprite_1, &boss_sprite_2, &boss_sprite_3};
static lv_obj_t *s_screen, *s_source, *s_reply, *s_effect, *s_status, *s_boss, *s_led, *s_meter;
static lv_obj_t *s_bubble, *s_card, *s_badge, *s_reply_title, *s_page_indicator, *s_action_hint;
static lv_obj_t *s_settings, *s_button;
static lv_group_t *s_group;
static bool s_open, s_requested, s_click, s_leave, s_restart, s_paused, s_zh;
static bool s_have_result, s_effect_armed, s_effect_ready, s_banner_is_effect;
static uint32_t s_click_at, s_paint_at, s_revision, s_last_utterance;
static uint32_t s_effect_at, s_result_at, s_page_at, s_source_at;
static unsigned s_reply_next, s_reply_page, s_reply_total, s_source_next;
static char s_source_text[BOSS_SOURCE_CAP], s_reply_text[BOSS_REPLY_CAP], s_effect_text[80];
static boss_snapshot_t s_snapshot;

bool boss_ui_is_open(void) { return s_open || s_requested; }
static lv_obj_t *box(lv_obj_t *parent, int x, int y, int w, int h, uint32_t color, int radius)
{
    lv_obj_t *o = lv_obj_create(parent); lv_obj_remove_style_all(o);
    lv_obj_set_pos(o, x, y); lv_obj_set_size(o, w, h);
    lv_obj_clear_flag(o, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_color(o, lv_color_hex(color), 0); lv_obj_set_style_bg_opa(o, 255, 0);
    lv_obj_set_style_radius(o, radius, 0); return o;
}
static lv_obj_t *label(lv_obj_t *parent, const char *text, int x, int y, int w, int h,
                       const lv_font_t *font, uint32_t color)
{
    lv_obj_t *l = lv_label_create(parent);
    lv_obj_set_pos(l, x, y); lv_obj_set_size(l, w, h);
    lv_obj_set_style_text_font(l, font, 0); lv_obj_set_style_text_color(l, lv_color_hex(color), 0);
    lv_obj_set_style_text_line_space(l, 0, 0);
    lv_obj_clear_flag(l, LV_OBJ_FLAG_CLICKABLE);
    lv_label_set_long_mode(l, LV_LABEL_LONG_CLIP); lv_label_set_text(l, text); return l;
}
static void rise(void *obj, int32_t value) { lv_obj_set_style_translate_y(obj, value, 0); }
static void pop(lv_obj_t *obj, int offset)
{
    lv_anim_del(obj, rise);
    lv_anim_t a; lv_anim_init(&a); lv_anim_set_var(&a, obj); lv_anim_set_exec_cb(&a, rise);
    lv_anim_set_values(&a, offset, 0); lv_anim_set_time(&a, 190);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out); rise(obj, offset); lv_anim_start(&a);
}
static void tail_draw(lv_event_t *e)
{
    lv_obj_t *o = lv_event_get_target(e); lv_area_t a; lv_obj_get_coords(o, &a);
    lv_point_t p[] = {{a.x2, a.y1}, {a.x1, a.y2 - 2}, {a.x2, a.y2}};
    lv_draw_rect_dsc_t d; lv_draw_rect_dsc_init(&d); d.bg_color = lv_color_hex(0xFFFCF5);
    lv_draw_polygon(lv_event_get_draw_ctx(e), &d, p, 3);
}
/* Use LVGL's actual font metrics to page two lines, never split a UTF-8 glyph. */
static unsigned page_end(const char *text, unsigned start)
{
    unsigned at = start;
    for (unsigned line = 0; line < 2 && text[at]; ++line) {
        unsigned n = _lv_txt_get_next_line(text + at, ui_font_16(), 0, TEXT_W, NULL, LV_TEXT_FLAG_NONE);
        if (!n) break;
        at += n;
    }
    return at;
}
static unsigned page_text(lv_obj_t *label_obj, const char *text, unsigned start)
{
    unsigned end = page_end(text, start);
    char page[BOSS_SOURCE_CAP]; unsigned n = end - start;
    if (n >= sizeof(page)) n = sizeof(page) - 1;
    memcpy(page, text + start, n); page[n] = 0;
    /* A line separator at a page boundary should not create an empty line. */
    while (n && (page[n-1] == '\n' || page[n-1] == '\r')) page[--n] = 0;
    lv_label_set_text(label_obj, page);
    return text[end] ? end : 0;
}
static void reply_page(unsigned start)
{
    s_reply_next = page_text(s_reply, s_reply_text, start);
    if (s_reply_total > 1) lv_label_set_text_fmt(s_page_indicator, "%u/%u", s_reply_page, s_reply_total);
    else lv_label_set_text(s_page_indicator, "");
    s_page_at = lv_tick_get();
}
static void release_input(void)
{
    if (!s_group) return;
    lv_indev_t *in = NULL;
    while ((in = lv_indev_get_next(in))) if (in->group == s_group) {
        lv_indev_reset(in, NULL); lv_indev_wait_release(in); lv_indev_set_group(in, g);
    }
    lv_group_del(s_group); s_group = NULL;
}
static void stop(void)
{
    boss_service_stop(); s_open = s_click = s_restart = s_effect_armed = false; release_input();
    lv_anim_del(s_bubble, rise); lv_anim_del(s_card, rise); lv_anim_del(s_badge, rise);
}
static void deleted(lv_event_t *e)
{
    if (lv_event_get_target(e) != s_screen) return;
    if (s_open) stop();
    s_screen = NULL;
}
static void toggle(void)
{
    boss_service_snapshot(&s_snapshot);
    if (!s_paused && (s_snapshot.state == BOSS_LISTENING || s_snapshot.state == BOSS_CONNECTING)) {
        s_paused = true; s_restart = s_effect_armed = false; boss_service_stop();
    } else { s_paused = false; s_restart = true; }
}
static void clicked(lv_event_t *e)
{
    if (!s_open || s_leave || lv_scr_act() != s_screen) return;
    idle_sleep_note_activity();
    if (s_click && lv_tick_elaps(s_click_at) < 450) { s_click = false; s_leave = true; return; }
    if (s_click) toggle();
    s_click = true; s_click_at = lv_tick_get();
}
static void build(void)
{
    lv_obj_t *old = lv_scr_act();
    s_open = true; s_paused = s_leave = s_click = false;
    s_have_result = s_effect_armed = s_effect_ready = s_banner_is_effect = false;
    s_source_text[0] = s_reply_text[0] = s_effect_text[0] = 0;
    s_reply_next = s_source_next = 0; s_reply_page = s_reply_total = 1;
    s_restart = true; s_revision = s_last_utterance = UINT32_MAX;
    s_screen = lv_obj_create(NULL); lv_obj_remove_style_all(s_screen);
    lv_obj_set_size(s_screen, 320, 170); lv_obj_clear_flag(s_screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(s_screen, lv_color_hex(PAPER), 0); lv_obj_set_style_bg_opa(s_screen, 255, 0);
    lv_obj_add_event_cb(s_screen, deleted, LV_EVENT_DELETE, NULL);
    lv_obj_add_event_cb(s_screen, clicked, LV_EVENT_SHORT_CLICKED, NULL);
    lv_obj_add_flag(s_screen, LV_OBJ_FLAG_CLICKABLE);
    box(s_screen, 8, 7, 3, 12, 0xC98E60, 1);
    label(s_screen, "黑话翻译局", 17, 5, 175, 18, ui_font_16(), INK);
    label(s_screen, "娱乐演绎", 246, 7, 68, 17, ui_font_12(), MUTED);
    box(s_screen, 7, 32, 69, 78, 0xEAE2D3, 15);
    s_boss = lv_img_create(s_screen); lv_img_set_src(s_boss, sprites[0]); lv_obj_set_pos(s_boss, 3, 32);
    s_bubble = box(s_screen, 86, 26, 226, 54, 0xFFFCF5, 10);
    lv_obj_set_style_border_color(s_bubble, lv_color_hex(0xDACFBD), 0); lv_obj_set_style_border_width(s_bubble, 1, 0);
    label(s_bubble, "老板说", 9, 2, 192, 17, ui_font_12(), MUTED);
    s_source = label(s_bubble, "试试：公司就是你的家", 9, 20, TEXT_W, 32, ui_font_16(), 0x655D51);
    lv_label_set_long_mode(s_source, LV_LABEL_LONG_WRAP);
    lv_obj_t *tail = box(s_screen, 78, 52, 9, 12, PAPER, 0);
    lv_obj_set_style_bg_opa(tail, 0, 0); lv_obj_add_event_cb(tail, tail_draw, LV_EVENT_DRAW_MAIN, NULL);
    s_card = box(s_screen, 86, 85, 226, 59, CARD, 10);
    s_reply_title = label(s_card, "等你开麦", 10, 3, 158, 18, ui_font_12(), MINT);
    s_page_indicator = label(s_card, "", 180, 5, 35, 16, ui_font_12(), MINT);
    lv_obj_set_style_text_align(s_page_indicator, LV_TEXT_ALIGN_RIGHT, 0);
    s_reply = label(s_card, "领导请开麦，潜台词我来猜。", 10, 24, TEXT_W, 32, ui_font_16(), 0xFFF8EA);
    lv_label_set_long_mode(s_reply, LV_LABEL_LONG_WRAP);
    s_badge = box(s_screen, 86, 148, 226, 20, 0xE9E0D0, 5);
    s_effect = label(s_badge, "正在连接…", 7, 2, 212, 17, ui_font_16(), MUTED);
    lv_label_set_long_mode(s_effect, LV_LABEL_LONG_SCROLL_CIRCULAR);
    s_led = box(s_screen, 15, 116, 5, 5, MUTED, LV_RADIUS_CIRCLE);
    s_status = label(s_screen, "LINK", 25, 112, 52, 17, ui_font_12(), MUTED);
    s_meter = lv_bar_create(s_screen); lv_obj_set_pos(s_meter, 15, 125); lv_obj_set_size(s_meter, 47, 3);
    lv_bar_set_range(s_meter, 0, 1500);
    lv_obj_set_style_bg_color(s_meter, lv_color_hex(0xD8CBB8), LV_PART_MAIN);
    lv_obj_set_style_bg_color(s_meter, lv_color_hex(0x6D9C88), LV_PART_INDICATOR);
    s_action_hint = label(s_screen, "单击暂停", 9, 132, 73, 17, ui_font_12(), MUTED);
    label(s_screen, "双击返回", 9, 150, 73, 17, ui_font_12(), MUTED);
    s_group = lv_group_create(); lv_group_add_obj(s_group, s_screen);
    lv_indev_t *in = NULL;
    while ((in = lv_indev_get_next(in))) {
        if (lv_indev_get_type(in) != LV_INDEV_TYPE_KEYPAD) continue;
        lv_indev_reset(in, NULL); lv_indev_wait_release(in); lv_indev_set_group(in, s_group);
    }
    inspiration_ui_mark_guider_deleted(old);
    lv_scr_load_anim(s_screen, LV_SCR_LOAD_ANIM_FADE_ON, 160, 0, true);
}
static void settings_event(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) s_requested = true;
    else if (lv_event_get_code(e) == LV_EVENT_DELETE) {
        if (lv_event_get_target(e) == s_settings) s_settings = NULL;
        if (lv_event_get_target(e) == s_button) s_button = NULL;
    }
}
static void entry_poll(void)
{
    lv_obj_t *screen = guider_ui.screen_setting;
    if (!screen || !lv_obj_is_valid(screen) || lv_scr_act() != screen) return;
    bool zh = ui_lang_is_zh();
    if (s_settings == screen && s_button) {
        if (s_zh != zh) { lv_label_set_text(lv_obj_get_child(s_button, 0), "Boss Trans"); s_zh = zh; }
        return;
    }
    s_button = screen_setting_list_add_btn("Boss Trans");
    if (!s_button) return;
    s_settings = screen; s_zh = zh;
    lv_obj_set_style_text_font(s_button, ui_font_14(), 0);
    lv_obj_set_style_text_font(s_button, ui_font_14(), LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(lv_obj_get_child(s_button, 0), ui_font_14(), 0);
    lv_obj_add_event_cb(s_button, settings_event, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(screen, settings_event, LV_EVENT_DELETE, NULL);
}
static void apply_content(void)
{
    const char *source = s_snapshot.source[0] ? s_snapshot.source : "试试：公司就是你的家";
    const char *reply = s_snapshot.reply[0] ? s_snapshot.reply : "领导请开麦，潜台词我来猜。";
    bool new_utterance = s_last_utterance != s_snapshot.utterance;
    if (new_utterance || strcmp(s_source_text, source)) {
        bool first = !s_source_text[0];
        boss_text_copy(s_source_text, sizeof(s_source_text), source);
        s_source_next = page_text(s_source, s_source_text, 0); s_source_at = lv_tick_get();
        if (first || new_utterance) pop(s_bubble, 4);
    }
    bool result = s_snapshot.translated && !s_snapshot.partial;
    bool changed = new_utterance || result != s_have_result || strcmp(s_reply_text, reply) || strcmp(s_effect_text, s_snapshot.effect);
    if (changed) {
        s_effect_armed = s_effect_ready = false;
        boss_text_copy(s_reply_text, sizeof(s_reply_text), reply);
        boss_text_copy(s_effect_text, sizeof(s_effect_text), s_snapshot.effect);
        s_reply_total = 0;
        for (unsigned at = 0, end; s_reply_text[at]; at = end) {
            end = page_end(s_reply_text, at); if (end <= at) break; ++s_reply_total;
        }
        if (!s_reply_total) s_reply_total = 1;
        s_reply_page = 1; reply_page(0);
        lv_label_set_text(s_reply_title, result ? "潜台词" : s_snapshot.turn == BOSS_TURN_SHOW ? "稍后再试" : s_snapshot.source[0] ? "解码中…" : "等你开麦");
        lv_obj_set_style_text_color(s_reply, lv_color_hex(result ? 0xFFF8EA : 0xAEBEB6), 0);
        if (result) {
            pop(s_card, 6); s_result_at = lv_tick_get();
            s_effect_at = s_result_at; s_effect_armed = s_effect_text[0] && s_snapshot.state == BOSS_LISTENING;
        }
    }
    s_have_result = result; s_last_utterance = s_snapshot.utterance;
    if (s_snapshot.state != BOSS_LISTENING) s_effect_armed = false;
    const char *state = s_snapshot.state == BOSS_LISTENING ? (s_snapshot.turn == BOSS_TURN_THINK ? "翻译中" : s_snapshot.turn == BOSS_TURN_SHOW ? "看翻译" : "请说话") : s_snapshot.state == BOSS_CONNECTING ? "LINK" : s_snapshot.state == BOSS_OFF ? "PAUSE" : "RETRY";
    lv_label_set_text(s_status, state);
    lv_obj_set_style_text_color(s_status, lv_color_hex(s_snapshot.turn == BOSS_TURN_LISTEN ? MUTED : 0xA16935), 0);
    lv_label_set_text(s_action_hint, s_snapshot.state == BOSS_OFF ? "单击继续" : s_snapshot.state == BOSS_ERROR ? "单击重试" : "单击暂停");
    uint32_t color = s_snapshot.state == BOSS_LISTENING ? 0x6D9C88 : s_snapshot.state == BOSS_ERROR ? 0xB3513F : MUTED;
    lv_obj_set_style_bg_color(s_led, lv_color_hex(color), 0);
}
static void banner(void)
{
    bool warning = s_snapshot.state != BOSS_LISTENING || strstr(s_snapshot.status, "缓冲") != NULL;
    bool effect = s_effect_ready && s_have_result && !warning;
    const char *text = effect ? s_effect_text : s_snapshot.status;
    if (!text[0]) text = "等待领导开麦…";
    if (strcmp(lv_label_get_text(s_effect), text)) lv_label_set_text(s_effect, text);
    bool error = s_snapshot.state == BOSS_ERROR;
    lv_obj_set_style_bg_color(s_badge, lv_color_hex(error ? 0xA84F3E : effect ? PEACH : 0xE9E0D0), 0);
    lv_obj_set_style_text_color(s_effect, lv_color_hex(error ? 0xFFFFFF : effect ? 0x633916 : MUTED), 0);
    if (effect && !s_banner_is_effect) pop(s_badge, 3);
    s_banner_is_effect = effect;
}
void boss_ui_poll(void)
{
    bool transition = lv_disp_get_default()->scr_to_load != NULL;
    if (!transition) entry_poll();
    if (!transition && s_requested && !voice_todo_is_open() && !voice_todo_audio_busy() && !focus_timer_is_running()) {
        s_requested = false; build(); transition = true;
    }
    if (!s_open || !s_screen) return;
    if (!transition && lv_scr_act() != s_screen) { stop(); return; }
    idle_sleep_note_activity();
    if (s_leave && !transition) {
        stop(); bool old_del = true;
        if (!guider_ui.screen_main || !lv_obj_is_valid(guider_ui.screen_main)) guider_ui.screen_main_del = true;
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_main, guider_ui.screen_main_del, &old_del,
                              setup_scr_screen_main, LV_SCR_LOAD_ANIM_FADE_ON, 160, 0, false, true);
        return;
    }
    if (s_click && lv_tick_elaps(s_click_at) >= 450) { s_click = false; toggle(); }
    if (s_restart && !transition && !boss_service_busy()) { s_restart = false; boss_service_start(); }
    if (lv_tick_elaps(s_paint_at) < 100) return;
    s_paint_at = lv_tick_get(); boss_service_snapshot(&s_snapshot);
    if (s_revision != s_snapshot.revision) { s_revision = s_snapshot.revision; apply_content(); }
    if (s_effect_armed && lv_tick_elaps(s_effect_at) >= EFFECT_DELAY_MS) {
        s_effect_armed = false; s_effect_ready = true;
    }
    if (s_reply_total > 1 && lv_tick_elaps(s_page_at) >= PAGE_MS) {
        s_reply_page = s_reply_next ? s_reply_page + 1 : 1; reply_page(s_reply_next);
    }
    if (lv_tick_elaps(s_source_at) >= PAGE_MS) {
        s_source_next = page_text(s_source, s_source_text, s_source_next); s_source_at = lv_tick_get();
    }
    banner();
    unsigned phase = s_paint_at / 180;
    bool talking = s_snapshot.state == BOSS_LISTENING && s_snapshot.turn == BOSS_TURN_LISTEN && s_snapshot.level > 120;
    bool punchline = s_have_result && lv_tick_elaps(s_result_at) < 900;
    unsigned frame = punchline ? 3 : talking ? (phase % 2 ? 2 : 0) : phase % 24 == 0 ? 1 : 0;
    lv_img_set_src(s_boss, sprites[frame]); lv_obj_set_y(s_boss, 32 + (phase % 8 < 4 ? 0 : 1));
    lv_bar_set_value(s_meter, s_snapshot.state == BOSS_LISTENING ? s_snapshot.level : 0, LV_ANIM_OFF);
}
