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

## Todo 蓝牙 JSON 与标签

GATT 仍是服务 `0x00FF`、特征 `0xFF01`。校时成功后再同步待办。

下发：`0x02` 单包 JSON 数组，或 `0x03`/`0x04`/`0x05` 分片。拉取：`0x06`，设备 notify
`{"todo":[...],"taskDone":[...]}`。

每条 todo：

```json
{
  "content": "Read ten focused pages",
  "durationStr": "25MIN",
  "done": false,
  "plannedDate": "2026-09-08",
  "label": "study"
}
```

`label` 与扭蛋四类对齐，**可省略**（用户未选分类）：

| 约定值 | 含义 | 扭蛋 |
| --- | --- | --- |
| `work` / `1` / `工作` | 工作 | cat 0 |
| `study` / `2` / `学习` | 学习 | cat 1 |
| `life` / `3` / `生活` | 生活 | cat 2 |
| `fun` / `4` / `娱乐` | 娱乐 | cat 3 |
| 省略 / `""` / `null` | 未分类 | — |

也接受字段名 `category`、`tag`，或 `categoryIndex` 0–3。设备回读时固定输出
`label` 为上述英文小写 token（未分类为 `""`）。

SPIFFS `todo.csv` 列为 `name,duration_min,done,date,label`。旧的四列文件仍能读，缺的标签视为未分类。`screen_todosel` 在任务名上方显示 Work / Study / Life / Fun；未分类不显示。

## screen_game：Dino Run

主页 Game 进入准备画面，点击旋钮开始，运行时点击跳跃。不能在空中再次起跳。
仙人掌随机出现，速度逐渐增加到上限；撞击后点击 RETRY 重试。
HI 是保存在 Flash NVS 中的最高分，开机自动加载。游戏结束、暂停或退出时保存新纪录；
分数未刷新纪录时不写入，失败后保留待保存值并每 5 秒重试。写入放在主任务的 LVGL
互斥锁外，跑动过程中不写 Flash，避免影响跳跃和频繁擦写。游玩中直接断电不会保存
本局尚未结算的新纪录，可先转动旋钮暂停再关机。

转动旋钮暂停；暂停后继续旋转，在 RESUME 和 HOME 之间选择，点击确认。
准备和结束画面也能旋转选择 HOME 退出。当前输入层把实体按压转换成一次 ENTER
脉冲，因此退出不依赖长按，也不修改其他页面的旋钮规则。

灯效为低亮度青绿流水、起跳时向两侧展开的绿光、每 100 分的金色闪光和碰撞红光。
游戏灯会让位最近正在写入的现有灯效；退出只清理由游戏持有的灯帧。
准备、暂停、结束的灯效短暂显示后停止，保留原有自动息屏行为；运行中保持亮屏。

### 模块与 GUI Guider 覆盖

- `main/dino_game.c/h`：不依赖 LVGL 或 ESP-IDF 的物理状态机、碰撞、计分。
- `main/dino_render.c/h`：代码绘制恐龙、仙人掌、场景和菜单，不依赖导出的图片或字体名。
- `main/game_machine.c/h`：页面生命周期、独立焦点组、输入和灯效状态。
- `main/game_score_storage.c/h`：最高分读写，独立 NVS 命名空间 `DinoGame`、`uint32_t` 键 `best`。
  启动时只读加载；UI 回调只更新内存，存储轮询负责提交及失败重试。
- `custom.c/h`：保留 `screen_game_custom_init` / `screen_game_buttion_add` 入口。
  `custom.c` 中是 Guider 预览用的弱符号，固件由 `main/game_machine.c` 的强符号实现玩法。
- `app_main_loop.c`：在现有 LVGL 互斥锁内调用 `game_machine_poll()`，锁外调用
  `game_machine_led_poll()` 和 `game_machine_storage_poll()`，无需新建游戏任务。

Guider 工程应同步本仓库的 `custom.c/h`，保留这两个弱符号入口；生成文件可以正常覆盖。
只要仍导出 `screen_game`，固件页面轮询就能挂接，不需要在 `generated/events_init.c/h`
或 `setup_scr_screen_game.c` 中添加游戏代码。Guider 单独预览显示入口占位；完整玩法在固件中。

所有游戏控件、定时器和输入状态只在主 UI 任务中访问。游戏使用一个绘制对象、一个
33ms LVGL 定时器和自己的焦点组；Guider 清空页面子对象、切屏或删除页面时均停止并释放。
实际切回主页放在事件处理完成后的页面轮询中，避免在点击事件栈内删除当前控件。
物理采用固定 10ms 步长，每帧最多补算 100ms，避免其他工作短暂阻塞后角色或障碍瞬移。

RGB 的新接口使用现有灯带互斥量，获取不到锁就跳帧，不在 LVGL 锁内调用灯带驱动。
200ms 的现有灯效优先窗口属于近期写入保护，不保存或恢复其他页面的静态灯帧。

### 游戏回归检查

```sh
python3 -B -m unittest discover -s scripts/tests -p 'test_dino_game.py' -v
python3 -B -m unittest discover -s scripts/tests -p 'test_game_led.py' -v
python3 -B -m unittest discover -s scripts/tests -p 'test_game_machine.py' -v
idf.py build
```

前两项覆盖真实物理核心和灯带驱动；第三项编译工程自带 LVGL，以模拟按键和软件显示
检查页面反复进入/退出及 Guider 的先清空、后延迟切屏行为，并启用内存/未定义行为检查。
可设置 `REPPO_GAME_SNAPSHOTS` 为输出目录，保存准备、跳跃、暂停、结束画面的 PPM 截图。
实机仍需检查旋钮手感、帧率和灯效亮度；这些主机测试不会连接或烧录设备。
