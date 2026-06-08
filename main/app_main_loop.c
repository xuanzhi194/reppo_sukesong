/**
 * @file app_main_loop.c
 * @brief 主循环事件分发（原 app_main while(1) 内逻辑）
 */
#include "app_main_loop.h"
#include "app_globals.h"
#include "app_led.h"
#include "app_ui.h"
#include "ai_chat_secrets.h"
#include "ai_demo_task.h"
#include "custom.h"
#include "esp_random.h"
#include "todo_storage.h"
#include "wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

#define AI_ANALYSIS_TIMEOUT_MS 12000

/** 答案之书：由 UI 置位，主循环写入随机数 */
static void main_loop_handle_answer_book(void)
{
    if (!answer_book_rand_req) {
        return;
    }
    answer_book_rand_req = 0;
    answer_book_rand = esp_random();
}

/** 番茄钟倒计时结束 → 蜂鸣 + 流水灯 */
static void main_loop_handle_countdown_timeout(void)
{
    if (!countdown_timeout_pending || buzzer_timeout_task_running) {
        return;
    }
    countdown_timeout_pending = 0;
    app_led_start_buzzer_timeout_task();
}

/** 启动日/周 AI 分析任务 */
static void main_loop_start_ai_summary(ai_summary_type_t kind)
{
    ai_request_pending = 1;
    ai_request_deadline = xTaskGetTickCount() + pdMS_TO_TICKS(AI_ANALYSIS_TIMEOUT_MS);
    app_led_ai_wait_start();
#if AI_CHAT_USE_PROXY
    if (!start_ai_summary(kind, &init_ntp, AI_DEVICE_TOKEN)) {
#else
    if (!start_ai_summary(kind, &init_ntp, AI_CHAT_API_KEY)) {
#endif
        app_led_ai_wait_stop();
        app_led_ai_result_show(false, 3);
        ai_request_pending = 0;
    }
}

static void main_loop_handle_ai_requests(void)
{
    if (ai_day_start) {
        ai_day_start = 0;
        if (!is_wifi_connected()) {
            app_led_ai_result_show(false, 3);
        } else if (!ai_summary_rate_limit_ok(AI_SUMMARY_DAILY)) {
            int remain = ai_summary_cooldown_remain_sec(AI_SUMMARY_DAILY);
            printf("[AI Demo] 每日总结冷却中，剩余 %d 秒\r\n", remain);
            app_led_ai_cooldown_show(3);
        } else {
            main_loop_start_ai_summary(AI_SUMMARY_DAILY);
        }
    }
    if (ai_week_start) {
        ai_week_start = 0;
        if (!is_wifi_connected()) {
            app_led_ai_result_show(false, 3);
        } else if (!ai_summary_rate_limit_ok(AI_SUMMARY_WEEKLY)) {
            int remain = ai_summary_cooldown_remain_sec(AI_SUMMARY_WEEKLY);
            printf("[AI Demo] 每周总结冷却中，剩余 %d 秒\r\n", remain);
            app_led_ai_cooldown_show(3);
        } else {
            main_loop_start_ai_summary(AI_SUMMARY_WEEKLY);
        }
    }
}

/** AI 结果就绪：同步缓存并刷新 screen_ai */
static void main_loop_handle_ai_results(void)
{
    if (!g_ai_daily_result_ready && !g_ai_weekly_result_ready) {
        return;
    }

    bool is_daily = (bool)g_ai_daily_result_ready;
    if (is_daily) {
        g_ai_daily_result_ready = false;
    } else {
        g_ai_weekly_result_ready = false;
    }

    ai_result_t *src = is_daily ? &g_ai_daily_result : &g_ai_weekly_result;
    ui_ai_result_t *dst = is_daily ? &g_ui_daily_result : &g_ui_weekly_result;
    memcpy(dst, src, sizeof(ui_ai_result_t));

    ai_result_t *r = src;
    if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        if (guider_ui.screen_ai != NULL && lv_obj_is_valid(guider_ui.screen_ai) &&
            lv_scr_act() == guider_ui.screen_ai) {
            lv_label_set_text(guider_ui.screen_ai_label_cat1, r->cat1_name);
            lv_label_set_text(guider_ui.screen_ai_label_cat2, r->cat2_name);
            lv_label_set_text(guider_ui.screen_ai_label_cat3, r->cat3_name);
            lv_label_set_text_fmt(guider_ui.screen_ai_label_cat1_num, "%d%%", r->cat1_percent);
            lv_label_set_text_fmt(guider_ui.screen_ai_label_cat2_num, "%d%%", r->cat2_percent);
            lv_label_set_text_fmt(guider_ui.screen_ai_label_cat3_num, "%d%%", r->cat3_percent);
            lv_arc_set_value(guider_ui.screen_ai_arc_1, r->cat1_percent);
            lv_arc_set_value(guider_ui.screen_ai_arc_2, r->cat2_percent);
            lv_arc_set_value(guider_ui.screen_ai_arc_3, r->cat3_percent);
            lv_label_set_text(guider_ui.screen_ai_label_sum, r->summary);
            lv_label_set_text(guider_ui.screen_ai_label_quote, r->quote);
        }
        xSemaphoreGive(lvgl_mutex);
    }

    if (ai_request_pending) {
        app_led_ai_wait_stop();
        app_led_ai_result_show(true, 3);
        ai_request_pending = 0;
    }
}

/** AI 超时视为失败 */
static void main_loop_handle_ai_timeout(void)
{
    if (!ai_request_pending || xTaskGetTickCount() <= ai_request_deadline) {
        return;
    }
    app_led_ai_wait_stop();
    app_led_ai_result_show(false, 3);
    ai_request_pending = 0;
}

static void main_loop_handle_ble_ui(void)
{
    if (ble_ui_refresh_req) {
        ble_ui_refresh_req = 0;
        if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            screen_setting_ble_refresh_icons();
            xSemaphoreGive(lvgl_mutex);
        }
    }
    if (todo_ble_ui_refresh_label) {
        todo_ble_ui_refresh_label = 0;
        ui_todo_list = g_todo_list;
        if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            todo_ble_sync_ui_refresh();
            xSemaphoreGive(lvgl_mutex);
        }
    }
}

/** 归档后刷新 donelist 分页 */
/** 单词卡切词：主循环 2ms 轮询，比后台线程更快响应 */
static void main_loop_handle_wordcard_ui(void)
{
    if (!wordcard_change && !reviewcard_change) {
        return;
    }
    if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(20)) != pdTRUE) {
        return;
    }
    if (wordcard_change) {
        wordcard_change = 0;
        app_ui_wordcard_process_change();
    }
    if (reviewcard_change) {
        reviewcard_change = 0;
        app_ui_reviewcard_process_change();
    }
    xSemaphoreGive(lvgl_mutex);
}

static void main_loop_handle_donelist_reload(void)
{
    if (!donelist_load_label) {
        return;
    }
    donelist_load_label = 0;
    if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(50)) != pdTRUE) {
        return;
    }
    if (guider_ui.screen_donelist != NULL &&
        lv_obj_is_valid(guider_ui.screen_donelist) &&
        lv_scr_act() == guider_ui.screen_donelist) {
        done_total_tasks = ui_done_list.count;
        donelist_page_num = done_total_tasks / 4;
        if ((done_total_tasks % 4 != 0) || (done_total_tasks == 0)) {
            donelist_page_num += 1;
        }
        if (donelist_page_cur < 1) {
            donelist_page_cur = 1;
        }
        if (donelist_page_cur > donelist_page_num) {
            donelist_page_cur = donelist_page_num;
        }
        lv_label_set_text_fmt(guider_ui.screen_donelist_label_donenum, "%d", done_total_tasks);
        lv_label_set_text_fmt(guider_ui.screen_donelist_label_nosavenum, "%d", done_sum);
        lv_label_set_text_fmt(guider_ui.screen_donelist_label_page, "%d/%d",
                              donelist_page_cur, donelist_page_num);
        donelist_render_page(donelist_page_cur);
    }
    xSemaphoreGive(lvgl_mutex);
}

void app_main_loop_once(void)
{
    main_loop_handle_wordcard_ui();
    main_loop_handle_answer_book();
    main_loop_handle_countdown_timeout();
    main_loop_handle_ai_requests();
    main_loop_handle_ai_results();
    main_loop_handle_ai_timeout();
    main_loop_handle_ble_ui();
    main_loop_handle_donelist_reload();
}

void app_main_loop_lvgl_tick(void)
{
    if (xSemaphoreTake(lvgl_mutex, portMAX_DELAY) == pdTRUE) {
        lv_task_handler();
        xSemaphoreGive(lvgl_mutex);
    }
}
