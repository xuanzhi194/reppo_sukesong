#include "boss_rules.h"
#include <string.h>
#include <ctype.h>

void boss_text_copy(char *out, size_t cap, const char *text)
{
    if (!cap) return;
    if (!text) text = "";
    size_t n = strlen(text);
    if (n >= cap) {
        n = cap - 1;
        while (n && ((unsigned char)text[n] & 0xc0) == 0x80) --n;
    }
    memcpy(out, text, n); out[n] = 0;
}

typedef struct { const char *a, *b, *reply, *effect; } rule_t;
static const rule_t rules[] = {
    {"年轻人", "工资", "工资是公司的，你年轻是你自己的。", "青春余额 -30%"},
    {"公司", "你的家", "所以你下班准备去哪？", "回城键已禁用"},
    {"需求", "一点点", "顺便把地基往左挪两米。", "项目进入装修模式"},
    {"不提倡", "加班", "我们提倡你自愿不下班。", "下班按钮已隐藏"},
    {"向上管理", NULL, "我的工作，你也顺手做一下。", "获得第二位老板：自己"},
    {"全权负责", NULL, "权力先欠着，责任先给你。", "背锅侠已转正"},
    {"拥抱变化", NULL, "昨天的需求，今天不认了。", "昨日存档已失效"},
    {"打工心态", NULL, "请用老板的心态，拿员工的工资。", "股东体验卡：无分红版"},
    {"只看结果", NULL, "做出来算我带得好。", "功劳已自动同步到上级"},
    {"主动", "一点", "别等我画饼，自己和面。", "解锁技能：自助烘焙"},
    {"狼性", NULL, "吃草的预算，想招吃肉的。", "物种匹配失败"},
    {"版本", "看看", "我也不知道要什么，你先帮我排除一个。", "需求盲盒已开启"},
    {"会", "有价值", "成功把一个问题变成了八个人的待办。", "待办发生有丝分裂"},
    {"不是钱", "问题", "因为根本没打算给钱。", "钱包：未参与本次讨论"},
    {"扁平化", NULL, "你可以直接接受所有人的指挥。", "同时连接 8 位领导"},
    {"舒适区", NULL, "公司暂不提供舒适区。", "出生点即困难模式"},
    {"长期主义", NULL, "涨薪这件事，请你长期等一下。", "预计到账：下个纪元"},
    {"太实在", NULL, "饼都画好了，你怎么还问配料表？", "反画饼雷达已开启"},
    {"锻炼", "机会", "健身房收钱，我们不给钱。", "负重训练：新增一口锅"},
    {"克服", "困难", "困难归你，汇报归我。", "困难已成功转交"},
    {"沉淀", NULL, "涨薪名单里，你先沉到底。", "浮力 -100%"},
    {"颗粒度", NULL, "我没听懂，但我得说得像你懂得不够。", "人话解码中…"},
    {"大局观", NULL, "这张饼很大，就是没切你那块。", "分饼权限不足"},
    {"抓手", NULL, "找个作业抄一抄！", "参考答案搜索中…"},
    {"底层逻辑", NULL, "我太笨了听不懂！", "理解进度：缓冲中"},
    {"方法论", NULL, "你们怎么搞出来的？", "教程作者：你们"},
    {"复盘", NULL, "请各位打开背锅二维码。", "正在匹配责任人…"},
    {"组合拳", NULL, "我来给你们提提工作量！", "工作量 +20%"},
    {"降本增效", NULL, "人减一半，剩下的人学会分身。", "分身术：强制更新"},
    {"打磨", NULL, "很好，再陪我刷一点存在感。", "文件名新增“最终版”"},
    {"辛苦", "一下", "我要开始享受了！", "辛苦已成功转交"},
    {"同步", "一下", "刚刚我走神了，你再讲一遍！", "会议开启单曲循环"},
    {"容易", "做", "你要是做不出来你就完蛋了！", "难度：由嘴决定"},
    {"简单", "两句", "接下来，请欣赏我的个人播客。", "预计时长：电量耗尽"},
    {"友商", "sb", "八十万对六十万，优势在我！", "自信值已超出量程"},
    {"友商", "傻", "八十万对六十万，优势在我！", "自信值已超出量程"},
    {"最后", "问题", "你知道你很急，但你先别急！", "会议结束概率 -30%"},
    {"下班前", NULL, "截止时间不变，下班时间可变。", "下班时间重新计算中"},
    {"看着办", NULL, "正确答案我先不说，等你交卷再公布。", "开启盲考模式"},
    {"周末", "有空", "你可以开始提交加班申请了！", "加班概率 +80%"},
    {"第一版", "好", "恭喜你，完成了一次付费为零的时间旅行。", "已绕地球一圈回到原点"},
};

bool boss_rules_translate(const char *source, char *out, size_t cap, char *effect, size_t effect_cap)
{
    char text[768];
    boss_text_copy(text, sizeof(text), source);
    for (size_t i = 0; text[i]; ++i)
        if ((unsigned char)text[i] < 128) text[i] = (char)tolower((unsigned char)text[i]);
    boss_text_copy(out, cap, ""); boss_text_copy(effect, effect_cap, "");
    /* First recognizable phrase in the utterance wins, rather than table order. */
    const rule_t *best = NULL;
    const char *first = NULL;
    for (size_t i = 0; i < sizeof(rules) / sizeof(rules[0]); ++i) {
        const char *at = strstr(text, rules[i].a);
        if (at && (!rules[i].b || strstr(text, rules[i].b)) && (!first || at < first)) {
            first = at; best = &rules[i];
        }
    }
    if (!best) return false;
    boss_text_copy(out, cap, best->reply); boss_text_copy(effect, effect_cap, best->effect);
    return true;
}

/* The model returns two short lines; no JSON parser/extra heap is needed. */
bool boss_parse_ai(const char *text, char *reply, size_t cap, char *effect, size_t effect_cap)
{
    boss_text_copy(reply, cap, ""); boss_text_copy(effect, effect_cap, "");
    if (!text || !cap) return false;
    while (*text && isspace((unsigned char)*text)) ++text;
    char first[256]; boss_text_copy(first, sizeof(first), text);
    char *line = strpbrk(first, "\r\n");
    if (line) *line = 0;
    size_t n = strlen(first);
    while (n && isspace((unsigned char)first[n - 1])) first[--n] = 0;
    if (!n) return false;
    boss_text_copy(reply, cap, first);
    const char *next = strpbrk(text, "\r\n");
    if (next) {
        while (*next && isspace((unsigned char)*next)) ++next;
        char second[80]; boss_text_copy(second, sizeof(second), next);
        char *end = strpbrk(second, "\r\n"); if (end) *end = 0;
        n = strlen(second);
        while (n && isspace((unsigned char)second[n - 1])) second[--n] = 0;
        boss_text_copy(effect, effect_cap, second);
    }
    if (effect_cap && !effect[0]) boss_text_copy(effect, effect_cap, "潜台词解码完成");
    return reply[0] != 0;
}
