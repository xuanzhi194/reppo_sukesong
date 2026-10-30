# 思必驰 ASR 接入与测试

实际购买套餐核对：当前产品购买的是「实时长语音」24小时语音用量体验包，与本文的一句话识别次数包不同。当前不部署服务器，先按 [本机直连测试说明](aispeech_direct_test.md) 验证已购服务。本文保留作为一句话识别代理方式的参考。

比赛项目：`reppo_sukesong`。接入的是[思必驰一句话识别 Server API](https://cloud.aispeech.com/docs/ct_asr_sentence2)，设备不依赖思必驰硬件。

链路：reppo 原麦克风 → 16kHz/16bit/单声道 PCM WAV → 自有后端 `/v1/asr/transcribe` → 思必驰 → 文字 → 原待办/灵感整理。待办录音仍为 4 秒，灵感最长 15 秒。语音界面、录音触发方式和文字整理模型沿用现有实现。

服务器转发是本项目的架构选择，不是思必驰 API 对调用设备的强制要求。后端可以临时运行在电脑上，不必先购买云服务器。根据[官方产品授权文档](https://cloud.aispeech.com/docs/2045)，API 授权支持登记公网 IP，也支持填写 `0.0.0.0/0` 取消 IP 限制；这会使持有该 Key 的任意 IPv4 来源都能尝试调用，仍然需要正确的 productId/apiKey 和有效额度。技术上可另做 reppo 直接 HTTPS 调用适配，但**当前准备的固件代码使用代理后端**，不能仅把 `VOICE_ASR_PROXY_URL` 改成思必驰地址就实现直连。

## 1. 拿到凭证后在哪里配置

向思必驰申请「一句话识别」的基础技术产品及云对云服务授权，需要 `productId`、`apiKey`、有效调用额度，以及**后端服务器公网出口 IP** 白名单。不是把 reppo 的局域网 IP 加进去。云端文档接口限制为单段不超过 60 秒、5MB；本项目后端进一步只接受上述 PCM WAV 格式。

在本项目根目录执行（已有 `.env` 时保留原文件，直接补充配置）：

```sh
cp -n server/ai_proxy/.env.example server/ai_proxy/.env
```

编辑 `server/ai_proxy/.env`：

```dotenv
DEVICE_TOKEN=换成你自己的设备访问口令
AISPEECH_PRODUCT_ID=拿到的productId
AISPEECH_API_KEY=拿到的apiKey
AISPEECH_LANG=ce
```

`ce` 是中英混合，`cn` 是普通话，可按测试结果调整。`.env` 已加入 Git 忽略。**思必驰 productId/apiKey 只填后端，不填到固件。** 原有 DeepSeek 配置可保留；单独测试 ASR 不需要 DeepSeek Key。原通义 ASR 与待办/灵感整理依然沿用 `main/voice_todo.c` 中原有的通义配置。

本机启动：

```sh
python3 -m venv server/ai_proxy/.venv
server/ai_proxy/.venv/bin/python -m pip install -r server/ai_proxy/requirements.txt
server/ai_proxy/.venv/bin/python server/ai_proxy/app.py
```

已有 ECS 服务：把更新后的 `app.py`、**新增的 `aispeech_asr.py`** 和 `requirements.txt` 部署到实际服务目录（原脚本为 `~/reppo-ai-proxy`），编辑该目录下 `.env`，然后 `sudo systemctl restart reppo-ai-proxy`。`install_on_ecs.sh` 已同步新增模块，但不会覆盖已有 `.env`。如果 systemd 的 `EnvironmentFile` 还有同名变量，应修改那里；环境变量优先于 `.env`，更改后重启服务。

如果经过 Nginx，需要合并 `server/ai_proxy/nginx-api.reppo.top.conf` 新增的 `/v1/asr/transcribe` location（含 5MB body 上限和超时），检查配置后 reload。服务器出口 IP 应与思必驰白名单一致。建议设备通过 HTTPS 域名访问；HTTP 地址仅用于可信局域网调试。

## 2. 先在电脑上验证真实思必驰识别

准备一段真实讲话的 WAV，16kHz、16bit、单声道、60 秒以内。例如录制“准备答辩25分钟”。如果原文件是 m4a，可在安装 FFmpeg 后转换：

```sh
ffmpeg -i input.m4a -ar 16000 -ac 1 -c:a pcm_s16le /tmp/reppo-asr.wav
```

另开终端，从项目根目录测试：

```sh
curl -s http://127.0.0.1:8080/health
export DEVICE_TOKEN='与你后端.env里相同的设备口令'
python3 scripts/test_aispeech_asr.py /tmp/reppo-asr.wav
```

远程服务器改用：

```sh
python3 scripts/test_aispeech_asr.py /tmp/reppo-asr.wav \
  --url https://你的API域名/v1/asr/transcribe
```

成功输出示例（文字和耗时以实际结果为准）：

```json
{"elapsed_ms": 1234, "provider": "aispeech", "text": "准备答辩25分钟。"}
```

此接口**不会回退到通义**；只有拿到思必驰成功响应才能返回 `provider=aispeech`。`/health` 中 `aispeech_configured=true` 只说明凭证已填写，不能证明凭证有效、白名单已配置或有余额。

常见错误：

| 返回内容 | 检查方向 |
| --- | --- |
| HTTP 401，`unauthorized` | 客户端的 `DEVICE_TOKEN` 与自有后端不一致 |
| HTTP 503，`aispeech_not_configured` | 实际运行进程未读到 productId/apiKey，检查 `.env` 位置及是否重启 |
| HTTP 502，`aispeech_auth_failed`，`upstream_status=401` | 思必驰产品/Key、服务授权、服务器公网出口 IP 白名单 |
| HTTP 502，`aispeech_quota_or_rate_limit`，`upstream_status=403` | 额度、套餐有效期、并发限制 |
| HTTP 400 / 415 | WAV 格式、空录音、时长或请求 Content-Type |
| HTTP 413 | 文件超过 5MB；也检查反向代理的 body 限制 |
| HTTP 422，`no_speech` | 录音是否包含可识别的人声 |
| HTTP 504，`aispeech_timeout` | 后端到思必驰的网络或服务响应超时 |

## 3. 切换 reppo 固件

复制设备配置模板（已有本地配置时直接编辑）：

```sh
cp -n main/voice_asr_local.example.h main/voice_asr_local.h
```

编辑 `main/voice_asr_local.h`：

```c
#pragma once
#define VOICE_ASR_PROVIDER VOICE_ASR_AISPEECH
#define VOICE_ASR_ALLOW_QWEN_FALLBACK 0
#define VOICE_ASR_PROXY_URL "https://你的API域名/v1/asr/transcribe"
#define VOICE_ASR_DEVICE_TOKEN "与你后端DEVICE_TOKEN一致的设备口令"
```

`voice_asr_local.h` 已加入 Git 忽略。设备地址不能写 `127.0.0.1`；它指向设备自身。电脑临时跑后端时，填电脑在同一 WiFi 内的局域网 IP，并允许电脑防火墙访问该端口。拿到服务器 API Key 后仅改服务器配置不需要重新烧录；改设备访问口令、代理地址、服务选择或回退策略则需要重新构建和烧录。

新项目原 `build/` 是从旧目录复制的缓存，因此统一使用新的 `build-asr/`。当前电脑使用 ESP-IDF 5.3.5：

```sh
source "$HOME/.espressif/v5.3.5/esp-idf/export.sh"
idf.py -B build-asr build
# 将 SERIAL_PORT 换成实际串口（可用 ls /dev/cu.* 查看）
idf.py -B build-asr -p SERIAL_PORT flash monitor
```

保持默认普通固件更新，不要启用工厂存储覆盖选项或执行 erase-flash。

连上 WiFi 后，按原方式触发语音待办，朗读测试句，再测试灵感入口。串口中应看到：

```text
ASR: primary=aispeech fallback=0
ASR: success provider=aispeech
```

首次测试 `VOICE_ASR_ALLOW_QWEN_FALLBACK` 保持 `0`，避免用通义成功掩盖思必驰未接通。确认识别结果、待办分钟数、灵感原文均正常，再改为 `1`、重新构建烧录，即可启用备用。思必驰失败时会上传**同一段录音**到原通义服务，日志明确显示：

```text
ASR: AISpeech failed
ASR: fallback provider=qwen (same recording)
ASR: success provider=qwen
```

开启回退后，失败场景可能先等待思必驰超时再等待通义，体验会更慢。录音也会发送给备用供应商。服务端保持单次请求，不做隐式重试；设备每次最多回退一次。

要只用原方案，把本地配置中的 `VOICE_ASR_PROVIDER` 改为 `VOICE_ASR_QWEN` 并重新烧录。没有 `voice_asr_local.h` 时默认就用原通义方案。

## 4. 无凭证回归测试

在项目根目录执行：

```sh
server/ai_proxy/.venv/bin/python -B -m unittest discover -s server/ai_proxy/tests -v
python3 -B -m unittest discover -s scripts/tests -p test_voice_asr.py -v
```

后端测试使用模拟的上游 HTTP 响应，覆盖真实 multipart 编码、鉴权、音频校验、结果拼接和异常处理；固件测试编译生产 ASR 模块并模拟 HTTP 传输，覆盖两种主服务、严格模式、失败回退、分段收发和资源释放。固件主机测试需要 C 编译器及 ESP-IDF 中的 cJSON，设置 `IDF_PATH` 可指定安装位置。这些测试不会消费思必驰或通义额度。

已进行离线验证不等于真实接通。最终需要有效凭证、白名单、真实语音及 reppo 实机，按第 2、3 节验收。
