# Reppo OS (ESP32-S3 + LVGL)

Reppo R2 是一块 ESP32-S3 桌面终端：320×170 横屏，EC06 旋钮。固件在 `main`，界面在 `components/ui_custom`。远程仓库是 `reppo_r2`。

比赛版已预备思必驰 ASR 接入，保留通义 ASR 备用。默认仍使用通义，拿到授权后按 [接入与测试说明](docs/aispeech_asr.md) 配置服务器凭证、设备代理地址并切换。

当前主链路：

- 主页进入待办、专注、阅读、游戏和设置。设置里可以切换中英文，选择记在 NVS。待办内容和 WiFi 名称始终可以显示中文。
- 时间以 RX8130 为准。WiFi 连上后用 SNTP 对北京时间（`CST-8`）并写回 RTC；手机 BLE 连接后仍会下发本地时间。
- 电量由 CW2015CHBD 读取，和 RTC、IMU 共用 I2C0。低电告警 ALRT# 接 GPIO8。充电状态仍看 TP4056 的 CHRG / STDBY。
- IMU（LSM6DSR）和 RTC 共用 I2C0。拿起唤醒、番茄钟晃动停闹仍然保留。语音改为合加速度大于 3g 的一下敲击；游戏页打开时不响应这次敲击。
- 游戏页先用左右倾斜选择 Dino Run 或 Tilt Orb，再进入对应玩法。两个最高分都在 NVS 命名空间 `DinoGame`。

## GUI Guider 移植后必做修改

不要改 `components/ui_custom/generated/`。这个目录留给 GUI Guider 整份覆盖。界面上的自定义逻辑放在 `components/ui_custom/custom/`，和界面无关的固件放在 `main/`。这样重新导出后，只要把下面这一处手工修正补回去，两边就能对接。

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

## 扭蛋随机任务：跟随界面语言

中文模式从工作、学习、生活、娱乐四类中文任务池抽取，每类 32 条，共 128 条，
例如“给项目写一句电影预告”“把一个知识点画成表情包”“喝口水给身体续个杯”
和“给橄榄树写一句出道宣言”。英文模式继续使用原有四类、每类 100 条英文任务。
抽取时读取当前语言；任务文字使用支持中文的字体，较长内容仍循环滚动显示。
点击“加入待办”会保存抽到的原文及对应分类，已有待办不会随界面语言重新翻译。
任务池在 `main/gash_tasks.h`，选择和显示逻辑在 `main/gash_machine.c`，不修改生成目录。

## Keep 比赛演示：左右倾斜预览等级

Keep 使用现场展示模式：**右倾立即下一级，左倾立即上一级**，1–10 级循环。
无需入页校准、保持手势或回正确认；进入页面时已经倾斜，也会在第一个有效采样时切换。
持续同侧倾斜，每 600ms 继续切一级；反向倾斜立即换方向，不受连续切换计时限制。
停止倾斜就停止连续翻图。方向沿用游戏页的 X 轴，约 6° 触发，4° 内停止，
只保留一个小防抖区。底部提示“右倾：连续下一级”或“左倾：连续上一级”。
明显冲击／无效采样不切换，下一帧有效倾斜立即恢复，不要求额外动作。

预览时图片、等级名称一起更新，同时轮到下一句 AI 总结。右下角显示 `DEMO n/10`，进度条表示预览等级位置。
健康／枯萎状态仍按实际养护记录显示。双击旋钮返回，重新进入恢复真实等级；演示不会修改
累计专注时长、养护日期或 NVS。没有 IMU 时仍可正常查看 Keep，息屏时暂停手势，唤醒后即可继续倾斜展示。

界面与提示位于 `main/keep_machine.c`，纯手势状态机和角度阈值位于 `main/keep_tilt.c`，
复用现有 IMU 缓存和 LVGL 主循环，不改生成目录，不新增采样任务或 I2C 读取。
IMU 缓存使用临界区复制完整样本，附带序号和时间戳，超过 250ms 未更新则标记无效。
Keep 只处理新采样，用样本时间控制连续切换间隔，避免 UI 帧率改变翻图速度。
主机测试检查首次采样立即切换、100 轮直接反向、600ms 连续切换、重复帧、
防抖区、冲击后立即恢复、时钟回绕，以及 AI 单／双击和页面生命周期。

```sh
python3 -B -m unittest discover -s scripts/tests -p 'test_keep_machine.py' -v
python3 -B -m unittest discover -s scripts/tests -p 'test_keep_tilt.py' -v
idf.py -B build-asr build
```

实机仍需检查左右方向、倾斜手感及旋钮单／双击。测试和编译不会自动烧录设备。

VS Code 的 ESP-IDF 构建／烧录目录已通过 `.vscode/settings.json` 的 `idf.buildPath`
统一为 `${workspaceFolder}/build-asr`。普通 `build/` 是旧项目复制过来的产物，不要选择其中的
固件烧录。倾斜切换时串口会输出
`keep: tilt demo level=... direction=...`。

### Keep 的 AI 个人总结

橄榄树对话已移除固定句子库。进入 Keep 并联网后，通过现有通义 `qwen-flash` 文本接口生成
三句轻松幽默的个人近况短评，沿用 `main/voice_todo.c` 的现有凭证，不需要新服务器或思必驰。
仅发送摘要所需的文本样本：当前未完成待办最多 12 条、已勾选待办最多 12 条、最新历史归档
最多 12 条、最近 3 条灵感（标题和整理正文，无整理正文时用原话），以及各类总数、记录日期、
累计专注分钟和当前界面语言。不会上传录音或把全部历史算成“今天”。

短评在对话框每 7 秒轮换，倾斜换等级也会切到下一句；中文使用通用汉字字体，较长文字自动滚动。
**单击旋钮手动刷新，双击旋钮返回主页。** 单击在 450ms 双击判定窗口结束后发起新总结，
跳过自动刷新的 30 秒间隔及 10 分钟内容缓存，即使记录没变也重新请求 AI。
双击不会先发送刷新请求；手动生成时显示“正在重新总结”。已有生成任务时重复单击不叠加请求，
仍可双击退出；联网和模型响应时间仍然需要等待。切屏、删除页面或息屏会取消未判定的单击。
后台采集及请求不持有 LVGL 锁，不影响操作。不因倾斜而发起新请求；记录变化后重新生成，
采集／失败重试最短间隔 30 秒，样本及总数不变时复用最多 10 分钟的开机内缓存。
正常回复直接显示正文，不加“AI”前缀。缺网或失败时有缓存则显示 `缓存`，无缓存则显示联网／重试提示；不伪装成已生成的总结。
缓存不写入 Flash，重新开机需要联网生成。样本之外的文字变化若未改变总数，可能要等缓存到期更新。

`main/keep_ai.c` 负责数据组装、缓存和后台任务；`inspiration_service.c` 为总结提供独立回复队列，
与灵感本读取互不抢消息；Done 直接在后台带锁读取归档，不依赖是否打开过 Done 页面。
`main/voice_todo.c` 提供复用的文本调用，生成失败不修改任何待办、完成状态或灵感。

```sh
python3 -B -m unittest discover -s scripts/tests -p 'test_keep_ai.py' -v
python3 -B -m unittest discover -s scripts/tests -p 'test_keep_machine.py' -v
python3 -B -m unittest discover -s scripts/tests -p 'test_inspiration_ui.py' -v
```

实机验收：联网进入 Keep 等待短评；新增待办、完成／归档一项、保存灵感后回到 Keep，至少间隔
30 秒观察自动更新，或单击旋钮立即刷新。断网应保留并标明缓存，重启断网应显示联网提示。串口 `keep_ai: summary ready`
表示本次生成成功，`cached` 表示复用，`unavailable` 表示数据读取或模型调用失败。

## Reppo Zero 手机自动校时

设备以手机的**本地墙钟**为准，RX8130 直接保存年月日时分秒，不转换 UTC，
也不推测时区。每次 App 建立 BLE 连接后主动同步一次，包括断开后重连。
WiFi 设置页继续保留。WiFi 已连接时，固件还会用 SNTP 对北京时间并写入同一颗 RTC；
两条路径都以最后一次成功写入为准。

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

语音待办由一下大于 3g 的敲击打开。识别后的日期默认是 RTC 上的今天；话里说到时长才写入分钟，没说到就留空。番茄钟闹钟、专注计时或游戏页打开时，这次敲击不会进入语音。

## screen_game：选关、Dino Run 和 Tilt Orb

进入游戏页先到选关。左右倾斜在 Dino Run 和 Tilt Orb 之间切换，旋钮在 PLAY 和 HOME 之间选择，按下确认。停留在这一页时，大于 3g 的语音敲击不起作用。

Dino Run：点击跳跃，不能在空中再次起跳。仙人掌随机出现，速度逐渐增加到上限；撞击后点击 RETRY。转动旋钮暂停；暂停后继续旋转，在 RESUME 和 HOME 之间选择。

Tilt Orb：倾斜机身推动光球穿过从右侧驶来的光门，门缝里的点可以吃。转动或按下旋钮暂停，暂停后同样在 RESUME 和 HOME 之间选择。

两个游戏的 HI 都在 NVS 命名空间 `DinoGame`：恐龙用 `best`，光球用 `tilt`。结束、暂停或退出时保存新纪录；分数没有超过纪录时不写入，失败后保留待保存值并每 5 秒重试。写入放在主任务的 LVGL 互斥锁外，游玩过程中不写 Flash。直接断电不会保存本局尚未结算的新纪录，可先暂停再关机。

当前输入层把实体按压转换成一次 ENTER 脉冲，因此退出不依赖长按，也不修改其他页面的旋钮规则。

灯效为低亮度青绿流水、恐龙起跳时向两侧展开的绿光、光球高度对应的灯位、里程碑闪光和碰撞红光。游戏灯会让位最近正在写入的现有灯效；退出只清理由游戏持有的灯帧。准备、暂停、结束和选关的灯效短暂显示后停止，保留原有自动息屏行为；运行中保持亮屏。

### 模块与 GUI Guider 覆盖

- `main/dino_game.c/h`、`main/tilt_game.c/h`：不依赖 LVGL 或 ESP-IDF 的物理状态机、碰撞、计分。
- `main/dino_render.c/h`、`main/tilt_render.c/h`：代码绘制场景和菜单，不依赖导出的图片或字体名。选关画面也在 `tilt_render.c`。
- `main/game_machine.c/h`：选关、两个游戏的生命周期、独立焦点组、IMU 倾斜、输入和灯效状态。
- `main/imu_motion.c/h`：26 Hz 采样。游戏读取最近一次加速度；语音在合加速度大于 3g 时进入，游戏页打开时跳过。
- `main/game_score_storage.c/h`：最高分读写，NVS 命名空间 `DinoGame`，键 `best` 和 `tilt`。
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
python3 -B -m unittest discover -s scripts/tests -p 'test_tilt_game.py' -v
python3 -B -m unittest discover -s scripts/tests -p 'test_game_led.py' -v
python3 -B -m unittest discover -s scripts/tests -p 'test_game_machine.py' -v
idf.py build
```

前三项覆盖两套物理核心和灯带驱动；`test_game_machine.py` 编译工程自带 LVGL，以模拟按键、倾斜采样和软件显示检查选关、反复进入/退出，以及 Guider 的先清空、后延迟切屏，并启用内存/未定义行为检查。可设置 `REPPO_GAME_SNAPSHOTS` 为输出目录，保存选关、光球、跳跃、暂停和结束画面的 PPM 截图。实机仍需检查旋钮手感、倾斜方向、帧率和灯效亮度；这些主机测试不会连接或烧录设备。


## 灵感闪记：共享语音入口与本地灵感本

一次大于 3g 的敲击打开共享入口，旋钮选择「记待办」「记灵感」「看灵感」，按下确认。
本次开机内记住上次录音模式。设置页的滚动列表也会追加「灵感闪记」，可直接离线回看，
不依赖敲击或 IMU。专注运行、番茄钟闹铃、游戏及语音页面打开期间仍保留原有敲击屏蔽。

灵感模式按「开始说」后，听到提示音再讲话。最长录音 15 秒，讲话后的连续约 1.2 秒
安静会提前结束；待办模式仍为 4 秒、约 0.6 秒静音结束。页面显示实测麦克风音量、
录音进度及秒数。识别和 AI 整理需要 WiFi，历史回看不需要联网。麦克风只在录音时启用，
识别完成后即释放 WAV，不让音频缓冲占着后续整理阶段的内存。

AI 整理灵感时保留原意、语气与细节，输出短标题和正文，不自动转换为待办。原始转写与
整理正文都保留，正文最多约 240 个字符。若整理请求失败，仍可预览和保存识别到的原话。
预览页提供「全文」「保存」「重说」「返回」，只有后台确认写入成功后才显示「已存入灵感本」。
写入失败或空间不足时保留当前草稿；返回入口可选「继续」，也可以先回看并删除旧记录后再保存。
草稿仅在 RAM 中，尚未确认保存时断电不会保留。

回看列表按最新顺序每页显示 3 条；旋钮选择，按下查看。详情页旋钮滚动正文，单击切换
原始转写/整理正文，双击返回。列表「删除」针对最近选中的记录，确认页显示标题；
选择「保留」不会删除。删除后会复用空槽，不自动覆盖旧灵感。

### 存储、容量与更新

- 文件：共享 SPIFFS 的 `/spiffs/inspirations.dat`；不占 Todo 列表或 NVS done 标记。
- 上限 500 条，每条固定 1632 字节，全部正文占 816000 字节（约 0.78 MiB，不含文件系统开销）。
- 每条包含 20 字节时间、64 字节标题、768 字节原话、768 字节正文，以及版本标记/序号/CRC。
  文本按完整 UTF-8 字符截断，时间未校准时留空，不制造假日期。
- 只保留偏移索引、当前页和详情缓存；大缓冲、队列载荷及索引放在 PSRAM。
  独立存储任务负责读写，不在 LVGL 锁内访问文件。
- 保存前检查共享分区余量，至少保留约四分之一给文件系统回收及其他数据；因此已有
  电子书/待办占用过大时，可能未到 500 条就提示空间不足。
- CRC 校验、先写正文后提交头部；启动忽略不完整/损坏记录，保留其他可读记录。
  文件系统本身的损坏仍需要实际硬件恢复与验证。

普通 `idf.py flash` 现在默认不刷 `storage`，避免固件更新覆盖灵感、待办和导入的电子书。
网页 app-only 烧录方式仍适用。只有首次出厂准备需要初始化样例数据时，显式使用：

```sh
idf.py -DREPPO_FLASH_FACTORY_STORAGE=ON reconfigure
idf.py build flash
# 出厂初始化完成后切回普通更新配置，避免以后误刷用户数据：
idf.py -DREPPO_FLASH_FACTORY_STORAGE=OFF reconfigure
```

### 模块与验证

`main/inspiration_storage.c` 管理固定记录、校验、排序及删除；
`main/inspiration_service.c` 管理后台请求/回复；`main/inspiration_ui.c` 以 LVGL 代码创建入口、
列表、详情、预览和确认页面，不修改 Guider 生成目录。
`main/voice_todo.c` 复用现有录音、识别和动画，分别处理待办与灵感提示词。

```sh
python3 -B -m unittest discover -s scripts/tests -p 'test_*.py' -v
# 可选：保存真实 LVGL 渲染的中英文页面、录音及整理状态 PPM 截图
REPPO_INSPIRATION_SNAPSHOTS=/absolute/output/dir \
  python3 -B -m unittest discover -s scripts/tests -p 'test_inspiration_ui.py' -v
idf.py build
```

主机测试覆盖重启恢复、容量、UTF-8 边界、半条写入、CRC 损坏、删除复用，以及真实 LVGL
输入、翻页、滚动、原话切换、保存失败、延迟回复和反复进出。实机需验证麦克风收音、
15 秒录音与 TLS 的连续内存余量、云端响应和旋钮手感；串口会打印录音前内部堆/PSRAM
剩余量及最大连续空闲块。

## 领导黑话翻译（比赛演示）

入口：**设置 → Boss Trans**。先连 WiFi，进入后自动建立通义实时语音连接，
边说边显示原话，停顿约 1.1 秒后得到完整句子并翻译。左侧像素老板有眨眼、说话、举杯四帧，
麦克风电平驱动说话动画。屏幕显示“请说话／翻译中／看翻译”和连接、暂停、重试状态及电平条；此页保持亮屏。
**单击旋钮暂停／继续，双击返回主页**；双击判定窗口 450ms，第一下不会立即暂停。
退出、页面删除或其他导航均通知收音任务停止，麦克风在当前读取结束后关闭；
网络在后台释放，界面不会等待。旧会话的 AI 回复不会写入重新打开的页面。

- 先演示“公司就是你的家”“这个需求就改一点点”“我们复盘一下”“还是第一版好”。
  本地词库有 40 组梗（另有一个识别别名），按关键词匹配，标点和语气词变化不影响这些例句。
  一句包含多个词条时优先翻译最先出现的词条。效果提示是娱乐梗，不是真实预测。
- 不在词库中的原话通过现有 `qwen-flash` 生成“短吐槽＋游戏系统式效果提示”两行。生成期间保持连接和收音上传，但跳过新句子的转写；
  翻译完成后保留约 4 秒，长回复 6 秒，再恢复接收新句。期间插入的话不排队；请等“请说话”再说下一句。
  本版本展示当前一句，不保存音频／转写历史、不写待办或灵感，也不识别说话人身份。
- 网络中断、额度／权限错误或音频发送拥堵会停止收音并显示错误；单击重试。
  暂停时停止上传，已发送的请求可能仍由云端处理，但回复不会覆盖新会话。
- 只在此页用 `qwen3-asr-flash-realtime` WebSocket（16kHz、单声道 PCM16、服务端 VAD），
  复用 `voice_todo.c` 内现有通义凭证，不部署业务服务器，不改变旧的待办 ASR 或思必驰开关。
  实时识别及陌生句子的文本翻译使用对应模型额度。
- `main/boss_ui.c` 是页面与按键，`boss_service.c` 是收音／WebSocket／翻译工作任务，
  `boss_rules.c` 是例句词库。麦克风任务、网络任务分开，用约 6 秒 PSRAM 缓冲吸收发送抖动；
  缓冲溢出会明确报错而不悄悄漏音。新旧语音入口相互排斥。
- `assets/boss/boss_sheet.png` 是参考图生成的四帧素材；
  `scripts/import_boss_sprite.py` 机械转换为 `custom/assets/boss_sprite.c`，四帧共约 68KiB。
  本功能未修改 `components/ui_custom/generated/`。

验证：

```sh
python3 -B -m unittest discover -s scripts/tests -p 'test_boss*.py' -v
idf.py -B build-asr build
# 可选云端验证（会调用通义，音频必须为 16kHz/单声道/16-bit WAV）：
# 在你的 Python 环境安装 websocket-client 和 certifi 后执行：
python3 scripts/test_boss_realtime.py /path/to/test.wav --use-firmware-key
```

主机测试覆盖 40 组例句、两行 AI 输出解析、UTF-8 截断、真实 LVGL 页面、暂停／恢复、双击、反复进出和断网重试；
同时验证 WebSocket 分片重组、中间转写拼接、旧会话／旧句子的隔离和接收缓冲溢出。
云端已使用合成测试音频验证 WebSocket 鉴权、中间转写与完整句子，陌生句子的文本翻译返回 HTTP 200；
不代表已完成板上收音测试。当前固件应用分区剩余约 203KiB（2%），后续新增大图需注意空间。
烧录继续用 VS Code 的 ESP-IDF 按钮（`build-asr`），普通更新保留原有用户存储。
实机验收时连续说两句，确认都有字幕／吐槽；再试单击暂停、双击退出、重新进入、断网与恢复，
确认退出后仍能正常使用原来的语音待办。当前功能面向中文演示，中英文设置下入口统一为 Boss Trans。

接口依据：[通义实时 ASR 连接指南](https://help.aliyun.com/en/model-studio/qwen-asr-realtime-interaction-process)、
[客户端事件](https://help.aliyun.com/en/model-studio/qwen-asr-realtime-client-events)、
[服务端事件](https://help.aliyun.com/en/model-studio/qwen-asr-realtime-server-events)。

### Boss 语音内存调整

Boss 音频环形缓冲改用 `xStreamBufferCreateStatic`：32,001 字节载荷显式放入 PSRAM，
保留 32,000 字节（一秒 PCM16）的可用容量；包含锁的控制结构仍在内部 RAM。
录音、发送、Base64 和接收 JSON 缓冲也明确放在 PSRAM，合计约 68KiB，
避免默认 `malloc` 对不大于 16KiB 的请求优先挤占内部 RAM。
WebSocket 收发缓冲从各 4KiB 缩为各 2KiB；完整消息仍由 16KiB 接收区重组。
AI 文本翻译任务的 12KiB 栈通过 ESP-IDF WithCaps API 放入 PSRAM；该任务仅做网络／文本处理，
不得新增 Flash/NVS 写入。网络发送、WebSocket、采集任务栈以及 I2S DMA、RTOS 控制结构仍放在内部 RAM。
页面切换完成、旧页面释放后才启动连接，避免新旧页面同时占内存。不缩减原有任务栈尺寸。
退出／失败均释放上述会话缓冲，重试不累积。主机测试覆盖低内部内存预算、100 次创建／释放及分配失败清理；实机效果仍需烧录验证。串口 `boss_asr` 日志增加分配阶段、
内部堆／PSRAM 的剩余量和最大连续块，不输出凭证和识别内容。

`partitions.csv` 只修正文档说明，分区地址和容量保持原样。它分配的是 Flash；
改变文件系统位置不能增加运行时 RAM，还可能影响原有数据。

2026-10-31 实机日志复查：大缓冲迁移已生效，`buffers_ready` 不再消耗内部堆；
但首次 WebSocket 任务创建失败。后续连接进入 DNS 解析阶段，反复报
`ESP_ERR_ESP_TLS_CANNOT_RESOLVE_HOSTNAME`，同时 WiFi RSSI 为 -91dBm。
新增连接前 RSSI／主备 DNS 和任务启动前后内存日志，页面区分 DNS、鉴权和证书错误。
保留 DHCP 下发的 DNS，不硬编码服务 IP，不绕过 TLS 校验。现场先用靠近设备的 2.4GHz 手机热点
验证；拿到局域网 IP 不代表该网络能访问互联网。任务栈与切屏时序调整需重新烧录后验证。

### boss-dma3：DMA 预留和版本确认

此历史版本标识为 `firmware=boss-dma3`（当前为 `hey-reppo8`）。若没有版本日志，
并且也没有 `network rssi`／`before_websocket_start`，先确认烧录的是当前项目的 `build-asr` 新产物。
该版本先创建采集任务并初始化麦克风，再启动 WebSocket/TLS；连接期间在本机读取并丢弃音频，
收到 `session.updated` 后才入队上传。失败／退出仍由采集任务关闭麦克风，网络任务等待其退出后释放会话。
Boss 使用专用 4×128 帧 DMA（双声道 32-bit，4096 字节，16kHz 时约 32ms），
采集任务持续读取后送入原有一秒 PSRAM 缓冲。`zts6672_init()` 的原 4×256 帧配置保留，
原待办／灵感录音不切换 DMA 配置。4KiB 配置需实机观察高负载时是否掉音。

在本项目目录执行一次构建并烧录，可避免只烧到上次编译的文件：

```sh
idf.py -B build-asr -p /dev/cu.usbserial-10 build flash
```

编译／主机测试不能替代实机验证；确认版本后观察 `mic_ready`、`before_websocket_start`、
证书校验和识别结果，若仍报错保留本次完整 Boss 日志。

### boss-net4：容忍短暂网络抖动

此历史版本标识为 `firmware=boss-net4`（当前为 `hey-reppo8`），保留 dma3 的先初始化麦克风和 4KiB DMA 配置。
启用官方 WebSocket 组件的 `CONFIG_ESP_WS_CLIENT_SEPARATE_TX_LOCK`，
接收中间转写分片时不再占用应用发送音频所用的同一把锁；配置同时写入 sdkconfig 和默认配置。
PSRAM 音频队列由 1 秒扩为 6 秒（192,000 字节），新增约 156KiB PSRAM，
不扩大内部 RAM/DMA。音频发送允许最多 2 秒的锁等待／单次底层写等待。
缓冲没有固定等待时间：有音频就立即发送，网络恢复后按原顺序尽快发送积压数据。

积压达到 1 秒显示“网络波动 · 语音缓冲中”，降至 200ms 以下恢复正常提示；
仅缓冲持续积满或实际发送失败才停止，不丢帧、不重发可能已部分发送的 WebSocket 消息。
这能吸收短暂抖动，但持续低于音频产生速率的网络仍会耗尽缓冲；积压期间字幕会延迟。
串口增加 `slow audio send`、`audio buffering`、`audio buffering recovered` 和缓冲满日志，
记录耗时／队列深度，不记录音频、识别内容或凭证。当前重新烧录后确认 `hey-reppo8` 再连续演示。

主机采集测试模拟连续三次各 3 秒的发送停顿及加速排空，检查全部 240 个音频块按原顺序恢复；
同时检查持续堵塞的容量上限、缓冲提示恢复，以及连接错误不会被后续收音提示覆盖。
这些是主机模拟结果，仍需在现场设备和网络上验证。

### boss-show5：对话气泡与吐槽演出

此历史版本标识为 `firmware=boss-show5`（当前为 `hey-reppo8`），保留 net4 的网络缓冲和 dma3 的麦克风内存配置。
页面采用暖纸色背景、老板旁的白色原话气泡、深绿色“潜台词”卡片和橙色效果提示条。
收到完整翻译时卡片轻弹，老板举杯，约 500ms 后弹出效果提示，形成原话→吐槽→效果的节奏。
动画只改变位移，不创建额外透明图层缓冲。网络告警优先于娱乐提示显示。
原话和翻译每页两行，长文本每 2.8 秒自动翻页；翻译右上角显示页码，避免长回复被截断。

新增 20 组本地梗并改写部分原有文案，例如：

| 老板说 | 潜台词 | 效果提示 |
| --- | --- | --- |
| 公司就是你的家 | 所以你下班准备去哪？ | 回城键已禁用 |
| 这个需求就改一点点 | 顺便把地基往左挪两米。 | 项目进入装修模式 |
| 我们复盘一下 | 请各位打开背锅二维码。 | 正在匹配责任人… |
| 还是第一版好 | 恭喜你，完成了一次付费为零的时间旅行。 | 已绕地球一圈回到原点 |

陌生句子的 AI 回复使用相同的两行格式，缺少效果提示时显示“潜台词解码完成”。
主机测试检查效果延迟、网络提示优先级、新转写清除旧效果、两行换行、UTF-8 分页及暂停/退出行为；
真实 LVGL 渲染预览见 `assets/boss/ui_preview_show5.png`。AI 两行输出已用云端请求验证。
最终观感、现场网络和真实收音仍需烧录到设备验收；本轮未自动烧录。

### boss-pace6：先翻译、看完再接下一句

此历史版本标识为 `firmware=boss-pace6`（当前为 `hey-reppo8`）。服务端断句静音由 600ms 调到 1100ms，
VAD 阈值从 0.0 调到 0.2，减少自然停顿断句及环境声音触发。配置范围与含义见上面的通义客户端事件文档。
这一改动会增加约半秒的断句等待，适合逐句现场演示。

收到完整句子后锁住当前原话：本地命中直接展示，陌生句子等待 AI 完成，期间不让新字幕替换旧句。
回复完成后从完成时刻保留 4 秒，超过 26 个 Unicode 字符的回复保留 6 秒，给翻页和效果提示留时间。
左侧显示“翻译中 → 看翻译 → 请说话”。未开始下一句时保留上一条结果，不清空屏幕。
展示期间收到的新句子直接跳过，不排队；同一句后续的最终转写即使在展示结束后到达也跳过。
麦克风和 WebSocket 仍持续运行、上传，避免频繁重连；单击暂停才关闭收音上传。

AI 最长等候 20 秒，超时显示重试提示并在阅读时间后恢复；迟到回复不能覆盖下一轮。
退出、暂停、连接错误仍优先处理。主机测试覆盖连续插话、跨展示窗口的最终转写、
完成后才开始阅读计时、长回复、超时／失败恢复，以及停止后的旧回复隔离。
本轮不修改 generated、分区或原待办／Keep 逻辑，不自动烧录；需现场观察 0.2 阈值对轻声讲话的表现。

## Hey Reppo：联网语音唤醒与操作（hey-reppo8）

在**设置 → Hey Reppo**中单击切换开关；按钮名称固定，默认关闭，状态保存到 NVS，重启保留。
选中时在右侧矩形详情区显示滑动开关、当前状态、唤醒方法，以及“开启后持续上传语音”的说明。
设置项聚焦和切换不再弹出遮挡列表的浮层，切到其他设置项后详情自动隐藏。本版是**云端识别唤醒**：开启且页面允许时，
麦克风音频持续发往现有通义实时 ASR，依赖 Wi-Fi 并消耗实时识别额度；不是离线唤醒词模型。
没有另外部署服务器或引入大模型文件，也没有增加第二套常驻录音任务。
页面右上角显示“Hey · 待唤醒／连接中／离线”；未联网不会声称已就绪。

说 **“Hi”或“嗨”**（也保留“Hey Reppo”）后，屏幕弹出“我在听”，12 秒内说指令；也可把唤醒词和指令连成一句。
普通息屏时仍可监听并唤亮屏幕。中间字幕用于唤醒和反馈，**仅完整句子才触发操作**；
一句只执行一个受支持的操作。“取消／算了”退出本次唤醒，超时自动收起。
兼容 ASR 的有限近音写法（如 Hey Rapple、嘿 Ripple、嘿瑞波），只匹配句首唤醒，不匹配聊天中间提到的名称。

| 示例指令（前面说 Hi／嗨，也兼容 Hey Reppo） | 行为 |
| --- | --- |
| 帮我开始二十五分钟的专注番茄钟 | 进入专注页并启动 25 分钟倒计时 |
| 开始5分钟专注 | 启动 5 分钟倒计时；允许 1–180 分钟整数，中文数字也支持 |
| 进入专注页面 | 只打开页面，不开始计时 |
| 进入领导黑话翻译功能 | 打开 Boss Trans 并开始该页监听 |
| 打开待办／已完成／Keep／扭蛋／阅读／游戏／设置／相册／答案之书／查看时间 | 进入对应页面 |
| 打开灵感闪记 | 进入语音待办／灵感入口，后续仍用原有操作 |
| 返回主页 | 返回主界面 |
| 关闭语音唤醒 | 关闭开关并持久保存 |

当前是确定的指令集，不是任意任务代理：未知指令会提示示例，不执行删除、批量完成等操作。
启动专注不绑定旧的待办任务，因此完成后只按原计时逻辑累计专注时间，不误勾旧任务。
计时启动沿用现有 Start 按钮行为（包括专注黑屏）；本版计时进行中／暂停中让出监听，
暂不支持语音暂停或结束番茄钟，请用原旋钮操作。

**麦克风交接与页面边界：**Boss、待办录音、灵感页面、游戏、已开始的计时、密码／任务编辑页面会暂停助手。
退出这些功能回到支持页面后自动恢复。敲击仍能打开原语音入口，不会被常驻监听拦住。
跳转前先停止助手上传并等待连接与麦克风释放；Boss 请求也等待麦克风空闲，避免重复申请 I2S/DMA。
待交接时用旋钮切页会取消语音操作。网络失败按 5 秒／30 秒退避重试；关闭开关后不再重连。

`main/hey_reppo_intent.c` 是有界指令解析和唤醒状态机；`main/hey_reppo.c` 负责开关、浮层、交接和导航。
NVS 写入在现有内部栈 NVS 工作任务执行，语音回调不操作 LVGL/Flash。
`boss_service.c` 增加仅 ASR 回调模式，助手不启动 Boss 的 AI 翻译任务；Boss 原有 4/6 秒展示保护保留。
助手使用 0.0 VAD 阈值与 900ms 断句，Boss 保留 0.2／1100ms。
未改 `components/ui_custom/generated/`、Flash 分区或现有用户数据。

验证与烧录（在本项目 ESP-IDF 终端运行）：

```sh
python3 -B -m unittest discover -s scripts/tests -p 'test_hey_reppo*.py' -v
python3 -B -m unittest discover -s scripts/tests -p 'test_boss*.py' -v
idf.py -B build-asr build
idf.py -B build-asr -p /dev/cu.usbserial-10 flash
```

新版本日志为 `firmware=hey-reppo8 mode=assistant` 或 `mode=boss`。
主机测试覆盖默认关闭、持久开关、未唤醒不执行、部分字幕纠正、单次执行、数字边界、取消/超时、
真实 LVGL 浮层、麦克风释放后才跳转、专注启动、Boss/手动录音交接、断网和快速失败重试。
已用合成语音请求云端识别验证“Hey Reppo＋25分钟专注”和“Hey Reppo＋Boss”两句，并将实际转写纳入解析测试。
这些不能替代真机麦克风测试；本轮未烧录、未录制用户环境音。现场请先靠近设备、逐句说上述例句，
观察唤醒浮层和串口版本，再测试 Boss 返回后的恢复、敲击录音、关闭开关与重启后的状态。

### hey-reppo2：简约白色界面

设置按钮固定为 `Hey Reppo`，沿用其他设置行的字体、尺寸和焦点动效。
详情位于现有右侧区域（135,31，170×126），采用白底、蓝灰文字与柔和绿色开关；
选中时轻移入，单击切换时拨钮滑动，状态与联网语音说明都留在右侧。
唤醒浮层改为白色圆角卡片，200ms 滑入／130ms 收起，右上角细声波随聆听状态轻动；
新的识别文字不会重复触发入场动画。待唤醒提示也改为白色胶囊。
动画只更新坐标和尺寸，不创建全屏透明图层；没有修改 generated 目录。
主机 LVGL 渲染见 `assets/hey_reppo/settings_white.png` 和 `assets/hey_reppo/listening_white.png`。

### hey-reppo3：助手进入 Boss 时 WebSocket 任务无法创建

2026-10-31 日志显示，助手结束后麦克风重新初始化成功，但 Boss 的 `before_websocket_start`
最大内部连续空闲块仅 7936 字节；WebSocket 接收任务需要 8192 字节的连续内部栈，
因此反复报 `Error create websocket task`。总空闲内部堆仍有约 32KB，PSRAM 还有约 7MB，
这次直接故障是内部内存碎片导致任务创建失败，尚未走到服务端语音识别。

将 `boss_stream` 网络发送／连接管理任务的完整 8KB 栈通过 WithCaps API 放入 PSRAM，
TCB 仍由 IDF 放入内部 RAM。此任务不能新增 Flash/NVS 写入；提前取消和正常退出均使用
`vTaskDeleteWithCaps` 释放，不能用普通 `vTaskDelete`。麦克风任务、I2S DMA、WebSocket
接收任务及其 8KB 栈仍留在内部 RAM。WebSocket 收发缓冲各从 2048 减为 1024 字节，
额外减少约 2KB 内部堆需求；消息仍由既有 16KB 缓冲重组，音频队列仍为 6 秒 PSRAM。
未缩小任务栈、修改分区、绕过 TLS，也未改动白色界面或 Boss 展示节奏。

此历史版本标识 `firmware=hey-reppo3`，并打印
`stream_stack=PSRAM bytes=8192 websocket_stack=8192 io_buffer=1024`。
新增主机测试驱动生产网络任务，覆盖 120 轮不同启动／退出组合，包括助手与 Boss 模式、
任务创建失败、缓冲分配失败、麦克风失败、连接超时、配置失败、发送失败，检查退出清理。
该测试验证分配策略和生命周期，不模拟真实 ESP32 堆碎片；仍需烧录后检查从 Hey Reppo
进入 Boss、退出恢复助手、重复切换，以及 `websocket_client: Started` 与实际识别结果。


### hey-reppo4：音频上传积压与英文唤醒句号

实机 hey-reppo3 已能连接，但每 100ms 采集的音频发送耗时多次超过 250ms，
队列从 1 秒持续增长至 6 秒并重连，导致唤醒延迟或没有响应。
源码检查发现：100ms PCM16 转为 base64 JSON 后约 4.3KB，旧 1KB WebSocket 缓冲
会把每条音频消息拆为 5 个协议分片，增加发送开销。这是本轮修正的瓶颈；
真实网络吞吐是否恢复仍需烧录验证，主机测试不能复现 ESP32 的 TLS／热点链路。

WebSocket 收发缓冲改为各 20KiB，让一条音频消息装入一个协议帧。
本工程启用 PSRAM malloc，内部优先阈值为 16KiB；20KiB 的普通 malloc 优先使用 PSRAM，
编译期检查这两项前提。保留 8KiB PSRAM 发送任务栈、内部麦克风／WebSocket 任务栈，
以及 6 秒有界音频队列，不通过增加排队时间掩盖延迟。

`Hey, Repo.` 的尾部英文句号不再被当作指令；命中后等待下一句。
仍保留实际指令中的小数点与负号，避免将“2.5分钟”错误启动成 25 分钟。
音频积压时，设置详情及待唤醒提示显示“语音延迟”。

烧录 `build-asr` 后，确认版本和诊断：

- `firmware=hey-reppo4`，`io_buffer=20480`。
- `websocket_buffers_ready`：观察缓冲分配后的内部 RAM 和 PSRAM。
- 每约 5 秒打印 `audio_upload pcm_ms=... wall_ms=... queued_ms=... peak_send_ms=...`。
  稳态时上传音频时长应接近经过时间，队列不应持续增长；恢复积压时上传时长可以更大。
- `first_asr_transcript` 表示本次连接已收到首条识别文字。
- `wake_detected` 表示命中唤醒词；`command_ready` 表示已解析可执行指令。

日志不记录原始语音、识别全文或凭证。本轮通过 6 项 Boss 主机测试、2 项 Hey Reppo
主机测试和 ESP-IDF 构建；未自动烧录，仍需实机检查唤醒速度、Boss 交接与重复进入退出。


### hey-reppo5：识别回调与界面解耦，异常重启恢复

hey-reppo4 实机日志已收到转写，上传队列从 1000ms 降至 900ms，终端最后还出现
`audio buffering recovered`；用户随后报告卡死、重启、白屏。当前没有取得 panic／
Backtrace，不能仅凭上传日志确认复位原因。本轮修正可见的栈与同步风险，并补充诊断。

- WebSocket 回调只把转写投递至 8 项有界收件箱；不在回调或跨核临界区内解析指令。
  主循环按顺序处理，保留“部分转写包含唤醒词，最终转写只包含指令”的情况。
- 收件箱、消费者暂存和解析工作区合计约 8.3KiB，首次启用时分配至 PSRAM，之后复用。
  不新增任务，不增大内部任务栈；UI 每轮最多处理 8 项，临界区内只做有界复制和索引更新。
  队列满时丢弃整轮；超过 2 秒未处理的事件不执行，关闭／让出麦克风时清空旧事件。
- 转写拼接移除 1536 字节临时数组，保持 UTF-8 截断和 text＋stash 拼接语义。
  Xtensa 编译产物中 `transcript` 栈帧由 2720 降到 1184 字节；回调也不再嵌套唤醒解析栈。
  UI 解析使用外部工作区，避免把这些大数组转移到仅 3584 字节的主任务栈。
- `boot reset_reason=... saved_enabled=... recovery=...` 记录重启原因。
  若已保存启用状态且本次是 panic／看门狗复位，则本次启动暂停助手，设置显示
  “异常重启 · 单击恢复”。不擦除 NVS，也不自动更改保存的偏好；正常上电仍按偏好恢复。
- `first_asr_transcript` 增加 `ws_stack_free`；`wake_detected` 增加 `ui_stack_free`。
  这些是运行期间任务栈最小剩余字节，配合 panic／Backtrace 判断实际栈余量。

版本标识 `firmware=hey-reppo5`，WebSocket 缓冲仍为 `io_buffer=20480`。
验证涵盖真实 LVGL、回调延迟处理、队列溢出／过期／清空、并发生产消费、PSRAM 分配失败、
异常重启偏好保留、唤醒部分句与最终句衔接，以及原有 Boss 和助手生命周期测试。
ESP-IDF 构建通过。尚未烧录验证本次白屏是否消失；若复现，请保留复位前后的完整串口日志。


### hey-reppo6：TLS 崩溃修复与 ASR／AI／切页压力测试

2026-10-31 提供的 hey-reppo4 日志已包含完整崩溃回溯：
`memcmp → ssl_check_ctr_renegotiate → mbedtls_ssl_write → network_worker`，
`EXCVADDR=0`。这次直接故障是 TLS 发送读取了空的接收计数器指针。
源码中的动态 RX 缓冲释放会暂时把 `ssl->in_ctr` 设为 NULL；独立 TX 锁允许另一任务
同时发送，与回溯吻合。该路径已用本机 ESP-IDF 对应源码的指针重置／计数器函数复现。

**修复与内存调整：**

- 在 `sdkconfig` 和 `sdkconfig.defaults` 关闭动态 TLS 缓冲、关闭 WebSocket 独立 TX 锁，
  同一 TLS 连接读写串行化。记录缓冲保留至连接结束，继续分配在 PSRAM；保留证书校验和
  TLS 重新协商功能。`boss_service.c` 增加编译检查，防止误开回危险组合。
  WebSocket 等待可读数据的轮询仍在锁外，不会仅因对端静默就每秒阻塞上传。
- 保留 hey-reppo4 的 20KiB WebSocket 缓冲、hey-reppo5 的有界转写收件箱及异常启动恢复。
- 导航检查同时尊重 Guider 的删除标记，防止旧页面地址被当前页面复用后误判“不需要切页”；
  此版本同时增加的设置入口删除标记判断存在回归，由下方 hey-reppo7 修正。
- AI/ASR 的 HTTP 请求头创建失败时立即清理并返回失败。故障注入测试捕获了原来的漏检。
- 手动语音待办／灵感的 20KiB 任务栈改用 PSRAM，所有退出使用 `vTaskDeleteWithCaps`。
  该任务只录音、转写和润色；保存记录仍由原来的 UI／存储路径执行。
- Keep AI 的 16KiB 网络任务栈改用 PSRAM。读取已完成 CSV 的工作通过
  `keep_ai_storage_poll()` 交给已有内部栈 NVS 工作任务，完成后再派发 JSON／网络任务。
  不在外部栈任务调用 Flash/VFS。单任务限制、手动刷新、缓存、失败回退行为保留。

**压力验证结果（主机 ASan/UBSan，加确定性资源计数）：**

| 范围 | 次数／覆盖 | 结果 |
| --- | --- | --- |
| TLS 已知崩溃 | 原 SDK 指针重置和计数器函数 | 旧 NULL 窗口可复现；新配置禁止该组合 |
| TLS 缓冲／锁策略 | 100,000 次 RX＋100,000 次 TX 计数器访问 | 通过；这是底层函数测试，不是完整 TLS 网络仿真 |
| 连续 ASR 生命周期 | 12,000 轮＋逐分配点失败 | 启停、超时、配置／发送失败、助手与 Boss 模式均释放资源 |
| 手动语音任务 | 3,000 次尝试，含任务创建、PCM、录音、ASR、AI 失败 | 2,500 次已启动任务均走正确外部栈退出；PCM 无残留 |
| AI／HTTP | 3,200 轮＋700 次分配失败注入 | 部分写入、响应错误、401/429/503、截断、无效 JSON、超长输出均安全退出 |
| Keep AI | 1,200 轮＋400 次分配失败注入 | 刷新连击不并发创建任务；存储／网络分工、缓存／离线与清理通过 |
| ASR 指令解析 | 50,000 组确定性模糊输入 | 噪声、标点、截断 UTF-8、输出边界保护通过 |
| Hey 页面导航 | 700 个语音目的地及返回 | 待办、完成、专注入口、Keep、扭蛋、阅读、游戏、设置、相册、答案、时钟、主页、Boss、灵感 |
| 导航的 LVGL 分配计数 | 预热后连续 48 轮比较 | 固定为 3,521 字节／71 个分配，峰值 8,977 字节，无逐轮增长 |
| 页面自身 UI | Boss 500 轮，Keep 300 轮，游戏 300 轮 | 进入／离开、Guider clean/fade/delete、输入与相关定时器清理通过 |
| 完整测试集 | 55 项 | 全部通过 |

导航压力测试使用真实 LVGL 和生产协调逻辑，目的页内容为测试夹具；Keep／游戏／Boss／
灵感另有独立页面测试。上述 LVGL 数值只代表主机测试场景，**不是 ESP32 的剩余内存**。
故障注入使用模拟网络和任务调度，资源计数证明这些分支的清理，不模拟 ESP32 堆碎片、
真实 I2S／热点延迟或物理屏幕。不能据此宣称实机永不重启。

运行全部测试：

```sh
python3 -B -m unittest discover -s scripts/tests -p 'test_*.py' -v
idf.py -B build-asr build
```

ESP-IDF 构建完成；未修改 generated、分区或擦除 NVS，未自动烧录。
VS Code/Cursor 仍使用 `build-asr`。更新后应看到：

```text
firmware=hey-reppo6
tls_buffers=fixed_psram websocket_io=serialized
```

实机验收仍需连续运行：Hey → 待办 → 扭蛋 → Keep（连续刷新）→ Boss → 返回 → 手动语音／
灵感 → 游戏 → 返回，并穿插断网重连。观察 `stream_released` 的内部空闲与最大块是否持续下降；
Keep 结果日志及 `voice_job_finished` 增加任务栈余量与内部堆信息。若复现重启，保留完整
`Guru Meditation`、`Backtrace` 和启动 `reset_reason`，使用对应固件 ELF 解码。

本轮可复查输出：[55 项测试完整日志](docs/test-results/hey-reppo6/host-tests.txt)、[固件构建日志](docs/test-results/hey-reppo6/firmware-build.txt)。


### hey-reppo7：恢复设置中的 Hey Reppo 入口

hey-reppo6 的 `settings_poll()` 错把 `screen_setting_del` 当成页面存活标志。
GUI Guider 的生成代码在创建／加载设置页后不会清除此重建标记，所以实际页面可见时
仍可能为 true，导致 Hey Reppo 按钮没有挂接。现在仅在当前设置页挂接，并由已有
`screen_setting_list_add_btn()` 校验设置列表的有效对象和父页面归属；不再靠重建标记
判断入口是否可用。保留按钮名称 `Hey Reppo`、白色详情卡、开关保存和异常启动恢复。

回归测试改用从生成文件提取的真实 `ui_load_scr_animation()`，设置页夹具也保留重建标记，
模拟旋钮进入的 clean／fade／delete 行为。新增 20 次进入／退出检查按钮存在且不重复；
旧逻辑在新增断言处失败，修复后通过。原有 700 个语音目的地／返回、旧地址复用、
开关保存、ASR 交接和 50,000 组解析模糊输入均通过 ASan/UBSan 主机测试。
本轮导航分配平台为 3,521 字节／71 个分配，峰值 9,001 字节，仅代表主机测试。

构建目录仍是 `build-asr`，ASR 启动版本日志为 `firmware=hey-reppo7`。
未修改 generated 或自动烧录；实机需用 VS Code 原烧录按钮更新，然后进入设置，
向下旋转列表找到 `Hey Reppo`，单击切换开关。


### hey-reppo8：Hi 短唤醒与音频积压发送调整

设置项仍叫 **Hey Reppo**。详情提示改为“说 Hi 或嗨”，支持单独唤醒后说指令，或
“Hi，打开领导黑话翻译”“嗨，帮我开始25分钟专注”。保留 Hey Reppo 及原有近音写法，
增加 Hi Reppo。英文短词按原文单词边界检查，high／history／hint 不作为 Hi 命中；
仍只匹配句首、仅最终转写执行任务。短唤醒也会对日常打招呼的“嗨”作出响应。

本次日志的 `queued_ms` 曾达 4600，说明上传本身滞后约 4.6 秒，不能仅归因于发音。
发送端以前每次最多取 100ms；现在取队列中已有的音频，最多 300ms，不等待凑满，
不丢弃或重排音频。100ms 采集粒度、6 秒有界队列和 TLS 串行读写保护保持原值。
PCM／Base64／JSON 发送临时缓冲共增加约 23KiB，显式从 PSRAM 分配；
最大消息仍装入已有 20KiB WebSocket 帧，不增加内部任务栈。

验证：2 项 Hey 回归（含 700 个混合 Hi／嗨／Hey 语音导航与 50,000 组模糊输入），
6 项 Boss 回归（含 12,000 次生命周期、100/200/300ms 消息边界与分配故障）通过。
生产发送循环在模拟每次发送阻塞 250ms 的场景连续运行 60 包，排队保持在 250ms，
PCM 字节总数完整。这是确定性主机模型，不代表实际热点或云端识别准确率。
实机仍需烧录后比较 `queued_ms` 是否回落，以及是否输出 `wake_detected`。
构建目标 `build-asr`，版本 `firmware=hey-reppo8`，未自动烧录。
