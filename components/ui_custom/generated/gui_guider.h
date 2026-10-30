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
	lv_obj_t *screen_main_btn_timer;
	lv_obj_t *screen_main_btn_timer_label;
	lv_obj_t *screen_main_btn_ai;
	lv_obj_t *screen_main_btn_ai_label;
	lv_obj_t *screen_main_btn_gashapon;
	lv_obj_t *screen_main_btn_gashapon_label;
	lv_obj_t *screen_main_btn_word;
	lv_obj_t *screen_main_btn_word_label;
	lv_obj_t *screen_main_btn_fun;
	lv_obj_t *screen_main_btn_fun_label;
	lv_obj_t *screen_main_btn_photo;
	lv_obj_t *screen_main_btn_photo_label;
	lv_obj_t *screen_main_btn_game;
	lv_obj_t *screen_main_btn_game_label;
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
	lv_obj_t *screen_main_img_photo_icon;
	lv_obj_t *screen_main_cat_left;
	lv_obj_t *screen_main_cat_right;
	lv_obj_t *screen_main_img_gash_icon;
	lv_obj_t *screen_main_img_game_icon;
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
	lv_obj_t *screen_todosel_label_1;
	lv_obj_t *screen_todoback;
	bool screen_todoback_del;
	lv_obj_t *screen_todoback_label_name;
	lv_obj_t *screen_todoback_btn_cancel;
	lv_obj_t *screen_todoback_btn_cancel_label;
	lv_obj_t *screen_todoback_btn_recover;
	lv_obj_t *screen_todoback_btn_recover_label;
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
	lv_obj_t *screen_pomodoro_img_pause;
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
	lv_obj_t *screen_picture;
	bool screen_picture_del;
	lv_obj_t *screen_picture_img_picture;
	lv_obj_t *screen_gash;
	bool screen_gash_del;
	lv_obj_t *screen_gash_img_stick;
	lv_obj_t *screen_gash_img_claw1;
	lv_obj_t *screen_gash_img_claw2;
	lv_obj_t *screen_gash_img_claw3;
	lv_obj_t *screen_gash_img_ball1;
	lv_obj_t *screen_gash_img_ball2;
	lv_obj_t *screen_gash_img_ball3;
	lv_obj_t *screen_gash_img_ball4;
	lv_obj_t *screen_gash_img_randontask;
	lv_obj_t *screen_gash_btn_addtodo;
	lv_obj_t *screen_gash_btn_addtodo_label;
	lv_obj_t *screen_gash_btn_again;
	lv_obj_t *screen_gash_btn_again_label;
	lv_obj_t *screen_game;
	bool screen_game_del;
	lv_obj_t *screen_keep;
	bool screen_keep_del;
	lv_obj_t *screen_keep_img_bg;
	lv_obj_t *screen_keep_img_lv10_wither;
	lv_obj_t *screen_keep_img_lv10;
	lv_obj_t *screen_keep_img_lv9_wither;
	lv_obj_t *screen_keep_img_lv9;
	lv_obj_t *screen_keep_img_lv8_wither;
	lv_obj_t *screen_keep_img_lv8;
	lv_obj_t *screen_keep_img_lv7_wither;
	lv_obj_t *screen_keep_img_lv7;
	lv_obj_t *screen_keep_img_lv6_wither;
	lv_obj_t *screen_keep_img_lv6;
	lv_obj_t *screen_keep_img_lv5_wither;
	lv_obj_t *screen_keep_img_lv5;
	lv_obj_t *screen_keep_img_lv4_wither;
	lv_obj_t *screen_keep_img_lv4;
	lv_obj_t *screen_keep_img_lv3_wither;
	lv_obj_t *screen_keep_img_lv3;
	lv_obj_t *screen_keep_img_lv2_wither;
	lv_obj_t *screen_keep_img_lv2;
	lv_obj_t *screen_keep_img_lv1;
	lv_obj_t *screen_keep_img_lv1_wither;
	lv_obj_t *screen_keep_bar_focustime;
	lv_obj_t *screen_keep_img_process_icon;
	lv_obj_t *screen_keep_img_dialog;
	lv_obj_t *screen_keep_label_state;
	lv_obj_t *screen_keep_label_conversion;
	lv_obj_t *screen_keep_label_focustime;
	lv_obj_t *screen_book;
	bool screen_book_del;
	lv_obj_t *screen_book_label_author;
	lv_obj_t *screen_book_label_content;
	lv_obj_t *screen_book_label_pageindex;
	lv_obj_t *screen_book_label_partname;
	lv_obj_t *screen_book_label_partindex;
	lv_obj_t *screen_book_label_bookname;
	lv_obj_t *screen_book_img_content_bg;
	lv_obj_t *screen_book_list_part;
	lv_obj_t *screen_book_list_part_item0;
	lv_obj_t *screen_book_list_part_item1;
	lv_obj_t *screen_book_list_part_item2;
	lv_obj_t *screen_book_list_part_item3;
	lv_obj_t *screen_book_list_part_item4;
	lv_obj_t *screen_book_list_part_item5;
	lv_obj_t *screen_book_btn_back;
	lv_obj_t *screen_book_btn_back_label;
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
void setup_scr_screen_timing(lv_ui *ui);
void setup_scr_screen_pomodoro(lv_ui *ui);
void setup_scr_screen_setting(lv_ui *ui);
void setup_scr_screen_wificfg(lv_ui *ui);
void setup_scr_screen_donelist(lv_ui *ui);
void setup_scr_screen_answer(lv_ui *ui);
void setup_scr_screen_loading(lv_ui *ui);
void setup_scr_screen_getanswer(lv_ui *ui);
void setup_scr_screen_picture(lv_ui *ui);
void setup_scr_screen_gash(lv_ui *ui);
void setup_scr_screen_game(lv_ui *ui);
void setup_scr_screen_keep(lv_ui *ui);
void setup_scr_screen_book(lv_ui *ui);
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

LV_IMG_DECLARE(_AAA_main_todo_1008_39x54);

LV_IMG_DECLARE(_AAA_main_todo_1008_39x54);

LV_IMG_DECLARE(_AAA_main_donelist_1008_45x54);

LV_IMG_DECLARE(_AAA_main_donelist_1008_45x54);

LV_IMG_DECLARE(_AAA_main_time_1008_43x55);

LV_IMG_DECLARE(_AAA_main_time_1008_43x55);

LV_IMG_DECLARE(_AAA_main_keep_1008_36x58);

LV_IMG_DECLARE(_AAA_main_keep_1008_36x58);

LV_IMG_DECLARE(_AAA_main_egg_1008_38x58);

LV_IMG_DECLARE(_AAA_main_egg_1008_38x58);

LV_IMG_DECLARE(_AAA_main_book_1008_69x51);

LV_IMG_DECLARE(_AAA_main_book_1008_69x51);

LV_IMG_DECLARE(_AAA_main_innovationbox_1008_55x54);

LV_IMG_DECLARE(_AAA_main_innovationbox_1008_55x54);

LV_IMG_DECLARE(_AAA_main_photo_1008_43x55);

LV_IMG_DECLARE(_AAA_main_photo_1008_43x55);

LV_IMG_DECLARE(_AAA_main_play_1008_46x54);

LV_IMG_DECLARE(_AAA_main_play_1008_46x54);

LV_IMG_DECLARE(_AAA_main_setting_1008_46x54);

LV_IMG_DECLARE(_AAA_main_setting_1008_46x54);
LV_IMG_DECLARE(_AAA_small_todo_alpha_16x14);
LV_IMG_DECLARE(_AAA_smallfun_alpha_19x18);
LV_IMG_DECLARE(_AAA_small_setting_alpha_15x15);
LV_IMG_DECLARE(_AAA_small_keep_0905_alpha_20x16);
LV_IMG_DECLARE(_AAA_small_timer_alpha_15x15);
LV_IMG_DECLARE(_AAA_small_words_alpha_17x15);
LV_IMG_DECLARE(_AAA_small_Done_alpha_15x15);
LV_IMG_DECLARE(_AAA_main_dian_alpha_6x6);
LV_IMG_DECLARE(_AAA_select_alpha_46x14);
LV_IMG_DECLARE(_AAA_select_alpha_56x23);
LV_IMG_DECLARE(_AAA_small_photo_alpha_15x14);
#include "extra/widgets/animimg/lv_animimg.h"
LV_IMG_DECLARE(screen_main_cat_leftAAA_yuexin_cat_left_00);
LV_IMG_DECLARE(screen_main_cat_leftAAA_yuexin_cat_left_01);
LV_IMG_DECLARE(screen_main_cat_leftAAA_yuexin_cat_left_02);
LV_IMG_DECLARE(screen_main_cat_leftAAA_yuexin_cat_left_03);
LV_IMG_DECLARE(screen_main_cat_leftAAA_yuexin_cat_left_04);
LV_IMG_DECLARE(screen_main_cat_leftAAA_yuexin_cat_left_05);
LV_IMG_DECLARE(screen_main_cat_leftAAA_yuexin_cat_left_06);
LV_IMG_DECLARE(screen_main_cat_leftAAA_yuexin_cat_left_07);
LV_IMG_DECLARE(screen_main_cat_leftAAA_yuexin_cat_left_08);
LV_IMG_DECLARE(screen_main_cat_leftAAA_yuexin_cat_left_09);
LV_IMG_DECLARE(screen_main_cat_leftAAA_yuexin_cat_left_10);
LV_IMG_DECLARE(screen_main_cat_leftAAA_yuexin_cat_left_11);
LV_IMG_DECLARE(screen_main_cat_leftAAA_yuexin_cat_left_12);
LV_IMG_DECLARE(screen_main_cat_leftAAA_yuexin_cat_left_13);
LV_IMG_DECLARE(screen_main_cat_leftAAA_yuexin_cat_left_14);
LV_IMG_DECLARE(screen_main_cat_leftAAA_yuexin_cat_left_15);
LV_IMG_DECLARE(screen_main_cat_leftAAA_yuexin_cat_left_16);
LV_IMG_DECLARE(screen_main_cat_leftAAA_yuexin_cat_left_17);
LV_IMG_DECLARE(screen_main_cat_leftAAA_yuexin_cat_left_18);
LV_IMG_DECLARE(screen_main_cat_leftAAA_yuexin_cat_left_19);
LV_IMG_DECLARE(screen_main_cat_leftAAA_yuexin_cat_left_20);
LV_IMG_DECLARE(screen_main_cat_leftAAA_yuexin_cat_left_21);
LV_IMG_DECLARE(screen_main_cat_leftAAA_yuexin_cat_left_22);
LV_IMG_DECLARE(screen_main_cat_leftAAA_yuexin_cat_left_23);
LV_IMG_DECLARE(screen_main_cat_leftAAA_yuexin_cat_left_24);
LV_IMG_DECLARE(screen_main_cat_leftAAA_yuexin_cat_left_25);
LV_IMG_DECLARE(screen_main_cat_leftAAA_yuexin_cat_left_26);
LV_IMG_DECLARE(screen_main_cat_leftAAA_yuexin_cat_left_27);
LV_IMG_DECLARE(screen_main_cat_leftAAA_yuexin_cat_left_28);
LV_IMG_DECLARE(screen_main_cat_leftAAA_yuexin_cat_left_29);
#include "extra/widgets/animimg/lv_animimg.h"
LV_IMG_DECLARE(screen_main_cat_rightAAA_yuexin_cat_right_00);
LV_IMG_DECLARE(screen_main_cat_rightAAA_yuexin_cat_right_01);
LV_IMG_DECLARE(screen_main_cat_rightAAA_yuexin_cat_right_02);
LV_IMG_DECLARE(screen_main_cat_rightAAA_yuexin_cat_right_03);
LV_IMG_DECLARE(screen_main_cat_rightAAA_yuexin_cat_right_04);
LV_IMG_DECLARE(screen_main_cat_rightAAA_yuexin_cat_right_05);
LV_IMG_DECLARE(screen_main_cat_rightAAA_yuexin_cat_right_06);
LV_IMG_DECLARE(screen_main_cat_rightAAA_yuexin_cat_right_07);
LV_IMG_DECLARE(screen_main_cat_rightAAA_yuexin_cat_right_08);
LV_IMG_DECLARE(screen_main_cat_rightAAA_yuexin_cat_right_09);
LV_IMG_DECLARE(screen_main_cat_rightAAA_yuexin_cat_right_10);
LV_IMG_DECLARE(screen_main_cat_rightAAA_yuexin_cat_right_11);
LV_IMG_DECLARE(screen_main_cat_rightAAA_yuexin_cat_right_12);
LV_IMG_DECLARE(screen_main_cat_rightAAA_yuexin_cat_right_13);
LV_IMG_DECLARE(screen_main_cat_rightAAA_yuexin_cat_right_14);
LV_IMG_DECLARE(screen_main_cat_rightAAA_yuexin_cat_right_15);
LV_IMG_DECLARE(screen_main_cat_rightAAA_yuexin_cat_right_16);
LV_IMG_DECLARE(screen_main_cat_rightAAA_yuexin_cat_right_17);
LV_IMG_DECLARE(screen_main_cat_rightAAA_yuexin_cat_right_18);
LV_IMG_DECLARE(screen_main_cat_rightAAA_yuexin_cat_right_19);
LV_IMG_DECLARE(screen_main_cat_rightAAA_yuexin_cat_right_20);
LV_IMG_DECLARE(screen_main_cat_rightAAA_yuexin_cat_right_21);
LV_IMG_DECLARE(screen_main_cat_rightAAA_yuexin_cat_right_22);
LV_IMG_DECLARE(screen_main_cat_rightAAA_yuexin_cat_right_23);
LV_IMG_DECLARE(screen_main_cat_rightAAA_yuexin_cat_right_24);
LV_IMG_DECLARE(screen_main_cat_rightAAA_yuexin_cat_right_25);
LV_IMG_DECLARE(screen_main_cat_rightAAA_yuexin_cat_right_26);
LV_IMG_DECLARE(screen_main_cat_rightAAA_yuexin_cat_right_27);
LV_IMG_DECLARE(screen_main_cat_rightAAA_yuexin_cat_right_28);
LV_IMG_DECLARE(screen_main_cat_rightAAA_yuexin_cat_right_29);
LV_IMG_DECLARE(_AAA_small_egg_0905_alpha_16x14);
LV_IMG_DECLARE(_AAA_small_game_0905_alpha_16x14);

LV_IMG_DECLARE(_AAA_todolist_bg_320x170);

LV_IMG_DECLARE(_AAA_todosel_bg_320x170);

LV_IMG_DECLARE(_AAA_todo_fanqie_bg2_320x170);

LV_IMG_DECLARE(_AAA_clock_bg_320x170);
LV_IMG_DECLARE(_AAA_shandian_icon_alpha_9x13);
LV_IMG_DECLARE(_AAA_clock_wifi_icon_alpha_14x14);
LV_IMG_DECLARE(_AAA_clock_wifiduan_icon_alpha_15x15);

LV_IMG_DECLARE(_AAA_fanqie_bg_320x170);
LV_IMG_DECLARE(_pomorect_alpha_257x52);
LV_IMG_DECLARE(_AAA_fanqie_xiangling_alpha_288x146);
LV_IMG_DECLARE(_AAA_daojishi_alpha_15x15);
LV_IMG_DECLARE(_AAA_miaobiao_alpha_15x15);
LV_IMG_DECLARE(_AAA_stop_alpha_69x21);

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

LV_IMG_DECLARE(_AAA_done_bg_320x170);

LV_IMG_DECLARE(_AAA_answer_1_0905_320x170);

LV_IMG_DECLARE(_AAA_answer_2_0905_320x170);
LV_IMG_DECLARE(_AAA_answer_icon_bg_alpha_58x58);

LV_IMG_DECLARE(_AAA__answer_end_bg_320x170);
LV_IMG_DECLARE(_word_BG1_alpha_320x170);

LV_IMG_DECLARE(_AAA_clawmachine_bg_0905_320x170);
LV_IMG_DECLARE(_AAA_Handle_0905_alpha_10x16);
LV_IMG_DECLARE(_AAA_claw1_0905_alpha_15x42);
LV_IMG_DECLARE(_AAA_claw2_0905_alpha_15x58);
LV_IMG_DECLARE(_AAA_claw3_0905_alpha_18x58);
LV_IMG_DECLARE(_AAA_ball1_0905_alpha_26x26);
LV_IMG_DECLARE(_AAA_ball2_0905_alpha_26x26);
LV_IMG_DECLARE(_AAA_ball3_0905_alpha_26x26);
LV_IMG_DECLARE(_AAA_ball4_0905_alpha_26x26);
LV_IMG_DECLARE(_AAA_randomtask_0905_alpha_199x140);
LV_IMG_DECLARE(_AAA_keep_bg_alpha_312x137);
LV_IMG_DECLARE(_AAA_keep_LV10_wither_alpha_200x150);
LV_IMG_DECLARE(_AAA_keep_LV10_alpha_184x160);
LV_IMG_DECLARE(_AAA_keep_LV9_wither_alpha_170x155);
LV_IMG_DECLARE(_AAA_keep_LV9_alpha_165x161);
LV_IMG_DECLARE(_AAA_keep_LV8_wither_alpha_154x170);
LV_IMG_DECLARE(_AAA_keep_LV8_alpha_154x170);
LV_IMG_DECLARE(_AAA_keep_LV7_wither_alpha_186x170);
LV_IMG_DECLARE(_AAA_keep_LV7_alpha_186x170);
LV_IMG_DECLARE(_AAA_keep_LV6_wither_alpha_180x160);
LV_IMG_DECLARE(_AAA_keep_LV6_alpha_189x160);
LV_IMG_DECLARE(_AAA_keep_LV5_wither_alpha_170x158);
LV_IMG_DECLARE(_AAA_keep_LV5_alpha_170x158);
LV_IMG_DECLARE(_AAA_keep_LV4_wither_alpha_164x152);
LV_IMG_DECLARE(_AAA_keep_LV4_alpha_164x149);
LV_IMG_DECLARE(_AAA_keep_LV3_wither_alpha_114x147);
LV_IMG_DECLARE(_AAA_keep_LV3_alpha_114x147);
LV_IMG_DECLARE(_AAA_keep_LV2_wither_alpha_88x141);
LV_IMG_DECLARE(_AAA_keep_LV2_alpha_88x141);
LV_IMG_DECLARE(_AAA_keep_LV1_alpha_96x107);
LV_IMG_DECLARE(_AAA_keep_LV1_wither_alpha_94x106);
LV_IMG_DECLARE(_AAA_keep_icon_alpha_14x14);
LV_IMG_DECLARE(_AAA_keep_dialogbox_alpha_204x78);

LV_IMG_DECLARE(_AAA_ebook_BG_320x170);
LV_IMG_DECLARE(_AAA_ebook_contents_BG_alpha_136x170);

LV_FONT_DECLARE(lv_font_montserratMedium_16)
LV_FONT_DECLARE(lv_font_montserratMedium_12)
LV_FONT_DECLARE(lv_font_Acme_Regular_12)
LV_FONT_DECLARE(lv_font_SourceHanSansSC_Medium_14)
LV_FONT_DECLARE(lv_font_SourceHanSansSC_Medium_16)
LV_FONT_DECLARE(lv_font_montserratMedium_20)
LV_FONT_DECLARE(lv_font_Acme_Regular_14)
LV_FONT_DECLARE(lv_font_montserratMedium_14)
LV_FONT_DECLARE(lv_font_SourceHanSansSC_Medium_52)
LV_FONT_DECLARE(lv_font_SourceHanSansSC_Medium_17)
LV_FONT_DECLARE(lv_font_Abel_regular_30)
LV_FONT_DECLARE(lv_font_AdobeFangsongStdRegular_14)
LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_13)
LV_FONT_DECLARE(lv_font_AdobeFangsongStdRegular_16)
LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_16)
LV_FONT_DECLARE(lv_font_montserratMedium_25)
LV_FONT_DECLARE(lv_font_SourceHanSansSC_Medium_12)
LV_FONT_DECLARE(lv_font_Alatsi_Regular_12)
LV_FONT_DECLARE(lv_font_SourceHanSansSC_Medium_11)
LV_FONT_DECLARE(lv_font_SourceHanSansSC_Medium_13)
LV_FONT_DECLARE(lv_font_arial_14)
LV_FONT_DECLARE(lv_font_SourceHanSansSC_Normal_10)
LV_FONT_DECLARE(lv_font_AlexBrush_Regular_16)
LV_FONT_DECLARE(lv_font_arial_16)
LV_FONT_DECLARE(lv_font_ArchitectsDaughter_12)
LV_FONT_DECLARE(lv_font_SourceHanSansSC_Medium_18)
LV_FONT_DECLARE(lv_font_SourceHanSansSC_Medium_20)
LV_FONT_DECLARE(lv_font_SourceHanSansSC_Normal_16)


#ifdef __cplusplus
}
#endif
#endif
