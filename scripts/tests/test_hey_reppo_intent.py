from pathlib import Path
import os
import subprocess
import tempfile
import unittest

class HeyIntentTests(unittest.TestCase):
    def test_wake_and_commands(self):
        root=Path(__file__).resolve().parents[2]
        with tempfile.TemporaryDirectory() as temp:
            binary=Path(temp)/'test'
            subprocess.run(['cc','-std=c11','-g','-Wall','-Wextra','-Werror','-fsanitize=address,undefined',
                '-I',str(root/'main'),str(root/'main/hey_reppo_intent.c'),str(root/'main/boss_rules.c'),
                str(root/'scripts/tests/hey_reppo/intent_test.c'),'-o',str(binary)],check=True)
            subprocess.run([str(binary)],check=True,env=dict(os.environ,ASAN_OPTIONS='detect_leaks=0:halt_on_error=1'))
