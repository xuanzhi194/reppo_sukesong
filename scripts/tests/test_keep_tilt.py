"""Replay deterministic IMU trajectories, independent of LVGL/board hardware."""
from pathlib import Path
import os
import shutil
import subprocess
import tempfile
import unittest


class KeepTiltTests(unittest.TestCase):
    def test_gesture_trajectories(self):
        root = Path(__file__).resolve().parents[2]
        with tempfile.TemporaryDirectory(prefix="reppo-keep-tilt-") as temp:
            binary = Path(temp) / "tilt_test"
            build = subprocess.run([
                shutil.which("cc"), "-std=c11", "-O2", "-g", "-Wall", "-Wextra", "-Werror",
                "-fsanitize=address,undefined", "-I", str(root / "main"),
                str(root / "main/keep_tilt.c"), str(root / "scripts/tests/keep_machine/tilt_test.c"),
                "-lm", "-o", str(binary)], capture_output=True, text=True)
            self.assertEqual(build.returncode, 0, build.stdout + build.stderr)
            env = dict(os.environ, ASAN_OPTIONS="detect_leaks=0:halt_on_error=1",
                       UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1")
            result = subprocess.run([str(binary)], capture_output=True, text=True, env=env, timeout=30)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            print(result.stdout, end="")


if __name__ == "__main__":
    unittest.main()
