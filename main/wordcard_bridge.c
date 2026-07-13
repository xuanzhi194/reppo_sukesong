/**
 * 单词卡与 word_storage 的桥接（放在 main，不放入 ui_custom，便于 GUI Guider 移植）
 */
#include "custom.h"
#include "word_storage.h"

void wordcard_bridge_init(void)
{
    /* 保证本翻译单元参与链接 */
}

void wordcard_on_menu_enter(void)
{
    word_storage_reload_progress_snapshot();
    int word_total = word_storage_get_total_count();
    total_cards_num = (word_total > 0) ? word_total : 1;
    learned_cards_num = word_storage_get_learned_count();
    review_cards_num = word_storage_get_review_count();
}
