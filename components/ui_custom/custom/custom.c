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
#include "lvgl.h"
#include "custom.h"
#include "widgets_init.h"

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
//timing variables
bool task_real_time_en = 0;
bool timing_back_en = 0;
int16_t timing_year = 0;
int16_t timing_month = 0;
int16_t timing_day = 0;
int16_t timing_weekday = 0;
//wificfg variable
bool keyboard_start_en = 0;
bool wifi_scan_en = 0;
bool wifi_disconnect_en = 0;
bool wifi_connect_en = 0;
bool wifi_load_en = 0;
bool wifi_success = 0;
char *wifi_ssid = NULL;
//todosel variable
bool focus_en = 0;
//wordcard variables
bool wordcard_change = 0;
bool reviewcard_change = 0;
bool wordcard_session_start = 0;
bool reviewcard_session_start = 0;
bool led_green_en = 0;
bool led_red_en = 0;
int  review_index = 0;
int  learned_cards_num = 0; //have learned cards num in all cards
int  total_cards_num = 5000; //all cards number
int  review_cards_num = 0; // review cards num in all cards
int  cards_index = 0;
int  round_cards_num = 30;
//screen_ai variables
bool ai_day_start = 0;
bool ai_week_start = 0;
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
static void countdown_cb(lv_timer_t *t){
    if(timer_all_seconds == 0){
        lv_timer_del(t);
        pomodoro_timer = NULL;
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
        countdown_start_label = 0;
        countdown_process_label = 0;
        return;
    }
    timer_all_seconds -= 1;
    int hours = timer_all_seconds / 3600;
    int minutes = (timer_all_seconds % 3600) / 60;
    int seconds = timer_all_seconds % 60;
    lv_label_set_text_fmt(guider_ui.screen_pomodoro_label_timer,"%02d:%02d:%02d",hours,minutes,seconds);
}

void countdown_start(int16_t minutes){
    timer_all_seconds = minutes * 60;
    pomodoro_timer = lv_timer_create(countdown_cb,1000,NULL);
}
//-----------------------pomodoro stopwatch------------------------//
//-----------------------------------------------------------------//
static void stopwatch_cb(lv_timer_t *t){
    timer_all_seconds += 1;
    int hours = timer_all_seconds / 3600;
    int minutes = (timer_all_seconds % 3600) / 60;
    int seconds = timer_all_seconds % 60;
    lv_label_set_text_fmt(guider_ui.screen_pomodoro_label_timer,"%02d:%02d:%02d",hours,minutes,seconds);
}

void stopwatch_start(){
    timer_all_seconds = 0;
    pomodoro_timer = lv_timer_create(stopwatch_cb,1000,NULL);
}

//-----------------------wifi_status show----------------------//
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
void screen_ai_load_nvs_data(bool day_or_week)
{
    ui_ai_result_t *r = NULL;
    if (day_or_week == 0)
        r = &g_ui_daily_result;
    else if (day_or_week == 1)
        r = &g_ui_weekly_result;
    else
        return;  /* NVS 中尚无数据，保持默认占位内容 */

    lv_label_set_text(guider_ui.screen_ai_label_cat1, r->cat1_name);
    lv_label_set_text(guider_ui.screen_ai_label_cat2, r->cat2_name);
    lv_label_set_text(guider_ui.screen_ai_label_cat3, r->cat3_name);
    lv_label_set_text_fmt(guider_ui.screen_ai_label_cat1_num, "%d%%", r->cat1_percent);
    lv_label_set_text_fmt(guider_ui.screen_ai_label_cat2_num, "%d%%", r->cat2_percent);
    lv_label_set_text_fmt(guider_ui.screen_ai_label_cat3_num, "%d%%", r->cat3_percent);
    lv_arc_set_value(guider_ui.screen_ai_arc_1, r->cat1_percent);
    lv_arc_set_value(guider_ui.screen_ai_arc_2, r->cat2_percent);
    lv_arc_set_value(guider_ui.screen_ai_arc_3, r->cat3_percent);
    lv_label_set_text(guider_ui.screen_ai_label_sum,   r->summary);
    lv_label_set_text(guider_ui.screen_ai_label_quote, r->quote);
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
    lv_group_add_obj(g, guider_ui.screen_main_btn_setting);
    lv_group_add_obj(g, guider_ui.screen_main_btn_realtime);
    //clear focus box
    clear_focus_box(guider_ui.screen_main_btn_todolist);
    clear_focus_box(guider_ui.screen_main_btn_done);
    clear_focus_box(guider_ui.screen_main_btn_word);
    clear_focus_box(guider_ui.screen_main_btn_timer);
    clear_focus_box(guider_ui.screen_main_btn_ai);
    clear_focus_box(guider_ui.screen_main_btn_fun);
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

}
//buttion add function screen_wordmeun
void screen_wordmenu_buttion_add(){
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
    
}

//buttion add function screen_wordcard
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

    reviewcard_session_start = 1;
    reviewcard_change = 1;
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
    lv_group_add_obj(g, guider_ui.screen_setting_btn_data);
    lv_group_add_obj(g, guider_ui.screen_setting_btn_todo);
    lv_group_add_obj(g, guider_ui.screen_setting_btn_vocab);
    lv_group_add_obj(g, guider_ui.screen_setting_btn_general);
    lv_group_add_obj(g, guider_ui.screen_setting_btn_back);  
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
void custom_init(lv_ui *ui)
{

}

