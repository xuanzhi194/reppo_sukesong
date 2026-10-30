import importlib.util
import io
import json
import os
from pathlib import Path
import sys
import subprocess
import tempfile
import threading
import unittest
from unittest.mock import patch
import wave

import httpx
from werkzeug.serving import make_server

PROXY = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(PROXY))
import aispeech_asr

# Do not load local .env credentials during offline tests.
spec = importlib.util.spec_from_file_location("reppo_test_proxy", PROXY / "app.py")
proxy = importlib.util.module_from_spec(spec)
with patch("os.path.isfile", return_value=False), patch.dict(os.environ, {
    "DEVICE_TOKEN": "test-device-token", "AISPEECH_PRODUCT_ID": "test-product",
    "AISPEECH_API_KEY": "test-api-key", "AISPEECH_LANG": "ce",
}):
    spec.loader.exec_module(proxy)


def wav_bytes(rate=16000, channels=1, frames=1600):
    buffer = io.BytesIO()
    with wave.open(buffer, "wb") as wav:
        wav.setnchannels(channels)
        wav.setsampwidth(2)
        wav.setframerate(rate)
        wav.writeframes(b"\x00\x00" * channels * frames)
    return buffer.getvalue()


class ASRTests(unittest.TestCase):
    def setUp(self):
        self.client = proxy.app.test_client()
        self.headers = {"Authorization": "Bearer test-device-token", "Content-Type": "audio/wav"}
        self.real_client = httpx.Client

    def mock_upstream(self, handler):
        return patch.object(aispeech_asr.httpx, "Client", side_effect=lambda **kwargs:
            self.real_client(transport=httpx.MockTransport(handler), **kwargs))

    def post(self, audio=None, **kwargs):
        return self.client.post("/v1/asr/transcribe", data=wav_bytes() if audio is None else audio,
                                headers=self.headers, **kwargs)

    def test_actual_multipart_contract_and_sentence_concatenation(self):
        audio = wav_bytes()
        def handler(request):
            self.assertEqual(request.url.params["productId"], "test-product")
            self.assertEqual(request.url.params["apiKey"], "test-api-key")
            self.assertNotIn("authorization", request.headers)
            self.assertIn("multipart/form-data; boundary=", request.headers["content-type"])
            body = request.read()
            self.assertIn(b'name="file"; filename="recording.wav"', body)
            self.assertIn(audio, body)
            params = next(part.split(b"\r\n\r\n", 1)[1].strip() for part in body.split(
                b"--" + request.headers["content-type"].split("boundary=")[1].encode()) if b'name="params"' in part)
            params = json.loads(params)
            self.assertEqual(params["audio"], {"audio_type": "wav", "sample_rate": 16000,
                                              "channel": 1, "sample_bytes": 2})
            self.assertEqual(params["asr"]["lang"], "ce")
            self.assertTrue(params["asr"]["use_itn"])
            return httpx.Response(200, json={"errno": 0, "data": {"result": [
                {"onebest": "准备答辩"}, {"onebest": "25分钟"}]}})
        with self.mock_upstream(handler):
            result = self.post(audio)
        self.assertEqual(result.status_code, 200)
        self.assertEqual(result.json, {"text": "准备答辩25分钟", "provider": "aispeech"})

    def test_rejects_unauthenticated_requests_before_contacting_vendor(self):
        with patch.object(proxy, "transcribe") as call:
            result = self.client.post("/v1/asr/transcribe", data=wav_bytes())
        self.assertEqual(result.status_code, 401)
        call.assert_not_called()

    def test_input_validation_before_network(self):
        with patch.object(aispeech_asr.httpx, "Client") as client:
            for audio in (b"", b"not WAV", wav_bytes(rate=8000), wav_bytes(channels=2),
                          wav_bytes(frames=960001), wav_bytes()[:-2]):
                with self.subTest(size=len(audio)):
                    self.assertEqual(self.post(audio).status_code, 400)
            result = self.post(b"x" * (aispeech_asr.MAX_AUDIO_BYTES + 1))
            self.assertEqual(result.status_code, 413)
        client.assert_not_called()

    def test_content_type_and_missing_configuration(self):
        result = self.client.post("/v1/asr/transcribe", json={"audio": "bad"},
                                  headers={"Authorization": "Bearer test-device-token"})
        self.assertEqual(result.status_code, 415)
        with patch.object(proxy, "AISPEECH_API_KEY", ""):
            self.assertEqual(self.post().status_code, 503)

    def test_upstream_http_errors_do_not_leak_key_or_body(self):
        for status, code in ((401, "aispeech_auth_failed"), (403, "aispeech_quota_or_rate_limit"),
                             (500, "aispeech_http_error")):
            with self.subTest(status=status), self.mock_upstream(lambda req: httpx.Response(
                status, text="sensitive-upstream-body test-api-key")):
                result = self.post()
                self.assertEqual(result.status_code, 502)
                self.assertEqual(result.json["error"], code)
                self.assertEqual(result.json["upstream_status"], status)
                self.assertNotIn(b"test-api-key", result.data)
                self.assertNotIn(b"sensitive", result.data)

    def test_business_errors_empty_and_malformed_responses(self):
        cases = [("bad JSON", 502), ({"errno": 521, "error": "secret"}, 502),
                 ({"errno": 0, "data": {"result": []}}, 422),
                 ({"errno": 0, "data": {"result": [{"onebest": 123}]}}, 502),
                 ({"errno": 0, "data": None}, 502), ([], 502)]
        for body, status in cases:
            def handler(req):
                return httpx.Response(200, text=body) if isinstance(body, str) else httpx.Response(200, json=body)
            with self.subTest(body=body), self.mock_upstream(handler):
                self.assertEqual(self.post().status_code, status)

    def test_timeout_and_network_errors_are_sanitized(self):
        for exception, status in ((httpx.ReadTimeout, 504), (httpx.ConnectError, 502)):
            def handler(req):
                raise exception("secret " + str(req.url), request=req)
            with self.subTest(exception=exception), self.mock_upstream(handler):
                result = self.post()
                self.assertEqual(result.status_code, status)
                self.assertNotIn(b"test-api-key", result.data)

    def test_health_and_existing_summary_still_work(self):
        health = self.client.get("/health")
        self.assertTrue(health.json["aispeech_configured"])
        self.assertNotIn(b"test-api-key", health.data)
        with patch.object(proxy, "DEEPSEEK_API_KEY", "test-deepseek"), self.mock_upstream(
            lambda req: httpx.Response(200, json={"choices": [{"message": {"content": "summary"}}]})
        ):
            result = self.client.post("/v1/ai/summary", json={"prompt": "hello"},
                                      headers={"Authorization": "Bearer test-device-token"})
        self.assertEqual(result.status_code, 200)
        self.assertEqual(result.json["content"], "summary")

    def test_cli_through_real_local_http_route(self):
        server = make_server("127.0.0.1", 0, proxy.app)
        thread = threading.Thread(target=server.serve_forever, daemon=True)
        thread.start()
        try:
            with tempfile.TemporaryDirectory() as temp, self.mock_upstream(
                lambda req: httpx.Response(200, json={"errno": 0, "data": {
                    "result": [{"onebest": "准备答辩25分钟"}]}})
            ):
                wav = Path(temp) / "recording.wav"
                wav.write_bytes(wav_bytes())
                result = subprocess.run([
                    sys.executable, str(PROXY.parents[1] / "scripts/test_aispeech_asr.py"), str(wav),
                    "--url", f"http://127.0.0.1:{server.server_port}/v1/asr/transcribe",
                ], capture_output=True, text=True, timeout=10,
                    env={**os.environ, "DEVICE_TOKEN": "test-device-token"})
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                output = json.loads(result.stdout)
                self.assertEqual(output["provider"], "aispeech")
                self.assertEqual(output["text"], "准备答辩25分钟")
                self.assertGreaterEqual(output["elapsed_ms"], 0)
        finally:
            server.shutdown()
            server.server_close()
            thread.join(timeout=2)


if __name__ == "__main__":
    unittest.main()
