/**
 * @file ui_locale.h
 * @brief 中英文界面。已有思源字保持原样，缺字才落到同字号汉字备用字。
 */
#ifndef UI_LOCALE_H
#define UI_LOCALE_H

#include "lvgl.h"
#include <stdbool.h>

void ui_locale_init(void);
bool ui_lang_is_zh(void);
void ui_lang_toggle(void);

/** 按当前语言取一句。en / zh 都要是长期有效的字符串。 */
const char *ui_tr(const char *en, const char *zh);

/** wday: 0 周日 … 6 周六。 */
const char *ui_weekday(int wday);

/** 答案之书。中文表对不上时退回英文。 */
const char *ui_answer(unsigned index, const char *en_fallback);

const lv_font_t *ui_font_12(void);
const lv_font_t *ui_font_14(void);
const lv_font_t *ui_font_16(void);

/**
 * 主页标题和设置按钮用的 14 号思源。英文仍是原来的字，
 * 中文只补这几句界面文案里的字。
 */
const lv_font_t *ui_font_chrome(void);
void ui_font_set_chrome(lv_obj_t *obj);

/**
 * 给思源字体挂上待办那套通用汉字。主页标题和设置按钮不要走这里。
 */
void ui_font_bind(lv_obj_t *root);

#endif
