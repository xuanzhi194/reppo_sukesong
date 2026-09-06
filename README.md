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

## Reppo Zero 手机自动校时

设备以手机的**本地墙钟**为准，RX8130 直接保存年月日时分秒，不转换 UTC，
也不推测时区。每次 App 建立 BLE 连接后主动同步一次，包括断开后重连。
WiFi 设置页继续保留、可访问；目前 WiFi 不会通过 NTP 改写 RTC。

### App 连接顺序

1. 连接设备，发现服务 `0x00FF` 和特征 `0xFF01`，安装通知处理并开启 notify。
   若 App 需要协商 MTU，可在此阶段完成；校时包只有 8/9 字节，无需扩大 MTU。
2. **准备实际写入时才读取手机当前本地时间**，向 `0xFF01` 写入 `0x10 TIME_SET`。
   不要使用扫描或建立连接之前缓存的时间。
3. 等待 `0x11 0x00`，表示 RTC 已写入且恢复走时，再执行 `0x06` 拉取 Todo 等操作。
   GATT write 成功只代表收包成功，不等于 RTC 已同步。
4. 收到 `0x11 0x01` 或等待结果超时（测试脚本为 5 秒）时，不标记同步成功；
   重试必须重新读取手机时间。同一连接一次只发一条校时请求；上一条尚未完成时，
   新的写入会返回 ATT 资源不足错误，应等上一条结果或重新连接后重试。

固件不会为了等手机而停止原有 RTC。收到有效时间后，主循环写入 RX8130，
随后刷新主页、timing 和 Keep 日期状态；断开连接后 RTC 继续自行走时。
通知暂不可用时，固件保留写入结果并重试；断开连接会清除该连接待处理的时间和通知，
正在写入的旧连接结果不会作为新连接的同步确认。

### `0x10 TIME_SET` 格式

| 字节 | 内容 |
| --- | --- |
| 0 | `0x10` |
| 1–2 | 年，小端，支持 2000–2099；2026 为 `0xEA 0x07` |
| 3 | 月，1–12 |
| 4 | 日，按实际月份和闰年校验 |
| 5 | 时，0–23 |
| 6 | 分，0–59 |
| 7 | 秒，0–59 |
| 8（可选） | 星期，0=周日 … 6=周六；设备最终按年月日计算星期 |

包含命令字的总长度必须为 8 或 9 字节。返回格式为 `0x11 status`，
`status=0` 表示成功，`status=1` 表示格式/日期错误或 RTC 写入失败。

### 本机验证

`python3 scripts/ble_send_phone_time.py` 使用运行脚本的电脑本地时间模拟手机下发，
需要安装 `bleak`。脚本会等到 RTC 成功确认后才报告成功。

无需设备的回归检查：

```sh
python3 -m unittest discover -s scripts/tests -p 'test_*.py' -v
```

实机联调应检查首次连接、断开重连、校时后断开仍走时、跨日同步及错误时间包。
