#include "lvgl.h"
#include "gui_guider.h"
#include "inspiration_service.h"
#include "ui_locale.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "inspiration_ui.c"

lv_ui guider_ui;
lv_group_t *g;
extern const lv_font_t ui_todo_cjk_font;
static lv_font_t font12, font14, font16;
static bool chinese = true;
const char *ui_tr(const char *en, const char *zh) { return chinese ? zh : en; }
const lv_font_t *ui_font_12(void) { return &font12; }
const lv_font_t *ui_font_14(void) { return &font14; }
const lv_font_t *ui_font_16(void) { return &font16; }
void idle_sleep_note_activity(void) {}
void clear_focus_box(lv_obj_t *obj) { lv_obj_set_style_outline_width(obj, 0, LV_STATE_FOCUSED); }
void group_focus_obj(lv_obj_t *obj) { lv_group_focus_obj(obj); }
bool ui_lang_is_zh(void) { return chinese; }
lv_obj_t *screen_setting_list_add_btn(const char *text)
{
    lv_obj_t *btn = lv_btn_create(guider_ui.screen_setting);
    lv_obj_t *l = lv_label_create(btn); lv_label_set_text(l, text); return btn;
}
void setup_scr_screen_wificfg(lv_ui *ui) { ui->screen_wificfg = lv_obj_create(NULL); }
void test_voice_show(bool idea, bool wifi);
void test_voice_state(int state);
void test_voice_feedback(unsigned ms, unsigned level);
static lv_color_t framebuffer[320 * 170], buffer[320 * 30];
static uint32_t key;
static bool key_down;
static unsigned voice_starts;
static bool voice_mode;
static const char *snapshots;

static void main_delete(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_DELETE && lv_event_get_target(e) == guider_ui.screen_main) {
        guider_ui.screen_main = NULL; guider_ui.screen_main_del = true;
    }
}
void setup_scr_screen_main(lv_ui *ui)
{
    ui->screen_main = lv_obj_create(NULL);
    lv_obj_add_event_cb(ui->screen_main, main_delete, LV_EVENT_DELETE, NULL);
    ui->screen_main_del = false;
}
void ui_load_scr_animation(lv_ui *ui, lv_obj_t **next, bool create, bool *old_del,
                          ui_setup_scr_t setup, lv_scr_load_anim_t anim,
                          uint32_t time, uint32_t delay, bool clean, bool auto_del)
{
    if (clean) lv_obj_clean(lv_scr_act());
    if (create) setup(ui);
    lv_scr_load_anim(*next, anim, time, delay, auto_del); *old_del = auto_del;
}
void voice_todo_start_mode(bool idea)
{
    ++voice_starts; voice_mode = idea;
    lv_obj_t *capture = lv_obj_create(NULL);
    lv_scr_load_anim(capture, LV_SCR_LOAD_ANIM_FADE_ON, 160, 0, true);
}
static void flush(lv_disp_drv_t *d, const lv_area_t *area, lv_color_t *pixels)
{
    for (int y = area->y1; y <= area->y2; ++y) for (int x = area->x1; x <= area->x2; ++x) {
        if (x >= 0 && x < 320 && y >= 0 && y < 170) framebuffer[y * 320 + x] = *pixels;
        ++pixels;
    }
    lv_disp_flush_ready(d);
}
static void read_key(lv_indev_drv_t *d, lv_indev_data_t *data)
{ (void)d; data->key = key; data->state = key_down ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL; }
static void step(unsigned ms)
{
    for (unsigned i = 0; i < ms; i += 10) { lv_tick_inc(10); lv_timer_handler(); inspiration_ui_poll(); }
}
static void press(uint32_t value)
{ key = value; key_down = true; step(50); key_down = false; step(60); }
static lv_obj_t *find_button(lv_obj_t *root, const char *text)
{
    if (lv_obj_check_type(root, &lv_btn_class)) {
        for (unsigned i = 0; i < lv_obj_get_child_cnt(root); ++i) {
            lv_obj_t *child = lv_obj_get_child(root, i);
            if (lv_obj_check_type(child, &lv_label_class) && !strcmp(lv_label_get_text(child), text)) return root;
        }
    }
    for (unsigned i = 0; i < lv_obj_get_child_cnt(root); ++i) {
        lv_obj_t *found = find_button(lv_obj_get_child(root, i), text); if (found) return found;
    }
    return NULL;
}
static void click(const char *text)
{
    lv_obj_t *b = find_button(s_screen, text); assert(b);
    lv_group_focus_obj(b); press(LV_KEY_ENTER); step(200);
}
static void snapshot(const char *name)
{
    if (!snapshots) return;
    lv_refr_now(NULL);
    char path[512]; snprintf(path, sizeof(path), "%s/%s.ppm", snapshots, name);
    FILE *f = fopen(path, "wb"); assert(f); fprintf(f, "P6\n320 170\n255\n");
    for (unsigned i = 0; i < 320 * 170; ++i) {
        uint32_t rgb = lv_color_to32(framebuffer[i]);
        uint8_t p[3] = {rgb >> 16, rgb >> 8, rgb}; fwrite(p, 1, 3, f);
    }
    fclose(f);
}
static unsigned timer_count(void)
{ unsigned count = 0; lv_timer_t *t = NULL; while ((t = lv_timer_get_next(t))) ++count; return count; }

int main(int argc, char **argv)
{
    assert(argc >= 2); snapshots = argc > 2 ? argv[2] : NULL;
    lv_init();
    font12 = lv_font_SourceHanSansSC_Medium_12; font12.fallback = &ui_todo_cjk_font;
    font14 = lv_font_SourceHanSansSC_Medium_14; font14.fallback = &ui_todo_cjk_font;
    font16 = lv_font_SourceHanSansSC_Medium_16; font16.fallback = &ui_todo_cjk_font;
    lv_disp_draw_buf_t draw; lv_disp_draw_buf_init(&draw, buffer, NULL, 320 * 30);
    lv_disp_drv_t disp; lv_disp_drv_init(&disp); disp.hor_res = 320; disp.ver_res = 170;
    disp.flush_cb = flush; disp.draw_buf = &draw; lv_disp_drv_register(&disp);
    lv_indev_drv_t in; lv_indev_drv_init(&in); in.type = LV_INDEV_TYPE_KEYPAD; in.read_cb = read_key;
    lv_indev_drv_register(&in); g = lv_group_create();
    setup_scr_screen_main(&guider_ui); lv_scr_load(guider_ui.screen_main);
    inspiration_service_init_at(argv[1]);
    inspiration_record_t record = {0};
    strcpy(record.time, "2026-10-12 14:35");
    inspiration_text_copy(record.original, sizeof(record.original), "我刚想到，可以让这个小屏幕像一本随手翻开的笔记。那些突然出现的想法，不用急着变成任务，也能安静地留在这里。\n以后再看，也许会找到新的方向。");
    inspiration_text_copy(record.text, sizeof(record.text), "让小屏幕像一本随手翻开的笔记。突然出现的想法不必马上变成任务，也能安静地留下。\n\n把原话与整理后的表达放在一起，给未来的自己留一点线索。\n\n以后翻看，也许会找到新的方向。");
    for (unsigned i = 0; i < 7; ++i) {
        const char *titles[] = {"给想法一个安静的家", "用光记录专注的节奏", "让日常有一点惊喜", "把阅读变成小小的旅程", "给桌面留一片绿", "做一个随手打开的笔记本", "留住突然出现的灵感"};
        inspiration_text_copy(record.title, sizeof(record.title), titles[i]);
        assert(inspiration_storage_add(&record) == INSPIRATION_OK);
    }
    inspiration_ui_open_hub();
    if (snapshots) {
        for (unsigned frame = 0; frame < 20; ++frame) {
            step(40); char name[48]; snprintf(name, sizeof(name), "anim-hub-%02u", frame); snapshot(name);
        }
    }
    step(700); snapshot("01-hub-zh");
    assert(inspiration_ui_is_open());
    press(LV_KEY_NEXT); step(220); snapshot("01b-hub-idea-zh");
    press(LV_KEY_ENTER); step(200);
    assert(voice_starts == 1 && voice_mode);
    inspiration_ui_open_hub(); step(700);
    click("返回"); assert(!inspiration_ui_is_open());
    inspiration_ui_open_hub(); step(700);
    /* Real keypad navigation to the notebook card. */
    press(LV_KEY_NEXT); press(LV_KEY_ENTER); step(400);
    assert(s_page == PAGE_LIST && s_count == 7 && s_rows_loaded == 3);
    snapshot("02-library-zh");
    press(LV_KEY_ENTER); step(400);
    assert(s_page == PAGE_DETAIL && !s_preview_detail && s_detail_ready);
    snapshot("03-detail-zh");
    press(LV_KEY_NEXT); step(250); assert(lv_obj_get_scroll_y(s_scroll) > 0);
    snapshot("04-detail-scroll-zh");
    press(LV_KEY_ENTER); step(500); assert(s_original);
    snapshot("05-original-zh");
    press(LV_KEY_ENTER); press(LV_KEY_ENTER); step(300); assert(s_page == PAGE_LIST);
    click("下页"); assert(s_start == 3 && s_rows_loaded == 3);
    click("删除"); assert(s_page == PAGE_DELETE && s_detail_ready);
    snapshot("06-delete-zh");
    click("保留"); assert(inspiration_storage_count() == 7);
    click("删除"); click("删除"); assert(inspiration_storage_count() == 6 && s_page == PAGE_LIST);
    inspiration_ui_preview(&record); step(500); snapshot("07-preview-zh");
    click("全文"); assert(s_page == PAGE_DETAIL && s_preview_detail);
    press(LV_KEY_ENTER); press(LV_KEY_ENTER); step(300); assert(s_page == PAGE_PREVIEW);
    click("保存"); assert(s_saved && inspiration_storage_count() == 7);
    snapshot("08-saved-zh"); step(1000); assert(s_page == PAGE_LIST);
    click("返回");
    unsigned timers = timer_count();
    for (unsigned i = 0; i < 30; ++i) {
        inspiration_ui_open_hub(); step(250);
        s_action = ACT_LIBRARY; inspiration_ui_poll();
        /* Leave before an asynchronous reply arrives: stale data cannot touch the new page. */
        s_action = ACT_HOME; inspiration_ui_poll(); step(450);
        assert(!s_open && !s_group && timer_count() == timers);
    }
    chinese = false; inspiration_ui_open_hub(); step(700); snapshot("09-hub-en");
    s_action = ACT_LIBRARY; inspiration_ui_poll(); step(500); snapshot("10-library-en");
    inspiration_service_init_at("/no-such-directory/reppo/ideas.dat");
    inspiration_ui_preview(&record); step(300); click("Save");
    assert(!s_saved && !s_saving && !strcmp(s_record.text, record.text));
    snapshot("11-save-error-en");
    click("Back"); assert(s_page == PAGE_HUB && s_draft_valid);
    click("Resume"); assert(s_page == PAGE_PREVIEW && !strcmp(s_record.text, record.text));
    chinese = true;
    s_action = ACT_HOME; inspiration_ui_poll(); step(300);
    test_voice_show(true, true); step(900); snapshot("12-idea-ready-zh");
    test_voice_state(2);
    if (snapshots) {
        static const unsigned levels[] = {200, 700, 1600, 900, 300, 1100, 1800, 600};
        for (unsigned frame = 0; frame < 24; ++frame) {
            test_voice_feedback(frame * 240, levels[frame % 8]); step(60);
            char name[48]; snprintf(name, sizeof(name), "anim-record-%02u", frame); snapshot(name);
        }
    }
    test_voice_feedback(6500, 1100); step(350); snapshot("13-recording-zh");
    test_voice_state(3); step(450); snapshot("14-polishing-zh");
    /* Cleanly close capture before another screen transition. */
    inspiration_ui_open_hub(); step(500); s_action = ACT_HOME; inspiration_ui_poll(); step(300);
    test_voice_show(true, false); step(900); snapshot("15-wifi-required-zh");
    inspiration_ui_open_hub(); step(500); s_action = ACT_HOME; inspiration_ui_poll(); step(300);
    test_voice_show(false, true); step(900); snapshot("16-todo-ready-zh");
    inspiration_ui_open_hub(); step(500); s_action = ACT_HOME; inspiration_ui_poll(); step(300);
    guider_ui.screen_setting = lv_obj_create(NULL); lv_scr_load(guider_ui.screen_setting);
    inspiration_ui_poll(); assert(s_settings_button);
    lv_obj_t *settings_button = s_settings_button; inspiration_ui_poll(); assert(settings_button == s_settings_button);
    lv_event_send(settings_button, LV_EVENT_CLICKED, NULL); step(500); assert(s_page == PAGE_LIST);
    puts("inspiration UI: real LVGL input, pages, scrolling, original, save, delete, stale replies and lifecycle passed");
    return 0;
}
