/*
* Copyright 2023 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/


/*********************
 *      INCLUDES
 *********************/
 #include <stdio.h>
 #include <string.h>
 #include "gui_guider.h"
 #include "lvgl.h"
 #include "custom.h"
 #include "ui_locale.h"
 #include "widgets_init.h"
 #include <stdlib.h>
 #ifdef ESP_PLATFORM
 #include "ws2812b.h"
 #include "lcd.h"
 #include "bat_percent.h"
 #include "buzzer.h"
 #endif
 /* Medium_28 随单词页从 Guider 工程移除；时钟/Sleep 用两边都还在的 30 号字。 */
 #define CUSTOM_DISPLAY_FONT lv_font_Abel_regular_30
 
 //lv group define
 lv_group_t *g = NULL;
 //todolist variables (debug seed data for GUI Guider preview)
 todo_list_t ui_todo_list = {
     .count = 8,
     .items = {
         {.name = "Morning standup",   .duration_min = 15, .done = 1, .planned_date = "2026-05-09", .label = TODO_LABEL_WORK},
         {.name = "Review PR #128",    .duration_min = 25, .done = 1, .planned_date = "2026-05-09", .label = TODO_LABEL_WORK},
         {.name = "Fix WiFi reconnect",.duration_min = 45, .done = 0, .planned_date = "", .label = TODO_LABEL_WORK},
         {.name = "LVGL animation tune", .duration_min = 30, .done = 0, .planned_date = "", .label = TODO_LABEL_NONE},
         {.name = "Write test notes",  .duration_min = 20, .done = 0, .planned_date = "", .label = TODO_LABEL_STUDY},
         {.name = "Refactor timer flow", .duration_min = 35, .done = 0, .planned_date = "", .label = TODO_LABEL_WORK},
         {.name = "Sync NVS defaults", .duration_min = 18, .done = 0, .planned_date = "", .label = TODO_LABEL_NONE},
         {.name = "Polish main screen",.duration_min = 40, .done = 0, .planned_date = "", .label = TODO_LABEL_FUN},
     }
 };
 bool   todosel_back = 0;
 int8_t todo_index = 0;
 int8_t page_num = 2;
 int8_t page_cur = 1;
 bool   done_array[25][4] = {0};
 int8_t done_sum = 2;
 int8_t todo_sum = 6;
 bool todo_back_label = 0;
 bool todo_archive_label = 0;
 bool main_load_label = 0;
 char current_task_name[128] = {0};
 bool volume_en = 0;
 uint8_t brightness_level = 2;
 uint8_t sleep_timeout_level = 0;
 bool ble_success = 0;
 bool ble_enabled = 0;
 volatile bool ble_toggle_on_req = 0;
 volatile bool ble_toggle_off_req = 0;
 volatile bool ble_disconnect_req = 0;
 bool ble_flow_led_en = 0;
 volatile bool ble_ui_refresh_req = 0;
 volatile bool todo_ble_ui_refresh_label = 0;
 //timer variables
 int16_t timer_all_num = 25; //minutes
 uint32_t timer_all_seconds = 0;
 lv_timer_t *pomodoro_timer = NULL;
 bool pomodoro_countdown = 0;
 bool pomodoro_stopwatch = 0;
 bool countdown_start_label = 0;
 bool countdown_process_label = 0;
 bool stopwatch_start_label = 0;
 bool stopwatch_process_label = 0;
 volatile bool countdown_timeout_pending = 0;
 int  current_task_index = 0;
 //timing variablesreview_card
 volatile bool task_real_time_en = 0;
 bool timing_back_en = 0;
 int16_t timing_year = 0;
 int16_t timing_month = 0;
 int16_t timing_day = 0;
 int16_t timing_weekday = 0;
 //wificfg variable
 volatile bool keyboard_start_en = 0;
 volatile bool wifi_scan_en = 0;
 volatile bool wifi_disconnect_en = 0;
 volatile bool wifi_connect_en = 0;
 volatile bool wifi_load_en = 0;
 volatile bool wifi_success = 0;
 char *wifi_ssid = NULL;
 char wifi_connect_ssid[33] = {0};
 //todosel variable
 bool focus_en = 0;
 bool focus_session_active = 0;
 bool focus_complete_pending = 0;
 int  focus_session_minutes = 0;
 int16_t focus_preset_minutes = 25;
 //wordcard variables
 bool wordcard_change = 0;
 bool reviewcard_change = 0;
 bool wordcard_session_start = 0;
 bool reviewcard_session_start = 0;
 bool led_green_en = 0;
 bool led_red_en = 0;
 int  review_index = 0;
 int  learned_cards_num = 0; //have learned cards num in all cards (NVS)
 int  total_cards_num = 1;   //all cards number (synced from wordbook)
 int  review_cards_num = 0;  // review cards num in all cards (SPIFFS)
 int  cards_index = 0;
 int  round_cards_num = 30;
 //screen_ai variables
 volatile bool ai_day_start = 0;
 volatile bool ai_week_start = 0;
 done_list_t ui_done_list      = {0};
 bool donelist_load_label      = 0;
 uint8_t donelist_page_cur     = 0;
 uint8_t donelist_page_num     = 0;
 int     done_total_tasks      = 0;
 int     all_focus_time = 0;
 int     focus_level_hour[10] = {3,9,15,30,60,100,150,230,350,500};
 int     max_focus_time = 30000;
 /* screen_ai 残留缓存；云端 AI 已移除，events_init 删屏前仍会引用 */
 ui_ai_result_t g_ui_daily_result  = {0};
 ui_ai_result_t g_ui_weekly_result = {0};
 
 bool answer_book_rand_req = 0;
 unsigned int answer_book_rand = 0;
 volatile bool answer_loading_led_stop_req = 0;
 
 static const char * const answer_book_answers[] = {
     /* —— Yes —— */
     "Without a doubt",
     "Yes",
     "Of course",
     "Go for it",
     "The time is now",
     "Worth a try",
     "Take the first step",
     "Fate says: yes",
     "The stars wink yes",
     "The universe likes this",
     "Oracle: great luck",
     "The signs say go",
     "Looks promising",
     "You will have support",
     "It will turn out better",
     "Trust your gut",
     "Now is the moment",
     "Just do it",
     "Luck is coming",
     "The door is open",
     "Go with the flow",
     "Help will appear",
     "Keep going; light ahead",
     "You've got this",
     "Worth the risk",
     "Move forward",
     "The lights are green",
     "YES",
     "Unanimous yes",
     "Swift success",
     "It will fall into place",
     "A way will open",
     "Lucky stars align",
     "No need to ask again",
     "The answer is yes",
     "Absolutely",
     "No problem",
     "A good outcome awaits",
     "Seize this chance",
     "The moon says yes",
     "All signs favor you",
     "Be a little bolder",
     "Follow what you want",
     "A strong start",
     "The worst is over",
     "Your luck is turning",
     "Fortune is on your side",
     "Good things are coming",
     "Kindness will be repaid",
     "Absolutely fine",
     "The outlook is bright",
     "An unexpected gift",
     "Take the leap",
     "Act, don't wait",
     "Lucky stars shine",
     "Wait, then the sky clears",
     "A path will appear",
     "It will work itself out",
     "You know what comes next",
     "Rare chance. Take it",
     "Your luck is strong",
     "The wind is with you",
     "Spring is coming",
     "A new path after this",
     "The hard part is ending",
     "Good fortune at the door",
     /* —— No —— */
     "Better not",
     "Not yet",
     "Think it over again",
     "Don't get your hopes up",
     "The answer is: no",
     "Not for now",
     "Wait a bit longer",
     "The timing is off",
     "The stars shake their heads",
     "The universe says: rethink",
     "Probably not",
     "Don't rush it",
     "Stay still for now",
     "You'll regret it now",
     "Find another way",
     "This path is closed",
     "Bad omen. Be careful",
     "Headwind. Don't go",
     "Mercury retrograde. Pause",
     "Avoid big moves today",
     "Oracle: unlucky",
     "Heaven did not nod today",
     "NO",
     "Unanimous no",
     "Think twice",
     "Haste makes waste",
     "Too much is as bad",
     "Wait. It may flip later",
     "You're climbing the wrong tree",
     "Wrong direction",
     "It's only a mirage",
     "Don't act rashly",
     "Don't force it",
     "Better to forget",
     "It won't go your way",
     "The outlook is unclear",
     "Don't go it alone",
     "Don't lose the big picture",
     "Don't fly into the flame",
     "Don't just wait around",
     "It won't happen on time",
     "You may be disappointed",
     "Don't expect too much",
     "Not the right time",
     "Not meant to be yet",
     "The odds are low",
     "Too much resistance",
     "Trouble is brewing",
     "The clouds haven't cleared",
     "A waning moon. Wait",
     "The door is shut",
     "The road is blocked",
     "This plan won't work",
     "This wish won't come true",
     "Let this one go",
     "Today is not the day",
     "Turn back while you can",
     "Stop before the edge",
     "Stop first. Then fix it",
     "Learn from the last miss",
     "Don't try; it can't be undone",
     "Don't walk into a trap",
     "Don't tie your own hands",
     /* —— Maybe / vague / funny —— */
     "Let it be",
     "Wait and see",
     "Maybe. Maybe not",
     "This will take time",
     "You already know",
     "Fate is off today",
     "The dice landed sideways",
     "Cupid says check again",
     "The mystic called in sick",
     "This question is extra credit",
     "The book needs to think",
     "Don't rush. Still calculating",
     "Schrodinger's answer is",
     "Shake it again",
     "Ask after you sleep",
     "Fate is joking",
     "Too much has been revealed",
     "You asked too many times",
     "The answer is on the next page",
     "This book refuses to answer",
     "Fate is still typing...",
     "Depends on the mood",
     "Depends on fate",
     "Depends on the heavens",
     "Depends on the timing",
     "Depends on the cat",
     "Flip a coin",
     "Universe loading. Please wait",
     "The cat says ask it",
     "The dog says: whatever",
     "The stars say: you guess",
     "The wind blew the answer away",
     "The lots are empty. Later",
     "Almanac: rest, don't act",
     "Luck is stuck in traffic",
     "The book went on break",
     "Fate's keyboard jammed",
     "Yes and also no",
     "Right and also wrong",
     "Whatever. Let it go",
     "You decide",
     "Leave it to fate",
     "Do your part. Then let go",
     "Cannot say. Cannot say",
     "Only half the secret",
     "If you don't get it, ask again",
     "Alright. Turn the page",
     "Asking may get no answer",
     "Answering may get no belief",
     "Or: stop asking. Go live",
     "Stand it up and ask again",
     "Today's quota is used up",
     "Mortal, know your limits",
     "You made the book nervous",
     "The door is ajar. Push it",
     "The moon isn't full. Wait",
     "The cat is still asleep",
     "Cupid is still typing",
     "Bad signal. Try again",
     "Ask again. Same line",
     "The tree says: be still",
     "The cloud says: drift",
     "Three guts tied. Shake again",
     "Figure out the other half",
     "Let your heart decide",
     "A clear conscience is enough",
     "The universe skipped this one",
     "If pigs fly, ask again",
     "Plan it. Then leave the rest",
     "You know. I know. That's it",
     "So: ask one more time",
 };
 
 void answer_book_show_random_answer(void)
 {
     const uint32_t count = (uint32_t)(sizeof(answer_book_answers) / sizeof(answer_book_answers[0]));
     uint32_t index = answer_book_rand % count;
     lv_label_set_text(guider_ui.screen_getanswer_btn_answer_label,
                      ui_answer(index, answer_book_answers[index]));
 }
 
 /* ========== 用户可改参数 ========== */
 #define DELAY_MS         1500          // 延时多久后切屏
 /* ================================= */
 
 static lv_timer_t *delay_timer = NULL;
 
 /* 时间到，切屏 + 清定时器 */
 static void delay_timer_cb(lv_timer_t *t)
 {
     lv_timer_del(t);                                    // 一次性，用完即删
     delay_timer = NULL;
     setup_scr_screen_main(&guider_ui);
     lv_scr_load_anim(guider_ui.screen_main,                     // 目标屏幕
                      LV_SCR_LOAD_ANIM_FADE_ON,          // 动画
                      300, 0, true);                     // 300 ms 淡入
 }
 /* 屏幕 Loaded 事件回调：启动一次性定时器 */
 static void on_screen_loaded(lv_event_t *e)
 {
     (void)e;                                            // 未用
     if(delay_timer) return;                             // 防止重复
     delay_timer = lv_timer_create(delay_timer_cb, DELAY_MS, NULL);
     lv_timer_set_repeat_count(delay_timer, 1);          // 只触发一次
 }
 
 
 //reppo delay x ms trans to log screen
 void screen_reppo_custom_init(){
     lv_obj_add_event_cb(guider_ui.screen_reppo,  on_screen_loaded, LV_EVENT_SCREEN_LOADED, NULL);
 }
 
 // ----------------my_kb_event_cb------------------------------//
 void my_kb_event_cb(lv_event_t * e) {
     lv_event_code_t code = lv_event_get_code(e);
     lv_obj_t * kb = lv_event_get_target(e);
     lv_group_t * g = lv_group_get_default();
     if(code == LV_EVENT_READY || code == LV_EVENT_CANCEL) {
         lv_group_set_editing(g, false);
         lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
         lv_group_focus_obj(guider_ui.screen_wificfg_btn_connect);
         lv_obj_remove_event_cb(kb, my_kb_event_cb);
         keyboard_start_en = 0;
     }
 }
 //-----------------------pomodoro timer------------------------//
 //-------------------------------------------------------------//
 static bool pomodoro_screen_is_alive(void)
 {
     return guider_ui.screen_pomodoro != NULL
         && lv_obj_is_valid(guider_ui.screen_pomodoro)
         && lv_scr_act() == guider_ui.screen_pomodoro;
 }
 
 #define POMODORO_BLACK_LED_COUNT 10U
 
 static bool s_pomodoro_black_active;
 static uint32_t s_pomodoro_black_total_seconds;
 static lv_obj_t *s_pomodoro_black_hit;
 static lv_obj_t *s_pomodoro_black_bound_screen;
 
 static void pomodoro_black_update_leds(void)
 {
 #ifdef ESP_PLATFORM
     if (!s_pomodoro_black_active) {
         return;
     }
 
     uint8_t lit_count = 0;
     if (s_pomodoro_black_total_seconds > 0 && timer_all_seconds > 0) {
         /* 向上取整：开始时 10 颗全亮，剩余时间归零时全灭。 */
         lit_count = (uint8_t)(
             (timer_all_seconds * POMODORO_BLACK_LED_COUNT +
              s_pomodoro_black_total_seconds - 1U) /
             s_pomodoro_black_total_seconds);
         if (lit_count > POMODORO_BLACK_LED_COUNT) {
             lit_count = POMODORO_BLACK_LED_COUNT;
         }
     }
 
     pomodoro_black_flow_set_lit_count(lit_count);
 #endif
 }
 
 static void pomodoro_black_start_leds(void)
 {
 #ifdef ESP_PLATFORM
     start_pomodoro_black_flow_light();
     pomodoro_black_update_leds();
 #endif
 }
 
 static void pomodoro_black_stop_leds(void)
 {
 #ifdef ESP_PLATFORM
     stop_pomodoro_black_flow_light();
     clear_led_bar();
 #endif
 }
 
 static void pomodoro_black_exit(bool focus_start)
 {
     bool was_active = s_pomodoro_black_active;
     s_pomodoro_black_active = false;
 
     if (g != NULL) {
         lv_group_focus_freeze(g, false);
     }
     if (s_pomodoro_black_hit != NULL &&
         lv_obj_is_valid(s_pomodoro_black_hit)) {
         lv_obj_add_flag(s_pomodoro_black_hit, LV_OBJ_FLAG_HIDDEN);
         if (g != NULL) {
             lv_group_remove_obj(s_pomodoro_black_hit);
         }
     }
 
 #ifdef ESP_PLATFORM
     if (was_active) {
         pomodoro_black_stop_leds();
         /* 直接恢复用户保存的亮度，避免先闪到 100% 再回落。 */
         lcd_backlight_set_level(brightness_level);
     }
 #else
     LV_UNUSED(was_active);
 #endif
 
     if (focus_start && pomodoro_screen_is_alive() && g != NULL &&
         guider_ui.screen_pomodoro_btn_start != NULL &&
         lv_obj_is_valid(guider_ui.screen_pomodoro_btn_start)) {
         lv_group_focus_obj(guider_ui.screen_pomodoro_btn_start);
     }
     if (pomodoro_screen_is_alive()) {
         pomodoro_pause_icon_refresh();
     }
 }
 
 static void pomodoro_black_hit_cb(lv_event_t *e)
 {
     if (lv_event_get_code(e) != LV_EVENT_CLICKED ||
         !s_pomodoro_black_active) {
         return;
     }
 
     /* 只退出黑屏，不暂停 timer，也不修改 countdown_process_label。 */
     pomodoro_black_exit(true);
 }
 
 static bool pomodoro_black_ensure_hit(void)
 {
     lv_obj_t *screen = guider_ui.screen_pomodoro;
     if (screen == NULL || !lv_obj_is_valid(screen)) {
         return false;
     }
     if (s_pomodoro_black_hit != NULL &&
         lv_obj_is_valid(s_pomodoro_black_hit)) {
         return true;
     }
 
     s_pomodoro_black_hit = lv_btn_create(screen);
     lv_obj_remove_style_all(s_pomodoro_black_hit);
     lv_obj_set_pos(s_pomodoro_black_hit, 0, 0);
     lv_obj_set_size(s_pomodoro_black_hit, 320, 170);
     lv_obj_set_style_bg_color(s_pomodoro_black_hit, lv_color_hex(0x000000),
                               LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_bg_opa(s_pomodoro_black_hit, LV_OPA_COVER,
                             LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_add_flag(s_pomodoro_black_hit, LV_OBJ_FLAG_CLICKABLE);
     lv_obj_add_flag(s_pomodoro_black_hit, LV_OBJ_FLAG_HIDDEN);
     lv_obj_add_event_cb(s_pomodoro_black_hit, pomodoro_black_hit_cb,
                         LV_EVENT_CLICKED, NULL);
     return true;
 }
 
 static void pomodoro_black_enter(void)
 {
     if (s_pomodoro_black_active || !pomodoro_screen_is_alive() ||
         !pomodoro_countdown || !countdown_start_label ||
         !countdown_process_label || !pomodoro_black_ensure_hit()) {
         return;
     }
 
     if (s_pomodoro_black_total_seconds == 0) {
         s_pomodoro_black_total_seconds = timer_all_seconds;
     }
 
     lv_obj_clear_flag(s_pomodoro_black_hit, LV_OBJ_FLAG_HIDDEN);
     lv_obj_move_foreground(s_pomodoro_black_hit);
     if (g != NULL) {
         lv_group_add_obj(g, s_pomodoro_black_hit);
         lv_group_focus_obj(s_pomodoro_black_hit);
         /* 黑屏期间旋转按键也不会把焦点移回普通控件。 */
         lv_group_focus_freeze(g, true);
     }
 
     s_pomodoro_black_active = true;
 #ifdef ESP_PLATFORM
     lcd_off();
 #endif
     pomodoro_black_start_leds();
 }
 
 static void pomodoro_timer_delete_only(void)
 {
     if (pomodoro_timer != NULL) {
         lv_timer_del(pomodoro_timer);
         pomodoro_timer = NULL;
     }
 }
 
 void pomodoro_timer_stop(void)
 {
     pomodoro_black_exit(false);
     s_pomodoro_black_total_seconds = 0;
     pomodoro_timer_delete_only();
     pomodoro_pause_icon_refresh();
     idle_sleep_note_activity();
 }

 static volatile bool s_idle_sleep_asleep;
 static volatile bool s_motion_wake_req;
 static bool s_idle_sleep_inited;
 static uint32_t s_idle_sleep_last_ms;

 static bool idle_sleep_timer_running(void)
 {
     if (pomodoro_countdown && countdown_start_label && countdown_process_label) {
         return true;
     }
     if (pomodoro_stopwatch && stopwatch_start_label && stopwatch_process_label) {
         return true;
     }
     return false;
 }

 bool focus_timer_is_running(void)
 {
     return s_pomodoro_black_active || idle_sleep_timer_running();
 }

 static bool idle_sleep_blocked(void)
 {
     return focus_timer_is_running();
 }

 __attribute__((weak)) bool voice_todo_wants_wificfg_return(void)
 {
     return false;
 }

 __attribute__((weak)) void voice_todo_return_from_wifi(void)
 {
 }

 void idle_sleep_note_activity(void)
 {
     s_idle_sleep_last_ms = lv_tick_get();
     s_idle_sleep_inited = true;
 }

#ifdef ESP_PLATFORM
 static void idle_sleep_charge_led_refresh(void)
 {
     battery_info_t info;
     int percent;
     uint8_t lit_count;

     battery_monitor_get(&info);
     if (!info.plugged_ui) {
         stop_idle_charge_flow_light();
         clear_led_bar();
         return;
     }

     percent = info.display_percent;
     if (percent < 0) {
         percent = 0;
     } else if (percent > 100) {
         percent = 100;
     }
     lit_count = (uint8_t)((percent * 10 + 99) / 100);
     if (lit_count == 0U) {
         lit_count = 1U;
     }
     idle_charge_flow_update(lit_count, percent > 90);
     start_idle_charge_flow_light();
 }
#endif

 static void idle_sleep_enter(void)
 {
     if (s_idle_sleep_asleep || idle_sleep_blocked()) {
         return;
     }
     s_motion_wake_req = false;
     s_idle_sleep_asleep = true;
#ifdef ESP_PLATFORM
     lcd_off();
     idle_sleep_charge_led_refresh();
#endif
 }

 static void idle_sleep_exit(void)
 {
     if (!s_idle_sleep_asleep) {
         return;
     }
     s_idle_sleep_asleep = false;
     s_motion_wake_req = false;
     idle_sleep_note_activity();
#ifdef ESP_PLATFORM
     stop_idle_charge_flow_light();
     lcd_backlight_set_level(brightness_level);
#endif
 }

 bool idle_sleep_consume_key(uint32_t lv_key)
 {
     if (s_idle_sleep_asleep) {
         if (lv_key == LV_KEY_ENTER) {
             idle_sleep_exit();
         }
         /* 息屏时旋转不唤醒，点按唤醒并吞掉，避免误点当前按钮。 */
         return true;
     }
     idle_sleep_note_activity();
     return false;
 }

 bool idle_sleep_is_asleep(void)
 {
     return s_idle_sleep_asleep;
 }

 void idle_sleep_request_wake(void)
 {
     if (!s_idle_sleep_asleep) {
         return;
     }
     s_motion_wake_req = true;
 }

 void pomodoro_alarm_dismiss(void)
 {
     countdown_timeout_pending = false;
 #ifdef ESP_PLATFORM
     buzzer_alarm_abort();
 #endif
 }

 void idle_sleep_poll(void)
 {
     uint32_t now = lv_tick_get();
     uint16_t minutes;
     uint32_t timeout_ms;

     if (!s_idle_sleep_inited) {
         idle_sleep_note_activity();
         return;
     }
     if (s_idle_sleep_asleep && s_motion_wake_req) {
         s_motion_wake_req = false;
         idle_sleep_exit();
         return;
     }
     minutes = sleep_timeout_get_minutes();
     if (minutes == 0 || s_idle_sleep_asleep) {
#ifdef ESP_PLATFORM
         if (s_idle_sleep_asleep) {
             idle_sleep_charge_led_refresh();
         }
#endif
         return;
     }
     if (idle_sleep_blocked()) {
         s_idle_sleep_last_ms = now;
         return;
     }
     timeout_ms = (uint32_t)minutes * 60000U;
     if ((uint32_t)(now - s_idle_sleep_last_ms) >= timeout_ms) {
         idle_sleep_enter();
     }
 }
 
 void pomodoro_pause_icon_refresh(void)
 {
     lv_obj_t *img = guider_ui.screen_pomodoro_img_pause;
     lv_obj_t *btn = guider_ui.screen_pomodoro_btn_start;
     if (img == NULL || !lv_obj_is_valid(img)) {
         return;
     }
     if (!pomodoro_screen_is_alive()) {
         lv_obj_add_flag(img, LV_OBJ_FLAG_HIDDEN);
         return;
     }
 
     bool running = false;
     if (pomodoro_countdown && countdown_start_label && countdown_process_label) {
         running = true;
     } else if (pomodoro_stopwatch && stopwatch_start_label && stopwatch_process_label) {
         running = true;
     }
 
     if (running) {
         lv_obj_clear_flag(img, LV_OBJ_FLAG_HIDDEN);
         lv_obj_clear_flag(img, LV_OBJ_FLAG_CLICKABLE);
         lv_obj_move_foreground(img);
     } else {
         lv_obj_add_flag(img, LV_OBJ_FLAG_HIDDEN);
     }
 
     if (btn != NULL && lv_obj_is_valid(btn)) {
         lv_opa_t btn_bg_opa = running ? 0 : 34;
         lv_obj_set_style_bg_opa(btn, btn_bg_opa, LV_PART_MAIN | LV_STATE_DEFAULT);
         lv_obj_set_style_bg_opa(btn, btn_bg_opa, LV_PART_MAIN | LV_STATE_FOCUSED);
     }
 }
 
 static void pomodoro_timeout_effect_play(void);
 
 static void countdown_cb(lv_timer_t *t){
     if (!pomodoro_screen_is_alive()) {
         pomodoro_timer_stop();
         return;
     }
     if(timer_all_seconds == 0){
         lv_timer_del(t);
         pomodoro_timer = NULL;
         pomodoro_black_exit(false);
         s_pomodoro_black_total_seconds = 0;
         if (!pomodoro_screen_is_alive()) {
             countdown_timeout_pending = 1;
             return;
         }
         lv_label_set_text(guider_ui.screen_pomodoro_label_timer,"00:00:00");
         lv_obj_clear_state(guider_ui.screen_pomodoro_btn_complete, LV_STATE_DISABLED);
         lv_obj_clear_state(guider_ui.screen_pomodoro_btn_continue, LV_STATE_DISABLED);
         lv_obj_clear_flag(guider_ui.screen_pomodoro_img_timeout, LV_OBJ_FLAG_HIDDEN);
         lv_obj_add_state(guider_ui.screen_pomodoro_btn_minus,LV_STATE_DISABLED);
         lv_obj_add_state(guider_ui.screen_pomodoro_btn_plus,LV_STATE_DISABLED);
         lv_obj_add_state(guider_ui.screen_pomodoro_btn_addnum,LV_STATE_DISABLED);
         lv_obj_add_state(guider_ui.screen_pomodoro_btn_reset,LV_STATE_DISABLED);
         lv_obj_add_state(guider_ui.screen_pomodoro_btn_start,LV_STATE_DISABLED);
         lv_obj_add_state(guider_ui.screen_pomodoro_btn_back,LV_STATE_DISABLED);
         lv_obj_add_state(guider_ui.screen_pomodoro_btn_countdown,LV_STATE_DISABLED);
         lv_obj_add_state(guider_ui.screen_pomodoro_btn_stopwatch,LV_STATE_DISABLED);
         lv_obj_add_flag(guider_ui.screen_pomodoro_btn_start,LV_OBJ_FLAG_HIDDEN);
         lv_obj_add_flag(guider_ui.screen_pomodoro_btn_reset,LV_OBJ_FLAG_HIDDEN);
         /* 先完成按钮状态切换，再弹出超时层并聚焦 Complete。 */
         pomodoro_timeout_effect_play();
         countdown_timeout_pending = 1;
         if (pomodoro_countdown) {
             focus_complete_pending = 1;
         }
         countdown_start_label = 0;
         countdown_process_label = 0;
         pomodoro_pause_icon_refresh();
         return;
     }
     timer_all_seconds -= 1;
     if (!pomodoro_screen_is_alive()) {
         return;
     }
     int hours = timer_all_seconds / 3600;
     int minutes = (timer_all_seconds % 3600) / 60;
     int seconds = timer_all_seconds % 60;
     lv_label_set_text_fmt(guider_ui.screen_pomodoro_label_timer,"%02d:%02d:%02d",hours,minutes,seconds);
     pomodoro_black_update_leds();
 }
 
 void countdown_start(int16_t minutes){
     /* 仅删旧 timer，勿刷新 pause 图标：此时 countdown_* 标志尚未置位。 */
     pomodoro_timer_delete_only();
     timer_all_seconds = minutes * 60;
     s_pomodoro_black_total_seconds = timer_all_seconds;
     focus_session_minutes = minutes;
     pomodoro_timer = lv_timer_create(countdown_cb,1000,NULL);
 }
 //-----------------------pomodoro stopwatch------------------------//
 //-----------------------------------------------------------------//
 static void stopwatch_cb(lv_timer_t *t){
     (void)t;
     if (!pomodoro_screen_is_alive()) {
         pomodoro_timer_stop();
         return;
     }
     timer_all_seconds += 1;
     int hours = timer_all_seconds / 3600;
     int minutes = (timer_all_seconds % 3600) / 60;
     int seconds = timer_all_seconds % 60;
     lv_label_set_text_fmt(guider_ui.screen_pomodoro_label_timer,"%02d:%02d:%02d",hours,minutes,seconds);
 }
 
 void stopwatch_start(){
     pomodoro_timer_delete_only();
     timer_all_seconds = 0;
     pomodoro_timer = lv_timer_create(stopwatch_cb,1000,NULL);
 }
 
 //-----------------------wifi_status show----------------------//
 /**
  * @param password          NULL = 清空密码框；非 NULL = 填入（可为 "" 表示无密码热点）
  * @param focus_connect_btn true 焦点到 Connect；false 焦点到密码框
  */
 void wificfg_show_connect_ui(lv_obj_t *list_btn, const char *password, bool focus_connect_btn)
 {
     if (list_btn == NULL || guider_ui.screen_wificfg_list_1 == NULL) {
         return;
     }
 
     const char *txt = lv_list_get_btn_text(guider_ui.screen_wificfg_list_1, list_btn);
     if (txt == NULL || txt[0] == '\0') {
         return;
     }
 
     lv_obj_add_flag(guider_ui.screen_wificfg_img_wififalse, LV_OBJ_FLAG_HIDDEN);
     lv_obj_add_flag(guider_ui.screen_wificfg_img_wifiicon, LV_OBJ_FLAG_HIDDEN);
     lv_obj_add_flag(guider_ui.screen_wificfg_label_status2, LV_OBJ_FLAG_HIDDEN);
     lv_obj_clear_flag(guider_ui.screen_wificfg_btn_cancel, LV_OBJ_FLAG_HIDDEN);
     lv_obj_clear_flag(guider_ui.screen_wificfg_btn_connect, LV_OBJ_FLAG_HIDDEN);
     lv_obj_clear_flag(guider_ui.screen_wificfg_img_passicon, LV_OBJ_FLAG_HIDDEN);
     lv_obj_clear_flag(guider_ui.screen_wificfg_ta_password, LV_OBJ_FLAG_HIDDEN);
 
     lv_label_set_text_fmt(guider_ui.screen_wificfg_label_status,
                          ui_tr("Connect to %s", "连接 %s"), txt);
 
     if (wifi_ssid != NULL) {
         free(wifi_ssid);
     }
     wifi_ssid = strdup(txt);
 
     if (password != NULL) {
         lv_textarea_set_text(guider_ui.screen_wificfg_ta_password, password);
     } else {
         lv_textarea_set_text(guider_ui.screen_wificfg_ta_password, "");
     }
 
     if (focus_connect_btn) {
         lv_group_focus_obj(guider_ui.screen_wificfg_btn_connect);
     } else {
         lv_group_focus_obj(guider_ui.screen_wificfg_ta_password);
     }
 
     lv_refr_now(NULL);
     ui_animation(guider_ui.screen_wificfg_img_statusbg, 200, 0,
                  lv_obj_get_height(guider_ui.screen_wificfg_img_statusbg), 80,
                  &lv_anim_path_linear, 1, 0, 0, 0,
                  (lv_anim_exec_xcb_t)lv_obj_set_height, NULL, NULL, NULL);
 }
 
 /**
  * 默认实现：供 GUI Guider 模拟器链接；固件中由 main/wificfg_bridge.c 强符号覆盖（NVS 读密码）。
  */
 /* ESP32 固件由 main/volume_bridge.c 提供强符号实现 */
 __attribute__((weak)) void volume_enabled_impl(void)
 {
 }
 
 __attribute__((weak)) void volume_disabled_impl(void)
 {
 }
 
 void volume_on_enabled(void)
 {
     volume_enabled_impl();
 }
 
 void volume_on_disabled(void)
 {
     volume_disabled_impl();
 }
 
 __attribute__((weak)) void brightness_apply_impl(uint8_t level)
 {
     (void)level;
 }
 
 __attribute__((weak)) void brightness_persist_impl(uint8_t level)
 {
     (void)level;
 }
 
 void brightness_apply(void)
 {
     brightness_apply_impl(brightness_level);
 }
 
 void brightness_update_icon(void)
 {
     static const lv_opa_t icon_opa[BRIGHTNESS_LEVEL_COUNT] = {
         70, 115, 160, 210, 255
     };
     uint8_t level = brightness_level;
 
     if (level >= BRIGHTNESS_LEVEL_COUNT) {
         level = BRIGHTNESS_LEVEL_COUNT - 1U;
     }
     if (guider_ui.screen_setting_img_brightness == NULL ||
         !lv_obj_is_valid(guider_ui.screen_setting_img_brightness)) {
         return;
     }
     lv_obj_set_style_img_opa(guider_ui.screen_setting_img_brightness,
                              icon_opa[level],
                              LV_PART_MAIN | LV_STATE_DEFAULT);
 }
 
 void brightness_cycle(void)
 {
     brightness_level = (uint8_t)((brightness_level + 1U) % BRIGHTNESS_LEVEL_COUNT);
     brightness_apply();
     brightness_update_icon();
     brightness_persist_impl(brightness_level);
 }

 static const uint16_t s_sleep_level_minutes[SLEEP_LEVEL_COUNT] = {
     0, 1, 5, 10
 };

 __attribute__((weak)) void sleep_persist_impl(uint8_t level)
 {
     (void)level;
 }

 uint16_t sleep_timeout_get_minutes(void)
 {
     uint8_t level = sleep_timeout_level;
     if (level >= SLEEP_LEVEL_COUNT) {
         level = 0;
     }
     return s_sleep_level_minutes[level];
 }
 
 /* ESP32 固件由 main/focus_bridge.c 提供强符号实现 */
 __attribute__((weak)) void focus_session_cancel_impl(void)
 {
     focus_session_active = 0;
     focus_session_minutes = 0;
     focus_complete_pending = 0;
 }
 
 void focus_session_cancel(void)
 {
     focus_session_cancel_impl();
 }
 
 __attribute__((weak)) void wificfg_on_list_item_clicked(lv_obj_t *list_item)
 {
     wificfg_show_connect_ui(list_item, NULL, false);
 }
 
 static void wificfg_status2_set(const char *en, const char *zh)
 {
     lv_obj_t *label = guider_ui.screen_wificfg_label_status2;
     if (label == NULL || !lv_obj_is_valid(label)) {
         return;
     }
     /* Alatsi 没有汉字。中文时改用带备用字的 12 号思源，英文仍用原来的字。 */
     lv_obj_set_style_text_font(label,
                                ui_lang_is_zh() ? ui_font_12()
                                                : &lv_font_Alatsi_Regular_12,
                                LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_label_set_text(label, ui_tr(en, zh));
 }

 static void wificfg_disconnect_locale_cb(lv_event_t *e)
 {
     if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
         return;
     }
     /* generated 回调先写成英文 Disconnected，这里再换成当前语言。 */
     wificfg_status2_set("Disconnected", "未连接");
     if (guider_ui.screen_wificfg_label_status != NULL &&
         lv_obj_is_valid(guider_ui.screen_wificfg_label_status)) {
         lv_label_set_text(guider_ui.screen_wificfg_label_status,
                           ui_tr("Connection Status", "连接状态"));
     }
 }

 void wifi_connect_show(){
 
     lv_obj_add_flag(guider_ui.screen_wificfg_spinner_connect,LV_OBJ_FLAG_HIDDEN);
     // 1. Hide components (Add HIDDEN flag)
     lv_obj_add_flag(guider_ui.screen_wificfg_img_passicon, LV_OBJ_FLAG_HIDDEN);
     lv_obj_add_flag(guider_ui.screen_wificfg_ta_password, LV_OBJ_FLAG_HIDDEN);
     lv_obj_add_flag(guider_ui.screen_wificfg_btn_connect, LV_OBJ_FLAG_HIDDEN);
     lv_obj_add_flag(guider_ui.screen_wificfg_btn_cancel, LV_OBJ_FLAG_HIDDEN);
     lv_obj_add_flag(guider_ui.screen_wificfg_img_wififalse, LV_OBJ_FLAG_HIDDEN);
 
     // 2. Show components (Clear HIDDEN flag)
     lv_obj_clear_flag(guider_ui.screen_wificfg_img_wifiicon, LV_OBJ_FLAG_HIDDEN);
     lv_obj_clear_flag(guider_ui.screen_wificfg_label_status2, LV_OBJ_FLAG_HIDDEN);
 
     // 3. Update Label Texts
    lv_label_set_text(guider_ui.screen_wificfg_label_status,
                      ui_tr("Connection Status", "连接状态"));
    wificfg_status2_set("Connected", "已连接");
 
     // 4. Update Label Color (Status2)
     // Assuming the light green color in the image is roughly #b7f0b8
     lv_obj_set_style_text_color(guider_ui.screen_wificfg_label_status2, lv_color_hex(0x274a6e), 0);
 
     // 5. Change Image Height
     ui_animation(guider_ui.screen_wificfg_img_statusbg, 200, 0, lv_obj_get_height(guider_ui.screen_wificfg_img_statusbg), 125, &lv_anim_path_linear, 1, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_height, NULL, NULL, NULL); 
 }
 
 void wifi_disconnect_show(){
     lv_obj_add_flag(guider_ui.screen_wificfg_spinner_connect,LV_OBJ_FLAG_HIDDEN);
     // 1. Hide components (Add HIDDEN flag)
     lv_obj_add_flag(guider_ui.screen_wificfg_img_passicon, LV_OBJ_FLAG_HIDDEN);
     lv_obj_add_flag(guider_ui.screen_wificfg_ta_password, LV_OBJ_FLAG_HIDDEN);
     lv_obj_add_flag(guider_ui.screen_wificfg_btn_connect, LV_OBJ_FLAG_HIDDEN);
     lv_obj_add_flag(guider_ui.screen_wificfg_btn_cancel, LV_OBJ_FLAG_HIDDEN);
     lv_obj_add_flag(guider_ui.screen_wificfg_img_wififalse, LV_OBJ_FLAG_HIDDEN);
 
     // 2. Show components (Clear HIDDEN flag)
     lv_obj_clear_flag(guider_ui.screen_wificfg_img_wifiicon, LV_OBJ_FLAG_HIDDEN);
     lv_obj_clear_flag(guider_ui.screen_wificfg_label_status2, LV_OBJ_FLAG_HIDDEN);
     lv_obj_clear_flag(guider_ui.screen_wificfg_img_wififalse, LV_OBJ_FLAG_HIDDEN);
 
     // 3. Update Label Texts
    lv_label_set_text(guider_ui.screen_wificfg_label_status,
                      ui_tr("Connection Status", "连接状态"));
    wificfg_status2_set("Disconnected", "未连接");
 
     // 4. Update Label Color (Status2)
     // Assuming the light green color in the image is roughly #b7f0b8
     lv_obj_set_style_text_color(guider_ui.screen_wificfg_label_status2, lv_color_hex(0x274a6e), 0);
 
     // 5. Change Image Height
     ui_animation(guider_ui.screen_wificfg_img_statusbg, 200, 0, lv_obj_get_height(guider_ui.screen_wificfg_img_statusbg), 125, &lv_anim_path_linear, 1, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_height, NULL, NULL, NULL);
 }
 
 /* screen_ai 已停用；events_init 在 Guider 删屏前仍会调用这些入口。 */
void screen_ai_apply_result(const ui_ai_result_t *r)
{
    (void)r;
}

void screen_ai_load_nvs_data(bool day_or_week)
{
    (void)day_or_week;
    ai_day_start = 0;
    ai_week_start = 0;
}

void screen_ai_update_focus_level(void)
{
}

int donelist_load_from_csv(void)
 {
     FILE *f = fopen("/spiffs/task_done.csv", "r");
     memset(&ui_done_list, 0, sizeof(ui_done_list));
     if (!f) return 0;
     char line[200];
     while (fgets(line, sizeof(line), f) && ui_done_list.count < DONE_MAX_ITEMS) {
         size_t len = strlen(line);
         while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) line[--len] = '\0';
         if (len == 0) continue;
         /* 日期列可能为空（小程序未下发），不能用 strtok：它会吞掉空字段导致列错位 */
         char *fields[3] = {line, NULL, NULL};
         int nf = 1;
         for (char *p = line; *p != '\0' && nf < 3; p++) {
             if (*p == ',') {
                 *p = '\0';
                 fields[nf++] = p + 1;
             }
         }
         if (nf < 2 || fields[0][0] == '\0') continue;
         strncpy(ui_done_list.items[ui_done_list.count].name, fields[0],
                 sizeof(ui_done_list.items[0].name) - 1);
         strncpy(ui_done_list.items[ui_done_list.count].date, fields[1],
                 sizeof(ui_done_list.items[0].date) - 1);
         ui_done_list.items[ui_done_list.count].duration_min =
                 (uint16_t)(nf > 2 ? atoi(fields[2]) : 0);
         ui_done_list.count++;
     }
     fclose(f);
     /* CSV 按归档时间追加（旧→新），展示时倒序：首页第一条为最新完成。 */
     for (int i = 0, j = ui_done_list.count - 1; i < j; i++, j--) {
         done_item_t tmp = ui_done_list.items[i];
         ui_done_list.items[i] = ui_done_list.items[j];
         ui_done_list.items[j] = tmp;
     }
     return ui_done_list.count;
 }
 
 void donelist_render_page(int page_cur)
 {
     lv_obj_t *name_labels[4] = {
         guider_ui.screen_donelist_label_done1,
         guider_ui.screen_donelist_label_done2,
         guider_ui.screen_donelist_label_done3,
         guider_ui.screen_donelist_label_done4,
     };
     lv_obj_t *date_labels[4] = {
         guider_ui.screen_donelist_label_date1,
         guider_ui.screen_donelist_label_date2,
         guider_ui.screen_donelist_label_date3,
         guider_ui.screen_donelist_label_date4,
     };
     lv_obj_t *focus_labels[4] = {
         guider_ui.screen_donelist_label_focus1,
         guider_ui.screen_donelist_label_focus2,
         guider_ui.screen_donelist_label_focus3,
         guider_ui.screen_donelist_label_focus4,
     };
     for (int i = 0; i < 4; i++) {
         int idx = (page_cur - 1) * 4 + i;
         if (idx < ui_done_list.count) {
             lv_label_set_text(name_labels[i], ui_done_list.items[idx].name);
             lv_label_set_text(date_labels[i], ui_done_list.items[idx].date);
             if (ui_done_list.items[idx].duration_min > 0) {
                 lv_label_set_text_fmt(focus_labels[i], "%dmin",
                                       ui_done_list.items[idx].duration_min);
             } else {
                 lv_label_set_text(focus_labels[i], "-");
             }
         } else {
             lv_label_set_text(name_labels[i], "");
             lv_label_set_text(date_labels[i], "");
             lv_label_set_text(focus_labels[i], "");
         }
     }
 }
 
 //-----------------button added -------------------------------//
 //-------------------------------------------------------------//
 //buttion add function screen_todolist
 //buttion add function screen_todolist
 
 void clear_focus_box(lv_obj_t * obj) {
     // 1. 彻底关掉外轮廓 (这是最常见的蓝框来源)
     lv_obj_set_style_outline_width(obj, 0, LV_STATE_FOCUSED);
     lv_obj_set_style_outline_width(obj, 0, LV_STATE_FOCUS_KEY); // 针对键盘聚焦
     
     // 2. 彻底关掉边框 (Material 主题有时会加粗边框作为焦点反馈)
     lv_obj_set_style_border_width(obj, 0, LV_STATE_FOCUSED);
     lv_obj_set_style_border_width(obj, 0, LV_STATE_FOCUS_KEY);
     
     // 3. 彻底关掉阴影
     lv_obj_set_style_shadow_width(obj, 0, LV_STATE_FOCUSED);
 }
 
 void clear_edge_box(lv_obj_t * obj) {
     // 1. 彻底关掉外轮廓 (这是最常见的蓝框来源)
     lv_obj_set_style_outline_width(obj, 0, LV_STATE_FOCUSED);
     lv_obj_set_style_outline_width(obj, 0, LV_STATE_FOCUS_KEY); // 针对键盘聚焦
 }
 
 void group_focus_obj(lv_obj_t *obj)
 {
     if (g != NULL && obj != NULL && lv_obj_is_valid(obj)) {
         lv_group_focus_obj(obj);
     }
 }
 
 
 static lv_obj_t *s_todolist_effect_screen = NULL;
 static int8_t s_todolist_effect_page = -1;
 static bool s_todolist_effect_active = false;
 static uint8_t s_todolist_effect_pending = 0;
 static const lv_coord_t s_todolist_final_x[] = {19, 177, 19, 177};
 /* Rest / focus-peak Y for Guider todo buttons (row0/1 vs row2/3). */
 static const lv_coord_t s_todolist_rest_y[] = {51, 51, 104, 104};
 static const lv_coord_t s_todolist_focus_y[] = {40, 40, 93, 93};
 
 static lv_obj_t *todolist_todo_btn(uint8_t index)
 {
     lv_obj_t *buttons[] = {
         guider_ui.screen_todolist_btn_todo1,
         guider_ui.screen_todolist_btn_todo2,
         guider_ui.screen_todolist_btn_todo3,
         guider_ui.screen_todolist_btn_todo4,
     };
     return index < 4 ? buttons[index] : NULL;
 }
 
 static lv_obj_t *todolist_todo_label(uint8_t index)
 {
     lv_obj_t *labels[] = {
         guider_ui.screen_todolist_btn_todo1_label,
         guider_ui.screen_todolist_btn_todo2_label,
         guider_ui.screen_todolist_btn_todo3_label,
         guider_ui.screen_todolist_btn_todo4_label,
     };
     return index < 4 ? labels[index] : NULL;
 }
 
 static bool todolist_todo_should_show(uint8_t index)
 {
     lv_obj_t *btn = todolist_todo_btn(index);
     lv_obj_t *label = todolist_todo_label(index);
     if (btn == NULL || label == NULL ||
         !lv_obj_is_valid(btn) || !lv_obj_is_valid(label)) {
         return false;
     }
     const char *text = lv_label_get_text(label);
     bool has_content = text != NULL && text[0] != '\0';
     return has_content && !lv_obj_has_state(btn, LV_STATE_DISABLED);
 }
 
 static void todolist_effect_x_exec(void *obj, int32_t value)
 {
     if (obj != NULL && lv_obj_is_valid((lv_obj_t *)obj)) {
         lv_obj_set_x((lv_obj_t *)obj, (lv_coord_t)value);
     }
 }
 
 static void todolist_effect_opa_exec(void *obj, int32_t value)
 {
     if (obj != NULL && lv_obj_is_valid((lv_obj_t *)obj)) {
         lv_obj_set_style_opa((lv_obj_t *)obj, (lv_opa_t)value,
                              LV_PART_MAIN | LV_STATE_DEFAULT);
     }
 }
 
 static void todolist_focus_y_stop(lv_obj_t *btn, lv_coord_t rest_y);
 
 /* Snap every todo row to its resting X/opacity. Used when encoder focus
  * interrupts the enter/page transition so items cannot freeze mid-slide. */
 static void todolist_effect_finish(void)
 {
     if (!s_todolist_effect_active) {
         return;
     }
 
     s_todolist_effect_active = false;
     s_todolist_effect_pending = 0;
 
     for (uint8_t i = 0; i < 4; i++) {
         lv_obj_t *btn = todolist_todo_btn(i);
         if (btn == NULL || !lv_obj_is_valid(btn)) {
             continue;
         }
 
         lv_anim_del(btn, todolist_effect_x_exec);
         lv_anim_del(btn, todolist_effect_opa_exec);
         lv_obj_set_x(btn, s_todolist_final_x[i]);
         lv_obj_set_style_opa(btn,
                              todolist_todo_should_show(i) ? LV_OPA_COVER
                                                          : LV_OPA_TRANSP,
                              LV_PART_MAIN | LV_STATE_DEFAULT);
         /* Drop any in-flight Guider Y bounce left by focus changes mid-enter. */
         if (!lv_obj_has_state(btn, LV_STATE_FOCUSED)) {
             todolist_focus_y_stop(btn, s_todolist_rest_y[i]);
         }
     }
 }
 
 static void todolist_effect_ready(lv_anim_t *anim)
 {
     LV_UNUSED(anim);
     if (s_todolist_effect_pending > 0) {
         s_todolist_effect_pending--;
     }
     if (s_todolist_effect_pending == 0) {
         s_todolist_effect_active = false;
     }
 }
 
 static int8_t todolist_todo_btn_index(lv_obj_t *obj)
 {
     for (uint8_t i = 0; i < 4; i++) {
         if (todolist_todo_btn(i) == obj) {
             return (int8_t)i;
         }
     }
     return -1;
 }
 
 static void todolist_focus_y_exec(void *obj, int32_t value)
 {
     if (obj != NULL && lv_obj_is_valid((lv_obj_t *)obj)) {
         lv_obj_set_y((lv_obj_t *)obj, (lv_coord_t)value);
     }
 }
 
 static void todolist_focus_y_stop(lv_obj_t *btn, lv_coord_t rest_y)
 {
     if (btn == NULL || !lv_obj_is_valid(btn)) {
         return;
     }
     /* Guider handlers use lv_obj_set_y as exec_cb; clear both. */
     lv_anim_del(btn, (lv_anim_exec_xcb_t)lv_obj_set_y);
     lv_anim_del(btn, todolist_focus_y_exec);
     lv_obj_set_y(btn, rest_y);
 }
 
 static void todolist_focus_y_play(lv_obj_t *btn, lv_coord_t rest_y,
                                  lv_coord_t focus_y)
 {
     if (btn == NULL || !lv_obj_is_valid(btn)) {
         return;
     }
 
     todolist_focus_y_stop(btn, rest_y);
 
     lv_anim_t bounce;
     lv_anim_init(&bounce);
     lv_anim_set_var(&bounce, btn);
     lv_anim_set_exec_cb(&bounce, todolist_focus_y_exec);
     lv_anim_set_values(&bounce, rest_y, focus_y);
     lv_anim_set_time(&bounce, 0);
     lv_anim_set_playback_time(&bounce, 200);
     lv_anim_set_repeat_count(&bounce, 1);
     lv_anim_set_path_cb(&bounce, lv_anim_path_linear);
     lv_anim_start(&bounce);
 }
 
 /*
  * Runs after Guider's generated focus handlers. Snaps enter/page motion if
  * still in progress, then corrects Y bounce so interrupted focus cannot leave
  * rows parked at a mid-animation offset.
  */
 static void todolist_effect_focus_interrupt_cb(lv_event_t *e)
 {
     lv_event_code_t code = lv_event_get_code(e);
     if (code != LV_EVENT_FOCUSED && code != LV_EVENT_DEFOCUSED) {
         return;
     }
 
     todolist_effect_finish();
 
     lv_obj_t *target = lv_event_get_target(e);
     int8_t index = todolist_todo_btn_index(target);
     if (index < 0) {
         return;
     }
 
     if (code == LV_EVENT_FOCUSED) {
         todolist_focus_y_play(target, s_todolist_rest_y[index],
                               s_todolist_focus_y[index]);
     } else {
         todolist_focus_y_stop(target, s_todolist_rest_y[index]);
     }
 }
 
 /*
  * direction: 0 = 页面进入（两侧汇入），1 = 下一页（右侧进入），
  *           -1 = 上一页（左侧进入）。
  * 只给当前确实有内容且可用的 todo 按钮播放，空项始终保持隐藏。
  * 动画只修改 X/透明度，不会干扰原有 focus 弹跳使用的 Y 坐标。
  */
 static void todolist_effect_play(int8_t direction)
 {
     todolist_effect_finish();
     s_todolist_effect_pending = 0;
 
     for (uint8_t i = 0; i < 4; i++) {
         lv_obj_t *btn = todolist_todo_btn(i);
         if (btn == NULL || !lv_obj_is_valid(btn)) {
             continue;
         }
 
         lv_anim_del(btn, todolist_effect_x_exec);
         lv_anim_del(btn, todolist_effect_opa_exec);
         lv_obj_set_x(btn, s_todolist_final_x[i]);
 
         if (!todolist_todo_should_show(i)) {
             lv_obj_set_style_opa(btn, LV_OPA_TRANSP,
                                  LV_PART_MAIN | LV_STATE_DEFAULT);
             continue;
         }
 
         lv_coord_t start_x;
         if (direction > 0) {
             start_x = s_todolist_final_x[i] + 34;
         } else if (direction < 0) {
             start_x = s_todolist_final_x[i] - 34;
         } else {
             start_x = s_todolist_final_x[i] + ((i & 1U) ? 30 : -30);
         }
 
         lv_obj_set_x(btn, start_x);
         lv_obj_set_style_opa(btn, LV_OPA_TRANSP,
                              LV_PART_MAIN | LV_STATE_DEFAULT);
 
         lv_anim_t move;
         lv_anim_init(&move);
         lv_anim_set_var(&move, btn);
         lv_anim_set_exec_cb(&move, todolist_effect_x_exec);
         lv_anim_set_values(&move, start_x, s_todolist_final_x[i]);
         lv_anim_set_time(&move, 240);
         lv_anim_set_delay(&move, i * 55);
         lv_anim_set_path_cb(&move, lv_anim_path_ease_out);
         lv_anim_set_ready_cb(&move, todolist_effect_ready);
         lv_anim_start(&move);
         s_todolist_effect_pending++;
 
         lv_anim_t fade;
         lv_anim_init(&fade);
         lv_anim_set_var(&fade, btn);
         lv_anim_set_exec_cb(&fade, todolist_effect_opa_exec);
         lv_anim_set_values(&fade, LV_OPA_TRANSP, LV_OPA_COVER);
         lv_anim_set_time(&fade, 150);
         lv_anim_set_delay(&fade, i * 55);
         lv_anim_set_path_cb(&fade, lv_anim_path_ease_out);
         lv_anim_set_ready_cb(&fade, todolist_effect_ready);
         lv_anim_start(&fade);
         s_todolist_effect_pending++;
     }
 
     s_todolist_effect_active = (s_todolist_effect_pending > 0);
 }
 
 static void todolist_effect_play_async(void *direction_ptr)
 {
     if (guider_ui.screen_todolist == NULL ||
         !lv_obj_is_valid(guider_ui.screen_todolist) ||
         lv_scr_act() != guider_ui.screen_todolist) {
         return;
     }
     todolist_effect_play((int8_t)(intptr_t)direction_ptr);
     s_todolist_effect_page = page_cur;
 }
 
 static void todolist_effect_screen_cb(lv_event_t *e)
 {
     lv_event_code_t code = lv_event_get_code(e);
     if (code == LV_EVENT_SCREEN_LOADED) {
         lv_async_call(todolist_effect_play_async, NULL);
     } else if (code == LV_EVENT_SCREEN_UNLOADED) {
         todolist_effect_finish();
         s_todolist_effect_screen = NULL;
         s_todolist_effect_page = -1;
     }
 }
 
 static void todolist_effect_page_cb(lv_event_t *e)
 {
     if (lv_event_get_code(e) != LV_EVENT_CLICKED ||
         page_cur == s_todolist_effect_page) {
         return;
     }
 
     lv_obj_t *target = lv_event_get_target(e);
     int8_t direction =
         target == guider_ui.screen_todolist_btn_nxtpage ? 1 : -1;
     lv_async_call(todolist_effect_play_async, (void *)(intptr_t)direction);
 }
 
 static void todolist_effect_prepare(void)
 {
     for (uint8_t i = 0; i < 4; i++) {
         lv_obj_t *btn = todolist_todo_btn(i);
         if (btn != NULL && lv_obj_is_valid(btn)) {
             lv_obj_set_style_opa(btn, LV_OPA_TRANSP,
                                  LV_PART_MAIN | LV_STATE_DEFAULT);
         }
     }
 }
 
 void screen_todolist_ensure_group_focus(void)
 {
     if (g == NULL) {
         return;
     }

     lv_obj_t *foc = lv_group_get_focused(g);
     if (foc != NULL && lv_obj_is_valid(foc) &&
         !lv_obj_has_state(foc, LV_STATE_DISABLED)) {
         return;
     }

     lv_obj_t *candidates[] = {
         guider_ui.screen_todolist_btn_todo1,
         guider_ui.screen_todolist_btn_todo2,
         guider_ui.screen_todolist_btn_todo3,
         guider_ui.screen_todolist_btn_todo4,
         guider_ui.screen_todolist_btn_lastpage,
         guider_ui.screen_todolist_btn_nxtpage,
         guider_ui.screen_todolist_btn_back,
     };

     for (size_t i = 0; i < sizeof(candidates) / sizeof(candidates[0]); i++) {
         lv_obj_t *obj = candidates[i];
         if (obj != NULL && lv_obj_is_valid(obj) &&
             !lv_obj_has_state(obj, LV_STATE_DISABLED)) {
             lv_group_focus_obj(obj);
             return;
         }
     }
 }

 void screen_todolist_buttion_add(){
     if(g == NULL) {
         g = lv_group_create();
     } else {
         lv_group_remove_all_objs(g); 
     }
     //setting default group
     lv_group_set_default(g);
     //indev with group
     lv_indev_t * indev = NULL;
     while ((indev = lv_indev_get_next(indev))) {
         if (lv_indev_get_type(indev) == LV_INDEV_TYPE_KEYPAD) {
             lv_indev_set_group(indev, g);
         }
     }

     /* Empty todolist leaves todo buttons DISABLED. Rebuilding the group while
      * they stay disabled makes lv_group_refocus() skip every candidate and
      * leave obj_focus=NULL, freezing keypad/encoder input after BLE 0→N sync. */
     lv_obj_clear_state(guider_ui.screen_todolist_btn_todo1, LV_STATE_DISABLED);
     lv_obj_clear_state(guider_ui.screen_todolist_btn_todo2, LV_STATE_DISABLED);
     lv_obj_clear_state(guider_ui.screen_todolist_btn_todo3, LV_STATE_DISABLED);
     lv_obj_clear_state(guider_ui.screen_todolist_btn_todo4, LV_STATE_DISABLED);
     
     lv_group_add_obj(g, guider_ui.screen_todolist_btn_todo1);
     lv_group_add_obj(g, guider_ui.screen_todolist_btn_todo2);
     lv_group_add_obj(g, guider_ui.screen_todolist_btn_todo3);
     lv_group_add_obj(g, guider_ui.screen_todolist_btn_todo4);
     lv_group_add_obj(g, guider_ui.screen_todolist_btn_lastpage);
     lv_group_add_obj(g, guider_ui.screen_todolist_btn_nxtpage);
     lv_group_add_obj(g, guider_ui.screen_todolist_btn_back);
 
     clear_focus_box(guider_ui.screen_todolist_btn_todo1);
     clear_focus_box(guider_ui.screen_todolist_btn_todo2);
     clear_focus_box(guider_ui.screen_todolist_btn_todo3);
     clear_focus_box(guider_ui.screen_todolist_btn_todo4);
     clear_edge_box(guider_ui.screen_todolist_btn_lastpage);
     clear_edge_box(guider_ui.screen_todolist_btn_nxtpage);
     clear_edge_box(guider_ui.screen_todolist_btn_back);
     ui_font_bind(guider_ui.screen_todolist);
 
     if (s_todolist_effect_screen != guider_ui.screen_todolist) {
         s_todolist_effect_screen = guider_ui.screen_todolist;
         s_todolist_effect_page = -1;
         todolist_effect_prepare();
         lv_obj_add_event_cb(guider_ui.screen_todolist,
                             todolist_effect_screen_cb, LV_EVENT_ALL, NULL);
         lv_obj_add_event_cb(guider_ui.screen_todolist_btn_lastpage,
                             todolist_effect_page_cb, LV_EVENT_CLICKED, NULL);
         lv_obj_add_event_cb(guider_ui.screen_todolist_btn_nxtpage,
                             todolist_effect_page_cb, LV_EVENT_CLICKED, NULL);
 
         /* Encoder focus during enter/page motion must snap layout first. */
         lv_obj_t *focus_targets[] = {
             guider_ui.screen_todolist_btn_todo1,
             guider_ui.screen_todolist_btn_todo2,
             guider_ui.screen_todolist_btn_todo3,
             guider_ui.screen_todolist_btn_todo4,
             guider_ui.screen_todolist_btn_lastpage,
             guider_ui.screen_todolist_btn_nxtpage,
             guider_ui.screen_todolist_btn_back,
         };
         for (size_t i = 0; i < sizeof(focus_targets) / sizeof(focus_targets[0]); i++) {
             if (focus_targets[i] != NULL) {
                 lv_obj_add_event_cb(focus_targets[i],
                                     todolist_effect_focus_interrupt_cb,
                                     LV_EVENT_FOCUSED, NULL);
                 lv_obj_add_event_cb(focus_targets[i],
                                     todolist_effect_focus_interrupt_cb,
                                     LV_EVENT_DEFOCUSED, NULL);
             }
         }
     }
 
 }
 //buttion add function screen_main
 static void screen_main_status_refresh_async(void *unused);
 
 /* 与 setup_scr_screen_main 中 cont_main 子控件创建顺序一致（flex 横滑顺序） */
 static const lv_coord_t s_main_nav_icon_x[] = {
     75, 93, 113, 130, 151, 169, 185, 205, 223, 240
 };
 static const lv_coord_t s_main_nav_point_x[] = {
     80, 97, 117, 137, 156, 174, 191, 209, 228, 244
 };
 static int8_t s_main_nav_prev_index = -1;
 
 static lv_obj_t *screen_main_nav_icon_at(uint8_t index)
 {
     lv_obj_t *icons[] = {
         guider_ui.screen_main_img_todo_icon,
         guider_ui.screen_main_img_done_icon,
         guider_ui.screen_main_img_timer_icon,
         guider_ui.screen_main_img_ai_icon,
         guider_ui.screen_main_img_gash_icon,
         guider_ui.screen_main_img_book_icon,
         guider_ui.screen_main_img_fun_icon,
         guider_ui.screen_main_img_photo_icon,
         guider_ui.screen_main_img_game_icon,
         guider_ui.screen_main_img_setting_icon,
     };
     return index < sizeof(icons) / sizeof(icons[0]) ? icons[index] : NULL;
 }
 
 static int8_t screen_main_nav_index_from_btn(lv_obj_t *btn)
 {
     lv_obj_t *buttons[] = {
         guider_ui.screen_main_btn_todolist,
         guider_ui.screen_main_btn_done,
         guider_ui.screen_main_btn_timer,
         guider_ui.screen_main_btn_ai,
         guider_ui.screen_main_btn_gashapon,
         guider_ui.screen_main_btn_word,
         guider_ui.screen_main_btn_fun,
         guider_ui.screen_main_btn_photo,
         guider_ui.screen_main_btn_game,
         guider_ui.screen_main_btn_setting,
     };
     for (uint8_t i = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++) {
         if (btn == buttons[i]) {
             return (int8_t)i;
         }
     }
     return -1;
 }
 
 static void screen_main_nav_x_exec(void *obj, int32_t value)
 {
     if (obj != NULL && lv_obj_is_valid((lv_obj_t *)obj)) {
         lv_obj_set_x((lv_obj_t *)obj, (lv_coord_t)value);
     }
 }
 
 static void screen_main_nav_opa_exec(void *obj, int32_t value)
 {
     if (obj != NULL && lv_obj_is_valid((lv_obj_t *)obj)) {
         lv_obj_set_style_img_opa((lv_obj_t *)obj, (lv_opa_t)value,
                                  LV_PART_MAIN | LV_STATE_DEFAULT);
     }
 }
 
 static void screen_main_nav_reset_icons(void)
 {
     for (uint8_t i = 0; i < sizeof(s_main_nav_icon_x) / sizeof(s_main_nav_icon_x[0]); i++) {
         lv_obj_t *icon = screen_main_nav_icon_at(i);
         if (icon == NULL || !lv_obj_is_valid(icon)) {
             continue;
         }
         lv_anim_del(icon, screen_main_nav_x_exec);
         lv_anim_del(icon, screen_main_nav_opa_exec);
         lv_obj_set_x(icon, s_main_nav_icon_x[i]);
         lv_obj_set_style_img_opa(icon, LV_OPA_COVER,
                                  LV_PART_MAIN | LV_STATE_DEFAULT);
     }
 }
 
 static void screen_main_nav_slide_to(uint8_t index)
 {
     if (index >= sizeof(s_main_nav_icon_x) / sizeof(s_main_nav_icon_x[0])) {
         return;
     }
 
     lv_obj_t *icon = screen_main_nav_icon_at(index);
     lv_obj_t *point = guider_ui.screen_main_img_point_icon;
     if (icon == NULL || !lv_obj_is_valid(icon) ||
         point == NULL || !lv_obj_is_valid(point)) {
         return;
     }
 
     lv_coord_t icon_from = s_main_nav_icon_x[index];
     lv_coord_t point_from = s_main_nav_point_x[index];
     if (s_main_nav_prev_index >= 0 &&
         s_main_nav_prev_index < (int8_t)(sizeof(s_main_nav_icon_x) /
                                          sizeof(s_main_nav_icon_x[0]))) {
         icon_from = s_main_nav_icon_x[s_main_nav_prev_index];
         point_from = s_main_nav_point_x[s_main_nav_prev_index];
     }
 
     screen_main_nav_reset_icons();
     lv_anim_del(point, screen_main_nav_x_exec);
 
     lv_obj_set_x(icon, icon_from);
     lv_obj_set_style_img_opa(icon, 80, LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_x(point, point_from);
 
     lv_anim_t slide;
     lv_anim_init(&slide);
     lv_anim_set_var(&slide, icon);
     lv_anim_set_exec_cb(&slide, screen_main_nav_x_exec);
     lv_anim_set_values(&slide, icon_from, s_main_nav_icon_x[index]);
     lv_anim_set_time(&slide, 220);
     lv_anim_set_path_cb(&slide, lv_anim_path_ease_out);
     lv_anim_start(&slide);
 
     lv_anim_t fade;
     lv_anim_init(&fade);
     lv_anim_set_var(&fade, icon);
     lv_anim_set_exec_cb(&fade, screen_main_nav_opa_exec);
     lv_anim_set_values(&fade, 80, LV_OPA_COVER);
     lv_anim_set_time(&fade, 170);
     lv_anim_set_path_cb(&fade, lv_anim_path_ease_out);
     lv_anim_start(&fade);
 
     lv_anim_t point_slide;
     lv_anim_init(&point_slide);
     lv_anim_set_var(&point_slide, point);
     lv_anim_set_exec_cb(&point_slide, screen_main_nav_x_exec);
     lv_anim_set_values(&point_slide, point_from, s_main_nav_point_x[index]);
     lv_anim_set_time(&point_slide, 240);
     lv_anim_set_path_cb(&point_slide, lv_anim_path_ease_in_out);
     lv_anim_start(&point_slide);
 
     s_main_nav_prev_index = (int8_t)index;
 }
 
 static void screen_main_nav_focus_effect_cb(lv_event_t *e)
 {
     if (lv_event_get_code(e) != LV_EVENT_FOCUSED) {
         return;
     }
 
     lv_obj_t *target = lv_event_get_target(e);
     int8_t index = screen_main_nav_index_from_btn(target);
     if (index >= 0) {
         screen_main_nav_slide_to((uint8_t)index);
     } else if (target == guider_ui.screen_main_btn_realtime) {
         /* Real Time 使用独立的长条选中图，不参与小图标滑动。 */
         screen_main_nav_reset_icons();
         s_main_nav_prev_index = -1;
     }
 }
 
 static void screen_main_nav_effect_init_async(void *unused)
 {
     LV_UNUSED(unused);
     if (guider_ui.screen_main == NULL ||
         !lv_obj_is_valid(guider_ui.screen_main) ||
         lv_scr_act() != guider_ui.screen_main) {
         return;
     }
 
     screen_main_nav_reset_icons();
     int8_t index = g != NULL
         ? screen_main_nav_index_from_btn(lv_group_get_focused(g))
         : -1;
     s_main_nav_prev_index = index >= 0 ? index : 0;
 }
 
 static void screen_main_nav_effect_bind_hooks(void)
 {
     lv_obj_t *buttons[] = {
         guider_ui.screen_main_btn_todolist,
         guider_ui.screen_main_btn_done,
         guider_ui.screen_main_btn_timer,
         guider_ui.screen_main_btn_ai,
         guider_ui.screen_main_btn_gashapon,
         guider_ui.screen_main_btn_word,
         guider_ui.screen_main_btn_fun,
         guider_ui.screen_main_btn_photo,
         guider_ui.screen_main_btn_game,
         guider_ui.screen_main_btn_setting,
         guider_ui.screen_main_btn_realtime,
     };
     for (size_t i = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++) {
         if (buttons[i] != NULL && lv_obj_is_valid(buttons[i])) {
             lv_obj_add_event_cb(buttons[i], screen_main_nav_focus_effect_cb,
                                 LV_EVENT_FOCUSED, NULL);
         }
     }
     s_main_nav_prev_index = -1;
     lv_async_call(screen_main_nav_effect_init_async, NULL);
 }
 
 static lv_obj_t *s_main_realtime_card;
 static lv_obj_t *s_main_realtime_card_time;
 static lv_obj_t *s_main_realtime_card_date;
 static char s_main_realtime_card_text[8] = "--:--";
 static char s_main_realtime_card_date_text[16] = "---- -- --";
 
 static void screen_main_realtime_opa_exec(void *obj, int32_t value)
 {
     if (obj != NULL && lv_obj_is_valid((lv_obj_t *)obj)) {
         lv_obj_set_style_opa((lv_obj_t *)obj, (lv_opa_t)value,
                              LV_PART_MAIN | LV_STATE_DEFAULT);
     }
 }
 
 static void screen_main_realtime_y_exec(void *obj, int32_t value)
 {
     if (obj != NULL && lv_obj_is_valid((lv_obj_t *)obj)) {
         lv_obj_set_y((lv_obj_t *)obj, (lv_coord_t)value);
     }
 }
 
 static void screen_main_realtime_width_exec(void *obj, int32_t value)
 {
     if (obj != NULL && lv_obj_is_valid((lv_obj_t *)obj)) {
         lv_obj_set_width((lv_obj_t *)obj, (lv_coord_t)value);
     }
 }
 
 static void screen_main_realtime_height_exec(void *obj, int32_t value)
 {
     if (obj != NULL && lv_obj_is_valid((lv_obj_t *)obj)) {
         lv_obj_set_height((lv_obj_t *)obj, (lv_coord_t)value);
     }
 }
 
 static void screen_main_realtime_text_opa_exec(void *obj, int32_t value)
 {
     if (obj != NULL && lv_obj_is_valid((lv_obj_t *)obj)) {
         lv_obj_set_style_text_opa((lv_obj_t *)obj, (lv_opa_t)value,
                                   LV_PART_MAIN | LV_STATE_DEFAULT);
     }
 }
 
 static void screen_main_realtime_hide_ready(lv_anim_t *anim)
 {
     lv_obj_t *obj = (lv_obj_t *)anim->var;
     if (obj != NULL && lv_obj_is_valid(obj)) {
         lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
     }
 }
 
 void screen_main_realtime_card_sync(const char *time_text,
                                     const char *date_text)
 {
     if (time_text != NULL) {
         snprintf(s_main_realtime_card_text, sizeof(s_main_realtime_card_text),
                  "%.5s", time_text);
         if (s_main_realtime_card_time != NULL &&
             lv_obj_is_valid(s_main_realtime_card_time)) {
             lv_label_set_text_static(s_main_realtime_card_time,
                                      s_main_realtime_card_text);
         }
     }
 
     if (date_text != NULL) {
         snprintf(s_main_realtime_card_date_text,
                  sizeof(s_main_realtime_card_date_text),
                  "%.10s", date_text);
         if (s_main_realtime_card_date != NULL &&
             lv_obj_is_valid(s_main_realtime_card_date)) {
             lv_label_set_text_static(s_main_realtime_card_date,
                                      s_main_realtime_card_date_text);
         }
     }
 }
 
 static void screen_main_realtime_card_create(void)
 {
     if (guider_ui.screen_main == NULL ||
         !lv_obj_is_valid(guider_ui.screen_main)) {
         return;
     }
 
     s_main_realtime_card = lv_obj_create(guider_ui.screen_main);
     lv_obj_set_pos(s_main_realtime_card, 57, 43);
     lv_obj_set_size(s_main_realtime_card, 206, 86);
     lv_obj_clear_flag(s_main_realtime_card, LV_OBJ_FLAG_SCROLLABLE);
     lv_obj_clear_flag(s_main_realtime_card, LV_OBJ_FLAG_CLICKABLE);
     lv_obj_set_style_radius(s_main_realtime_card, 15,
                             LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_bg_color(s_main_realtime_card, lv_color_hex(0xeaf3fb),
                               LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_bg_opa(s_main_realtime_card, 200,
                             LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_border_width(s_main_realtime_card, 1,
                                   LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_border_color(s_main_realtime_card, lv_color_hex(0xffffff),
                                   LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_border_opa(s_main_realtime_card, 210,
                                 LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_shadow_width(s_main_realtime_card, 22,
                                   LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_shadow_color(s_main_realtime_card, lv_color_hex(0x6689aa),
                                   LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_shadow_opa(s_main_realtime_card, 65,
                                 LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_shadow_spread(s_main_realtime_card, 2,
                                    LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_pad_all(s_main_realtime_card, 0,
                              LV_PART_MAIN | LV_STATE_DEFAULT);
 
     s_main_realtime_card_time = lv_label_create(guider_ui.screen_main);
     lv_label_set_text_static(s_main_realtime_card_time,
                              s_main_realtime_card_text);
     lv_obj_set_pos(s_main_realtime_card_time, 90, 61);
     lv_obj_set_size(s_main_realtime_card_time, 140, 42);
     lv_obj_set_style_text_align(s_main_realtime_card_time, LV_TEXT_ALIGN_CENTER,
                                 LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_text_font(s_main_realtime_card_time,
                                &CUSTOM_DISPLAY_FONT,
                                LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_text_color(s_main_realtime_card_time, lv_color_hex(0x173c5c),
                                 LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_bg_opa(s_main_realtime_card_time, LV_OPA_TRANSP,
                             LV_PART_MAIN | LV_STATE_DEFAULT);
 
     s_main_realtime_card_date = lv_label_create(guider_ui.screen_main);
     lv_label_set_text_static(s_main_realtime_card_date,
                              s_main_realtime_card_date_text);
     lv_obj_set_pos(s_main_realtime_card_date, 90, 102);
     lv_obj_set_size(s_main_realtime_card_date, 140, 20);
     lv_obj_set_style_text_align(s_main_realtime_card_date, LV_TEXT_ALIGN_CENTER,
                                 LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_text_font(s_main_realtime_card_date,
                                &lv_font_SourceHanSansSC_Medium_14,
                                LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_text_color(s_main_realtime_card_date, lv_color_hex(0x55738d),
                                 LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_text_letter_space(s_main_realtime_card_date, 1,
                                        LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_bg_opa(s_main_realtime_card_date, LV_OPA_TRANSP,
                             LV_PART_MAIN | LV_STATE_DEFAULT);
 
     lv_obj_add_flag(s_main_realtime_card, LV_OBJ_FLAG_HIDDEN);
     lv_obj_add_flag(s_main_realtime_card_time, LV_OBJ_FLAG_HIDDEN);
     lv_obj_add_flag(s_main_realtime_card_date, LV_OBJ_FLAG_HIDDEN);
 }
 
 static void screen_main_realtime_time_progress_exec(void *obj, int32_t progress)
 {
     lv_obj_t *label = (lv_obj_t *)obj;
     if (label == NULL || !lv_obj_is_valid(label)) {
         return;
     }
 
     if (progress < 0) {
         progress = 0;
     } else if (progress > 1000) {
         progress = 1000;
     }
 
     /* smoothstep：运动开始与结束都柔和，中段速度明显。 */
     int32_t eased = (int32_t)(((int64_t)progress * progress *
                               (3000 - 2 * progress)) / 1000000);
     /* 从右下角原始时钟附近飞入卡片中央。 */
     int32_t x = 220 + (90 - 220) * eased / 1000;
     int32_t y = 128 + (61 - 128) * eased / 1000;
 
     /* 只用坐标制造落地回弹，避免硬件端文字变换图层不显示。 */
     if (progress >= 700 && progress < 850) {
         y += (progress - 700) * 10 / 150;
     } else if (progress >= 850) {
         y += (1000 - progress) * 10 / 150;
     }
 
     lv_obj_clear_flag(label, LV_OBJ_FLAG_HIDDEN);
     lv_obj_set_pos(label, (lv_coord_t)x, (lv_coord_t)y);
     lv_obj_set_style_transform_zoom(label, 256,
                                     LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_transform_angle(label, 0,
                                      LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_text_letter_space(label,
                                        (lv_coord_t)(9 - 8 * eased / 1000),
                                        LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_text_opa(label, LV_OPA_COVER,
                               LV_PART_MAIN | LV_STATE_DEFAULT);
 }
 
 static void screen_main_realtime_time_start(void)
 {
     if (s_main_realtime_card_time == NULL ||
         !lv_obj_is_valid(s_main_realtime_card_time) ||
         g == NULL ||
         lv_group_get_focused(g) != guider_ui.screen_main_btn_realtime) {
         return;
     }
 
     lv_anim_del(s_main_realtime_card_time, NULL);
     lv_obj_clear_flag(s_main_realtime_card_time, LV_OBJ_FLAG_HIDDEN);
     lv_obj_move_foreground(s_main_realtime_card_time);
     screen_main_realtime_time_progress_exec(s_main_realtime_card_time, 0);
 
     lv_anim_t time_fly;
     lv_anim_init(&time_fly);
     lv_anim_set_var(&time_fly, s_main_realtime_card_time);
     lv_anim_set_exec_cb(&time_fly, screen_main_realtime_time_progress_exec);
     lv_anim_set_values(&time_fly, 0, 1000);
     lv_anim_set_time(&time_fly, 360);
     lv_anim_set_path_cb(&time_fly, lv_anim_path_linear);
     lv_anim_start(&time_fly);
 
     if (s_main_realtime_card_date == NULL ||
         !lv_obj_is_valid(s_main_realtime_card_date)) {
         return;
     }
 
     lv_anim_del(s_main_realtime_card_date, NULL);
     lv_obj_clear_flag(s_main_realtime_card_date, LV_OBJ_FLAG_HIDDEN);
     lv_obj_move_foreground(s_main_realtime_card_date);
     lv_obj_set_y(s_main_realtime_card_date, 116);
     lv_obj_set_style_text_opa(s_main_realtime_card_date, LV_OPA_TRANSP,
                               LV_PART_MAIN | LV_STATE_DEFAULT);
 
     /* 日期在时间飞入后从下方轻盈跟随，形成清晰的信息层级。 */
     lv_anim_t date_move;
     lv_anim_init(&date_move);
     lv_anim_set_var(&date_move, s_main_realtime_card_date);
     lv_anim_set_exec_cb(&date_move, screen_main_realtime_y_exec);
     lv_anim_set_values(&date_move, 116, 102);
     lv_anim_set_delay(&date_move, 170);
     lv_anim_set_time(&date_move, 220);
     lv_anim_set_path_cb(&date_move, lv_anim_path_overshoot);
     lv_anim_start(&date_move);
 
     lv_anim_t date_fade;
     lv_anim_init(&date_fade);
     lv_anim_set_var(&date_fade, s_main_realtime_card_date);
     lv_anim_set_exec_cb(&date_fade, screen_main_realtime_text_opa_exec);
     lv_anim_set_values(&date_fade, LV_OPA_TRANSP, LV_OPA_COVER);
     lv_anim_set_delay(&date_fade, 170);
     lv_anim_set_time(&date_fade, 130);
     lv_anim_set_path_cb(&date_fade, lv_anim_path_ease_out);
     lv_anim_start(&date_fade);
 }
 
 static void screen_main_realtime_card_open_ready(lv_anim_t *anim)
 {
     LV_UNUSED(anim);
     screen_main_realtime_time_start();
 }
 
 static void screen_main_realtime_card_show(void)
 {
     if (s_main_realtime_card == NULL ||
         !lv_obj_is_valid(s_main_realtime_card) ||
         s_main_realtime_card_time == NULL ||
         !lv_obj_is_valid(s_main_realtime_card_time) ||
         s_main_realtime_card_date == NULL ||
         !lv_obj_is_valid(s_main_realtime_card_date)) {
         return;
     }
 
     if (guider_ui.screen_main_btn_realtime_label != NULL &&
         lv_obj_is_valid(guider_ui.screen_main_btn_realtime_label)) {
         screen_main_realtime_card_sync(
             lv_label_get_text(guider_ui.screen_main_btn_realtime_label),
             NULL);
     }
 
     lv_anim_del(s_main_realtime_card, NULL);
     lv_anim_del(s_main_realtime_card_time, NULL);
     lv_anim_del(s_main_realtime_card_date, NULL);
     lv_obj_clear_flag(s_main_realtime_card, LV_OBJ_FLAG_HIDDEN);
     lv_obj_add_flag(s_main_realtime_card_time, LV_OBJ_FLAG_HIDDEN);
     lv_obj_add_flag(s_main_realtime_card_date, LV_OBJ_FLAG_HIDDEN);
     lv_obj_move_foreground(s_main_realtime_card);
 
     /* 卡片从左上角的一个小点扩展到屏幕中央。 */
     lv_obj_set_pos(s_main_realtime_card, 18, 18);
     lv_obj_set_size(s_main_realtime_card, 4, 4);
     lv_obj_set_style_opa(s_main_realtime_card, LV_OPA_TRANSP,
                          LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_transform_zoom(s_main_realtime_card, 256,
                                     LV_PART_MAIN | LV_STATE_DEFAULT);
 
     lv_anim_t card_fade;
     lv_anim_init(&card_fade);
     lv_anim_set_var(&card_fade, s_main_realtime_card);
     lv_anim_set_exec_cb(&card_fade, screen_main_realtime_opa_exec);
     lv_anim_set_values(&card_fade, LV_OPA_TRANSP, LV_OPA_COVER);
     lv_anim_set_time(&card_fade, 300);
     lv_anim_set_path_cb(&card_fade, lv_anim_path_ease_out);
     lv_anim_start(&card_fade);
 
     lv_anim_t card_x;
     lv_anim_init(&card_x);
     lv_anim_set_var(&card_x, s_main_realtime_card);
     lv_anim_set_exec_cb(&card_x, screen_main_nav_x_exec);
     lv_anim_set_values(&card_x, 18, 57);
     lv_anim_set_time(&card_x, 430);
     lv_anim_set_path_cb(&card_x, lv_anim_path_ease_out);
     lv_anim_start(&card_x);
 
     lv_anim_t card_y;
     lv_anim_init(&card_y);
     lv_anim_set_var(&card_y, s_main_realtime_card);
     lv_anim_set_exec_cb(&card_y, screen_main_realtime_y_exec);
     lv_anim_set_values(&card_y, 18, 43);
     lv_anim_set_time(&card_y, 430);
     lv_anim_set_path_cb(&card_y, lv_anim_path_ease_out);
     lv_anim_start(&card_y);
 
     lv_anim_t card_w;
     lv_anim_init(&card_w);
     lv_anim_set_var(&card_w, s_main_realtime_card);
     lv_anim_set_exec_cb(&card_w, screen_main_realtime_width_exec);
     lv_anim_set_values(&card_w, 4, 206);
     lv_anim_set_time(&card_w, 430);
     lv_anim_set_path_cb(&card_w, lv_anim_path_overshoot);
     lv_anim_set_ready_cb(&card_w, screen_main_realtime_card_open_ready);
     lv_anim_start(&card_w);
 
     lv_anim_t card_h;
     lv_anim_init(&card_h);
     lv_anim_set_var(&card_h, s_main_realtime_card);
     lv_anim_set_exec_cb(&card_h, screen_main_realtime_height_exec);
     lv_anim_set_values(&card_h, 4, 86);
     lv_anim_set_time(&card_h, 430);
     lv_anim_set_path_cb(&card_h, lv_anim_path_overshoot);
     lv_anim_start(&card_h);
 
 }
 
 static void screen_main_realtime_card_hide(void)
 {
     if (s_main_realtime_card == NULL ||
         !lv_obj_is_valid(s_main_realtime_card) ||
         s_main_realtime_card_time == NULL ||
         !lv_obj_is_valid(s_main_realtime_card_time) ||
         s_main_realtime_card_date == NULL ||
         !lv_obj_is_valid(s_main_realtime_card_date) ||
         lv_obj_has_flag(s_main_realtime_card, LV_OBJ_FLAG_HIDDEN)) {
         return;
     }
 
     lv_anim_del(s_main_realtime_card, NULL);
     lv_anim_del(s_main_realtime_card_time, NULL);
     lv_anim_del(s_main_realtime_card_date, NULL);
 
     lv_anim_t card_fade;
     lv_anim_init(&card_fade);
     lv_anim_set_var(&card_fade, s_main_realtime_card);
     lv_anim_set_exec_cb(&card_fade, screen_main_realtime_opa_exec);
     lv_anim_set_values(&card_fade,
                        lv_obj_get_style_opa(s_main_realtime_card,
                                             LV_PART_MAIN),
                        LV_OPA_TRANSP);
     lv_anim_set_time(&card_fade, 240);
     lv_anim_set_ready_cb(&card_fade, screen_main_realtime_hide_ready);
     lv_anim_start(&card_fade);
 
     lv_anim_t card_w;
     lv_anim_init(&card_w);
     lv_anim_set_var(&card_w, s_main_realtime_card);
     lv_anim_set_exec_cb(&card_w, screen_main_realtime_width_exec);
     lv_anim_set_values(&card_w, lv_obj_get_width(s_main_realtime_card), 4);
     lv_anim_set_time(&card_w, 240);
     lv_anim_set_path_cb(&card_w, lv_anim_path_ease_in);
     lv_anim_start(&card_w);
 
     lv_anim_t card_h;
     lv_anim_init(&card_h);
     lv_anim_set_var(&card_h, s_main_realtime_card);
     lv_anim_set_exec_cb(&card_h, screen_main_realtime_height_exec);
     lv_anim_set_values(&card_h, lv_obj_get_height(s_main_realtime_card), 4);
     lv_anim_set_time(&card_h, 240);
     lv_anim_set_path_cb(&card_h, lv_anim_path_ease_in);
     lv_anim_start(&card_h);
 
     lv_anim_t time_fade;
     lv_anim_init(&time_fade);
     lv_anim_set_var(&time_fade, s_main_realtime_card_time);
     lv_anim_set_exec_cb(&time_fade, screen_main_realtime_text_opa_exec);
     lv_anim_set_values(&time_fade,
                        lv_obj_get_style_text_opa(s_main_realtime_card_time,
                                                  LV_PART_MAIN),
                        LV_OPA_TRANSP);
     lv_anim_set_time(&time_fade, 150);
     lv_anim_set_ready_cb(&time_fade, screen_main_realtime_hide_ready);
     lv_anim_start(&time_fade);
 
     lv_anim_t date_fade;
     lv_anim_init(&date_fade);
     lv_anim_set_var(&date_fade, s_main_realtime_card_date);
     lv_anim_set_exec_cb(&date_fade, screen_main_realtime_text_opa_exec);
     lv_anim_set_values(&date_fade,
                        lv_obj_get_style_text_opa(s_main_realtime_card_date,
                                                 LV_PART_MAIN),
                        LV_OPA_TRANSP);
     lv_anim_set_time(&date_fade, 120);
     lv_anim_set_ready_cb(&date_fade, screen_main_realtime_hide_ready);
     lv_anim_start(&date_fade);
 }
 
 static void screen_main_realtime_card_event_cb(lv_event_t *e)
 {
     if (lv_event_get_code(e) == LV_EVENT_FOCUSED) {
         screen_main_realtime_card_show();
     } else if (lv_event_get_code(e) == LV_EVENT_DEFOCUSED) {
         screen_main_realtime_card_hide();
     }
 }
 
 static void screen_main_realtime_card_bind_hook(void)
 {
     lv_obj_t *button = guider_ui.screen_main_btn_realtime;
     if (button == NULL || !lv_obj_is_valid(button)) {
         return;
     }
     screen_main_realtime_card_create();
     lv_obj_add_event_cb(button, screen_main_realtime_card_event_cb,
                         LV_EVENT_FOCUSED, NULL);
     lv_obj_add_event_cb(button, screen_main_realtime_card_event_cb,
                         LV_EVENT_DEFOCUSED, NULL);
 }
 
 typedef enum {
     SCREEN_MAIN_CAT_NONE = 0,
     SCREEN_MAIN_CAT_LEFT,
     SCREEN_MAIN_CAT_RIGHT,
 } screen_main_cat_side_t;
 
 static screen_main_cat_side_t s_screen_main_cat_visible = SCREEN_MAIN_CAT_NONE;
 
 static bool screen_main_cat_is_ready(lv_obj_t *cat)
 {
     return cat != NULL && lv_obj_is_valid(cat);
 }
 
 static void screen_main_cat_hide_ready(lv_anim_t *anim)
 {
     lv_obj_t *cat = (lv_obj_t *)anim->var;
     if (screen_main_cat_is_ready(cat)) {
         lv_obj_add_flag(cat, LV_OBJ_FLAG_HIDDEN);
     }
 }
 
 static void screen_main_cat_prepare(lv_obj_t *cat, lv_coord_t x, lv_coord_t y)
 {
     if (!screen_main_cat_is_ready(cat)) {
         return;
     }
 
     lv_anim_del(cat, screen_main_nav_x_exec);
     lv_anim_del(cat, screen_main_realtime_opa_exec);
     lv_obj_clear_flag(cat, LV_OBJ_FLAG_HIDDEN);
     lv_obj_set_pos(cat, x, y);
     lv_obj_set_style_opa(cat, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_move_foreground(cat);
     lv_animimg_start(cat);
 }
 
 static void screen_main_cat_show(lv_obj_t *cat, lv_coord_t from_x, lv_coord_t to_x)
 {
     if (!screen_main_cat_is_ready(cat)) {
         return;
     }
 
     screen_main_cat_prepare(cat, from_x, 50);
 
     lv_anim_t x_anim;
     lv_anim_init(&x_anim);
     lv_anim_set_var(&x_anim, cat);
     lv_anim_set_exec_cb(&x_anim, screen_main_nav_x_exec);
     lv_anim_set_values(&x_anim, from_x, to_x);
     lv_anim_set_time(&x_anim, 300);
     lv_anim_set_path_cb(&x_anim, lv_anim_path_overshoot);
     lv_anim_start(&x_anim);
 
     lv_anim_t fade_anim;
     lv_anim_init(&fade_anim);
     lv_anim_set_var(&fade_anim, cat);
     lv_anim_set_exec_cb(&fade_anim, screen_main_realtime_opa_exec);
     lv_anim_set_values(&fade_anim, LV_OPA_TRANSP, LV_OPA_COVER);
     lv_anim_set_time(&fade_anim, 180);
     lv_anim_set_path_cb(&fade_anim, lv_anim_path_ease_out);
     lv_anim_start(&fade_anim);
 }
 
 static void screen_main_cat_hide(lv_obj_t *cat, lv_coord_t to_x)
 {
     if (!screen_main_cat_is_ready(cat) ||
         lv_obj_has_flag(cat, LV_OBJ_FLAG_HIDDEN)) {
         return;
     }
 
     lv_anim_del(cat, screen_main_nav_x_exec);
     lv_anim_del(cat, screen_main_realtime_opa_exec);
 
     lv_anim_t x_anim;
     lv_anim_init(&x_anim);
     lv_anim_set_var(&x_anim, cat);
     lv_anim_set_exec_cb(&x_anim, screen_main_nav_x_exec);
     lv_anim_set_values(&x_anim, lv_obj_get_x(cat), to_x);
     lv_anim_set_time(&x_anim, 220);
     lv_anim_set_path_cb(&x_anim, lv_anim_path_ease_in);
     lv_anim_start(&x_anim);
 
     lv_anim_t fade_anim;
     lv_anim_init(&fade_anim);
     lv_anim_set_var(&fade_anim, cat);
     lv_anim_set_exec_cb(&fade_anim, screen_main_realtime_opa_exec);
     lv_anim_set_values(&fade_anim,
                        lv_obj_get_style_opa(cat, LV_PART_MAIN),
                        LV_OPA_TRANSP);
     lv_anim_set_time(&fade_anim, 160);
     lv_anim_set_ready_cb(&fade_anim, screen_main_cat_hide_ready);
     lv_anim_set_path_cb(&fade_anim, lv_anim_path_ease_in);
     lv_anim_start(&fade_anim);
 }
 
 static void screen_main_cat_focus_set(screen_main_cat_side_t side)
 {
     if (side != SCREEN_MAIN_CAT_LEFT) {
         screen_main_cat_hide(guider_ui.screen_main_cat_left, -55);
     }
     if (side != SCREEN_MAIN_CAT_RIGHT) {
         screen_main_cat_hide(guider_ui.screen_main_cat_right, 320);
     }
 
     if (side == SCREEN_MAIN_CAT_LEFT) {
         screen_main_cat_show(guider_ui.screen_main_cat_left, -55, 0);
     } else if (side == SCREEN_MAIN_CAT_RIGHT) {
         screen_main_cat_show(guider_ui.screen_main_cat_right, 320, 265);
     }
 
     s_screen_main_cat_visible = side;
 }
 
 static void screen_main_cat_focus_event_cb(lv_event_t *e)
 {
     lv_event_code_t code = lv_event_get_code(e);
     lv_obj_t *target = lv_event_get_target(e);
 
     if (code == LV_EVENT_FOCUSED) {
         if (target == guider_ui.screen_main_btn_todolist) {
             screen_main_cat_focus_set(SCREEN_MAIN_CAT_LEFT);
         } else if (target == guider_ui.screen_main_btn_setting) {
             screen_main_cat_focus_set(SCREEN_MAIN_CAT_RIGHT);
         } else {
             screen_main_cat_focus_set(SCREEN_MAIN_CAT_NONE);
         }
     } else if (code == LV_EVENT_DEFOCUSED &&
                (target == guider_ui.screen_main_btn_todolist ||
                 target == guider_ui.screen_main_btn_setting)) {
         screen_main_cat_focus_set(SCREEN_MAIN_CAT_NONE);
     }
 }
 
 static void screen_main_cat_focus_init_async(void *unused)
 {
     LV_UNUSED(unused);
     if (guider_ui.screen_main == NULL ||
         !lv_obj_is_valid(guider_ui.screen_main) ||
         g == NULL) {
         return;
     }
 
     lv_obj_t *focused = lv_group_get_focused(g);
     if (focused == guider_ui.screen_main_btn_todolist) {
         screen_main_cat_focus_set(SCREEN_MAIN_CAT_LEFT);
     } else if (focused == guider_ui.screen_main_btn_setting) {
         screen_main_cat_focus_set(SCREEN_MAIN_CAT_RIGHT);
     } else {
         screen_main_cat_focus_set(SCREEN_MAIN_CAT_NONE);
     }
 }
 
 static void screen_main_cat_focus_bind_hook(void)
 {
     if (guider_ui.screen_main == NULL ||
         !lv_obj_is_valid(guider_ui.screen_main)) {
         return;
     }
 
     lv_obj_t *buttons[] = {
         guider_ui.screen_main_btn_todolist,
         guider_ui.screen_main_btn_done,
         guider_ui.screen_main_btn_timer,
         guider_ui.screen_main_btn_ai,
         guider_ui.screen_main_btn_gashapon,
         guider_ui.screen_main_btn_word,
         guider_ui.screen_main_btn_fun,
         guider_ui.screen_main_btn_photo,
         guider_ui.screen_main_btn_game,
         guider_ui.screen_main_btn_setting,
         guider_ui.screen_main_btn_realtime,
     };
 
     s_screen_main_cat_visible = SCREEN_MAIN_CAT_NONE;
 
     if (screen_main_cat_is_ready(guider_ui.screen_main_cat_left)) {
         lv_anim_del(guider_ui.screen_main_cat_left, NULL);
         lv_obj_set_pos(guider_ui.screen_main_cat_left, -55, 50);
         lv_obj_set_style_opa(guider_ui.screen_main_cat_left, LV_OPA_TRANSP,
                              LV_PART_MAIN | LV_STATE_DEFAULT);
         lv_obj_add_flag(guider_ui.screen_main_cat_left, LV_OBJ_FLAG_HIDDEN);
     }
     if (screen_main_cat_is_ready(guider_ui.screen_main_cat_right)) {
         lv_anim_del(guider_ui.screen_main_cat_right, NULL);
         lv_obj_set_pos(guider_ui.screen_main_cat_right, 320, 50);
         lv_obj_set_style_opa(guider_ui.screen_main_cat_right, LV_OPA_TRANSP,
                              LV_PART_MAIN | LV_STATE_DEFAULT);
         lv_obj_add_flag(guider_ui.screen_main_cat_right, LV_OBJ_FLAG_HIDDEN);
     }
 
     for (size_t i = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++) {
         if (buttons[i] != NULL && lv_obj_is_valid(buttons[i])) {
             while (lv_obj_remove_event_cb(buttons[i],
                                           screen_main_cat_focus_event_cb)) {
             }
             lv_obj_add_event_cb(buttons[i], screen_main_cat_focus_event_cb,
                                 LV_EVENT_FOCUSED, NULL);
             lv_obj_add_event_cb(buttons[i], screen_main_cat_focus_event_cb,
                                 LV_EVENT_DEFOCUSED, NULL);
         }
     }
 
     lv_async_call(screen_main_cat_focus_init_async, NULL);
 }
 
 static void screen_main_extra_scr_clicked(lv_event_t *e);
 static void screen_main_gash_init_async(void *unused);
 static void screen_main_keep_init_async(void *unused);
 static void screen_main_game_init_async(void *unused);
 static void screen_main_book_init_async(void *unused);

 static void screen_main_gash_init_async(void *unused)
 {
     LV_UNUSED(unused);
     /* PRESSED 时 fade delay 尚未结束，lv_scr_act() 仍是被 clean 的主页。
      * custom_init 只会先绑 DELETE；真正抢 keypad group 要等 poll / 本屏已是 act。 */
     screen_gash_custom_init();
 }

 static void screen_main_keep_init_async(void *unused)
 {
     LV_UNUSED(unused);
     screen_keep_custom_init();
 }

 static void screen_main_game_init_async(void *unused)
 {
     LV_UNUSED(unused);
#ifndef ESP_PLATFORM
     /* The standalone Guider preview has no firmware page poll. */
     if (guider_ui.screen_game != NULL &&
         lv_obj_is_valid(guider_ui.screen_game) &&
         lv_scr_act() != guider_ui.screen_game) {
         lv_async_call(screen_main_game_init_async, NULL);
         return;
     }
#endif
     /* Firmware init is idempotent; main/game_machine.c polls through the fade. */
     screen_game_custom_init();
 }

 static void screen_main_book_init_async(void *unused)
 {
     LV_UNUSED(unused);
     /* PRESSED 时 fade delay 尚未结束，lv_scr_act() 仍是被 clean 的主页。
      * custom_init 只会先绑 DELETE；真正抢 keypad group 要等 poll / 本屏已是 act。 */
     screen_book_custom_init();
 }

 static void screen_main_locale_cb(lv_event_t *e)
 {
     lv_obj_t *target;
     const char *en = NULL;
     const char *zh = NULL;

     if (lv_event_get_code(e) != LV_EVENT_FOCUSED) {
         return;
     }
     if (guider_ui.screen_main_label_chosen == NULL ||
         !lv_obj_is_valid(guider_ui.screen_main_label_chosen)) {
         return;
     }
     target = lv_event_get_target(e);
     if (target == guider_ui.screen_main_btn_todolist) {
         en = "Todo"; zh = "待办";
     } else if (target == guider_ui.screen_main_btn_done) {
         en = "Done"; zh = "完成";
     } else if (target == guider_ui.screen_main_btn_timer) {
         en = "Focus"; zh = "专注";
     } else if (target == guider_ui.screen_main_btn_ai) {
         en = "Keep"; zh = "养成";
     } else if (target == guider_ui.screen_main_btn_gashapon) {
         en = "Surprise"; zh = "惊喜";
     } else if (target == guider_ui.screen_main_btn_word) {
         en = "E-book"; zh = "阅读";
     } else if (target == guider_ui.screen_main_btn_fun) {
         en = "Ask Reppo"; zh = "问答";
     } else if (target == guider_ui.screen_main_btn_photo) {
         en = "Picture"; zh = "相册";
     } else if (target == guider_ui.screen_main_btn_game) {
         en = "Game"; zh = "游戏";
     } else if (target == guider_ui.screen_main_btn_setting) {
         en = "Setting"; zh = "设置";
     } else if (target == guider_ui.screen_main_btn_realtime) {
         en = "Real Time"; zh = "时间";
     }
     if (en != NULL) {
         lv_label_set_text(guider_ui.screen_main_label_chosen, ui_tr(en, zh));
     }
 }

 void screen_main_buttion_add(){
     if(g == NULL) {
         g = lv_group_create();
     } else {
         lv_group_remove_all_objs(g);
         lv_group_set_edge_cb(g, NULL);
         lv_group_set_wrap(g, true);
     }
     //setting default group
     lv_group_set_default(g);
     //indev with group
     lv_indev_t * indev = NULL;
     while ((indev = lv_indev_get_next(indev))) {
         if (lv_indev_get_type(indev) == LV_INDEV_TYPE_KEYPAD) {
             lv_indev_set_group(indev, g);
         }
     }
     ui_font_bind(guider_ui.screen_main);
     ui_font_set_chrome(guider_ui.screen_main_label_chosen);
     {
         lv_obj_t *chosen_btns[] = {
             guider_ui.screen_main_btn_todolist,
             guider_ui.screen_main_btn_done,
             guider_ui.screen_main_btn_timer,
             guider_ui.screen_main_btn_ai,
             guider_ui.screen_main_btn_gashapon,
             guider_ui.screen_main_btn_word,
             guider_ui.screen_main_btn_fun,
             guider_ui.screen_main_btn_photo,
             guider_ui.screen_main_btn_game,
             guider_ui.screen_main_btn_setting,
             guider_ui.screen_main_btn_realtime,
         };
         size_t i;
         for (i = 0; i < sizeof(chosen_btns) / sizeof(chosen_btns[0]); i++) {
             if (chosen_btns[i] != NULL && lv_obj_is_valid(chosen_btns[i])) {
                 lv_obj_add_event_cb(chosen_btns[i], screen_main_locale_cb,
                                     LV_EVENT_FOCUSED, NULL);
             }
         }
     }
     lv_group_add_obj(g, guider_ui.screen_main_btn_todolist);
     lv_group_add_obj(g, guider_ui.screen_main_btn_done);
     lv_group_add_obj(g, guider_ui.screen_main_btn_timer);
     lv_group_add_obj(g, guider_ui.screen_main_btn_ai);
     lv_group_add_obj(g, guider_ui.screen_main_btn_gashapon);
     lv_group_add_obj(g, guider_ui.screen_main_btn_word);
     lv_group_add_obj(g, guider_ui.screen_main_btn_fun);
     lv_group_add_obj(g, guider_ui.screen_main_btn_photo);
     lv_group_add_obj(g, guider_ui.screen_main_btn_game);
     lv_group_add_obj(g, guider_ui.screen_main_btn_setting);
     lv_group_add_obj(g, guider_ui.screen_main_btn_realtime);
     //clear focus box
     clear_focus_box(guider_ui.screen_main_btn_todolist);
     clear_focus_box(guider_ui.screen_main_btn_done);
     clear_focus_box(guider_ui.screen_main_btn_timer);
     clear_focus_box(guider_ui.screen_main_btn_ai);
     clear_focus_box(guider_ui.screen_main_btn_gashapon);
     clear_focus_box(guider_ui.screen_main_btn_word);
     clear_focus_box(guider_ui.screen_main_btn_fun);
     clear_focus_box(guider_ui.screen_main_btn_photo);
     clear_focus_box(guider_ui.screen_main_btn_game);
     clear_focus_box(guider_ui.screen_main_btn_setting);
     clear_focus_box(guider_ui.screen_main_btn_realtime);
     screen_main_nav_effect_bind_hooks();
     screen_main_realtime_card_bind_hook();
     screen_main_cat_focus_bind_hook();
     if (guider_ui.screen_main_btn_gashapon != NULL &&
         lv_obj_is_valid(guider_ui.screen_main_btn_gashapon)) {
         /* PRESSED 在 Guider 的 CLICKED(clean 主页) 之前，才能排到异步初始化。 */
         lv_obj_add_event_cb(guider_ui.screen_main_btn_gashapon,
                             screen_main_extra_scr_clicked,
                             LV_EVENT_PRESSED, NULL);
     }
     if (guider_ui.screen_main_btn_ai != NULL &&
         lv_obj_is_valid(guider_ui.screen_main_btn_ai)) {
         lv_obj_add_event_cb(guider_ui.screen_main_btn_ai,
                             screen_main_extra_scr_clicked,
                             LV_EVENT_PRESSED, NULL);
     }
     if (guider_ui.screen_main_btn_game != NULL &&
         lv_obj_is_valid(guider_ui.screen_main_btn_game)) {
         lv_obj_add_event_cb(guider_ui.screen_main_btn_game,
                             screen_main_extra_scr_clicked,
                             LV_EVENT_PRESSED, NULL);
     }
     if (guider_ui.screen_main_btn_word != NULL &&
         lv_obj_is_valid(guider_ui.screen_main_btn_word)) {
         lv_obj_add_event_cb(guider_ui.screen_main_btn_word,
                             screen_main_extra_scr_clicked,
                             LV_EVENT_PRESSED, NULL);
     }
     lv_async_call(screen_main_status_refresh_async, NULL);
 }
 
 static void screen_main_status_refresh_async(void *unused)
 {
     LV_UNUSED(unused);
     main_screen_status_refresh();
 }
 
 /* GUI Guider 模拟器用弱符号占位；ESP-IDF 固件由 main/app_ui.c 强符号覆盖 */
 __attribute__((weak)) void main_screen_status_refresh(void)
 {
 }
 
 static void screen_timing_enter_obj(lv_obj_t *obj,
                                     lv_coord_t start_x, lv_coord_t start_y,
                                     lv_coord_t end_x, lv_coord_t end_y,
                                     uint32_t delay, uint32_t duration)
 {
     if (obj == NULL || !lv_obj_is_valid(obj)) {
         return;
     }
 
     lv_anim_del(obj, NULL);
     lv_obj_set_pos(obj, start_x, start_y);
     lv_obj_set_style_opa(obj, LV_OPA_TRANSP,
                          LV_PART_MAIN | LV_STATE_DEFAULT);
 
     lv_anim_t x_anim;
     lv_anim_init(&x_anim);
     lv_anim_set_var(&x_anim, obj);
     lv_anim_set_exec_cb(&x_anim, screen_main_nav_x_exec);
     lv_anim_set_values(&x_anim, start_x, end_x);
     lv_anim_set_delay(&x_anim, delay);
     lv_anim_set_time(&x_anim, duration);
     lv_anim_set_path_cb(&x_anim, lv_anim_path_ease_out);
     lv_anim_start(&x_anim);
 
     lv_anim_t y_anim;
     lv_anim_init(&y_anim);
     lv_anim_set_var(&y_anim, obj);
     lv_anim_set_exec_cb(&y_anim, screen_main_realtime_y_exec);
     lv_anim_set_values(&y_anim, start_y, end_y);
     lv_anim_set_delay(&y_anim, delay);
     lv_anim_set_time(&y_anim, duration);
     lv_anim_set_path_cb(&y_anim, lv_anim_path_ease_out);
     lv_anim_start(&y_anim);
 
     lv_anim_t fade_anim;
     lv_anim_init(&fade_anim);
     lv_anim_set_var(&fade_anim, obj);
     lv_anim_set_exec_cb(&fade_anim, screen_main_realtime_opa_exec);
     lv_anim_set_values(&fade_anim, LV_OPA_TRANSP, LV_OPA_COVER);
     lv_anim_set_delay(&fade_anim, delay);
     lv_anim_set_time(&fade_anim, duration > 300 ? 300 : duration);
     lv_anim_set_path_cb(&fade_anim, lv_anim_path_ease_out);
     lv_anim_start(&fade_anim);
 }
 
 static void screen_timing_prepare_obj(lv_obj_t *obj,
                                       lv_coord_t x, lv_coord_t y)
 {
     if (obj == NULL || !lv_obj_is_valid(obj)) {
         return;
     }
     lv_anim_del(obj, NULL);
     lv_obj_set_pos(obj, x, y);
     lv_obj_set_style_opa(obj, LV_OPA_TRANSP,
                          LV_PART_MAIN | LV_STATE_DEFAULT);
 }
 
 static void screen_timing_prepare_enter_state(void)
 {
     /* SCREEN_LOAD_START 阶段先摆到动画起点，避免最终布局闪现一帧。 */
     screen_timing_prepare_obj(guider_ui.screen_timing_label_realtime, -145, 43);
     screen_timing_prepare_obj(guider_ui.screen_timing_label_date, 320, 49);
     screen_timing_prepare_obj(guider_ui.screen_timing_label_day, 320, 72);
     screen_timing_prepare_obj(guider_ui.screen_timing_bar_battery, 32, 151);
     screen_timing_prepare_obj(guider_ui.screen_timing_img_baticon, 326, 109);
     screen_timing_prepare_obj(guider_ui.screen_timing_img_wificonnect, 288, -18);
     screen_timing_prepare_obj(guider_ui.screen_timing_img_wifidis, 288, -18);
 }
 
 static void screen_timing_enter_event_cb(lv_event_t *e)
 {
     if (lv_event_get_code(e) != LV_EVENT_SCREEN_LOADED) {
         return;
     }
 
     /* 主时钟先从左侧进入，日期信息随后从右侧依次进入。 */
     screen_timing_enter_obj(guider_ui.screen_timing_label_realtime,
                             -145, 43, 17, 43, 0, 520);
     screen_timing_enter_obj(guider_ui.screen_timing_label_date,
                             320, 49, 195, 49, 80, 400);
     screen_timing_enter_obj(guider_ui.screen_timing_label_day,
                             320, 72, 198, 72, 150, 400);
 
     /* 底部状态组件从下方与右侧收拢，形成分层入场。 */
     screen_timing_enter_obj(guider_ui.screen_timing_bar_battery,
                             32, 151, 32, 127, 230, 420);
     int32_t battery_percent = 50;
     if (guider_ui.screen_timing_bar_battery != NULL &&
         lv_obj_is_valid(guider_ui.screen_timing_bar_battery)) {
         battery_percent = lv_bar_get_value(guider_ui.screen_timing_bar_battery);
     }
     if (battery_percent < 0) {
         battery_percent = 0;
     } else if (battery_percent > 100) {
         battery_percent = 100;
     }
     lv_coord_t baticon_x = (lv_coord_t)(28 + 257 * battery_percent / 100);
     screen_timing_enter_obj(guider_ui.screen_timing_img_baticon,
                             326, 109, baticon_x, 109, 300, 330);
     screen_timing_enter_obj(guider_ui.screen_timing_img_wificonnect,
                             288, -18, 288, 7, 360, 320);
     screen_timing_enter_obj(guider_ui.screen_timing_img_wifidis,
                             288, -18, 288, 7, 360, 320);
 }
 
 //buttion add function screen_pomodoro
 void screen_timing_buttion_add(){
     if(g == NULL) {
         g = lv_group_create();
     } else {
         lv_group_remove_all_objs(g); 
     }
     //setting default group
     lv_group_set_default(g);
     //indev with group
     lv_indev_t * indev = NULL;
     while ((indev = lv_indev_get_next(indev))) {
         if (lv_indev_get_type(indev) == LV_INDEV_TYPE_KEYPAD) {
             lv_indev_set_group(indev, g);
         }
     }
     lv_group_add_obj(g, guider_ui.screen_timing_btn_main);  
     clear_focus_box(guider_ui.screen_timing_btn_main);
     screen_timing_prepare_enter_state();
     lv_obj_add_event_cb(guider_ui.screen_timing, screen_timing_enter_event_cb,
                         LV_EVENT_SCREEN_LOADED, NULL);
     ui_font_bind(guider_ui.screen_timing);
 
 }
 
 static void pomodoro_pause_icon_refresh_async(void *unused)
 {
     LV_UNUSED(unused);
     pomodoro_pause_icon_refresh();
 }
 
 typedef enum {
     POMODORO_EFFECT_ENTER = 0,
     POMODORO_EFFECT_MODE,
     POMODORO_EFFECT_TIMER,
     POMODORO_EFFECT_START,
 } pomodoro_effect_t;
 
 static lv_obj_t *s_pomodoro_effect_screen;
 static pomodoro_effect_t s_pomodoro_pending_effect;
 
 static void pomodoro_effect_x_exec(void *obj, int32_t value)
 {
     if (obj != NULL && lv_obj_is_valid((lv_obj_t *)obj)) {
         lv_obj_set_x((lv_obj_t *)obj, (lv_coord_t)value);
     }
 }
 
 static void pomodoro_effect_y_exec(void *obj, int32_t value)
 {
     if (obj != NULL && lv_obj_is_valid((lv_obj_t *)obj)) {
         lv_obj_set_y((lv_obj_t *)obj, (lv_coord_t)value);
     }
 }
 
 static void pomodoro_effect_opa_exec(void *obj, int32_t value)
 {
     if (obj != NULL && lv_obj_is_valid((lv_obj_t *)obj)) {
         lv_obj_set_style_opa((lv_obj_t *)obj, (lv_opa_t)value,
                              LV_PART_MAIN | LV_STATE_DEFAULT);
     }
 }
 
 static void pomodoro_effect_animate(lv_obj_t *obj,
                                     lv_coord_t start_x, lv_coord_t start_y,
                                     lv_coord_t end_x, lv_coord_t end_y,
                                     uint32_t delay, uint32_t duration)
 {
     if (obj == NULL || !lv_obj_is_valid(obj) ||
         lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN)) {
         return;
     }
 
     lv_anim_del(obj, pomodoro_effect_x_exec);
     lv_anim_del(obj, pomodoro_effect_y_exec);
     lv_anim_del(obj, pomodoro_effect_opa_exec);
     lv_obj_set_pos(obj, start_x, start_y);
     lv_obj_set_style_opa(obj, LV_OPA_TRANSP,
                          LV_PART_MAIN | LV_STATE_DEFAULT);
 
     lv_anim_t x_anim;
     lv_anim_init(&x_anim);
     lv_anim_set_var(&x_anim, obj);
     lv_anim_set_exec_cb(&x_anim, pomodoro_effect_x_exec);
     lv_anim_set_values(&x_anim, start_x, end_x);
     lv_anim_set_delay(&x_anim, delay);
     lv_anim_set_time(&x_anim, duration);
     lv_anim_set_path_cb(&x_anim, lv_anim_path_ease_out);
     lv_anim_start(&x_anim);
 
     lv_anim_t y_anim;
     lv_anim_init(&y_anim);
     lv_anim_set_var(&y_anim, obj);
     lv_anim_set_exec_cb(&y_anim, pomodoro_effect_y_exec);
     lv_anim_set_values(&y_anim, start_y, end_y);
     lv_anim_set_delay(&y_anim, delay);
     lv_anim_set_time(&y_anim, duration);
     lv_anim_set_path_cb(&y_anim, lv_anim_path_ease_out);
     lv_anim_start(&y_anim);
 
     lv_anim_t fade;
     lv_anim_init(&fade);
     lv_anim_set_var(&fade, obj);
     lv_anim_set_exec_cb(&fade, pomodoro_effect_opa_exec);
     lv_anim_set_values(&fade, LV_OPA_TRANSP, LV_OPA_COVER);
     lv_anim_set_delay(&fade, delay);
     lv_anim_set_time(&fade, duration > 200 ? 200 : duration);
     lv_anim_set_path_cb(&fade, lv_anim_path_ease_out);
     lv_anim_start(&fade);
 }
 
 static void pomodoro_effect_play(pomodoro_effect_t effect)
 {
     if (guider_ui.screen_pomodoro == NULL ||
         !lv_obj_is_valid(guider_ui.screen_pomodoro) ||
         lv_scr_act() != guider_ui.screen_pomodoro) {
         return;
     }
 
     if (effect == POMODORO_EFFECT_ENTER) {
         pomodoro_effect_animate(guider_ui.screen_pomodoro_label_pomodoro,
                                 60, -15, 60, 5, 0, 330);
         pomodoro_effect_animate(guider_ui.screen_pomodoro_label_timer,
                                 150, 40, 94, 40, 45, 380);
         pomodoro_effect_animate(guider_ui.screen_pomodoro_img_countdown,
                                 276, 47, 252, 47, 90, 290);
         pomodoro_effect_animate(guider_ui.screen_pomodoro_img_stopwatch,
                                 276, 47, 252, 47, 90, 290);
     } else if (effect == POMODORO_EFFECT_MODE) {
         pomodoro_effect_animate(guider_ui.screen_pomodoro_label_timer,
                                 124, 40, 94, 40, 0, 260);
         pomodoro_effect_animate(guider_ui.screen_pomodoro_img_countdown,
                                 276, 47, 252, 47, 30, 240);
         pomodoro_effect_animate(guider_ui.screen_pomodoro_img_stopwatch,
                                 276, 47, 252, 47, 30, 240);
     } else if (effect == POMODORO_EFFECT_TIMER) {
         pomodoro_effect_animate(guider_ui.screen_pomodoro_label_timer,
                                 94, 34, 94, 40, 0, 220);
     } else {
         pomodoro_effect_animate(guider_ui.screen_pomodoro_label_timer,
                                 94, 35, 94, 40, 0, 190);
     }
 }
 
 static void pomodoro_timeout_effect_play(void)
 {
     lv_obj_t *timeout = guider_ui.screen_pomodoro_img_timeout;
     if (timeout == NULL || !lv_obj_is_valid(timeout) ||
         lv_obj_has_flag(timeout, LV_OBJ_FLAG_HIDDEN)) {
         return;
     }
 
     lv_anim_del(timeout, pomodoro_effect_y_exec);
     lv_anim_del(timeout, pomodoro_effect_opa_exec);
     lv_obj_set_pos(timeout, 20, 58);
     lv_obj_set_style_opa(timeout, LV_OPA_TRANSP,
                          LV_PART_MAIN | LV_STATE_DEFAULT);
 
     lv_anim_t pop;
     lv_anim_init(&pop);
     lv_anim_set_var(&pop, timeout);
     lv_anim_set_exec_cb(&pop, pomodoro_effect_y_exec);
     lv_anim_set_values(&pop, 58, 22);
     lv_anim_set_time(&pop, 420);
     lv_anim_set_path_cb(&pop, lv_anim_path_overshoot);
     lv_anim_start(&pop);
 
     lv_anim_t fade;
     lv_anim_init(&fade);
     lv_anim_set_var(&fade, timeout);
     lv_anim_set_exec_cb(&fade, pomodoro_effect_opa_exec);
     lv_anim_set_values(&fade, LV_OPA_TRANSP, LV_OPA_COVER);
     lv_anim_set_time(&fade, 220);
     lv_anim_set_path_cb(&fade, lv_anim_path_ease_out);
     lv_anim_start(&fade);
 
     /* 超时图片作为背景，操作按钮必须位于其上方才能显示焦点状态。 */
     if (guider_ui.screen_pomodoro_btn_complete != NULL &&
         lv_obj_is_valid(guider_ui.screen_pomodoro_btn_complete)) {
         lv_obj_move_foreground(guider_ui.screen_pomodoro_btn_complete);
     }
     if (guider_ui.screen_pomodoro_btn_continue != NULL &&
         lv_obj_is_valid(guider_ui.screen_pomodoro_btn_continue)) {
         lv_obj_move_foreground(guider_ui.screen_pomodoro_btn_continue);
     }
     group_focus_obj(guider_ui.screen_pomodoro_btn_complete);
 }
 
 static void pomodoro_effect_async(void *unused)
 {
     LV_UNUSED(unused);
     pomodoro_effect_play(s_pomodoro_pending_effect);
 }
 
 static void pomodoro_effect_event_cb(lv_event_t *e)
 {
     lv_event_code_t code = lv_event_get_code(e);
     lv_obj_t *target = lv_event_get_target(e);
     if (code == LV_EVENT_SCREEN_LOADED) {
         s_pomodoro_pending_effect = POMODORO_EFFECT_ENTER;
     } else if (code == LV_EVENT_SCREEN_UNLOADED) {
         s_pomodoro_effect_screen = NULL;
         return;
     } else if (code == LV_EVENT_CLICKED) {
         if (target == guider_ui.screen_pomodoro_btn_countdown ||
             target == guider_ui.screen_pomodoro_btn_stopwatch) {
             s_pomodoro_pending_effect = POMODORO_EFFECT_MODE;
         } else if (target == guider_ui.screen_pomodoro_btn_plus ||
                    target == guider_ui.screen_pomodoro_btn_minus ||
                    target == guider_ui.screen_pomodoro_btn_addnum ||
                    target == guider_ui.screen_pomodoro_btn_reset) {
             s_pomodoro_pending_effect = POMODORO_EFFECT_TIMER;
         } else if (target == guider_ui.screen_pomodoro_btn_start) {
             /* 运行中马上进入黑屏，无需在背景再播放 Start 位移。 */
             if (pomodoro_countdown && countdown_start_label &&
                 countdown_process_label) {
                 return;
             }
             s_pomodoro_pending_effect = POMODORO_EFFECT_START;
         } else {
             return;
         }
     } else {
         return;
     }
 
     /* 等 generated 回调完成状态和文字更新后再播放。 */
     lv_async_call(pomodoro_effect_async, NULL);
 }
 
 static void pomodoro_effect_bind_hooks(void)
 {
     lv_obj_t *scr = guider_ui.screen_pomodoro;
     if (scr == NULL || !lv_obj_is_valid(scr) ||
         s_pomodoro_effect_screen == scr) {
         return;
     }
     s_pomodoro_effect_screen = scr;
 
     lv_obj_t *entrance_objects[] = {
         guider_ui.screen_pomodoro_label_pomodoro,
         guider_ui.screen_pomodoro_label_timer,
         guider_ui.screen_pomodoro_img_countdown,
         guider_ui.screen_pomodoro_img_stopwatch,
     };
     for (size_t i = 0;
          i < sizeof(entrance_objects) / sizeof(entrance_objects[0]); i++) {
         if (entrance_objects[i] != NULL &&
             lv_obj_is_valid(entrance_objects[i])) {
             lv_obj_set_style_opa(entrance_objects[i], LV_OPA_TRANSP,
                                  LV_PART_MAIN | LV_STATE_DEFAULT);
         }
     }
 
     lv_obj_add_event_cb(scr, pomodoro_effect_event_cb,
                         LV_EVENT_SCREEN_LOADED, NULL);
     lv_obj_add_event_cb(scr, pomodoro_effect_event_cb,
                         LV_EVENT_SCREEN_UNLOADED, NULL);
 
     lv_obj_t *buttons[] = {
         guider_ui.screen_pomodoro_btn_countdown,
         guider_ui.screen_pomodoro_btn_stopwatch,
         guider_ui.screen_pomodoro_btn_plus,
         guider_ui.screen_pomodoro_btn_minus,
         guider_ui.screen_pomodoro_btn_addnum,
         guider_ui.screen_pomodoro_btn_reset,
         guider_ui.screen_pomodoro_btn_start,
     };
     for (size_t i = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++) {
         if (buttons[i] != NULL && lv_obj_is_valid(buttons[i])) {
             lv_obj_add_event_cb(buttons[i], pomodoro_effect_event_cb,
                                 LV_EVENT_CLICKED, NULL);
         }
     }
 }
 
 static void pomodoro_pause_icon_after_guider(lv_event_t *e)
 {
     lv_event_code_t code = lv_event_get_code(e);
     if (code == LV_EVENT_CLICKED || code == LV_EVENT_SHORT_CLICKED) {
         /* 等 events_init 里 btn_start 逻辑跑完后再刷新，避免时序竞态。 */
         lv_async_call(pomodoro_pause_icon_refresh_async, NULL);
     } else if (code == LV_EVENT_SCREEN_LOADED) {
         pomodoro_pause_icon_refresh();
     }
 }
 
 static lv_obj_t *s_pomodoro_pause_bound_screen;
 
 static void pomodoro_pause_icon_screen_unloaded(lv_event_t *e)
 {
     if (lv_event_get_code(e) == LV_EVENT_SCREEN_UNLOADED) {
         s_pomodoro_pause_bound_screen = NULL;
     }
 }
 
 static void pomodoro_pause_icon_bind_hooks(void)
 {
     lv_obj_t *scr = guider_ui.screen_pomodoro;
     if (scr == NULL || !lv_obj_is_valid(scr)) {
         return;
     }
     if (s_pomodoro_pause_bound_screen == scr) {
         return;
     }
     s_pomodoro_pause_bound_screen = scr;
 
     lv_obj_add_event_cb(scr, pomodoro_pause_icon_after_guider, LV_EVENT_SCREEN_LOADED, NULL);
     lv_obj_add_event_cb(scr, pomodoro_pause_icon_screen_unloaded, LV_EVENT_SCREEN_UNLOADED, NULL);
 
     lv_obj_t *btns[] = {
         guider_ui.screen_pomodoro_btn_start,
         guider_ui.screen_pomodoro_btn_reset,
         guider_ui.screen_pomodoro_btn_back,
         guider_ui.screen_pomodoro_btn_complete,
         guider_ui.screen_pomodoro_btn_continue,
         guider_ui.screen_pomodoro_btn_stopwatch,
         guider_ui.screen_pomodoro_btn_countdown,
         NULL,
     };
     for (int i = 0; btns[i] != NULL; i++) {
         lv_obj_add_event_cb(btns[i], pomodoro_pause_icon_after_guider, LV_EVENT_CLICKED, NULL);
         lv_obj_add_event_cb(btns[i], pomodoro_pause_icon_after_guider, LV_EVENT_SHORT_CLICKED, NULL);
     }
 }
 
 static void pomodoro_black_start_click_cb(lv_event_t *e)
 {
     if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
         return;
     }
 
     /* generated 的 Start 回调先更新运行/暂停标志，这里只消费最终状态。 */
     if (pomodoro_countdown && countdown_start_label &&
         countdown_process_label) {
         pomodoro_black_enter();
     } else if (s_pomodoro_black_active) {
         pomodoro_black_exit(true);
     }
 }
 
 static void pomodoro_black_screen_unloaded_cb(lv_event_t *e)
 {
     if (lv_event_get_code(e) != LV_EVENT_SCREEN_UNLOADED) {
         return;
     }
 
     pomodoro_black_exit(false);
     s_pomodoro_black_total_seconds = 0;
     s_pomodoro_black_hit = NULL;
     s_pomodoro_black_bound_screen = NULL;
 }
 
 static void pomodoro_black_bind_hooks(void)
 {
     lv_obj_t *screen = guider_ui.screen_pomodoro;
     if (screen == NULL || !lv_obj_is_valid(screen) ||
         s_pomodoro_black_bound_screen == screen) {
         return;
     }
     s_pomodoro_black_bound_screen = screen;
 
     lv_obj_add_event_cb(screen, pomodoro_black_screen_unloaded_cb,
                         LV_EVENT_SCREEN_UNLOADED, NULL);
     if (guider_ui.screen_pomodoro_btn_start != NULL &&
         lv_obj_is_valid(guider_ui.screen_pomodoro_btn_start)) {
         lv_obj_add_event_cb(guider_ui.screen_pomodoro_btn_start,
                             pomodoro_black_start_click_cb,
                             LV_EVENT_CLICKED, NULL);
     }
 }
 
//--------------- 任务秒表暂停结算（stopwatch + 待办任务） ----------------//
/* 从待办任务进入番茄钟并选择秒表模式时，按暂停后弹出与倒计时超时一致的
 * Complete/Continue 结算层：Complete 以秒表实际用时完成任务并归档，
 * Continue 从暂停处继续走秒。全部通过 post-Guider 钩子实现，不改 generated。 */
static bool s_stopwatch_settle_active = false;
static uint32_t s_stopwatch_settle_seconds = 0;
static lv_obj_t *s_stopwatch_settle_screen = NULL;

static void stopwatch_settle_show(void)
{
    lv_obj_clear_state(guider_ui.screen_pomodoro_btn_complete, LV_STATE_DISABLED);
    lv_obj_clear_state(guider_ui.screen_pomodoro_btn_continue, LV_STATE_DISABLED);
    lv_obj_clear_flag(guider_ui.screen_pomodoro_img_timeout, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_state(guider_ui.screen_pomodoro_btn_minus, LV_STATE_DISABLED);
    lv_obj_add_state(guider_ui.screen_pomodoro_btn_plus, LV_STATE_DISABLED);
    lv_obj_add_state(guider_ui.screen_pomodoro_btn_addnum, LV_STATE_DISABLED);
    lv_obj_add_state(guider_ui.screen_pomodoro_btn_reset, LV_STATE_DISABLED);
    lv_obj_add_state(guider_ui.screen_pomodoro_btn_start, LV_STATE_DISABLED);
    lv_obj_add_state(guider_ui.screen_pomodoro_btn_back, LV_STATE_DISABLED);
    lv_obj_add_state(guider_ui.screen_pomodoro_btn_countdown, LV_STATE_DISABLED);
    lv_obj_add_state(guider_ui.screen_pomodoro_btn_stopwatch, LV_STATE_DISABLED);
    lv_obj_add_flag(guider_ui.screen_pomodoro_btn_start, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(guider_ui.screen_pomodoro_btn_reset, LV_OBJ_FLAG_HIDDEN);
    pomodoro_timeout_effect_play();
}

static void stopwatch_settle_resume(void)
{
    /* generated 的 Continue 回调已把界面重置回倒计时默认态，这里恢复秒表续走 */
    pomodoro_countdown = 0;
    pomodoro_stopwatch = 1;
    stopwatch_start_label = 1;
    stopwatch_process_label = 1;
    timer_all_num = 0;
    stopwatch_start();
    timer_all_seconds = s_stopwatch_settle_seconds;
    lv_obj_clear_flag(guider_ui.screen_pomodoro_img_stopwatch, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(guider_ui.screen_pomodoro_img_countdown, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text_fmt(guider_ui.screen_pomodoro_label_timer,
                          "%02d:%02d:%02d",
                          (int)(timer_all_seconds / 3600),
                          (int)((timer_all_seconds % 3600) / 60),
                          (int)(timer_all_seconds % 60));
}

static void stopwatch_settle_after_start_async(void *unused)
{
    LV_UNUSED(unused);
    if (s_stopwatch_settle_active) {
        return;
    }
    /* 秒表已启动再暂停：弹出 Complete/Continue。
     * 不要求 focus_session_active，主页进入的秒表同样可用；
     * 真正标记待办完成仍只在 Complete 且有任务会话时发生。 */
    if (pomodoro_stopwatch && stopwatch_start_label &&
        !stopwatch_process_label) {
        s_stopwatch_settle_active = true;
        s_stopwatch_settle_seconds = timer_all_seconds;
        stopwatch_settle_show();
    }
}

static void stopwatch_settle_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);

    if (code == LV_EVENT_SCREEN_UNLOADED) {
        s_stopwatch_settle_active = false;
        s_stopwatch_settle_screen = NULL;
        return;
    }
    if (code != LV_EVENT_CLICKED && code != LV_EVENT_SHORT_CLICKED) {
        return;
    }

    if (target == guider_ui.screen_pomodoro_btn_start) {
        /* 等 generated 把 pause 标志写完再判断，避免和 SHORT_CLICKED 时序打架。 */
        lv_async_call(stopwatch_settle_after_start_async, NULL);
        return;
    }
    if (code != LV_EVENT_CLICKED) {
        return;
    }

    if (target == guider_ui.screen_pomodoro_btn_complete ||
        target == guider_ui.screen_pomodoro_btn_continue) {
        /* 倒计时到点后蜂鸣和流水灯不会跟着 generated 回调停，这里统一停。 */
        pomodoro_alarm_dismiss();
    }

    if (target == guider_ui.screen_pomodoro_btn_complete) {
        if (s_stopwatch_settle_active) {
            s_stopwatch_settle_active = false;
            /* 以秒表实际用时（四舍五入，至少 1 分钟）作为任务时长，
             * 置 pending 交由主循环 focus_bridge_on_countdown_complete()
             * 统一完成任务标记、归档与专注时长累计。 */
            int minutes = (int)((s_stopwatch_settle_seconds + 30U) / 60U);
            if (minutes < 1) {
                minutes = 1;
            }
            focus_session_minutes = minutes;
            focus_complete_pending = 1;
            /* generated 的 Complete 回调把内部状态重置成了倒计时并把
             * label_timer 写成预设时长，但界面模式图标仍是秒表，
             * 这里恢复为秒表空闲态并把计时归零，保持显示一致。 */
            pomodoro_countdown = 0;
            pomodoro_stopwatch = 1;
            timer_all_num = 0;
            timer_all_seconds = 0;
            lv_label_set_text(guider_ui.screen_pomodoro_label_timer,
                              "00:00:00");
            /* generated 回调只给 btn_start 加了 FOCUSED 视觉态，
             * group 实际焦点仍在已禁用的 Complete 上，需真正移过去。 */
            group_focus_obj(guider_ui.screen_pomodoro_btn_start);
        }
    } else if (target == guider_ui.screen_pomodoro_btn_continue) {
        if (s_stopwatch_settle_active) {
            s_stopwatch_settle_active = false;
            stopwatch_settle_resume();
            /* 同上：把 group 焦点从已禁用的 Continue 真正切回 btn_start，
             * 否则按下确认键仍打在 Continue 上，Start 点不动。 */
            group_focus_obj(guider_ui.screen_pomodoro_btn_start);
        }
    }
}

static void stopwatch_settle_bind_hooks(void)
{
    lv_obj_t *scr = guider_ui.screen_pomodoro;
    if (scr == NULL || !lv_obj_is_valid(scr) ||
        s_stopwatch_settle_screen == scr) {
        return;
    }
    s_stopwatch_settle_screen = scr;
    s_stopwatch_settle_active = false;

    lv_obj_add_event_cb(scr, stopwatch_settle_event_cb,
                        LV_EVENT_SCREEN_UNLOADED, NULL);
    lv_obj_add_event_cb(guider_ui.screen_pomodoro_btn_start,
                        stopwatch_settle_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(guider_ui.screen_pomodoro_btn_start,
                        stopwatch_settle_event_cb, LV_EVENT_SHORT_CLICKED, NULL);
    lv_obj_add_event_cb(guider_ui.screen_pomodoro_btn_complete,
                        stopwatch_settle_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(guider_ui.screen_pomodoro_btn_continue,
                        stopwatch_settle_event_cb, LV_EVENT_CLICKED, NULL);
}

//buttion add function screen_pomodoro
void screen_pomodoro_buttion_add(){
     if(g == NULL) {
         g = lv_group_create();
     } else {
         lv_group_remove_all_objs(g); 
     }
     //setting default group
     lv_group_set_default(g);
     //indev with group
     lv_indev_t * indev = NULL;
     while ((indev = lv_indev_get_next(indev))) {
         if (lv_indev_get_type(indev) == LV_INDEV_TYPE_KEYPAD) {
             lv_indev_set_group(indev, g);
         }
     }
     lv_group_add_obj(g, guider_ui.screen_pomodoro_btn_start);
     lv_group_add_obj(g, guider_ui.screen_pomodoro_btn_reset);
     lv_group_add_obj(g, guider_ui.screen_pomodoro_btn_back);  
     lv_group_add_obj(g, guider_ui.screen_pomodoro_btn_stopwatch); 
     lv_group_add_obj(g, guider_ui.screen_pomodoro_btn_countdown);
     lv_group_add_obj(g, guider_ui.screen_pomodoro_btn_minus);
     lv_group_add_obj(g, guider_ui.screen_pomodoro_btn_addnum);
     lv_group_add_obj(g, guider_ui.screen_pomodoro_btn_plus);
     lv_group_add_obj(g, guider_ui.screen_pomodoro_btn_complete);
     lv_group_add_obj(g, guider_ui.screen_pomodoro_btn_continue);
     clear_edge_box(guider_ui.screen_pomodoro_btn_start);
     clear_edge_box(guider_ui.screen_pomodoro_btn_reset);
     clear_edge_box(guider_ui.screen_pomodoro_btn_back);
     clear_edge_box(guider_ui.screen_pomodoro_btn_stopwatch);
     clear_edge_box(guider_ui.screen_pomodoro_btn_countdown);
     clear_edge_box(guider_ui.screen_pomodoro_btn_minus);
     clear_edge_box(guider_ui.screen_pomodoro_btn_addnum);
     clear_edge_box(guider_ui.screen_pomodoro_btn_plus);
     clear_edge_box(guider_ui.screen_pomodoro_btn_complete);
     clear_edge_box(guider_ui.screen_pomodoro_btn_continue);
 
   pomodoro_pause_icon_bind_hooks();
   pomodoro_effect_bind_hooks();
   pomodoro_black_bind_hooks();
   stopwatch_settle_bind_hooks();
   if (guider_ui.screen_pomodoro_label_pomodoro != NULL &&
       lv_obj_is_valid(guider_ui.screen_pomodoro_label_pomodoro)) {
       lv_label_set_text(guider_ui.screen_pomodoro_label_pomodoro,
                         ui_tr("Pomodoro Focus", "番茄专注"));
   }
   ui_font_bind(guider_ui.screen_pomodoro);
}

 void screen_wordmenu_buttion_add(void)
 {
 }

 void wordcard_hide_dog_anims(void)
 {
 }

 void wordcard_show_dog_dui(void)
 {
 }

 void wordcard_show_dog_cuo(void)
 {
 }

 void screen_wordcard_buttion_add(void)
 {
 }

 void screen_reviewcard_buttion_add(void)
 {
 }
 
 void todo_ble_sync_ui_refresh(void)
 {
     done_sum = 0;
     for (uint8_t i = 0; i < ui_todo_list.count; i++) {
         if (ui_todo_list.items[i].done) {
             done_sum++;
         }
     }
     todo_sum = ui_todo_list.count - done_sum;
 
     if (guider_ui.screen_main_label_todonum != NULL &&
         lv_obj_is_valid(guider_ui.screen_main_label_todonum)) {
         lv_label_set_text_fmt(guider_ui.screen_main_label_todonum, "%d", todo_sum);
     }
 
     if (ui_todo_list.count > 0 &&
         guider_ui.screen_todolist != NULL &&
         lv_obj_is_valid(guider_ui.screen_todolist) &&
         lv_scr_act() == guider_ui.screen_todolist &&
         guider_ui.screen_todolist_btn_todo1_label != NULL &&
         lv_obj_is_valid(guider_ui.screen_todolist_btn_todo1_label)) {
         page_cur = 1;
         lv_event_send(guider_ui.screen_todolist, LV_EVENT_SCREEN_LOAD_START, NULL);
         screen_todolist_ensure_group_focus();
         lv_async_call(todolist_effect_play_async, NULL);
     }
 }
 
 void screen_setting_ble_refresh_icons(void)
 {
     lv_obj_t *img_on = guider_ui.screen_setting_img_ble;
     lv_obj_t *img_dis = guider_ui.screen_setting_img_ble_dis;
 
     if (guider_ui.screen_setting == NULL || !lv_obj_is_valid(guider_ui.screen_setting)) {
         return;
     }
     if (lv_scr_act() != guider_ui.screen_setting) {
         return;
     }
     if (img_on == NULL || img_dis == NULL ||
         !lv_obj_is_valid(img_on) || !lv_obj_is_valid(img_dis)) {
         return;
     }
     if (ble_enabled) {
         lv_obj_clear_flag(img_on, LV_OBJ_FLAG_HIDDEN);
         lv_obj_add_flag(img_dis, LV_OBJ_FLAG_HIDDEN);
     } else {
         lv_obj_add_flag(img_on, LV_OBJ_FLAG_HIDDEN);
         lv_obj_clear_flag(img_dis, LV_OBJ_FLAG_HIDDEN);
     }
 }
 
 static void screen_setting_ble_icon_after_guider(lv_event_t *e)
 {
     lv_event_code_t code = lv_event_get_code(e);
     if (code == LV_EVENT_FOCUSED || code == LV_EVENT_CLICKED) {
         screen_setting_ble_refresh_icons();
     }
 }
 
 static lv_obj_t *s_screen_setting_ble_bound_screen;
 static lv_obj_t *s_screen_setting_effect_bound_screen;
 static void screen_setting_list_teardown(void);
 
 static void screen_setting_ble_icon_bind_hooks(void)
 {
     lv_obj_t *btn = guider_ui.screen_setting_btn_ble;
     lv_obj_t *screen = guider_ui.screen_setting;
     if (screen == NULL || !lv_obj_is_valid(screen) ||
         btn == NULL || !lv_obj_is_valid(btn)) {
         return;
     }
     if (s_screen_setting_ble_bound_screen == screen) {
         return;
     }
     s_screen_setting_ble_bound_screen = screen;
     lv_obj_add_event_cb(btn, screen_setting_ble_icon_after_guider, LV_EVENT_FOCUSED, NULL);
     lv_obj_add_event_cb(btn, screen_setting_ble_icon_after_guider, LV_EVENT_CLICKED, NULL);
 }
 
 static void screen_setting_icon_zoom_exec(void *obj, int32_t value)
 {
     if (obj != NULL && lv_obj_is_valid((lv_obj_t *)obj)) {
         lv_img_set_zoom((lv_obj_t *)obj, (uint16_t)value);
     }
 }
 
 static void screen_setting_icon_opa_exec(void *obj, int32_t value)
 {
     if (obj != NULL && lv_obj_is_valid((lv_obj_t *)obj)) {
         lv_obj_set_style_img_opa((lv_obj_t *)obj, (lv_opa_t)value,
                                  LV_PART_MAIN | LV_STATE_DEFAULT);
     }
 }
 
 static void screen_setting_label_opa_exec(void *obj, int32_t value)
 {
     if (obj != NULL && lv_obj_is_valid((lv_obj_t *)obj)) {
         lv_obj_set_style_text_opa((lv_obj_t *)obj, (lv_opa_t)value,
                                   LV_PART_MAIN | LV_STATE_DEFAULT);
     }
 }
 
 static lv_opa_t screen_setting_icon_target_opa(lv_obj_t *icon)
 {
     if (icon == guider_ui.screen_setting_img_brightness) {
         static const lv_opa_t icon_opa[BRIGHTNESS_LEVEL_COUNT] = {
             70, 115, 160, 210, 255
         };
         uint8_t level = brightness_level;
         if (level >= BRIGHTNESS_LEVEL_COUNT) {
             level = BRIGHTNESS_LEVEL_COUNT - 1U;
         }
         return icon_opa[level];
     }
     return LV_OPA_COVER;
 }
 
 static void screen_setting_icon_pop(lv_obj_t *icon)
 {
     if (icon == NULL || !lv_obj_is_valid(icon) ||
         lv_obj_has_flag(icon, LV_OBJ_FLAG_HIDDEN)) {
         return;
     }
 
     lv_opa_t target_opa = screen_setting_icon_target_opa(icon);
     lv_anim_del(icon, screen_setting_icon_zoom_exec);
     lv_anim_del(icon, screen_setting_icon_opa_exec);
     lv_img_set_zoom(icon, 214);
     lv_obj_set_style_img_opa(icon, LV_OPA_TRANSP,
                              LV_PART_MAIN | LV_STATE_DEFAULT);
 
     lv_anim_t zoom;
     lv_anim_init(&zoom);
     lv_anim_set_var(&zoom, icon);
     lv_anim_set_exec_cb(&zoom, screen_setting_icon_zoom_exec);
     lv_anim_set_values(&zoom, 214, 256);
     lv_anim_set_time(&zoom, 260);
     lv_anim_set_path_cb(&zoom, lv_anim_path_overshoot);
     lv_anim_start(&zoom);
 
     lv_anim_t fade;
     lv_anim_init(&fade);
     lv_anim_set_var(&fade, icon);
     lv_anim_set_exec_cb(&fade, screen_setting_icon_opa_exec);
     lv_anim_set_values(&fade, LV_OPA_TRANSP, target_opa);
     lv_anim_set_time(&fade, 150);
     lv_anim_set_path_cb(&fade, lv_anim_path_ease_out);
     lv_anim_start(&fade);
 }
 
 static void screen_setting_icon_pulse(lv_obj_t *icon)
 {
     if (icon == NULL || !lv_obj_is_valid(icon) ||
         lv_obj_has_flag(icon, LV_OBJ_FLAG_HIDDEN)) {
         return;
     }
 
     lv_anim_del(icon, screen_setting_icon_zoom_exec);
     lv_obj_set_style_img_opa(icon, screen_setting_icon_target_opa(icon),
                              LV_PART_MAIN | LV_STATE_DEFAULT);
 
     lv_anim_t pulse;
     lv_anim_init(&pulse);
     lv_anim_set_var(&pulse, icon);
     lv_anim_set_exec_cb(&pulse, screen_setting_icon_zoom_exec);
     lv_anim_set_values(&pulse, 256, 232);
     lv_anim_set_time(&pulse, 90);
     lv_anim_set_playback_time(&pulse, 150);
     lv_anim_set_path_cb(&pulse, lv_anim_path_ease_in_out);
     lv_anim_start(&pulse);
 }
 
 static void screen_setting_animate_visible_icons(bool clicked)
 {
     lv_obj_t *icons[] = {
         guider_ui.screen_setting_img_wifi_open,
         guider_ui.screen_setting_img_wifi_close,
         guider_ui.screen_setting_img_ble,
         guider_ui.screen_setting_img_ble_dis,
         guider_ui.screen_setting_img_volume_open,
         guider_ui.screen_setting_img_volume_close,
         guider_ui.screen_setting_img_brightness,
     };
 
     for (size_t i = 0; i < sizeof(icons) / sizeof(icons[0]); i++) {
         if (clicked) {
             screen_setting_icon_pulse(icons[i]);
         } else {
             screen_setting_icon_pop(icons[i]);
         }
     }
 }
 
 static void screen_setting_general_content_animate(void)
 {
     lv_obj_t *labels[] = {
         guider_ui.screen_setting_label_about,
         guider_ui.screen_setting_label_txt1,
         guider_ui.screen_setting_label_info1,
         guider_ui.screen_setting_label_txt2,
         guider_ui.screen_setting_label_info2,
         guider_ui.screen_setting_label_txt3,
         guider_ui.screen_setting_label_info3,
         guider_ui.screen_setting_label_txt4,
         guider_ui.screen_setting_label_info4,
     };
 
     for (size_t i = 0; i < sizeof(labels) / sizeof(labels[0]); i++) {
         lv_obj_t *label = labels[i];
         if (label == NULL || !lv_obj_is_valid(label) ||
             lv_obj_has_flag(label, LV_OBJ_FLAG_HIDDEN)) {
             continue;
         }
         lv_anim_del(label, screen_setting_label_opa_exec);
         lv_obj_set_style_text_opa(label, LV_OPA_TRANSP,
                                   LV_PART_MAIN | LV_STATE_DEFAULT);
 
         lv_anim_t fade;
         lv_anim_init(&fade);
         lv_anim_set_var(&fade, label);
         lv_anim_set_exec_cb(&fade, screen_setting_label_opa_exec);
         lv_anim_set_values(&fade, LV_OPA_TRANSP, LV_OPA_COVER);
         lv_anim_set_delay(&fade, (uint32_t)i * 24U);
         lv_anim_set_time(&fade, 180);
         lv_anim_set_path_cb(&fade, lv_anim_path_ease_out);
         lv_anim_start(&fade);
     }
 }
 
 static void screen_setting_visual_effect_cb(lv_event_t *e)
 {
     lv_event_code_t code = lv_event_get_code(e);
     lv_obj_t *target = lv_event_get_target(e);
 
     if (code == LV_EVENT_FOCUSED) {
         if (target == guider_ui.screen_setting_btn_general) {
             screen_setting_general_content_animate();
         } else {
             screen_setting_animate_visible_icons(false);
         }
     } else if (code == LV_EVENT_CLICKED &&
                target != guider_ui.screen_setting_btn_wifi &&
                target != guider_ui.screen_setting_btn_general) {
         screen_setting_animate_visible_icons(true);
     }
 }
 
 static void screen_setting_initial_effect_async(void *unused)
 {
     LV_UNUSED(unused);
     if (guider_ui.screen_setting != NULL &&
         lv_obj_is_valid(guider_ui.screen_setting) &&
         lv_scr_act() == guider_ui.screen_setting) {
         screen_setting_animate_visible_icons(false);
     }
 }
 
 static void screen_setting_effect_screen_event_cb(lv_event_t *e)
 {
     if (lv_event_get_code(e) == LV_EVENT_SCREEN_UNLOADED) {
         lv_async_call_cancel(screen_setting_initial_effect_async, NULL);
         screen_setting_list_teardown();
         s_screen_setting_ble_bound_screen = NULL;
         s_screen_setting_effect_bound_screen = NULL;
     }
 }
 
 static void screen_setting_visual_effect_bind_hooks(void)
 {
     lv_obj_t *screen = guider_ui.screen_setting;
     if (screen == NULL || !lv_obj_is_valid(screen) ||
         s_screen_setting_effect_bound_screen == screen) {
         return;
     }
     s_screen_setting_effect_bound_screen = screen;
 
     lv_obj_t *buttons[] = {
         guider_ui.screen_setting_btn_wifi,
         guider_ui.screen_setting_btn_ble,
         guider_ui.screen_setting_btn_volume,
         guider_ui.screen_setting_btn_brightness,
         guider_ui.screen_setting_btn_general,
     };
 
     for (size_t i = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++) {
         if (buttons[i] != NULL && lv_obj_is_valid(buttons[i])) {
             lv_obj_add_event_cb(buttons[i], screen_setting_visual_effect_cb,
                                 LV_EVENT_FOCUSED, NULL);
             lv_obj_add_event_cb(buttons[i], screen_setting_visual_effect_cb,
                                 LV_EVENT_CLICKED, NULL);
         }
     }
     lv_obj_add_event_cb(screen, screen_setting_effect_screen_event_cb,
                         LV_EVENT_SCREEN_UNLOADED, NULL);
     /* 首个 Wi-Fi 按钮在加入 group 时已自动 focus，延迟到 generated
      * SCREEN_LOAD_START 完成后再播放首次图标动画。 */
     lv_async_call(screen_setting_initial_effect_async, NULL);
 }
 
 /* 左侧设置按钮滚动区：对齐原 WIFI(y=35) 到 General(y=135+16) 的上下边界。 */
 #define SETTING_LIST_X           19
 #define SETTING_LIST_Y           35
 #define SETTING_LIST_W           95
 #define SETTING_LIST_H           116
 #define SETTING_BTN_W            95
 #define SETTING_BTN_H            16
 #define SETTING_BTN_STEP         25
 #define SETTING_BTN_FOCUS_LIFT   5
 #define SETTING_LIST_CORE_COUNT  5
 #define SETTING_LIST_EXTRA_MAX   8
 
 typedef struct {
     const char *text;
 } setting_extra_desc_t;
 
 /* 在此追加新设置项即可，尺寸/字体/焦点样式与 WIFI 五键一致。 */
 static const setting_extra_desc_t s_setting_extra_descs[] = {
     { "Sleep" },
     { "Language" },
 };
 
 static lv_obj_t *s_setting_list;
 static lv_obj_t *s_setting_extra_btns[SETTING_LIST_EXTRA_MAX];
 static uint8_t s_setting_extra_count;
 static lv_obj_t *s_setting_btn_sleep;
 static lv_obj_t *s_setting_btn_lang;
 static lv_obj_t *s_sleep_value_label;
 static lv_obj_t *s_lang_value_label;

 static const char *const s_sleep_level_text[SLEEP_LEVEL_COUNT] = {
     "Off",
     "1min",
     "5min",
     "10min",
 };
 
 static lv_obj_t *screen_setting_list_core_btn(uint8_t index)
 {
     switch (index) {
     case 0: return guider_ui.screen_setting_btn_wifi;
     case 1: return guider_ui.screen_setting_btn_ble;
     case 2: return guider_ui.screen_setting_btn_volume;
     case 3: return guider_ui.screen_setting_btn_brightness;
     case 4: return guider_ui.screen_setting_btn_general;
     default: return NULL;
     }
 }
 
 static bool screen_setting_list_ready(void)
 {
     return s_setting_list != NULL &&
            lv_obj_is_valid(s_setting_list) &&
            guider_ui.screen_setting != NULL &&
            lv_obj_is_valid(guider_ui.screen_setting) &&
            lv_obj_get_parent(s_setting_list) == guider_ui.screen_setting;
 }
 
 static int screen_setting_list_btn_index(const lv_obj_t *btn)
 {
     uint8_t i;
 
     for (i = 0; i < SETTING_LIST_CORE_COUNT; i++) {
         if (btn == screen_setting_list_core_btn(i)) {
             return (int)i;
         }
     }
     for (i = 0; i < s_setting_extra_count; i++) {
         if (btn == s_setting_extra_btns[i]) {
             return (int)SETTING_LIST_CORE_COUNT + (int)i;
         }
     }
     return -1;
 }
 
 static lv_coord_t screen_setting_list_rest_y(int index)
 {
     return (lv_coord_t)(index * SETTING_BTN_STEP);
 }
 
 static bool screen_setting_sleep_ui_ready(void)
 {
     return s_sleep_value_label != NULL &&
            lv_obj_is_valid(s_sleep_value_label) &&
            guider_ui.screen_setting != NULL &&
            lv_obj_get_parent(s_sleep_value_label) == guider_ui.screen_setting;
 }

 static void screen_setting_sleep_hide(void)
 {
     if (s_sleep_value_label != NULL && lv_obj_is_valid(s_sleep_value_label)) {
         lv_obj_add_flag(s_sleep_value_label, LV_OBJ_FLAG_HIDDEN);
     }
 }

 static void screen_setting_sleep_place_label(void)
 {
     lv_obj_set_pos(s_sleep_value_label, 135, 31);
     lv_obj_set_size(s_sleep_value_label, 170, 126);
     lv_obj_set_style_pad_top(s_sleep_value_label, 47,
                              LV_PART_MAIN | LV_STATE_DEFAULT);
 }

 static void screen_setting_sleep_create_ui(void)
 {
     if (guider_ui.screen_setting == NULL ||
         !lv_obj_is_valid(guider_ui.screen_setting) ||
         screen_setting_sleep_ui_ready()) {
         return;
     }

     s_sleep_value_label = lv_label_create(guider_ui.screen_setting);
     lv_label_set_text(s_sleep_value_label, s_sleep_level_text[0]);
     lv_label_set_long_mode(s_sleep_value_label, LV_LABEL_LONG_CLIP);
     screen_setting_sleep_place_label();
     lv_obj_set_style_text_font(s_sleep_value_label,
                                &CUSTOM_DISPLAY_FONT,
                                LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_text_color(s_sleep_value_label, lv_color_hex(0x274a6e),
                                 LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_text_opa(s_sleep_value_label, LV_OPA_COVER,
                               LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_text_align(s_sleep_value_label, LV_TEXT_ALIGN_CENTER,
                                 LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_bg_opa(s_sleep_value_label, LV_OPA_TRANSP,
                             LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_add_flag(s_sleep_value_label, LV_OBJ_FLAG_HIDDEN);
     lv_obj_clear_flag(s_sleep_value_label, LV_OBJ_FLAG_CLICKABLE);
 }

 void sleep_timeout_update_ui(void)
 {
     uint8_t level = sleep_timeout_level;

     if (level >= SLEEP_LEVEL_COUNT) {
         level = 0;
     }
     if (!screen_setting_sleep_ui_ready()) {
         return;
     }

     lv_label_set_text(s_sleep_value_label, s_sleep_level_text[level]);
     screen_setting_sleep_place_label();
     lv_anim_del(s_sleep_value_label, screen_setting_label_opa_exec);
     lv_obj_set_style_text_opa(s_sleep_value_label, LV_OPA_TRANSP,
                               LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_anim_t fade;
     lv_anim_init(&fade);
     lv_anim_set_var(&fade, s_sleep_value_label);
     lv_anim_set_exec_cb(&fade, screen_setting_label_opa_exec);
     lv_anim_set_values(&fade, LV_OPA_TRANSP, LV_OPA_COVER);
     lv_anim_set_time(&fade, 160);
     lv_anim_set_path_cb(&fade, lv_anim_path_ease_out);
     lv_anim_start(&fade);
 }

 static void screen_setting_sleep_show(void)
 {
     screen_setting_sleep_create_ui();
     if (!screen_setting_sleep_ui_ready()) {
         return;
     }
     sleep_timeout_update_ui();
     lv_obj_clear_flag(s_sleep_value_label, LV_OBJ_FLAG_HIDDEN);
 }

 void sleep_timeout_cycle(void)
 {
     sleep_timeout_level = (uint8_t)((sleep_timeout_level + 1U) % SLEEP_LEVEL_COUNT);
     sleep_timeout_update_ui();
     sleep_persist_impl(sleep_timeout_level);
     idle_sleep_note_activity();
 }

 static void screen_setting_sleep_event_cb(lv_event_t *e)
 {
     if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
         sleep_timeout_cycle();
     }
 }

 static void screen_setting_sleep_bind_btn(lv_obj_t *btn)
 {
     if (btn == NULL || !lv_obj_is_valid(btn)) {
         return;
     }
     s_setting_btn_sleep = btn;
     lv_obj_add_event_cb(btn, screen_setting_sleep_event_cb, LV_EVENT_CLICKED, NULL);
 }

 static bool screen_setting_lang_ui_ready(void)
 {
     return s_lang_value_label != NULL &&
            lv_obj_is_valid(s_lang_value_label) &&
            guider_ui.screen_setting != NULL &&
            lv_obj_get_parent(s_lang_value_label) == guider_ui.screen_setting;
 }

 static void screen_setting_lang_hide(void)
 {
     if (s_lang_value_label != NULL && lv_obj_is_valid(s_lang_value_label)) {
         lv_obj_add_flag(s_lang_value_label, LV_OBJ_FLAG_HIDDEN);
     }
 }

 static void screen_setting_apply_locale(void);

 static void screen_setting_lang_show(void)
 {
     if (guider_ui.screen_setting == NULL ||
         !lv_obj_is_valid(guider_ui.screen_setting)) {
         return;
     }
     if (!screen_setting_lang_ui_ready()) {
         s_lang_value_label = lv_label_create(guider_ui.screen_setting);
         lv_obj_set_pos(s_lang_value_label, 135, 31);
         lv_obj_set_size(s_lang_value_label, 170, 126);
         lv_obj_set_style_text_font(s_lang_value_label, ui_font_chrome(),
                                    LV_PART_MAIN | LV_STATE_DEFAULT);
         lv_obj_set_style_text_color(s_lang_value_label, lv_color_hex(0x274a6e),
                                     LV_PART_MAIN | LV_STATE_DEFAULT);
         lv_obj_set_style_text_align(s_lang_value_label, LV_TEXT_ALIGN_CENTER,
                                     LV_PART_MAIN | LV_STATE_DEFAULT);
         lv_obj_set_style_pad_top(s_lang_value_label, 52,
                                  LV_PART_MAIN | LV_STATE_DEFAULT);
         lv_obj_set_style_bg_opa(s_lang_value_label, LV_OPA_TRANSP,
                                 LV_PART_MAIN | LV_STATE_DEFAULT);
         lv_obj_clear_flag(s_lang_value_label, LV_OBJ_FLAG_CLICKABLE);
     }
     lv_label_set_text(s_lang_value_label, ui_tr("English", "中文"));
     lv_obj_clear_flag(s_lang_value_label, LV_OBJ_FLAG_HIDDEN);
 }

 static void screen_setting_lang_event_cb(lv_event_t *e)
 {
     if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
         return;
     }
     ui_lang_toggle();
     screen_setting_apply_locale();
     screen_setting_lang_show();
 }

 static void screen_setting_lang_bind_btn(lv_obj_t *btn)
 {
     if (btn == NULL || !lv_obj_is_valid(btn)) {
         return;
     }
     s_setting_btn_lang = btn;
     lv_obj_add_event_cb(btn, screen_setting_lang_event_cb, LV_EVENT_CLICKED, NULL);
 }

 static void screen_setting_set_label(lv_obj_t *label, const char *text)
 {
     if (label != NULL && lv_obj_is_valid(label)) {
         lv_label_set_text(label, text);
     }
 }

 static void screen_setting_set_btn(lv_obj_t *btn, const char *text)
 {
     lv_obj_t *label;
     if (btn == NULL || !lv_obj_is_valid(btn)) {
         return;
     }
     label = lv_obj_get_child(btn, 0);
     if (label != NULL && lv_obj_is_valid(label)) {
         lv_label_set_text(label, text);
     }
 }

 static void screen_setting_apply_locale(void)
 {
     screen_setting_set_label(guider_ui.screen_setting_btn_wifi_label,
                              ui_tr("WIFI", "无线"));
     screen_setting_set_label(guider_ui.screen_setting_btn_ble_label,
                              ui_tr("BlueTooth", "蓝牙"));
     screen_setting_set_label(guider_ui.screen_setting_btn_volume_label,
                              ui_tr("Volume", "音量"));
     screen_setting_set_label(guider_ui.screen_setting_btn_brightness_label,
                              ui_tr("Brightness", "亮度"));
     screen_setting_set_label(guider_ui.screen_setting_btn_general_label,
                              ui_tr("General", "通用"));
     screen_setting_set_btn(s_setting_btn_sleep, ui_tr("Sleep", "休眠"));
     screen_setting_set_btn(s_setting_btn_lang, ui_tr("Language", "语言"));
     screen_setting_set_label(guider_ui.screen_setting_label_about,
                              ui_tr("About", "关于"));
     screen_setting_set_label(guider_ui.screen_setting_label_txt1,
                              ui_tr("Device", "设备"));
     screen_setting_set_label(guider_ui.screen_setting_label_txt2,
                              ui_tr("Model", "型号"));
     screen_setting_set_label(guider_ui.screen_setting_label_txt4,
                              ui_tr("Hardware", "硬件"));
     if (screen_setting_lang_ui_ready() &&
         !lv_obj_has_flag(s_lang_value_label, LV_OBJ_FLAG_HIDDEN)) {
         lv_label_set_text(s_lang_value_label, ui_tr("English", "中文"));
     }
 }

 static void screen_setting_hide_detail_widgets(void)
 {
     lv_obj_t *widgets[] = {
         guider_ui.screen_setting_img_wifi_open,
         guider_ui.screen_setting_img_wifi_close,
         guider_ui.screen_setting_img_volume_close,
         guider_ui.screen_setting_img_volume_open,
         guider_ui.screen_setting_img_ble,
         guider_ui.screen_setting_img_ble_dis,
         guider_ui.screen_setting_img_brightness,
         guider_ui.screen_setting_label_txt1,
         guider_ui.screen_setting_label_txt2,
         guider_ui.screen_setting_label_txt3,
         guider_ui.screen_setting_label_txt4,
         guider_ui.screen_setting_label_info1,
         guider_ui.screen_setting_label_info2,
         guider_ui.screen_setting_label_info3,
         guider_ui.screen_setting_label_info4,
         guider_ui.screen_setting_label_about,
     };
     size_t i;
 
     for (i = 0; i < sizeof(widgets) / sizeof(widgets[0]); i++) {
         if (widgets[i] != NULL && lv_obj_is_valid(widgets[i])) {
             lv_obj_add_flag(widgets[i], LV_OBJ_FLAG_HIDDEN);
         }
     }
     screen_setting_sleep_hide();
     screen_setting_lang_hide();
 }
 
 static void screen_setting_reset_container_scroll(lv_obj_t *obj)
 {
     if (obj == NULL || !lv_obj_is_valid(obj)) {
         return;
     }
     lv_obj_scroll_to_x(obj, 0, LV_ANIM_OFF);
     lv_obj_scroll_to_y(obj, 0, LV_ANIM_OFF);
 }
 
 static void screen_setting_list_snap_y(lv_obj_t *btn, int index)
 {
     lv_coord_t rest = screen_setting_list_rest_y(index);
 
     lv_anim_del(btn, (lv_anim_exec_xcb_t)lv_obj_set_y);
     lv_obj_set_y(btn, rest);
 }
 
 static void screen_setting_list_snap_all(void)
 {
     uint8_t i;
     lv_obj_t *btn;
 
     if (!screen_setting_list_ready()) {
         return;
     }
 
     for (i = 0; i < SETTING_LIST_CORE_COUNT; i++) {
         btn = screen_setting_list_core_btn(i);
         if (btn != NULL && lv_obj_is_valid(btn)) {
             screen_setting_list_snap_y(btn, (int)i);
         }
     }
     for (i = 0; i < s_setting_extra_count; i++) {
         btn = s_setting_extra_btns[i];
         if (btn != NULL && lv_obj_is_valid(btn)) {
             screen_setting_list_snap_y(btn, (int)SETTING_LIST_CORE_COUNT + (int)i);
         }
     }
 }
 
 static void screen_setting_list_ensure_visible(int index, lv_anim_enable_t anim)
 {
     lv_coord_t y;
     lv_coord_t h;
     lv_coord_t scroll;
     lv_coord_t view_h;
 
     if (!screen_setting_list_ready() || index < 0) {
         return;
     }
 
     y = screen_setting_list_rest_y(index);
     if (y < 0) {
         y = 0;
     }
     h = SETTING_BTN_H;
     scroll = lv_obj_get_scroll_y(s_setting_list);
     view_h = lv_obj_get_height(s_setting_list);
 
     if (y < scroll) {
         lv_obj_scroll_to_y(s_setting_list, y, anim);
     } else if ((y + h) > (scroll + view_h)) {
         lv_obj_scroll_to_y(s_setting_list, y + h - view_h, anim);
     }
 }
 
 static void screen_setting_list_lift(lv_obj_t *btn, int index)
 {
     lv_coord_t rest = screen_setting_list_rest_y(index);
 
     /* 与原先一致：100ms 上抬 5px，再 playback 回落，形成焦点弹跳。 */
     lv_anim_del(btn, (lv_anim_exec_xcb_t)lv_obj_set_y);
     lv_obj_set_y(btn, rest);
     ui_animation(btn, 100, 0, rest, rest - SETTING_BTN_FOCUS_LIFT,
                  &lv_anim_path_linear, 0, 0, 100, 0,
                  (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
 }
 
 static void screen_setting_list_focus_cb(lv_event_t *e)
 {
     lv_obj_t *btn = lv_event_get_target(e);
     lv_event_code_t code = lv_event_get_code(e);
     int index = screen_setting_list_btn_index(btn);
 
     if (index < 0 || btn == NULL || !lv_obj_is_valid(btn)) {
         return;
     }
 
     if (code == LV_EVENT_FOCUSED) {
         /* 只纠正当前项：盖掉 Guider 的屏幕绝对 Y，保留其余按钮的弹跳动效。 */
         screen_setting_list_ensure_visible(index, LV_ANIM_ON);
         if (btn == s_setting_btn_sleep) {
             screen_setting_hide_detail_widgets();
             screen_setting_sleep_show();
         } else if (btn == s_setting_btn_lang) {
             screen_setting_hide_detail_widgets();
             screen_setting_lang_show();
         } else if (index >= SETTING_LIST_CORE_COUNT) {
             screen_setting_hide_detail_widgets();
         } else {
             screen_setting_sleep_hide();
             screen_setting_lang_hide();
         }
         screen_setting_list_lift(btn, index);
     } else if (code == LV_EVENT_DEFOCUSED) {
         screen_setting_list_snap_y(btn, index);
         if (btn == s_setting_btn_sleep) {
             screen_setting_sleep_hide();
         }
         if (btn == s_setting_btn_lang) {
             screen_setting_lang_hide();
         }
     }
 }
 
 static void screen_setting_list_settle(void)
 {
     /* 进入页只清掉屏幕误滚动，不要 snap_all / 重放 lift，以免掐掉焦点弹跳。 */
     if (guider_ui.screen_setting != NULL && lv_obj_is_valid(guider_ui.screen_setting)) {
         screen_setting_reset_container_scroll(guider_ui.screen_setting);
     }
 }
 
 static void screen_setting_list_teardown(void)
 {
     s_setting_list = NULL;
     memset(s_setting_extra_btns, 0, sizeof(s_setting_extra_btns));
     s_setting_extra_count = 0;
     s_setting_btn_sleep = NULL;
     s_setting_btn_lang = NULL;
     s_sleep_value_label = NULL;
     s_lang_value_label = NULL;
 }
 
 static void screen_setting_list_bind_btn(lv_obj_t *btn)
 {
     if (btn == NULL || !lv_obj_is_valid(btn)) {
         return;
     }
     lv_obj_add_flag(btn, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
     while (lv_obj_remove_event_cb(btn, screen_setting_list_focus_cb)) {
     }
     lv_obj_add_event_cb(btn, screen_setting_list_focus_cb, LV_EVENT_FOCUSED, NULL);
     lv_obj_add_event_cb(btn, screen_setting_list_focus_cb, LV_EVENT_DEFOCUSED, NULL);
 }
 
 static void screen_setting_list_apply_btn_style(lv_obj_t *btn)
 {
     lv_obj_set_size(btn, SETTING_BTN_W, SETTING_BTN_H);
     lv_obj_set_style_pad_all(btn, 0, LV_STATE_DEFAULT);
     lv_obj_set_style_bg_opa(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_border_width(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_radius(btn, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_shadow_width(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_text_color(btn, lv_color_hex(0x274a6e),
                                 LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_text_font(btn, &lv_font_SourceHanSansSC_Medium_14,
                                LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_text_opa(btn, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_text_align(btn, LV_TEXT_ALIGN_LEFT,
                                 LV_PART_MAIN | LV_STATE_DEFAULT);
 
     lv_obj_set_style_bg_opa(btn, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
     lv_obj_set_style_bg_color(btn, lv_color_hex(0xffffff),
                               LV_PART_MAIN | LV_STATE_FOCUSED);
     lv_obj_set_style_bg_grad_dir(btn, LV_GRAD_DIR_NONE,
                                  LV_PART_MAIN | LV_STATE_FOCUSED);
     lv_obj_set_style_border_width(btn, 0, LV_PART_MAIN | LV_STATE_FOCUSED);
     lv_obj_set_style_radius(btn, 5, LV_PART_MAIN | LV_STATE_FOCUSED);
     lv_obj_set_style_shadow_width(btn, 0, LV_PART_MAIN | LV_STATE_FOCUSED);
     lv_obj_set_style_text_color(btn, lv_color_hex(0x274a6e),
                                 LV_PART_MAIN | LV_STATE_FOCUSED);
     lv_obj_set_style_text_font(btn, &lv_font_SourceHanSansSC_Medium_14,
                                LV_PART_MAIN | LV_STATE_FOCUSED);
     lv_obj_set_style_text_opa(btn, 255, LV_PART_MAIN | LV_STATE_FOCUSED);
 }
 
 static void screen_setting_list_place_btn(lv_obj_t *btn, int index)
 {
     lv_obj_set_parent(btn, s_setting_list);
     lv_obj_set_pos(btn, 0, screen_setting_list_rest_y(index));
     lv_obj_set_size(btn, SETTING_BTN_W, SETTING_BTN_H);
 }
 
 static void screen_setting_list_reset_layout(void)
 {
     if (!screen_setting_list_ready()) {
         return;
     }
 
     screen_setting_list_snap_all();
     lv_obj_scroll_to_y(s_setting_list, 0, LV_ANIM_OFF);
     lv_obj_update_layout(s_setting_list);
 }
 
 lv_obj_t *screen_setting_list_add_btn(const char *text)
 {
     lv_obj_t *btn;
     lv_obj_t *label;
     int index;
 
     if (text == NULL || !screen_setting_list_ready() ||
         s_setting_extra_count >= SETTING_LIST_EXTRA_MAX) {
         return NULL;
     }
 
     btn = lv_btn_create(s_setting_list);
     if (btn == NULL) {
         return NULL;
     }
 
     label = lv_label_create(btn);
     lv_label_set_text(label, text);
     lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
     lv_obj_align(label, LV_ALIGN_LEFT_MID, 0, 0);
     lv_obj_set_style_pad_all(btn, 0, LV_STATE_DEFAULT);
     lv_obj_set_width(label, LV_PCT(100));
 
     screen_setting_list_apply_btn_style(btn);
     index = (int)SETTING_LIST_CORE_COUNT + (int)s_setting_extra_count;
     lv_obj_set_pos(btn, 0, screen_setting_list_rest_y(index));
     screen_setting_list_bind_btn(btn);
     clear_edge_box(btn);
 
     s_setting_extra_btns[s_setting_extra_count] = btn;
     s_setting_extra_count++;
 
     if (g != NULL && guider_ui.screen_setting_btn_back != NULL &&
         lv_obj_is_valid(guider_ui.screen_setting_btn_back) &&
         lv_obj_get_group(guider_ui.screen_setting_btn_back) == g) {
         lv_group_remove_obj(guider_ui.screen_setting_btn_back);
         lv_group_add_obj(g, btn);
         lv_group_add_obj(g, guider_ui.screen_setting_btn_back);
     }
 
     lv_obj_update_layout(s_setting_list);
     return btn;
 }
 
 static void screen_setting_list_create(void)
 {
     uint8_t i;
     lv_obj_t *btn;
     size_t extra_n;
 
     s_setting_list = lv_obj_create(guider_ui.screen_setting);
     lv_obj_remove_style_all(s_setting_list);
     lv_obj_set_pos(s_setting_list, SETTING_LIST_X, SETTING_LIST_Y);
     lv_obj_set_size(s_setting_list, SETTING_LIST_W, SETTING_LIST_H);
     lv_obj_set_style_bg_opa(s_setting_list, LV_OPA_TRANSP, LV_PART_MAIN);
     lv_obj_set_style_border_width(s_setting_list, 0, LV_PART_MAIN);
     lv_obj_set_style_outline_width(s_setting_list, 0, LV_PART_MAIN);
     lv_obj_set_style_pad_all(s_setting_list, 0, LV_PART_MAIN);
     lv_obj_set_style_radius(s_setting_list, 0, LV_PART_MAIN);
     lv_obj_set_style_shadow_width(s_setting_list, 0, LV_PART_MAIN);
     lv_obj_set_scrollbar_mode(s_setting_list, LV_SCROLLBAR_MODE_OFF);
     lv_obj_set_scroll_dir(s_setting_list, LV_DIR_VER);
     lv_obj_add_flag(s_setting_list, LV_OBJ_FLAG_SCROLLABLE);
     lv_obj_clear_flag(s_setting_list, LV_OBJ_FLAG_CLICKABLE);
     lv_obj_clear_flag(s_setting_list, LV_OBJ_FLAG_SCROLL_ELASTIC);
     lv_obj_clear_flag(s_setting_list, LV_OBJ_FLAG_SCROLL_MOMENTUM);
     lv_obj_clear_flag(s_setting_list, LV_OBJ_FLAG_SCROLL_CHAIN);
     lv_obj_clear_flag(s_setting_list, LV_OBJ_FLAG_CLICK_FOCUSABLE);
     lv_obj_clear_flag(s_setting_list, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
     lv_obj_clear_flag(guider_ui.screen_setting, LV_OBJ_FLAG_SCROLLABLE);
     screen_setting_reset_container_scroll(guider_ui.screen_setting);
 
     s_setting_extra_count = 0;
     memset(s_setting_extra_btns, 0, sizeof(s_setting_extra_btns));
     s_setting_btn_sleep = NULL;
     s_setting_btn_lang = NULL;
 
     for (i = 0; i < SETTING_LIST_CORE_COUNT; i++) {
         btn = screen_setting_list_core_btn(i);
         if (btn != NULL && lv_obj_is_valid(btn)) {
             screen_setting_list_place_btn(btn, (int)i);
             screen_setting_list_bind_btn(btn);
         }
     }
 
     extra_n = sizeof(s_setting_extra_descs) / sizeof(s_setting_extra_descs[0]);
     for (i = 0; i < extra_n && i < SETTING_LIST_EXTRA_MAX; i++) {
         btn = screen_setting_list_add_btn(s_setting_extra_descs[i].text);
         if (btn != NULL && s_setting_extra_descs[i].text != NULL &&
             strcmp(s_setting_extra_descs[i].text, "Sleep") == 0) {
             screen_setting_sleep_bind_btn(btn);
         }
         if (btn != NULL && s_setting_extra_descs[i].text != NULL &&
             strcmp(s_setting_extra_descs[i].text, "Language") == 0) {
             screen_setting_lang_bind_btn(btn);
         }
     }
     screen_setting_sleep_create_ui();
 }
 
 static void screen_setting_list_setup(void)
 {
     if (guider_ui.screen_setting == NULL ||
         !lv_obj_is_valid(guider_ui.screen_setting)) {
         return;
     }
 
     if (!screen_setting_list_ready()) {
         screen_setting_list_create();
     }
     screen_setting_list_reset_layout();
 }
 
 //buttion add function screen_setting
 void screen_setting_buttion_add(){
     uint8_t i;
     lv_obj_t *btn;
 
     if(g == NULL) {
         g = lv_group_create();
     } else {
         lv_group_remove_all_objs(g); 
     }
     /* 创建 Sleep 等额外按钮前不要设 default group，否则会自动入组并
      * SCROLL_ON_FOCUS 把列表/屏幕滚偏。 */
     lv_group_set_default(NULL);
     lv_group_focus_freeze(g, true);
     //indev with group
     lv_indev_t * indev = NULL;
     while ((indev = lv_indev_get_next(indev))) {
         if (lv_indev_get_type(indev) == LV_INDEV_TYPE_KEYPAD) {
             lv_indev_set_group(indev, g);
         }
     }
     if (guider_ui.screen_setting != NULL && lv_obj_is_valid(guider_ui.screen_setting)) {
         lv_obj_clear_flag(guider_ui.screen_setting, LV_OBJ_FLAG_SCROLLABLE);
         screen_setting_reset_container_scroll(guider_ui.screen_setting);
     }
     screen_setting_list_setup();
     if (guider_ui.screen_setting_label_txt4 != NULL &&
         lv_obj_is_valid(guider_ui.screen_setting_label_txt4)) {
         lv_obj_set_width(guider_ui.screen_setting_label_txt4, 72);
     }
     screen_setting_apply_locale();
     ui_font_bind(guider_ui.screen_setting);
     ui_font_set_chrome(guider_ui.screen_setting_btn_wifi);
     ui_font_set_chrome(guider_ui.screen_setting_btn_ble);
     ui_font_set_chrome(guider_ui.screen_setting_btn_volume);
     ui_font_set_chrome(guider_ui.screen_setting_btn_brightness);
     ui_font_set_chrome(guider_ui.screen_setting_btn_general);
     ui_font_set_chrome(s_setting_btn_sleep);
     ui_font_set_chrome(s_setting_btn_lang);
     if (guider_ui.screen_setting_label_info2 != NULL &&
         lv_obj_is_valid(guider_ui.screen_setting_label_info2)) {
         lv_label_set_text(guider_ui.screen_setting_label_info2, "Reppo Zero");
     }
     lv_group_set_default(g);
     lv_group_add_obj(g, guider_ui.screen_setting_btn_wifi);  
     lv_group_add_obj(g, guider_ui.screen_setting_btn_ble);
     lv_group_add_obj(g, guider_ui.screen_setting_btn_volume);
     lv_group_add_obj(g, guider_ui.screen_setting_btn_brightness);
     lv_group_add_obj(g, guider_ui.screen_setting_btn_general);
     for (i = 0; i < s_setting_extra_count; i++) {
         btn = s_setting_extra_btns[i];
         if (btn != NULL && lv_obj_is_valid(btn)) {
             lv_group_add_obj(g, btn);
             clear_edge_box(btn);
         }
     }
     lv_group_add_obj(g, guider_ui.screen_setting_btn_back);  
     clear_edge_box(guider_ui.screen_setting_btn_wifi);
     clear_edge_box(guider_ui.screen_setting_btn_ble);
     clear_edge_box(guider_ui.screen_setting_btn_volume);
     clear_edge_box(guider_ui.screen_setting_btn_brightness);
    clear_edge_box(guider_ui.screen_setting_btn_general);
    clear_edge_box(guider_ui.screen_setting_btn_back);
    if (guider_ui.screen_setting_btn_back != NULL &&
        lv_obj_is_valid(guider_ui.screen_setting_btn_back)) {
        lv_obj_clear_flag(guider_ui.screen_setting_btn_back, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    }
    lv_group_focus_freeze(g, false);
    group_focus_obj(guider_ui.screen_setting_btn_wifi);
    screen_setting_list_settle();
    screen_setting_ble_icon_bind_hooks();
     screen_setting_visual_effect_bind_hooks();
 }
 
 static void voice_wificfg_back_cb(lv_event_t *e)
 {
     if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
         return;
     }
     if (!voice_todo_wants_wificfg_return()) {
         return;
     }
     /* 抢在 generated 的「回设置」之前处理，并停掉后面的回调。 */
     lv_event_stop_processing(e);
     voice_todo_return_from_wifi();
 }

 static void voice_wificfg_back_bind(void)
 {
     static lv_obj_t *bound;
     lv_obj_t *btn = guider_ui.screen_wificfg_btn_back;
     if (btn == NULL || !lv_obj_is_valid(btn) || bound == btn) {
         return;
     }
     bound = btn;
     lv_obj_add_event_cb(btn, voice_wificfg_back_cb,
                         LV_EVENT_CLICKED | LV_EVENT_PREPROCESS, NULL);
 }

 //buttion add function screen_wificfg
 void screen_wificfg_buttion_add(){
     if(g == NULL) {
         g = lv_group_create();
     } else {
         lv_group_remove_all_objs(g); 
     }
     //setting default group
     lv_group_set_default(g);
     //indev with group
     lv_indev_t * indev = NULL;
     while ((indev = lv_indev_get_next(indev))) {
         if (lv_indev_get_type(indev) == LV_INDEV_TYPE_KEYPAD) {
             lv_indev_set_group(indev, g);
         }
     }
     lv_group_add_obj(g, guider_ui.screen_wificfg_list_1_item0);  
     lv_group_add_obj(g, guider_ui.screen_wificfg_list_1_item1);  
     lv_group_add_obj(g, guider_ui.screen_wificfg_list_1_item2);  
     lv_group_add_obj(g, guider_ui.screen_wificfg_list_1_item3);  
     lv_group_add_obj(g, guider_ui.screen_wificfg_list_1_item4);  
     lv_group_add_obj(g, guider_ui.screen_wificfg_btn_scan);  
     lv_group_add_obj(g, guider_ui.screen_wificfg_btn_disconnect);  
     lv_group_add_obj(g, guider_ui.screen_wificfg_ta_password); 
     lv_group_add_obj(g, guider_ui.screen_wificfg_btn_cancel);  
     lv_group_add_obj(g, guider_ui.screen_wificfg_btn_connect); 
     lv_group_add_obj(g, guider_ui.screen_wificfg_btn_back);  
     clear_edge_box(guider_ui.screen_wificfg_list_1_item0);
     clear_edge_box(guider_ui.screen_wificfg_list_1_item1);
     clear_edge_box(guider_ui.screen_wificfg_list_1_item2);
     clear_edge_box(guider_ui.screen_wificfg_list_1_item3);
     clear_edge_box(guider_ui.screen_wificfg_list_1_item4);
     clear_edge_box(guider_ui.screen_wificfg_btn_scan);
     clear_edge_box(guider_ui.screen_wificfg_btn_disconnect);
     clear_edge_box(guider_ui.screen_wificfg_btn_cancel);
     clear_edge_box(guider_ui.screen_wificfg_btn_connect);
     clear_edge_box(guider_ui.screen_wificfg_btn_back);
     voice_wificfg_back_bind();
     if (guider_ui.screen_wificfg_btn_cancel_label != NULL &&
         lv_obj_is_valid(guider_ui.screen_wificfg_btn_cancel_label)) {
         lv_label_set_text(guider_ui.screen_wificfg_btn_cancel_label,
                           ui_tr("Cancel", "取消"));
     }
     if (guider_ui.screen_wificfg_btn_connect_label != NULL &&
         lv_obj_is_valid(guider_ui.screen_wificfg_btn_connect_label)) {
         lv_label_set_text(guider_ui.screen_wificfg_btn_connect_label,
                           ui_tr("Connect", "连接"));
     }
     ui_font_bind(guider_ui.screen_wificfg);
     if (guider_ui.screen_wificfg_label_status != NULL &&
         lv_obj_is_valid(guider_ui.screen_wificfg_label_status)) {
         lv_label_set_text(guider_ui.screen_wificfg_label_status,
                           ui_tr("Connection Status", "连接状态"));
     }
     if (wifi_success) {
         wificfg_status2_set("Connected", "已连接");
     } else {
         wificfg_status2_set("Disconnected", "未连接");
     }
     if (guider_ui.screen_wificfg_btn_disconnect != NULL &&
         lv_obj_is_valid(guider_ui.screen_wificfg_btn_disconnect)) {
         lv_obj_add_event_cb(guider_ui.screen_wificfg_btn_disconnect,
                             wificfg_disconnect_locale_cb, LV_EVENT_CLICKED, NULL);
     }
 } 
 
 /* screen_todosel 底部 Cancel 那一行：左侧显示任务日期，右侧显示预设专注时长。
  * 右上角显示 App/扭蛋标签类目（Work/Study/Life/Fun）；未分类则留空。
  * 该 screen 每次进入都会重建，故用 parent 校验决定是否重新创建。 */
 static lv_obj_t *s_todosel_label_date;
 static lv_obj_t *s_todosel_label_focus;
 static lv_obj_t *s_todosel_label_cat;

 /* 与背景图上的 "Cancel"（x 137~177, y 130~139）同行居中；
  * 左右外沿对齐上方 Complete / Start focus 的文字列（x 30 ~ 281）。 */
 #define TODOSEL_INFO_Y      127
 #define TODOSEL_INFO_W      100
 #define TODOSEL_INFO_H      14
 #define TODOSEL_INFO_DATE_X 30
 #define TODOSEL_INFO_MIN_X  181
 #define TODOSEL_CAT_X       255
 #define TODOSEL_CAT_Y       25
 #define TODOSEL_CAT_W       58
 #define TODOSEL_CAT_H       16

 static const char *todosel_label_text(uint8_t label)
 {
     switch (label) {
    case TODO_LABEL_WORK:  return ui_tr("Work", "工作");
    case TODO_LABEL_STUDY: return ui_tr("Study", "学习");
    case TODO_LABEL_LIFE:  return ui_tr("Life", "生活");
    case TODO_LABEL_FUN:   return ui_tr("Fun", "娱乐");
     default:               return "";
     }
 }

 static uint32_t todosel_label_color(uint8_t label)
 {
     switch (label) {
     case TODO_LABEL_WORK:  return 0x13407a;
     case TODO_LABEL_STUDY: return 0x2e7d4f;
     case TODO_LABEL_LIFE:  return 0xc47b2b;
     case TODO_LABEL_FUN:   return 0xa33d7c;
     default:               return 0x274a6e;
     }
 }

 static bool screen_todosel_info_date_focus_ready(void)
 {
     return s_todosel_label_date != NULL && lv_obj_is_valid(s_todosel_label_date) &&
            s_todosel_label_focus != NULL && lv_obj_is_valid(s_todosel_label_focus) &&
            guider_ui.screen_todosel != NULL &&
            lv_obj_get_parent(s_todosel_label_date) == guider_ui.screen_todosel &&
            lv_obj_get_parent(s_todosel_label_focus) == guider_ui.screen_todosel;
 }

 static bool screen_todosel_info_cat_ready(void)
 {
     return s_todosel_label_cat != NULL && lv_obj_is_valid(s_todosel_label_cat) &&
            guider_ui.screen_todosel != NULL &&
            lv_obj_get_parent(s_todosel_label_cat) == guider_ui.screen_todosel;
 }

 static bool screen_todosel_info_ready(void)
 {
     return screen_todosel_info_date_focus_ready() && screen_todosel_info_cat_ready();
 }

 static void screen_todosel_info_place(lv_obj_t *label, lv_coord_t x)
 {
     lv_obj_set_pos(label, x, TODOSEL_INFO_Y);
     lv_obj_set_size(label, TODOSEL_INFO_W, TODOSEL_INFO_H);
 }

 static lv_obj_t *screen_todosel_info_label_create(lv_coord_t x, lv_text_align_t align)
 {
     lv_obj_t *label = lv_label_create(guider_ui.screen_todosel);

     lv_label_set_text(label, "");
     lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
     screen_todosel_info_place(label, x);
     lv_obj_set_style_text_font(label, &lv_font_SourceHanSansSC_Medium_14,
                                LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_text_color(label, lv_color_hex(0x274a6e),
                                 LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_text_opa(label, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_text_align(label, align, LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_bg_opa(label, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_pad_all(label, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_border_width(label, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_clear_flag(label, LV_OBJ_FLAG_CLICKABLE);
     return label;
 }

 static void screen_todosel_info_create_ui(void)
 {
     if (guider_ui.screen_todosel == NULL ||
         !lv_obj_is_valid(guider_ui.screen_todosel)) {
         return;
     }
     if (!screen_todosel_info_date_focus_ready()) {
         s_todosel_label_date =
             screen_todosel_info_label_create(TODOSEL_INFO_DATE_X, LV_TEXT_ALIGN_LEFT);
         s_todosel_label_focus =
             screen_todosel_info_label_create(TODOSEL_INFO_MIN_X, LV_TEXT_ALIGN_RIGHT);
     }
     if (screen_todosel_info_cat_ready()) {
         return;
     }
     s_todosel_label_cat = lv_label_create(guider_ui.screen_todosel);
     lv_label_set_text(s_todosel_label_cat, "");
     lv_label_set_long_mode(s_todosel_label_cat, LV_LABEL_LONG_CLIP);
     lv_obj_set_pos(s_todosel_label_cat, TODOSEL_CAT_X, TODOSEL_CAT_Y);
     lv_obj_set_size(s_todosel_label_cat, TODOSEL_CAT_W, TODOSEL_CAT_H);
     lv_obj_set_style_text_font(s_todosel_label_cat, &lv_font_SourceHanSansSC_Medium_12,
                                LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_text_color(s_todosel_label_cat, lv_color_hex(0x274a6e),
                                 LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_text_opa(s_todosel_label_cat, LV_OPA_COVER,
                               LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_text_align(s_todosel_label_cat, LV_TEXT_ALIGN_LEFT,
                                 LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_bg_opa(s_todosel_label_cat, LV_OPA_TRANSP,
                             LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_pad_all(s_todosel_label_cat, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_border_width(s_todosel_label_cat, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_clear_flag(s_todosel_label_cat, LV_OBJ_FLAG_CLICKABLE);
 }

 static void screen_todosel_info_update(void)
 {
     screen_todosel_info_create_ui();
     if (!screen_todosel_info_ready()) {
         return;
     }

     /* 每次进入都重新钉住位置，避免受任何布局副作用影响 */
     screen_todosel_info_place(s_todosel_label_date, TODOSEL_INFO_DATE_X);
     screen_todosel_info_place(s_todosel_label_focus, TODOSEL_INFO_MIN_X);
     lv_obj_set_pos(s_todosel_label_cat, TODOSEL_CAT_X, TODOSEL_CAT_Y);
     lv_obj_set_size(s_todosel_label_cat, TODOSEL_CAT_W, TODOSEL_CAT_H);

     int idx = (page_cur - 1) * 4 + todo_index;
     if (idx < 0 || idx >= (int)ui_todo_list.count) {
         lv_label_set_text(s_todosel_label_date, "");
         lv_label_set_text(s_todosel_label_focus, "");
         lv_label_set_text(s_todosel_label_cat, "");
         return;
     }

     /* 日期仅来自微信小程序，未下发时留空 */
     lv_label_set_text(s_todosel_label_date, ui_todo_list.items[idx].planned_date);
     if (ui_todo_list.items[idx].duration_min > 0) {
         lv_label_set_text_fmt(s_todosel_label_focus, "%uMIN",
                               (unsigned)ui_todo_list.items[idx].duration_min);
     } else {
         lv_label_set_text(s_todosel_label_focus, "");
     }
     {
         uint8_t cat = ui_todo_list.items[idx].label;
         lv_label_set_text(s_todosel_label_cat, todosel_label_text(cat));
         lv_obj_set_style_text_color(s_todosel_label_cat,
                                     lv_color_hex(todosel_label_color(cat)),
                                     LV_PART_MAIN | LV_STATE_DEFAULT);
     }
 }

 //buttion add function screen_todosel
 void screen_todosel_buttion_add(){
     if(g == NULL) {
         g = lv_group_create();
     } else {
         lv_group_remove_all_objs(g); 
     }
     //setting default group
     lv_group_set_default(g);
     //indev with group
     lv_indev_t * indev = NULL;
     while ((indev = lv_indev_get_next(indev))) {
         if (lv_indev_get_type(indev) == LV_INDEV_TYPE_KEYPAD) {
             lv_indev_set_group(indev, g);
         }
     }
     lv_group_add_obj(g, guider_ui.screen_todosel_btn_complete);  
     lv_group_add_obj(g, guider_ui.screen_todosel_btn_focus); 
     lv_group_add_obj(g, guider_ui.screen_todosel_btn_cancel); 
     clear_edge_box(guider_ui.screen_todosel_btn_complete);
     clear_edge_box(guider_ui.screen_todosel_btn_focus);
     clear_edge_box(guider_ui.screen_todosel_btn_cancel);
     screen_todosel_info_update();
     ui_font_bind(guider_ui.screen_todosel);
 }
 
 void screen_ai_buttion_add(void)
{
}

typedef enum {
     DONELIST_EFFECT_ENTER = 0,
     DONELIST_EFFECT_NEXT,
     DONELIST_EFFECT_PREVIOUS,
     DONELIST_EFFECT_SAVE,
 } donelist_effect_t;
 
 static lv_obj_t *s_donelist_effect_screen;
 static donelist_effect_t s_donelist_pending_effect;
 static lv_timer_t *s_donelist_restore_timer;
 static lv_obj_t *s_donelist_rows[4];
 
 static void donelist_effect_restore_layout(void)
 {
     lv_obj_t *names[] = {
         guider_ui.screen_donelist_label_done1,
         guider_ui.screen_donelist_label_done2,
         guider_ui.screen_donelist_label_done3,
         guider_ui.screen_donelist_label_done4,
     };
     lv_obj_t *dates[] = {
         guider_ui.screen_donelist_label_date1,
         guider_ui.screen_donelist_label_date2,
         guider_ui.screen_donelist_label_date3,
         guider_ui.screen_donelist_label_date4,
     };
     lv_obj_t *focus[] = {
         guider_ui.screen_donelist_label_focus1,
         guider_ui.screen_donelist_label_focus2,
         guider_ui.screen_donelist_label_focus3,
         guider_ui.screen_donelist_label_focus4,
     };
     lv_obj_t *lines[] = {
         guider_ui.screen_donelist_line_1,
         guider_ui.screen_donelist_line_2,
         guider_ui.screen_donelist_line_3,
         guider_ui.screen_donelist_line_4,
     };
     static const lv_coord_t container_y[] = {32, 59, 85, 112};
 
     for (uint8_t i = 0; i < 4; i++) {
         if (s_donelist_rows[i] != NULL &&
             lv_obj_is_valid(s_donelist_rows[i])) {
             lv_obj_set_pos(s_donelist_rows[i], 0, container_y[i]);
             lv_obj_set_style_opa(s_donelist_rows[i], LV_OPA_COVER,
                                  LV_PART_MAIN | LV_STATE_DEFAULT);
         }
         if (names[i] != NULL && lv_obj_is_valid(names[i])) {
             lv_obj_set_pos(names[i], 28, 6);
         }
         if (dates[i] != NULL && lv_obj_is_valid(dates[i])) {
             lv_obj_set_pos(dates[i], 205, 7);
         }
         if (focus[i] != NULL && lv_obj_is_valid(focus[i])) {
             lv_obj_set_pos(focus[i], 268, 7);
         }
         if (lines[i] != NULL && lv_obj_is_valid(lines[i])) {
             lv_obj_set_pos(lines[i], 25, 13);
         }
     }
 
     if (guider_ui.screen_donelist_label_page != NULL &&
         lv_obj_is_valid(guider_ui.screen_donelist_label_page)) {
         lv_obj_set_pos(guider_ui.screen_donelist_label_page, 117, 147);
         lv_obj_set_style_opa(guider_ui.screen_donelist_label_page, LV_OPA_COVER,
                              LV_PART_MAIN | LV_STATE_DEFAULT);
     }
 }
 
 static void donelist_effect_restore_timer_cb(lv_timer_t *timer)
 {
     LV_UNUSED(timer);
     s_donelist_restore_timer = NULL;
     if (guider_ui.screen_donelist == NULL ||
         !lv_obj_is_valid(guider_ui.screen_donelist) ||
         lv_scr_act() != guider_ui.screen_donelist) {
         return;
     }
     /* 动画结束后强制归位，避免连续异步刷新留下坐标或透明度残值。 */
     donelist_effect_restore_layout();
 }
 
 static void donelist_effect_pos_exec(void *obj, int32_t value)
 {
     if (obj != NULL && lv_obj_is_valid((lv_obj_t *)obj)) {
         lv_obj_set_x((lv_obj_t *)obj, (lv_coord_t)value);
     }
 }
 
 static void donelist_effect_y_exec(void *obj, int32_t value)
 {
     if (obj != NULL && lv_obj_is_valid((lv_obj_t *)obj)) {
         lv_obj_set_y((lv_obj_t *)obj, (lv_coord_t)value);
     }
 }
 
 static void donelist_effect_opa_exec(void *obj, int32_t value)
 {
     if (obj != NULL && lv_obj_is_valid((lv_obj_t *)obj)) {
         lv_obj_set_style_opa((lv_obj_t *)obj, (lv_opa_t)value,
                              LV_PART_MAIN | LV_STATE_DEFAULT);
     }
 }
 
 static void donelist_effect_animate_obj(lv_obj_t *obj,
                                         lv_coord_t end_x, lv_coord_t end_y,
                                         int8_t direction, uint32_t delay)
 {
     if (obj == NULL || !lv_obj_is_valid(obj)) {
         return;
     }
 
     lv_coord_t start_x = end_x;
     lv_coord_t start_y = end_y;
     if (direction > 0) {
         start_x += 54;
     } else if (direction < 0) {
         start_x -= 54;
     } else {
         start_y += 10;
     }
 
     lv_anim_del(obj, NULL);
     lv_obj_set_pos(obj, start_x, start_y);
     lv_obj_set_style_opa(obj, LV_OPA_TRANSP,
                          LV_PART_MAIN | LV_STATE_DEFAULT);
 
     lv_anim_t move;
     lv_anim_init(&move);
     lv_anim_set_var(&move, obj);
     lv_anim_set_exec_cb(&move, direction == 0
                                ? donelist_effect_y_exec
                                : donelist_effect_pos_exec);
     lv_anim_set_values(&move, direction == 0 ? start_y : start_x,
                               direction == 0 ? end_y : end_x);
     lv_anim_set_delay(&move, delay);
     lv_anim_set_time(&move, 300);
     lv_anim_set_path_cb(&move, lv_anim_path_ease_out);
     lv_anim_start(&move);
 
     lv_anim_t fade;
     lv_anim_init(&fade);
     lv_anim_set_var(&fade, obj);
     lv_anim_set_exec_cb(&fade, donelist_effect_opa_exec);
     lv_anim_set_values(&fade, LV_OPA_TRANSP, LV_OPA_COVER);
     lv_anim_set_delay(&fade, delay);
     lv_anim_set_time(&fade, 190);
     lv_anim_set_path_cb(&fade, lv_anim_path_ease_out);
     lv_anim_start(&fade);
 }
 
 static void donelist_effect_play(donelist_effect_t effect)
 {
     if (guider_ui.screen_donelist == NULL ||
         !lv_obj_is_valid(guider_ui.screen_donelist) ||
         lv_scr_act() != guider_ui.screen_donelist) {
         return;
     }
 
     static const lv_coord_t container_y[] = {32, 59, 85, 112};
 
     int8_t direction = effect == DONELIST_EFFECT_PREVIOUS ? -1 :
                        effect == DONELIST_EFFECT_SAVE ? 0 : 1;
     for (uint8_t i = 0; i < 4; i++) {
         uint32_t delay = (uint32_t)i * 55U;
         if (s_donelist_rows[i] != NULL &&
             lv_obj_is_valid(s_donelist_rows[i])) {
             donelist_effect_animate_obj(s_donelist_rows[i], 0,
                                         container_y[i], direction, delay);
         }
     }
 
     donelist_effect_animate_obj(guider_ui.screen_donelist_label_page,
                                 117, 147, 0, 190);
 
     if (s_donelist_restore_timer != NULL) {
         lv_timer_del(s_donelist_restore_timer);
     }
     s_donelist_restore_timer =
         lv_timer_create(donelist_effect_restore_timer_cb, 650, NULL);
     lv_timer_set_repeat_count(s_donelist_restore_timer, 1);
 }
 
 static void donelist_effect_async(void *unused)
 {
     LV_UNUSED(unused);
     donelist_effect_play(s_donelist_pending_effect);
 }
 
 static void donelist_effect_event_cb(lv_event_t *e)
 {
     lv_event_code_t code = lv_event_get_code(e);
     lv_obj_t *target = lv_event_get_target(e);
 
     if (code == LV_EVENT_SCREEN_LOADED) {
         s_donelist_pending_effect = DONELIST_EFFECT_ENTER;
         lv_async_call(donelist_effect_async, NULL);
     } else if (code == LV_EVENT_SCREEN_UNLOADED) {
         if (s_donelist_restore_timer != NULL) {
             lv_timer_del(s_donelist_restore_timer);
             s_donelist_restore_timer = NULL;
         }
         for (uint8_t i = 0; i < 4; i++) {
             s_donelist_rows[i] = NULL;
         }
         s_donelist_effect_screen = NULL;
     } else if (code == LV_EVENT_CLICKED) {
         if (target == guider_ui.screen_donelist_btn_lastpage) {
             s_donelist_pending_effect = DONELIST_EFFECT_PREVIOUS;
         } else if (target == guider_ui.screen_donelist_btn_nxtpage) {
             s_donelist_pending_effect = DONELIST_EFFECT_NEXT;
         } else if (target == guider_ui.screen_donelist_btn_save) {
             s_donelist_pending_effect = DONELIST_EFFECT_SAVE;
         } else {
             return;
         }
         /* generated event 已完成翻页/保存状态更新后再播放。 */
         lv_async_call(donelist_effect_async, NULL);
     }
 }
 
 static void donelist_effect_prepare(void)
 {
     lv_obj_t *objects[] = {
         s_donelist_rows[0],
         s_donelist_rows[1],
         s_donelist_rows[2],
         s_donelist_rows[3],
         guider_ui.screen_donelist_label_page,
     };
     for (size_t i = 0; i < sizeof(objects) / sizeof(objects[0]); i++) {
         if (objects[i] != NULL && lv_obj_is_valid(objects[i])) {
             lv_obj_set_style_opa(objects[i], LV_OPA_TRANSP,
                                  LV_PART_MAIN | LV_STATE_DEFAULT);
         }
     }
 }
 
 static void donelist_effect_create_rows(void)
 {
     lv_obj_t *names[] = {
         guider_ui.screen_donelist_label_done1,
         guider_ui.screen_donelist_label_done2,
         guider_ui.screen_donelist_label_done3,
         guider_ui.screen_donelist_label_done4,
     };
     lv_obj_t *dates[] = {
         guider_ui.screen_donelist_label_date1,
         guider_ui.screen_donelist_label_date2,
         guider_ui.screen_donelist_label_date3,
         guider_ui.screen_donelist_label_date4,
     };
     lv_obj_t *focus[] = {
         guider_ui.screen_donelist_label_focus1,
         guider_ui.screen_donelist_label_focus2,
         guider_ui.screen_donelist_label_focus3,
         guider_ui.screen_donelist_label_focus4,
     };
     lv_obj_t *lines[] = {
         guider_ui.screen_donelist_line_1,
         guider_ui.screen_donelist_line_2,
         guider_ui.screen_donelist_line_3,
         guider_ui.screen_donelist_line_4,
     };
     static const lv_coord_t container_y[] = {32, 59, 85, 112};
 
     for (uint8_t i = 0; i < 4; i++) {
         lv_obj_t *row = lv_obj_create(guider_ui.screen_donelist);
         s_donelist_rows[i] = row;
         lv_obj_set_pos(row, 0, container_y[i]);
         lv_obj_set_size(row, 320, 27);
         lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);
         lv_obj_clear_flag(row, LV_OBJ_FLAG_CLICKABLE);
         lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP,
                                 LV_PART_MAIN | LV_STATE_DEFAULT);
         lv_obj_set_style_border_width(row, 0,
                                       LV_PART_MAIN | LV_STATE_DEFAULT);
         lv_obj_set_style_pad_all(row, 0,
                                  LV_PART_MAIN | LV_STATE_DEFAULT);
 
         lv_obj_set_parent(names[i], row);
         lv_obj_set_parent(dates[i], row);
         lv_obj_set_parent(focus[i], row);
         lv_obj_set_parent(lines[i], row);
     }
 
     donelist_effect_restore_layout();
 }
 
 static void donelist_effect_bind_hooks(void)
 {
     lv_obj_t *scr = guider_ui.screen_donelist;
     if (scr == NULL || !lv_obj_is_valid(scr) ||
         s_donelist_effect_screen == scr) {
         return;
     }
     s_donelist_effect_screen = scr;
     /*
      * 数据标签保持固定坐标和样式，仅动画整行透明容器。
      * 避免动画期间反复刷新名称文本，也保证日期始终处于标准位置。
      */
     donelist_effect_create_rows();
     donelist_effect_prepare();
 
     lv_obj_add_event_cb(scr, donelist_effect_event_cb,
                         LV_EVENT_SCREEN_LOADED, NULL);
     lv_obj_add_event_cb(scr, donelist_effect_event_cb,
                         LV_EVENT_SCREEN_UNLOADED, NULL);
     lv_obj_add_event_cb(guider_ui.screen_donelist_btn_lastpage,
                         donelist_effect_event_cb, LV_EVENT_CLICKED, NULL);
     lv_obj_add_event_cb(guider_ui.screen_donelist_btn_nxtpage,
                         donelist_effect_event_cb, LV_EVENT_CLICKED, NULL);
     lv_obj_add_event_cb(guider_ui.screen_donelist_btn_save,
                         donelist_effect_event_cb, LV_EVENT_CLICKED, NULL);
 }
 
 //buttion add function screen_donelist
 void screen_donelist_buttion_add(){
     if(g == NULL) {
         g = lv_group_create();
     } else {
         lv_group_remove_all_objs(g); 
     }
     //setting default group
     lv_group_set_default(g);
     //indev with group
     lv_indev_t * indev = NULL;
     while ((indev = lv_indev_get_next(indev))) {
         if (lv_indev_get_type(indev) == LV_INDEV_TYPE_KEYPAD) {
             lv_indev_set_group(indev, g);
         }
     }
     lv_group_add_obj(g, guider_ui.screen_donelist_btn_lastpage);  
     lv_group_add_obj(g, guider_ui.screen_donelist_btn_nxtpage); 
     lv_group_add_obj(g, guider_ui.screen_donelist_btn_back); 
     lv_group_add_obj(g, guider_ui.screen_donelist_btn_save);
     clear_edge_box(guider_ui.screen_donelist_btn_lastpage);
     clear_edge_box(guider_ui.screen_donelist_btn_nxtpage);
     clear_edge_box(guider_ui.screen_donelist_btn_back);
     clear_edge_box(guider_ui.screen_donelist_btn_save);
     donelist_effect_bind_hooks();
     ui_font_bind(guider_ui.screen_donelist);
 }
 //buttion add function screen_todooback
 void screen_todoback_buttion_add(){
     if(g == NULL) {
         g = lv_group_create();
     } else {
         lv_group_remove_all_objs(g); 
     }
     //setting default group
     lv_group_set_default(g);
     //indev with group
     lv_indev_t * indev = NULL;
     while ((indev = lv_indev_get_next(indev))) {
         if (lv_indev_get_type(indev) == LV_INDEV_TYPE_KEYPAD) {
             lv_indev_set_group(indev, g);
         }
     }
     lv_group_add_obj(g, guider_ui.screen_todoback_btn_recover);  
     lv_group_add_obj(g, guider_ui.screen_todoback_btn_cancel); 
     clear_edge_box(guider_ui.screen_todoback_btn_recover);
     clear_edge_box(guider_ui.screen_todoback_btn_cancel);
     ui_font_bind(guider_ui.screen_todoback);
 }
 
 
 //buttion add function screen_answer
 void screen_answer_buttion_add(){
     if(g == NULL) {
         g = lv_group_create();
     } else {
         lv_group_remove_all_objs(g); 
     }
     //setting default group
     lv_group_set_default(g);
     //indev with group
     lv_indev_t * indev = NULL;
     while ((indev = lv_indev_get_next(indev))) {
         if (lv_indev_get_type(indev) == LV_INDEV_TYPE_KEYPAD) {
             lv_indev_set_group(indev, g);
         }
     }
     lv_group_add_obj(g, guider_ui.screen_answer_btn_start); 
     clear_focus_box(guider_ui.screen_answer_btn_start);
 }
 
 //buttion add function screen_getanswer
 void screen_getanswer_buttion_add(){
     if(g == NULL) {
         g = lv_group_create();
     } else {
         lv_group_remove_all_objs(g); 
     }
     //setting default group
     lv_group_set_default(g);
     //indev with group
     lv_indev_t * indev = NULL;
     while ((indev = lv_indev_get_next(indev))) {
         if (lv_indev_get_type(indev) == LV_INDEV_TYPE_KEYPAD) {
             lv_indev_set_group(indev, g);
         }
     }
     lv_group_add_obj(g, guider_ui.screen_getanswer_btn_answer); 
     clear_focus_box(guider_ui.screen_getanswer_btn_answer);
     answer_loading_led_stop_req = 1;
     ui_font_bind(guider_ui.screen_getanswer);
 }
 
 //buttion add function screen_picture
 void screen_picture_buttion_add(){
     if(g == NULL) {
         g = lv_group_create();
     } else {
         lv_group_remove_all_objs(g); 
     }
     //setting default group
     lv_group_set_default(g);
     //indev with group
     lv_indev_t * indev = NULL;
     while ((indev = lv_indev_get_next(indev))) {
         if (lv_indev_get_type(indev) == LV_INDEV_TYPE_KEYPAD) {
             lv_indev_set_group(indev, g);
         }
     }
     lv_group_add_obj(g, guider_ui.screen_picture_img_picture); 
     clear_focus_box(guider_ui.screen_picture_img_picture);
 }
 
 /* GUI Guider 模拟器用弱符号占位；ESP-IDF 固件由 main/picture_bridge.c 强符号覆盖 */
 __attribute__((weak)) void picture_bridge_screen_on_load(void)
 {
     lv_img_set_src(guider_ui.screen_picture_img_picture, &_word_BG1_alpha_320x170);
 }
 
 void screen_picture_custom_init(void)
 {
     screen_picture_buttion_add();
     picture_bridge_screen_on_load();
     lv_obj_invalidate(guider_ui.screen_picture_img_picture);
 }
 
 static void screen_keypad_group_reset(void)
 {
     if(g == NULL) {
         g = lv_group_create();
     } else {
         lv_group_remove_all_objs(g);
         lv_group_set_edge_cb(g, NULL);
         lv_group_set_wrap(g, true);
     }
     lv_group_set_default(g);
     lv_indev_t * indev = NULL;
     while ((indev = lv_indev_get_next(indev))) {
         if (lv_indev_get_type(indev) == LV_INDEV_TYPE_KEYPAD) {
             lv_indev_set_group(indev, g);
         }
     }
 }
 
 static void screen_load_main_from(bool *old_scr_del)
 {
     ui_load_scr_animation(&guider_ui, &guider_ui.screen_main,
                           guider_ui.screen_main_del, old_scr_del,
                           setup_scr_screen_main, LV_SCR_LOAD_ANIM_FADE_ON,
                           100, 100, true, true);
 }
 
 /* GUI Guider 预览用弱符号；ESP32 固件由 main/gash_machine.c 强符号覆盖。 */
 __attribute__((weak)) void screen_gash_buttion_add(void)
 {
     screen_keypad_group_reset();
     if (guider_ui.screen_gash_btn_addtodo != NULL &&
         lv_obj_is_valid(guider_ui.screen_gash_btn_addtodo)) {
         lv_group_add_obj(g, guider_ui.screen_gash_btn_addtodo);
         clear_focus_box(guider_ui.screen_gash_btn_addtodo);
         group_focus_obj(guider_ui.screen_gash_btn_addtodo);
     }
     if (guider_ui.screen_gash_btn_again != NULL &&
         lv_obj_is_valid(guider_ui.screen_gash_btn_again)) {
         lv_group_add_obj(g, guider_ui.screen_gash_btn_again);
         clear_focus_box(guider_ui.screen_gash_btn_again);
     }
 }

 __attribute__((weak)) void screen_gash_custom_init(void)
 {
     if (guider_ui.screen_gash == NULL ||
         !lv_obj_is_valid(guider_ui.screen_gash)) {
         return;
     }
     if (guider_ui.screen_gash_img_claw2 != NULL &&
         lv_obj_is_valid(guider_ui.screen_gash_img_claw2)) {
         lv_obj_add_flag(guider_ui.screen_gash_img_claw2, LV_OBJ_FLAG_HIDDEN);
     }
     if (guider_ui.screen_gash_img_claw3 != NULL &&
         lv_obj_is_valid(guider_ui.screen_gash_img_claw3)) {
         lv_obj_add_flag(guider_ui.screen_gash_img_claw3, LV_OBJ_FLAG_HIDDEN);
     }
     if (guider_ui.screen_gash_img_ball1 != NULL &&
         lv_obj_is_valid(guider_ui.screen_gash_img_ball1)) {
         lv_obj_add_flag(guider_ui.screen_gash_img_ball1, LV_OBJ_FLAG_HIDDEN);
     }
     if (guider_ui.screen_gash_img_ball2 != NULL &&
         lv_obj_is_valid(guider_ui.screen_gash_img_ball2)) {
         lv_obj_add_flag(guider_ui.screen_gash_img_ball2, LV_OBJ_FLAG_HIDDEN);
     }
     if (guider_ui.screen_gash_img_ball3 != NULL &&
         lv_obj_is_valid(guider_ui.screen_gash_img_ball3)) {
         lv_obj_add_flag(guider_ui.screen_gash_img_ball3, LV_OBJ_FLAG_HIDDEN);
     }
     if (guider_ui.screen_gash_img_ball4 != NULL &&
         lv_obj_is_valid(guider_ui.screen_gash_img_ball4)) {
         lv_obj_add_flag(guider_ui.screen_gash_img_ball4, LV_OBJ_FLAG_HIDDEN);
     }
     if (guider_ui.screen_gash_img_randontask != NULL &&
         lv_obj_is_valid(guider_ui.screen_gash_img_randontask)) {
         lv_obj_add_flag(guider_ui.screen_gash_img_randontask, LV_OBJ_FLAG_HIDDEN);
     }
     if (guider_ui.screen_gash_btn_addtodo != NULL &&
         lv_obj_is_valid(guider_ui.screen_gash_btn_addtodo)) {
         lv_obj_add_flag(guider_ui.screen_gash_btn_addtodo, LV_OBJ_FLAG_HIDDEN);
     }
     if (guider_ui.screen_gash_btn_again != NULL &&
         lv_obj_is_valid(guider_ui.screen_gash_btn_again)) {
         lv_obj_add_flag(guider_ui.screen_gash_btn_again, LV_OBJ_FLAG_HIDDEN);
     }
     screen_gash_buttion_add();
 }

 /* GUI Guider 预览用弱符号；ESP32 固件由 main/keep_machine.c 强符号覆盖。 */
 __attribute__((weak)) void screen_keep_buttion_add(void)
 {
     screen_keypad_group_reset();
     if (guider_ui.screen_keep == NULL ||
         !lv_obj_is_valid(guider_ui.screen_keep)) {
         return;
     }
     lv_obj_add_flag(guider_ui.screen_keep, LV_OBJ_FLAG_CLICKABLE);
     lv_obj_add_flag(guider_ui.screen_keep, LV_OBJ_FLAG_CLICK_FOCUSABLE);
     lv_group_add_obj(g, guider_ui.screen_keep);
     clear_focus_box(guider_ui.screen_keep);
     group_focus_obj(guider_ui.screen_keep);
 }

 __attribute__((weak)) void screen_keep_custom_init(void)
 {
     lv_obj_t *hide[] = {
         guider_ui.screen_keep_img_lv2,
         guider_ui.screen_keep_img_lv3,
         guider_ui.screen_keep_img_lv4,
         guider_ui.screen_keep_img_lv5,
         guider_ui.screen_keep_img_lv6,
         guider_ui.screen_keep_img_lv7,
         guider_ui.screen_keep_img_lv8,
         guider_ui.screen_keep_img_lv9,
         guider_ui.screen_keep_img_lv10,
         guider_ui.screen_keep_img_lv1_wither,
         guider_ui.screen_keep_img_lv2_wither,
         guider_ui.screen_keep_img_lv3_wither,
         guider_ui.screen_keep_img_lv4_wither,
         guider_ui.screen_keep_img_lv5_wither,
         guider_ui.screen_keep_img_lv6_wither,
         guider_ui.screen_keep_img_lv7_wither,
         guider_ui.screen_keep_img_lv8_wither,
         guider_ui.screen_keep_img_lv9_wither,
         guider_ui.screen_keep_img_lv10_wither,
     };
     if (guider_ui.screen_keep == NULL ||
         !lv_obj_is_valid(guider_ui.screen_keep)) {
         return;
     }
     for (size_t i = 0; i < sizeof(hide) / sizeof(hide[0]); i++) {
         if (hide[i] != NULL && lv_obj_is_valid(hide[i])) {
             lv_obj_add_flag(hide[i], LV_OBJ_FLAG_HIDDEN);
         }
     }
     if (guider_ui.screen_keep_img_lv1 != NULL &&
         lv_obj_is_valid(guider_ui.screen_keep_img_lv1)) {
         lv_obj_clear_flag(guider_ui.screen_keep_img_lv1, LV_OBJ_FLAG_HIDDEN);
     }
     screen_keep_buttion_add();
 }

 /* GUI Guider 预览用弱符号；ESP32 固件由 main/book_machine.c 强符号覆盖。 */
 __attribute__((weak)) void screen_book_buttion_add(void)
 {
     screen_keypad_group_reset();
     if (guider_ui.screen_book == NULL ||
         !lv_obj_is_valid(guider_ui.screen_book)) {
         return;
     }
     lv_obj_add_flag(guider_ui.screen_book, LV_OBJ_FLAG_CLICKABLE);
     lv_obj_add_flag(guider_ui.screen_book, LV_OBJ_FLAG_CLICK_FOCUSABLE);
     lv_group_add_obj(g, guider_ui.screen_book);
     clear_focus_box(guider_ui.screen_book);
     group_focus_obj(guider_ui.screen_book);
 }

 __attribute__((weak)) void screen_book_custom_init(void)
 {
     if (guider_ui.screen_book == NULL ||
         !lv_obj_is_valid(guider_ui.screen_book)) {
         return;
     }
     if (guider_ui.screen_book_img_content_bg != NULL &&
         lv_obj_is_valid(guider_ui.screen_book_img_content_bg)) {
         lv_obj_add_flag(guider_ui.screen_book_img_content_bg, LV_OBJ_FLAG_HIDDEN);
     }
     if (guider_ui.screen_book_list_part != NULL &&
         lv_obj_is_valid(guider_ui.screen_book_list_part)) {
         lv_obj_add_flag(guider_ui.screen_book_list_part, LV_OBJ_FLAG_HIDDEN);
     }
     if (guider_ui.screen_book_btn_back != NULL &&
         lv_obj_is_valid(guider_ui.screen_book_btn_back)) {
         lv_obj_add_flag(guider_ui.screen_book_btn_back, LV_OBJ_FLAG_HIDDEN);
     }
     screen_book_buttion_add();
 }

 static bool s_game_events_bound;

 static void screen_game_on_delete(lv_event_t *e)
 {
     if (lv_event_get_code(e) != LV_EVENT_DELETE) {
         return;
     }
     if (lv_event_get_target(e) == guider_ui.screen_game) {
         guider_ui.screen_game = NULL;
     }
     s_game_events_bound = false;
     lv_async_call_cancel(screen_main_game_init_async, NULL);
 }

 static void screen_game_back_clicked(lv_event_t *e)
 {
     if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
         return;
     }
     if (lv_scr_act() != guider_ui.screen_game) {
         return;
     }
     screen_load_main_from(&guider_ui.screen_game_del);
 }
 
 __attribute__((weak)) void screen_game_buttion_add(void)
 {
     if (guider_ui.screen_game == NULL ||
         !lv_obj_is_valid(guider_ui.screen_game) ||
         lv_scr_act() != guider_ui.screen_game) {
         return;
     }
     screen_keypad_group_reset();
     lv_obj_add_flag(guider_ui.screen_game, LV_OBJ_FLAG_CLICKABLE);
     lv_obj_add_flag(guider_ui.screen_game, LV_OBJ_FLAG_CLICK_FOCUSABLE);
     if (!s_game_events_bound) {
         lv_obj_add_event_cb(guider_ui.screen_game, screen_game_on_delete,
                             LV_EVENT_DELETE, NULL);
         lv_obj_add_event_cb(guider_ui.screen_game, screen_game_back_clicked,
                             LV_EVENT_CLICKED, NULL);
         s_game_events_bound = true;
     }
     lv_group_add_obj(g, guider_ui.screen_game);
     clear_focus_box(guider_ui.screen_game);
     group_focus_obj(guider_ui.screen_game);
 }
 
 /* Guider-only fallback. Firmware gameplay lives in main/game_machine.c. */
 __attribute__((weak)) void screen_game_custom_init(void)
 {
     lv_obj_t *label;
     if (guider_ui.screen_game == NULL ||
         !lv_obj_is_valid(guider_ui.screen_game) ||
         lv_scr_act() != guider_ui.screen_game || s_game_events_bound) {
         return;
     }
     lv_obj_set_style_bg_opa(guider_ui.screen_game, 255,
                             LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_bg_color(guider_ui.screen_game, lv_color_hex(0x0e1b28),
                              LV_PART_MAIN | LV_STATE_DEFAULT);
     label = lv_label_create(guider_ui.screen_game);
     lv_label_set_text(label, "Dino Run\nClick to return");
     lv_obj_set_style_text_color(label, lv_color_hex(0xffffff),
                                LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_set_style_text_font(label, &lv_font_montserratMedium_16,
                               LV_PART_MAIN | LV_STATE_DEFAULT);
     lv_obj_center(label);
     screen_game_buttion_add();
 }
 
 static void screen_main_extra_scr_clicked(lv_event_t *e)
 {
     if (lv_event_get_code(e) != LV_EVENT_PRESSED) {
         return;
     }
     if (lv_event_get_target(e) == guider_ui.screen_main_btn_gashapon) {
         lv_async_call(screen_main_gash_init_async, NULL);
     } else if (lv_event_get_target(e) == guider_ui.screen_main_btn_ai) {
         lv_async_call(screen_main_keep_init_async, NULL);
     } else if (lv_event_get_target(e) == guider_ui.screen_main_btn_game) {
         lv_async_call(screen_main_game_init_async, NULL);
     } else if (lv_event_get_target(e) == guider_ui.screen_main_btn_word) {
         lv_async_call(screen_main_book_init_async, NULL);
     }
 }
 
 void custom_init(lv_ui *ui)
 {
 
 }
