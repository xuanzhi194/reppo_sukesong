#!/usr/bin/env python3
"""Production high-score NVS adapter: persistence, write coalescing and failures."""
from pathlib import Path
import os
import shutil
import subprocess
import tempfile
import unittest


class GameScoreStorageTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        compiler = shutil.which("cc")
        if not compiler:
            raise RuntimeError("A host C compiler (cc) is required")
        root = Path(__file__).resolve().parents[2]
        fixture = Path(__file__).resolve().parent / "game_score_storage"
        directory = tempfile.TemporaryDirectory(prefix="reppo-game-storage-")
        cls.addClassCleanup(directory.cleanup)
        cls.binary = Path(directory.name) / "storage_test"
        command = [
            compiler, "-std=c11", "-O1", "-g", "-Wall", "-Wextra", "-Werror",
            "-fsanitize=address,undefined", "-fno-omit-frame-pointer", "-DESP_PLATFORM",
            "-I", str(fixture / "stubs"), "-I", str(root / "main"),
            str(root / "main/game_score_storage.c"), str(fixture / "storage_test.c"),
            "-o", str(cls.binary),
        ]
        build = subprocess.run(command, text=True, capture_output=True, timeout=60)
        if build.returncode:
            raise AssertionError(build.stdout + build.stderr)

    def run_case(self, case):
        env = dict(os.environ, ASAN_OPTIONS="detect_leaks=0:halt_on_error=1",
                   UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1")
        result = subprocess.run([str(self.binary), case], text=True, capture_output=True,
                                env=env, timeout=10)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)


CASES = (
    "reboot_restore", "lower_and_equal_do_not_write", "offer_only_updates_ram",
    "missing_namespace_is_zero", "missing_key_is_zero", "open_failure_retries",
    "set_failure_retries", "commit_failure_retries", "initial_open_failure_preserves_record",
    "initial_read_failure_preserves_record", "type_mismatch_preserves_record",
    "repeated_load_failure_never_writes", "tick_wrap_keeps_backoff", "full_u32_score_roundtrips",
)


for case_name in CASES:
    def test_case(self, case=case_name):
        self.run_case(case)
    setattr(GameScoreStorageTests, "test_" + case_name, test_case)


if __name__ == "__main__":
    unittest.main()
