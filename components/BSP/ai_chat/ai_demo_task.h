#ifndef AI_DEMO_TASK_H
#define AI_DEMO_TASK_H

#include <stdbool.h>

/* 总结类型 */
typedef enum {
    AI_SUMMARY_DAILY  = 0,  /* 每日总结：当天完成任务，最多 25 条 */
    AI_SUMMARY_WEEKLY = 1,  /* 每周总结：近 7 天完成任务，最多 80 条 */
} ai_summary_type_t;

/* AI 分析结果结构体，支持 NVS 掉电存储 */
typedef struct {
    char cat1_name[32];      /* 第一类分类名称 */
    int  cat1_percent;       /* 第一类占比 (0-100) */
    char cat2_name[32];      /* 第二类分类名称 */
    int  cat2_percent;
    char cat3_name[32];      /* 第三类分类名称 */
    int  cat3_percent;
    char summary[256];       /* 一句话总结 */
    char quote[128];         /* 英文励志名言 */
    char summary_date[40];   /* 总结起始日期 YYYY-MM-DD */
    bool valid;              /* true 表示数据有效 */
} ai_result_t;

/* 每日 / 每周全局结果，UI 可直接读取 */
extern ai_result_t g_ai_daily_result;
extern ai_result_t g_ai_weekly_result;

/* AI 结果就绪标志：AI 任务完成解析后置 1，UI 读取后清 0 */
extern volatile bool g_ai_daily_result_ready;
extern volatile bool g_ai_weekly_result_ready;

/* 启动 AI 总结任务（type = AI_SUMMARY_DAILY 或 AI_SUMMARY_WEEKLY） */
void start_ai_summary(ai_summary_type_t type, bool *ntp_initialized, const char *api_key);

/* 从 NVS 加载对应类型的上次结果，有有效数据则返回 true */
bool ai_daily_result_load(void);
bool ai_weekly_result_load(void);

#endif /* AI_DEMO_TASK_H */
