/**
 * @file ui_locale.c
 * @brief 中英文切换，以及给思源字体补上汉字备用字。
 *
 * 备用字只在原字体没有这个字时才用，所以英文和界面里原来的汉字样子不变。
 */
#include "ui_locale.h"

#include "gui_guider.h"
#include "nvs.h"

extern const lv_font_t ui_chrome_font_14;
extern const lv_font_t ui_todo_cjk_font;

#define LOCALE_NVS_NS  "Settings"
#define LOCALE_NVS_KEY "lang"

static bool s_zh = true;
static bool s_ready;
static lv_font_t s_chrome;
static lv_font_t s_f11;
static lv_font_t s_f12;
static lv_font_t s_f13;
static lv_font_t s_f14;
static lv_font_t s_f16;
static lv_font_t s_f17;

static const char *const s_answer_zh[] = {
    "毫无疑问", "是的", "当然", "去做吧", "就是现在",
    "值得一试", "先迈一步", "命运说：可以", "星星说可以", "宇宙挺你",
    "签说：大吉", "迹象说出发", "看起来不错", "会有人帮你", "结果会更好",
    "相信直觉", "就是这一刻", "放手去做", "好运要来了", "门开着",
    "顺势而为", "帮助会出现", "继续走，前面有光", "你可以的", "值得冒险",
    "向前走", "绿灯，走", "是", "一致通过", "很快会成",
    "会顺起来", "路会打开", "吉星排好了", "不必再问", "答案是可以",
    "绝对可以", "没问题", "好结果在等你", "抓住这次", "月亮说可以",
    "迹象都偏向你", "再大胆一点", "跟着想要的走", "开局很好", "最难的过去了",
    "运气在转", "运气站你这边", "好事要来", "善意会回来", "完全没问题",
    "前景明亮", "会有意外礼物", "跳吧", "做，别等", "吉星在亮",
    "等一等，天会晴", "路会出现", "会自己理顺", "你知道下一步", "难得，拿下",
    "你的运气很足", "风在帮你", "春天要来", "过了这关有新路", "难处要结束了",
    "好运在门口",
    "最好别", "还没到", "再想一想", "先别抱太大希望", "答案是不行",
    "现在先不要", "再等一等", "时机不对", "星星在摇头", "再想想",
    "大概不行", "别急", "先停一停", "现在做会后悔", "换条路",
    "这条路关了", "不太吉利，小心", "逆风，先别走", "先停一停", "今天别做大决定",
    "签说：不宜", "今天天没点头", "不行", "一致否决", "想两遍",
    "急了反而糟", "过了反而不好", "等，以后可能翻盘", "找错地方了", "方向不对",
    "那只是幻影", "别冲动", "别硬来", "还是放下", "不会顺你的意",
    "前景不明", "别一个人扛", "别丢了大局", "别往火里扑", "别干等",
    "不会准时发生", "可能会失望", "别期望太高", "不是时候", "还不是时候",
    "胜算不高", "阻力太大", "麻烦在酝酿", "云还没散", "月缺，再等",
    "门关着", "路堵住了", "这个计划不行", "这个愿望还不成", "这一次放下",
    "今天不是那天", "还能回头", "停在边上", "先停，再补", "记上次的失手",
    "别试，收不回", "别走进圈套", "别把自己捆住",
    "随它去", "走着看", "也许，也许不", "这要时间", "你其实知道",
    "今天命运休假", "骰子立住了", "再看一次", "神算请假了", "这题是附加题",
    "书还要再想", "别催，还在算", "既是也不是", "再摇一次", "睡醒再问",
    "命运在开玩笑", "透露得够多了", "你问太多次了", "答案在下一页", "这本书拒答",
    "命运还在打字", "看心情", "看命运", "看天意", "看时机",
    "看猫的意思", "抛硬币吧", "宇宙加载中", "猫说你去问它", "狗说随便",
    "星星说你猜", "风把答案吹走了", "签筒空了，下次", "宜休息，不宜动", "运气堵在路上",
    "书去休息了", "命运的键盘卡住了", "是，也不是", "对，也不对", "随便，放下吧",
    "你来定", "交给命运", "先做你的，再放下", "不能说，不能说", "只告诉你一半",
    "没懂就再问", "好，翻页", "问了未必有答", "答了你也未必信", "或者别问了，去生活",
    "立起来再问", "今天次数用完了", "知道分寸", "你把书问紧张了", "门虚掩着，推一下",
    "月还没圆，等", "猫还在睡", "还在打字", "信号不好，再试", "再问，还是这句",
    "静一静", "飘着吧", "再摇一次", "另一半你自己想", "让心来定",
    "问心无愧就够", "这题被跳过了", "以后再来问", "计划好，剩下交给天", "你知我知，到此",
    "所以：再问一次",
};

static void locale_load(void)
{
    nvs_handle_t h;
    uint8_t stored = 1;

    if (nvs_open(LOCALE_NVS_NS, NVS_READONLY, &h) != ESP_OK) {
        s_zh = true;
        return;
    }
    if (nvs_get_u8(h, LOCALE_NVS_KEY, &stored) == ESP_OK) {
        s_zh = stored != 0;
    }
    nvs_close(h);
}

static void locale_save(void)
{
    nvs_handle_t h;

    if (nvs_open(LOCALE_NVS_NS, NVS_READWRITE, &h) != ESP_OK) {
        return;
    }
    nvs_set_u8(h, LOCALE_NVS_KEY, s_zh ? 1U : 0U);
    nvs_commit(h);
    nvs_close(h);
}

static void font_init(void)
{
    if (s_ready) {
        return;
    }
    s_chrome = lv_font_SourceHanSansSC_Medium_14;
    s_f11 = lv_font_SourceHanSansSC_Medium_11;
    s_f12 = lv_font_SourceHanSansSC_Medium_12;
    s_f13 = lv_font_SourceHanSansSC_Medium_13;
    s_f14 = lv_font_SourceHanSansSC_Medium_14;
    s_f16 = lv_font_SourceHanSansSC_Medium_16;
    s_f17 = lv_font_SourceHanSansSC_Medium_17;
    /* 标题和设置按钮只补那几个字，和原来的 14 号思源是同一套。 */
    s_chrome.fallback = &ui_chrome_font_14;
    /* 其余缺字都用待办这套通用汉字，不再另做一份。 */
    s_f11.fallback = &ui_todo_cjk_font;
    s_f12.fallback = &ui_todo_cjk_font;
    s_f13.fallback = &ui_todo_cjk_font;
    s_f14.fallback = &ui_todo_cjk_font;
    s_f16.fallback = &ui_todo_cjk_font;
    s_f17.fallback = &ui_todo_cjk_font;
    s_ready = true;
}

void ui_locale_init(void)
{
    font_init();
    locale_load();
}

bool ui_lang_is_zh(void)
{
    return s_zh;
}

void ui_lang_toggle(void)
{
    s_zh = !s_zh;
    locale_save();
}

const char *ui_tr(const char *en, const char *zh)
{
    if (s_zh && zh != NULL) {
        return zh;
    }
    return en != NULL ? en : "";
}

const char *ui_weekday(int wday)
{
    static const char *const en[7] = {
        "Sunday", "Monday", "Tuesday", "Wednesday",
        "Thursday", "Friday", "Saturday",
    };
    static const char *const zh[7] = {
        "星期日", "星期一", "星期二", "星期三",
        "星期四", "星期五", "星期六",
    };
    if (wday < 0 || wday > 6) {
        wday = 0;
    }
    return s_zh ? zh[wday] : en[wday];
}

const char *ui_answer(unsigned index, const char *en_fallback)
{
    if (!s_zh || index >= (unsigned)(sizeof(s_answer_zh) / sizeof(s_answer_zh[0]))) {
        return en_fallback;
    }
    return s_answer_zh[index];
}

const lv_font_t *ui_font_12(void)
{
    font_init();
    return &s_f12;
}

const lv_font_t *ui_font_14(void)
{
    font_init();
    return &s_f14;
}

const lv_font_t *ui_font_16(void)
{
    font_init();
    return &s_f16;
}

const lv_font_t *ui_font_chrome(void)
{
    font_init();
    return &s_chrome;
}

void ui_font_set_chrome(lv_obj_t *obj)
{
    const lv_font_t *font;
    lv_obj_t *child;

    if (obj == NULL || !lv_obj_is_valid(obj)) {
        return;
    }
    font = ui_font_chrome();
    lv_obj_set_style_text_font(obj, font, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(obj, font, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(obj, font, LV_PART_MAIN | LV_STATE_PRESSED);
    if (lv_obj_get_child_cnt(obj) == 0) {
        return;
    }
    child = lv_obj_get_child(obj, 0);
    if (child != NULL && lv_obj_check_type(child, &lv_label_class)) {
        lv_obj_set_style_text_font(child, font, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(child, font, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_text_font(child, font, LV_PART_MAIN | LV_STATE_PRESSED);
    }
}

static const lv_font_t *font_wrap(const lv_font_t *font)
{
    font_init();
    if (font == &lv_font_SourceHanSansSC_Medium_11) return &s_f11;
    if (font == &lv_font_SourceHanSansSC_Medium_12) return &s_f12;
    if (font == &lv_font_SourceHanSansSC_Medium_13) return &s_f13;
    if (font == &lv_font_SourceHanSansSC_Medium_14) return &s_f14;
    if (font == &lv_font_SourceHanSansSC_Medium_16) return &s_f16;
    if (font == &lv_font_SourceHanSansSC_Medium_17) return &s_f17;
    return NULL;
}

static void font_bind_obj(lv_obj_t *obj)
{
    const lv_font_t *wrapped;
    uint32_t i;
    uint32_t n;

    if (obj == NULL || !lv_obj_is_valid(obj)) {
        return;
    }
    wrapped = font_wrap(lv_obj_get_style_text_font(obj, LV_PART_MAIN));
    if (wrapped != NULL) {
        lv_obj_set_style_text_font(obj, wrapped, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(obj, wrapped, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_text_font(obj, wrapped, LV_PART_MAIN | LV_STATE_PRESSED);
    }
    n = lv_obj_get_child_cnt(obj);
    for (i = 0; i < n; i++) {
        font_bind_obj(lv_obj_get_child(obj, i));
    }
}

void ui_font_bind(lv_obj_t *root)
{
    font_bind_obj(root);
}
