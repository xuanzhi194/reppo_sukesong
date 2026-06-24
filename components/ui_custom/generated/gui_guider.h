/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#ifndef GUI_GUIDER_H
#define GUI_GUIDER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

typedef struct
{
  
	lv_obj_t *screen_reppo;
	bool screen_reppo_del;
	lv_obj_t *screen_reppo_reppo_gif;
	lv_obj_t *screen_main;
	bool screen_main_del;
	lv_obj_t *screen_main_cont_main;
	lv_obj_t *screen_main_btn_todolist;
	lv_obj_t *screen_main_btn_todolist_label;
	lv_obj_t *screen_main_btn_done;
	lv_obj_t *screen_main_btn_done_label;
	lv_obj_t *screen_main_btn_word;
	lv_obj_t *screen_main_btn_word_label;
	lv_obj_t *screen_main_btn_timer;
	lv_obj_t *screen_main_btn_timer_label;
	lv_obj_t *screen_main_btn_ai;
	lv_obj_t *screen_main_btn_ai_label;
	lv_obj_t *screen_main_btn_fun;
	lv_obj_t *screen_main_btn_fun_label;
	lv_obj_t *screen_main_btn_setting;
	lv_obj_t *screen_main_btn_setting_label;
	lv_obj_t *screen_main_label_donenum;
	lv_obj_t *screen_main_label_todonum;
	lv_obj_t *screen_main_btn_realtime;
	lv_obj_t *screen_main_btn_realtime_label;
	lv_obj_t *screen_main_img_todo_icon;
	lv_obj_t *screen_main_img_fun_icon;
	lv_obj_t *screen_main_img_setting_icon;
	lv_obj_t *screen_main_img_ai_icon;
	lv_obj_t *screen_main_img_timer_icon;
	lv_obj_t *screen_main_img_book_icon;
	lv_obj_t *screen_main_img_done_icon;
	lv_obj_t *screen_main_img_point_icon;
	lv_obj_t *screen_main_label_chosen;
	lv_obj_t *screen_main_arc_battery;
	lv_obj_t *screen_main_img_select_timing;
	lv_obj_t *screen_main_img_select;
	lv_obj_t *screen_todolist;
	bool screen_todolist_del;
	lv_obj_t *screen_todolist_btn_back;
	lv_obj_t *screen_todolist_btn_back_label;
	lv_obj_t *screen_todolist_btn_todo1;
	lv_obj_t *screen_todolist_btn_todo1_label;
	lv_obj_t *screen_todolist_btn_todo2;
	lv_obj_t *screen_todolist_btn_todo2_label;
	lv_obj_t *screen_todolist_btn_todo3;
	lv_obj_t *screen_todolist_btn_todo3_label;
	lv_obj_t *screen_todolist_btn_todo4;
	lv_obj_t *screen_todolist_btn_todo4_label;
	lv_obj_t *screen_todolist_label_indexcut;
	lv_obj_t *screen_todolist_label_indexcur;
	lv_obj_t *screen_todolist_label_indexall;
	lv_obj_t *screen_todolist_label_donenum;
	lv_obj_t *screen_todolist_label_todonum;
	lv_obj_t *screen_todolist_btn_lastpage;
	lv_obj_t *screen_todolist_btn_lastpage_label;
	lv_obj_t *screen_todolist_btn_nxtpage;
	lv_obj_t *screen_todolist_btn_nxtpage_label;
	lv_obj_t *screen_todolist_line_1;
	lv_obj_t *screen_todolist_line_2;
	lv_obj_t *screen_todolist_line_3;
	lv_obj_t *screen_todolist_line_4;
	lv_obj_t *screen_todosel;
	bool screen_todosel_del;
	lv_obj_t *screen_todosel_btn_complete;
	lv_obj_t *screen_todosel_btn_complete_label;
	lv_obj_t *screen_todosel_btn_focus;
	lv_obj_t *screen_todosel_btn_focus_label;
	lv_obj_t *screen_todosel_btn_cancel;
	lv_obj_t *screen_todosel_btn_cancel_label;
	lv_obj_t *screen_todosel_label_task;
	lv_obj_t *screen_todoback;
	bool screen_todoback_del;
	lv_obj_t *screen_todoback_label_name;
	lv_obj_t *screen_todoback_btn_cancel;
	lv_obj_t *screen_todoback_btn_cancel_label;
	lv_obj_t *screen_todoback_btn_recover;
	lv_obj_t *screen_todoback_btn_recover_label;
	lv_obj_t *screen_wordcard;
	bool screen_wordcard_del;
	lv_obj_t *screen_wordcard_label_index;
	lv_obj_t *screen_wordcard_label_phonetic;
	lv_obj_t *screen_wordcard_label_meaning;
	lv_obj_t *screen_wordcard_btn_dui;
	lv_obj_t *screen_wordcard_btn_dui_label;
	lv_obj_t *screen_wordcard_btn_cuo;
	lv_obj_t *screen_wordcard_btn_cuo_label;
	lv_obj_t *screen_wordcard_btn_back;
	lv_obj_t *screen_wordcard_btn_back_label;
	lv_obj_t *screen_wordcard_label_spelling;
	lv_obj_t *screen_wordcard_dog_cuo;
	lv_obj_t *screen_wordcard_dog_dui;
	lv_obj_t *screen_timing;
	bool screen_timing_del;
	lv_obj_t *screen_timing_btn_main;
	lv_obj_t *screen_timing_btn_main_label;
	lv_obj_t *screen_timing_label_realtime;
	lv_obj_t *screen_timing_label_day;
	lv_obj_t *screen_timing_label_date;
	lv_obj_t *screen_timing_bar_battery;
	lv_obj_t *screen_timing_img_baticon;
	lv_obj_t *screen_timing_img_wificonnect;
	lv_obj_t *screen_timing_img_wifidis;
	lv_obj_t *screen_wordmeun;
	bool screen_wordmeun_del;
	lv_obj_t *screen_wordmeun_bar_process;
	lv_obj_t *screen_wordmeun_label_wordpercent;
	lv_obj_t *screen_wordmeun_label_reviewnum;
	lv_obj_t *screen_wordmeun_btn_learn;
	lv_obj_t *screen_wordmeun_btn_learn_label;
	lv_obj_t *screen_wordmeun_btn_review;
	lv_obj_t *screen_wordmeun_btn_review_label;
	lv_obj_t *screen_wordmeun_btn_back;
	lv_obj_t *screen_wordmeun_btn_back_label;
	lv_obj_t *screen_reviewcard;
	bool screen_reviewcard_del;
	lv_obj_t *screen_reviewcard_label_index;
	lv_obj_t *screen_reviewcard_label_spelling;
	lv_obj_t *screen_reviewcard_label_phonetic;
	lv_obj_t *screen_reviewcard_label_meaning;
	lv_obj_t *screen_reviewcard_btn_dui;
	lv_obj_t *screen_reviewcard_btn_dui_label;
	lv_obj_t *screen_reviewcard_btn_back;
	lv_obj_t *screen_reviewcard_btn_back_label;
	lv_obj_t *screen_reviewcard_dog_dance;
	lv_obj_t *screen_pomodoro;
	bool screen_pomodoro_del;
	lv_obj_t *screen_pomodoro_img_pomorect1;
	lv_obj_t *screen_pomodoro_label_timer;
	lv_obj_t *screen_pomodoro_label_pomodoro;
	lv_obj_t *screen_pomodoro_btn_stopwatch;
	lv_obj_t *screen_pomodoro_btn_stopwatch_label;
	lv_obj_t *screen_pomodoro_btn_countdown;
	lv_obj_t *screen_pomodoro_btn_countdown_label;
	lv_obj_t *screen_pomodoro_btn_back;
	lv_obj_t *screen_pomodoro_btn_back_label;
	lv_obj_t *screen_pomodoro_btn_start;
	lv_obj_t *screen_pomodoro_btn_start_label;
	lv_obj_t *screen_pomodoro_btn_reset;
	lv_obj_t *screen_pomodoro_btn_reset_label;
	lv_obj_t *screen_pomodoro_btn_addnum;
	lv_obj_t *screen_pomodoro_btn_addnum_label;
	lv_obj_t *screen_pomodoro_img_timeout;
	lv_obj_t *screen_pomodoro_btn_complete;
	lv_obj_t *screen_pomodoro_btn_complete_label;
	lv_obj_t *screen_pomodoro_btn_continue;
	lv_obj_t *screen_pomodoro_btn_continue_label;
	lv_obj_t *screen_pomodoro_btn_plus;
	lv_obj_t *screen_pomodoro_btn_plus_label;
	lv_obj_t *screen_pomodoro_btn_minus;
	lv_obj_t *screen_pomodoro_btn_minus_label;
	lv_obj_t *screen_pomodoro_img_countdown;
	lv_obj_t *screen_pomodoro_img_stopwatch;
	lv_obj_t *screen_setting;
	bool screen_setting_del;
	lv_obj_t *screen_setting_btn_back;
	lv_obj_t *screen_setting_btn_back_label;
	lv_obj_t *screen_setting_btn_general;
	lv_obj_t *screen_setting_btn_general_label;
	lv_obj_t *screen_setting_btn_brightness;
	lv_obj_t *screen_setting_btn_brightness_label;
	lv_obj_t *screen_setting_btn_volume;
	lv_obj_t *screen_setting_btn_volume_label;
	lv_obj_t *screen_setting_btn_ble;
	lv_obj_t *screen_setting_btn_ble_label;
	lv_obj_t *screen_setting_btn_wifi;
	lv_obj_t *screen_setting_btn_wifi_label;
	lv_obj_t *screen_setting_img_volume_open;
	lv_obj_t *screen_setting_img_volume_close;
	lv_obj_t *screen_setting_img_wifi_open;
	lv_obj_t *screen_setting_img_wifi_close;
	lv_obj_t *screen_setting_img_ble;
	lv_obj_t *screen_setting_img_ble_dis;
	lv_obj_t *screen_setting_img_brightness;
	lv_obj_t *screen_setting_label_about;
	lv_obj_t *screen_setting_label_txt1;
	lv_obj_t *screen_setting_label_txt2;
	lv_obj_t *screen_setting_label_txt3;
	lv_obj_t *screen_setting_label_txt4;
	lv_obj_t *screen_setting_label_info1;
	lv_obj_t *screen_setting_label_info2;
	lv_obj_t *screen_setting_label_info3;
	lv_obj_t *screen_setting_label_info4;
	lv_obj_t *screen_wificfg;
	bool screen_wificfg_del;
	lv_obj_t *screen_wificfg_img_network;
	lv_obj_t *screen_wificfg_btn_back;
	lv_obj_t *screen_wificfg_btn_back_label;
	lv_obj_t *screen_wificfg_img_statusbg;
	lv_obj_t *screen_wificfg_label_status;
	lv_obj_t *screen_wificfg_img_wifiicon;
	lv_obj_t *screen_wificfg_label_status2;
	lv_obj_t *screen_wificfg_img_wififalse;
	lv_obj_t *screen_wificfg_list_1;
	lv_obj_t *screen_wificfg_list_1_item0;
	lv_obj_t *screen_wificfg_list_1_item1;
	lv_obj_t *screen_wificfg_list_1_item2;
	lv_obj_t *screen_wificfg_list_1_item3;
	lv_obj_t *screen_wificfg_list_1_item4;
	lv_obj_t *screen_wificfg_btn_scan;
	lv_obj_t *screen_wificfg_btn_scan_label;
	lv_obj_t *screen_wificfg_btn_disconnect;
	lv_obj_t *screen_wificfg_btn_disconnect_label;
	lv_obj_t *screen_wificfg_btn_cancel;
	lv_obj_t *screen_wificfg_btn_cancel_label;
	lv_obj_t *screen_wificfg_btn_connect;
	lv_obj_t *screen_wificfg_btn_connect_label;
	lv_obj_t *screen_wificfg_ta_password;
	lv_obj_t *screen_wificfg_img_passicon;
	lv_obj_t *screen_wificfg_spinner_connect;
	lv_obj_t *screen_ai;
	bool screen_ai_del;
	lv_obj_t *screen_ai_ai_cont;
	lv_obj_t *screen_ai_arc_1;
	lv_obj_t *screen_ai_arc_2;
	lv_obj_t *screen_ai_arc_3;
	lv_obj_t *screen_ai_label_cat3;
	lv_obj_t *screen_ai_label_cat2;
	lv_obj_t *screen_ai_label_cat1;
	lv_obj_t *screen_ai_label_cat3_num;
	lv_obj_t *screen_ai_label_cat2_num;
	lv_obj_t *screen_ai_label_cat1_num;
	lv_obj_t *screen_ai_line_1;
	lv_obj_t *screen_ai_line_2;
	lv_obj_t *screen_ai_label_quote;
	lv_obj_t *screen_ai_label_sum;
	lv_obj_t *screen_ai_btn_sum;
	lv_obj_t *screen_ai_btn_sum_label;
	lv_obj_t *screen_ai_btn_cat;
	lv_obj_t *screen_ai_btn_cat_label;
	lv_obj_t *screen_ai_img_v10;
	lv_obj_t *screen_ai_img_v9;
	lv_obj_t *screen_ai_img_v8;
	lv_obj_t *screen_ai_img_v7;
	lv_obj_t *screen_ai_img_v6;
	lv_obj_t *screen_ai_img_v5;
	lv_obj_t *screen_ai_img_v4;
	lv_obj_t *screen_ai_img_v3;
	lv_obj_t *screen_ai_img_v2;
	lv_obj_t *screen_ai_img_v1;
	lv_obj_t *screen_ai_btn_week;
	lv_obj_t *screen_ai_btn_week_label;
	lv_obj_t *screen_ai_btn_day;
	lv_obj_t *screen_ai_btn_day_label;
	lv_obj_t *screen_ai_bar_focus;
	lv_obj_t *screen_ai_label_duration;
	lv_obj_t *screen_ai_btn_back;
	lv_obj_t *screen_ai_btn_back_label;
	lv_obj_t *screen_donelist;
	bool screen_donelist_del;
	lv_obj_t *screen_donelist_line_4;
	lv_obj_t *screen_donelist_line_3;
	lv_obj_t *screen_donelist_line_2;
	lv_obj_t *screen_donelist_line_1;
	lv_obj_t *screen_donelist_label_donenum;
	lv_obj_t *screen_donelist_label_nosavenum;
	lv_obj_t *screen_donelist_label_date1;
	lv_obj_t *screen_donelist_label_done1;
	lv_obj_t *screen_donelist_label_done2;
	lv_obj_t *screen_donelist_label_done3;
	lv_obj_t *screen_donelist_label_done4;
	lv_obj_t *screen_donelist_label_date2;
	lv_obj_t *screen_donelist_label_date3;
	lv_obj_t *screen_donelist_label_date4;
	lv_obj_t *screen_donelist_label_page;
	lv_obj_t *screen_donelist_btn_save;
	lv_obj_t *screen_donelist_btn_save_label;
	lv_obj_t *screen_donelist_btn_nxtpage;
	lv_obj_t *screen_donelist_btn_nxtpage_label;
	lv_obj_t *screen_donelist_btn_lastpage;
	lv_obj_t *screen_donelist_btn_lastpage_label;
	lv_obj_t *screen_donelist_label_focus1;
	lv_obj_t *screen_donelist_label_focus2;
	lv_obj_t *screen_donelist_label_focus3;
	lv_obj_t *screen_donelist_label_focus4;
	lv_obj_t *screen_donelist_btn_back;
	lv_obj_t *screen_donelist_btn_back_label;
	lv_obj_t *screen_answer;
	bool screen_answer_del;
	lv_obj_t *screen_answer_btn_start;
	lv_obj_t *screen_answer_btn_start_label;
	lv_obj_t *screen_loading;
	bool screen_loading_del;
	lv_obj_t *screen_loading_img_icon;
	lv_obj_t *screen_getanswer;
	bool screen_getanswer_del;
	lv_obj_t *screen_getanswer_btn_answer;
	lv_obj_t *screen_getanswer_btn_answer_label;
	lv_obj_t *g_kb_top_layer;
}lv_ui;

typedef void (*ui_setup_scr_t)(lv_ui * ui);

void ui_init_style(lv_style_t * style);

void ui_load_scr_animation(lv_ui *ui, lv_obj_t ** new_scr, bool new_scr_del, bool * old_scr_del, ui_setup_scr_t setup_scr,
                           lv_scr_load_anim_t anim_type, uint32_t time, uint32_t delay, bool is_clean, bool auto_del);

void ui_animation(void * var, int32_t duration, int32_t delay, int32_t start_value, int32_t end_value, lv_anim_path_cb_t path_cb,
                       uint16_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time, uint32_t playback_delay,
                       lv_anim_exec_xcb_t exec_cb, lv_anim_start_cb_t start_cb, lv_anim_ready_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb);


void init_scr_del_flag(lv_ui *ui);

void setup_ui(lv_ui *ui);

void init_keyboard(lv_ui *ui);

extern lv_ui guider_ui;


void setup_scr_screen_reppo(lv_ui *ui);
void setup_scr_screen_main(lv_ui *ui);
void setup_scr_screen_todolist(lv_ui *ui);
void setup_scr_screen_todosel(lv_ui *ui);
void setup_scr_screen_todoback(lv_ui *ui);
void setup_scr_screen_wordcard(lv_ui *ui);
void setup_scr_screen_timing(lv_ui *ui);
void setup_scr_screen_wordmeun(lv_ui *ui);
void setup_scr_screen_reviewcard(lv_ui *ui);
void setup_scr_screen_pomodoro(lv_ui *ui);
void setup_scr_screen_setting(lv_ui *ui);
void setup_scr_screen_wificfg(lv_ui *ui);
void setup_scr_screen_ai(lv_ui *ui);
void setup_scr_screen_donelist(lv_ui *ui);
void setup_scr_screen_answer(lv_ui *ui);
void setup_scr_screen_loading(lv_ui *ui);
void setup_scr_screen_getanswer(lv_ui *ui);
#include "extra/widgets/animimg/lv_animimg.h"
LV_IMG_DECLARE(screen_reppo_reppo_gifAAA_reppo_start_00);
LV_IMG_DECLARE(screen_reppo_reppo_gifAAA_reppo_start_01);
LV_IMG_DECLARE(screen_reppo_reppo_gifAAA_reppo_start_02);
LV_IMG_DECLARE(screen_reppo_reppo_gifAAA_reppo_start_03);
LV_IMG_DECLARE(screen_reppo_reppo_gifAAA_reppo_start_04);
LV_IMG_DECLARE(screen_reppo_reppo_gifAAA_reppo_start_05);
LV_IMG_DECLARE(screen_reppo_reppo_gifAAA_reppo_start_06);
LV_IMG_DECLARE(screen_reppo_reppo_gifAAA_reppo_start_07);
LV_IMG_DECLARE(screen_reppo_reppo_gifAAA_reppo_start_08);
LV_IMG_DECLARE(screen_reppo_reppo_gifAAA_reppo_start_09);
LV_IMG_DECLARE(screen_reppo_reppo_gifAAA_reppo_start_10);
LV_IMG_DECLARE(screen_reppo_reppo_gifAAA_reppo_start_11);
LV_IMG_DECLARE(screen_reppo_reppo_gifAAA_reppo_start_12);
LV_IMG_DECLARE(screen_reppo_reppo_gifAAA_reppo_start_13);
LV_IMG_DECLARE(screen_reppo_reppo_gifAAA_reppo_start_14);
LV_IMG_DECLARE(screen_reppo_reppo_gifAAA_reppo_start_15);
LV_IMG_DECLARE(screen_reppo_reppo_gifAAA_reppo_start_16);
LV_IMG_DECLARE(screen_reppo_reppo_gifAAA_reppo_start_17);
LV_IMG_DECLARE(screen_reppo_reppo_gifAAA_reppo_start_18);
LV_IMG_DECLARE(screen_reppo_reppo_gifAAA_reppo_start_19);
LV_IMG_DECLARE(screen_reppo_reppo_gifAAA_reppo_start_20);
LV_IMG_DECLARE(screen_reppo_reppo_gifAAA_reppo_start_21);
LV_IMG_DECLARE(screen_reppo_reppo_gifAAA_reppo_start_22);
LV_IMG_DECLARE(screen_reppo_reppo_gifAAA_reppo_start_23);

LV_IMG_DECLARE(_AAA_main_bg_320x170);

LV_IMG_DECLARE(_AAA_main_todolist_39x54);

LV_IMG_DECLARE(_AAA_main_todolist_39x54);

LV_IMG_DECLARE(_AAA_main_donelist_45x54);

LV_IMG_DECLARE(_AAA_main_donelist_45x54);

LV_IMG_DECLARE(_AAA_main_book_37x55);

LV_IMG_DECLARE(_AAA_main_book_37x55);

LV_IMG_DECLARE(_AAA_main_time_43x55);

LV_IMG_DECLARE(_AAA_main_time_43x55);

LV_IMG_DECLARE(_AAA_main_ai_49x54);

LV_IMG_DECLARE(_AAA_main_ai_49x54);

LV_IMG_DECLARE(_AAA_main_fun_55x54);

LV_IMG_DECLARE(_AAA_main_fun_55x54);

LV_IMG_DECLARE(_AAA_main_setting_46x54);

LV_IMG_DECLARE(_AAA_main_setting_46x54);
LV_IMG_DECLARE(_AAA_small_todo_alpha_16x15);
LV_IMG_DECLARE(_AAA_smallfun_alpha_19x18);
LV_IMG_DECLARE(_AAA_small_setting_alpha_16x16);
LV_IMG_DECLARE(_AAA_small_deepseek_alpha_20x16);
LV_IMG_DECLARE(_AAA_small_timer_alpha_15x15);
LV_IMG_DECLARE(_AAA_small_words_alpha_17x15);
LV_IMG_DECLARE(_AAA_small_Done_alpha_15x15);
LV_IMG_DECLARE(_AAA_main_dian_alpha_6x6);
LV_IMG_DECLARE(_AAA_select_alpha_46x14);
LV_IMG_DECLARE(_AAA_select_alpha_56x23);

LV_IMG_DECLARE(_AAA_todolist_bg_320x170);

LV_IMG_DECLARE(_AAA_todosel_bg_320x170);

LV_IMG_DECLARE(_AAA_todo_fanqie_bg2_320x170);

LV_IMG_DECLARE(_AAA_flashcards_learn_bg_320x170);
#include "extra/widgets/animimg/lv_animimg.h"
LV_IMG_DECLARE(screen_wordcard_dog_cuoAAA_dog_shake_0);
LV_IMG_DECLARE(screen_wordcard_dog_cuoAAA_dog_shake_1);
LV_IMG_DECLARE(screen_wordcard_dog_cuoAAA_dog_shake_2);
LV_IMG_DECLARE(screen_wordcard_dog_cuoAAA_dog_shake_3);
LV_IMG_DECLARE(screen_wordcard_dog_cuoAAA_dog_shake_4);
LV_IMG_DECLARE(screen_wordcard_dog_cuoAAA_dog_shake_5);
#include "extra/widgets/animimg/lv_animimg.h"
LV_IMG_DECLARE(screen_wordcard_dog_duiAAA_dog_love_0);
LV_IMG_DECLARE(screen_wordcard_dog_duiAAA_dog_love_1);
LV_IMG_DECLARE(screen_wordcard_dog_duiAAA_dog_love_2);

LV_IMG_DECLARE(_AAA_clock_bg_320x170);
LV_IMG_DECLARE(_AAA_shandian_icon_alpha_9x13);
LV_IMG_DECLARE(_AAA_clock_wifi_icon_alpha_14x14);
LV_IMG_DECLARE(_AAA_clock_wifiduan_icon_alpha_15x15);

LV_IMG_DECLARE(_AAA_cishu_320x170);

LV_IMG_DECLARE(_AAA_flashcards_review_bg_320x170);
#include "extra/widgets/animimg/lv_animimg.h"
LV_IMG_DECLARE(screen_reviewcard_dog_danceAAA_dog_dance_0);
LV_IMG_DECLARE(screen_reviewcard_dog_danceAAA_dog_dance_1);
LV_IMG_DECLARE(screen_reviewcard_dog_danceAAA_dog_dance_2);

LV_IMG_DECLARE(_AAA_fanqie_bg_320x170);
LV_IMG_DECLARE(_pomorect_alpha_257x52);
LV_IMG_DECLARE(_AAA_fanqie_xiangling_alpha_288x146);
LV_IMG_DECLARE(_AAA_daojishi_alpha_15x15);
LV_IMG_DECLARE(_AAA_miaobiao_alpha_15x15);

LV_IMG_DECLARE(_AAA_reppo_setting_bg_320x170);
LV_IMG_DECLARE(_AAA_reppo_volume_kai_icon_alpha_93x75);
LV_IMG_DECLARE(_AAA_reppo_volume_guan_icon_alpha_93x75);
LV_IMG_DECLARE(_AAA_clock_wifi_icon_alpha_100x100);
LV_IMG_DECLARE(_AAA_clock_wifiduan_icon_alpha_100x100);
LV_IMG_DECLARE(_AAA_reppo_bluetooth_icon_alpha_57x92);
LV_IMG_DECLARE(_AAA_reppo_ble_dis_alpha_100x100);
LV_IMG_DECLARE(_AAA_setting_brightness_alpha_105x105);

LV_IMG_DECLARE(_AAA_wifi_lianjie_320x170);
LV_IMG_DECLARE(_pomorect_alpha_160x125);
LV_IMG_DECLARE(_AAA_wifi_kuang_alpha_145x125);
LV_IMG_DECLARE(_AAA_wifi_icons_alpha_110x110);
LV_IMG_DECLARE(_wifi_false_alpha_20x20);
LV_IMG_DECLARE(_AAA_wifi_suo_icon_alpha_13x15);

LV_IMG_DECLARE(_AAA_Overview_BG_320x510);
LV_IMG_DECLARE(_AAA_V10_alpha_304x158);
LV_IMG_DECLARE(_AAA_V9_alpha_304x158);
LV_IMG_DECLARE(_AAA_V8_alpha_304x158);
LV_IMG_DECLARE(_AAA_V7_alpha_304x158);
LV_IMG_DECLARE(_AAA_V6_alpha_304x158);
LV_IMG_DECLARE(_AAA_V5_alpha_304x158);
LV_IMG_DECLARE(_AAA_V4_alpha_304x158);
LV_IMG_DECLARE(_AAA_V3_alpha_304x158);
LV_IMG_DECLARE(_AAA_V2_alpha_304x158);
LV_IMG_DECLARE(_AAA_V1_alpha_304x158);

LV_IMG_DECLARE(_AAA_done_bg_320x170);

LV_IMG_DECLARE(_AAA_answer_start_bg_320x170);

LV_IMG_DECLARE(_AAA_answer_wait_bg_320x170);
LV_IMG_DECLARE(_AAA_answer_icon_bg_alpha_58x58);

LV_IMG_DECLARE(_AAA__answer_end_bg_320x170);

LV_FONT_DECLARE(lv_font_montserratMedium_16)
LV_FONT_DECLARE(lv_font_Acme_Regular_12)
LV_FONT_DECLARE(lv_font_SourceHanSansSC_Medium_14)
LV_FONT_DECLARE(lv_font_SourceHanSansSC_Medium_16)
LV_FONT_DECLARE(lv_font_montserratMedium_20)
LV_FONT_DECLARE(lv_font_Acme_Regular_14)
LV_FONT_DECLARE(lv_font_arial_14)
LV_FONT_DECLARE(lv_font_montserratMedium_12)
LV_FONT_DECLARE(lv_font_SourceHanSansSC_Medium_28)
LV_FONT_DECLARE(lv_font_SourceHanSansSC_Medium_52)
LV_FONT_DECLARE(lv_font_SourceHanSansSC_Medium_17)
LV_FONT_DECLARE(lv_font_SourceHanSansSC_Medium_12)
LV_FONT_DECLARE(lv_font_Abel_regular_30)
LV_FONT_DECLARE(lv_font_AdobeFangsongStdRegular_14)
LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_13)
LV_FONT_DECLARE(lv_font_AdobeFangsongStdRegular_16)
LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_16)
LV_FONT_DECLARE(lv_font_montserratMedium_25)
LV_FONT_DECLARE(lv_font_Alatsi_Regular_12)
LV_FONT_DECLARE(lv_font_SourceHanSansSC_Medium_11)
LV_FONT_DECLARE(lv_font_SourceHanSansSC_Medium_13)


#ifdef __cplusplus
}
#endif
#endif
