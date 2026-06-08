#ifndef __WORD_STORAGE_H__
#define __WORD_STORAGE_H__

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "esp_spiffs.h"
#include "esp_log.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WORDCARD_ROUND_SIZE
#define WORDCARD_ROUND_SIZE 30
#endif

#ifndef WORDCARD_MAX_REVIEW_ITEMS
/* Upper bound of wrong words tracked across power cycles.
 * Each entry is one int (4B) and lives in .bss, keep it small.
 * Override at compile time if you really need more. */
#define WORDCARD_MAX_REVIEW_ITEMS 500
#endif

typedef struct {
    char word[32];
    char phonetic[32];
    char definition[64];
} word_item_t;

/* A snapshot of current learn/review session position. */
typedef struct {
    word_item_t word;       /* current word data (only meaningful when valid==true) */
    int  current;           /* 1-based current position */
    int  total;             /* total cards in current session */
    bool valid;             /* false when CSV row missing or session finished */
    bool finished;          /* true when no more words to process */
} word_session_state_t;

extern word_item_t current_word;

/* ----- filesystem & init ----- */
void init_spiffs(void);

/* Initializes runtime state: load total word count + persisted review indices.
 * Call once after init_spiffs(). */
void word_storage_init(void);

/* ----- low level dictionary access ----- */
const char *word_storage_get_active_csv_path(void);
int  word_storage_get_total_count(void);
bool word_storage_get_word_by_index(int line_num, word_item_t *out_item);
int  word_storage_pick_random_indices(int *out_indices, int out_count);

/* ----- review-index persistence ----- */
int  word_storage_load_review_indices(int *out_indices, int max_count);
bool word_storage_save_review_indices(const int *indices, int count);
int  word_storage_get_review_count(void);

/* ----- learned progress (wordmenu, persisted in NVS) ----- */
int  word_storage_get_learned_count(void);

/* ----- learn session (wordcard) ----- */
int  word_storage_learn_start(int round_size);
bool word_storage_learn_current(word_session_state_t *out);
void word_storage_learn_mark_known(void);
void word_storage_learn_mark_wrong(void);
bool word_storage_learn_signal(bool marked_wrong, word_session_state_t *out);
bool word_storage_learn_finished(void);

/* ----- review session (reviewcard) ----- */
int  word_storage_review_start(void);
bool word_storage_review_current(word_session_state_t *out);
void word_storage_review_mark_known(void);
bool word_storage_review_signal(word_session_state_t *out);
bool word_storage_review_finished(void);
void word_storage_ensure_review_saved(void);

/* ----- legacy wrappers ----- */
bool get_word_from_csv(int line_num, word_item_t *out_item);
int  get_wordcard_line_count(void);

#ifdef __cplusplus
}
#endif

#endif
