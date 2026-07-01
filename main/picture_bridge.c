/**
 * screen_picture 与 badge 存储的桥接（放在 main，不放入 ui_custom，便于 GUI Guider 移植）
 */
#include "badge_storage.h"
#include "gui_guider.h"
#include "esp_log.h"
#include "lvgl.h"

static const char *TAG = "picture_bridge";

void picture_bridge_init(void)
{
    /* no-op: 使链接器拉入本翻译单元，覆盖 custom.c 中的弱符号 */
}

void picture_bridge_screen_on_load(void)
{
    if (badge_storage_has_image()) {
        ESP_LOGI(TAG, "load badge image: %s", BADGE_LVGL_PATH);
        lv_img_set_src(guider_ui.screen_picture_img_picture, BADGE_LVGL_PATH);
    } else {
        ESP_LOGI(TAG, "no badge image, show placeholder");
        lv_img_set_src(guider_ui.screen_picture_img_picture, &_word_BG1_alpha_320x170);
    }
}
