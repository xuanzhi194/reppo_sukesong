#!/usr/bin/env python3
"""Exercise the production RGB driver with simulated FreeRTOS and LED hardware."""
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest


class GameLedTests(unittest.TestCase):
    def test_game_priority_release_and_existing_effects(self):
        compiler = shutil.which("cc")
        self.assertIsNotNone(compiler, "A host C compiler (cc) is required")
        root = Path(__file__).resolve().parents[2]
        fixture = Path(__file__).resolve().parent / "game_led"
        driver = root / "components" / "BSP" / "RGB"
        with tempfile.TemporaryDirectory(prefix="reppo-game-led-test-") as temp:
            binary = Path(temp) / "game_led_test"
            build = subprocess.run(
                [compiler, "-std=c11", "-O3", "-Wall", "-Wextra", "-Werror",
                 "-I", str(fixture / "stubs"), "-I", str(driver),
                 str(driver / "ws2812b.c"), str(fixture / "driver_test.c"),
                 "-o", str(binary)],
                text=True, capture_output=True,
            )
            self.assertEqual(build.returncode, 0, build.stdout + build.stderr)
            for mode in ("legacy", "game"):
                with self.subTest(mode=mode):
                    result = subprocess.run(
                        [str(binary), mode], text=True, capture_output=True,
                    )
                    self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                    print(result.stdout, end="")


if __name__ == "__main__":
    unittest.main()
