/**
 * @file app_main_loop.c
 * @brief 主循环事件分发（原 app_main while(1) 内逻辑）
 */
#include "app_main_loop.h"
#include "app_globals.h"
#include "app_led.h"
#include "app_time_sync.h"
#include "app_ui.h"
#include "bsp_ble.h"
#include "custom.h"
#include "focus_bridge.h"
#include "gash_machine.h"
#include "keep_machine.h"
#include "book_machine.h"
#include "game_machine.h"
#include "voice_todo.h"
#include "boss_ui.h"
#include "inspiration_ui.h"
#include "esp_random.h"
#include "todo_storage.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/** 答案之书：由 UI 置位，主循环写入随机数并启动 loading 流水灯 */
static void main_loop_handle_answer_book(void)
{
    if (!answer_book_rand_req) {
        return;
    }
    answer_book_rand_req = 0;
    answer_book_rand = esp_random();
    app_led_answer_loading_start();
}

/** 答案之书 loading 结束：停止流水灯 */
static void main_loop_handle_answer_loading_led(void)
{
    if (!answer_loading_led_stop_req) {
        return;
    }
    answer_loading_led_stop_req = 0;
    app_led_answer_loading_stop();
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

static void main_loop_handle_ble_ui(void)
{
    if (ble_ui_refresh_req) {
        if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            ble_ui_refresh_req = 0;
            screen_setting_ble_refresh_icons();
            xSemaphoreGive(lvgl_mutex);
        }
    }
    if (todo_ble_ui_refresh_label) {
        if (todo_storage_lock(pdMS_TO_TICKS(100))) {
            ui_todo_list = g_todo_list;
            todo_storage_unlock();
            if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
                todo_ble_ui_refresh_label = 0;
                todo_ble_sync_ui_refresh();
                xSemaphoreGive(lvgl_mutex);
            }
        }
    }
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

static void main_loop_handle_battery_plug_ui(void)
{
    if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(20)) != pdTRUE) {
        return;
    }
    app_ui_battery_poll_plug_change();
    xSemaphoreGive(lvgl_mutex);
}

static void main_loop_handle_phone_time_sync(void)
{
    static bool ui_refresh_pending;

    if (bsp_ble_time_poll()) {
        keep_machine_refresh_today();
        ui_refresh_pending = true;
    }
    if (!ui_refresh_pending) {
        return;
    }
    if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(50)) != pdTRUE) {
        return;
    }
    app_ui_rtc_labels_refresh_now();
    ui_refresh_pending = false;
    xSemaphoreGive(lvgl_mutex);
}

void app_main_loop_once(void)
{
    main_loop_handle_phone_time_sync();
    app_time_sync_poll();
    main_loop_handle_answer_book();
    main_loop_handle_answer_loading_led();
    main_loop_handle_countdown_timeout();
    focus_bridge_on_countdown_complete();
    keep_machine_tick();
    main_loop_handle_battery_plug_ui();
    main_loop_handle_ble_ui();
    main_loop_handle_donelist_reload();
    /* Game LEDs use the strip mutex only; never wait for RMT with LVGL locked. */
    game_machine_led_poll();
    /* Commit new records only while the game is stopped, without the LVGL lock. */
    game_machine_storage_poll();
}

void app_main_loop_lvgl_tick(void)
{
    if (xSemaphoreTake(lvgl_mutex, portMAX_DELAY) == pdTRUE) {
        lv_task_handler();
        /* Guider 切屏会先 clean 主页按钮，CLICKED 里的 custom_init 到不了。
         * setup_scr 之后立刻在这里接上扭蛋机。 */
        gash_machine_poll();
        keep_machine_poll();
        book_machine_poll();
        game_machine_poll();
        voice_todo_poll();
        inspiration_ui_poll();
        boss_ui_poll();
        xSemaphoreGive(lvgl_mutex);
    }
}
