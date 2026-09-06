"""Phone-time sender checks without a BLE adapter or third-party packages.

Run: python3 -m unittest discover -s scripts/tests -p 'test_ble_send_phone_time.py'
"""

import asyncio
import contextlib
import datetime
import importlib.util
import io
from pathlib import Path
import struct
import sys
import types
import unittest
from unittest import mock


fake_bleak = types.ModuleType("bleak")
fake_bleak.BleakClient = mock.Mock()
fake_bleak.BleakScanner = mock.Mock()
spec = importlib.util.spec_from_file_location(
    "ble_send_phone_time", Path(__file__).resolve().parents[1] / "ble_send_phone_time.py"
)
sender = importlib.util.module_from_spec(spec)
with mock.patch.dict(sys.modules, {"bleak": fake_bleak}):
    spec.loader.exec_module(sender)


class PhoneTimePacketTests(unittest.TestCase):
    def test_sunday_and_monday_use_phone_local_fields(self):
        for day, weekday in ((6, 0), (7, 1)):
            with self.subTest(day=day):
                local_time = datetime.datetime(
                    2026, 9, day, 0, 3, 4,
                    tzinfo=datetime.timezone(datetime.timedelta(hours=8)),
                )
                self.assertEqual(
                    sender.pack_phone_time(local_time),
                    b"\x10" + struct.pack("<HBBBBBB", 2026, 9, day, 0, 3, 4, weekday),
                )


class PhoneTimeConnectionTests(unittest.IsolatedAsyncioTestCase):
    def setUp(self):
        self.events = []
        self.notifications = [b"\x11\x00"]
        self.subscription_error = None
        self.packet = None
        self.now = datetime.datetime(2026, 9, 6, 12, 34, 56)
        self.output = io.StringIO()
        self.enterContext(contextlib.redirect_stdout(self.output))
        test = self

        class FakeClock:
            @staticmethod
            def now():
                test.events.append("now")
                return test.now

        class FakeClient:
            def __init__(self, address, timeout):
                test.assertEqual(address, "fake-address")
                self.on_notify = None

            async def __aenter__(self):
                test.events.append("connected")
                return self

            async def __aexit__(self, *_args):
                test.events.append("disconnected")

            async def start_notify(self, uuid, callback):
                test.assertEqual(uuid, sender.CHR_UUID)
                test.events.append("subscribed")
                if test.subscription_error:
                    raise test.subscription_error
                self.on_notify = callback

            async def write_gatt_char(self, uuid, packet, response):
                test.assertEqual(uuid, sender.CHR_UUID)
                test.assertTrue(response)
                test.events.append("write")
                test.packet = packet
                for notification in test.notifications:
                    self.on_notify(3, bytearray(notification))

        self.enterContext(mock.patch.object(sender, "BleakClient", FakeClient))
        self.enterContext(mock.patch.object(sender, "datetime", types.SimpleNamespace(datetime=FakeClock)))

    async def test_time_is_sampled_after_subscribe_immediately_before_write(self):
        await sender.send_phone_time("fake-address")
        self.assertEqual(self.events, ["connected", "subscribed", "now", "write", "disconnected"])
        self.assertEqual(self.packet, sender.pack_phone_time(self.now))
        self.assertIn("0x11 OK", self.output.getvalue())

    async def test_subscription_error_prevents_sampling_or_writing(self):
        self.subscription_error = RuntimeError("subscription failed")
        with self.assertRaisesRegex(RuntimeError, "未发送时间"):
            await sender.send_phone_time("fake-address")
        self.assertEqual(self.events, ["connected", "subscribed", "disconnected"])
        self.assertIsNone(self.packet)

    async def test_device_write_error_is_failure(self):
        self.notifications = [b"\x11\x01"]
        with self.assertRaisesRegex(RuntimeError, "0x11 ERR"):
            await sender.send_phone_time("fake-address")
        self.assertNotIn("0x11 OK", self.output.getvalue())

    async def test_unknown_device_status_is_failure(self):
        self.notifications = [b"\x11\x02"]
        with self.assertRaisesRegex(RuntimeError, "未知时间同步状态 0x02"):
            await sender.send_phone_time("fake-address")
        self.assertNotIn("0x11 OK", self.output.getvalue())

    async def test_missing_ack_is_failure_and_does_not_claim_rtc_sync(self):
        self.notifications = []
        with mock.patch.object(sender, "TIME_STATUS_TIMEOUT_SECONDS", 0):
            with self.assertRaisesRegex(RuntimeError, "超时，未确认 RTC 写入成功"):
                await sender.send_phone_time("fake-address")
        self.assertNotIn("0x11 OK", self.output.getvalue())

    async def test_unrelated_and_truncated_notifications_are_not_success(self):
        self.notifications = [b"", b"\x11", b"\x04\x00"]
        with mock.patch.object(sender, "TIME_STATUS_TIMEOUT_SECONDS", 0):
            with self.assertRaisesRegex(RuntimeError, "未确认 RTC 写入成功"):
                await sender.send_phone_time("fake-address")

    async def test_duplicate_status_is_safe_and_unrelated_notify_is_ignored(self):
        self.notifications = [b"\x04\x00", b"\x11\x00", b"\x11\x00"]
        await sender.send_phone_time("fake-address")
        self.assertIn("0x11 OK", self.output.getvalue())

    async def test_discovery_and_connection_use_same_event_loop(self):
        loops = []

        async def discover(_address):
            loops.append(asyncio.get_running_loop())
            return "fake-address"

        async def send(address):
            self.assertEqual(address, "fake-address")
            loops.append(asyncio.get_running_loop())

        with mock.patch.object(sender, "find_device", discover):
            with mock.patch.object(sender, "send_phone_time", send):
                await sender.sync_phone_time(None)
        self.assertEqual(len(loops), 2)
        self.assertIs(loops[0], loops[1])


if __name__ == "__main__":
    unittest.main()
