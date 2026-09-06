#!/usr/bin/env python3
"""Compile and exercise the actual RTC driver without ESP-IDF or hardware."""
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest


class Rx8130DriverTests(unittest.TestCase):
    def test_stop_recovery_under_i2c_failures(self):
        compiler = shutil.which("cc")
        self.assertIsNotNone(compiler, "A host C compiler (cc) is required")
        root = Path(__file__).resolve().parents[2]
        fixture = Path(__file__).resolve().parent / "rx8130"
        driver = root / "components" / "BSP" / "RX8130"
        with tempfile.TemporaryDirectory(prefix="reppo-rx8130-test-") as temp:
            binary = Path(temp) / "rx8130_driver_test"
            build = subprocess.run(
                [compiler, "-std=c11", "-D_POSIX_C_SOURCE=200809L", "-Wall", "-Wextra", "-Werror",
                 "-I", str(fixture / "stubs"), "-I", str(driver),
                 str(driver / "rx8130.c"), str(fixture / "driver_test.c"), "-o", str(binary)],
                text=True, capture_output=True,
            )
            self.assertEqual(build.returncode, 0, build.stdout + build.stderr)
            result = subprocess.run([str(binary)], text=True, capture_output=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            print(result.stdout, end="")


if __name__ == "__main__":
    unittest.main()
