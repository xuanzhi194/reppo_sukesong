#include "word_storage.h"
#include <stdlib.h>
#include <stdbool.h>
#include "esp_attr.h"
#include "esp_random.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "DICT_READER";
static const char *REVIEW_INDEX_FILE = "/spiffs/word_review_index.csv";

word_item_t current_word = {
    .definition = "",
    .phonetic = "",
    .word = ""
};

/* ===== runtime session state (kept inside this module) ===== */
static int  s_total_count = 0;

static int  s_round_indices[WORDCARD_ROUND_SIZE] EXT_RAM_BSS_ATTR = {0};
static int  s_round_size = 0;
static int  s_learn_cursor = 0;
static bool s_learn_initial_pending = false;

static int  s_review_indices[WORDCARD_MAX_REVIEW_ITEMS] EXT_RAM_BSS_ATTR = {0};
static int  s_review_count = 0;
static int  s_review_session_total = 0;
static int  s_review_session_processed = 0;
static bool s_review_initial_pending = false;

static word_item_t s_round_words[WORDCARD_ROUND_SIZE] EXT_RAM_BSS_ATTR;

static long *s_line_offsets = NULL;
static int   s_line_offset_count = 0;

static TaskHandle_t s_review_save_task = NULL;

/* ===== filesystem helpers ===== */
static const char *resolve_csv_path(void)
{
    static const char *candidates[] = {
        "/spiffs/wordcards.csv",
        "/spiffs/wordcard.csv",
    };
    for (int i = 0; i < (int)(sizeof(candidates) / sizeof(candidates[0])); i++) {
        FILE *f = fopen(candidates[i], "r");
        if (f != NULL) {
            fclose(f);
            return candidates[i];
        }
    }
    return NULL;
}

const char *word_storage_get_active_csv_path(void)
{
    return resolve_csv_path();
}

void init_spiffs(void) {
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = "storage",
        .max_files = 5,
        .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount or format filesystem");
    } else {
        ESP_LOGI(TAG, "SPIFFS mounted successfully");
    }
}

static void word_storage_free_line_index(void)
{
    if (s_line_offsets != NULL) {
        heap_caps_free(s_line_offsets);
    }
    s_line_offsets = NULL;
    s_line_offset_count = 0;
}

static bool word_storage_build_line_index(const char *csv_path)
{
    word_storage_free_line_index();

    FILE *f = fopen(csv_path, "r");
    if (f == NULL) {
        return false;
    }

    int cap = 256;
    int count = 0;
    long *offsets = (long *)heap_caps_malloc(sizeof(long) * cap, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (offsets == NULL) {
        offsets = (long *)malloc(sizeof(long) * cap);
    }
    if (offsets == NULL) {
        fclose(f);
        return false;
    }

    char line_buffer[256];
    while (fgets(line_buffer, sizeof(line_buffer), f) != NULL) {
        if (line_buffer[0] == '\n' || line_buffer[0] == '\r' || line_buffer[0] == '\0') {
            continue;
        }
        if (count >= cap) {
            cap *= 2;
            long *grown = (long *)heap_caps_realloc(offsets, sizeof(long) * cap, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
            if (grown == NULL) {
                grown = (long *)realloc(offsets, sizeof(long) * cap);
            }
            if (grown == NULL) {
                heap_caps_free(offsets);
                fclose(f);
                return false;
            }
            offsets = grown;
        }
        offsets[count++] = ftell(f) - (long)strlen(line_buffer);
    }

    fclose(f);
    s_line_offsets = offsets;
    s_line_offset_count = count;
    return true;
}

static bool word_storage_parse_line(const char *line_buffer, word_item_t *out_item)
{
    char local[256];
    strncpy(local, line_buffer, sizeof(local) - 1);
    local[sizeof(local) - 1] = '\0';

    char *token = strtok(local, ",");
    if (token) {
        strncpy(out_item->word, token, sizeof(out_item->word) - 1);
    }

    token = strtok(NULL, ",");
    if (token) {
        strncpy(out_item->phonetic, token, sizeof(out_item->phonetic) - 1);
    }

    token = strtok(NULL, "\n");
    if (token) {
        strncpy(out_item->definition, token, sizeof(out_item->definition) - 1);
    }

    return out_item->word[0] != '\0';
}

static void review_save_task(void *param)
{
    (void)param;
    word_storage_save_review_indices(s_review_indices, s_review_count);
    s_review_save_task = NULL;
    vTaskDelete(NULL);
}

static void word_storage_schedule_review_save(void)
{
    if (s_review_save_task != NULL) {
        return;
    }
    xTaskCreatePinnedToCore(review_save_task, "review_save", 4096, NULL, 3, &s_review_save_task, 0);
}

/* ===== low level dictionary access ===== */
int word_storage_get_total_count(void)
{
    if (s_line_offset_count > 0) {
        return s_line_offset_count;
    }

    const char *csv_path = resolve_csv_path();
    if (csv_path == NULL) {
        ESP_LOGW(TAG, "No word csv found: wordcards.csv/wordcard.csv");
        return 0;
    }

    FILE *f = fopen(csv_path, "r");
    if (f == NULL) {
        ESP_LOGI(TAG, "Failed to open csv when counting lines");
        return 0;
    }

    char line_buffer[256];
    int line_count = 0;
    while (fgets(line_buffer, sizeof(line_buffer), f) != NULL) {
        if (line_buffer[0] == '\n' || line_buffer[0] == '\r' || line_buffer[0] == '\0') continue;
        line_count++;
    }

    fclose(f);
    return line_count;
}

bool word_storage_get_word_by_index(int line_num, word_item_t *out_item)
{
    const char *csv_path = resolve_csv_path();
    if (csv_path == NULL || out_item == NULL || line_num < 0) return false;

    memset(out_item, 0, sizeof(*out_item));

    FILE *f = fopen(csv_path, "r");
    if (f == NULL) {
        ESP_LOGI(TAG, "Failed to open file");
        return false;
    }

    bool found = false;
    char line_buffer[256];

    if (s_line_offsets != NULL && line_num < s_line_offset_count) {
        if (fseek(f, s_line_offsets[line_num], SEEK_SET) == 0 &&
            fgets(line_buffer, sizeof(line_buffer), f) != NULL) {
            found = word_storage_parse_line(line_buffer, out_item);
        }
    } else {
        int current_line = 0;
        while (fgets(line_buffer, sizeof(line_buffer), f) != NULL) {
            if (line_buffer[0] == '\n' || line_buffer[0] == '\r' || line_buffer[0] == '\0') {
                continue;
            }
            if (current_line == line_num) {
                found = word_storage_parse_line(line_buffer, out_item);
                break;
            }
            current_line++;
        }
    }

    fclose(f);
    return found;
}

int word_storage_pick_random_indices(int *out_indices, int out_count)
{
    if (out_indices == NULL || out_count <= 0) return 0;

    int total = (s_total_count > 0) ? s_total_count : word_storage_get_total_count();
    if (total <= 0) return 0;
    int target = (total < out_count) ? total : out_count;

    int *pool = (int *)malloc(sizeof(int) * total);
    if (pool == NULL) return 0;

    for (int i = 0; i < total; i++) pool[i] = i;
    for (int i = 0; i < target; i++) {
        int j = i + (esp_random() % (total - i));
        int tmp = pool[i];
        pool[i] = pool[j];
        pool[j] = tmp;
        out_indices[i] = pool[i];
    }
    free(pool);
    return target;
}

/* ===== review index persistence ===== */
int word_storage_load_review_indices(int *out_indices, int max_count)
{
    if (out_indices == NULL || max_count <= 0) return 0;
    int total = (s_total_count > 0) ? s_total_count : word_storage_get_total_count();
    int loaded = 0;

    FILE *f = fopen(REVIEW_INDEX_FILE, "r");
    if (f == NULL) return 0;

    char line[32];
    while (fgets(line, sizeof(line), f) != NULL && loaded < max_count) {
        int idx = atoi(line);
        if (idx < 0 || idx >= total) continue;

        bool duplicated = false;
        for (int i = 0; i < loaded; i++) {
            if (out_indices[i] == idx) {
                duplicated = true;
                break;
            }
        }
        if (!duplicated) {
            out_indices[loaded++] = idx;
        }
    }

    fclose(f);
    return loaded;
}

bool word_storage_save_review_indices(const int *indices, int count)
{
    if (indices == NULL || count < 0) return false;
    FILE *f = fopen(REVIEW_INDEX_FILE, "w");
    if (f == NULL) return false;

    for (int i = 0; i < count; i++) {
        fprintf(f, "%d\n", indices[i]);
    }
    fclose(f);
    return true;
}

int word_storage_get_review_count(void)
{
    return s_review_count;
}

/* ===== init ===== */
void word_storage_init(void)
{
    const char *csv_path = resolve_csv_path();
    if (csv_path != NULL) {
        word_storage_build_line_index(csv_path);
    }
    s_total_count = s_line_offset_count;
    s_review_count = word_storage_load_review_indices(s_review_indices,
                                                      WORDCARD_MAX_REVIEW_ITEMS);
    s_round_size = 0;
    s_learn_cursor = 0;
    s_learn_initial_pending = false;
    s_review_session_total = 0;
    s_review_session_processed = 0;
    s_review_initial_pending = false;
}

/* ===== learn session ===== */
static bool review_index_append_if_needed(int csv_idx)
{
    if (csv_idx < 0 || csv_idx >= s_total_count) return false;
    for (int i = 0; i < s_review_count; i++) {
        if (s_review_indices[i] == csv_idx) return false;
    }
    if (s_review_count >= WORDCARD_MAX_REVIEW_ITEMS) return false;
    s_review_indices[s_review_count++] = csv_idx;
    word_storage_schedule_review_save();
    return true;
}

int word_storage_learn_start(int round_size)
{
    int target = (round_size > 0) ? round_size : WORDCARD_ROUND_SIZE;
    if (target > WORDCARD_ROUND_SIZE) target = WORDCARD_ROUND_SIZE;
    s_round_size = word_storage_pick_random_indices(s_round_indices, target);
    for (int i = 0; i < s_round_size; i++) {
        if (!word_storage_get_word_by_index(s_round_indices[i], &s_round_words[i])) {
            memset(&s_round_words[i], 0, sizeof(s_round_words[i]));
        }
    }
    s_learn_cursor = 0;
    s_learn_initial_pending = true;
    return s_round_size;
}

bool word_storage_learn_current(word_session_state_t *out)
{
    if (out == NULL) return false;
    memset(out, 0, sizeof(*out));
    out->total = s_round_size;
    out->current = s_learn_cursor + 1;
    out->finished = (s_round_size <= 0) || (s_learn_cursor >= s_round_size);
    if (out->finished) return false;
    out->word = s_round_words[s_learn_cursor];
    out->valid = out->word.word[0] != '\0';
    return out->valid;
}

void word_storage_learn_mark_known(void)
{
    if (s_learn_cursor < s_round_size) s_learn_cursor++;
}

void word_storage_learn_mark_wrong(void)
{
    if (s_learn_cursor < s_round_size) {
        review_index_append_if_needed(s_round_indices[s_learn_cursor]);
        s_learn_cursor++;
    }
}

bool word_storage_learn_signal(bool marked_wrong, word_session_state_t *out)
{
    if (!s_learn_initial_pending) {
        if (marked_wrong) {
            word_storage_learn_mark_wrong();
        } else {
            word_storage_learn_mark_known();
        }
    } else {
        s_learn_initial_pending = false;
    }
    return word_storage_learn_current(out);
}

bool word_storage_learn_finished(void)
{
    return (s_round_size <= 0) || (s_learn_cursor >= s_round_size);
}

/* ===== review session ===== */
int word_storage_review_start(void)
{
    s_review_session_total = s_review_count;
    s_review_session_processed = 0;
    s_review_initial_pending = true;
    return s_review_session_total;
}

bool word_storage_review_current(word_session_state_t *out)
{
    if (out == NULL) return false;
    memset(out, 0, sizeof(*out));
    out->total = s_review_session_total;
    out->current = s_review_session_processed + 1;
    out->finished = word_storage_review_finished();
    if (out->finished) return false;
    out->valid = word_storage_get_word_by_index(s_review_indices[0], &out->word);
    return out->valid;
}

void word_storage_review_mark_known(void)
{
    if (s_review_count <= 0) return;
    for (int i = 0; i < s_review_count - 1; i++) {
        s_review_indices[i] = s_review_indices[i + 1];
    }
    s_review_count--;
    word_storage_schedule_review_save();
    s_review_session_processed++;
}

void word_storage_ensure_review_saved(void)
{
    for (int i = 0; i < 200 && s_review_save_task != NULL; i++) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

bool word_storage_review_signal(word_session_state_t *out)
{
    if (!s_review_initial_pending) {
        word_storage_review_mark_known();
    } else {
        s_review_initial_pending = false;
    }
    return word_storage_review_current(out);
}

bool word_storage_review_finished(void)
{
    return (s_review_session_total <= 0)
        || (s_review_session_processed >= s_review_session_total)
        || (s_review_count <= 0);
}

/* ===== legacy wrappers ===== */
int get_wordcard_line_count(void)
{
    return word_storage_get_total_count();
}

bool get_word_from_csv(int line_num, word_item_t *out_item)
{
    return word_storage_get_word_by_index(line_num, out_item);
}
