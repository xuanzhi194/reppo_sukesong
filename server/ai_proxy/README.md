# Reppo AI Proxy（ECS 部署）

已加入思必驰一句话 ASR，接口为 `POST /v1/asr/transcribe`。配置 `AISPEECH_PRODUCT_ID`、`AISPEECH_API_KEY` 后可独立测试，不需要 DeepSeek Key。固件切换、备用策略和完整测试步骤见 [思必驰 ASR 接入说明](../../docs/aispeech_asr.md)。部署时请同时上传 `app.py` 和新增的 `aispeech_asr.py`；已有服务的 `.env` 需要手工补充思必驰配置。

ESP32 通过 HTTP 访问本服务，由 ECS 代为调用 DeepSeek。**DeepSeek API Key 只写在服务器 `.env` 里。**

适用：**Alibaba Cloud Linux 3.2104 LTS**（`dnf`）。代码需拷贝/克隆到 ECS 上运行，不会从本仓库自动启动。

---

## 一、阿里云安全组

ECS 控制台 → 安全组 → **入方向** → 添加规则：

| 协议 | 端口 | 来源 |
|------|------|------|
| TCP | 8080 | 0.0.0.0/0（测试；量产可收紧） |

---

## 二、SSH 登录 ECS 后执行（root 无需 sudo）

### 方式 A：一键脚本（推荐）

先把本目录 `server/ai_proxy/` 上传到 ECS，例如 `/root/ai_proxy/`，然后：

```bash
cd /root/ai_proxy
bash install_on_ecs.sh
vi /root/reppo-ai-proxy/.env    # 填入 DEEPSEEK_API_KEY、DEVICE_TOKEN
systemctl restart reppo-ai-proxy
```

### 方式 B：手动逐步执行

```bash
# 1. 安装 Python
dnf install -y python3 python3-pip curl

# 2. 工作目录
mkdir -p ~/reppo-ai-proxy && cd ~/reppo-ai-proxy
python3 -m venv venv
source venv/bin/activate
pip install -U pip
pip install flask httpx

# 3. 创建 app.py（或 scp 上传仓库里的 app.py）
# 4. 创建 .env
cat > .env << 'EOF'
DEEPSEEK_API_KEY=sk-你的deepseek密钥
DEVICE_TOKEN=reppo-dev-token-change-me
DEEPSEEK_MODEL=deepseek-v4-flash
PORT=8080
EOF
vi .env   # 确认 Key 正确

# 5. 本机防火墙（ACL3 若启用了 firewalld）
firewall-cmd --permanent --add-port=8080/tcp
firewall-cmd --reload

# 6. 前台试跑（Ctrl+C 停）
python app.py
# 另开终端: curl -s http://127.0.0.1:8080/health

# 7. systemd 开机自启（确认试跑 OK 后再做）
deactivate
cat > /etc/systemd/system/reppo-ai-proxy.service << EOF
[Unit]
Description=Reppo AI Proxy
After=network.target

[Service]
Type=simple
User=root
WorkingDirectory=/root/reppo-ai-proxy
EnvironmentFile=/root/reppo-ai-proxy/.env
ExecStart=/root/reppo-ai-proxy/venv/bin/python /root/reppo-ai-proxy/app.py
Restart=on-failure

[Install]
WantedBy=multi-user.target
EOF

systemctl daemon-reload
systemctl enable reppo-ai-proxy
systemctl start reppo-ai-proxy
systemctl status reppo-ai-proxy
```

---

## 三、验证

在 **ECS 上**：

```bash
curl -s http://127.0.0.1:8080/health
# 期望: {"ok":true}

source /root/reppo-ai-proxy/.env
curl -s -X POST http://127.0.0.1:8080/v1/ai/summary \
  -H "Authorization: Bearer $DEVICE_TOKEN" \
  -H "Content-Type: application/json" \
  -d '{"prompt":"只回复一行: CAT1|测试|100"}'
```

在 **你电脑上**（把 `公网IP` 换成 ECS 公网 IP）：

```bash
curl -s http://公网IP:8080/health
```

---

## 四、固件配置

`components/BSP/ai_chat/ai_chat_secrets.h`：

```c
#define AI_PROXY_URL    "http://公网IP:8080/v1/ai/summary"
#define AI_DEVICE_TOKEN "reppo-dev-token-change-me"   /* 与 .env 一致 */
```

编译烧录后，串口应出现 `proxy=1 URL=http://...`。

---

## 五、常用运维命令

```bash
systemctl status reppo-ai-proxy
systemctl restart reppo-ai-proxy
journalctl -u reppo-ai-proxy -f          # 看日志
vi /root/reppo-ai-proxy/.env             # 改 Key 后 restart
```

---

## 六、备案完成后：API 子域名 api.reppo.top + HTTPS

AI 代理使用 **`api.reppo.top`**，与官网 **`reppo.top`** 分离（官网可另配 Nginx，互不冲突）。

`app.py` **不用改**，仍监听本机 `127.0.0.1:8080`。对外由 **Nginx 443** 反代。

### 6.1 域名解析（阿里云 DNS）

| 记录类型 | 主机记录 | 记录值 | 用途 |
|----------|----------|--------|------|
| A | `api` | ECS 公网 IP（如 47.96.132.214） | **ESP32 AI 代理** |
| A | `@` | 同上（可选） | 以后官网 reppo.top |
| A 或 CNAME | `www` | 同上或 reppo.top（可选） | 官网 |

等待解析生效：

```bash
ping api.reppo.top
```

### 6.2 安全组

| 协议 | 端口 | 说明 |
|------|------|------|
| TCP | 80 | certbot 验证 |
| TCP | 443 | HTTPS（ESP32 / curl） |

### 6.3 ECS 安装 Nginx + 证书

```bash
dnf install -y nginx certbot python3-certbot-nginx

# 使用 API 子域名配置（仓库文件 nginx-api.reppo.top.conf）
cp nginx-api.reppo.top.conf /etc/nginx/conf.d/api.reppo.top.conf

# 若之前用过 reppo.top 做 API，可删除旧配置避免冲突：
# rm -f /etc/nginx/conf.d/reppo.top.conf

nginx -t && systemctl enable nginx && systemctl start nginx

certbot --nginx -d api.reppo.top

systemctl reload nginx
```

确认代理服务：

```bash
systemctl status reppo-ai-proxy
curl -s https://api.reppo.top/health
```

### 6.4 验证 AI 接口

```bash
curl -s -X POST https://api.reppo.top/v1/ai/summary \
  -H "Authorization: Bearer 你的DEVICE_TOKEN" \
  -H "Content-Type: application/json" \
  -d '{"prompt":"只回复: OK"}'
```

### 6.5 固件配置（本仓库）

`components/BSP/ai_chat/ai_chat_secrets.h`：

```c
#define AI_PROXY_URL "https://api.reppo.top/v1/ai/summary"
```

重新编译烧录。串口应看到 `proxy=1 URL=https://api.reppo.top/...`。

### 6.6 从 reppo.top 迁移到 api.reppo.top（已在跑旧域名时）

1. DNS 添加 `api` → ECS IP  
2. 部署 `api.reppo.top.conf`，`certbot --nginx -d api.reppo.top`  
3. 验证 `https://api.reppo.top/health`  
4. 烧录新固件（或改 secrets 后重编）  
5. 确认设备正常后，可从旧 `reppo.top.conf` 中删除 `/v1/ai/summary` 反代，留给官网使用  

### 6.7 可选加固

- 安全组 **关闭公网 8080**
- `certbot renew --dry-run` 检查续期
