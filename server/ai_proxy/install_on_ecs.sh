#!/bin/bash
# Alibaba Cloud Linux 3 / RHEL 8+（dnf）
# 用法：bash install_on_ecs.sh
set -euo pipefail

APP_DIR="${HOME}/reppo-ai-proxy"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

if [[ "$(id -u)" -eq 0 ]]; then
  SUDO=""
else
  SUDO="sudo"
fi

echo "==> 安装依赖 (dnf)"
${SUDO} dnf install -y python3 python3-pip curl

echo "==> 部署到 ${APP_DIR}"
mkdir -p "${APP_DIR}"
cp "${SCRIPT_DIR}/app.py" "${SCRIPT_DIR}/requirements.txt" "${APP_DIR}/"
if [[ ! -f "${APP_DIR}/.env" ]]; then
  cp "${SCRIPT_DIR}/.env.example" "${APP_DIR}/.env"
  echo ""
  echo "*** 请编辑 ${APP_DIR}/.env 填入 DEEPSEEK_API_KEY 和 DEVICE_TOKEN ***"
  echo "    vi ${APP_DIR}/.env"
  echo ""
fi

python3 -m venv "${APP_DIR}/venv"
"${APP_DIR}/venv/bin/pip" install -U pip
"${APP_DIR}/venv/bin/pip" install -r "${APP_DIR}/requirements.txt"

echo "==> 放行本机防火墙 8080（若 firewalld 在运行）"
if systemctl is-active --quiet firewalld 2>/dev/null; then
  ${SUDO} firewall-cmd --permanent --add-port=8080/tcp
  ${SUDO} firewall-cmd --reload
  echo "firewalld: 已放行 8080/tcp"
else
  echo "firewalld 未运行，跳过（仍需在阿里云安全组放行 8080）"
fi

echo "==> 创建 systemd 服务"
${SUDO} tee /etc/systemd/system/reppo-ai-proxy.service >/dev/null <<EOF
[Unit]
Description=Reppo AI Proxy (ESP32 -> DeepSeek)
After=network.target

[Service]
Type=simple
User=${USER}
WorkingDirectory=${APP_DIR}
EnvironmentFile=${APP_DIR}/.env
ExecStart=${APP_DIR}/venv/bin/python ${APP_DIR}/app.py
Restart=on-failure
RestartSec=5

[Install]
WantedBy=multi-user.target
EOF

${SUDO} systemctl daemon-reload
${SUDO} systemctl enable reppo-ai-proxy
${SUDO} systemctl restart reppo-ai-proxy

echo "==> 服务状态"
${SUDO} systemctl status reppo-ai-proxy --no-pager || true
echo ""
echo "本地测试: curl -s http://127.0.0.1:8080/health"
echo "阿里云控制台 -> 安全组 -> 入方向 -> 放行 TCP 8080"
echo "编辑密钥: vi ${APP_DIR}/.env  然后 systemctl restart reppo-ai-proxy"
