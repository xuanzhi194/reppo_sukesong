# Reppo OS (ESP32-S3 + LVGL)

## GUI Guider 移植后必做修改

每次从 GUI Guider 重新导出并覆盖 `components/ui_custom/custom/`、`components/ui_custom/generated/` 后，编译前请做以下手工修正（Guider 会再次生成，需每次重做）：

### 1. 删除 WiFi 配置页多余的键盘回调

必须删除 `components/ui_custom/generated/setup_scr_screen_wificfg.c` 中如下三行。本工程不用 Guider 顶层键盘；保留会导致 WiFi 页显示异常。

```c
#if LV_USE_KEYBOARD != 0 || LV_USE_ZH_KEYBOARD != 0
    lv_obj_add_event_cb(ui->screen_wificfg_ta_password, ta_event_cb, LV_EVENT_ALL, ui->g_kb_top_layer);
#endif
```

### 2. 动图 `lv_animimg_set_src` 多余的 `false`（已兼容，不用手改）

Guider 仍会生成第 4 个参数 `false`。本仓库 LVGL 在 `lv_animimg.h` 已加兼容宏，多余参数会被丢掉，不必再删。

完成 WiFi 键盘删除后再执行 `idf.py reconfigure`（如有新 `.c`）和 `idf.py build`。
