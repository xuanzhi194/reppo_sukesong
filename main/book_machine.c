/**
 * screen_book ebook reader (lives in main, not ui_custom/custom).
 * Strong symbols override the Guider-safe weak stubs in custom.c.
 *
 * Book body is /spiffs/book.txt (built from data/book.txt). App import
 * overwrites that file and calls book_machine_reload().
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "lvgl.h"
#include "gui_guider.h"
#include "custom.h"
#include "book_machine.h"
#ifdef ESP_PLATFORM
#include "nvs.h"
#include "esp_log.h"
extern const uint8_t book_txt_start[] asm("_binary_book_txt_start");
extern const uint8_t book_txt_end[] asm("_binary_book_txt_end");
#endif

#define BOOK_NVS_NS            "ebook"
#define BOOK_NVS_KEY           "page"
#define BOOK_SPIFFS_PATH       "/spiffs/book.txt"
#define BOOK_MAX_PARTS         32
#define BOOK_MAX_PAGES         96
#define BOOK_PAGE_CHARS        400
#define BOOK_NAME_LEN          48
#define BOOK_PART_NAME_LEN     48
#define BOOK_LINE_MAX          512
#define BOOK_DOUBLE_CLICK_MS   750
#define BOOK_TAG               "book"

/* Rest positions from setup_scr_screen_book. */
#define BOOK_X_AUTHOR          157
#define BOOK_Y_AUTHOR          10
#define BOOK_X_CONTENT         5
#define BOOK_Y_CONTENT         0
#define BOOK_X_PAGEINDEX       235
#define BOOK_Y_PAGEINDEX       156
#define BOOK_X_PARTNAME        12
#define BOOK_Y_PARTNAME        111
#define BOOK_X_PARTINDEX       12
#define BOOK_Y_PARTINDEX       82
#define BOOK_X_BOOKNAME        12
#define BOOK_Y_BOOKNAME        10
#define BOOK_X_TOC_BG          184
#define BOOK_Y_TOC_BG          0
#define BOOK_X_TOC_LIST        195
#define BOOK_Y_TOC_LIST        26
#define BOOK_X_TOC_BACK        199
#define BOOK_Y_TOC_BACK        7

typedef enum {
    BOOK_ANIM_NONE = 0,
    BOOK_ANIM_ENTER,
    BOOK_ANIM_NEXT,
    BOOK_ANIM_PREV,
    BOOK_ANIM_CHAPTER,
} book_anim_t;

typedef enum {
    BOOK_EXIT_LABEL = 0,
    BOOK_EXIT_TOC,
} book_exit_kind_t;

typedef enum {
    BOOK_PAGE_CHAPTER = 0,
    BOOK_PAGE_CONTENT,
} book_page_kind_t;

typedef struct {
    book_page_kind_t kind;
    uint8_t part;
    const char *text;
} book_page_t;

typedef struct {
    const char *name;
    const char *author;
    const char *const *parts;
    uint8_t part_count;
    const book_page_t *pages;
    uint16_t page_count;
} book_info_t;

static const char *const s_fallback_parts[] = {
    "Part 1",
};

static const book_page_t s_fallback_pages[] = {
    {BOOK_PAGE_CHAPTER, 0, NULL},
    {BOOK_PAGE_CONTENT, 0,
     "Book file missing. Put book.txt on /spiffs, then call book_machine_reload()."},
};

static char s_title[BOOK_NAME_LEN];
static char s_author_disp[BOOK_NAME_LEN + 4];
static char s_part_names[BOOK_MAX_PARTS][BOOK_PART_NAME_LEN];
static char s_page_texts[BOOK_MAX_PAGES][BOOK_PAGE_CHARS];
static const char *s_part_ptrs[BOOK_MAX_PARTS];
static book_page_t s_pages[BOOK_MAX_PAGES];
static book_info_t s_loaded;

static struct {
    const book_info_t *book;
    lv_obj_t *bound_scr;
    lv_obj_t *part_btns[BOOK_MAX_PARTS];
    lv_timer_t *click_timer;
    uint16_t page;
    uint8_t part_btn_count;
    bool events_bound;
    bool list_built;
    bool toc_open;
    bool leaving;
    bool alive;
    bool page_loaded;
    bool toc_rest_cached;
    bool content_draw_bound;
    const char *content_src;
    lv_coord_t toc_bg_x;
    lv_coord_t toc_bg_y;
    lv_coord_t toc_list_x;
    lv_coord_t toc_list_y;
    lv_coord_t toc_back_x;
    lv_coord_t toc_back_y;
} s_book;

static void book_reading_group_bind(void);
static void book_toc_group_bind(void);
static void book_render_page(book_anim_t anim);
static void book_toc_open(void);
static void book_toc_close(bool jump_chapter);
static void book_leave_to_main_async(void *unused);
static void book_anim_stop(lv_obj_t *obj);
static void book_snap_rest(lv_obj_t *obj, lv_coord_t x, lv_coord_t y);

static bool book_obj_ok(lv_obj_t *obj)
{
    return obj != NULL && lv_obj_is_valid(obj);
}

static bool book_scr_ok(void)
{
    if (guider_ui.screen_book != NULL &&
        !lv_obj_is_valid(guider_ui.screen_book)) {
        guider_ui.screen_book = NULL;
        return false;
    }
    return book_obj_ok(guider_ui.screen_book);
}

static const book_info_t *book_active(void)
{
    if (s_loaded.page_count > 0) {
        return &s_loaded;
    }
    return (s_book.book != NULL) ? s_book.book : &s_loaded;
}

static void book_copy_field(char *dst, size_t dst_sz, const char *src)
{
    size_t n = 0;

    if (dst == NULL || dst_sz == 0) {
        return;
    }
    if (src == NULL) {
        dst[0] = '\0';
        return;
    }
    while (*src == ' ' || *src == '\t') {
        src++;
    }
    while (src[n] != '\0' && src[n] != '\r' && (n + 1) < dst_sz) {
        n++;
    }
    while (n > 0 && (src[n - 1] == ' ' || src[n - 1] == '\t')) {
        n--;
    }
    memcpy(dst, src, n);
    dst[n] = '\0';
}

static void book_page_append(char *dst, size_t dst_sz, const char *line)
{
    size_t len;

    if (dst == NULL || dst_sz == 0 || line == NULL) {
        return;
    }
    while (*line == ' ' || *line == '\t') {
        line++;
    }
    if (*line == '\0') {
        return;
    }
    len = strlen(dst);
    if (len > 0 && (len + 1) < dst_sz) {
        dst[len++] = ' ';
        dst[len] = '\0';
    }
    if (len >= dst_sz - 1) {
        return;
    }
    {
        size_t room = dst_sz - 1 - len;
        size_t copy = strlen(line);

        if (copy > room) {
            copy = room;
        }
        memcpy(dst + len, line, copy);
        dst[len + copy] = '\0';
    }
}

static bool book_flush_page(char *acc, int part, uint16_t *page_count)
{
    if (acc == NULL || acc[0] == '\0') {
        return true;
    }
    if (page_count == NULL || *page_count >= BOOK_MAX_PAGES || part < 0) {
        acc[0] = '\0';
        return false;
    }
    strncpy(s_page_texts[*page_count], acc, BOOK_PAGE_CHARS - 1);
    s_page_texts[*page_count][BOOK_PAGE_CHARS - 1] = '\0';
    s_pages[*page_count].kind = BOOK_PAGE_CONTENT;
    s_pages[*page_count].part = (uint8_t)part;
    s_pages[*page_count].text = s_page_texts[*page_count];
    (*page_count)++;
    acc[0] = '\0';
    return true;
}

static bool book_add_part(const char *name, int *part,
                          uint16_t *page_count, uint8_t *part_count)
{
    if (part == NULL || page_count == NULL || part_count == NULL) {
        return false;
    }
    if (*part_count >= BOOK_MAX_PARTS || *page_count >= BOOK_MAX_PAGES) {
        return false;
    }
    book_copy_field(s_part_names[*part_count], BOOK_PART_NAME_LEN, name);
    if (s_part_names[*part_count][0] == '\0') {
        snprintf(s_part_names[*part_count], BOOK_PART_NAME_LEN,
                 "Part %u", (unsigned)(*part_count + 1));
    }
    *part = (int)*part_count;
    s_pages[*page_count].kind = BOOK_PAGE_CHAPTER;
    s_pages[*page_count].part = (uint8_t)*part;
    s_pages[*page_count].text = NULL;
    (*page_count)++;
    (*part_count)++;
    return true;
}

static void book_bind_loaded(uint8_t parts, uint16_t pages)
{
    uint8_t i;

    for (i = 0; i < parts; i++) {
        s_part_ptrs[i] = s_part_names[i];
    }
    if (s_title[0] == '\0') {
        strncpy(s_title, "Untitled", BOOK_NAME_LEN - 1);
    }
    if (s_author_disp[0] == '\0') {
        strncpy(s_author_disp, "- Unknown", sizeof(s_author_disp) - 1);
    }
    s_loaded.name = s_title;
    s_loaded.author = s_author_disp;
    s_loaded.parts = s_part_ptrs;
    s_loaded.part_count = parts;
    s_loaded.pages = s_pages;
    s_loaded.page_count = pages;
}

static void book_use_fallback(void)
{
    s_loaded.name = "Siddhartha";
    s_loaded.author = "- Hermann Hesse";
    s_loaded.parts = s_fallback_parts;
    s_loaded.part_count = 1;
    s_loaded.pages = s_fallback_pages;
    s_loaded.page_count = 2;
    s_book.book = &s_loaded;
}

static bool book_parse_text(char *text)
{
    char line[BOOK_LINE_MAX];
    char page_acc[BOOK_PAGE_CHARS];
    char author[BOOK_NAME_LEN];
    uint8_t part_count = 0;
    uint16_t page_count = 0;
    int part = -1;
    bool in_page = false;
    char *p = text;

    if (p == NULL) {
        return false;
    }
    memset(s_title, 0, sizeof(s_title));
    memset(s_author_disp, 0, sizeof(s_author_disp));
    memset(s_part_names, 0, sizeof(s_part_names));
    memset(s_page_texts, 0, sizeof(s_page_texts));
    memset(s_pages, 0, sizeof(s_pages));
    page_acc[0] = '\0';
    author[0] = '\0';

    while (p != NULL && *p != '\0') {
        char *nl = strchr(p, '\n');
        size_t n;

        if (nl != NULL) {
            n = (size_t)(nl - p);
            if (n >= sizeof(line)) {
                n = sizeof(line) - 1;
            }
            memcpy(line, p, n);
            line[n] = '\0';
            p = nl + 1;
        } else {
            n = strlen(p);
            if (n >= sizeof(line)) {
                n = sizeof(line) - 1;
            }
            memcpy(line, p, n);
            line[n] = '\0';
            p = NULL;
        }
        while (n > 0 && (line[n - 1] == '\r' || line[n - 1] == ' ')) {
            line[--n] = '\0';
        }
        if (line[0] == '@') {
            if (!book_flush_page(page_acc, part, &page_count)) {
                return false;
            }
            in_page = false;
            if (strncmp(line, "@title", 6) == 0) {
                book_copy_field(s_title, sizeof(s_title), line + 6);
            } else if (strncmp(line, "@author", 7) == 0) {
                book_copy_field(author, sizeof(author), line + 7);
                if (author[0] == '-') {
                    strncpy(s_author_disp, author, sizeof(s_author_disp) - 1);
                } else if (author[0] != '\0') {
                    snprintf(s_author_disp, sizeof(s_author_disp), "- %s", author);
                }
            } else if (strncmp(line, "@part", 5) == 0) {
                if (!book_add_part(line + 5, &part, &page_count, &part_count)) {
                    return false;
                }
            } else if (strncmp(line, "@page", 5) == 0) {
                if (part < 0 &&
                    !book_add_part("Part 1", &part, &page_count, &part_count)) {
                    return false;
                }
                in_page = true;
                page_acc[0] = '\0';
            }
            continue;
        }
        if (!in_page || line[0] == '#') {
            continue;
        }
        book_page_append(page_acc, sizeof(page_acc), line);
    }
    if (!book_flush_page(page_acc, part, &page_count)) {
        return false;
    }
    if (page_count == 0 || part_count == 0) {
        return false;
    }
    book_bind_loaded(part_count, page_count);
    s_book.book = &s_loaded;
    return true;
}

static bool book_load_from_mem(const uint8_t *data, size_t len)
{
    char *buf;
    bool ok;

    if (data == NULL || len == 0) {
        return false;
    }
    buf = (char *)malloc(len + 1);
    if (buf == NULL) {
        return false;
    }
    memcpy(buf, data, len);
    buf[len] = '\0';
    ok = book_parse_text(buf);
    free(buf);
    return ok;
}

static bool book_load_from_path(const char *path)
{
    FILE *f;
    long sz;
    char *buf;
    bool ok;

    f = fopen(path, "rb");
    if (f == NULL) {
        return false;
    }
    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        return false;
    }
    sz = ftell(f);
    if (sz <= 0 || sz > (80 * 1024)) {
        fclose(f);
        return false;
    }
    rewind(f);
    buf = (char *)malloc((size_t)sz + 1);
    if (buf == NULL) {
        fclose(f);
        return false;
    }
    if (fread(buf, 1, (size_t)sz, f) != (size_t)sz) {
        free(buf);
        fclose(f);
        return false;
    }
    fclose(f);
    buf[sz] = '\0';
    ok = book_parse_text(buf);
    free(buf);
    return ok;
}

static void book_seed_spiffs_from_embed(void)
{
#ifdef ESP_PLATFORM
    FILE *f;
    size_t n;
    size_t wrote;

    f = fopen(BOOK_SPIFFS_PATH, "rb");
    if (f != NULL) {
        fclose(f);
        return;
    }
    n = (size_t)(book_txt_end - book_txt_start);
    f = fopen(BOOK_SPIFFS_PATH, "wb");
    if (f == NULL) {
        ESP_LOGW(BOOK_TAG, "cannot create %s", BOOK_SPIFFS_PATH);
        return;
    }
    wrote = fwrite(book_txt_start, 1, n, f);
    fclose(f);
    if (wrote != n) {
        ESP_LOGW(BOOK_TAG, "short write %s", BOOK_SPIFFS_PATH);
    }
#endif
}

static bool book_load_spiffs(void)
{
#ifdef ESP_PLATFORM
    size_t embed_len = (size_t)(book_txt_end - book_txt_start);

    book_seed_spiffs_from_embed();
    if (book_load_from_path(BOOK_SPIFFS_PATH)) {
        ESP_LOGI(BOOK_TAG, "loaded %s (%u parts, %u pages)",
                 BOOK_SPIFFS_PATH,
                 (unsigned)s_loaded.part_count,
                 (unsigned)s_loaded.page_count);
        return true;
    }
    if (book_load_from_mem(book_txt_start, embed_len)) {
        ESP_LOGI(BOOK_TAG, "loaded embedded book.txt (%u parts, %u pages)",
                 (unsigned)s_loaded.part_count,
                 (unsigned)s_loaded.page_count);
        return true;
    }
    ESP_LOGW(BOOK_TAG, "book.txt missing in SPIFFS and embed");
    return false;
#else
    return false;
#endif
}

static void book_clear_group_hooks(void)
{
    if (g != NULL) {
        lv_group_set_edge_cb(g, NULL);
        lv_group_set_wrap(g, true);
    }
}

static void book_keypad_group_reset(void)
{
    if (g == NULL) {
        g = lv_group_create();
    } else {
        lv_group_remove_all_objs(g);
        lv_group_set_edge_cb(g, NULL);
        lv_group_set_wrap(g, true);
    }
    lv_group_set_default(g);
    lv_indev_t *indev = NULL;
    while ((indev = lv_indev_get_next(indev))) {
        if (lv_indev_get_type(indev) == LV_INDEV_TYPE_KEYPAD) {
            lv_indev_set_group(indev, g);
        }
    }
}

static void book_clear_focus_box(lv_obj_t *obj)
{
    if (!book_obj_ok(obj)) {
        return;
    }
    lv_obj_set_style_outline_width(obj, 0, LV_STATE_FOCUSED);
    lv_obj_set_style_outline_width(obj, 0, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_border_width(obj, 0, LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(obj, 0, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_shadow_width(obj, 0, LV_STATE_FOCUSED);
}

static void book_set_hidden(lv_obj_t *obj, bool hidden)
{
    if (!book_obj_ok(obj)) {
        return;
    }
    if (hidden) {
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
}

static void book_set_toc_widget(lv_obj_t *obj, bool show)
{
    if (!book_obj_ok(obj)) {
        return;
    }
    if (show) {
        lv_obj_clear_state(obj, LV_STATE_DISABLED);
        lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_state(obj, LV_STATE_DISABLED);
    }
}

static void book_anim_x_exec(void *var, int32_t v)
{
    lv_obj_set_x((lv_obj_t *)var, (lv_coord_t)v);
}

static void book_anim_y_exec(void *var, int32_t v)
{
    lv_obj_set_y((lv_obj_t *)var, (lv_coord_t)v);
}

static void book_anim_opa_exec(void *var, int32_t v)
{
    lv_obj_set_style_opa((lv_obj_t *)var, (lv_opa_t)v,
                         LV_PART_MAIN | LV_STATE_DEFAULT);
}

static void book_anim_stop(lv_obj_t *obj)
{
    if (book_obj_ok(obj)) {
        lv_anim_del(obj, NULL);
    }
}

static void book_snap_rest(lv_obj_t *obj, lv_coord_t x, lv_coord_t y)
{
    if (!book_obj_ok(obj)) {
        return;
    }
    book_anim_stop(obj);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_style_opa(obj, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
}

static void book_cache_toc_rest(void)
{
    if (s_book.toc_rest_cached) {
        return;
    }
    if (book_obj_ok(guider_ui.screen_book_img_content_bg)) {
        s_book.toc_bg_x = lv_obj_get_x(guider_ui.screen_book_img_content_bg);
        s_book.toc_bg_y = lv_obj_get_y(guider_ui.screen_book_img_content_bg);
    } else {
        s_book.toc_bg_x = BOOK_X_TOC_BG;
        s_book.toc_bg_y = BOOK_Y_TOC_BG;
    }
    if (book_obj_ok(guider_ui.screen_book_list_part)) {
        s_book.toc_list_x = lv_obj_get_x(guider_ui.screen_book_list_part);
        s_book.toc_list_y = lv_obj_get_y(guider_ui.screen_book_list_part);
    } else {
        s_book.toc_list_x = BOOK_X_TOC_LIST;
        s_book.toc_list_y = BOOK_Y_TOC_LIST;
    }
    if (book_obj_ok(guider_ui.screen_book_btn_back)) {
        s_book.toc_back_x = lv_obj_get_x(guider_ui.screen_book_btn_back);
        s_book.toc_back_y = lv_obj_get_y(guider_ui.screen_book_btn_back);
    } else {
        s_book.toc_back_x = BOOK_X_TOC_BACK;
        s_book.toc_back_y = BOOK_Y_TOC_BACK;
    }
    s_book.toc_rest_cached = true;
}

static void book_rest_of(lv_obj_t *obj, lv_coord_t *x, lv_coord_t *y)
{
    if (obj == guider_ui.screen_book_label_author) {
        *x = BOOK_X_AUTHOR;
        *y = BOOK_Y_AUTHOR;
    } else if (obj == guider_ui.screen_book_label_content) {
        *x = BOOK_X_CONTENT;
        *y = BOOK_Y_CONTENT;
    } else if (obj == guider_ui.screen_book_label_pageindex) {
        *x = BOOK_X_PAGEINDEX;
        *y = BOOK_Y_PAGEINDEX;
    } else if (obj == guider_ui.screen_book_label_partname) {
        *x = BOOK_X_PARTNAME;
        *y = BOOK_Y_PARTNAME;
    } else if (obj == guider_ui.screen_book_label_partindex) {
        *x = BOOK_X_PARTINDEX;
        *y = BOOK_Y_PARTINDEX;
    } else if (obj == guider_ui.screen_book_label_bookname) {
        *x = BOOK_X_BOOKNAME;
        *y = BOOK_Y_BOOKNAME;
    } else if (obj == guider_ui.screen_book_img_content_bg) {
        book_cache_toc_rest();
        *x = s_book.toc_bg_x;
        *y = s_book.toc_bg_y;
    } else if (obj == guider_ui.screen_book_list_part) {
        book_cache_toc_rest();
        *x = s_book.toc_list_x;
        *y = s_book.toc_list_y;
    } else {
        book_cache_toc_rest();
        *x = s_book.toc_back_x;
        *y = s_book.toc_back_y;
    }
}

static void book_snap_obj(lv_obj_t *obj)
{
    lv_coord_t x = 0;
    lv_coord_t y = 0;

    book_rest_of(obj, &x, &y);
    book_snap_rest(obj, x, y);
}

static void book_rest_and_hide(lv_obj_t *obj)
{
    if (!book_obj_ok(obj)) {
        return;
    }
    book_snap_obj(obj);
    book_set_hidden(obj, true);
}

static void book_label_exit_ready(lv_anim_t *anim)
{
    lv_obj_t *obj = (lv_obj_t *)anim->var;

    if (!book_obj_ok(obj)) {
        return;
    }
    book_snap_obj(obj);
    book_set_hidden(obj, true);
}

static void book_toc_exit_ready(lv_anim_t *anim)
{
    lv_obj_t *obj = (lv_obj_t *)anim->var;

    if (!book_obj_ok(obj)) {
        return;
    }
    book_snap_obj(obj);
    book_set_toc_widget(obj, false);
    book_snap_obj(obj);
}

static void book_anim_run(lv_obj_t *obj,
                          lv_coord_t start_x, lv_coord_t start_y,
                          lv_coord_t end_x, lv_coord_t end_y,
                          int32_t opa_from, int32_t opa_to,
                          uint32_t delay, uint32_t duration,
                          bool exit_anim, book_exit_kind_t exit_kind)
{
    uint32_t fade_time;
    lv_anim_t a;

    if (!book_obj_ok(obj) || duration == 0) {
        return;
    }

    book_anim_stop(obj);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_pos(obj, start_x, start_y);
    lv_obj_set_style_opa(obj, (lv_opa_t)opa_from,
                         LV_PART_MAIN | LV_STATE_DEFAULT);

    fade_time = duration;
    if (!exit_anim && fade_time > 300) {
        fade_time = 300;
    }

    if (start_x != end_x) {
        lv_anim_init(&a);
        lv_anim_set_var(&a, obj);
        lv_anim_set_exec_cb(&a, book_anim_x_exec);
        lv_anim_set_values(&a, start_x, end_x);
        lv_anim_set_delay(&a, delay);
        lv_anim_set_time(&a, duration);
        lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
        lv_anim_start(&a);
    }

    if (start_y != end_y) {
        lv_anim_init(&a);
        lv_anim_set_var(&a, obj);
        lv_anim_set_exec_cb(&a, book_anim_y_exec);
        lv_anim_set_values(&a, start_y, end_y);
        lv_anim_set_delay(&a, delay);
        lv_anim_set_time(&a, duration);
        lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
        lv_anim_start(&a);
    }

    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_exec_cb(&a, book_anim_opa_exec);
    lv_anim_set_values(&a, opa_from, opa_to);
    lv_anim_set_delay(&a, delay);
    lv_anim_set_time(&a, fade_time);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    if (exit_anim) {
        lv_anim_set_ready_cb(&a, (exit_kind == BOOK_EXIT_TOC) ?
                             book_toc_exit_ready : book_label_exit_ready);
    }
    lv_anim_start(&a);
}

static void book_anim_enter(lv_obj_t *obj,
                            lv_coord_t start_x, lv_coord_t start_y,
                            lv_coord_t end_x, lv_coord_t end_y,
                            uint32_t delay, uint32_t duration)
{
    book_anim_run(obj, start_x, start_y, end_x, end_y,
                  LV_OPA_TRANSP, LV_OPA_COVER, delay, duration,
                  false, BOOK_EXIT_LABEL);
}

static void book_anim_exit(lv_obj_t *obj,
                           lv_coord_t start_x, lv_coord_t start_y,
                           lv_coord_t end_x, lv_coord_t end_y,
                           uint32_t delay, uint32_t duration,
                           book_exit_kind_t kind)
{
    if (!book_obj_ok(obj) || lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN)) {
        book_rest_and_hide(obj);
        return;
    }
    book_anim_run(obj, start_x, start_y, end_x, end_y,
                  LV_OPA_COVER, LV_OPA_TRANSP, delay, duration,
                  true, kind);
}

static void book_stop_all_anims(void)
{
    book_anim_stop(guider_ui.screen_book_label_author);
    book_anim_stop(guider_ui.screen_book_label_content);
    book_anim_stop(guider_ui.screen_book_label_pageindex);
    book_anim_stop(guider_ui.screen_book_label_partname);
    book_anim_stop(guider_ui.screen_book_label_partindex);
    book_anim_stop(guider_ui.screen_book_label_bookname);
    book_anim_stop(guider_ui.screen_book_img_content_bg);
    book_anim_stop(guider_ui.screen_book_list_part);
    book_anim_stop(guider_ui.screen_book_btn_back);
}

static void book_settle_reading(bool chapter, bool hide_all)
{
    book_anim_stop(guider_ui.screen_book_label_author);
    book_anim_stop(guider_ui.screen_book_label_content);
    book_anim_stop(guider_ui.screen_book_label_pageindex);
    book_anim_stop(guider_ui.screen_book_label_partname);
    book_anim_stop(guider_ui.screen_book_label_partindex);
    book_anim_stop(guider_ui.screen_book_label_bookname);

    book_snap_rest(guider_ui.screen_book_label_author,
                   BOOK_X_AUTHOR, BOOK_Y_AUTHOR);
    book_snap_rest(guider_ui.screen_book_label_content,
                   BOOK_X_CONTENT, BOOK_Y_CONTENT);
    book_snap_rest(guider_ui.screen_book_label_pageindex,
                   BOOK_X_PAGEINDEX, BOOK_Y_PAGEINDEX);
    book_snap_rest(guider_ui.screen_book_label_partname,
                   BOOK_X_PARTNAME, BOOK_Y_PARTNAME);
    book_snap_rest(guider_ui.screen_book_label_partindex,
                   BOOK_X_PARTINDEX, BOOK_Y_PARTINDEX);
    book_snap_rest(guider_ui.screen_book_label_bookname,
                   BOOK_X_BOOKNAME, BOOK_Y_BOOKNAME);

    if (hide_all) {
        book_set_hidden(guider_ui.screen_book_label_author, true);
        book_set_hidden(guider_ui.screen_book_label_content, true);
        book_set_hidden(guider_ui.screen_book_label_pageindex, true);
        book_set_hidden(guider_ui.screen_book_label_partname, true);
        book_set_hidden(guider_ui.screen_book_label_partindex, true);
        book_set_hidden(guider_ui.screen_book_label_bookname, true);
        return;
    }
    book_set_hidden(guider_ui.screen_book_label_content, chapter);
    book_set_hidden(guider_ui.screen_book_label_bookname, !chapter);
    book_set_hidden(guider_ui.screen_book_label_author, !chapter);
    book_set_hidden(guider_ui.screen_book_label_partindex, !chapter);
    book_set_hidden(guider_ui.screen_book_label_partname, !chapter);
    book_set_hidden(guider_ui.screen_book_label_pageindex, false);
}

static void book_settle_toc_hidden(void)
{
    book_cache_toc_rest();
    book_anim_stop(guider_ui.screen_book_img_content_bg);
    book_anim_stop(guider_ui.screen_book_list_part);
    book_anim_stop(guider_ui.screen_book_btn_back);
    book_snap_rest(guider_ui.screen_book_img_content_bg,
                   s_book.toc_bg_x, s_book.toc_bg_y);
    book_snap_rest(guider_ui.screen_book_list_part,
                   s_book.toc_list_x, s_book.toc_list_y);
    book_snap_rest(guider_ui.screen_book_btn_back,
                   s_book.toc_back_x, s_book.toc_back_y);
    book_set_toc_widget(guider_ui.screen_book_img_content_bg, false);
    book_set_toc_widget(guider_ui.screen_book_list_part, false);
    book_set_toc_widget(guider_ui.screen_book_btn_back, false);
}

static void book_cancel_click_timer(void)
{
    if (s_book.click_timer != NULL) {
        lv_timer_del(s_book.click_timer);
        s_book.click_timer = NULL;
    }
}

#ifdef ESP_PLATFORM
static void book_nvs_save_page(uint16_t page)
{
    nvs_handle_t h;

    if (nvs_open(BOOK_NVS_NS, NVS_READWRITE, &h) != ESP_OK) {
        return;
    }
    nvs_set_u16(h, BOOK_NVS_KEY, page);
    nvs_commit(h);
    nvs_close(h);
}

static uint16_t book_nvs_load_page(void)
{
    nvs_handle_t h;
    uint16_t page = 0;

    if (nvs_open(BOOK_NVS_NS, NVS_READONLY, &h) != ESP_OK) {
        return 0;
    }
    if (nvs_get_u16(h, BOOK_NVS_KEY, &page) != ESP_OK) {
        nvs_close(h);
        return 0;
    }
    nvs_close(h);
    return page;
}
#endif

static void book_clamp_page(void)
{
    const book_info_t *book = book_active();

    if (book->page_count == 0) {
        s_book.page = 0;
        return;
    }
    if (s_book.page >= book->page_count) {
        s_book.page = (uint16_t)(book->page_count - 1);
    }
}

static void book_save_page(void)
{
#ifdef ESP_PLATFORM
    book_nvs_save_page(s_book.page);
#endif
}

static const book_page_t *book_current_page(void)
{
    const book_info_t *book = book_active();

    book_clamp_page();
    if (book->page_count == 0) {
        return NULL;
    }
    return &book->pages[s_book.page];
}

static uint16_t book_part_first_page(uint8_t part)
{
    const book_info_t *book = book_active();
    uint16_t i;

    for (i = 0; i < book->page_count; i++) {
        if (book->pages[i].part == part &&
            book->pages[i].kind == BOOK_PAGE_CHAPTER) {
            return i;
        }
    }
    for (i = 0; i < book->page_count; i++) {
        if (book->pages[i].part == part) {
            return i;
        }
    }
    return 0;
}

static void book_goto_page(uint16_t page, bool persist, book_anim_t anim)
{
    const book_info_t *book = book_active();

    if (book->page_count == 0) {
        return;
    }
    if (page >= book->page_count) {
        page = (uint16_t)(book->page_count - 1);
    }
    s_book.page = page;
    if (persist) {
        book_save_page();
    }
    book_render_page(anim);
}

static void book_turn_page(int delta)
{
    const book_info_t *book = book_active();
    int next;

    if (s_book.toc_open || book->page_count == 0) {
        return;
    }
    book_cancel_click_timer();
    next = (int)s_book.page + delta;
    if (next < 0 || next >= (int)book->page_count) {
        return;
    }
    book_goto_page((uint16_t)next, true,
                   (delta > 0) ? BOOK_ANIM_NEXT : BOOK_ANIM_PREV);
}

static void book_reading_edge_cb(lv_group_t *group, bool next)
{
    (void)group;
    if (!s_book.alive || s_book.toc_open || s_book.leaving) {
        return;
    }
    book_turn_page(next ? 1 : -1);
}

static void book_apply_page_index_text(void)
{
    char buf[12];

    if (!book_obj_ok(guider_ui.screen_book_label_pageindex)) {
        return;
    }
    snprintf(buf, sizeof(buf), "%u", (unsigned)(s_book.page + 1));
    lv_label_set_text(guider_ui.screen_book_label_pageindex, buf);
}

static void book_enter_chapter_labels(void)
{
    book_anim_enter(guider_ui.screen_book_label_bookname,
                    -70, BOOK_Y_BOOKNAME, BOOK_X_BOOKNAME, BOOK_Y_BOOKNAME,
                    0, 440);
    book_anim_enter(guider_ui.screen_book_label_author,
                    320, BOOK_Y_AUTHOR, BOOK_X_AUTHOR, BOOK_Y_AUTHOR,
                    70, 400);
    book_anim_enter(guider_ui.screen_book_label_partindex,
                    -50, BOOK_Y_PARTINDEX, BOOK_X_PARTINDEX, BOOK_Y_PARTINDEX,
                    120, 420);
    book_anim_enter(guider_ui.screen_book_label_partname,
                    -70, BOOK_Y_PARTNAME, BOOK_X_PARTNAME, BOOK_Y_PARTNAME,
                    180, 420);
}

#define BOOK_JUST_WORDS   40

typedef struct {
    const char *ptr;
    uint16_t len;
    lv_coord_t w;
} book_just_word_t;

static bool book_is_gap_char(uint32_t c)
{
    return c == ' ' || c == '\t';
}

static void book_draw_plain_span(lv_draw_ctx_t *draw_ctx,
                                 const lv_draw_label_dsc_t *dsc,
                                 lv_coord_t x, lv_coord_t y,
                                 const char *txt, uint32_t len)
{
    char buf[BOOK_PAGE_CHARS];
    lv_area_t area;
    lv_coord_t w;

    if (txt == NULL || len == 0 || dsc->font == NULL) {
        return;
    }
    if (len >= sizeof(buf)) {
        len = sizeof(buf) - 1;
    }
    memcpy(buf, txt, len);
    buf[len] = '\0';
    w = lv_txt_get_width(buf, len, dsc->font, dsc->letter_space, LV_TEXT_FLAG_NONE);
    area.x1 = x;
    area.y1 = y;
    area.x2 = (lv_coord_t)(x + w);
    area.y2 = (lv_coord_t)(y + lv_font_get_line_height(dsc->font));
    lv_draw_label(draw_ctx, dsc, &area, buf, NULL);
}

static void book_draw_letter_justified(lv_draw_ctx_t *draw_ctx,
                                       const lv_draw_label_dsc_t *dsc,
                                       lv_coord_t x, lv_coord_t y,
                                       lv_coord_t extra, const char *line,
                                       uint32_t len, uint32_t glyphs)
{
    uint32_t i = 0;
    uint32_t gi = 0;
    uint32_t gaps;
    lv_coord_t gap;
    lv_coord_t rem;

    if (glyphs <= 1 || extra <= 0) {
        book_draw_plain_span(draw_ctx, dsc, x, y, line, len);
        return;
    }
    gaps = glyphs - 1;
    gap = extra / (lv_coord_t)gaps;
    rem = extra % (lv_coord_t)gaps;
    while (i < len) {
        uint32_t letter;
        uint32_t letter_next;
        lv_point_t pos;

        _lv_txt_encoded_letter_next_2(line, &letter, &letter_next, &i);
        if (letter == 0) {
            break;
        }
        pos.x = x;
        pos.y = y;
        lv_draw_letter(draw_ctx, dsc, &pos, letter);
        x = (lv_coord_t)(x + lv_font_get_glyph_width(dsc->font, letter, letter_next) +
                         dsc->letter_space);
        if (gi < gaps) {
            x = (lv_coord_t)(x + gap + (gi < (uint32_t)rem ? 1 : 0));
        }
        gi++;
    }
}

static uint32_t book_split_words(const char *line, uint32_t len,
                                 const lv_font_t *font, lv_coord_t letter_space,
                                 book_just_word_t *words, uint32_t max_words,
                                 lv_coord_t *words_w)
{
    uint32_t i = 0;
    uint32_t n = 0;
    lv_coord_t total = 0;

    *words_w = 0;
    while (i < len && n < max_words) {
        uint32_t start;
        uint32_t cur = i;

        while (i < len) {
            uint32_t look = cur;
            uint32_t c = _lv_txt_encoded_next(line, &look);

            if (!book_is_gap_char(c)) {
                break;
            }
            i = look;
            cur = look;
        }
        if (i >= len) {
            break;
        }
        start = i;
        while (i < len) {
            uint32_t look = i;
            uint32_t c = _lv_txt_encoded_next(line, &look);

            if (book_is_gap_char(c)) {
                break;
            }
            i = look;
        }
        words[n].ptr = line + start;
        words[n].len = (uint16_t)(i - start);
        words[n].w = lv_txt_get_width(words[n].ptr, words[n].len, font,
                                      letter_space, LV_TEXT_FLAG_NONE);
        total = (lv_coord_t)(total + words[n].w);
        n++;
    }
    *words_w = total;
    return n;
}

static uint32_t book_count_glyphs(const char *line, uint32_t len)
{
    uint32_t i = 0;
    uint32_t n = 0;

    while (i < len) {
        if (_lv_txt_encoded_next(line, &i) == 0) {
            break;
        }
        n++;
    }
    return n;
}

static void book_draw_justified_line(lv_draw_ctx_t *draw_ctx,
                                     const lv_draw_label_dsc_t *dsc,
                                     lv_coord_t x, lv_coord_t y,
                                     lv_coord_t max_w, const char *line,
                                     uint32_t len, bool justify)
{
    book_just_word_t words[BOOK_JUST_WORDS];
    lv_coord_t words_w = 0;
    uint32_t n;
    uint32_t i;
    lv_coord_t extra;
    lv_coord_t gap;
    lv_coord_t rem;

    if (line == NULL || len == 0 || dsc->font == NULL) {
        return;
    }
    n = book_split_words(line, len, dsc->font, dsc->letter_space,
                         words, BOOK_JUST_WORDS, &words_w);
    extra = (lv_coord_t)(max_w - words_w);
    if (!justify || extra <= 0) {
        book_draw_plain_span(draw_ctx, dsc, x, y, line, len);
        return;
    }
    if (n >= 2) {
        gap = extra / (lv_coord_t)(n - 1);
        rem = extra % (lv_coord_t)(n - 1);
        for (i = 0; i < n; i++) {
            book_draw_plain_span(draw_ctx, dsc, x, y, words[i].ptr, words[i].len);
            x = (lv_coord_t)(x + words[i].w);
            if (i + 1 < n) {
                x = (lv_coord_t)(x + gap + (i < (uint32_t)rem ? 1 : 0));
            }
        }
        return;
    }
    /* Single token / CJK: spread leftover pixels between glyphs. */
    book_draw_letter_justified(draw_ctx, dsc, x, y, extra, line, len,
                               book_count_glyphs(line, len));
}

static void book_content_draw_cb(lv_event_t *e)
{
    lv_obj_t *obj;
    lv_draw_ctx_t *draw_ctx;
    lv_draw_label_dsc_t dsc;
    lv_area_t coords;
    const char *txt;
    char line[BOOK_PAGE_CHARS];
    lv_coord_t max_w;
    lv_coord_t line_h;
    lv_coord_t y;

    if (lv_event_get_code(e) != LV_EVENT_DRAW_MAIN) {
        return;
    }
    obj = lv_event_get_target(e);
    draw_ctx = lv_event_get_draw_ctx(e);
    txt = s_book.content_src;
    if (obj == NULL || draw_ctx == NULL || txt == NULL || txt[0] == '\0') {
        return;
    }

    lv_draw_label_dsc_init(&dsc);
    lv_obj_init_draw_label_dsc(obj, LV_PART_MAIN, &dsc);
    if (dsc.font == NULL || dsc.opa <= LV_OPA_MIN) {
        return;
    }
    dsc.flag = LV_TEXT_FLAG_NONE;

    lv_obj_get_content_coords(obj, &coords);
    max_w = lv_area_get_width(&coords);
    if (max_w <= 0) {
        return;
    }
    line_h = (lv_coord_t)(lv_font_get_line_height(dsc.font) + dsc.line_space);
    y = coords.y1;

    while (txt[0] != '\0' && y <= coords.y2) {
        lv_coord_t used_w = 0;
        uint32_t adv;
        uint32_t draw_len;
        bool para_end = false;
        bool last;
        bool justify;

        adv = _lv_txt_get_next_line(txt, dsc.font, dsc.letter_space, max_w,
                                    &used_w, LV_TEXT_FLAG_NONE);
        if (adv == 0) {
            break;
        }
        draw_len = adv;
        while (draw_len > 0 &&
               (txt[draw_len - 1] == '\n' || txt[draw_len - 1] == '\r')) {
            para_end = true;
            draw_len--;
        }
        while (draw_len > 0 && book_is_gap_char((uint8_t)txt[draw_len - 1])) {
            draw_len--;
        }
        last = (txt[adv] == '\0');
        justify = (!para_end && draw_len > 0 &&
                   (!last || (used_w * 10 >= max_w * 9)));
        if (draw_len > 0 && draw_len < sizeof(line)) {
            memcpy(line, txt, draw_len);
            line[draw_len] = '\0';
            book_draw_justified_line(draw_ctx, &dsc, coords.x1, y, max_w,
                                     line, draw_len, justify);
        }
        y = (lv_coord_t)(y + line_h);
        txt += adv;
    }
}

static void book_bind_content_draw(void)
{
    if (!book_obj_ok(guider_ui.screen_book_label_content) ||
        s_book.content_draw_bound) {
        return;
    }
    lv_obj_add_event_cb(guider_ui.screen_book_label_content,
                        book_content_draw_cb, LV_EVENT_DRAW_MAIN, NULL);
    s_book.content_draw_bound = true;
}

static void book_enter_content(book_anim_t anim)
{
    lv_coord_t start_x = BOOK_X_CONTENT;
    lv_coord_t start_y = BOOK_Y_CONTENT;

    if (anim == BOOK_ANIM_NEXT) {
        start_x = (lv_coord_t)(BOOK_X_CONTENT + 36);
    } else if (anim == BOOK_ANIM_PREV) {
        start_x = (lv_coord_t)(BOOK_X_CONTENT - 28);
    } else {
        start_y = (lv_coord_t)(BOOK_Y_CONTENT + 18);
    }
    book_anim_enter(guider_ui.screen_book_label_content,
                    start_x, start_y, BOOK_X_CONTENT, BOOK_Y_CONTENT,
                    0, (anim == BOOK_ANIM_ENTER) ? 400 : 340);
}

static void book_enter_pageindex(book_anim_t anim)
{
    lv_coord_t start_x = BOOK_X_PAGEINDEX;
    lv_coord_t start_y = BOOK_Y_PAGEINDEX;

    if (anim == BOOK_ANIM_NEXT) {
        start_x = (lv_coord_t)(BOOK_X_PAGEINDEX + 18);
    } else if (anim == BOOK_ANIM_PREV) {
        start_x = (lv_coord_t)(BOOK_X_PAGEINDEX - 18);
    } else {
        start_y = 176;
    }
    book_anim_enter(guider_ui.screen_book_label_pageindex,
                    start_x, start_y, BOOK_X_PAGEINDEX, BOOK_Y_PAGEINDEX,
                    80, 320);
}

static void book_render_page(book_anim_t anim)
{
    const book_info_t *book = book_active();
    const book_page_t *page = book_current_page();
    bool chapter;
    char part_buf[20];

    if (!book_scr_ok() || page == NULL) {
        return;
    }

    chapter = (page->kind == BOOK_PAGE_CHAPTER);

    if (book_obj_ok(guider_ui.screen_book_label_bookname)) {
        lv_label_set_text(guider_ui.screen_book_label_bookname, book->name);
    }
    if (book_obj_ok(guider_ui.screen_book_label_author)) {
        lv_label_set_text(guider_ui.screen_book_label_author, book->author);
    }
    if (page->part < book->part_count) {
        snprintf(part_buf, sizeof(part_buf), "Part %u",
                 (unsigned)(page->part + 1));
        if (book_obj_ok(guider_ui.screen_book_label_partindex)) {
            lv_label_set_text(guider_ui.screen_book_label_partindex, part_buf);
        }
        if (book_obj_ok(guider_ui.screen_book_label_partname)) {
            lv_label_set_text(guider_ui.screen_book_label_partname,
                              book->parts[page->part]);
        }
    }
    if (!chapter && book_obj_ok(guider_ui.screen_book_label_content)) {
        /* LVGL labels have no justify align. Keep the widget empty so
         * its default left-aligned draw stays off; book_content_draw_cb
         * paints the page with word/glyph spacing instead. */
        s_book.content_src = (page->text != NULL) ? page->text : "";
        lv_label_set_text(guider_ui.screen_book_label_content, "");
        lv_obj_invalidate(guider_ui.screen_book_label_content);
    } else {
        s_book.content_src = NULL;
    }
    book_apply_page_index_text();

    /* Interrupt: kill in-flight anims and snap every reading label home
     * before the new enter starts, so fast knobs never leave a widget
     * stranded mid-slide. */
    book_settle_reading(chapter, anim != BOOK_ANIM_NONE);
    if (anim == BOOK_ANIM_NONE) {
        return;
    }
    if (chapter) {
        book_enter_chapter_labels();
        book_enter_pageindex(anim);
        return;
    }
    book_enter_content(anim);
    book_enter_pageindex(anim);
}

static void book_part_style_apply(lv_obj_t *btn)
{
    uint32_t i;
    uint32_t n;

    if (!book_obj_ok(btn)) {
        return;
    }
    lv_obj_set_style_pad_all(btn, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(btn, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0xffffff),
                              LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(btn, lv_color_hex(0x0D3055),
                                LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(btn, &lv_font_montserratMedium_12,
                               LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0xe1e6ee),
                              LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0xe1e6ee),
                              LV_PART_MAIN | LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_width(btn, 0, LV_STATE_FOCUSED);
    lv_obj_set_style_outline_width(btn, 0, LV_STATE_FOCUS_KEY);

    n = lv_obj_get_child_cnt(btn);
    for (i = 0; i < n; i++) {
        lv_obj_t *child = lv_obj_get_child(btn, i);
        if (child != NULL && lv_obj_check_type(child, &lv_label_class)) {
            lv_label_set_long_mode(child, LV_LABEL_LONG_DOT);
        }
    }
}

static void book_on_part_clicked(lv_event_t *e)
{
    uintptr_t part;

    if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }
    if (!s_book.toc_open || s_book.leaving) {
        return;
    }
    lv_event_stop_bubbling(e);
    part = (uintptr_t)lv_event_get_user_data(e);
    book_goto_page(book_part_first_page((uint8_t)part), true, BOOK_ANIM_CHAPTER);
    book_toc_close(true);
}

static void book_on_back_clicked(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }
    if (!s_book.toc_open || s_book.leaving) {
        return;
    }
    lv_event_stop_bubbling(e);
    book_toc_close(false);
}

static void book_list_rebuild(void)
{
    const book_info_t *book = book_active();
    lv_obj_t *list = guider_ui.screen_book_list_part;
    uint8_t i;
    uint8_t count;

    if (!book_obj_ok(list)) {
        return;
    }

    for (i = lv_obj_get_child_cnt(list); i > 0; i--) {
        lv_obj_t *child = lv_obj_get_child(list, i - 1);
        if (book_obj_ok(child)) {
            lv_obj_del(child);
        }
    }
    guider_ui.screen_book_list_part_item0 = NULL;
    guider_ui.screen_book_list_part_item1 = NULL;
    guider_ui.screen_book_list_part_item2 = NULL;
    guider_ui.screen_book_list_part_item3 = NULL;
    guider_ui.screen_book_list_part_item4 = NULL;
    guider_ui.screen_book_list_part_item5 = NULL;
    memset(s_book.part_btns, 0, sizeof(s_book.part_btns));

    count = book->part_count;
    if (count > BOOK_MAX_PARTS) {
        count = BOOK_MAX_PARTS;
    }
    s_book.part_btn_count = count;

    lv_obj_set_scrollbar_mode(list, LV_SCROLLBAR_MODE_AUTO);

    for (i = 0; i < count; i++) {
        lv_obj_t *btn = lv_list_add_btn(list, NULL, book->parts[i]);
        s_book.part_btns[i] = btn;
        if (i == 0) {
            guider_ui.screen_book_list_part_item0 = btn;
        } else if (i == 1) {
            guider_ui.screen_book_list_part_item1 = btn;
        } else if (i == 2) {
            guider_ui.screen_book_list_part_item2 = btn;
        } else if (i == 3) {
            guider_ui.screen_book_list_part_item3 = btn;
        } else if (i == 4) {
            guider_ui.screen_book_list_part_item4 = btn;
        } else if (i == 5) {
            guider_ui.screen_book_list_part_item5 = btn;
        }
        if (!book_obj_ok(btn)) {
            continue;
        }
        book_part_style_apply(btn);
        lv_obj_add_event_cb(btn, book_on_part_clicked, LV_EVENT_CLICKED,
                            (void *)(uintptr_t)i);
    }
    s_book.list_built = true;
}

static void book_toc_open(void)
{
    const book_page_t *page;
    uint8_t focus_part = 0;

    if (!book_scr_ok() || s_book.leaving) {
        return;
    }
    book_cancel_click_timer();
    page = book_current_page();
    book_settle_reading(page != NULL && page->kind == BOOK_PAGE_CHAPTER, false);
    if (!s_book.list_built) {
        book_list_rebuild();
    }

    s_book.toc_open = true;
    book_set_toc_widget(guider_ui.screen_book_img_content_bg, true);
    book_set_toc_widget(guider_ui.screen_book_list_part, true);
    book_set_toc_widget(guider_ui.screen_book_btn_back, true);
    if (book_obj_ok(guider_ui.screen_book_img_content_bg)) {
        lv_obj_clear_flag(guider_ui.screen_book_img_content_bg,
                          LV_OBJ_FLAG_CLICKABLE);
        lv_obj_move_foreground(guider_ui.screen_book_img_content_bg);
    }
    if (book_obj_ok(guider_ui.screen_book_list_part)) {
        lv_obj_move_foreground(guider_ui.screen_book_list_part);
    }
    if (book_obj_ok(guider_ui.screen_book_btn_back)) {
        lv_obj_move_foreground(guider_ui.screen_book_btn_back);
    }
    book_cache_toc_rest();
    /* Stay on the Guider coordinates; only fade in from hidden. */
    book_anim_enter(guider_ui.screen_book_img_content_bg,
                    s_book.toc_bg_x, s_book.toc_bg_y,
                    s_book.toc_bg_x, s_book.toc_bg_y,
                    0, 320);
    book_anim_enter(guider_ui.screen_book_list_part,
                    s_book.toc_list_x, s_book.toc_list_y,
                    s_book.toc_list_x, s_book.toc_list_y,
                    40, 320);
    book_anim_enter(guider_ui.screen_book_btn_back,
                    s_book.toc_back_x, s_book.toc_back_y,
                    s_book.toc_back_x, s_book.toc_back_y,
                    60, 280);

    page = book_current_page();
    if (page != NULL) {
        focus_part = page->part;
    }
    book_toc_group_bind();
    if (focus_part < s_book.part_btn_count &&
        book_obj_ok(s_book.part_btns[focus_part])) {
        group_focus_obj(s_book.part_btns[focus_part]);
        lv_obj_scroll_to_view(s_book.part_btns[focus_part], LV_ANIM_OFF);
    }
}

static void book_toc_close(bool jump_chapter)
{
    (void)jump_chapter;
    if (!book_scr_ok()) {
        return;
    }
    s_book.toc_open = false;
    book_cache_toc_rest();
    book_anim_stop(guider_ui.screen_book_img_content_bg);
    book_anim_stop(guider_ui.screen_book_list_part);
    book_anim_stop(guider_ui.screen_book_btn_back);
    book_snap_rest(guider_ui.screen_book_img_content_bg,
                   s_book.toc_bg_x, s_book.toc_bg_y);
    book_snap_rest(guider_ui.screen_book_list_part,
                   s_book.toc_list_x, s_book.toc_list_y);
    book_snap_rest(guider_ui.screen_book_btn_back,
                   s_book.toc_back_x, s_book.toc_back_y);
    if (book_obj_ok(guider_ui.screen_book_img_content_bg)) {
        lv_obj_clear_flag(guider_ui.screen_book_img_content_bg,
                          LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_state(guider_ui.screen_book_img_content_bg,
                         LV_STATE_DISABLED);
    }
    if (book_obj_ok(guider_ui.screen_book_list_part)) {
        lv_obj_clear_flag(guider_ui.screen_book_list_part,
                          LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_state(guider_ui.screen_book_list_part, LV_STATE_DISABLED);
    }
    if (book_obj_ok(guider_ui.screen_book_btn_back)) {
        lv_obj_clear_flag(guider_ui.screen_book_btn_back,
                          LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_state(guider_ui.screen_book_btn_back, LV_STATE_DISABLED);
    }
    book_anim_exit(guider_ui.screen_book_img_content_bg,
                   s_book.toc_bg_x, s_book.toc_bg_y,
                   s_book.toc_bg_x, s_book.toc_bg_y,
                   0, 240, BOOK_EXIT_TOC);
    book_anim_exit(guider_ui.screen_book_list_part,
                   s_book.toc_list_x, s_book.toc_list_y,
                   s_book.toc_list_x, s_book.toc_list_y,
                   20, 240, BOOK_EXIT_TOC);
    book_anim_exit(guider_ui.screen_book_btn_back,
                   s_book.toc_back_x, s_book.toc_back_y,
                   s_book.toc_back_x, s_book.toc_back_y,
                   20, 200, BOOK_EXIT_TOC);
    book_reading_group_bind();
}

static void book_click_timer_cb(lv_timer_t *timer)
{
    LV_UNUSED(timer);
    s_book.click_timer = NULL;
    if (!s_book.alive || s_book.leaving || s_book.toc_open) {
        return;
    }
    book_toc_open();
}

static void book_on_read_clicked(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }
    if (lv_event_get_target(e) != guider_ui.screen_book) {
        return;
    }
    if (!s_book.alive || s_book.leaving || s_book.toc_open) {
        return;
    }
    if (!book_scr_ok() || lv_scr_act() != guider_ui.screen_book) {
        return;
    }
    /* First click waits 750ms. A second click in that window goes home.
     * Turning the knob cancels the wait so TOC is not opened by accident. */
    if (s_book.click_timer != NULL) {
        book_cancel_click_timer();
        s_book.leaving = true;
        lv_async_call(book_leave_to_main_async, NULL);
        return;
    }
    s_book.click_timer = lv_timer_create(book_click_timer_cb,
                                         BOOK_DOUBLE_CLICK_MS, NULL);
    lv_timer_set_repeat_count(s_book.click_timer, 1);
}

static void book_load_main(void)
{
    book_clear_group_hooks();
    ui_load_scr_animation(&guider_ui, &guider_ui.screen_main,
                          guider_ui.screen_main_del, &guider_ui.screen_book_del,
                          setup_scr_screen_main, LV_SCR_LOAD_ANIM_FADE_ON,
                          100, 100, true, true);
}

static void book_leave_to_main_async(void *unused)
{
    LV_UNUSED(unused);
    book_cancel_click_timer();
    book_stop_all_anims();
    book_save_page();
    book_load_main();
}

static void book_on_screen_delete(lv_event_t *e)
{
    lv_obj_t *target;

    if (lv_event_get_code(e) != LV_EVENT_DELETE) {
        return;
    }
    target = lv_event_get_target(e);
    book_cancel_click_timer();
    book_stop_all_anims();
    book_clear_group_hooks();
    if (guider_ui.screen_book == target) {
        guider_ui.screen_book = NULL;
        guider_ui.screen_book_label_author = NULL;
        guider_ui.screen_book_label_content = NULL;
        guider_ui.screen_book_label_pageindex = NULL;
        guider_ui.screen_book_label_partname = NULL;
        guider_ui.screen_book_label_partindex = NULL;
        guider_ui.screen_book_label_bookname = NULL;
        guider_ui.screen_book_img_content_bg = NULL;
        guider_ui.screen_book_list_part = NULL;
        guider_ui.screen_book_list_part_item0 = NULL;
        guider_ui.screen_book_list_part_item1 = NULL;
        guider_ui.screen_book_list_part_item2 = NULL;
        guider_ui.screen_book_list_part_item3 = NULL;
        guider_ui.screen_book_list_part_item4 = NULL;
        guider_ui.screen_book_list_part_item5 = NULL;
        guider_ui.screen_book_btn_back = NULL;
        guider_ui.screen_book_btn_back_label = NULL;
    }
    memset(s_book.part_btns, 0, sizeof(s_book.part_btns));
    s_book.bound_scr = NULL;
    s_book.part_btn_count = 0;
    s_book.events_bound = false;
    s_book.list_built = false;
    s_book.toc_open = false;
    s_book.leaving = false;
    s_book.alive = false;
    s_book.toc_rest_cached = false;
    s_book.content_draw_bound = false;
    s_book.content_src = NULL;
}

static void book_bind_delete(void)
{
    if (!book_scr_ok() || s_book.events_bound) {
        return;
    }
    lv_obj_add_event_cb(guider_ui.screen_book, book_on_screen_delete,
                        LV_EVENT_DELETE, NULL);
    lv_obj_add_event_cb(guider_ui.screen_book, book_on_read_clicked,
                        LV_EVENT_CLICKED, NULL);
    if (book_obj_ok(guider_ui.screen_book_btn_back)) {
        lv_obj_add_event_cb(guider_ui.screen_book_btn_back, book_on_back_clicked,
                            LV_EVENT_CLICKED, NULL);
    }
    s_book.events_bound = true;
}

static void book_prepare_widgets(void)
{
    lv_obj_t *labels[] = {
        guider_ui.screen_book_label_author,
        guider_ui.screen_book_label_content,
        guider_ui.screen_book_label_pageindex,
        guider_ui.screen_book_label_partname,
        guider_ui.screen_book_label_partindex,
        guider_ui.screen_book_label_bookname,
    };
    size_t i;

    book_cache_toc_rest();
    book_bind_content_draw();
    if (book_obj_ok(guider_ui.screen_book_label_content)) {
        lv_label_set_text(guider_ui.screen_book_label_content, "");
    }
    lv_obj_add_flag(guider_ui.screen_book, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(guider_ui.screen_book, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    book_clear_focus_box(guider_ui.screen_book);

    for (i = 0; i < sizeof(labels) / sizeof(labels[0]); i++) {
        if (book_obj_ok(labels[i])) {
            lv_obj_clear_flag(labels[i], LV_OBJ_FLAG_CLICKABLE);
            lv_obj_clear_flag(labels[i], LV_OBJ_FLAG_CLICK_FOCUSABLE);
            book_rest_and_hide(labels[i]);
        }
    }
    if (!s_book.list_built) {
        book_list_rebuild();
    }
    book_settle_toc_hidden();
}

static void book_reading_group_bind(void)
{
    if (!book_scr_ok() || lv_scr_act() != guider_ui.screen_book) {
        return;
    }
    book_keypad_group_reset();
    lv_obj_add_flag(guider_ui.screen_book, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(guider_ui.screen_book, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_group_add_obj(g, guider_ui.screen_book);
    book_clear_focus_box(guider_ui.screen_book);
    group_focus_obj(guider_ui.screen_book);
    if (g != NULL) {
        lv_group_set_wrap(g, true);
        lv_group_set_edge_cb(g, book_reading_edge_cb);
    }
}

static void book_toc_group_bind(void)
{
    uint8_t i;

    if (!book_scr_ok() || lv_scr_act() != guider_ui.screen_book) {
        return;
    }
    book_keypad_group_reset();
    book_clear_group_hooks();
    if (book_obj_ok(guider_ui.screen_book_btn_back)) {
        lv_group_add_obj(g, guider_ui.screen_book_btn_back);
    }
    for (i = 0; i < s_book.part_btn_count; i++) {
        if (book_obj_ok(s_book.part_btns[i])) {
            lv_group_add_obj(g, s_book.part_btns[i]);
        }
    }
    if (g != NULL) {
        lv_group_set_wrap(g, true);
    }
}

void screen_book_buttion_add(void)
{
    if (!book_scr_ok() || lv_scr_act() != guider_ui.screen_book) {
        return;
    }
    book_bind_delete();
    if (s_book.toc_open) {
        book_toc_group_bind();
    } else {
        book_reading_group_bind();
    }
}

void screen_book_custom_init(void)
{
    if (!book_scr_ok() || s_book.leaving) {
        return;
    }
    s_book.book = &s_loaded;
    book_bind_delete();
    if (lv_scr_act() != guider_ui.screen_book) {
        return;
    }
    lv_async_call_cancel(book_leave_to_main_async, NULL);
    s_book.alive = true;
    s_book.bound_scr = guider_ui.screen_book;
    s_book.toc_open = false;
    book_clamp_page();
    book_prepare_widgets();
    book_render_page(BOOK_ANIM_ENTER);
    book_reading_group_bind();
#ifdef ESP_PLATFORM
    ESP_LOGI(BOOK_TAG, "reader ready page=%u/%u",
             (unsigned)(s_book.page + 1),
             (unsigned)book_active()->page_count);
#endif
}

void book_machine_reload(void)
{
    if (!book_load_spiffs()) {
        book_use_fallback();
    }
    s_book.book = &s_loaded;
    s_book.list_built = false;
    book_clamp_page();
}

void book_machine_init(void)
{
    book_machine_reload();
    if (!s_book.page_loaded) {
#ifdef ESP_PLATFORM
        s_book.page = book_nvs_load_page();
#endif
        s_book.page_loaded = true;
        book_clamp_page();
    }
}

void book_machine_poll(void)
{
    lv_obj_t *act;

    if (s_book.leaving) {
        return;
    }
    if (!book_scr_ok()) {
        return;
    }
    act = lv_scr_act();
    if (act == NULL || act != guider_ui.screen_book) {
        return;
    }
    if (s_book.alive && s_book.bound_scr == act) {
        return;
    }
    screen_book_custom_init();
}
