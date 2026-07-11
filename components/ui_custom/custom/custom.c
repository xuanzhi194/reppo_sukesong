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
volatile bool answer_loading_led_stop_req = 0;

static const char * const answer_book_answers[] = {
    /* —— 肯定 —— */
    "毫无疑问",
    "是的",
    "当然",
    "放手去做",
    "时机已到",
    "值得尝试",
    "勇敢迈出一步",
    "命运说：行",
    "星星眨眼表示同意",
    "宇宙给你点了赞",
    "签文：大吉",
    "卦象：可行",
    "前景看好",
    "会得到支持",
    "结果会比你想的好",
    "相信直觉",
    "现在正是时候",
    "去做就对了",
    "好运正在靠近",
    "门已为你打开",
    "顺水行舟",
    "贵人会出现",
    "坚持到底会看到光",
    "你可以的",
    "值得一搏",
    "向前走吧",
    "老天开了绿灯",
    "YES",
    "全票通过",
    "马到成功",
    "水到渠成",
    "柳暗花明",
    "福星高照",
    "不必再问了",
    "答案是肯定的",
    "绝对可以",
    "没问题",
    "会有好结果",
    "抓住这次机会",
    "月亮说可以",
    "诸事皆宜",
    "大胆一点",
    "心之所向，便是答案",
    "旗开得胜",
    "否极泰来",
    "时来运转",
    "鸿运当头",
    "紫气东来",
    "天佑善人",
    "绝对没问题",
    "前景一片光明",
    "会得到意外之喜",
    "放手一搏",
    "宜动不宜静",
    "吉星高照",
    "守得云开见月明",
    "车到山前必有路",
    "船到桥头自然直",
    "山重水复，后面你懂的",
    "机会难得，抓住",
    "运势正旺",
    "东风正好",
    "春暖花开",
    "柳暗花明又一村",
    "苦尽甘来",
    "瑞气盈门",
    /* —— 否定 —— */
    "最好不要",
    "现在还不是时候",
    "重新考虑一下吧",
    "别抱太大希望",
    "答案是：不行",
    "暂时不要",
    "再等等",
    "时机未到",
    "星星摇头了",
    "宇宙说：再想想",
    "恐怕不行",
    "别冲动",
    "冷静，先别动",
    "现在做会后悔",
    "另寻他路吧",
    "此路不通",
    "卦象示凶，慎行",
    "逆风，不宜",
    "水逆，请回避",
    "今日忌大事",
    "签文：凶",
    "老天今天没点头",
    "NO",
    "全票否决",
    "三思而后行",
    "欲速则不达",
    "过犹不及",
    "塞翁失马，先别动",
    "缘木求鱼",
    "南辕北辙",
    "镜花水月，别当真",
    "不宜妄动",
    "不宜强求",
    "最好忘记",
    "不会如你所愿",
    "前景不明",
    "别一意孤行",
    "别因小失大",
    "别飞蛾扑火",
    "别守株待兔",
    "不会如期发生",
    "恐怕要失望",
    "别指望太多",
    "不是时候",
    "缘分未到",
    "胜算不大",
    "阻力不小",
    "风波四起",
    "乌云未散",
    "月缺不宜",
    "门已紧闭",
    "路已堵死",
    "此计不通",
    "此愿难成",
    "此事作罢",
    "今非良日",
    "回头是岸",
    "悬崖勒马",
    "亡羊补牢，先停",
    "前车之鉴，慎之",
    "覆水难收，勿试",
    "画地为牢，勿入",
    "作茧自缚，勿为",
    /* —— 中立 / 模糊 / 幽默 —— */
    "顺其自然",
    "再看看吧",
    "也许，也许不",
    "这需要时间",
    "你其实已经知道了",
    "命运今天休假",
    "骰子投出了模棱两可",
    "月老说再看看",
    "风水师请假了",
    "这题超纲了",
    "答案之书也需要思考",
    "别催，正在算",
    "薛定谔的答案是",
    "建议重摇一次",
    "睡醒再来问",
    "老天开了个玩笑",
    "天机不可泄露太多",
    "你问得太频繁了",
    "答案在下一页",
    "此书拒绝回答",
    "命运正在输入中…",
    "看心情",
    "看缘分",
    "看天意",
    "看时机",
    "看猫的脸色",
    "抛硬币吧",
    "宇宙正在加载，请稍候",
    "猫说你得问问它",
    "狗说：汪（翻译：随便）",
    "星星说：你猜",
    "风把答案吹跑了",
    "签筒空了，改日",
    "黄历：宜静不宜动",
    "福星在路上堵车",
    "答案之书摸了会儿鱼",
    "命运键盘卡了",
    "是又不是",
    "对又不对",
    "算了，随它",
    "你定",
    "听天由命",
    "尽人事，听天命",
    "不可说，不可说",
    "天机只泄露一半",
    "悟不到就再问",
    "好了，翻页吧",
    "问了也不一定答",
    "答了也不一定信",
    "或者：别问了，去生活",
    "立起来就再问",
    "今日额度已用完",
    "凡人，请自重",
    "你问得我心虚",
    "门开了条缝，自己推",
    "月亮还没圆，再等等",
    "猫主子没睡醒",
    "月老正在输入",
    "信号不好，请重试",
    "再问还是这句",
    "树说：静一静",
    "云说：飘哪算哪",
    "三个直觉打平，重摇",
    "另一半你自己悟",
    "由心而定",
    "问心无愧即可",
    "宇宙略过此题",
    "太阳打西边出来了？那再问",
    "谋事在人，成事在天",
    "天知地知，你知我知",
    "所以：再问一次吧",
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
}

void countdown_start(int16_t minutes){
    pomodoro_timer_stop();
    timer_all_seconds = minutes * 60;
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

static void wordcard_sync_dog_focus(void *unused)
{
    LV_UNUSED(unused);
    if (guider_ui.screen_wordcard == NULL ||
        !lv_obj_is_valid(guider_ui.screen_wordcard) ||
        lv_scr_act() != guider_ui.screen_wordcard ||
        g == NULL) {
        return;
    }
    lv_obj_t *focused = lv_group_get_focused(g);
    if (focused == guider_ui.screen_wordcard_btn_dui) {
        wordcard_show_dog_dui();
    } else if (focused == guider_ui.screen_wordcard_btn_cuo) {
        wordcard_show_dog_cuo();
    }
}

static void reviewcard_hide_dog_dui(void)
{
    if (guider_ui.screen_reviewcard_dog_dui != NULL &&
        lv_obj_is_valid(guider_ui.screen_reviewcard_dog_dui)) {
        lv_obj_add_flag(guider_ui.screen_reviewcard_dog_dui, LV_OBJ_FLAG_HIDDEN);
    }
}

void reviewcard_show_dog_dui(void)
{
    if (guider_ui.screen_reviewcard_dog_dui == NULL ||
        !lv_obj_is_valid(guider_ui.screen_reviewcard_dog_dui)) {
        return;
    }
    lv_animimg_start(guider_ui.screen_reviewcard_dog_dui);
    lv_obj_clear_flag(guider_ui.screen_reviewcard_dog_dui, LV_OBJ_FLAG_HIDDEN);
}

static void reviewcard_sync_dog_dui_focus(void *unused)
{
    LV_UNUSED(unused);
    if (guider_ui.screen_reviewcard == NULL ||
        !lv_obj_is_valid(guider_ui.screen_reviewcard) ||
        lv_scr_act() != guider_ui.screen_reviewcard ||
        review_cards_num == 0) {
        return;
    }
    if (g != NULL &&
        lv_group_get_focused(g) == guider_ui.screen_reviewcard_btn_dui) {
        reviewcard_show_dog_dui();
    }
}

static void reviewcard_btn_dui_focus_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_FOCUSED) {
        if (review_cards_num > 0) {
            reviewcard_show_dog_dui();
        }
    } else if (code == LV_EVENT_DEFOCUSED) {
        reviewcard_hide_dog_dui();
    }
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

    /* events_init 在 button_add 之后会 wordcard_hide_dog_anims()，延迟同步 focus 状态 */
    lv_async_call(wordcard_sync_dog_focus, NULL);

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
    lv_obj_add_event_cb(guider_ui.screen_reviewcard_btn_dui,
                        reviewcard_btn_dui_focus_cb, LV_EVENT_ALL, NULL);
    /* events_init 在 button_add 之后会再次 HIDDEN dog_dui，延迟同步 focus 状态 */
    lv_async_call(reviewcard_sync_dog_dui_focus, NULL);

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
    if (code == LV_EVENT_CLICKED && ble_toggle_on_req) {
        ble_enabled = true;
    }
    if (code == LV_EVENT_FOCUSED || code == LV_EVENT_CLICKED) {
        screen_setting_ble_refresh_icons();
    }
}

static void screen_setting_ble_icon_bind_hooks(void)
{
    static lv_obj_t *bound_btn;

    lv_obj_t *btn = guider_ui.screen_setting_btn_ble;
    if (btn == NULL || !lv_obj_is_valid(btn)) {
        return;
    }
    if (bound_btn == btn) {
        return;
    }
    bound_btn = btn;
    lv_obj_add_event_cb(btn, screen_setting_ble_icon_after_guider, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(btn, screen_setting_ble_icon_after_guider, LV_EVENT_CLICKED, NULL);
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
    screen_setting_ble_icon_bind_hooks();
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
    answer_loading_led_stop_req = 1;
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

