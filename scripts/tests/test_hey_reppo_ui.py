"""Exercise Hey Reppo navigation and settings against real LVGL."""
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
import os
import re
import shutil
import subprocess
import tempfile
import unittest


class HeyUITests(unittest.TestCase):
    def test_screen_lifecycle(self):
        compiler = shutil.which("cc")
        self.assertIsNotNone(compiler, "A host C compiler is required")
        root = Path(__file__).resolve().parents[2]
        fixture = Path(__file__).resolve().parent / "hey_reppo"
        sources = sorted((root / "components/lvgl/src").rglob("*.c"))
        generated = root / "components/ui_custom/generated"
        sources += [generated / "guider_fonts/lv_font_SourceHanSansSC_Medium_16.c",
                    generated / "guider_fonts/lv_font_SourceHanSansSC_Medium_12.c",
                    generated / "guider_fonts/lv_font_SourceHanSansSC_Medium_14.c",
                    root / "components/ui_custom/custom/ui_todo_cjk_font.c",
                    generated / "images/_AAA_reppo_setting_bg_320x170.c",
                    root / "main/hey_reppo_intent.c",
                    root / "main/boss_rules.c", fixture / "host_test.c", root / "scripts/tests/support/lv_heap.c"]
        flags = ["-include", str(root / "scripts/tests/support/lv_heap.h"),
                 "-DLV_MEM_CUSTOM_ALLOC=test_lv_malloc", "-DLV_MEM_CUSTOM_FREE=test_lv_free",
                 "-DLV_MEM_CUSTOM_REALLOC=test_lv_realloc", "-std=c11", "-O1", "-g", "-fsanitize=address,undefined",
                 "-fno-omit-frame-pointer", "-DLV_KCONFIG_IGNORE=1",
                 f"-DLV_CONF_PATH={fixture.parent / 'game_machine/lv_conf.h'}",
                 "-I", str(fixture / "stubs"), "-I", str(root / "components/lvgl"),
                 "-I", str(root / "components"),
                 "-I", str(root / "components/lvgl/src"),
                 "-I", str(root / "components/ui_custom/generated"),
                 "-I", str(root / "components/ui_custom/custom"),
                 "-I", str(root / "main")]
        with tempfile.TemporaryDirectory(prefix="reppo-boss-lvgl-") as temp:
            directory = Path(temp)
            # Use production Guider transition semantics (including clean and
            # recreate flags) rather than a more permissive mock loader.
            loader = (generated / "gui_guider.c").read_text()
            loader = loader[loader.index("void ui_load_scr_animation("):loader.index("void ui_animation(")]
            (directory / "guider_loader_under_test.h").write_text(loader)
            flags.extend(["-I", str(directory)])

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
            binary = directory / "boss_ui_test"
            link = subprocess.run([compiler, "-fsanitize=address,undefined", *map(str, objects),
                                   "-lm", "-o", str(binary)], text=True, capture_output=True)
            self.assertEqual(link.returncode, 0, link.stdout + link.stderr)
            env = dict(os.environ, ASAN_OPTIONS="detect_leaks=0:halt_on_error=1",
                       UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1")
            result = subprocess.run([str(binary)], text=True, capture_output=True,
                                    env=env, timeout=180)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            print(result.stdout, end="")


if __name__ == "__main__":
    unittest.main()
