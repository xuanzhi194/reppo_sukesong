#!/usr/bin/env python3
"""Todo category tokens used by BLE JSON and todo.csv."""
from pathlib import Path
import os
import shutil
import subprocess
import tempfile
import unittest


class TodoLabelTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        compiler = shutil.which("cc")
        if not compiler:
            raise RuntimeError("A host C compiler (cc) is required")
        root = Path(__file__).resolve().parents[2]
        fixture = Path(__file__).resolve().parent / "todo_label"
        directory = tempfile.TemporaryDirectory(prefix="reppo-todo-label-")
        cls.addClassCleanup(directory.cleanup)
        cls.binary = Path(directory.name) / "label_test"
        command = [
            compiler, "-std=c11", "-O1", "-g", "-Wall", "-Wextra", "-Werror",
            "-fsanitize=address,undefined", "-fno-omit-frame-pointer",
            "-I", str(root / "components/BSP/todo_storage"),
            str(root / "components/BSP/todo_storage/todo_label.c"),
            str(fixture / "label_test.c"),
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


for case_name in ("tokens", "english", "chinese", "numbers", "roundtrip"):
    def test_case(self, case=case_name):
        self.run_case(case)
    setattr(TodoLabelTests, "test_" + case_name, test_case)


if __name__ == "__main__":
    unittest.main()
