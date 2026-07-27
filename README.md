# Reppo OS (ESP32-S3 + LVGL)

## GUI Guider 移植后必做修改

每次从 GUI Guider 重新导出并覆盖 `components/ui_custom/custom/`、`components/ui_custom/generated/` 后，编译前请做以下两处手工修正（Guider 会再次生成，需每次重做）：

### 1. 删除动图 `lv_animimg_set_src` 多余的 `false` 参数

本工程 LVGL 的 `lv_animimg_set_src` 只接受 3 个参数：

```c
void lv_animimg_set_src(lv_obj_t * img, const void * dsc[], uint8_t num);
```

Guider 常会生成第 4 个参数 `false`，导致编译报错 `too many arguments`。请在 `generated/` 下搜索并去掉末尾的 `, false`，例如：

```c
// 错误（Guider 导出）
lv_animimg_set_src(ui->screen_main_cat_left, (const void **) screen_main_cat_left_imgs, 30, false);

// 正确
lv_animimg_set_src(ui->screen_main_cat_left, (const void **) screen_main_cat_left_imgs, 30);
```

可用检索：`lv_animimg_set_src` / `, false)`，常见文件包括 `setup_scr_screen_main.c`、`setup_scr_screen_reppo.c`、`setup_scr_screen_wordcard.c`、`setup_scr_screen_reviewcard.c` 等。

### 2. 删除 WiFi 配置页多余的键盘回调

删除 `components/ui_custom/generated/setup_scr_screen_wificfg.c` 中如下三行（本工程不使用 Guider 顶层键盘，保留会引入未定义/`ta_event_cb` 相关问题）：

```c
#if LV_USE_KEYBOARD != 0 || LV_USE_ZH_KEYBOARD != 0
    lv_obj_add_event_cb(ui->screen_wificfg_ta_password, ta_event_cb, LV_EVENT_ALL, ui->g_kb_top_layer);
#endif
```

完成以上修改后再执行 `idf.py build`。
