#include "hey_reppo_intent.h"
#include "boss_rules.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

/* Strip only punctuation/whitespace; preserve UTF-8 and bound every read. */
static void normalize(const char *in, char *out, size_t cap)
{
    size_t n = 0;
    const char *punct[] = {"，", "。", "！", "？", "、", "：", "“", "”", "（", "）", NULL};
    while (*in && n + 4 < cap) {
        bool skip = false;
        for (unsigned i=0; punct[i]; ++i) {
            size_t len = strlen(punct[i]);
            if (!strncmp(in, punct[i], len)) { in += len; skip=true; break; }
        }
        if (skip) continue;
        unsigned char c = (unsigned char)*in;
        if (c < 128) {
            ++in;
            if (isalnum(c)||c=='.'||c=='-') out[n++] = (char)tolower(c);
        } else {
            size_t len = c < 0xe0 ? 2 : c < 0xf0 ? 3 : 4;
            if (strlen(in) < len) break;
            memcpy(out+n,in,len); n+=len; in+=len;
        }
    }
    out[n]=0;
}
static bool wake_tail(const char *text, char *tail, size_t cap, char compact[768])
{
    normalize(text,compact,768);
    /* Only an utterance prefix wakes us. Names elsewhere in a sentence do not. */
    const char *calls[]={"hi", "hey", "嘿", "嗨", "嗨喽", NULL};
    const char *names[]={"reppo", "repo", "rapple", "repple", "ripple", "rebo", "瑞波", "瑞博", "瑞普", "瑞宝", "瑞珀", "瑞坡", "雷波", NULL};
    for (unsigned i=0; calls[i]; ++i) {
        size_t a=strlen(calls[i]); if (strncmp(compact,calls[i],a)) continue;
        while(compact[a]=='.'||compact[a]=='-') ++a;
        for (unsigned j=0; names[j]; ++j) {
            size_t b=strlen(names[j]); if (strncmp(compact+a,names[j],b)) continue;
            /* Reject prefix matches such as "hey repository". */
            unsigned char next=(unsigned char)compact[a+b];
            if (next && next<128 && isalpha(next) &&
                strncmp(compact+a+b,"open",4) && strncmp(compact+a+b,"start",5) &&
                strncmp(compact+a+b,"go",2) && strncmp(compact+a+b,"cancel",6)) continue;
            const char *body=compact+a+b;
            /* ASR may punctuate the wake phrase as "Hey, Repo.". Do not
             * treat that full stop as a command; retain decimals in commands. */
            while(*body=='.') ++body;
            boss_text_copy(tail,cap,body); return true;
        }
    }
    /* Short demo wake: Hi / 嗨. Check the original English word boundary
     * before normalization joins spaces ("hi open" vs "high"/"history"). */
    const char *raw=text;
    while(*raw && (isspace((unsigned char)*raw)||(*raw<127&&ispunct((unsigned char)*raw)))) ++raw;
    size_t short_len=0;
    if(tolower((unsigned char)raw[0])=='h' && tolower((unsigned char)raw[1])=='i' &&
       !isalnum((unsigned char)raw[2]) && !strncmp(compact,"hi",2)) short_len=2;
    else if(!strncmp(compact,"嗨",3)) short_len=3;
    if(short_len) {
        const char *body=compact+short_len;
        while(*body=='.') ++body;
        boss_text_copy(tail,cap,body);return true;
    }
    if (cap) tail[0]=0;
    return false;
}
bool hey_wake_tail(const char *text, char *tail, size_t cap)
{
    char compact[768]; return wake_tail(text,tail,cap,compact);
}
static int cn_digit(const char *s)
{
    const char *d[]={"零","一","二","三","四","五","六","七","八","九"};
    for(int i=0;i<10;++i) if(!strncmp(s,d[i],3)) return i;
    return !strncmp(s,"两",3)?2:-1;
}
static int number(const char *begin, const char *end)
{
    if(begin==end) return -1;
    unsigned total=0, digit=0; bool have=false, arabic=false, chinese=false;
    while(begin<end) {
        if((unsigned char)*begin<128) {
            if(chinese||!isdigit((unsigned char)*begin)) return -1;
            arabic=true;
            total=total*10+(unsigned)(*begin++-'0'); if(total>180) return -1;
        } else {
            if(arabic) return -1;
            chinese=true;
            int d=cn_digit(begin);
            if(d>=0) {if(have&&digit!=0) return -1;digit=(unsigned)d;have=true;}
            else if(!strncmp(begin,"十",3)||!strncmp(begin,"百",3)) {
                total+=(have?digit:1)*(!strncmp(begin,"十",3)?10:100);digit=0;have=false;
            } else return -1;
            begin+=3;
        }
    }
    return (int)(total+digit);
}
static int minutes(const char *s)
{
    const char *unit=strstr(s,"分钟");
    if(!unit) unit=strstr(s,"min");
    if(strstr(s,"小时")||strstr(s,"秒")||strstr(s,"hour")||strstr(s,"半")||strstr(s,"点")||strstr(s,"负")||strstr(s,"到")||strstr(s,"至")||strchr(s,'.')||strchr(s,'-')) return -1;
    if(!unit) {
        for(const char *p=s;*p;++p) if(isdigit((unsigned char)*p)) return -1;
        return 25;
    }
    const char *p=unit;
    while(p>s) {
        unsigned char c=(unsigned char)p[-1];
        if(c<128) { if(!isdigit(c)) break; --p; }
        else {
            if(p-s<3) break;
            const char *q=p-3;
            if(cn_digit(q)<0&&strncmp(q,"十",3)&&strncmp(q,"百",3)) break;
            p=q;
        }
    }
    int n=number(p,unit);return n>=1&&n<=180?n:-1;
}
static hey_intent_t parse_command(const char *text, char s[768])
{
    normalize(text,s,768);
    hey_intent_t r={HEY_NONE,0};
    if(!s[0]) return r;
    if(strstr(s,"取消")||strstr(s,"算了")||strstr(s,"不用了")||strstr(s,"cancel")) {r.action=HEY_CANCEL;return r;}
    if(strstr(s,"不")||strstr(s,"别")||strstr(s,"not")||strstr(s,"dont")||strstr(s,"如果")||strstr(s,"还是")||strstr(s,"然后")||strstr(s,"并且")||strstr(s,"同时")||strstr(s,"和")||strstr(s,"and")) {r.action=HEY_INVALID;return r;}
    if((strstr(s,"关闭")||strstr(s,"关掉"))&&(strstr(s,"唤醒")||strstr(s,"助手")||strstr(s,"heyreppo"))) {r.action=HEY_DISABLE;return r;}
    if(strstr(s,"停止")||strstr(s,"暂停")||strstr(s,"结束")||strstr(s,"删除")||strstr(s,"stop")||strstr(s,"pause")||strstr(s,"delete")) {r.action=HEY_INVALID;return r;}
    bool start=strstr(s,"开始")||strstr(s,"启动")||strstr(s,"开启")||strstr(s,"设定")||strstr(s,"设置")||strstr(s,"start");
    bool focus=strstr(s,"专注")||strstr(s,"番茄")||strstr(s,"倒计时")||strstr(s,"focus")||strstr(s,"pomodoro");
    if(start&&focus) {int n=minutes(s); r.action=n<0?HEY_INVALID:HEY_START_FOCUS;r.minutes=n<0?0:(unsigned)n;return r;}
    /* Do not turn a destructive or unsupported operation into plain navigation. */
    if(strstr(s,"删除")||strstr(s,"清空")||strstr(s,"停止")||strstr(s,"暂停")||strstr(s,"结束")||strstr(s,"关闭")||strstr(s,"完成任务")||strstr(s,"delete")) {r.action=HEY_INVALID;return r;}
    bool nav=strstr(s,"进入")||strstr(s,"打开")||strstr(s,"去")||strstr(s,"返回")||strstr(s,"回到")||strstr(s,"看看")||strstr(s,"查看")||strstr(s,"open")||strstr(s,"goto")||strstr(s,"gohome")||start;
    if(!nav) {r.action=HEY_INVALID;return r;}
    if(strstr(s,"领导")||strstr(s,"黑话")||strstr(s,"黑化")||strstr(s,"bosstrans")||strstr(s,"boss")) r.action=HEY_BOSS;
    else if(strstr(s,"灵感")||strstr(s,"闪记")) r.action=HEY_IDEAS;
    else if(strstr(s,"待办")||strstr(s,"todo")) r.action=HEY_TODO;
    else if(strstr(s,"已完成")||strstr(s,"done")) r.action=HEY_DONE;
    else if(focus) r.action=HEY_FOCUS_PAGE;
    else if(strstr(s,"keep")||strstr(s,"养成")||strstr(s,"橄榄")||strstr(s,"个人总结")) r.action=HEY_KEEP;
    else if(strstr(s,"扭蛋")||strstr(s,"随机任务")) r.action=HEY_GASH;
    else if(strstr(s,"阅读")||strstr(s,"看书")||strstr(s,"书架")||strstr(s,"book")) r.action=HEY_BOOK;
    else if(strstr(s,"游戏")||strstr(s,"game")) r.action=HEY_GAME;
    else if(strstr(s,"照片")||strstr(s,"相册")||strstr(s,"图片")) r.action=HEY_PHOTO;
    else if(strstr(s,"答案之书")) r.action=HEY_ANSWER;
    else if(strstr(s,"时间")||strstr(s,"时钟")||strstr(s,"日期")) r.action=HEY_CLOCK;
    else if(strstr(s,"设置")||strstr(s,"setting")) r.action=HEY_SETTINGS;
    else if(strstr(s,"主页")||strstr(s,"首页")||strstr(s,"主界面")||strstr(s,"home")) r.action=HEY_HOME;
    else r.action=HEY_INVALID;
    return r;
}
hey_intent_t hey_parse_command(const char *text)
{
    char s[768]; return parse_command(text,s);
}
void hey_dialog_reset(hey_dialog_t *d)
{
    uint32_t revision=d->revision+1;memset(d,0,sizeof(*d));d->revision=revision;
}
void hey_dialog_tick(hey_dialog_t *d,uint32_t now)
{
    if(d->awake&&!d->pending&&(int32_t)(now-d->deadline)>=0) hey_dialog_reset(d);
}
void hey_dialog_receive_with_workspace(hey_dialog_t *d,const char *text,bool final,uint32_t now,hey_parse_workspace_t *work)
{
    hey_dialog_tick(d,now);if(d->pending) return;
    char *tail=work->tail;bool wake=wake_tail(text,tail,sizeof(work->tail),work->normalized);
    if(!d->awake&&!wake) return;
    if(!d->awake) {d->awake=true;d->deadline=now+12000;}
    boss_text_copy(d->heard,sizeof(d->heard),text);
    boss_text_copy(d->hint,sizeof(d->hint),"我在，请说要去哪里或开始几分钟专注");
    ++d->revision;
    if(!final) return; /* Partial recognition may revise the page or number. */
    const char *command=wake?tail:text;
    if(!command[0]) {d->deadline=now+12000;return;}
    hey_intent_t intent=parse_command(command,work->normalized);
    if(intent.action==HEY_CANCEL) {hey_dialog_reset(d);return;}
    if(intent.action==HEY_NONE||intent.action==HEY_INVALID) {
        boss_text_copy(d->hint,sizeof(d->hint),"试试：进入领导黑话翻译／开始25分钟专注（1–180分钟）");
        d->deadline=now+12000;return;
    }
    d->intent=intent;d->pending=true;
}

void hey_dialog_receive(hey_dialog_t *d,const char *text,bool final,uint32_t now)
{
    hey_parse_workspace_t work;
    hey_dialog_receive_with_workspace(d,text,final,now,&work);
}
