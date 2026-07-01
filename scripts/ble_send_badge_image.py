#!/usr/bin/env python3
"""
Reppo 吧唧图 BLE 传输测试脚本

模拟微信小程序向 REPPO 设备发送 JPEG（协议 cmd 0x20 / 0x21 / 0x22）。

依赖:
    pip install bleak

使用前:
    1. 设备进入 设置 -> 打开蓝牙
    2. 确认串口能看到 "REPPO_BLE_Device" 广播
    3. macOS 需授予终端/IDE 蓝牙权限

示例:
    python3 scripts/ble_send_badge_image.py test.jpg
    python3 scripts/ble_send_badge_image.py --address AA:BB:CC:DD:EE:FF badge.jpg
"""

from __future__ import annotations

import argparse
import asyncio
import struct
import sys
import zlib
from pathlib import Path

try:
    from bleak import BleakClient, BleakScanner
except ImportError:
    print("请先安装 bleak:  pip install bleak", file=sys.stderr)
    sys.exit(1)

# 与 components/BSP/bsp_ble/bsp_ble.c、badge_storage.h 一致
DEVICE_NAME = "REPPO_BLE_Device"
SVC_UUID = "000000ff-0000-1000-8000-00805f9b34fb"
CHR_UUID = "0000ff01-0000-1000-8000-00805f9b34fb"

CMD_IMG_INFO = 0x20
CMD_IMG_DATA = 0x21
CMD_IMG_COMMIT = 0x22
CMD_IMG_STATUS = 0x23

FMT_JPEG = 1
STATUS_OK = 0
STATUS_ERR = 1
STATUS_BUSY = 2

MAX_CHUNK = 251  # GATT 写 256 字节: 1(cmd)+4(offset)+251(data)
MAX_IMAGE_BYTES = 768 * 1024


def esp_crc32(data: bytes) -> int:
    """与 ESP-IDF esp_crc32_le(0, data, len) 一致（IEEE CRC32）。"""
    return zlib.crc32(data) & 0xFFFFFFFF


def pack_u32_le(value: int) -> bytes:
    return struct.pack("<I", value)


class StatusWaiter:
    def __init__(self) -> None:
        self.event = asyncio.Event()
        self.last: tuple[int, int] | None = None

    def feed(self, data: bytearray) -> None:
        if len(data) < 2 or data[0] != CMD_IMG_STATUS:
            return
        status, progress = data[1], data[2] if len(data) > 2 else 0
        self.last = (status, progress)
        label = {STATUS_OK: "OK", STATUS_ERR: "ERR", STATUS_BUSY: "BUSY"}.get(
            status, str(status)
        )
        print(f"  [notify 0x23] status={label} progress={progress}%")
        if status in (STATUS_OK, STATUS_ERR):
            self.event.set()
        # BUSY 可能在 INFO 阶段发出但 NOTIFY 延迟到达，继续等待 OK/ERR


async def find_device(name: str, address: str | None, timeout: float) -> str:
    if address:
        return address

    print(f"扫描 BLE 设备 ({timeout:.0f}s)，查找 {name!r} ...")
    device = await BleakScanner.find_device_by_filter(
        lambda d, ad: (d.name and d.name == name)
        or (ad.local_name and ad.local_name == name),
        timeout=timeout,
    )
    if device is None:
        raise RuntimeError(
            f"未找到 {name!r}。请确认设备已开蓝牙且未被其他手机占用。"
        )
    print(f"找到: {device.name} [{device.address}]")
    return device.address


async def send_image(
    address: str,
    image_path: Path,
    chunk_size: int,
    use_write_response: bool,
    packet_delay_ms: float,
    commit_timeout: float,
) -> None:
    data = image_path.read_bytes()
    if len(data) == 0:
        raise ValueError("图片文件为空")
    if len(data) > MAX_IMAGE_BYTES:
        raise ValueError(
            f"图片过大 ({len(data)} B)，上限 {MAX_IMAGE_BYTES} B ({MAX_IMAGE_BYTES // 1024} KB)"
        )
    if data[0:2] != b"\xff\xd8":
        print("警告: 文件头不是 JPEG (FF D8)，设备仍按 JPEG 格式接收")

    crc = esp_crc32(data)
    print(f"文件: {image_path}  大小: {len(data)} B  CRC32: 0x{crc:08X}")

    waiter = StatusWaiter()

    def on_notify(_handle: int, payload: bytearray) -> None:
        waiter.feed(payload)

    async with BleakClient(address, timeout=20.0) as client:
        print(f"已连接: {client.is_connected}")
        await client.start_notify(CHR_UUID, on_notify)

        # 0x20 INFO — 带响应，确保设备已开始接收
        info = bytes([CMD_IMG_INFO]) + pack_u32_le(len(data)) + pack_u32_le(crc) + bytes([FMT_JPEG])
        await client.write_gatt_char(CHR_UUID, info, response=True)
        print("已发送 0x20 INFO")

        # 0x21 DATA
        offset = 0
        total = len(data)
        while offset < total:
            piece = data[offset : offset + chunk_size]
            pkt = bytes([CMD_IMG_DATA]) + pack_u32_le(offset) + piece
            await client.write_gatt_char(
                CHR_UUID, pkt, response=use_write_response
            )
            offset += len(piece)
            pct = offset * 100 // total
            print(f"\r  传输进度: {offset}/{total} ({pct}%)", end="", flush=True)
            if packet_delay_ms > 0:
                await asyncio.sleep(packet_delay_ms / 1000.0)
        print()

        # 0x22 COMMIT — 带响应，避免最后一包丢失
        await client.write_gatt_char(CHR_UUID, bytes([CMD_IMG_COMMIT]), response=True)
        print("已发送 0x22 COMMIT，等待设备校验 ...")

        try:
            await asyncio.wait_for(waiter.event.wait(), timeout=commit_timeout)
        except asyncio.TimeoutError:
            print(
                f"等待 {commit_timeout:.0f}s 未收到 OK/ERR NOTIFY。"
                " 若 ACL mbuf 紧张，OK 可能丢失；请进相片页查看或加大 --packet-delay-ms 重试。"
            )
            return

        if waiter.last is None:
            return
        status, _ = waiter.last
        if status == STATUS_OK:
            print("传输成功。请在设备上进入 主页 -> 相片 查看。")
        elif status == STATUS_ERR:
            raise RuntimeError("设备返回 ERR，请查看设备串口 badge_storage / REPPO_BLE 日志")
        else:
            print("收到 BUSY 但未结束，请加大 --packet-delay-ms 或使用 --write-with-response 重试")


def parse_args() -> argparse.Namespace:
    p = argparse.ArgumentParser(description="Reppo BLE 吧唧图传输测试")
    p.add_argument("image", type=Path, help="JPEG 文件路径")
    p.add_argument(
        "--address",
        "-a",
        help="设备 BLE MAC（不填则按名称 REPPO_BLE_Device 扫描）",
    )
    p.add_argument(
        "--name",
        default=DEVICE_NAME,
        help=f"扫描用的设备名（默认 {DEVICE_NAME}）",
    )
    p.add_argument(
        "--scan-timeout",
        type=float,
        default=12.0,
        help="扫描超时秒数",
    )
    p.add_argument(
        "--chunk-size",
        type=int,
        default=MAX_CHUNK,
        help=f"每包数据字节数（默认 {MAX_CHUNK}）",
    )
    p.add_argument(
        "--write-with-response",
        action="store_true",
        help="DATA 包也使用 Write Request（更稳但更慢；默认仅 INFO/COMMIT 带响应）",
    )
    p.add_argument(
        "--packet-delay-ms",
        type=float,
        default=8.0,
        help="DATA 包之间的间隔毫秒（默认 8，减轻 ACL mbuf 丢包）",
    )
    p.add_argument(
        "--commit-timeout",
        type=float,
        default=30.0,
        help="等待 COMMIT 后 OK/ERR 的超时秒数（默认 30）",
    )
    return p.parse_args()


def main() -> None:
    args = parse_args()
    if not args.image.is_file():
        print(f"文件不存在: {args.image}", file=sys.stderr)
        sys.exit(1)
    if args.chunk_size < 1 or args.chunk_size > MAX_CHUNK:
        print(f"--chunk-size 须在 1..{MAX_CHUNK}", file=sys.stderr)
        sys.exit(1)

    async def run() -> None:
        addr = await find_device(args.name, args.address, args.scan_timeout)
        await send_image(
            addr,
            args.image,
            args.chunk_size,
            use_write_response=args.write_with_response,
            packet_delay_ms=max(0.0, args.packet_delay_ms),
            commit_timeout=args.commit_timeout,
        )

    try:
        asyncio.run(run())
    except KeyboardInterrupt:
        print("\n已取消")
        sys.exit(130)
    except Exception as exc:
        print(f"失败: {exc}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
