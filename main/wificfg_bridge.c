/**
 * WiFi 配网页与 NVS 凭据的桥接（放在 main，不放入 ui_custom，便于 GUI Guider 移植）
 */
#include "custom.h"
#include "gui_guider.h"
#include "wifi_cred_storage.h"
#include "lvgl.h"

void wificfg_bridge_init(void)
{
    /* 空函数：在 app_main 调用以保证本翻译单元参与链接 */
}

void wificfg_on_list_item_clicked(lv_obj_t *list_item)
{
    if (list_item == NULL || guider_ui.screen_wificfg_list_1 == NULL) {
        return;
    }

    const char *ssid = lv_list_get_btn_text(guider_ui.screen_wificfg_list_1, list_item);
    char saved_pw[WIFI_CRED_PASS_LEN] = {0};
    bool has_saved = false;

    if (ssid != NULL && ssid[0] != '\0') {
        has_saved = wifi_cred_get_password(ssid, saved_pw, sizeof(saved_pw));
    }

    wificfg_show_connect_ui(list_item, has_saved ? saved_pw : NULL, has_saved);
}
