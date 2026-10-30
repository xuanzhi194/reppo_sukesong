# 思必驰本机直连测试：不部署服务器

## 已确认的产品和套餐

- 产品：`reppo_sukesong`，Product ID：`279636052`。
- 2026-10-30 控制台订单显示：实时长语音时长资源包，规格为「24小时（体验包）」，10元，交易完成。
- 这里的 24 小时是语音用量，不应直接当作套餐有效期；有效期需查看套餐详情。
- 实时长语音转写与「一句话识别」是不同技术服务。前者走 WebSocket 并按时长计费；后者走 HTTP 上传并按次数计费，额度不能直接混用。

当前优先使用已购实时长语音套餐测试，真实接口见[官方 Server 接入](https://cloud.aispeech.com/docs/lasr_rt_2)。短录音也能作为实时长语音输入。

## 本次真实调用结果（2026-10-30）

当前尚未收到真实识别文字，不能认为服务已经接通：

| 检查 | 实际结果 |
| --- | --- |
| API 授权创建 | 成功，名称 `reppo_local_test`；API Key 已保存到本地忽略目录 |
| 测试样本 | macOS Tingting 合成语音，4.41 秒，16kHz/16bit/单声道 WAV |
| 实时长语音 WebSocket 握手 | HTTP 401，尚未进入音频上传阶段 |
| 同参数 HTTPS Upgrade 诊断 | `errId=401`，`detailErrId=201922`，`client's param verified failed.` |
| 一句话识别 HTTP 诊断 | HTTP 403，`errId=120100`，`ocs check error` |
| 错误 Key 对照 | 一句话接口也返回相同 403，因此这个接口的 403 不能证明 Key/IP 鉴权通过 |
| 实际部署 | 没有部署业务服务器，也没有切换或烧录 reppo 固件 |

用户已确认将白名单改为 `0.0.0.0/0`。浏览器重载后出现空白画面，同时可访问性读取仍返回旧列表，因此工具的白名单读数不可靠，应以用户实际保存的页面为准。放开限制后的重试仍返回同一 401。不能把当前错误直接归因于识别模型，也不能仅据此认定是 IP、产品未授权或供应商故障。需要核对当前产品授权、套餐开通状态，以及平台现行实时长语音接口是否还需额外连接参数。

现场向思必驰技术支持说明时，可复制以下内容（不包含密钥）：

> 产品 ID 279636052，已购买实时长语音 24 小时体验包，已申请接入 API 的 APIKEY。按公开文档连接 `wss://lasr.duiopen.com/live/ws2`，带 productId、apikey、res=aitranson、lang=cn 和 device_id，握手返回 401；同参数 HTTPS Upgrade 返回 detailErrId=201922、client's param verified failed。请确认当前产品的实时长语音 API 权限、套餐开通/绑定状态，以及最新 WebSocket 地址和完整鉴权参数。

## 本地配置

API 授权已在用户确认后创建，名称为 `reppo_local_test`。真实凭证保存在项目根目录下的 `.secrets/aispeech.env`（Git 忽略，文件权限 0600）。配置格式如下；不要把真实 Key 写到本文档或提交 Git：

```dotenv
AISPEECH_PRODUCT_ID=279636052
AISPEECH_API_KEY=你的APIKEY
AISPEECH_SERVICE=realtime
```

授权绑定的是调用时的公网出口。本次网络存在分流：海外查询的出口与国内查询的出口不同。最初登记海外出口后，真实 WebSocket 握手返回 401；国内查询显示另一个出口，需更正白名单后再验证。修改情况以平台保存结果和真实请求为准，不能仅凭查询 IP 就认定鉴权成功。换 WiFi、热点或代理后，白名单可能需要更新。

## 执行真实服务测试

从项目根目录执行：

```sh
# 已创建此虚拟环境；首次准备其他电脑时再创建即可
python3 -m venv server/ai_proxy/.venv
server/ai_proxy/.venv/bin/python -m pip install -r scripts/requirements-asr-test.txt

server/ai_proxy/.venv/bin/python scripts/test_aispeech_direct.py recording.wav
```

脚本直接连接 `wss://lasr.duiopen.com/live/ws2`，不启动任何代理或 Web 服务。按照协议发送 start、每 40ms 发送一帧音频、发送二进制空帧结束，接收最终转写结果。临时结果不拼入最终文字；同一起点的最终分段会更新而非重复追加。

音频要求：16kHz、16bit、单声道 PCM WAV。本机测试脚本主动限制为 60 秒/5MB，便于快速验证；这不是实时长语音服务本身的时长上限。

也可用 macOS 系统语音合成生成无私人内容的样本：

```sh
say -v Tingting -r 160 -o /tmp/reppo-aispeech-test.aiff \
  '准备答辩二十五分钟。结束前三分钟提醒我。'
afconvert /tmp/reppo-aispeech-test.aiff /tmp/reppo-aispeech-test.wav \
  -f WAVE -d LEI16@16000 -c 1
server/ai_proxy/.venv/bin/python scripts/test_aispeech_direct.py /tmp/reppo-aispeech-test.wav
```

输出包含 `ok`、`service`、识别文字、总耗时和协议事件码，不包含 Key。总耗时包含按实际音频时长发送数据的过程，不能直接当作录音结束后的识别延迟。合成语音测试通过仅证明调用链路工作，不能代替 reppo 真麦克风的噪声、口音、距离和录音截断测试。

如果另外开通了「一句话识别」的次数额度，才可运行：

```sh
server/ai_proxy/.venv/bin/python scripts/test_aispeech_direct.py recording.wav --service sentence
```

## 错误排查

| 错误 | 检查方向 |
| --- | --- |
| WebSocket 握手 401 | productId/apiKey、API授权、国内接口实际公网出口与白名单 |
| WebSocket 握手 403 | 当前所选技术服务的额度、有效期、并发 |
| `realtime_start_not_acknowledged` | start 应返回 errno=7，检查已购服务和协议参数 |
| `realtime_vendor_error_11` | 平台服务异常，稍后重试或提供会话信息给现场技术支持 |
| `no_speech` | 音频是否含人声，以及录音格式是否正确 |
| `realtime_final_timeout` | 最后一帧、二进制空帧、网络及服务响应 |

## 当前设备固件的边界

`main/voice_asr.c` 上一版准备的是「一句话识别代理」方式；本次新增的是**本机实时长语音直连测试脚本**。不能把 WebSocket 地址填到固件的 `VOICE_ASR_PROXY_URL` 就完成适配。真实 API 验证通过后，设备若采用该已购套餐，需要继续加入 ESP32 WebSocket 直连实现；原通义方案保留备用。没有完成该部分之前，reppo 仍按原配置运行。
