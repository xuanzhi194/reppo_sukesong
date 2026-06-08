/**
 * 单词卡与 word_storage 的桥接（放在 main，不放入 ui_custom，便于 GUI Guider 移植）
 */
#include "word_storage.h"

void wordcard_bridge_init(void)
{
    /* 保证本翻译单元参与链接 */
}

void wordcard_on_menu_enter(void)
{
    word_storage_ensure_review_saved();
}
