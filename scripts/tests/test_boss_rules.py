"""Exercise demo vocabulary and UTF-8 bounds in the firmware's actual C code."""
import ctypes
from pathlib import Path
import subprocess
import tempfile
import unittest

class BossRulesTests(unittest.TestCase):
    def test_demo_phrases_and_utf8(self):
        root = Path(__file__).resolve().parents[2]
        with tempfile.TemporaryDirectory() as temp:
            libpath = Path(temp) / 'rules.so'
            subprocess.run(['cc', '-std=c11', '-Wall', '-Wextra', '-Werror', '-shared', '-fPIC',
                            str(root / 'main/boss_rules.c'), '-o', str(libpath)], check=True)
            lib = ctypes.CDLL(str(libpath))
            lib.boss_rules_translate.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_size_t,
                                                 ctypes.c_char_p, ctypes.c_size_t]
            lib.boss_rules_translate.restype = ctypes.c_bool
            pairs = [
                ('小同志还需要沉淀啊！','涨薪名单里，你先沉到底。'),
                ('来我们对齐一下颗粒度！','我没听懂，但我得说得像你懂得不够。'),
                ('你要有大局观！','这张饼很大，就是没切你那块。'),
                ('我们要找到抓手！','找个作业抄一抄！'),
                ('这个东西的底层逻辑是什么？','我太笨了听不懂！'),
                ('我们要总结一个方法论！','你们怎么搞出来的？'),
                ('我们复盘一下！','请各位打开背锅二维码。'),
                ('我们要打出一记组合拳！','我来给你们提提工作量！'),
                ('公司需要降本增效！','人减一半，剩下的人学会分身。'),
                ('这个方案还需要再打磨一下！','很好，再陪我刷一点存在感。'),
                ('麻烦你辛苦一下！','我要开始享受了！'),
                ('我们再同步一下！','刚刚我走神了，你再讲一遍！'),
                ('这个应该很容易做出来的！','你要是做不出来你就完蛋了！'),
                ('我简单讲两句！','接下来，请欣赏我的个人播客。'),
                ('友商是SB！','八十万对六十万，优势在我！'),
                ('我再讲最后一个问题！','你知道你很急，但你先别急！'),
                ('今天下班前把这个做了！','截止时间不变，下班时间可变。'),
                ('这件事你看着办！','正确答案我先不说，等你交卷再公布。'),
                ('这周末有空吗？','你可以开始提交加班申请了！'),
                ('还是第一版好！','恭喜你，完成了一次付费为零的时间旅行。'),
            ]
            pairs += [('年轻人，工资', '工资是公司的，你年轻是你自己的。'), ('公司，你的家', '所以你下班准备去哪？'), ('需求，一点点', '顺便把地基往左挪两米。'), ('不提倡，加班', '我们提倡你自愿不下班。'), ('向上管理', '我的工作，你也顺手做一下。'), ('全权负责', '权力先欠着，责任先给你。'), ('拥抱变化', '昨天的需求，今天不认了。'), ('打工心态', '请用老板的心态，拿员工的工资。'), ('只看结果', '做出来算我带得好。'), ('主动，一点', '别等我画饼，自己和面。'), ('狼性', '吃草的预算，想招吃肉的。'), ('版本，看看', '我也不知道要什么，你先帮我排除一个。'), ('会，有价值', '成功把一个问题变成了八个人的待办。'), ('不是钱，问题', '因为根本没打算给钱。'), ('扁平化', '你可以直接接受所有人的指挥。'), ('舒适区', '公司暂不提供舒适区。'), ('长期主义', '涨薪这件事，请你长期等一下。'), ('太实在', '饼都画好了，你怎么还问配料表？'), ('锻炼，机会', '健身房收钱，我们不给钱。'), ('克服，困难', '困难归你，汇报归我。')]
            for source, expected in pairs:
                reply, effect = ctypes.create_string_buffer(256), ctypes.create_string_buffer(80)
                self.assertTrue(lib.boss_rules_translate(source.encode(), reply, 256, effect, 80))
                self.assertEqual(reply.value.decode(), expected)
                self.assertTrue(effect.value.decode())
            reply, effect = ctypes.create_string_buffer(256), ctypes.create_string_buffer(80)
            self.assertFalse(lib.boss_rules_translate('你好，午饭吃什么'.encode(), reply, 256, effect, 80))
            self.assertEqual(reply.value, b'')
            self.assertTrue(lib.boss_rules_translate('先复盘再沉淀'.encode(), reply, 256, effect, 80))
            self.assertEqual(reply.value.decode(), '请各位打开背锅二维码。')
            for cap in range(1, 40):
                small = ctypes.create_string_buffer(cap)
                lib.boss_rules_translate('周末有空吗'.encode(), small, cap, effect, 80)
                small.value.decode('utf-8')
                self.assertLess(len(small.value), cap)
            lib.boss_text_copy.argtypes = [ctypes.c_char_p, ctypes.c_size_t, ctypes.c_char_p]
            for cap in range(1, 30):
                small = ctypes.create_string_buffer(cap)
                lib.boss_text_copy(small, cap, 'ASCII中文😀尾巴'.encode())
                small.value.decode('utf-8')
            lib.boss_text_copy(None, 0, None)
            lib.boss_parse_ai.argtypes = lib.boss_rules_translate.argtypes
            lib.boss_parse_ai.restype = ctypes.c_bool
            self.assertTrue(lib.boss_parse_ai('  所以你下班准备去哪？\r\n回城键已禁用\n多余第三行'.encode(),reply,256,effect,80))
            self.assertEqual(reply.value.decode(),'所以你下班准备去哪？')
            self.assertEqual(effect.value.decode(),'回城键已禁用')
            self.assertTrue(lib.boss_parse_ai('只有一句也能显示'.encode(),reply,256,effect,80))
            self.assertEqual(effect.value.decode(),'潜台词解码完成')
            self.assertFalse(lib.boss_parse_ai(b' \r\n ',reply,256,effect,80))


if __name__ == '__main__':
    unittest.main()
