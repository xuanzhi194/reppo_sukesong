#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <string.h>
#include "driver/gpio.h"
#include "lcd.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lv_tick.h"
#include "gui_guider.h"
#include "custom.h"
#include "events_init.h"
#include "keypad.h"
#include "wifi.h"
#include "nvs_flash.h"
#include "buzzer.h"
#include "word_storage.h"
#include "todo_storage.h"
#include "bat_percent.h"
#include "ds3231.h"
#include "ws2812b.h"
#include "ec06.h"
#include "bsp_ble.h"
#include "ai_demo_task.h"
#include "ai_chat_secrets.h"
#include "esp_random.h"

const char *week_days[7] = {
    "Sunday",   // 0
    "Monday",   // 1
    "Tuesday",  // 2
    "Wednesday",// 3
    "Thursday", // 4
    "Friday",   // 5
    "Saturday"  // 6
};
lv_ui guider_ui;
lv_timer_t * bat_timer = NULL;
SemaphoreHandle_t lvgl_mutex = NULL;  // Protects all LVGL API calls across threads
bool init_ntp = 0;
bool buzzer_timeout_task_running = 0;
bool timeout_alarm_light_running = 0;
bool ai_wait_light_running = 0;
bool ai_request_pending = 0;
TickType_t ai_request_deadline = 0;
TaskHandle_t ai_wait_light_task_handle = NULL;
#define AI_ANALYSIS_TIMEOUT_MS 12000

static int s_last_review_cards_num = 0;

/* 复查 timing 屏当前是否仍是活动屏且未被销毁，避免在快速切屏时访问悬空指针 */
static inline bool timing_screen_is_alive(void)
{
    return guider_ui.screen_timing != NULL
        && lv_obj_is_valid(guider_ui.screen_timing)
        && lv_scr_act() == guider_ui.screen_timing;
}

/* 复查 wordcard / reviewcard 屏是否仍是当前屏，避免快速切屏访问悬空对象 */
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

static void wordcard_render(const word_session_state_t *st)
{
    if (!wordcard_screen_is_alive()) return;
    if (st->finished || !st->valid) return;
    lv_obj_clear_state(guider_ui.screen_wordcard_btn_dui, LV_STATE_DISABLED);
    lv_obj_clear_state(guider_ui.screen_wordcard_btn_cuo, LV_STATE_DISABLED);
    lv_label_set_text(guider_ui.screen_wordcard_label_spelling, st->word.word);
    lv_label_set_text(guider_ui.screen_wordcard_label_phonetic, st->word.phonetic);
    lv_label_set_text(guider_ui.screen_wordcard_label_meaning, st->word.definition);
}

static void reviewcard_render(const word_session_state_t *st)
{
    if (!reviewcard_screen_is_alive()) return;
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
// timeout alarm task, including buzzer alarm and flow light alarm
static void timeout_alarm_light_task(void *param)
{
    while(timeout_alarm_light_running){
        set_flow_light(30, 52, 83);
    }
    
    vTaskDelete(NULL);
}
//buzzer timeout alarm task, including buzzer alarm and flow light alarm
static void buzzer_timeout_task(void *param)
{
    timeout_alarm_light_running = 1;
    xTaskCreatePinnedToCore(timeout_alarm_light_task, "timeout_alarm_light_task", 2048, NULL, 7, NULL, 1);
    buzzer_timeout();
    timeout_alarm_light_running = 0;
    buzzer_timeout_task_running = 0;
    vTaskDelete(NULL);

    
}

static void ai_wait_light_task(void *param)
{
    while(ai_wait_light_running){
        /* AI 分析等待中：持续蓝色流水灯 */
        set_flow_light(30, 52, 83);
    }
    ai_wait_light_task_handle = NULL;
    vTaskDelete(NULL);
}

static void ai_wait_light_start(void)
{
    if (ai_wait_light_running || ai_wait_light_task_handle != NULL) return;
    ai_wait_light_running = 1;
    xTaskCreatePinnedToCore(ai_wait_light_task, "ai_wait_light_task", 2048, NULL, 7, &ai_wait_light_task_handle, 0);
}

static void ai_wait_light_stop(void)
{
    ai_wait_light_running = 0;
    for (int i = 0; i < 100 && ai_wait_light_task_handle != NULL; i++) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static void ai_result_light_show(bool success,uint8_t retry_count)
{
    if (success) {
        for(int i = 0; i < retry_count; i++) {
            set_flow_light(34, 80, 32);  // green
        }
    } else {
        for(int i = 0; i < retry_count; i++) {
            set_flow_light(80, 32, 33);  // red
        }
    }
    set_flow_light(0, 0, 0);         // off
}

static void screen_timing_status_update(bool wifi_connected)
{
    /* timing 屏可能尚未创建 / 已被销毁，所有子对象指针逐个校验，避免悬空指针 */
    if (!timing_screen_is_alive()) return;

    lv_obj_t *img_on  = guider_ui.screen_timing_img_wificonnect;
    lv_obj_t *img_off = guider_ui.screen_timing_img_wifidis;
    lv_obj_t *bar     = guider_ui.screen_timing_bar_battery;

    if (img_on != NULL && lv_obj_is_valid(img_on) &&
        img_off != NULL && lv_obj_is_valid(img_off)) {
        if (wifi_connected) {
            lv_obj_clear_flag(img_on,  LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag  (img_off, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag  (img_on,  LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(img_off, LV_OBJ_FLAG_HIDDEN);
        }
    }

    if (bar != NULL && lv_obj_is_valid(bar)) {
        int bat = get_battery_percentage();
        if (bat < 0) bat = 0;
        if (bat > 100) bat = 100;
        lv_bar_set_value(bar, bat, LV_ANIM_ON);
    }
}

//------------lvgl timer to battery show----------//
void my_battery_update_cb(lv_timer_t * timer) {
    lv_obj_t * act_scr = lv_scr_act();
    if (act_scr == NULL) return;
    if (guider_ui.screen_main != NULL && act_scr == guider_ui.screen_main) {
        lv_obj_t * arc = guider_ui.screen_main_arc_battery;
        if(arc != NULL && lv_obj_is_valid(arc)) {
            int bat = get_battery_percentage();
            lv_arc_set_value(arc, bat);
        }
    } else if (guider_ui.screen_timing != NULL && act_scr == guider_ui.screen_timing) {
        screen_timing_status_update(is_wifi_connected());
    }
}
//------------lvgl timer to realtime clock show----//
static void realtime_clock_update_cb(lv_timer_t *timer) {
    if (lv_scr_act() != guider_ui.screen_main) return;
    if (guider_ui.screen_main_btn_realtime_label == NULL ||
        !lv_obj_is_valid(guider_ui.screen_main_btn_realtime_label)) return;
    ds3231_time_t now;
    if (ds3231_get_time(I2C_NUM_0, &now) != ESP_OK) return;
    lv_label_set_text_fmt(guider_ui.screen_main_btn_realtime_label, "%02d:%02d",
                          now.hour, now.minute);
}
//------------nvs flash task thread---------------//
void nvs_thread(void *param){
    nvs_handle_t timing_handle;
    nvs_open("Timing", NVS_READWRITE,&timing_handle);
     while (1)
     {
        if(todo_back_label){ //done flags already saved to NVS by set_done()
            printf("Saving done flags to NVS...\n");
            g_todo_list = ui_todo_list; // Sync back the done flags from UI to Storage struct
            _nvs_save_done();
            todo_back_label = 0;
        }else if(main_load_label){ //reload task list from CSV + NVS done flags
            main_load_label = 0;
        }else if(todo_archive_label){ // user triggered archiving
            todo_archive_label = 0;
            printf("Archiving completed tasks...\n");
            // Sync struct to get the latest done state just to be safe
            g_todo_list = ui_todo_list;
            todo_storage_archive_done_tasks();
            // Update ui list to reflect removals
            ui_todo_list = g_todo_list;
            done_sum = todo_storage_get_done_count();
            todo_sum = g_todo_list.count - done_sum;
            // 重新加载 task_done.csv 到内存，通知主循环刷新 UI
            donelist_load_from_csv();
            donelist_load_label = 1;
        }else if(led_green_en){
            led_green_en = 0;
            set_flow_light(34, 80, 32);
        }else if(led_red_en){
            led_red_en = 0;
            set_flow_light(80, 32, 33);
        }else if(ble_toggle_on_req){
            ble_toggle_on_req = 0;
            bsp_ble_init();
            start_ble_flow_light();
        }else if(ble_toggle_off_req){
            ble_toggle_off_req = 0;
            stop_ble_flow_light();
            bsp_ble_deinit();
        }else if(ble_disconnect_req){
            ble_disconnect_req = 0;
            bsp_ble_disconnect();
        }

        if(wordcard_change || reviewcard_change){
            if(xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(50)) == pdTRUE){
                if(wordcard_change){
                    wordcard_change = 0;
                    if(wordcard_session_start){
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

                if(reviewcard_change){
                    reviewcard_change = 0;
                    if(reviewcard_session_start){
                        reviewcard_session_start = 0;
                        word_storage_review_start();
                    }
                    word_session_state_t st;
                    word_storage_review_signal(&st);
                    review_cards_num = word_storage_get_review_count();
                    s_last_review_cards_num = review_cards_num;
                    reviewcard_render(&st);
                }

                xSemaphoreGive(lvgl_mutex);
            }
        }
        vTaskDelay(10);
     }

}
//------------Get real time thread---------------//
void wifi_time_thread(void *arg)
{
    while (1)
    {
        if(task_real_time_en){
            if(is_wifi_connected()){
                time_t now;
                struct tm timeinfo;
                time(&now);
                localtime_r(&now, &timeinfo);
                // copy info to flash
                timing_year = timeinfo.tm_year + 1900;
                timing_month = timeinfo.tm_mon + 1;
                timing_day = timeinfo.tm_mday;
                timing_weekday = timeinfo.tm_wday;
                //ESP_LOGI("TIMING","%04d-%02d-%02d %s\r\n",timing_year,timing_month,timing_day,week_days[timeinfo.tm_wday]);
                ESP_LOGI("REAL_TIME", "Current time: %04d-%02d-%02d %02d:%02d:%02d",
                timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
                timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
                char date_str[30];   // "YYYY/MM/DD" + '\0'
                char time_str[20];    // "HH:MM:SS"   + '\0'
                snprintf(date_str, sizeof(date_str), "%04d-%02d-%02d",
                        timeinfo.tm_year + 1900,
                        timeinfo.tm_mon + 1,
                        timeinfo.tm_mday);
                snprintf(time_str, sizeof(time_str), "%02d:%02d",
                        timeinfo.tm_hour,
                        timeinfo.tm_min);
                
                if(xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(50)) == pdTRUE){
                    /* 拿到锁后复查：用户可能已经离开页面，对象已被销毁 */
                    if (timing_screen_is_alive()) {
                        if(timeinfo.tm_year+1900 >= 2026){
                            lv_label_set_text(guider_ui.screen_timing_label_date,date_str);
                            lv_label_set_text(guider_ui.screen_timing_label_realtime,time_str);
                            lv_label_set_text(guider_ui.screen_timing_label_day,week_days[timeinfo.tm_wday]);
                        }
                        screen_timing_status_update(true);
                    }
                    xSemaphoreGive(lvgl_mutex);
                }
                //calibration time to ds3231
                if(timeinfo.tm_year+1900 >= 2026){
                    ds3231_time_t set_time = {
                        .second = timeinfo.tm_sec, .minute = timeinfo.tm_min, .hour = timeinfo.tm_hour,
                        .day = timeinfo.tm_wday, .date = timeinfo.tm_mday, .month = timeinfo.tm_mon+1, .year = timeinfo.tm_year
                    };
                    ds3231_set_time(I2C_NUM_0, &set_time);
                }
                vTaskDelay(990);
            }else{
                ds3231_time_t now = {0};
                esp_err_t rtc_err = ds3231_get_time(I2C_NUM_0, &now);
                char date_str[30];
                char time_str[20];
                snprintf(date_str, sizeof(date_str), "%04d-%02d-%02d",
                        now.year,
                        now.month,
                        now.date);
                snprintf(time_str, sizeof(time_str), "%02d:%02d",
                        now.hour,
                        now.minute);
                int wday = (rtc_err == ESP_OK && now.day < 7) ? now.day : 0;
                if(xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(50)) == pdTRUE){
                    if (timing_screen_is_alive()) {
                        if (rtc_err == ESP_OK) {
                            lv_label_set_text(guider_ui.screen_timing_label_date,date_str);
                            lv_label_set_text(guider_ui.screen_timing_label_realtime,time_str);
                            lv_label_set_text(guider_ui.screen_timing_label_day,week_days[wday]);
                        }
                        screen_timing_status_update(false);
                    }
                    xSemaphoreGive(lvgl_mutex);
                }
                vTaskDelay(990);
            }
        }else if(wifi_scan_en){
            //-----------------------scan wifi---------------------//
            wifi_scan_top_5();
            // wifi item btn label modify from wifi scan info
            if(xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(100)) == pdTRUE){
                if (wificfg_screen_is_alive() &&
                    guider_ui.screen_wificfg_list_1 != NULL &&
                    lv_obj_is_valid(guider_ui.screen_wificfg_list_1)) {
                    for(int i = 0; i < g_scan_ap_num ; i++){
                        lv_obj_t * list_btn = lv_obj_get_child(guider_ui.screen_wificfg_list_1, i);
                        if (list_btn == NULL || !lv_obj_is_valid(list_btn)) continue;
                        lv_obj_t * label = lv_obj_get_child(list_btn, -1);
                        if(label != NULL && lv_obj_is_valid(label)) {
                            lv_label_set_text(label,g_scan_results[i].ssid);
                        }
                    }
                }
                xSemaphoreGive(lvgl_mutex);
            }
            wifi_scan_en = 0;
        }else if(wifi_connect_en){
            //------------------connect to wifi------------------//
            // Read password under mutex, then connect without holding it
            char wifi_pw_buf[64] = {0};
            if(xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(100)) == pdTRUE){
                const char * wifi_password = lv_textarea_get_text(guider_ui.screen_wificfg_ta_password);
                strncpy(wifi_pw_buf, wifi_password, sizeof(wifi_pw_buf)-1);
                xSemaphoreGive(lvgl_mutex);
            }
            wifi_connect_to_new_network(wifi_ssid, wifi_pw_buf);
            vTaskDelay(5000);
            wifi_success = is_wifi_connected();
            if(xSemaphoreTake(lvgl_mutex, portMAX_DELAY) == pdTRUE){
                if(wifi_success){
                    wifi_connect_show();
                    if(init_ntp == 0){
                        initialize_sntp();
                        init_ntp = 1;
                    }
                }else{
                    wifi_disconnect_show();
                }
                xSemaphoreGive(lvgl_mutex);
            }
            wifi_connect_en = 0; 
        }else if(wifi_disconnect_en){
            esp_wifi_disconnect();
            wifi_disconnect_en = 0;
            wifi_success = 0;
        }
        vTaskDelay(10);
    }
}
//-------------main loop-------------------------//
void app_main(void)
{
    //-----------RGB boot flow light (non-blocking)---//
    init_led_strip();
    start_boot_led_blink();
    //-----------encoder init-------------------//
    init_encoder_direction();
    //-----------buzzer init--------------------//
    buzzer_init();
    //-----------battery show-------------------//
    battery_adc_init();
    //-----------DS3231 INIT--------------------//
    ds3231_init(I2C_NUM_0, 15, 16); // GPIO 15 (SDA) and GPIO 16 (SCL)
    //--------------NVS init--------------------//
    //initialize nvs flash
    nvs_flash_init();
    // 加载上次 AI 分析结果（掉电保存），供 UI 显示
    ai_daily_result_load();
    ai_weekly_result_load();
    // 同步到 UI 缓存（custom 层不依赖 ai_demo_task.h）
    memcpy(&g_ui_daily_result,  &g_ai_daily_result,  sizeof(ui_ai_result_t));
    memcpy(&g_ui_weekly_result, &g_ai_weekly_result, sizeof(ui_ai_result_t));
    //----------init spiffs for word card-----------//
    init_spiffs();
    word_storage_init();
    int word_total = word_storage_get_total_count();
    total_cards_num  = (word_total > 0) ? word_total : 1; /* avoid /0 in UI */
    review_cards_num = word_storage_get_review_count();
    s_last_review_cards_num = review_cards_num;
    //----------init todo storage (SPIFFS + NVS)----//
    todo_storage_init();
    ui_todo_list = g_todo_list;  // sync initial todo list from storage
    done_sum = todo_storage_get_done_count();
    todo_sum = g_todo_list.count - done_sum;
    //----------wifi cfg init-----------------//
    //wifi sta mode
    init_wifi();
    init_ntp = 0;
    // while(!is_wifi_connected()){
    //      wifi_connect_to_new_network("wphone", "seanhhhh");
    //      vTaskDelay(3000);
    //      wifi_success = is_wifi_connected();
    // }
    // ------------create LVGL mutex-----------//
    lvgl_mutex = xSemaphoreCreateMutex();
    // ------------set lvgl by gui guider-----------//
    // LVGL initialize
    lvgl_tick_timer_init(); // init lvgl timer
    lv_init();              // init lvgl
    lv_port_disp_init();    // init lvgl display cfg
    lv_port_indev_init();   // init lvgl indev
    setup_ui(&guider_ui);   // connect to gui guider 
    lv_indev_set_group(indev_keypad, g);

    // Create battery timer once — runs every 5s, updates only when on screen_main
    bat_timer = lv_timer_create(my_battery_update_cb, 5000, NULL);
    // Create realtime clock timer — runs every 3s, updates btn_realtime on screen_main
    lv_timer_create(realtime_clock_update_cb, 3000, NULL);

    //-------------multi task thread----------------//
    xTaskCreatePinnedToCore(nvs_thread,"nvs_thread",8192,NULL,10,NULL,0);
    xTaskCreatePinnedToCore(wifi_time_thread,"time_thread",4096,NULL,10,NULL,1);

    //--------------main loop task-----------------//
    while (1) 
    {
        //------------答案之书随机数----------------//
        if (answer_book_rand_req) {
            answer_book_rand_req = 0;
            answer_book_rand = esp_random();
        }

        //------------buzzer timeout alarm trigger----------------//
        if(countdown_timeout_pending && !buzzer_timeout_task_running){
            countdown_timeout_pending = 0;
            buzzer_timeout_task_running = 1;
            xTaskCreatePinnedToCore(buzzer_timeout_task, "buzzer_timeout_task", 2048, NULL, 8, NULL, 0);
        }

        // -------------- AI 请求触发 --------------//
        if (ai_day_start) {
            ai_day_start = 0;
            if (!is_wifi_connected()) {
                ai_result_light_show(false, 3);
            } else {
            ai_request_pending = 1;
            ai_request_deadline = xTaskGetTickCount() + pdMS_TO_TICKS(AI_ANALYSIS_TIMEOUT_MS);
            ai_wait_light_start();
            start_ai_summary(AI_SUMMARY_DAILY, &init_ntp, AI_CHAT_API_KEY);
            }
        }
        if (ai_week_start) {
            ai_week_start = 0;
            if (!is_wifi_connected()) {
                ai_result_light_show(false, 3);
            } else {
            ai_request_pending = 1;
            ai_request_deadline = xTaskGetTickCount() + pdMS_TO_TICKS(AI_ANALYSIS_TIMEOUT_MS);
            ai_wait_light_start();
            start_ai_summary(AI_SUMMARY_WEEKLY, &init_ntp, AI_CHAT_API_KEY);
            }
        }
        // ------------ AI 结果就绪，刷新 UI ----------//
        if (g_ai_daily_result_ready || g_ai_weekly_result_ready) {
            bool is_daily = (bool)g_ai_daily_result_ready;
            if (is_daily) g_ai_daily_result_ready  = false;
            else          g_ai_weekly_result_ready = false;
            ai_result_t    *src = is_daily ? &g_ai_daily_result  : &g_ai_weekly_result;
            ui_ai_result_t *dst = is_daily ? &g_ui_daily_result  : &g_ui_weekly_result;
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
                ai_wait_light_stop();
                ai_result_light_show(true, 3);
                ai_request_pending = 0;
            }
        }
        /* 无法直接获知 ai_demo 内部失败状态，这里用超时作为失败判定 */
        if (ai_request_pending && xTaskGetTickCount() > ai_request_deadline) {
            ai_wait_light_stop();
            ai_result_light_show(false, 3);
            ai_request_pending = 0;
        }
        
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

        /* donelist 异步刷新：归档完成后重新渲染当前页 */
        if (donelist_load_label) {
            donelist_load_label = 0;
            if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(50)) == pdTRUE) {
                if (guider_ui.screen_donelist != NULL &&
                    lv_obj_is_valid(guider_ui.screen_donelist) &&
                    lv_scr_act() == guider_ui.screen_donelist) {
                    done_total_tasks = ui_done_list.count;
                    donelist_page_num = done_total_tasks / 4;
                    if ((done_total_tasks % 4 != 0) || (done_total_tasks == 0)) donelist_page_num += 1;
                    if (donelist_page_cur < 1) donelist_page_cur = 1;
                    if (donelist_page_cur > donelist_page_num) donelist_page_cur = donelist_page_num;
                    lv_label_set_text_fmt(guider_ui.screen_donelist_label_donenum,  "%d", done_total_tasks);
                    lv_label_set_text_fmt(guider_ui.screen_donelist_label_nosavenum, "%d", done_sum);
                    lv_label_set_text_fmt(guider_ui.screen_donelist_label_page,      "%d/%d", donelist_page_cur, donelist_page_num);
                    lv_label_set_text_fmt(guider_ui.screen_donelist_label_page,      "%d/%d", donelist_page_cur + 1, donelist_page_num);
                    donelist_render_page(donelist_page_cur);
                }
                xSemaphoreGive(lvgl_mutex);
            }
        }
        // -----------------LVGL task managing-----------------//
        if(xSemaphoreTake(lvgl_mutex, portMAX_DELAY) == pdTRUE){
            lv_task_handler();  // LVGL task managing
            xSemaphoreGive(lvgl_mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }
    
}

 