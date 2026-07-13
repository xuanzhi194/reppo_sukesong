#!/usr/bin/env python3
"""
Reppo 单词书 BLE 上传测试脚本

模拟微信小程序向 REPPO 设备上传 CSV 单词书。设备端协议:
    0x40 WORD_INFO   payload = total_size(4) + crc32(4) + word_count(4) + format(1)
    0x41 WORD_DATA   payload = offset(4) + chunk
    0x42 WORD_COMMIT payload = empty
    0x43 WORD_STATUS notify  = status(1) + progress(1) + detail(1)

依赖:
    pip install bleak

示例:
    python3 scripts/ble_send_wordbook.py scripts/user_wordbook_sample.csv --dry-run
    python3 scripts/ble_send_wordbook.py scripts/user_wordbook_sample.csv
    python3 scripts/ble_send_wordbook.py --address AA:BB:CC:DD:EE:FF scripts/user_wordbook_sample.csv
"""

from __future__ import annotations

import argparse
import asyncio
import struct
import sys
import zlib
from pathlib import Path

DEVICE_NAME = "REPPO_BLE_Device"
CHR_UUID = "0000ff01-0000-1000-8000-00805f9b34fb"

CMD_WORD_INFO = 0x40
CMD_WORD_DATA = 0x41
CMD_WORD_COMMIT = 0x42
CMD_WORD_STATUS = 0x43

WORD_FMT_CSV = 1
STATUS_OK = 0
STATUS_ERR = 1
STATUS_BUSY = 2
STATUS_PROGRESS = 3

MAX_CHUNK = 251
MAX_WORD_BYTES = 768 * 1024
MAX_WORDS = 10000
MAX_LINE_BYTES = 255


def esp_crc32(data: bytes) -> int:
    return zlib.crc32(data) & 0xFFFFFFFF


def pack_u32_le(value: int) -> bytes:
    return struct.pack("<I", value)


def load_and_validate_wordbook(path: Path) -> tuple[bytes, int, int]:
    data = path.read_bytes()
    if not data:
        raise ValueError("词书文件为空")
    if len(data) > MAX_WORD_BYTES:
        raise ValueError(
            f"词书过大: {len(data)} B，设备端上限 {MAX_WORD_BYTES} B"
        )

    try:
        text = data.decode("utf-8-sig")
    except UnicodeDecodeError as exc:
        raise ValueError("词书必须是 UTF-8 CSV") from exc

    word_count = 0
    for line_no, raw_line in enumerate(text.splitlines(), start=1):
        line = raw_line.strip("\r\n")
        if not line:
            continue
        if len(raw_line.encode("utf-8")) > MAX_LINE_BYTES:
            raise ValueError(f"第 {line_no} 行超过 {MAX_LINE_BYTES} 字节")
        parts = line.split(",", 2)
        if len(parts) < 3 or not parts[0] or not parts[2]:
            raise ValueError(
                f"第 {line_no} 行格式错误，应为 word,phonetic,definition"
            )
        word_count += 1
        if word_count > MAX_WORDS:
            raise ValueError(f"词数超过 {MAX_WORDS}")

    if word_count == 0:
        raise ValueError("词书没有有效单词行")

    return data, word_count, esp_crc32(data)


class WordStatusWaiter:
    def __init__(self) -> None:
        self.event = asyncio.Event()
        self.last: tuple[int, int, int] | None = None

    def feed(self, data: bytearray) -> None:
        if len(data) < 2 or data[0] != CMD_WORD_STATUS:
            return
        status = data[1]
        progress = data[2] if len(data) > 2 else 0
        detail = data[3] if len(data) > 3 else 0
        self.last = (status, progress, detail)
        label = {
            STATUS_OK: "OK",
            STATUS_ERR: "ERR",
            STATUS_BUSY: "BUSY",
            STATUS_PROGRESS: "PROGRESS",
        }.get(status, str(status))
        print(
            f"  [notify 0x43] status={label} progress={progress}% detail={detail}"
        )
        if status in (STATUS_OK, STATUS_ERR):
            self.event.set()


async def import_bleak():
    try:
        from bleak import BleakClient, BleakScanner
    except ImportError:
        print("请先安装 bleak:  pip install bleak", file=sys.stderr)
        raise
    return BleakClient, BleakScanner


async def find_device(name: str, address: str | None, timeout: float) -> str:
    if address:
        return address

    _, BleakScanner = await import_bleak()
    print(f"扫描 BLE 设备 ({timeout:.0f}s)，查找 {name!r} ...")
    device = await BleakScanner.find_device_by_filter(
        lambda d, ad: (d.name and d.name == name)
        or (ad.local_name and ad.local_name == name),
        timeout=timeout,
    )
    if device is None:
        raise RuntimeError(f"未找到 {name!r}，请先在设备 screen_setting 打开蓝牙")
    print(f"找到: {device.name} [{device.address}]")
    return device.address


async def send_wordbook(
    address: str,
    data: bytes,
    word_count: int,
    crc32: int,
    chunk_size: int,
    use_write_response: bool,
    packet_delay_ms: float,
    commit_timeout: float,
) -> None:
    BleakClient, _ = await import_bleak()
    waiter = WordStatusWaiter()

    def on_notify(_handle: int, payload: bytearray) -> None:
        waiter.feed(payload)

    async with BleakClient(address, timeout=20.0) as client:
        print(f"已连接: {client.is_connected}")
        await client.start_notify(CHR_UUID, on_notify)

        info = (
            bytes([CMD_WORD_INFO])
            + pack_u32_le(len(data))
            + pack_u32_le(crc32)
            + pack_u32_le(word_count)
            + bytes([WORD_FMT_CSV])
        )
        await client.write_gatt_char(CHR_UUID, info, response=True)
        print("已发送 0x40 WORD_INFO")

        offset = 0
        total = len(data)
        while offset < total:
            piece = data[offset : offset + chunk_size]
            packet = bytes([CMD_WORD_DATA]) + pack_u32_le(offset) + piece
            await client.write_gatt_char(
                CHR_UUID, packet, response=use_write_response
            )
            offset += len(piece)
            pct = offset * 100 // total
            print(f"\r  传输进度: {offset}/{total} ({pct}%)", end="", flush=True)
            if packet_delay_ms > 0:
                await asyncio.sleep(packet_delay_ms / 1000.0)
        print()

        commit_write_error: Exception | None = None
        try:
            await client.write_gatt_char(
                CHR_UUID, bytes([CMD_WORD_COMMIT]), response=True
            )
        except Exception as exc:
            commit_write_error = exc
        print("已发送 0x42 WORD_COMMIT，等待设备校验并覆盖词书 ...")

        try:
            await asyncio.wait_for(waiter.event.wait(), timeout=commit_timeout)
        except asyncio.TimeoutError:
            raise RuntimeError(
                f"等待 {commit_timeout:.0f}s 未收到 0x43 OK/ERR，请查看设备日志"
            )

        if waiter.last is None:
            if commit_write_error is not None:
                raise RuntimeError(f"COMMIT 写入返回错误: {commit_write_error}")
            return
        status, _, detail = waiter.last
        if status == STATUS_OK:
            print("词书上传成功，设备已覆盖 /spiffs/wordcards.csv 并重建索引。")
        else:
            msg = f"设备返回 ERR，detail={detail}"
            if commit_write_error is not None:
                msg += f"，GATT={commit_write_error}"
            raise RuntimeError(msg)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Reppo BLE 单词书上传测试")
    parser.add_argument("csv", type=Path, help="CSV 单词书路径")
    parser.add_argument("--address", "-a", help="设备 BLE MAC，不填则按名称扫描")
    parser.add_argument("--name", default=DEVICE_NAME, help="扫描用设备名")
    parser.add_argument("--scan-timeout", type=float, default=12.0)
    parser.add_argument("--chunk-size", type=int, default=MAX_CHUNK)
    parser.add_argument(
        "--write-with-response",
        action="store_true",
        help="DATA 包使用 Write Request，更稳但更慢",
    )
    parser.add_argument(
        "--packet-delay-ms",
        type=float,
        default=8.0,
        help="DATA 包之间的间隔毫秒",
    )
    parser.add_argument("--commit-timeout", type=float, default=30.0)
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="只校验词书并打印协议信息，不连接蓝牙",
    )
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    if not args.csv.is_file():
        print(f"文件不存在: {args.csv}", file=sys.stderr)
        sys.exit(1)
    if args.chunk_size < 1 or args.chunk_size > MAX_CHUNK:
        print(f"--chunk-size 须在 1..{MAX_CHUNK}", file=sys.stderr)
        sys.exit(1)

    try:
        data, word_count, crc32 = load_and_validate_wordbook(args.csv)
    except Exception as exc:
        print(f"词书校验失败: {exc}", file=sys.stderr)
        sys.exit(1)

    packets = (len(data) + args.chunk_size - 1) // args.chunk_size
    print(f"词书: {args.csv}")
    print(f"大小: {len(data)} B")
    print(f"词数: {word_count}")
    print(f"CRC32: 0x{crc32:08X}")
    print(f"DATA 包数: {packets}，chunk={args.chunk_size} B")

    if args.dry_run:
        print("dry-run 完成，未连接设备。")
        return

    async def run() -> None:
        address = await find_device(args.name, args.address, args.scan_timeout)
        await send_wordbook(
            address,
            data,
            word_count,
            crc32,
            chunk_size=args.chunk_size,
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
