"""Run production firmware ASR routing/transport against a simulated HTTP client."""
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest


class VoiceASRTests(unittest.TestCase):
    def run_mode(self, provider, fallback):
        root = Path(__file__).resolve().parents[2]
        fixture = Path(__file__).parent / "voice_asr"
        candidates = [Path(os.environ.get("IDF_PATH", "/nonexistent")),
                      Path.home() / ".espressif/v5.3.5/esp-idf", Path.home() / "esp/esp-idf"]
        cjson = next((p / "components/json/cJSON" for p in candidates
                      if (p / "components/json/cJSON/cJSON.c").is_file()), None)
        if cjson is None:
            self.skipTest("Set IDF_PATH to an ESP-IDF checkout containing cJSON")
        compiler = shutil.which("cc")
        self.assertIsNotNone(compiler)
        with tempfile.TemporaryDirectory(prefix="reppo-asr-test-") as temp:
            temp = Path(temp)
            # Isolate from the developer's ignored local configuration/credentials.
            for name in ("voice_asr.c", "voice_asr.h", "voice_asr_config.h"):
                shutil.copy(root / "main" / name, temp / name)
            result = subprocess.run([
                compiler, "-std=c11", "-Wall", "-Wextra", "-Werror", "-I", str(temp),
                "-I", str(fixture / "stubs"), "-I", str(cjson),
                f"-DVOICE_ASR_PROVIDER={provider}", f"-DVOICE_ASR_ALLOW_QWEN_FALLBACK={fallback}",
                '-DVOICE_ASR_PROXY_URL="https://proxy.test/v1/asr/transcribe"',
                '-DVOICE_ASR_DEVICE_TOKEN="test-token"', str(temp / "voice_asr.c"),
                str(fixture / "host_test.c"), str(cjson / "cJSON.c"), "-lm", "-o", str(temp / "test"),
            ], capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            result = subprocess.run([str(temp / "test")], capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_existing_qwen_primary(self):
        self.run_mode(0, 1)

    def test_aispeech_strict(self):
        self.run_mode(1, 0)

    def test_aispeech_with_qwen_fallback(self):
        self.run_mode(1, 1)


if __name__ == "__main__":
    unittest.main()
