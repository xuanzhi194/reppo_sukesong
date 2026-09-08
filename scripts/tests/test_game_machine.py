#!/usr/bin/env python3
"""Real LVGL lifecycle/input/render checks, with optional framebuffer snapshots.

REPPO_GAME_SNAPSHOTS=/absolute/output/dir python3 scripts/tests/test_game_machine.py
"""
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
import os
import shutil
import subprocess
import tempfile
import unittest


class GameMachineTests(unittest.TestCase):
    def test_real_lvgl_lifecycle_and_input(self):
        compiler = shutil.which("cc")
        self.assertIsNotNone(compiler, "A host C compiler (cc) is required")
        root = Path(__file__).resolve().parents[2]
        fixture = Path(__file__).resolve().parent / "game_machine"
        sources = sorted((root / "components/lvgl/src").rglob("*.c"))
        sources += [root / "main/dino_game.c", root / "main/dino_render.c",
                    root / "main/game_score_storage.c", fixture / "host_test.c"]
        with tempfile.TemporaryDirectory(prefix="reppo-game-lvgl-") as temp:
            directory = Path(temp)
            flags = ["-std=c11", "-O1", "-g", "-fsanitize=address,undefined", "-fno-omit-frame-pointer",
                     "-DLV_KCONFIG_IGNORE=1", f"-DLV_CONF_PATH={fixture / 'lv_conf.h'}",
                     "-I", str(fixture / "stubs"), "-I", str(root / "components/lvgl"),
                     "-I", str(fixture.parent / "game_score_storage/stubs"),
                     "-I", str(root / "main")]

            def compile_one(item):
                index, source = item
                obj = directory / f"{index}.o"
                result = subprocess.run([compiler, *flags, "-c", str(source), "-o", str(obj)],
                                        text=True, capture_output=True)
                if result.returncode:
                    raise AssertionError(f"{source}\n{result.stdout}{result.stderr}")
                return obj

            with ThreadPoolExecutor(max_workers=6) as executor:
                objects = list(executor.map(compile_one, enumerate(sources)))
            binary = directory / "game_machine_test"
            link = subprocess.run([compiler, "-fsanitize=address,undefined", *map(str, objects),
                                   "-lm", "-o", str(binary)], text=True, capture_output=True)
            self.assertEqual(link.returncode, 0, link.stdout + link.stderr)
            command = [str(binary)]
            snapshots = os.environ.get("REPPO_GAME_SNAPSHOTS")
            if snapshots:
                Path(snapshots).mkdir(parents=True, exist_ok=True)
                command += ["--snapshots", str(Path(snapshots).resolve())]
            env = dict(os.environ, ASAN_OPTIONS="detect_leaks=0:halt_on_error=1",
                       UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1")
            result = subprocess.run(command, text=True, capture_output=True, env=env, timeout=90)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            print(result.stdout, end="")


if __name__ == "__main__":
    unittest.main()
