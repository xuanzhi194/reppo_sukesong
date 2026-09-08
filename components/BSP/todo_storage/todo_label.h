#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Same four buckets as the claw-machine (gash) cats, plus "App left it blank". */
#ifndef TODO_LABEL_NONE
#define TODO_LABEL_NONE  0
#define TODO_LABEL_WORK  1
#define TODO_LABEL_STUDY 2
#define TODO_LABEL_LIFE  3
#define TODO_LABEL_FUN   4
#endif

/** CSV / BLE wire token: "" / "work" / "study" / "life" / "fun". */
const char *todo_label_token(uint8_t label);

/** UI English name matching gash_cat_names; empty when unlabeled. */
const char *todo_label_display_name(uint8_t label);

/**
 * Accepts the wire token, gash English names, Chinese 工作/学习/生活/娱乐,
 * and decimal "1".."4". Unknown / empty → TODO_LABEL_NONE.
 */
uint8_t todo_label_parse(const char *s);

/** 0 = none, 1..4 = work..fun; anything else → none. */
uint8_t todo_label_from_int(int v);

/** Gash cat_idx 0..3 → WORK..FUN; out of range → none. */
uint8_t todo_label_from_gash_cat(int cat_idx);

#ifdef __cplusplus
}
#endif
