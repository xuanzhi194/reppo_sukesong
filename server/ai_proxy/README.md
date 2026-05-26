# Reppo AI Proxy（ECS 部署）

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
DEEPSEEK_MODEL=deepseek-v4-pro
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

## 六、备案完成后

Nginx + HTTPS + 域名 `reppo.top`，固件改为：

```c
#define AI_PROXY_URL "https://reppo.top/v1/ai/summary"
```

服务 `app.py` 与路径 `/v1/ai/summary` 可保持不变。
