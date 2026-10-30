"""Exercise Keep tilt previews against real LVGL with ASan/UBSan."""
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
import os
import re
import shutil
import subprocess
import tempfile
import unittest


class KeepMachineTests(unittest.TestCase):
    def test_tilt_preview_and_lifecycle(self):
        compiler = shutil.which("cc")
        self.assertIsNotNone(compiler, "A host C compiler is required")
        root = Path(__file__).resolve().parents[2]
        fixture = Path(__file__).resolve().parent / "keep_machine"
        sources = sorted((root / "components/lvgl/src").rglob("*.c"))
        generated = root / "components/ui_custom/generated"
        screen = generated / "setup_scr_screen_keep.c"
        images = set(re.findall(r"&(_AAA_\w+)", screen.read_text()))
        sources += [generated / "images" / (name + ".c") for name in sorted(images)]
        fonts = set(re.findall(r"&(lv_font_\w+)", screen.read_text()))
        sources += [generated / "guider_fonts" / (name + ".c") for name in sorted(fonts)]
        sources += [screen, root / "components/ui_custom/custom/ui_todo_cjk_font.c",
                    root / "main/keep_tilt.c",
                    fixture / "host_test.c"]
        flags = ["-std=c11", "-O1", "-g", "-fsanitize=address,undefined",
                 "-fno-omit-frame-pointer", "-DLV_KCONFIG_IGNORE=1",
                 f"-DLV_CONF_PATH={fixture.parent / 'game_machine/lv_conf.h'}",
                 "-I", str(fixture / "stubs"), "-I", str(root / "components/lvgl"),
                 "-I", str(root / "components"),
                 "-I", str(root / "components/lvgl/src"),
                 "-I", str(root / "components/ui_custom/generated"),
                 "-I", str(root / "components/ui_custom/custom"),
                 "-I", str(root / "main")]
        with tempfile.TemporaryDirectory(prefix="reppo-keep-lvgl-") as temp:
            directory = Path(temp)

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
            binary = directory / "keep_machine_test"
            link = subprocess.run([compiler, "-fsanitize=address,undefined", *map(str, objects),
                                   "-lm", "-o", str(binary)], text=True, capture_output=True)
            self.assertEqual(link.returncode, 0, link.stdout + link.stderr)
            env = dict(os.environ, ASAN_OPTIONS="detect_leaks=0:halt_on_error=1",
                       UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1")
            result = subprocess.run([str(binary)], text=True, capture_output=True,
                                    env=env, timeout=60)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            print(result.stdout, end="")


if __name__ == "__main__":
    unittest.main()
