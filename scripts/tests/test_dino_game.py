#!/usr/bin/env python3
"""Compile and run the production runner physics without ESP-IDF or LVGL."""
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest


class DinoGameTests(unittest.TestCase):
    def test_runner_physics_and_playability(self):
        compiler = shutil.which("cc")
        self.assertIsNotNone(compiler, "A host C compiler (cc) is required")
        root = Path(__file__).resolve().parents[2]
        fixture = Path(__file__).resolve().parent / "dino_game" / "core_test.c"
        with tempfile.TemporaryDirectory(prefix="reppo-dino-test-") as temp:
            binary = Path(temp) / "dino_game_test"
            build = subprocess.run(
                [compiler, "-std=c11", "-O3", "-ffast-math", "-Wall", "-Wextra", "-Werror",
                 "-I", str(root / "main"), str(root / "main" / "dino_game.c"),
                 str(fixture), "-o", str(binary)],
                text=True, capture_output=True,
            )
            self.assertEqual(build.returncode, 0, build.stdout + build.stderr)
            result = subprocess.run([str(binary)], text=True, capture_output=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            print(result.stdout, end="")


if __name__ == "__main__":
    unittest.main()
