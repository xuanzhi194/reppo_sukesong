#include "hey_reppo_intent.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static void command(const char *text,hey_action_t action,unsigned minutes) {
    hey_intent_t r=hey_parse_command(text);assert(r.action==action);assert(r.minutes==minutes);
}
int main(void) {
    const char *focus[]={"帮我开始25分钟的专注番茄钟","开始二十五分钟专注","开始一百八十分钟专注","开始两分钟倒计时","start a 25 min focus timer"};
    unsigned values[]={25,25,180,2,25};
    for(unsigned i=0;i<5;i++) command(focus[i],HEY_START_FOCUS,values[i]);
    command("开始专注",HEY_START_FOCUS,25);
    command("进入领导黑话翻译功能",HEY_BOSS,0);
    command("打开待办",HEY_TODO,0);command("打开已完成",HEY_DONE,0);
    command("进入专注页面",HEY_FOCUS_PAGE,0);command("打开Keep",HEY_KEEP,0);
    command("去扭蛋",HEY_GASH,0);command("打开阅读",HEY_BOOK,0);
    command("打开游戏",HEY_GAME,0);command("进入设置",HEY_SETTINGS,0);
    command("打开相册",HEY_PHOTO,0);command("打开答案之书",HEY_ANSWER,0);command("查看时间",HEY_CLOCK,0);
    command("返回主页",HEY_HOME,0);command("打开灵感闪记",HEY_IDEAS,0);
    command("关闭语音唤醒",HEY_DISABLE,0);command("算了",HEY_CANCEL,0);
    const char *bad[]={"不要开始25分钟专注","don't start 25 min focus","开始0分钟专注","开始181分钟专注","开始99999分钟专注","开始-5分钟专注","开始2.5分钟专注","开始二零五分钟专注","开始负五分钟专注","开始25到30分钟专注","开始两个小时专注","开始25专注","删除待办","不关闭语音唤醒","打开Boss和开始25分钟专注","停止后开始专注","停止专注","暂停番茄钟","如果我说开始专注","打开游戏然后开始专注","今天的领导说了什么","hey repository"};
    for(unsigned i=0;i<sizeof(bad)/sizeof(bad[0]);i++) command(bad[i],HEY_INVALID,0);
    char tail[768];assert(hey_wake_tail("Hey，Reppo！帮我开始25分钟专注",tail,sizeof(tail)));assert(strstr(tail,"25"));
    assert(hey_wake_tail("嘿，瑞波",tail,sizeof(tail))&&!tail[0]);
    assert(!hey_wake_tail("刚才有人说Hey Reppo",tail,sizeof(tail)));
    assert(!hey_wake_tail("hey repository",tail,sizeof(tail)));
    const char *short_wakes[]={"Hi", "hi!", "HI.", "嗨", "嗨！", "Hi Reppo", "Hi, Repo."};
    for(unsigned i=0;i<sizeof(short_wakes)/sizeof(short_wakes[0]);i++) {
        assert(hey_wake_tail(short_wakes[i],tail,sizeof(tail))&&!tail[0]);
    }
    const char *not_wakes[]={"high", "history", "hint", "hiking", "hi2", "this is hi", "他说嗨", "", "h"};
    for(unsigned i=0;i<sizeof(not_wakes)/sizeof(not_wakes[0]);i++) assert(!hey_wake_tail(not_wakes[i],tail,sizeof(tail)));
    assert(hey_wake_tail("Hi，打开领导黑话翻译",tail,sizeof(tail)));command(tail,HEY_BOSS,0);
    assert(hey_wake_tail("hi open settings",tail,sizeof(tail)));command(tail,HEY_SETTINGS,0);
    assert(hey_wake_tail("嗨，开始2.5分钟专注",tail,sizeof(tail)));command(tail,HEY_INVALID,0);
    hey_dialog_t d={0};
    hey_dialog_receive(&d,"打开领导黑话翻译",true,100);assert(!d.awake&&!d.pending);
    hey_dialog_receive(&d,"Hey Reppo",false,200);assert(d.awake&&!d.pending);
    hey_dialog_receive(&d,"Hey Reppo",true,600);assert(d.awake&&!d.pending);
    hey_dialog_receive(&d,"开始两分钟专注",false,1000);assert(!d.pending);
    hey_dialog_receive(&d,"开始二十五分钟专注",true,1500);assert(d.pending&&d.intent.minutes==25);
    hey_dialog_receive(&d,"打开游戏",true,1600);assert(d.intent.action==HEY_START_FOCUS); // One command per wake.
    hey_dialog_reset(&d);hey_dialog_receive(&d,"Hey Rapple，帮我开始二十五分钟的专注番茄钟。",true,2500);assert(d.pending&&d.intent.minutes==25);
    hey_dialog_reset(&d);hey_dialog_receive(&d,"Hey Reppo，打开领导黑话翻译",true,3000);assert(d.pending&&d.intent.action==HEY_BOSS);
    hey_dialog_reset(&d);hey_dialog_receive(&d,"Hey Reppo",true,4000);hey_dialog_receive(&d,"取消",true,4100);assert(!d.awake);
    hey_dialog_receive(&d,"Hey Reppo",true,5000);hey_dialog_tick(&d,17000);assert(!d.awake);
    hey_dialog_receive(&d,"打开游戏",true,17001);assert(!d.pending);
    hey_dialog_receive(&d,"Hey Reppo",true,UINT32_MAX-5000);hey_dialog_tick(&d,1000);assert(d.awake);hey_dialog_tick(&d,7000);assert(!d.awake);
    hey_dialog_reset(&d);hey_dialog_receive(&d,"嘿，Ripple，进入",false,20000);
    hey_dialog_receive(&d,"进入领导黑化翻译功能。",true,22000);assert(d.pending&&d.intent.action==HEY_BOSS);
    hey_dialog_reset(&d);hey_dialog_receive(&d,"Hey, Repo.",true,30000);
    assert(d.awake&&!d.pending&&strstr(d.hint,"我在"));
    hey_dialog_receive(&d,"进入领导黑话翻译",true,31000);assert(d.pending&&d.intent.action==HEY_BOSS);
    assert(hey_wake_tail("Hey-Reppo. 开始2.5分钟专注",tail,sizeof(tail)));
    command(tail,HEY_INVALID,0); // Preserve the decimal rejection in the command.
    hey_dialog_reset(&d);hey_dialog_receive(&d,"Hi!",false,40000);assert(d.awake&&!d.pending);
    hey_dialog_receive(&d,"Hi.",true,40100);assert(d.awake&&!d.pending);
    hey_dialog_receive(&d,"开始25分钟专注",true,41000);assert(d.pending&&d.intent.minutes==25);
    hey_dialog_reset(&d);hey_dialog_receive(&d,"嗨，打开领导黑话翻译",false,42000);assert(d.awake&&!d.pending);
    hey_dialog_receive(&d,"嗨，打开领导黑话翻译",true,43000);assert(d.pending&&d.intent.action==HEY_BOSS);
    puts("PASS short Hi/嗨, retained Hey Reppo, word boundaries, partial/final and one-shot commands");
    char huge[2048];memset(huge,'x',sizeof(huge)-1);huge[sizeof(huge)-1]=0;
    command(huge,HEY_INVALID,0);assert(!hey_wake_tail(huge,tail,1)&&tail[0]==0);

    /* Deterministic fuzz of malformed/noisy ASR text and bounded output sizes. */
    uint32_t rng=0x72657070u;
    const char *tokens[]={"Hey","Reppo","打开","25","-",".","分钟","，","嗨","\n","x","\x80","\xf0\x9f"," ","专注"};
    for(unsigned round=0;round<50000;round++) {
        char noise[2048]={0};size_t used=0;
        rng=rng*1664525u+1013904223u;unsigned count=rng%80;
        for(unsigned j=0;j<count;j++) {
            rng=rng*1664525u+1013904223u;const char *t=tokens[rng%15];size_t n=strlen(t);
            if(used+n>=sizeof(noise))break;memcpy(noise+used,t,n);used+=n;
        }
        struct {unsigned a;hey_dialog_t d;unsigned b;hey_parse_workspace_t w;unsigned c;} box={.a=0xabcdef01,.b=0x12345678,.c=0x87654321};
        hey_dialog_receive_with_workspace(&box.d,noise,round%2,round,&box.w);
        assert(box.a==0xabcdef01&&box.b==0x12345678&&box.c==0x87654321);
        unsigned char bounded[40];memset(bounded,0xcc,sizeof(bounded));unsigned cap=round%33;
        (void)hey_wake_tail(noise,(char*)bounded+1,cap);
        assert(bounded[0]==0xcc&&bounded[cap+1]==0xcc);
        (void)hey_parse_command(noise);
    }
    puts("PASS 50000 deterministic ASR-text fuzz cases, truncated UTF-8 and output guards");
    puts("PASS wake prefix, Chinese/Arabic minutes, partial correction, single command, cancellation, timeout/wrap, rejection and bounds");
}
