#!/usr/bin/env python3
"""
向 REPPO 下发手机本地墙钟（BLE 0x10 TIME_SET）。

设备协议:
    0x10 TIME_SET    payload = year(u16 LE) + month + date + hour + min + sec + wday
                     wday: 0=Sunday .. 6=Saturday（可省略，设备会按日期推算）
    0x11 TIME_STATUS notify = status(1)  0=ok  1=err

依赖:
    pip install bleak

示例:
    python3 scripts/ble_send_phone_time.py
    python3 scripts/ble_send_phone_time.py --address AA:BB:CC:DD:EE:FF
"""

from __future__ import annotations

import argparse
import asyncio
import datetime
import struct
import sys

try:
    from bleak import BleakClient, BleakScanner
except ImportError:
    print("请先安装 bleak:  pip install bleak", file=sys.stderr)
    sys.exit(1)

DEVICE_NAME = "REPPO_BLE_Device"
CHR_UUID = "0000ff01-0000-1000-8000-00805f9b34fb"
CMD_TIME_SET = 0x10
CMD_TIME_STATUS = 0x11
TIME_STATUS_TIMEOUT_SECONDS = 5.0


def pack_phone_time(now: datetime.datetime) -> bytes:
    return bytes([CMD_TIME_SET]) + struct.pack(
        "<HBBBBBB",
        now.year,
        now.month,
        now.day,
        now.hour,
        now.minute,
        now.second,
        (now.weekday() + 1) % 7,
    )


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
        raise RuntimeError(f"未找到 {DEVICE_NAME}")
    print(f"找到: {device.name} [{device.address}]")
    return device.address


async def send_phone_time(address: str) -> None:
    status = asyncio.get_running_loop().create_future()

    def on_notify(_handle, data: bytearray) -> None:
        if len(data) >= 2 and data[0] == CMD_TIME_STATUS and not status.done():
            status.set_result(data[1])

    async with BleakClient(address, timeout=30.0) as client:
        print(f"已连接 {address}")
        try:
            await client.start_notify(CHR_UUID, on_notify)
        except Exception as exc:
            raise RuntimeError(f"无法订阅时间同步结果，未发送时间: {exc}") from exc
        # 在连接、服务发现和订阅完成后取时间，避免把连接耗时写进 RTC。
        now = datetime.datetime.now()
        packet = pack_phone_time(now)
        await client.write_gatt_char(CHR_UUID, packet, response=True)
        print(
            f"已发送 0x10 TIME_SET {now:%Y-%m-%d %H:%M:%S} "
            f"wday={packet[-1]}"
        )
        try:
            result = await asyncio.wait_for(status, timeout=TIME_STATUS_TIMEOUT_SECONDS)
        except asyncio.TimeoutError as exc:
            raise RuntimeError("等待 0x11 超时，未确认 RTC 写入成功") from exc
        if result == 0:
            print("设备回复 0x11 OK")
        elif result == 1:
            raise RuntimeError("设备回复 0x11 ERR")
        else:
            raise RuntimeError(f"设备回复未知时间同步状态 0x{result:02X}，未确认 RTC 写入成功")


async def sync_phone_time(address: str | None) -> None:
    address = await find_device(address)
    await send_phone_time(address)


def main() -> None:
    parser = argparse.ArgumentParser(description="REPPO BLE 手机时间同步")
    parser.add_argument("--address", help="设备 BLE 地址")
    args = parser.parse_args()
    try:
        asyncio.run(sync_phone_time(args.address))
    except Exception as exc:
        print(f"失败: {exc}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
