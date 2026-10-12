/* Code-built voice hub and notebook. No generated Guider objects or assets. */
#include "inspiration_ui.h"
#include "inspiration_service.h"
#include "voice_todo.h"
#include "ui_locale.h"
#include "gui_guider.h"
#include "custom.h"
#include <stdio.h>
#include <string.h>
#ifdef ESP_PLATFORM
#include "esp_attr.h"
#define IDEA_RAM EXT_RAM_BSS_ATTR
#else
#define IDEA_RAM
#endif

#define PAPER 0xEEF3F8
#define NAVY 0x274A6E
#define INK 0x3D5670
#define MUTED 0x8194A8
#define MINT 0x5E9C92
#define GOLD 0xC49A57
#define WHITE 0xFFFFFF

typedef enum { PAGE_HUB, PAGE_LIST, PAGE_PREVIEW, PAGE_DETAIL, PAGE_DELETE } page_t;
typedef enum {
    ACT_NONE, ACT_HOME, ACT_TODO, ACT_IDEA, ACT_LIBRARY, ACT_PREV, ACT_NEXT,
    ACT_ROW0, ACT_ROW1, ACT_ROW2, ACT_SAVE, ACT_READ_PREVIEW, ACT_BACK,
    ACT_ORIGINAL, ACT_DELETE, ACT_DELETE_CONFIRM, ACT_RESUME,
} action_t;

static lv_obj_t *s_screen, *s_heading, *s_caption, *s_message;
static lv_obj_t *s_rows[3], *s_row_titles[3], *s_row_dates[3];
static lv_obj_t *s_scroll, *s_body, *s_rail, *s_thumb;
static lv_obj_t *s_focus_bar;
static lv_group_t *s_group;
static lv_timer_t *s_click_timer, *s_saved_timer;
static page_t s_page;
static action_t s_action;
static volatile bool s_open;
static bool s_busy, s_need_request, s_original, s_preview_detail, s_saving;
static bool s_hub_req, s_preview_req, s_library_req;
static lv_obj_t *s_settings_screen, *s_settings_button;
static bool s_settings_zh;
static bool s_click_armed, s_saved, s_detail_ready;
static unsigned s_count, s_start, s_selected, s_rows_loaded, s_last_mode;
static uint32_t s_token, s_click_at;
static inspiration_operation_t s_operation;
static inspiration_record_t s_record IDEA_RAM;
static inspiration_record_t s_draft IDEA_RAM;
static bool s_draft_valid;
static inspiration_request_t s_request IDEA_RAM;
static inspiration_reply_t s_reply IDEA_RAM;

static void build(page_t page);
static void bind(lv_obj_t *obj);
static void request(inspiration_operation_t op, unsigned index);
static void render_detail(void);

bool inspiration_ui_is_open(void) { return s_open || s_hub_req || s_preview_req || s_library_req; }

void inspiration_ui_mark_guider_deleted(void *screen)
{
#define MARK(name) if (screen == guider_ui.name) { guider_ui.name##_del = true; return; }
    MARK(screen_reppo) MARK(screen_main) MARK(screen_todolist) MARK(screen_todosel)
    MARK(screen_todoback) MARK(screen_timing) MARK(screen_pomodoro) MARK(screen_setting)
    MARK(screen_wificfg) MARK(screen_donelist) MARK(screen_answer) MARK(screen_loading)
    MARK(screen_getanswer) MARK(screen_picture) MARK(screen_gash) MARK(screen_game)
    MARK(screen_keep) MARK(screen_book)
#undef MARK
}

static void anim_opa(void *obj, int32_t value) { lv_obj_set_style_opa(obj, value, LV_PART_MAIN); }
static void anim_y(void *obj, int32_t value) { lv_obj_set_style_translate_y(obj, value, LV_PART_MAIN); }
static void anim_x(void *obj, int32_t value) { lv_obj_set_x(obj, value); }
static void animate(lv_obj_t *obj, lv_anim_exec_xcb_t exec, int from, int to,
                    unsigned duration, unsigned delay)
{
    lv_anim_t a;
    lv_anim_init(&a); lv_anim_set_var(&a, obj); lv_anim_set_exec_cb(&a, exec);
    lv_anim_set_values(&a, from, to); lv_anim_set_time(&a, duration); lv_anim_set_delay(&a, delay);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out); lv_anim_start(&a);
    exec(obj, from);
}
static void reveal(lv_obj_t *obj, unsigned delay)
{
    animate(obj, anim_opa, 0, 255, 210, delay);
    animate(obj, anim_y, 10, 0, 310, delay);
}
static lv_obj_t *plain(lv_obj_t *parent, int x, int y, int w, int h)
{
    lv_obj_t *o = lv_obj_create(parent);
    lv_obj_remove_style_all(o); lv_obj_set_pos(o, x, y); lv_obj_set_size(o, w, h);
    lv_obj_clear_flag(o, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    return o;
}
static lv_obj_t *label(lv_obj_t *parent, const char *text, int x, int y, int w,
                       const lv_font_t *font, uint32_t color)
{
    lv_obj_t *l = lv_label_create(parent);
    lv_obj_set_pos(l, x, y); lv_obj_set_width(l, w);
    lv_obj_set_style_text_font(l, font, LV_PART_MAIN);
    lv_obj_set_style_text_color(l, lv_color_hex(color), LV_PART_MAIN);
    lv_label_set_text(l, text);
    return l;
}
static const char *tr(const char *en, const char *zh) { return ui_tr(en, zh); }

static void release_input(void)
{
    if (!s_group) return;
    lv_indev_t *in = NULL;
    while ((in = lv_indev_get_next(in))) {
        if (in->group != s_group) continue;
        lv_indev_reset(in, NULL); lv_indev_wait_release(in); lv_indev_set_group(in, g);
    }
    lv_group_del(s_group); s_group = NULL;
}
static void cancel_timers(void)
{
    if (s_click_timer) { lv_timer_del(s_click_timer); s_click_timer = NULL; }
    if (s_saved_timer) { lv_timer_del(s_saved_timer); s_saved_timer = NULL; }
    s_click_armed = false;
}
static void deleted(lv_event_t *e)
{
    if (lv_event_get_target(e) != s_screen) return;
    cancel_timers(); release_input(); s_screen = NULL; s_open = false;
}
static void focus(lv_event_t *e)
{
    lv_obj_t *o = lv_event_get_target(e);
    if (s_page == PAGE_LIST) {
        for (unsigned i = 0; i < s_rows_loaded; ++i) if (o == s_rows[i]) s_selected = s_start + i;
    }
    if (s_page == PAGE_HUB && s_focus_bar) {
        if (lv_obj_get_y(o) != 53) { lv_obj_add_flag(s_focus_bar, LV_OBJ_FLAG_HIDDEN); return; }
        lv_obj_clear_flag(s_focus_bar, LV_OBJ_FLAG_HIDDEN);
        lv_coord_t x = lv_obj_get_x(o) + 8;
        unsigned card = lv_obj_get_x(o) / 101;
        uint32_t color = card == 0 ? MINT : (card == 1 ? GOLD : NAVY);
        lv_obj_set_style_bg_color(s_focus_bar, lv_color_hex(color), LV_PART_MAIN);
        animate(s_focus_bar, anim_x, lv_obj_get_x(s_focus_bar), x, 160, 0);
    }
}
static void clicked(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED || s_saving || s_saved) return;
    s_action = (action_t)(uintptr_t)lv_event_get_user_data(e);
}
static lv_obj_t *button(lv_obj_t *parent, const char *text, int x, int y, int w, int h, action_t act)
{
    lv_obj_t *o = lv_btn_create(parent);
    lv_obj_remove_style_all(o); lv_obj_set_pos(o, x, y); lv_obj_set_size(o, w, h);
    lv_obj_clear_flag(o, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_set_style_radius(o, h < 30 ? 8 : 12, LV_PART_MAIN);
    lv_obj_set_style_bg_color(o, lv_color_hex(WHITE), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(o, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(o, lv_color_hex(NAVY), LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(o, lv_color_hex(INK), LV_PART_MAIN);
    lv_obj_set_style_text_color(o, lv_color_hex(WHITE), LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(o, ui_font_12(), LV_PART_MAIN);
    lv_obj_set_style_translate_y(o, 2, LV_STATE_PRESSED);
    static const lv_style_prop_t properties[] = {LV_STYLE_BG_COLOR, LV_STYLE_BORDER_COLOR, LV_STYLE_SHADOW_WIDTH, LV_STYLE_TRANSLATE_Y, 0};
    static lv_style_transition_dsc_t transition;
    static bool ready;
    if (!ready) { lv_style_transition_dsc_init(&transition, properties, lv_anim_path_ease_out, 140, 0, NULL); ready = true; }
    lv_obj_set_style_transition(o, &transition, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(o, 8, LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_opa(o, 30, LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(o, lv_color_hex(NAVY), LV_STATE_FOCUSED);
    if (text) { lv_obj_t *l = lv_label_create(o); lv_label_set_text(l, text); lv_obj_center(l); }
    lv_obj_add_event_cb(o, clicked, LV_EVENT_CLICKED, (void *)(uintptr_t)act);
    lv_obj_add_event_cb(o, focus, LV_EVENT_FOCUSED, NULL);
    return o;
}
static void bind(lv_obj_t *o)
{
    lv_group_add_obj(s_group, o);
}
static void init_input(void)
{
    s_group = lv_group_create();
    lv_group_set_wrap(s_group, true);
    lv_indev_t *in = NULL;
    while ((in = lv_indev_get_next(in))) {
        if (lv_indev_get_type(in) != LV_INDEV_TYPE_KEYPAD) continue;
        lv_indev_reset(in, NULL); lv_indev_wait_release(in); lv_indev_set_group(in, s_group);
    }
}
static void header(const char *title, const char *caption)
{
    s_heading = label(s_screen, title, 16, 9, 280, ui_font_16(), NAVY);
    lv_label_set_long_mode(s_heading, LV_LABEL_LONG_DOT); lv_obj_set_height(s_heading, 20);
    s_caption = label(s_screen, caption, 17, 30, 285, ui_font_12(), MUTED);
    lv_label_set_long_mode(s_caption, LV_LABEL_LONG_DOT); lv_obj_set_height(s_caption, 16);
    reveal(s_heading, 20); reveal(s_caption, 65);
}
static void message(const char *text)
{
    if (s_message) lv_label_set_text(s_message, text);
}
static void go_home(void)
{
    cancel_timers(); release_input(); s_open = false;
    bool old_del = true;
    if (!guider_ui.screen_main || !lv_obj_is_valid(guider_ui.screen_main)) guider_ui.screen_main_del = true;
    ui_load_scr_animation(&guider_ui, &guider_ui.screen_main, guider_ui.screen_main_del, &old_del,
                          setup_scr_screen_main, LV_SCR_LOAD_ANIM_FADE_ON, 160, 0, false, true);
}

static void hub(void)
{
    header(tr("Catch a little spark", "把一闪的想法留下"),
           s_draft_valid ? tr("Your unsaved thought is still here", "刚才的想法还在，选择「继续」保存") :
           tr("A thought, a task, a place to return", "记一件事，留一个念头，随时回来看看"));
    const char *names[] = {tr("Todo", "记待办"), tr("Inspiration", "记灵感"), tr("Notebook", "看灵感")};
    const char *notes[] = {tr("Next step", "让事情发生"), tr("A new spark", "留住这一刻"), tr("Your ideas", "想法在这里")};
    const char *icons[] = {LV_SYMBOL_OK, LV_SYMBOL_EDIT, LV_SYMBOL_LIST};
    uint32_t colors[] = {MINT, GOLD, NAVY};
    lv_obj_t *cards[3];
    for (unsigned i = 0; i < 3; ++i) {
        cards[i] = button(s_screen, NULL, 12 + i * 101, 53, 94, 82, ACT_TODO + i);
        lv_obj_set_style_bg_color(cards[i], lv_color_hex(WHITE), LV_STATE_FOCUSED);
        lv_obj_set_style_border_width(cards[i], 2, LV_STATE_FOCUSED);
        lv_obj_set_style_border_color(cards[i], lv_color_hex(colors[i]), LV_STATE_FOCUSED);
        lv_obj_set_style_shadow_color(cards[i], lv_color_hex(colors[i]), LV_STATE_FOCUSED);
        lv_obj_set_style_shadow_width(cards[i], 12, LV_STATE_FOCUSED);
        lv_obj_t *disc = plain(cards[i], 9, 9, 27, 27);
        lv_obj_set_style_radius(disc, LV_RADIUS_CIRCLE, LV_PART_MAIN);
        lv_obj_set_style_bg_color(disc, lv_color_hex(colors[i]), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(disc, 28, LV_PART_MAIN);
        lv_obj_t *icon = label(disc, icons[i], 0, 0, 27, &lv_font_montserrat_14, colors[i]);
        lv_obj_set_style_text_align(icon, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN); lv_obj_center(icon);
        label(cards[i], names[i], 10, 41, 82, ui_font_14(), NAVY);
        label(cards[i], notes[i], 10, 62, 82, ui_font_12(), MUTED);
        bind(cards[i]); reveal(cards[i], 110 + i * 45);
    }
    s_focus_bar = plain(s_screen, 20, 139, 78, 3);
    lv_obj_set_style_radius(s_focus_bar, 2, LV_PART_MAIN);
    lv_obj_set_style_bg_color(s_focus_bar, lv_color_hex(s_last_mode ? GOLD : MINT), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(s_focus_bar, 180, LV_PART_MAIN);
    lv_group_focus_obj(cards[s_last_mode]);
    if (s_draft_valid) bind(button(s_screen, tr("Resume", "继续"), 208, 146, 48, 20, ACT_RESUME));
    bind(button(s_screen, tr("Home", "返回"), 262, 146, 46, 20, ACT_HOME));
    lv_obj_t *hint = label(s_screen, tr("Turn to choose / press to enter", "旋钮选择 · 按下确认"), 16, 148, s_draft_valid ? 186 : 240, ui_font_12(), MUTED);
    lv_label_set_long_mode(hint, LV_LABEL_LONG_DOT);
}

static void library(void)
{
    header(tr("Little inspirations", "灵感闪记"), tr("Newest first · stored on your Reppo", "最新的想法在前 · 本地保存"));
    s_rows_loaded = 0;
    for (unsigned i = 0; i < 3; ++i) {
        s_rows[i] = button(s_screen, NULL, 12, 46 + i * 32, 296, 29, ACT_ROW0 + i);
        lv_obj_set_style_bg_color(s_rows[i], lv_color_hex(0xDFEAF3), LV_STATE_FOCUSED);
        lv_obj_set_style_border_width(s_rows[i], 1, LV_STATE_FOCUSED);
        lv_obj_set_style_border_color(s_rows[i], lv_color_hex(0xADC5D8), LV_STATE_FOCUSED);
        char n[8]; snprintf(n, sizeof(n), "%02u", (s_start + i + 1) % 1000);
        label(s_rows[i], n, 9, 7, 24, ui_font_12(), GOLD);
        s_row_titles[i] = label(s_rows[i], tr("Opening...", "正在打开…"), 37, 5, 182, ui_font_14(), NAVY);
        lv_label_set_long_mode(s_row_titles[i], LV_LABEL_LONG_DOT); lv_obj_set_height(s_row_titles[i], 20);
        s_row_dates[i] = label(s_rows[i], "", 221, 8, 70, ui_font_12(), MUTED);
        bind(s_rows[i]); lv_obj_add_state(s_rows[i], LV_STATE_DISABLED);
        reveal(s_rows[i], 75 + i * 35);
    }
    bind(button(s_screen, tr("New", "新记"), 12, 145, 54, 20, ACT_IDEA));
    bind(button(s_screen, tr("Prev", "上页"), 72, 145, 54, 20, ACT_PREV));
    bind(button(s_screen, tr("Next", "下页"), 132, 145, 54, 20, ACT_NEXT));
    bind(button(s_screen, tr("Delete", "删除"), 192, 145, 54, 20, ACT_DELETE));
    bind(button(s_screen, tr("Back", "返回"), 252, 145, 56, 20, s_draft_valid ? ACT_BACK : ACT_HOME));
    request(IDEA_PAGE, s_start);
}

static void scroll_indicator(lv_event_t *e)
{
    (void)e;
    if (!s_scroll || !s_body || !s_thumb) return;
    lv_obj_update_layout(s_scroll);
    int height = lv_obj_get_height(s_body);
    int visible = lv_obj_get_height(s_scroll);
    int track = visible - 4;
    int thumb = height > visible ? track * visible / height : track;
    if (thumb < 10) thumb = 10;
    lv_obj_set_height(s_thumb, thumb);
    int y = height > visible ? lv_obj_get_scroll_y(s_scroll) * (track - thumb) / (height - visible) : 0;
    if (y < 0) y = 0;
    if (y > track - thumb) y = track - thumb;
    lv_obj_set_y(s_thumb, 2 + y);
}
static void reading_turn(lv_group_t *group, bool forward)
{
    if (group != s_group || !s_scroll || s_page != PAGE_DETAIL) return;
    idle_sleep_note_activity();
    lv_obj_scroll_by(s_scroll, 0, forward ? -36 : 36, LV_ANIM_ON);
}
static void detail_click_timeout(lv_timer_t *timer)
{
    (void)timer;
    if (s_click_armed) { s_click_armed = false; s_action = ACT_ORIGINAL; }
}
static void reading_click(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;
    if (s_click_armed && lv_tick_elaps(s_click_at) < 450) {
        s_click_armed = false; s_action = ACT_BACK;
    } else {
        s_click_armed = true; s_click_at = lv_tick_get(); lv_timer_reset(s_click_timer);
    }
}
static void reading_area(int y, int height)
{
    s_scroll = plain(s_screen, 16, y, 282, height);
    lv_obj_add_flag(s_scroll, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scroll_dir(s_scroll, LV_DIR_VER); lv_obj_set_scrollbar_mode(s_scroll, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_bottom(s_scroll, 4, LV_PART_MAIN);
    lv_obj_set_style_bg_color(s_scroll, lv_color_hex(WHITE), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(s_scroll, 255, LV_PART_MAIN);
    lv_obj_set_style_radius(s_scroll, 10, LV_PART_MAIN);
    lv_obj_set_style_clip_corner(s_scroll, true, LV_PART_MAIN);
    s_body = label(s_scroll, "", 10, 4, 258, ui_font_14(), INK);
    lv_label_set_long_mode(s_body, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_line_space(s_body, 5, LV_PART_MAIN);
    s_rail = plain(s_screen, 302, y + 1, 3, height - 2);
    lv_obj_set_style_bg_color(s_rail, lv_color_hex(0xDBE5EE), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(s_rail, 255, LV_PART_MAIN); lv_obj_set_style_radius(s_rail, 2, LV_PART_MAIN);
    s_thumb = plain(s_rail, 0, 2, 3, height - 6);
    lv_obj_set_style_bg_color(s_thumb, lv_color_hex(GOLD), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(s_thumb, 255, LV_PART_MAIN); lv_obj_set_style_radius(s_thumb, 2, LV_PART_MAIN);
    lv_obj_add_event_cb(s_scroll, scroll_indicator, LV_EVENT_SCROLL, NULL);
    for (unsigned i = 0; i < 5; ++i) {
        lv_obj_t *fade = plain(s_screen, 25, y + height - 10 + i * 2, 264, 2);
        lv_obj_set_style_bg_color(fade, lv_color_hex(WHITE), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(fade, 45 + i * 50, LV_PART_MAIN);
    }
}
static void render_detail(void)
{
    lv_label_set_text(s_heading, s_record.title);
    const char *time = s_record.time[0] ? s_record.time : tr("Time not set", "时间未校准");
    char meta[96]; snprintf(meta, sizeof(meta), "%s  ·  %s", time,
                           (s_original || !strcmp(s_record.text, s_record.original)) ? tr("Original words", "原始转写") : tr("Gently polished", "整理后的想法"));
    lv_label_set_text(s_caption, meta);
    lv_label_set_text(s_body, s_original ? s_record.original : s_record.text);
    lv_obj_scroll_to_y(s_scroll, 0, LV_ANIM_OFF); scroll_indicator(NULL);
    reveal(s_body, 0);
}
static void detail(void)
{
    header(tr("Opening inspiration", "正在打开灵感"), "");
    reading_area(49, 86);
    lv_obj_t *hit = button(s_screen, NULL, 0, 0, 320, 170, ACT_NONE);
    lv_obj_remove_style_all(hit); lv_obj_move_background(hit);
    lv_obj_add_event_cb(hit, reading_click, LV_EVENT_CLICKED, NULL); bind(hit);
    lv_group_set_wrap(s_group, false); lv_group_set_edge_cb(s_group, reading_turn);
    s_click_timer = lv_timer_create(detail_click_timeout, 450, NULL);
    label(s_screen, tr("Turn: scroll / tap: original / 2 taps: back", "旋钮翻阅 · 单击原话 · 双击返回"), 16, 146, 290, ui_font_12(), MUTED);
    if (s_preview_detail) render_detail();
    else request(IDEA_READ, s_selected);
}
static void preview(void)
{
    header(tr("Keep this spark?", "把这个想法留下？"),
           !strcmp(s_record.text, s_record.original) ? tr("Original words kept. Ready to save.", "原话已保留，可以直接保存") :
           tr("Read it once. Save it for later.", "读一遍，留给以后的自己"));
    lv_obj_t *card = plain(s_screen, 12, 47, 296, 85);
    lv_obj_set_style_radius(card, 12, LV_PART_MAIN);
    lv_obj_set_style_bg_color(card, lv_color_hex(WHITE), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(card, 245, LV_PART_MAIN);
    lv_obj_t *accent = plain(card, 10, 13, 3, 59);
    lv_obj_set_style_bg_color(accent, lv_color_hex(GOLD), LV_PART_MAIN); lv_obj_set_style_bg_opa(accent, 255, LV_PART_MAIN);
    lv_obj_t *title = label(card, s_record.title, 23, 9, 258, ui_font_16(), NAVY);
    lv_label_set_long_mode(title, LV_LABEL_LONG_DOT); lv_obj_set_height(title, 20);
    s_message = label(card, s_record.text, 23, 34, 258, ui_font_14(), INK);
    lv_obj_set_height(s_message, 43); lv_label_set_long_mode(s_message, LV_LABEL_LONG_DOT);
    reveal(card, 100);
    bind(button(s_screen, tr("Read", "全文"), 12, 143, 65, 22, ACT_READ_PREVIEW));
    bind(button(s_screen, tr("Save", "保存"), 83, 143, 72, 22, ACT_SAVE));
    bind(button(s_screen, tr("Again", "重说"), 161, 143, 72, 22, ACT_IDEA));
    bind(button(s_screen, tr("Back", "返回"), 239, 143, 69, 22, ACT_BACK));
    lv_group_focus_next(s_group); /* Save is the primary action. */
}
static void delete_confirm(void)
{
    header(tr("Delete this inspiration?", "删除这条灵感？"), tr("This removes it from this device", "确认后将从本机删除"));
    s_body = label(s_screen, tr("Opening...", "正在打开…"), 20, 67, 280, ui_font_16(), NAVY);
    s_detail_ready = false;
    s_message = label(s_screen, tr("Keep the ones you want to return to", "留下你还想回来看的念头"), 20, 101, 280, ui_font_12(), MUTED);
    bind(button(s_screen, tr("Keep", "保留"), 16, 141, 136, 24, ACT_BACK));
    bind(button(s_screen, tr("Delete", "删除"), 168, 141, 136, 24, ACT_DELETE_CONFIRM));
    request(IDEA_READ, s_selected);
}
static void build(page_t page)
{
    lv_obj_t *old = lv_scr_act();
    cancel_timers(); release_input(); ++s_token; s_need_request = false;
    s_page = page; s_detail_ready = false; s_action = ACT_NONE; s_saving = s_saved = false; s_original = false;
    s_scroll = s_body = s_thumb = s_rail = s_focus_bar = s_message = NULL;
    s_open = true;
    s_screen = lv_obj_create(NULL); lv_obj_remove_style_all(s_screen);
    lv_obj_set_size(s_screen, 320, 170); lv_obj_clear_flag(s_screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(s_screen, lv_color_hex(PAPER), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(s_screen, 255, LV_PART_MAIN);
    lv_obj_add_event_cb(s_screen, deleted, LV_EVENT_DELETE, NULL);
    /* Small warm accent, drawn with LVGL objects rather than a bitmap. */
    lv_obj_t *dot = plain(s_screen, 307, 12, 5, 5);
    lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(dot, lv_color_hex(GOLD), LV_PART_MAIN); lv_obj_set_style_bg_opa(dot, 200, LV_PART_MAIN);
    init_input();
    switch (page) {
    case PAGE_HUB: hub(); break;
    case PAGE_LIST: library(); break;
    case PAGE_PREVIEW: preview(); break;
    case PAGE_DETAIL: detail(); break;
    case PAGE_DELETE: delete_confirm(); break;
    }
    inspiration_ui_mark_guider_deleted(old);
    lv_scr_load_anim(s_screen, LV_SCR_LOAD_ANIM_FADE_ON, 160, 0, true);
}
void inspiration_ui_open_hub(void) { s_hub_req = true; }
void inspiration_ui_preview(const inspiration_record_t *r)
{ s_record = s_draft = *r; s_draft_valid = true; s_preview_detail = true; s_preview_req = true; }

static void request(inspiration_operation_t op, unsigned index)
{ s_operation = op; s_selected = index; s_need_request = true; }
static void saved_timeout(lv_timer_t *t)
{ (void)t; s_saved_timer = NULL; s_action = ACT_LIBRARY; }
static void apply_reply(void)
{
    s_count = s_reply.count;
    if (s_reply.status != INSPIRATION_OK) {
        const char *error = s_reply.status == INSPIRATION_FULL ? tr("Notebook full. Delete an old idea first.", "闪记已满，请先删除旧灵感") :
                            s_reply.status == INSPIRATION_NO_SPACE ? tr("Storage is nearly full. This idea is still here.", "存储空间不足，这条想法仍在这里") :
                            tr("Couldn't read or save. Please try again.", "读写失败，请重试");
        s_saving = false;
        if (s_caption) {
            lv_label_set_text(s_caption, error);
            lv_obj_set_style_text_color(s_caption, lv_color_hex(0xA65A5A), LV_PART_MAIN);
        }
        if (s_page == PAGE_LIST) {
            for (unsigned i = 0; i < 3; ++i) lv_obj_add_flag(s_rows[i], LV_OBJ_FLAG_HIDDEN);
            label(s_screen, tr("Try returning and opening again", "返回后重新打开试试"), 22, 80, 276, ui_font_14(), MUTED);
        }
        return;
    }
    if (s_reply.operation == IDEA_PAGE && s_page == PAGE_LIST) {
        s_rows_loaded = s_reply.rows;
        char caption[96];
        if (!s_count) {
            lv_label_set_text(s_caption, tr("Your first idea can start here", "第一条灵感，就从这里开始"));
            label(s_screen, tr("Nothing here yet\nTurn to New and catch a spark", "这里还很安静\n选择「新记」，留住第一个想法"), 24, 66, 270, ui_font_14(), MUTED);
        } else {
            snprintf(caption, sizeof(caption), "%u / %u  ·  %u %s", s_start / 3 + 1, (s_count + 2) / 3,
                     s_count, tr("ideas", "条灵感"));
            lv_label_set_text(s_caption, caption);
        }
        for (unsigned i = 0; i < 3; ++i) {
            if (i >= s_rows_loaded) { lv_obj_add_flag(s_rows[i], LV_OBJ_FLAG_HIDDEN); continue; }
            lv_obj_clear_state(s_rows[i], LV_STATE_DISABLED);
            lv_label_set_text(s_row_titles[i], s_reply.records[i].title);
            char stamp[6] = "";
            if (s_reply.records[i].time[0]) memcpy(stamp, s_reply.records[i].time + 5, 5);
            lv_label_set_text(s_row_dates[i], stamp);
            reveal(s_rows[i], i * 35);
        }
        if (s_rows_loaded) lv_group_focus_obj(s_rows[0]);
        else lv_group_focus_next(s_group);
    } else if (s_reply.operation == IDEA_READ && (s_page == PAGE_DETAIL || s_page == PAGE_DELETE)) {
        s_record = s_reply.records[0]; s_detail_ready = true;
        if (s_page == PAGE_DETAIL) render_detail();
        else lv_label_set_text(s_body, s_record.title);
    } else if (s_reply.operation == IDEA_SAVE && s_page == PAGE_PREVIEW) {
        s_saving = false; s_saved = true; s_draft_valid = false;
        lv_label_set_text(s_heading, tr("Spark safely kept", "这一闪，已经留下"));
        lv_label_set_text(s_caption, tr("Saved on your Reppo · available offline", "已存入本机 · 离线也能回看"));
        message(s_record.text); reveal(s_heading, 0);
        for (unsigned i = 0; i < lv_obj_get_child_cnt(s_screen); ++i) {
            lv_obj_t *child = lv_obj_get_child(s_screen, i);
            if (lv_obj_check_type(child, &lv_btn_class)) lv_obj_add_flag(child, LV_OBJ_FLAG_HIDDEN);
        }
        lv_obj_t *seal = plain(s_screen, 88, 143, 144, 22);
        lv_obj_set_style_radius(seal, 11, LV_PART_MAIN);
        lv_obj_set_style_bg_color(seal, lv_color_hex(MINT), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(seal, 35, LV_PART_MAIN);
        label(seal, LV_SYMBOL_OK, 10, 4, 20, &lv_font_montserrat_14, MINT);
        label(seal, tr("Safely kept", "已存入灵感本"), 34, 3, 105, ui_font_12(), NAVY);
        reveal(seal, 40);
        s_saved_timer = lv_timer_create(saved_timeout, 950, NULL); lv_timer_set_repeat_count(s_saved_timer, 1);
    } else if (s_reply.operation == IDEA_DELETE && s_page == PAGE_DELETE) {
        s_start = s_start < s_count ? s_start : (s_count ? (s_count - 1) / 3 * 3 : 0);
        build(PAGE_LIST);
    }
}

/* A second entry in the existing scrolling settings list makes offline review
 * available without knocking, including boards whose IMU is absent. */
static void settings_event(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) s_library_req = true;
    else if (lv_event_get_code(e) == LV_EVENT_DELETE) {
        if (lv_event_get_target(e) == s_settings_screen) s_settings_screen = NULL;
        if (lv_event_get_target(e) == s_settings_button) s_settings_button = NULL;
    }
}
static void settings_entry_poll(void)
{
    lv_obj_t *screen = guider_ui.screen_setting;
    if (!screen || !lv_obj_is_valid(screen) || lv_scr_act() != screen) return;
    bool zh = ui_lang_is_zh();
    if (s_settings_screen == screen && s_settings_button) {
        if (zh != s_settings_zh) {
            lv_label_set_text(lv_obj_get_child(s_settings_button, 0), tr("Notebook", "灵感闪记"));
            s_settings_zh = zh;
        }
        return;
    }
    lv_obj_t *btn = screen_setting_list_add_btn(tr("Notebook", "灵感闪记"));
    if (!btn) return;
    s_settings_screen = screen; s_settings_button = btn; s_settings_zh = zh;
    lv_obj_set_style_text_font(btn, ui_font_14(), LV_PART_MAIN);
    lv_obj_set_style_text_font(btn, ui_font_14(), LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(lv_obj_get_child(btn, 0), ui_font_14(), LV_PART_MAIN);
    lv_obj_add_event_cb(btn, settings_event, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(screen, settings_event, LV_EVENT_DELETE, NULL);
}

void inspiration_ui_poll(void)
{
    /* This LVGL port cannot interrupt a screen-load animation safely. Keep the
     * requested action until the incoming screen has finished loading. */
    bool transitioning = lv_disp_get_default()->scr_to_load != NULL;
    if (!transitioning) settings_entry_poll();
    if (!transitioning && (s_hub_req || s_preview_req || s_library_req)) {
        bool preview_requested = s_preview_req;
        bool library_requested = s_library_req;
        s_hub_req = s_preview_req = s_library_req = false;
        if (library_requested) { s_start = 0; s_preview_detail = false; }
        build(preview_requested ? PAGE_PREVIEW : (library_requested ? PAGE_LIST : PAGE_HUB));
        transitioning = true;
    }
    if (s_busy && inspiration_service_receive(&s_reply)) {
        s_busy = false;
        if (s_reply.token == s_token && s_open) apply_reply();
    }
    if (!s_open || !s_screen || !lv_obj_is_valid(s_screen)) return;
    if (s_saving || s_busy || s_need_request) idle_sleep_note_activity();
    if (transitioning) return;
    action_t act = s_action; s_action = ACT_NONE;
    switch (act) {
    case ACT_HOME: go_home(); break;
    case ACT_TODO: case ACT_IDEA:
        s_last_mode = act == ACT_IDEA; cancel_timers(); release_input(); s_open = false;
        ++s_token; s_need_request = false; voice_todo_start_mode(act == ACT_IDEA); break;
    case ACT_LIBRARY: s_start = 0; s_preview_detail = false; build(PAGE_LIST); break;
    case ACT_PREV: if (s_start >= 3) { s_start -= 3; build(PAGE_LIST); } break;
    case ACT_NEXT: if (s_start + 3 < s_count) { s_start += 3; build(PAGE_LIST); } break;
    case ACT_ROW0: case ACT_ROW1: case ACT_ROW2: {
        unsigned row = act - ACT_ROW0;
        if (row < s_rows_loaded) { s_selected = s_start + row; s_preview_detail = false; build(PAGE_DETAIL); }
        break;
    }
    case ACT_RESUME: if (s_draft_valid) { s_record = s_draft; s_preview_detail = true; build(PAGE_PREVIEW); } break;
    case ACT_READ_PREVIEW: s_preview_detail = true; build(PAGE_DETAIL); break;
    case ACT_ORIGINAL:
        if (s_page == PAGE_DETAIL && (s_preview_detail || s_detail_ready)) { s_original = !s_original; render_detail(); }
        break;
    case ACT_BACK:
        if (s_page == PAGE_DELETE) build(PAGE_LIST);
        else if (s_page == PAGE_DETAIL) build(s_preview_detail ? PAGE_PREVIEW : PAGE_LIST);
        else build(PAGE_HUB);
        break;
    case ACT_DELETE: if (s_page == PAGE_LIST && s_count && s_rows_loaded) build(PAGE_DELETE); break;
    case ACT_DELETE_CONFIRM:
        if (s_detail_ready) { s_saving = true; message(tr("Deleting...", "正在删除…")); request(IDEA_DELETE, s_selected); } break;
    case ACT_SAVE:
        s_saving = true; lv_label_set_text(s_caption, tr("Saving to your Reppo...", "正在保存到本机…")); request(IDEA_SAVE, 0); break;
    default: break;
    }
    if (s_need_request && !s_busy && s_open) {
        memset(&s_request, 0, sizeof(s_request));
        s_request.token = s_token; s_request.operation = s_operation; s_request.index = s_selected;
        if (s_operation == IDEA_SAVE) s_request.record = s_record;
        if (inspiration_service_request(&s_request)) { s_busy = true; s_need_request = false; }
        else {
            s_saving = false; s_need_request = false;
            if (s_caption) lv_label_set_text(s_caption, tr("Storage unavailable. Try again later.", "存储暂不可用，请稍后重试"));
        }
    }
}
