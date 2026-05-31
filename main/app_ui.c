/**
 * @file app_ui.c
 * @brief 屏对象存活检查与 UI 刷新（避免切屏时访问悬空指针）
 */
#include "app_ui.h"
#include "app_globals.h"
#include "bat_percent.h"
#include "custom.h"
#include "ds3231.h"
#include "wifi.h"
#include "word_storage.h"
#include "driver/i2c.h"

static int s_last_review_cards_num = 0;

/* -------------------------------------------------------------------------- */
/* 屏存活检查                                                                  */
/* -------------------------------------------------------------------------- */

static inline bool timing_screen_is_alive(void)
{
    return guider_ui.screen_timing != NULL
        && lv_obj_is_valid(guider_ui.screen_timing)
        && lv_scr_act() == guider_ui.screen_timing;
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
    if (wordcard_session_start) {
        wordcard_session_start = 0;
        round_cards_num = word_storage_learn_start(WORDCARD_ROUND_SIZE);
        cards_index = (round_cards_num > 0) ? 1 : 0;
    }
    bool marked_wrong = (review_cards_num > s_last_review_cards_num);
    word_session_state_t st;
    word_storage_learn_signal(marked_wrong, &st);
    review_cards_num = word_storage_get_review_count();
    s_last_review_cards_num = review_cards_num;
    wordcard_render(&st);
}

void app_ui_reviewcard_process_change(void)
{
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
    review_cards_num = word_storage_get_review_count();
    s_last_review_cards_num = review_cards_num;
}

bool app_ui_wificfg_screen_is_alive(void)
{
    return wificfg_screen_is_alive();
}

bool app_ui_timing_screen_is_alive(void)
{
    return timing_screen_is_alive();
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

    if (bar != NULL && lv_obj_is_valid(bar) &&
        lv_obj_get_parent(bar) == guider_ui.screen_timing) {
        int bat = get_battery_percentage();
        if (bat < 0) {
            bat = 0;
        }
        if (bat > 100) {
            bat = 100;
        }
        lv_bar_set_value(bar, bat, LV_ANIM_OFF);
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
        lv_obj_t *arc = guider_ui.screen_main_arc_battery;
        if (arc != NULL && lv_obj_is_valid(arc)) {
            lv_arc_set_value(arc, get_battery_percentage());
        }
    } else if (guider_ui.screen_timing != NULL &&
               lv_obj_is_valid(guider_ui.screen_timing) &&
               act_scr == guider_ui.screen_timing) {
        app_ui_timing_status_update(is_wifi_connected());
    }
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
    ds3231_time_t now;
    if (ds3231_get_time(I2C_NUM_0, &now) != ESP_OK) {
        return;
    }
    lv_label_set_text_fmt(guider_ui.screen_main_btn_realtime_label,
                          "%02d:%02d", now.hour, now.minute);
}
