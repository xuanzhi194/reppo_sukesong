#!/usr/bin/env python3
"""
Reppo 电子书 BLE 上传测试脚本

模拟 App 向 REPPO 发送 UTF-8 book.txt（协议 cmd 0x50 / 0x51 / 0x52）。
文件格式见 data/book.txt：@title / @author / @part / @page。

依赖:
    pip install bleak

示例:
    python3 scripts/ble_send_ebook.py data/book.txt
    python3 scripts/ble_send_ebook.py --address AA:BB:CC:DD:EE:FF data/book.txt
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

DEVICE_NAME = "REPPO_BLE_Device"
CHR_UUID = "0000ff01-0000-1000-8000-00805f9b34fb"

CMD_BOOK_INFO = 0x50
CMD_BOOK_DATA = 0x51
CMD_BOOK_COMMIT = 0x52
CMD_BOOK_STATUS = 0x53

FMT_TXT = 1
STATUS_OK = 0
STATUS_ERR = 1
STATUS_BUSY = 2

MAX_CHUNK = 251
MAX_BOOK_BYTES = 1024 * 1024


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
        if len(data) < 2 or data[0] != CMD_BOOK_STATUS:
            return
        status = data[1]
        progress = data[2] if len(data) > 2 else 0
        self.last = (status, progress)
        label = {STATUS_OK: "OK", STATUS_ERR: "ERR", STATUS_BUSY: "BUSY"}.get(
            status, str(status)
        )
        print(f"  [notify 0x53] status={label} progress={progress}%")
        if status in (STATUS_OK, STATUS_ERR):
            self.event.set()


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


async def send_book(
    address: str,
    book_path: Path,
    chunk_size: int,
    use_write_response: bool,
    packet_delay_ms: float,
    commit_timeout: float,
) -> None:
    data = book_path.read_bytes()
    if len(data) == 0:
        raise ValueError("电子书文件为空")
    if len(data) > MAX_BOOK_BYTES:
        raise ValueError(
            f"文件过大 ({len(data)} B)，上限 {MAX_BOOK_BYTES} B"
        )
    try:
        text = data.decode("utf-8-sig")
    except UnicodeDecodeError as exc:
        raise ValueError("电子书必须是 UTF-8") from exc
    if "@page" not in text:
        print("警告: 文件里没有 @page，设备解析可能会失败")

    crc = esp_crc32(data)
    print(f"文件: {book_path}  大小: {len(data)} B  CRC32: 0x{crc:08X}")

    waiter = StatusWaiter()

    def on_notify(_handle: int, payload: bytearray) -> None:
        waiter.feed(payload)

    async with BleakClient(address, timeout=20.0) as client:
        print(f"已连接: {client.is_connected}")
        await client.start_notify(CHR_UUID, on_notify)

        info = (
            bytes([CMD_BOOK_INFO])
            + pack_u32_le(len(data))
            + pack_u32_le(crc)
            + bytes([FMT_TXT])
        )
        await client.write_gatt_char(CHR_UUID, info, response=True)
        print("已发送 0x50 INFO")

        offset = 0
        total = len(data)
        while offset < total:
            piece = data[offset : offset + chunk_size]
            pkt = bytes([CMD_BOOK_DATA]) + pack_u32_le(offset) + piece
            await client.write_gatt_char(
                CHR_UUID, pkt, response=use_write_response
            )
            offset += len(piece)
            pct = offset * 100 // total
            print(f"\r  传输进度: {offset}/{total} ({pct}%)", end="", flush=True)
            if packet_delay_ms > 0:
                await asyncio.sleep(packet_delay_ms / 1000.0)
        print()

        await client.write_gatt_char(
            CHR_UUID, bytes([CMD_BOOK_COMMIT]), response=True
        )
        print("已发送 0x52 COMMIT，等待设备校验 ...")

        try:
            await asyncio.wait_for(waiter.event.wait(), timeout=commit_timeout)
        except asyncio.TimeoutError:
            print(
                f"等待 {commit_timeout:.0f}s 未收到 OK/ERR NOTIFY。"
                " 可加大 --packet-delay-ms 后重试。"
            )
            return

        if waiter.last is None:
            return
        status, _ = waiter.last
        if status == STATUS_OK:
            print("传输成功。请在设备上进入 主页 -> E-book 查看。")
        elif status == STATUS_ERR:
            raise RuntimeError("设备返回 ERR，请查看串口 book / REPPO_BLE 日志")
        else:
            print("收到 BUSY 但未结束，请加大 --packet-delay-ms 重试")


def parse_args() -> argparse.Namespace:
    p = argparse.ArgumentParser(description="Reppo BLE 电子书上传")
    p.add_argument("book", type=Path, help="UTF-8 book.txt 路径")
    p.add_argument("--address", "-a", help="设备 BLE MAC")
    p.add_argument("--name", default=DEVICE_NAME, help="扫描用设备名")
    p.add_argument("--scan-timeout", type=float, default=12.0)
    p.add_argument("--chunk-size", type=int, default=MAX_CHUNK)
    p.add_argument(
        "--write-with-response",
        action="store_true",
        help="DATA 包也使用 Write Request",
    )
    p.add_argument("--packet-delay-ms", type=float, default=8.0)
    p.add_argument("--commit-timeout", type=float, default=30.0)
    return p.parse_args()


def main() -> None:
    args = parse_args()
    if not args.book.is_file():
        print(f"文件不存在: {args.book}", file=sys.stderr)
        sys.exit(1)
    if args.chunk_size < 1 or args.chunk_size > MAX_CHUNK:
        print(f"--chunk-size 须在 1..{MAX_CHUNK}", file=sys.stderr)
        sys.exit(1)

    async def run() -> None:
        addr = await find_device(args.name, args.address, args.scan_timeout)
        await send_book(
            addr,
            args.book,
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
