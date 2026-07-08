#!/usr/bin/env python3
"""
宣传视频：向 REPPO 发送 BLE 0x30，或按设备 ENTER 键，触发正常开机流程。

设备需烧录 REPPO_DEMO_BLE_BOOT_GATE=1 的固件（见 components/BSP/bsp_ble/reppo_demo_mode.h）。

依赖:
    pip install bleak

示例:
    python3 scripts/ble_demo_boot_start.py
    python3 scripts/ble_demo_boot_start.py --address AA:BB:CC:DD:EE:FF
"""

from __future__ import annotations

import argparse
import asyncio
import sys

try:
    from bleak import BleakClient, BleakScanner
except ImportError:
    print("请先安装 bleak:  pip install bleak", file=sys.stderr)
    sys.exit(1)

DEVICE_NAME = "REPPO_BLE_Device"
CHR_UUID = "0000ff01-0000-1000-8000-00805f9b34fb"
CMD_DEMO_BOOT_START = bytes([0x30])


async def find_device(address: str | None) -> str:
    if address:
        return address
    print(f"扫描 {DEVICE_NAME} ...")
    device = await BleakScanner.find_device_by_filter(
        lambda d, ad: (d.name and DEVICE_NAME in d.name)
        or (ad.local_name and DEVICE_NAME in ad.local_name),
        timeout=20.0,
    )
    if device is None:
        raise RuntimeError(f"未找到 {DEVICE_NAME}，请确认设备已上电且在 BLE 待命广播")
    print(f"找到: {device.name} [{device.address}]")
    return device.address


async def send_demo_boot(address: str) -> None:
    async with BleakClient(address, timeout=30.0) as client:
        print(f"已连接 {address}")
        await client.write_gatt_char(CHR_UUID, CMD_DEMO_BOOT_START, response=True)
        print("已发送 0x30 DEMO_BOOT_START，设备应开始正常开机动画")


def main() -> None:
    parser = argparse.ArgumentParser(description="REPPO 宣传视频 BLE 开机触发")
    parser.add_argument(
        "--address",
        help="设备 BLE 地址（省略则自动扫描 REPPO_BLE_Device）",
    )
    args = parser.parse_args()
    try:
        addr = asyncio.run(find_device(args.address))
        asyncio.run(send_demo_boot(addr))
    except Exception as exc:
        print(f"失败: {exc}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
