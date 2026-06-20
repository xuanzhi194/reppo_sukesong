/**
 * @file app_ui.c
 * @brief 屏对象存活检查与 UI 刷新（避免切屏时访问悬空指针）
 */
#include "app_ui.h"
#include "app_globals.h"
#include "bat_percent.h"
#include "custom.h"
#include "rx8130.h"
#include "wifi.h"
#include "word_storage.h"
#include "driver/i2c.h"
#include <string.h>

static int s_last_review_cards_num = 0;

#define BAT_ARC_COLOR_NORMAL   0x0a2a4c
#define BAT_ARC_COLOR_CHARGING 0x8fd68f
#define BAT_ARC_COLOR_LOW      0xf0a8a8
#define BAT_ARC_LOW_PERCENT    20

static void app_ui_main_battery_arc_update(lv_obj_t *arc, const battery_info_t *info)
{
    if (arc == NULL || !lv_obj_is_valid(arc) || info == NULL) {
        return;
    }

    int pct = info->display_percent;
    if (pct < 0) {
        pct = 0;
    }
    if (pct > 100) {
        pct = 100;
    }
    lv_arc_set_value(arc, pct);

    uint32_t color_hex = BAT_ARC_COLOR_NORMAL;
    if (info->plugged) {
        color_hex = BAT_ARC_COLOR_CHARGING;
    } else if (pct < BAT_ARC_LOW_PERCENT) {
        color_hex = BAT_ARC_COLOR_LOW;
    }
    lv_obj_set_style_arc_color(arc, lv_color_hex(color_hex), LV_PART_INDICATOR | LV_STATE_DEFAULT);
}

/* -------------------------------------------------------------------------- */
/* 屏存活检查                                                                  */
/* -------------------------------------------------------------------------- */

static inline bool timing_screen_is_alive(void)
{
    return guider_ui.screen_timing != NULL
        && lv_obj_is_valid(guider_ui.screen_timing)
        && lv_scr_act() == guider_ui.screen_timing;
}

static bool timing_label_is_ready(lv_obj_t *label)
{
    return label != NULL
        && lv_obj_is_valid(label)
        && guider_ui.screen_timing != NULL
        && lv_obj_is_valid(guider_ui.screen_timing)
        && lv_obj_get_parent(label) == guider_ui.screen_timing;
}

static inline bool wordcard_screen_is_alive(void)
{
    return guider_ui.screen_wordcard != NULL
        && lv_obj_is_valid(guider_ui.screen_wordcard)
        && lv_scr_act() == guider_ui.screen_wordcard;
}

static inline bool reviewcard_screen_is_alive(void)
{
    return guider_ui.screen_reviewcard != NULL
        && lv_obj_is_valid(guider_ui.screen_reviewcard)
        && lv_scr_act() == guider_ui.screen_reviewcard;
}

static inline bool wificfg_screen_is_alive(void)
{
    return guider_ui.screen_wificfg != NULL
        && lv_obj_is_valid(guider_ui.screen_wificfg)
        && lv_scr_act() == guider_ui.screen_wificfg;
}

/* -------------------------------------------------------------------------- */
/* 单词卡 / 复习卡渲染（供 NVS 工作线程调用）                                   */
/* -------------------------------------------------------------------------- */

static void wordcard_render(const word_session_state_t *st)
{
    if (!wordcard_screen_is_alive()) {
        return;
    }
    if (st->finished || !st->valid) {
        return;
    }
    lv_obj_clear_state(guider_ui.screen_wordcard_btn_dui, LV_STATE_DISABLED);
    lv_obj_clear_state(guider_ui.screen_wordcard_btn_cuo, LV_STATE_DISABLED);
    lv_label_set_text(guider_ui.screen_wordcard_label_spelling, st->word.word);
    lv_label_set_text(guider_ui.screen_wordcard_label_phonetic, st->word.phonetic);
    lv_label_set_text(guider_ui.screen_wordcard_label_meaning, st->word.definition);
}

static void reviewcard_render(const word_session_state_t *st)
{
    if (!reviewcard_screen_is_alive()) {
        return;
    }
    if (st->finished || !st->valid) {
        lv_label_set_text(guider_ui.screen_reviewcard_label_index, "0/0");
        lv_label_set_text(guider_ui.screen_reviewcard_label_spelling, "Good Job!");
        lv_label_set_text(guider_ui.screen_reviewcard_label_phonetic, "");
        lv_label_set_text(guider_ui.screen_reviewcard_label_meaning, "No word need to review");
        lv_obj_add_state(guider_ui.screen_reviewcard_btn_dui, LV_STATE_DISABLED);
        return;
    }
    lv_label_set_text(guider_ui.screen_reviewcard_label_spelling, st->word.word);
    lv_label_set_text(guider_ui.screen_reviewcard_label_phonetic, st->word.phonetic);
    lv_label_set_text(guider_ui.screen_reviewcard_label_meaning, st->word.definition);
}

void app_ui_wordcard_process_change(void)
{
    if (!wordcard_screen_is_alive()) {
        wordcard_change = 0;
        return;
    }
    if (wordcard_session_start) {
        wordcard_session_start = 0;
        round_cards_num = word_storage_learn_start(WORDCARD_ROUND_SIZE);
        cards_index = (round_cards_num > 0) ? 1 : 0;
    }
    bool marked_wrong = (review_cards_num > s_last_review_cards_num);
    word_session_state_t st;
    word_storage_learn_signal(marked_wrong, &st);
    review_cards_num = word_storage_get_review_count();
    learned_cards_num = word_storage_get_learned_count();
    s_last_review_cards_num = review_cards_num;
    wordcard_render(&st);
}

void app_ui_reviewcard_process_change(void)
{
    if (!reviewcard_screen_is_alive()) {
        reviewcard_change = 0;
        return;
    }
    if (reviewcard_session_start) {
        reviewcard_session_start = 0;
        word_storage_review_start();
    }
    word_session_state_t st;
    word_storage_review_signal(&st);
    review_cards_num = word_storage_get_review_count();
    s_last_review_cards_num = review_cards_num;
    reviewcard_render(&st);
}

void app_ui_init_word_counters(void)
{
    int word_total = word_storage_get_total_count();
    total_cards_num = (word_total > 0) ? word_total : 1;
    learned_cards_num = word_storage_get_learned_count();
    review_cards_num = word_storage_get_review_count();
    s_last_review_cards_num = review_cards_num;
}

bool app_ui_wificfg_screen_is_alive(void)
{
    return wificfg_screen_is_alive();
}

bool app_ui_timing_screen_is_alive(void)
{
    if (!timing_screen_is_alive()) {
        return false;
    }
    return timing_label_is_ready(guider_ui.screen_timing_label_date)
        && timing_label_is_ready(guider_ui.screen_timing_label_realtime)
        && timing_label_is_ready(guider_ui.screen_timing_label_day);
}

void app_ui_timing_labels_update(const char *date_str, const char *time_str, const char *day_str)
{
    if (!app_ui_timing_screen_is_alive()) {
        return;
    }
    if (date_str == NULL || time_str == NULL || day_str == NULL) {
        return;
    }

    static char s_date[32];
    static char s_time[16];
    static char s_day[16];
    strncpy(s_date, date_str, sizeof(s_date) - 1);
    s_date[sizeof(s_date) - 1] = '\0';
    strncpy(s_time, time_str, sizeof(s_time) - 1);
    s_time[sizeof(s_time) - 1] = '\0';
    strncpy(s_day, day_str, sizeof(s_day) - 1);
    s_day[sizeof(s_day) - 1] = '\0';

    lv_label_set_text_static(guider_ui.screen_timing_label_date, s_date);
    lv_label_set_text_static(guider_ui.screen_timing_label_realtime, s_time);
    lv_label_set_text_static(guider_ui.screen_timing_label_day, s_day);
}

void app_ui_timing_status_update(bool wifi_connected)
{
    if (!timing_screen_is_alive()) {
        return;
    }

    lv_obj_t *img_on = guider_ui.screen_timing_img_wificonnect;
    lv_obj_t *img_off = guider_ui.screen_timing_img_wifidis;
    lv_obj_t *bar = guider_ui.screen_timing_bar_battery;

    if (img_on != NULL && lv_obj_is_valid(img_on) &&
        img_off != NULL && lv_obj_is_valid(img_off)) {
        if (wifi_connected) {
            lv_obj_clear_flag(img_on, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(img_off, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(img_on, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(img_off, LV_OBJ_FLAG_HIDDEN);
        }
    }

    battery_info_t bat_info;
    battery_monitor_get(&bat_info);

    if (bar != NULL && lv_obj_is_valid(bar) &&
        lv_obj_get_parent(bar) == guider_ui.screen_timing) {
        int bat = bat_info.display_percent;
        if (bat < 0) {
            bat = 0;
        }
        if (bat > 100) {
            bat = 100;
        }
        lv_bar_set_value(bar, bat, LV_ANIM_OFF);
    }

    /* 闪电图标：充电中或已充满（仍插线）时显示 */
    lv_obj_t *baticon = guider_ui.screen_timing_img_baticon;
    if (baticon != NULL && lv_obj_is_valid(baticon) &&
        lv_obj_get_parent(baticon) == guider_ui.screen_timing) {
        if (bat_info.plugged) {
            lv_obj_clear_flag(baticon, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(baticon, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

void app_ui_battery_timer_cb(lv_timer_t *timer)
{
    (void)timer;
    lv_obj_t *act_scr = lv_scr_act();
    if (act_scr == NULL) {
        return;
    }
    if (guider_ui.screen_main != NULL &&
        lv_obj_is_valid(guider_ui.screen_main) &&
        act_scr == guider_ui.screen_main) {
        battery_info_t bat_info;
        battery_monitor_get(&bat_info);
        app_ui_main_battery_arc_update(guider_ui.screen_main_arc_battery, &bat_info);
    } else if (guider_ui.screen_timing != NULL &&
               lv_obj_is_valid(guider_ui.screen_timing) &&
               act_scr == guider_ui.screen_timing) {
        app_ui_timing_status_update(is_wifi_connected());
    }
}

void app_ui_init_clock_label_static(void)
{
    if (guider_ui.screen_main_btn_realtime_label == NULL ||
        !lv_obj_is_valid(guider_ui.screen_main_btn_realtime_label)) {
        return;
    }
    /* Convert the clock label from dynamic text (allocated in LVGL heap by
     * the generated setup_ui code) to static-text mode.  This frees the
     * original "00:00" allocation exactly once here, while the LVGL heap is
     * in a clean, unfragmented state.  From now on, lv_label_set_text_static
     * stores a pointer to our static buffer and never touches the LVGL heap,
     * eliminating the alloc/free cycle that triggered heap corruption. */
    lv_label_set_text_static(guider_ui.screen_main_btn_realtime_label, "--:--");
}

void app_ui_realtime_clock_timer_cb(lv_timer_t *timer)
{
    (void)timer;
    if (lv_scr_act() != guider_ui.screen_main) {
        return;
    }
    if (guider_ui.screen_main_btn_realtime_label == NULL ||
        !lv_obj_is_valid(guider_ui.screen_main_btn_realtime_label)) {
        return;
    }
    rx8130_time_t now;
    if (rx8130_get_time(I2C_NUM_0, &now) != ESP_OK) {
        return;
    }
    /* Cache last displayed time: only update LVGL when the minute changes.
     * Using lv_label_set_text_static avoids repeated alloc/free on the LVGL
     * heap for what is effectively a fixed-length 5-char string. */
    static uint8_t s_last_hour = 0xff;
    static uint8_t s_last_min  = 0xff;
    if (now.hour == s_last_hour && now.minute == s_last_min) {
        return;
    }
    s_last_hour = now.hour;
    s_last_min  = now.minute;
    static char s_clock_buf[8];
    snprintf(s_clock_buf, sizeof(s_clock_buf), "%02d:%02d", now.hour, now.minute);
    lv_label_set_text_static(guider_ui.screen_main_btn_realtime_label, s_clock_buf);
}
