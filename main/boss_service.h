#pragma once
#include <stdbool.h>
#include <stdint.h>
#define BOSS_SOURCE_CAP 768
#define BOSS_REPLY_CAP 256

typedef enum { BOSS_OFF, BOSS_CONNECTING, BOSS_LISTENING, BOSS_ERROR } boss_state_t;
typedef enum { BOSS_TURN_LISTEN, BOSS_TURN_THINK, BOSS_TURN_SHOW } boss_turn_t;
typedef struct {
    uint32_t revision;
    boss_state_t state;
    boss_turn_t turn;
    unsigned level;
    bool partial;
    bool translated;
    uint32_t utterance;
    char source[BOSS_SOURCE_CAP];
    char reply[BOSS_REPLY_CAP];
    char effect[80];
    char status[96];
} boss_snapshot_t;
/* Start/stop/poll are called from the LVGL task. Workers never touch LVGL. */
bool boss_service_start(void);
void boss_service_stop(void);
bool boss_service_busy(void);
void boss_service_snapshot(boss_snapshot_t *out);
