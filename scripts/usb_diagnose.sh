#!/usr/bin/env bash
# Mac 上排查 ESP/Reppo 是否被系统识别为 USB 串口
set -euo pipefail

echo "========== 串口设备 (/dev/cu.*) =========="
found_serial=0
for p in /dev/cu.*; do
    case "$p" in
        *Bluetooth*|*debug-console*) continue ;;
    esac
    echo "  $p"
    found_serial=1
done
if [ "$found_serial" -eq 0 ]; then
    echo "  (无 ESP 相关串口 — 只有蓝牙/debug 时说明 Mac 未看到下载芯片)"
fi

echo ""
echo "========== USB 设备（与烧录相关关键词）=========="
system_profiler SPUSBDataType 2>/dev/null | grep -E 'Product ID|Vendor ID|Manufacturer|Location ID|Serial Number|USB Product Name|USB Vendor Name|CH34|CP210|FTDI|UART|Serial|Espressif|wch|JTAG|CDC|Modem' -B0 -A0 || true

echo ""
echo "========== 解读 =========="
if system_profiler SPUSBDataType 2>/dev/null | grep -q 'Digital AV Adapter'; then
    echo "  检测到: USB Type-C Digital AV Adapter（HDMI/视频转接头）"
    echo "  → 这类转接头通常不能用来给 ESP 烧录，Mac 也看不到板子串口。"
fi
if system_profiler SPUSBDataType 2>/dev/null | grep -qiE '1a86|CH34|wch|WCH'; then
    echo "  USB: 已看到 CH340 (WCH 1a86) — 若仍无串口，请安装/允许 Mac 驱动后重启"
elif system_profiler SPUSBDataType 2>/dev/null | grep -qiE 'CP210|FTDI|USB Single|UART|Serial'; then
    echo "  检测到: 其他 USB-串口芯片 — 运行 scripts/set_esp_port.sh"
else
    echo "  USB: 未看到 CH340 (vendor 1a86) — 线/转接头/插口可能无数据，或板子未上电"
    echo "  Windows COM6 正常 => 焊接 OK；请 Mac 上用与 Win 相同的 CH340 口 + 数据线直连"
fi

echo ""
if ls /dev/cu.wchusbserial* &>/dev/null; then
    echo "  串口: $(ls /dev/cu.wchusbserial* | tr '\n' ' ')"
elif ls /dev/cu.usbserial-* &>/dev/null; then
    echo "  串口(旧驱动名): $(ls /dev/cu.usbserial-* | tr '\n' ' ')"
fi

echo ""
echo "建议连接方式:"
echo "  Mac USB口 ──[仅数据转接头/Hub]── 与 Windows 上 COM6 相同的那根线 ── Reppo 下载口"
echo "  不要经过: HDMI 转接头、仅充电线、无数据 USB-C 扩展坞"
