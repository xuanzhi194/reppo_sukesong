#!/usr/bin/env bash
# 检测 Mac 上 ESP32 USB 串口并写入 .vscode/settings.json 的 idf.port
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SETTINGS="$ROOT/.vscode/settings.json"

# CH340 在 Mac 上装好 WCH 驱动后通常是 /dev/cu.wchusbserial*
find_port() {
    local p
    for p in /dev/cu.wchusbserial* /dev/tty.wchusbserial* \
             /dev/cu.usbserial-* /dev/cu.SLAB_USBtoUART*; do
        if [ -e "$p" ]; then
            # 烧录用 cu.*（callout），不用 tty.*
            case "$p" in /dev/tty.*) continue ;; esac
            echo "$p"
            return 0
        fi
    done
    return 1
}

ch340_driver_hint() {
    echo "CH340 on Mac:"
    echo "  1. Install WCH driver: https://www.wch.cn/downloads/CH341SER_MAC_ZIP.html"
    echo "     (CH341SER_MAC.ZIP -> pkg, restart Mac)"
    echo "  2. System Settings -> Privacy & Security -> allow WCH if prompted"
    echo "  3. Plug board directly (data cable); expect /dev/cu.wchusbserialXXXX"
    echo "  4. Avoid HDMI-only USB-C adapters"
}

PORT=""
if PORT="$(find_port)"; then
    echo "Found CH340/serial port: $PORT"
else
    echo "No CH340 serial port on this Mac."
    ch340_driver_hint
    echo ""
    echo "Current /dev/cu.* :"
    ls /dev/cu.* 2>/dev/null || true
    if ! system_profiler SPUSBDataType 2>/dev/null | grep -qi '1a86\|CH34\|wch'; then
        echo ""
        echo "USB: CH340 chip not visible — check cable/port (Windows COM6 path may differ)."
    fi
    exit 1
fi

python3 <<PY
import json
from pathlib import Path

settings_path = Path("$SETTINGS")
data = json.loads(settings_path.read_text(encoding="utf-8"))
data["idf.port"] = "$PORT"
data["idf.espIdfPath"] = "/Users/sean/.espressif/v5.3.5/esp-idf"
data["idf.currentSetup"] = "/Users/sean/.espressif/v5.3.5/esp-idf"
settings_path.write_text(json.dumps(data, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
print(f"Updated {settings_path}: idf.port = $PORT")
PY
