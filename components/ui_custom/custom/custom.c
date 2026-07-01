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
#include "widgets_init.h"
#include <stdlib.h>

//lv group define
lv_group_t *g = NULL;
//todolist variables (debug seed data for GUI Guider preview)
todo_list_t ui_todo_list = {
    .count = 8,
    .items = {
        {.name = "Morning standup",   .duration_min = 15, .done = 1, .finish_date = "2026-05-09"},
        {.name = "Review PR #128",    .duration_min = 25, .done = 1, .finish_date = "2026-05-09"},
        {.name = "Fix WiFi reconnect",.duration_min = 45, .done = 0, .finish_date = ""},
        {.name = "LVGL animation tune", .duration_min = 30, .done = 0, .finish_date = ""},
        {.name = "Write test notes",  .duration_min = 20, .done = 0, .finish_date = ""},
        {.name = "Refactor timer flow", .duration_min = 35, .done = 0, .finish_date = ""},
        {.name = "Sync NVS defaults", .duration_min = 18, .done = 0, .finish_date = ""},
        {.name = "Polish main screen",.duration_min = 40, .done = 0, .finish_date = ""},
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
bool countdown_timeout_pending = 0;
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
/* AI 结果 UI 缓存，由 main.c 负责从 g_ai_* 同步进来 */
ui_ai_result_t g_ui_daily_result  = {0};
ui_ai_result_t g_ui_weekly_result = {0};

bool answer_book_rand_req = 0;
unsigned int answer_book_rand = 0;

static const char * const answer_book_answers[] = {
    "先别急，答案会自己浮现",
    "可以试试，但别用力过猛",
    "今天适合迈出第一步",
    "换个角度看，会更清楚",
    "等等看，时机还差一点",
    "答案在你忽略的细节里",
    "把问题写下来，它会变小",
    "去做，边做边修正",
    "先完成，再完美",
    "这件事值得认真对待",
    "问问同伴，会有新思路",
    "不要高估今天，也别低估长期",
    "休息一下，效率会回来",
    "它没有你想的那么难",
    "这次选择简单一点",
    "保留余地，是更聪明的决定",
    "先把最小版本跑通",
    "今晚不适合硬扛",
    "你已经知道答案了",
    "把复杂问题拆成三步",
    "相信直觉，但检查证据",
    "再等一个信号",
    "别被情绪带着走",
    "认真可以，焦虑不必",
    "这会是一个不错的开始",
    "答案偏向肯定",
    "答案偏向否定",
    "现在还不是最佳时机",
    "大胆一点，问题不大",
    "先和重要的人确认一下",
    "今天适合收尾，不适合开新坑",
    "把手机放下，答案会清楚些",
    "去启真湖边走走再决定",
    "图书馆会给你一点方向",
    "先吃饭，别空腹做决定",
    "这件事需要睡醒后再判断",
    "保持节奏，不用冲刺",
    "少想一点，多验证一点",
    "它会比预期顺利",
    "留给明天也可以",
    "先处理最确定的部分",
    "不要为了快而绕远路",
    "接受不确定，然后继续",
    "这不是终点，只是一次迭代",
    "问问自己真正想要什么",
    "答案藏在长期收益里",
    "可以拒绝，不必内耗",
    "选让你更平静的那个",
    "先把桌面整理干净",
    "把仿真跑起来再说",
    "今天适合做减法",
    "慢一点，结果会更稳",
    "你需要的是开始，不是答案",
    "别急着证明自己",
    "这件事值得再坚持一下",
    "方向没错，只是需要调整",
    "让数据说话",
    "先别承诺太满",
    "好消息会晚一点到",
    "给自己十分钟缓冲",
    "选择更可持续的方案",
    "可以，但要设边界",
    "暂时不要",
    "试试看",
    "先把风险列出来",
    "答案在下一次尝试里",
    "别把小事想成大事",
    "这次先相信自己",
    "慢慢来，比较快",
    "不要急着给结论",
    "先问清楚问题本身",
    "这条路可以继续走",
    "换个环境会有答案",
    "少一点完美主义",
    "从最简单的地方开始",
    "这不是坏事",
    "今天适合整理思路",
    "把它交给时间一点点",
    "答案会偏向温柔",
    "先保护好自己的节奏",
    "不要同时做太多决定",
    "它需要一个备选方案",
    "先和现实对齐",
    "不要忽略身体的提醒",
    "可以先小范围试验",
    "这件事不用马上答复",
    "把期待调低一点",
    "你需要更多信息",
    "这个选择会带来成长",
    "先停下，再出发",
    "保持简单就好",
    "答案比你想的朴素",
    "别让拖延伪装成思考",
    "这次可以主动一点",
    "别急，先喝口水",
    "今晚适合早点收工",
    "选择更清醒的那个",
    "让明天的你再看一次",
    "它值得一个认真开始",
    "先把边界说清楚"
};

void answer_book_show_random_answer(void)
{
    const uint32_t count = (uint32_t)(sizeof(answer_book_answers) / sizeof(answer_book_answers[0]));
    uint32_t index = answer_book_rand % count;
    lv_label_set_text(guider_ui.screen_getanswer_btn_answer_label, answer_book_answers[index]);
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

void pomodoro_timer_stop(void)
{
    if (pomodoro_timer != NULL) {
        lv_timer_del(pomodoro_timer);
        pomodoro_timer = NULL;
    }
    pomodoro_pause_icon_refresh();
}

void pomodoro_pause_icon_refresh(void)
{
    lv_obj_t *img = guider_ui.screen_pomodoro_img_pause;
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
    } else {
        lv_obj_add_flag(img, LV_OBJ_FLAG_HIDDEN);
    }
}

static void countdown_cb(lv_timer_t *t){
    if (!pomodoro_screen_is_alive()) {
        pomodoro_timer_stop();
        return;
    }
    if(timer_all_seconds == 0){
        lv_timer_del(t);
        pomodoro_timer = NULL;
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
        countdown_timeout_pending = 1;
        if (focus_session_active && pomodoro_countdown) {
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
}

void countdown_start(int16_t minutes){
    pomodoro_timer_stop();
    timer_all_seconds = minutes * 60;
    if (focus_session_active) {
        focus_session_minutes = minutes;
    }
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
    pomodoro_timer_stop();
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

    lv_label_set_text_fmt(guider_ui.screen_wificfg_label_status, "Connect to %s", txt);

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
/* GUI Guider 模拟器用空实现；ESP32 固件由 main/wordcard_bridge.c 覆盖 */
__attribute__((weak)) void wordcard_on_menu_enter(void)
{
}

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

void brightness_apply(void)
{
    brightness_apply_impl(brightness_level);
}

void brightness_update_icon(void)
{
    static const lv_opa_t icon_opa[3] = { 90, 170, 255 };

    if (guider_ui.screen_setting_img_brightness == NULL) {
        return;
    }
    lv_obj_set_style_img_opa(guider_ui.screen_setting_img_brightness,
                             icon_opa[brightness_level % 3],
                             LV_PART_MAIN | LV_STATE_DEFAULT);
}

void brightness_cycle(void)
{
    brightness_level = (brightness_level + 1U) % 3U;
    brightness_apply();
    brightness_update_icon();
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
    lv_label_set_text(guider_ui.screen_wificfg_label_status, "Connection Status");
    lv_label_set_text(guider_ui.screen_wificfg_label_status2, "Connected");

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
    lv_label_set_text(guider_ui.screen_wificfg_label_status, "Connection Status");
    lv_label_set_text(guider_ui.screen_wificfg_label_status2, "Disconnected");

    // 4. Update Label Color (Status2)
    // Assuming the light green color in the image is roughly #b7f0b8
    lv_obj_set_style_text_color(guider_ui.screen_wificfg_label_status2, lv_color_hex(0x274a6e), 0);

    // 5. Change Image Height
    ui_animation(guider_ui.screen_wificfg_img_statusbg, 200, 0, lv_obj_get_height(guider_ui.screen_wificfg_img_statusbg), 125, &lv_anim_path_linear, 1, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_height, NULL, NULL, NULL);
}

/* 进入 screen_ai 时显示 NVS 已存储的 AI 结果
 * 优先显示每日结果，若无有效数据则尝试每周结果 */
void screen_ai_apply_result(const ui_ai_result_t *r)
{
    static char s_cat1[32];
    static char s_cat2[32];
    static char s_cat3[32];
    static char s_summary[256];
    static char s_quote[128];
    static char s_pct1[8];
    static char s_pct2[8];
    static char s_pct3[8];

    if (r == NULL || !r->valid) {
        return;
    }

    strncpy(s_cat1, r->cat1_name, sizeof(s_cat1) - 1);
    s_cat1[sizeof(s_cat1) - 1] = '\0';
    strncpy(s_cat2, r->cat2_name, sizeof(s_cat2) - 1);
    s_cat2[sizeof(s_cat2) - 1] = '\0';
    strncpy(s_cat3, r->cat3_name, sizeof(s_cat3) - 1);
    s_cat3[sizeof(s_cat3) - 1] = '\0';
    strncpy(s_summary, r->summary, sizeof(s_summary) - 1);
    s_summary[sizeof(s_summary) - 1] = '\0';
    strncpy(s_quote, r->quote, sizeof(s_quote) - 1);
    s_quote[sizeof(s_quote) - 1] = '\0';
    snprintf(s_pct1, sizeof(s_pct1), "%d%%", r->cat1_percent);
    snprintf(s_pct2, sizeof(s_pct2), "%d%%", r->cat2_percent);
    snprintf(s_pct3, sizeof(s_pct3), "%d%%", r->cat3_percent);

    lv_label_set_text_static(guider_ui.screen_ai_label_cat1, s_cat1);
    lv_label_set_text_static(guider_ui.screen_ai_label_cat2, s_cat2);
    lv_label_set_text_static(guider_ui.screen_ai_label_cat3, s_cat3);
    lv_label_set_text_static(guider_ui.screen_ai_label_cat1_num, s_pct1);
    lv_label_set_text_static(guider_ui.screen_ai_label_cat2_num, s_pct2);
    lv_label_set_text_static(guider_ui.screen_ai_label_cat3_num, s_pct3);
    lv_arc_set_value(guider_ui.screen_ai_arc_1, r->cat1_percent);
    lv_arc_set_value(guider_ui.screen_ai_arc_2, r->cat2_percent);
    lv_arc_set_value(guider_ui.screen_ai_arc_3, r->cat3_percent);
    lv_label_set_text_static(guider_ui.screen_ai_label_sum, s_summary);
    lv_label_set_text_static(guider_ui.screen_ai_label_quote, s_quote);
}

void screen_ai_load_nvs_data(bool day_or_week)
{
    ui_ai_result_t *r = NULL;
    if (day_or_week == 0) {
        r = &g_ui_daily_result;
    } else if (day_or_week == 1) {
        r = &g_ui_weekly_result;
    } else {
        return;
    }
    screen_ai_apply_result(r);
}

#define FOCUS_LEVEL_COUNT 10

void screen_ai_update_focus_level(void)
{
    lv_obj_t *level_imgs[FOCUS_LEVEL_COUNT];

    level_imgs[0] = guider_ui.screen_ai_img_v1;
    level_imgs[1] = guider_ui.screen_ai_img_v2;
    level_imgs[2] = guider_ui.screen_ai_img_v3;
    level_imgs[3] = guider_ui.screen_ai_img_v4;
    level_imgs[4] = guider_ui.screen_ai_img_v5;
    level_imgs[5] = guider_ui.screen_ai_img_v6;
    level_imgs[6] = guider_ui.screen_ai_img_v7;
    level_imgs[7] = guider_ui.screen_ai_img_v8;
    level_imgs[8] = guider_ui.screen_ai_img_v9;
    level_imgs[9] = guider_ui.screen_ai_img_v10;

    int focus_level = 1;
    int remain_focus_time = all_focus_time;
    int max_level_time = focus_level_hour[0] * 60;
    bool maxed = true;

    for (int i = 0; i < FOCUS_LEVEL_COUNT; i++) {
        int threshold_min = focus_level_hour[i] * 60;
        if (all_focus_time < threshold_min) {
            focus_level = i + 1;
            max_focus_time = threshold_min;
            maxed = false;
            if (i == 0) {
                remain_focus_time = all_focus_time;
                max_level_time = threshold_min;
            } else {
                int prev_min = focus_level_hour[i - 1] * 60;
                remain_focus_time = all_focus_time - prev_min;
                max_level_time = threshold_min - prev_min;
            }
            break;
        }
    }

    if (maxed) {
        focus_level = FOCUS_LEVEL_COUNT;
        max_focus_time = focus_level_hour[FOCUS_LEVEL_COUNT - 1] * 60;
        int prev_min = focus_level_hour[FOCUS_LEVEL_COUNT - 2] * 60;
        remain_focus_time = all_focus_time - prev_min;
        max_level_time = max_focus_time - prev_min;
    }

    int focus_percent = 100;
    if (max_level_time > 0 && focus_level < FOCUS_LEVEL_COUNT) {
        focus_percent = remain_focus_time * 100 / max_level_time;
        if (focus_percent > 100) {
            focus_percent = 100;
        }
    }

    if (guider_ui.screen_ai_bar_focus != NULL &&
        lv_obj_is_valid(guider_ui.screen_ai_bar_focus)) {
        lv_bar_set_value(guider_ui.screen_ai_bar_focus, focus_percent, LV_ANIM_ON);
    }
    if (guider_ui.screen_ai_label_duration != NULL &&
        lv_obj_is_valid(guider_ui.screen_ai_label_duration)) {
        lv_label_set_text_fmt(guider_ui.screen_ai_label_duration,
                              "%d/%d", all_focus_time, max_focus_time);
    }

    for (int i = 0; i < FOCUS_LEVEL_COUNT; i++) {
        if (level_imgs[i] == NULL || !lv_obj_is_valid(level_imgs[i])) {
            continue;
        }
        if (i + 1 == focus_level) {
            lv_obj_clear_flag(level_imgs[i], LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(level_imgs[i], LV_OBJ_FLAG_HIDDEN);
        }
    }
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
        char *p_name = strtok(line, ",");
        char *p_date = strtok(NULL, ",");
        char *p_dur  = strtok(NULL, ",");
        if (!p_name || !p_date) continue;
        strncpy(ui_done_list.items[ui_done_list.count].name, p_name,
                sizeof(ui_done_list.items[0].name) - 1);
        strncpy(ui_done_list.items[ui_done_list.count].date, p_date,
                sizeof(ui_done_list.items[0].date) - 1);
        ui_done_list.items[ui_done_list.count].duration_min =
                (uint16_t)(p_dur ? atoi(p_dur) : 0);
        ui_done_list.count++;
    }
    fclose(f);
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

}
//buttion add function screen_main
void screen_main_buttion_add(){
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
    lv_group_add_obj(g, guider_ui.screen_main_btn_todolist);  
    lv_group_add_obj(g, guider_ui.screen_main_btn_done); 
    lv_group_add_obj(g, guider_ui.screen_main_btn_word); 
    lv_group_add_obj(g, guider_ui.screen_main_btn_timer);
    lv_group_add_obj(g, guider_ui.screen_main_btn_ai); 
    lv_group_add_obj(g, guider_ui.screen_main_btn_fun);
    lv_group_add_obj(g, guider_ui.screen_main_btn_photo);
    lv_group_add_obj(g, guider_ui.screen_main_btn_setting);
    lv_group_add_obj(g, guider_ui.screen_main_btn_realtime);
    //clear focus box
    clear_focus_box(guider_ui.screen_main_btn_todolist);
    clear_focus_box(guider_ui.screen_main_btn_done);
    clear_focus_box(guider_ui.screen_main_btn_word);
    clear_focus_box(guider_ui.screen_main_btn_timer);
    clear_focus_box(guider_ui.screen_main_btn_ai);
    clear_focus_box(guider_ui.screen_main_btn_fun);
    clear_focus_box(guider_ui.screen_main_btn_photo);
    clear_focus_box(guider_ui.screen_main_btn_setting);
    clear_focus_box(guider_ui.screen_main_btn_realtime);
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

}

static void pomodoro_pause_icon_after_guider(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED || code == LV_EVENT_SCREEN_LOADED) {
        pomodoro_pause_icon_refresh();
    }
}

static void pomodoro_pause_icon_bind_hooks(void)
{
    static lv_obj_t *bound_screen;

    lv_obj_t *scr = guider_ui.screen_pomodoro;
    if (scr == NULL || !lv_obj_is_valid(scr)) {
        return;
    }
    if (bound_screen == scr) {
        return;
    }
    bound_screen = scr;

    lv_obj_add_event_cb(scr, pomodoro_pause_icon_after_guider, LV_EVENT_SCREEN_LOADED, NULL);

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
    }
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
}
//buttion add function screen_wordmeun
void screen_wordmenu_buttion_add(){
    wordcard_on_menu_enter();
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
    
    lv_group_add_obj(g, guider_ui.screen_wordmeun_btn_learn); 
    lv_group_add_obj(g, guider_ui.screen_wordmeun_btn_review); 
    lv_group_add_obj(g, guider_ui.screen_wordmeun_btn_back); 
    clear_edge_box(guider_ui.screen_wordmeun_btn_learn);
    clear_edge_box(guider_ui.screen_wordmeun_btn_review);
    clear_edge_box(guider_ui.screen_wordmeun_btn_back);
    
}

//buttion add function screen_wordcard
static void wordcard_hide_dog_anims_on(lv_ui *ui)
{
    if(ui->screen_wordcard_dog_cuo) {
        lv_anim_del(ui->screen_wordcard_dog_cuo, NULL);
        lv_obj_add_flag(ui->screen_wordcard_dog_cuo, LV_OBJ_FLAG_HIDDEN);
    }
    if(ui->screen_wordcard_dog_dui) {
        lv_anim_del(ui->screen_wordcard_dog_dui, NULL);
        lv_obj_add_flag(ui->screen_wordcard_dog_dui, LV_OBJ_FLAG_HIDDEN);
    }
}

void wordcard_hide_dog_anims(void)
{
    wordcard_hide_dog_anims_on(&guider_ui);
}

void wordcard_show_dog_dui(void)
{
    wordcard_hide_dog_anims();
    lv_animimg_start(guider_ui.screen_wordcard_dog_dui);
    lv_obj_clear_flag(guider_ui.screen_wordcard_dog_dui, LV_OBJ_FLAG_HIDDEN);
}

void wordcard_show_dog_cuo(void)
{
    wordcard_hide_dog_anims();
    lv_animimg_start(guider_ui.screen_wordcard_dog_cuo);
    lv_obj_clear_flag(guider_ui.screen_wordcard_dog_cuo, LV_OBJ_FLAG_HIDDEN);
}

void wordcard_screen_custom_init(lv_ui *ui)
{
    wordcard_hide_dog_anims_on(ui);
}

void screen_wordcard_buttion_add(){
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
    lv_group_add_obj(g, guider_ui.screen_wordcard_btn_dui);  
    lv_group_add_obj(g, guider_ui.screen_wordcard_btn_cuo);  
    lv_group_add_obj(g, guider_ui.screen_wordcard_btn_back);
    clear_edge_box(guider_ui.screen_wordcard_btn_dui);
    clear_edge_box(guider_ui.screen_wordcard_btn_cuo);
    clear_edge_box(guider_ui.screen_wordcard_btn_back);

    wordcard_session_start = 1;
    wordcard_change = 1;
}

//buttion add function screen_reviewcard
void screen_reviewcard_buttion_add(){
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
    lv_group_add_obj(g, guider_ui.screen_reviewcard_btn_dui);   
    lv_group_add_obj(g, guider_ui.screen_reviewcard_btn_back); 
    clear_edge_box(guider_ui.screen_reviewcard_btn_dui);
    clear_edge_box(guider_ui.screen_reviewcard_btn_back);

    reviewcard_session_start = 1;
    reviewcard_change = 1;
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
    if (ble_success) {
        lv_obj_clear_flag(img_on, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(img_dis, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(img_on, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(img_dis, LV_OBJ_FLAG_HIDDEN);
    }
}

//buttion add function screen_setting
void screen_setting_buttion_add(){
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
    lv_group_add_obj(g, guider_ui.screen_setting_btn_wifi);  
    lv_group_add_obj(g, guider_ui.screen_setting_btn_ble);
    lv_group_add_obj(g, guider_ui.screen_setting_btn_volume);
    lv_group_add_obj(g, guider_ui.screen_setting_btn_brightness);
    lv_group_add_obj(g, guider_ui.screen_setting_btn_general);
    lv_group_add_obj(g, guider_ui.screen_setting_btn_back);  
    clear_edge_box(guider_ui.screen_setting_btn_wifi);
    clear_edge_box(guider_ui.screen_setting_btn_ble);
    clear_edge_box(guider_ui.screen_setting_btn_volume);
    clear_edge_box(guider_ui.screen_setting_btn_brightness);
    clear_edge_box(guider_ui.screen_setting_btn_general);
    clear_edge_box(guider_ui.screen_setting_btn_back);
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
}

//buttion add function screen_todosel
void screen_ai_buttion_add(){
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
    lv_group_add_obj(g, guider_ui.screen_ai_btn_day);  
    lv_group_add_obj(g, guider_ui.screen_ai_btn_week); 
    lv_group_add_obj(g, guider_ui.screen_ai_btn_cat); 
    lv_group_add_obj(g, guider_ui.screen_ai_btn_sum); 
    lv_group_add_obj(g, guider_ui.screen_ai_btn_back);  
    clear_edge_box(guider_ui.screen_ai_btn_day);
    clear_edge_box(guider_ui.screen_ai_btn_week);
    clear_edge_box(guider_ui.screen_ai_btn_cat);
    clear_edge_box(guider_ui.screen_ai_btn_sum);
    clear_edge_box(guider_ui.screen_ai_btn_back);
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

void custom_init(lv_ui *ui)
{

}

