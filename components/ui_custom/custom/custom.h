/*
* Copyright 2023 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#ifndef __CUSTOM_H_
#define __CUSTOM_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "gui_guider.h"
/* todolist 类型定义（内联，防止与 todo_storage.h 重复定义）*/
#ifndef TODO_MAX_ITEMS
#define TODO_MAX_ITEMS  100
#define TODO_NAME_LEN   128
typedef struct {
    char     name[TODO_NAME_LEN];
    uint16_t duration_min;
    uint8_t  done;
    char     finish_date[16];
} todo_item_t;
typedef struct {
    uint8_t     count;
    todo_item_t items[TODO_MAX_ITEMS];
} todo_list_t;
#endif

/* AI 结果 UI 缓存结构体（与 ai_result_t 字段一致，无需引用 ai_demo_task.h） */
typedef struct {
    char cat1_name[32];
    int  cat1_percent;
    char cat2_name[32];
    int  cat2_percent;
    char cat3_name[32];
    int  cat3_percent;
    char summary[256];
    char quote[128];
    char summary_date[40];
    bool valid;
} ui_ai_result_t;

extern ui_ai_result_t g_ui_daily_result;
extern ui_ai_result_t g_ui_weekly_result;

// variables indicate
//todolist
extern todo_list_t ui_todo_list;  // 在 main 中由 g_todo_list 赋值
extern bool   todosel_back;
extern int8_t todo_index;
extern int8_t page_num;
extern int8_t page_cur;
extern bool   done_array[25][4];
extern bool todo_back_label;
extern bool todo_archive_label;
extern bool main_load_label;
extern int8_t done_sum;
extern int8_t todo_sum;
//timer
extern int  current_task_index;
extern int16_t timer_all_num ;
extern uint32_t timer_all_seconds;
extern lv_timer_t *pomodoro_timer;
extern bool pomodoro_countdown;
extern bool pomodoro_stopwatch;
extern bool countdown_start_label;
extern bool countdown_process_label;
extern bool stopwatch_start_label;
extern bool stopwatch_process_label;
extern volatile bool task_real_time_en;
extern bool timing_back_en;
extern int16_t timing_year;
extern int16_t timing_month;
extern int16_t timing_day;
extern int16_t timing_weekday;
extern bool wordcard_change;
extern bool reviewcard_change;
extern bool wordcard_session_start;
extern bool reviewcard_session_start;
extern bool led_green_en; // Trigger green flow light on btn_dui click
extern bool led_red_en;   // Trigger red flow light on btn_cuo click
extern int  review_index;
extern int  learned_cards_num;
extern int  total_cards_num;
extern int  review_cards_num;
extern int  cards_index;
extern int  round_cards_num;
extern volatile bool keyboard_start_en;
extern volatile bool wifi_scan_en;
extern volatile bool wifi_disconnect_en;
extern volatile bool wifi_connect_en;
extern volatile bool wifi_load_en;
extern volatile bool wifi_success;
extern char *wifi_ssid;
extern char wifi_connect_ssid[33];
extern char  current_task_name[128];
extern bool  focus_en;
extern bool  focus_session_active;
extern bool  focus_complete_pending;
extern int   focus_session_minutes;
extern int16_t focus_preset_minutes;
extern bool countdown_timeout_pending;
extern volatile bool ai_day_start;
extern volatile bool ai_week_start;
extern int  all_focus_time;
extern int  max_focus_time;
extern int  focus_level_hour[10];
extern bool volume_en;
extern uint8_t brightness_level;
extern bool ble_success;
extern bool ble_enabled;
extern volatile bool ble_toggle_on_req;
extern volatile bool ble_toggle_off_req;
extern volatile bool ble_disconnect_req;
extern bool ble_flow_led_en;
extern volatile bool ble_ui_refresh_req;
extern volatile bool todo_ble_ui_refresh_label;
void screen_setting_ble_refresh_icons(void);
void todo_ble_sync_ui_refresh(void);
/* 答案之书：custom 置位请求，main 写入 answer_book_rand */
extern bool answer_book_rand_req;
extern unsigned int answer_book_rand;
extern volatile bool answer_loading_led_stop_req;
/* done_list 结构体，用于 screen_donelist 展示 task_done.csv */
#define DONE_MAX_ITEMS  100
typedef struct {
    char name[128];
    char date[16];
    uint16_t duration_min;
} done_item_t;
typedef struct {
    int         count;
    done_item_t items[DONE_MAX_ITEMS];
} done_list_t;
extern done_list_t ui_done_list;
extern bool donelist_load_label;
extern uint8_t donelist_page_cur;
extern uint8_t donelist_page_num;
extern int     done_total_tasks;
// func indicate
int  donelist_load_from_csv(void);
void donelist_render_page(int page_cur);
void custom_init(lv_ui *ui);
void screen_reppo_custom_init();
void countdown_start(int16_t minutes);
void stopwatch_start();
void pomodoro_timer_stop(void);
void pomodoro_countdown_led_refresh(void);
void pomodoro_countdown_led_clear(void);
/** 番茄钟/秒表运行中显示 img_pause，暂停或未开始时隐藏 */
void pomodoro_pause_icon_refresh(void);
void focus_session_cancel(void);
extern void wifi_connect_show();
extern void wifi_disconnect_show();
void wificfg_show_connect_ui(lv_obj_t *list_btn, const char *password, bool focus_connect_btn);
/* 模拟器用 custom.c 弱符号默认实现；ESP32 固件由 main/wificfg_bridge.c 强符号覆盖 */
void wordcard_on_menu_enter(void);
void volume_on_enabled(void);
void volume_on_disabled(void);
void brightness_apply(void);
void brightness_update_icon(void);
void brightness_cycle(void);
void wificfg_on_list_item_clicked(lv_obj_t *list_item);
extern void screen_ai_load_nvs_data(bool day_or_week);//0 -> day 1->week
void screen_ai_apply_result(const ui_ai_result_t *r);
void screen_ai_update_focus_level(void);
//screens buttion add
extern lv_group_t *g;
void group_focus_obj(lv_obj_t *obj);
void screen_todolist_buttion_add();
void screen_main_buttion_add();
void screen_log_buttion_add();
void screen_timing_buttion_add();
void screen_pomodoro_buttion_add();
void screen_wordcard_buttion_add();
void wordcard_screen_custom_init(lv_ui *ui);
void wordcard_show_dog_dui(void);
void wordcard_show_dog_cuo(void);
void wordcard_hide_dog_anims(void);
void screen_wordmenu_buttion_add();
void screen_reviewcard_buttion_add();
void screen_setting_buttion_add();
void screen_wificfg_buttion_add();
void screen_todosel_buttion_add();
void screen_ai_buttion_add();
void screen_donelist_buttion_add();
void screen_todoback_buttion_add();
void screen_answer_buttion_add();
void screen_getanswer_buttion_add();
void screen_picture_buttion_add();
void screen_picture_custom_init();
/** 弱符号默认在 custom.c；固件强符号见 main/picture_bridge.c */
void picture_bridge_screen_on_load(void);
void answer_book_show_random_answer(void);

void my_kb_event_cb(lv_event_t * e);
#ifdef __cplusplus
}
#endif
#endif /* EVENT_CB_H_ */
