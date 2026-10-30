import contextlib
import importlib.util
import io
import json
import os
from pathlib import Path
import queue
import tempfile
import unittest
from unittest.mock import patch
import wave

SCRIPT = Path(__file__).resolve().parents[3] / "scripts/test_aispeech_direct.py"
spec = importlib.util.spec_from_file_location("reppo_direct_asr", SCRIPT)
direct = importlib.util.module_from_spec(spec)
spec.loader.exec_module(direct)


def recording():
    buffer = io.BytesIO()
    with wave.open(buffer, "wb") as wav:
        wav.setframerate(16000)
        wav.setnchannels(1)
        wav.setsampwidth(2)
        wav.writeframes(b"\x00\x00" * 1600)
    return buffer.getvalue()


class FakeWebSocket:
    def __init__(self, final=None):
        self.incoming = queue.Queue()
        self.incoming.put({"errno": 7})
        self.frames = []
        self.command = None
        self.closed = False
        self.final = final or {"errno": 9, "data": {"bg": 0, "onebest": "准备答辩25分钟"}}

    def send(self, command):
        self.command = json.loads(command)

    def recv(self):
        return json.dumps(self.incoming.get(timeout=1)).encode()

    def send_binary(self, data):
        self.frames.append(data)
        if not data:
            self.incoming.put({"errno": 8, "data": {"var": "不要拼接临时结果"}})
            self.incoming.put({"errno": 0, "data": {"bg": 0, "onebest": "准备答辩"}})
            self.incoming.put(self.final)

    def close(self):
        self.closed = True


class DirectASRTests(unittest.TestCase):
    def test_inherits_system_proxy_without_changing_system_settings(self):
        with patch.object(direct.urllib.request, "proxy_bypass", return_value=False), \
             patch.object(direct.urllib.request, "getproxies", return_value={
                 "https": "http://127.0.0.1:7897"}):
            self.assertEqual(direct.system_proxy_options(), {
                "http_proxy_host": "127.0.0.1", "http_proxy_port": 7897, "proxy_type": "http"})
        with patch.object(direct.urllib.request, "proxy_bypass", return_value=True):
            self.assertEqual(direct.system_proxy_options(), {})

    def test_realtime_protocol_pacing_and_final_segments(self):
        fake = FakeWebSocket()
        audio = recording()
        with patch.object(direct.websocket, "create_connection", return_value=fake) as connect, \
             patch.object(direct.time, "sleep") as sleep:
            result = direct.realtime(audio, "product", "private-test-key")
        url = connect.call_args.args[0]
        self.assertIn("apikey=private-test-key", url)
        self.assertIn("res=aitranson", url)
        self.assertTrue(connect.call_args.kwargs["sslopt"]["ca_certs"])
        self.assertEqual(fake.command["command"], "start")
        self.assertEqual(fake.command["params"]["audio"]["sampleRate"], 16000)
        self.assertEqual(b"".join(fake.frames), audio)
        self.assertEqual(fake.frames[-1], b"")
        self.assertTrue(all(len(chunk) <= 1280 for chunk in fake.frames))
        self.assertTrue(sleep.called)
        self.assertEqual(result["text"], "准备答辩25分钟")
        self.assertEqual(result["event_codes"], [7, 8, 0, 9])
        self.assertTrue(fake.closed)

    def test_vendor_errors_close_connection(self):
        fake = FakeWebSocket(final={"errno": 11, "error": "sensitive server diagnostic"})
        with patch.object(direct.websocket, "create_connection", return_value=fake), \
             patch.object(direct.time, "sleep"), self.assertRaises(direct.ASRError) as error:
            direct.realtime(recording(), "product", "private-test-key")
        self.assertEqual(error.exception.code, "realtime_vendor_error_11")
        self.assertTrue(fake.closed)

    def test_cli_uses_local_config_and_sanitizes_exception(self):
        with tempfile.TemporaryDirectory() as folder:
            folder = Path(folder)
            wav = folder / "recording.wav"
            wav.write_bytes(recording())
            env = folder / "aispeech.env"
            env.write_text("AISPEECH_PRODUCT_ID=product\nAISPEECH_API_KEY=private-test-key\n")
            output = io.StringIO()
            with patch.dict(os.environ, {}, clear=True), \
                 patch.object(direct.sys, "argv", [str(SCRIPT), str(wav), "--env", str(env)]), \
                 patch.object(direct.websocket, "create_connection", side_effect=OSError(
                     "https://example?apikey=private-test-key")), contextlib.redirect_stdout(output):
                self.assertEqual(direct.main(), 1)
            self.assertNotIn("private-test-key", output.getvalue())
            self.assertEqual(json.loads(output.getvalue())["error"], "OSError")


if __name__ == "__main__":
    unittest.main()
