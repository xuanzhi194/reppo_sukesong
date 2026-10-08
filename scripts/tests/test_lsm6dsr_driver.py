#!/usr/bin/env python3
"""Compile and exercise the LSM6DSR driver without ESP-IDF or hardware."""
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest


class Lsm6dsrDriverTests(unittest.TestCase):
    def test_probe_and_solder_check(self):
        compiler = shutil.which("cc")
        self.assertIsNotNone(compiler, "A host C compiler (cc) is required")
        root = Path(__file__).resolve().parents[2]
        fixture = Path(__file__).resolve().parent / "lsm6dsr"
        driver = root / "components" / "BSP" / "LSM6DSR"
        with tempfile.TemporaryDirectory(prefix="reppo-lsm6dsr-test-") as temp:
            binary = Path(temp) / "lsm6dsr_driver_test"
            build = subprocess.run(
                [compiler, "-std=c11", "-D_POSIX_C_SOURCE=200809L", "-Wall", "-Wextra",
                 "-Werror", "-I", str(fixture / "stubs"), "-I", str(driver),
                 str(driver / "lsm6dsr.c"), str(fixture / "driver_test.c"),
                 "-lm", "-o", str(binary)],
                text=True, capture_output=True,
            )
            self.assertEqual(build.returncode, 0, build.stdout + build.stderr)
            result = subprocess.run([str(binary)], text=True, capture_output=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            print(result.stdout, end="")


if __name__ == "__main__":
    unittest.main()
