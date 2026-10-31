#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
typedef enum {
    HEY_NONE, HEY_HOME, HEY_TODO, HEY_DONE, HEY_FOCUS_PAGE, HEY_START_FOCUS,
    HEY_BOSS, HEY_KEEP, HEY_GASH, HEY_BOOK, HEY_GAME, HEY_SETTINGS, HEY_IDEAS,
    HEY_PHOTO, HEY_ANSWER, HEY_CLOCK, HEY_CANCEL, HEY_DISABLE, HEY_INVALID
} hey_action_t;
typedef struct { hey_action_t action; unsigned minutes; } hey_intent_t;
typedef struct {
    bool awake, pending;
    uint32_t deadline, revision;
    hey_intent_t intent;
    char heard[192], hint[128];
} hey_dialog_t;
/* Caller-owned scratch permits parsing on a small UI stack. */
typedef struct { char normalized[768], tail[768]; } hey_parse_workspace_t;
void hey_dialog_receive_with_workspace(hey_dialog_t *d, const char *text, bool final,
                                       uint32_t now, hey_parse_workspace_t *work);
/* Pure bounded parser: explicit supported commands only, never arbitrary actions. */
hey_intent_t hey_parse_command(const char *text);
bool hey_wake_tail(const char *text, char *tail, size_t cap);
void hey_dialog_receive(hey_dialog_t *d, const char *text, bool final, uint32_t now);
void hey_dialog_tick(hey_dialog_t *d, uint32_t now);
void hey_dialog_reset(hey_dialog_t *d);
