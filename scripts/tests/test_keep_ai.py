"""Personal summary inputs, reply validation and notebook channel isolation."""
from pathlib import Path
import os
import shutil
import subprocess
import tempfile
import unittest


class KeepAITests(unittest.TestCase):
    def test_personal_summary_data_and_channels(self):
        root = Path(__file__).resolve().parents[2]
        idf = Path(os.environ.get("IDF_PATH", Path.home() / ".espressif/v5.3.5/esp-idf"))
        cjson = idf / "components/json/cJSON"
        self.assertTrue((cjson / "cJSON.c").exists(), "Set IDF_PATH to locate cJSON")
        with tempfile.TemporaryDirectory(prefix="reppo-keep-ai-") as temp:
            binary = Path(temp) / "test"
            command = [shutil.which("cc"), "-std=c11", "-O1", "-g",
                       "-fsanitize=address,undefined", "-fno-omit-frame-pointer",
                       "-DLV_KCONFIG_IGNORE=1",
                       f"-DLV_CONF_PATH={root / 'scripts/tests/game_machine/lv_conf.h'}"]
            for folder in [root / "main", root / "components/lvgl", root / "components/lvgl/src",
                           root / "components/ui_custom/custom", root / "components/ui_custom/generated", cjson]:
                command += ["-I", str(folder)]
            command += [str(root / "scripts/tests/keep_machine/ai_test.c"),
                        str(root / "main/inspiration_service.c"), str(root / "main/inspiration_storage.c"),
                        str(cjson / "cJSON.c"), "-lm", "-o", str(binary)]
            built = subprocess.run(command, text=True, capture_output=True)
            self.assertEqual(built.returncode, 0, built.stdout + built.stderr)
            env = dict(os.environ, ASAN_OPTIONS="detect_leaks=1:halt_on_error=1",
                       UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1")
            # Apple ASan does not support leak detection.
            if os.uname().sysname == "Darwin":
                env["ASAN_OPTIONS"] = "detect_leaks=0:halt_on_error=1"
            run = subprocess.run([str(binary), str(Path(temp) / "ideas.dat")],
                                 text=True, capture_output=True, env=env, timeout=30)
            self.assertEqual(run.returncode, 0, run.stdout + run.stderr)
            print(run.stdout, end="")


if __name__ == "__main__":
    unittest.main()
