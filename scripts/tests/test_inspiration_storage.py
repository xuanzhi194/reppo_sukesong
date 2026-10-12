from pathlib import Path
import os
import shutil
import subprocess
import tempfile
import unittest


class InspirationStorageTests(unittest.TestCase):
    def run_storage(self, spiffs=False):
        root = Path(__file__).resolve().parents[2]
        with tempfile.TemporaryDirectory(prefix="reppo-idea-store-") as temp:
            binary = Path(temp) / "test"
            command = [shutil.which("cc"), "-std=c11", "-O1", "-g", "-Wall", "-Wextra", "-Werror",
                       "-fsanitize=address,undefined", "-fno-omit-frame-pointer", "-I", str(root / "main"),
                       str(root / "main/inspiration_storage.c"),
                       str(root / "scripts/tests/inspiration/storage_test.c"), "-o", str(binary)]
            if spiffs:
                command[1:1] = ["-DESP_PLATFORM", "-I", str(root / "scripts/tests/inspiration/stubs")]
            build = subprocess.run(command, capture_output=True, text=True)
            self.assertEqual(build.returncode, 0, build.stdout + build.stderr)
            env = dict(os.environ, ASAN_OPTIONS="detect_leaks=0:halt_on_error=1",
                       UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1")
            run = subprocess.run([str(binary), str(Path(temp) / "ideas.dat")],
                                 capture_output=True, text=True, env=env, timeout=60)
            self.assertEqual(run.returncode, 0, run.stdout + run.stderr)

    def test_persistence_capacity_recovery_and_utf8(self):
        self.run_storage()

    def test_spiffs_close_commit_path(self):
        self.run_storage(spiffs=True)


if __name__ == "__main__":
    unittest.main()
