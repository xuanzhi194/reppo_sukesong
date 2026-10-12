from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
import os
import shutil
import subprocess
import tempfile
import unittest


class InspirationUiTests(unittest.TestCase):
    def test_real_lvgl_notebook_lifecycle_and_input(self):
        root = Path(__file__).resolve().parents[2]
        sources = sorted((root / "components/lvgl/src").rglob("*.c"))
        sources += [root / "main/inspiration_storage.c", root / "main/inspiration_service.c",
                    root / "components/ui_custom/custom/ui_todo_cjk_font.c",
                    root / "scripts/tests/inspiration/ui_test.c",
                    root / "main/voice_todo_font.c", root / "main/voice_todo_title_font.c",
                    root / "components/ui_custom/generated/images/_AAA_reppo_setting_bg_320x170.c"]
        sources += [root / f"components/ui_custom/generated/guider_fonts/lv_font_SourceHanSansSC_Medium_{size}.c"
                    for size in (12, 14, 16)]
        with tempfile.TemporaryDirectory(prefix="reppo-idea-ui-") as temp:
            directory = Path(temp)
            voice = (root / "main/voice_todo.c").read_text()
            widgets = voice[voice.index("#define ASR_URL"):voice.index("static void voice_write_date")]
            widgets = widgets.replace("void voice_todo_start_mode(", "void fixture_voice_todo_start_mode(")
            adapter = directory / "voice_widgets.c"
            adapter.write_text(
                '#include "lvgl.h"\n#include "gui_guider.h"\n#include "custom.h"\n'
                '#include "ui_locale.h"\n#include "inspiration_ui.h"\n#include "inspiration_storage.h"\n'
                '#include <stdio.h>\n#include <string.h>\n#define EXT_RAM_BSS_ATTR\n'
                'extern const lv_font_t voice_todo_font, voice_todo_title_font;\n'
                'static bool fake_wifi = true;\nstatic bool is_wifi_connected(void) { return fake_wifi; }\n'
                + widgets + '\nstatic void voice_btn_event(lv_event_t *e) { (void)e; }\n'
                'static void voice_start_listen(void) { voice_apply(VS_LISTEN); }\n'
                'void test_voice_show(bool idea, bool wifi) { s_inspiration = idea; fake_wifi = wifi; voice_show(); }\n'
                'void test_voice_state(int state) { voice_apply((voice_state_t)state); }\n'
                'void test_voice_feedback(unsigned ms, unsigned level) { s_audio_ms = ms; s_audio_level = level; '
                's_feedback_at = lv_tick_get() - 50; s_feedback_seconds = UINT32_MAX; '
                'lv_bar_set_range(s_record_bar, 0, s_inspiration ? VOICE_MAX_MS : VOICE_TODO_MS); voice_recording_feedback(); }\n')
            sources.append(adapter)
            flags = ["-std=c11", "-O1", "-g", "-fsanitize=address,undefined", "-fno-omit-frame-pointer",
                     "-DLV_KCONFIG_IGNORE=1", f"-DLV_CONF_PATH={root / 'scripts/tests/inspiration/lv_conf.h'}",
                     "-I", str(root / "components"), "-I", str(root / "components/lvgl"), "-I", str(root / "components/lvgl/src"), "-I", str(root / "main"),
                     "-I", str(root / "components/ui_custom/custom"),
                     "-I", str(root / "components/ui_custom/generated")]

            def compile_one(item):
                index, source = item
                obj = directory / f"{index}.o"
                result = subprocess.run([shutil.which("cc"), *flags, "-c", str(source), "-o", str(obj)],
                                        capture_output=True, text=True)
                if result.returncode:
                    raise AssertionError(f"{source}\n{result.stdout}{result.stderr}")
                return obj

            with ThreadPoolExecutor(max_workers=6) as executor:
                objects = list(executor.map(compile_one, enumerate(sources)))
            binary = directory / "ui_test"
            link = subprocess.run([shutil.which("cc"), "-fsanitize=address,undefined", *map(str, objects),
                                   "-lm", "-o", str(binary)], capture_output=True, text=True)
            self.assertEqual(link.returncode, 0, link.stdout + link.stderr)
            command = [str(binary), str(directory / "ideas.dat")]
            snapshots = os.environ.get("REPPO_INSPIRATION_SNAPSHOTS")
            if snapshots:
                Path(snapshots).mkdir(parents=True, exist_ok=True)
                command.append(str(Path(snapshots).resolve()))
            env = dict(os.environ, ASAN_OPTIONS="detect_leaks=0:halt_on_error=1",
                       UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1")
            run = subprocess.run(command, capture_output=True, text=True, env=env, timeout=60)
            self.assertEqual(run.returncode, 0, (run.stdout + run.stderr)[-5000:])
            self.assertNotIn("glyph dsc. not found", run.stdout + run.stderr)
            self.assertNotIn("Couldn't create a new layer", run.stdout + run.stderr)


if __name__ == "__main__":
    unittest.main()
