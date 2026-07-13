#include "word_storage.h"
#include <stdlib.h>
#include <stdbool.h>
#include "esp_attr.h"
#include "esp_crc.h"
#include "esp_random.h"
#include "esp_heap_caps.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <sys/stat.h>
#include <unistd.h>

static const char *TAG = "DICT_READER";
static const char *REVIEW_INDEX_FILE = "/spiffs/word_review_index.csv";
static const char *WORD_NVS_NAMESPACE = "WordData";
static const char *WORD_NVS_KEY_LEARNED = "learned_cnt";
static const char *WORD_NVS_KEY_BOOK_SIZE = "book_size";
static const char *WORD_NVS_KEY_BOOK_CRC = "book_crc";

#define WORD_CSV_PATH        "/spiffs/wordcards.csv"
#define WORD_CSV_LEGACY_PATH "/spiffs/wordcard.csv"
#define WORD_TMP_PATH        "/spiffs/wordcards.tmp"
#define WORD_BAK_PATH        "/spiffs/wordcards.bak"

#define WORD_STORAGE_MAX_LINE_BYTES 255

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
static int s_learned_count = 0;
static bool s_word_rx_active = false;
static uint32_t s_word_rx_total = 0;
static uint32_t s_word_rx_crc_expected = 0;
static uint32_t s_word_rx_expected_words = 0;
static uint32_t s_word_rx_received_max = 0;
static FILE *s_word_rx_fp = NULL;

static const char *resolve_csv_path(void);
static uint32_t word_storage_file_crc32(const char *path);

/* ===== learned-count persistence (NVS) ===== */
static int word_storage_dict_total(void)
{
    return (s_total_count > 0) ? s_total_count : word_storage_get_total_count();
}

static bool word_storage_active_book_signature(uint32_t *out_size,
                                               uint32_t *out_crc)
{
    const char *csv_path = resolve_csv_path();
    if (csv_path == NULL || out_size == NULL || out_crc == NULL) {
        return false;
    }

    struct stat st;
    if (stat(csv_path, &st) != 0 || st.st_size < 0) {
        return false;
    }

    *out_size = (uint32_t)st.st_size;
    *out_crc = word_storage_file_crc32(csv_path);
    return true;
}

static bool word_storage_save_book_signature(void)
{
    uint32_t size = 0;
    uint32_t crc = 0;
    if (!word_storage_active_book_signature(&size, &crc)) {
        return false;
    }

    nvs_handle_t h;
    esp_err_t err = nvs_open(WORD_NVS_NAMESPACE, NVS_READWRITE, &h);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "open NVS failed when saving book signature");
        return false;
    }

    err = nvs_set_u32(h, WORD_NVS_KEY_BOOK_SIZE, size);
    if (err == ESP_OK) {
        err = nvs_set_u32(h, WORD_NVS_KEY_BOOK_CRC, crc);
    }
    if (err == ESP_OK) {
        err = nvs_commit(h);
    }
    nvs_close(h);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "save book signature failed: %s", esp_err_to_name(err));
        return false;
    }
    return true;
}

static bool word_storage_save_learned_count(void)
{
    nvs_handle_t h;
    if (nvs_open(WORD_NVS_NAMESPACE, NVS_READWRITE, &h) != ESP_OK) {
        ESP_LOGW(TAG, "open NVS failed when saving learned count");
        return false;
    }
    esp_err_t err = nvs_set_i32(h, WORD_NVS_KEY_LEARNED,
                                (int32_t)s_learned_count);
    if (err == ESP_OK) {
        err = nvs_commit(h);
    }
    nvs_close(h);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "save learned count failed: %s", esp_err_to_name(err));
        return false;
    }
    return true;
}

static void word_storage_normalize_learned_count(void)
{
    int total = word_storage_dict_total();
    if (total > 0 && s_learned_count >= total) {
        s_learned_count = 0;
        word_storage_save_learned_count();
    }
}

static void word_storage_load_learned_count(void)
{
    nvs_handle_t h;
    s_learned_count = 0;
    if (nvs_open(WORD_NVS_NAMESPACE, NVS_READONLY, &h) != ESP_OK) {
        word_storage_save_book_signature();
        return;
    }

    uint32_t current_size = 0;
    uint32_t current_crc = 0;
    uint32_t stored_size = 0;
    uint32_t stored_crc = 0;
    bool has_current_sig = word_storage_active_book_signature(&current_size,
                                                              &current_crc);
    bool has_stored_sig =
        nvs_get_u32(h, WORD_NVS_KEY_BOOK_SIZE, &stored_size) == ESP_OK &&
        nvs_get_u32(h, WORD_NVS_KEY_BOOK_CRC, &stored_crc) == ESP_OK;

    int32_t val = 0;
    if (nvs_get_i32(h, WORD_NVS_KEY_LEARNED, &val) == ESP_OK && val >= 0) {
        s_learned_count = (int)val;
    }
    nvs_close(h);

    if (has_current_sig && has_stored_sig &&
        (stored_size != current_size || stored_crc != current_crc)) {
        ESP_LOGI(TAG, "wordbook changed, reset progress");
        word_storage_reset_all_progress();
        return;
    }
    if (has_current_sig && !has_stored_sig) {
        word_storage_save_book_signature();
    }

    word_storage_normalize_learned_count();
}

static void word_storage_learned_increment(void)
{
    int total = word_storage_dict_total();
    if (total <= 0) {
        total = 1;
    }
    s_learned_count++;
    if (s_learned_count >= total) {
        s_learned_count = 0;
    }
    word_storage_save_learned_count();
}

int word_storage_get_learned_count(void)
{
    return s_learned_count;
}

void word_storage_reload_progress_snapshot(void)
{
    word_storage_ensure_review_saved();
    word_storage_load_learned_count();
    memset(s_review_indices, 0, sizeof(s_review_indices));
    s_review_count = word_storage_load_review_indices(s_review_indices,
                                                      WORDCARD_MAX_REVIEW_ITEMS);
}

void word_storage_reset_all_progress(void)
{
    word_storage_ensure_review_saved();

    s_learned_count = 0;
    word_storage_save_learned_count();
    word_storage_save_book_signature();

    memset(s_review_indices, 0, sizeof(s_review_indices));
    s_review_count = 0;
    if (!word_storage_save_review_indices(s_review_indices, 0)) {
        ESP_LOGW(TAG, "clear review index file failed");
    }

    memset(s_round_indices, 0, sizeof(s_round_indices));
    memset(s_round_words, 0, sizeof(s_round_words));
    s_round_size = 0;
    s_learn_cursor = 0;
    s_learn_initial_pending = false;
    s_review_session_total = 0;
    s_review_session_processed = 0;
    s_review_initial_pending = false;
}

/* ===== filesystem helpers ===== */
static const char *resolve_csv_path(void)
{
    static const char *candidates[] = {
        WORD_CSV_PATH,
        WORD_CSV_LEGACY_PATH,
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
        uint8_t *bytes = (uint8_t *)token;
        if (bytes[0] == 0xef && bytes[1] == 0xbb && bytes[2] == 0xbf) {
            token += 3;
        }
        strncpy(out_item->word, token, sizeof(out_item->word) - 1);
    }

    token = strtok(NULL, ",");
    if (token) {
        strncpy(out_item->phonetic, token, sizeof(out_item->phonetic) - 1);
    }

    token = strtok(NULL, "\n");
    if (token) {
        size_t len = strlen(token);
        while (len > 0 && token[len - 1] == '\r') {
            token[--len] = '\0';
        }
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
    word_storage_load_learned_count();
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
    if (s_learn_cursor < s_round_size) {
        s_learn_cursor++;
        word_storage_learned_increment();
    }
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

/* ===== BLE wordbook upload: tmp write + validated replace ===== */
static void word_storage_rx_close_file(void)
{
    if (s_word_rx_fp != NULL) {
        fclose(s_word_rx_fp);
        s_word_rx_fp = NULL;
    }
}

static uint32_t word_storage_file_crc32(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        return 0;
    }

    uint32_t crc = 0;
    uint8_t buf[1024];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), f)) > 0) {
        crc = esp_crc32_le(crc, buf, n);
    }
    fclose(f);
    return crc;
}

static void word_storage_trim_line_end(char *line)
{
    size_t len = strlen(line);
    while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
        line[--len] = '\0';
    }
}

static bool word_storage_csv_line_valid(char *line)
{
    uint8_t *bytes = (uint8_t *)line;
    char *p = line;
    if (bytes[0] == 0xef && bytes[1] == 0xbb && bytes[2] == 0xbf) {
        p += 3;
    }

    char *comma1 = strchr(p, ',');
    if (comma1 == NULL || comma1 == p) {
        return false;
    }
    char *comma2 = strchr(comma1 + 1, ',');
    if (comma2 == NULL || comma2[1] == '\0') {
        return false;
    }
    return true;
}

static esp_err_t word_storage_validate_csv_file(const char *path,
                                                uint32_t expected_words,
                                                uint32_t *actual_words)
{
    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        return ESP_ERR_NOT_FOUND;
    }

    uint32_t count = 0;
    char line[WORD_STORAGE_MAX_LINE_BYTES + 2];
    while (fgets(line, sizeof(line), f) != NULL) {
        size_t len = strlen(line);
        bool has_eol = len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r');
        if (!has_eol && !feof(f)) {
            fclose(f);
            ESP_LOGW(TAG, "wordbook line too long");
            return ESP_ERR_INVALID_SIZE;
        }

        word_storage_trim_line_end(line);
        if (line[0] == '\0') {
            continue;
        }
        if (!word_storage_csv_line_valid(line)) {
            fclose(f);
            ESP_LOGW(TAG, "wordbook invalid csv line near %lu", (unsigned long)count + 1);
            return ESP_ERR_INVALID_ARG;
        }

        count++;
        if (count > WORD_STORAGE_MAX_UPLOAD_WORDS) {
            fclose(f);
            ESP_LOGW(TAG, "wordbook too many words: %lu", (unsigned long)count);
            return ESP_ERR_INVALID_SIZE;
        }
    }

    fclose(f);
    if (count == 0) {
        return ESP_ERR_INVALID_SIZE;
    }
    if (expected_words > 0 && count != expected_words) {
        ESP_LOGW(TAG, "wordbook word count mismatch got=%lu expect=%lu",
                 (unsigned long)count, (unsigned long)expected_words);
        return ESP_ERR_INVALID_SIZE;
    }
    if (actual_words != NULL) {
        *actual_words = count;
    }
    return ESP_OK;
}

static bool word_storage_reload_current_csv(void)
{
    const char *csv_path = resolve_csv_path();
    if (csv_path == NULL) {
        word_storage_free_line_index();
        s_total_count = 0;
        return false;
    }

    if (!word_storage_build_line_index(csv_path)) {
        s_total_count = 0;
        return false;
    }
    s_total_count = s_line_offset_count;
    return s_total_count > 0;
}

static void word_storage_reset_progress_after_replace(void)
{
    word_storage_reset_all_progress();
    word_storage_reload_progress_snapshot();
}

bool word_storage_rx_in_progress(void)
{
    return s_word_rx_active;
}

void word_storage_rx_abort(void)
{
    word_storage_rx_close_file();
    if (s_word_rx_active) {
        unlink(WORD_TMP_PATH);
    }
    s_word_rx_active = false;
    s_word_rx_total = 0;
    s_word_rx_crc_expected = 0;
    s_word_rx_expected_words = 0;
    s_word_rx_received_max = 0;
    ESP_LOGW(TAG, "wordbook RX aborted");
}

esp_err_t word_storage_rx_begin(uint32_t total_size, uint32_t crc32,
                                uint32_t expected_words, uint8_t format)
{
    if (format != WORD_FMT_CSV) {
        ESP_LOGW(TAG, "unsupported wordbook format %u", (unsigned)format);
        return ESP_ERR_NOT_SUPPORTED;
    }
    if (total_size == 0 || total_size > WORD_STORAGE_MAX_UPLOAD_BYTES) {
        ESP_LOGW(TAG, "invalid wordbook size %lu", (unsigned long)total_size);
        return ESP_ERR_INVALID_SIZE;
    }
    if (expected_words == 0 || expected_words > WORD_STORAGE_MAX_UPLOAD_WORDS) {
        ESP_LOGW(TAG, "invalid wordbook count %lu", (unsigned long)expected_words);
        return ESP_ERR_INVALID_SIZE;
    }

    word_storage_rx_abort();
    unlink(WORD_TMP_PATH);

    s_word_rx_fp = fopen(WORD_TMP_PATH, "wb");
    if (s_word_rx_fp == NULL) {
        ESP_LOGE(TAG, "open wordbook tmp failed");
        return ESP_FAIL;
    }

    s_word_rx_active = true;
    s_word_rx_total = total_size;
    s_word_rx_crc_expected = crc32;
    s_word_rx_expected_words = expected_words;
    s_word_rx_received_max = 0;
    ESP_LOGI(TAG, "wordbook RX begin size=%lu words=%lu crc=0x%08lX",
             (unsigned long)total_size, (unsigned long)expected_words,
             (unsigned long)crc32);
    return ESP_OK;
}

esp_err_t word_storage_rx_write(uint32_t offset, const uint8_t *data, size_t len)
{
    if (!s_word_rx_active || s_word_rx_fp == NULL || data == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    if (len == 0) {
        return ESP_OK;
    }
    if (offset + len > s_word_rx_total) {
        ESP_LOGW(TAG, "wordbook write overflow off=%lu len=%u total=%lu",
                 (unsigned long)offset, (unsigned)len, (unsigned long)s_word_rx_total);
        return ESP_ERR_INVALID_SIZE;
    }

    if (fseek(s_word_rx_fp, (long)offset, SEEK_SET) != 0) {
        ESP_LOGE(TAG, "wordbook fseek failed off=%lu", (unsigned long)offset);
        return ESP_FAIL;
    }
    if (fwrite(data, 1, len, s_word_rx_fp) != len) {
        ESP_LOGE(TAG, "wordbook fwrite failed off=%lu len=%u",
                 (unsigned long)offset, (unsigned)len);
        return ESP_FAIL;
    }

    uint32_t end = offset + (uint32_t)len;
    if (end > s_word_rx_received_max) {
        s_word_rx_received_max = end;
    }
    return ESP_OK;
}

esp_err_t word_storage_rx_commit(void)
{
    if (!s_word_rx_active || s_word_rx_fp == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    fflush(s_word_rx_fp);
    word_storage_rx_close_file();

    struct stat st;
    if (stat(WORD_TMP_PATH, &st) != 0 || (uint32_t)st.st_size != s_word_rx_total) {
        long got_size = (stat(WORD_TMP_PATH, &st) == 0) ? (long)st.st_size : -1L;
        ESP_LOGE(TAG, "wordbook size mismatch got=%ld expect=%lu",
                 got_size, (unsigned long)s_word_rx_total);
        word_storage_rx_abort();
        return ESP_ERR_INVALID_SIZE;
    }
    if (s_word_rx_received_max < s_word_rx_total) {
        ESP_LOGE(TAG, "wordbook incomplete received=%lu total=%lu",
                 (unsigned long)s_word_rx_received_max,
                 (unsigned long)s_word_rx_total);
        word_storage_rx_abort();
        return ESP_ERR_INVALID_SIZE;
    }

    uint32_t crc = word_storage_file_crc32(WORD_TMP_PATH);
    if (crc != s_word_rx_crc_expected) {
        ESP_LOGE(TAG, "wordbook CRC mismatch got=0x%08lX expect=0x%08lX",
                 (unsigned long)crc, (unsigned long)s_word_rx_crc_expected);
        word_storage_rx_abort();
        return ESP_ERR_INVALID_CRC;
    }

    uint32_t actual_words = 0;
    esp_err_t err = word_storage_validate_csv_file(WORD_TMP_PATH,
                                                   s_word_rx_expected_words,
                                                   &actual_words);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "wordbook validation failed: %s", esp_err_to_name(err));
        word_storage_rx_abort();
        return err;
    }

    unlink(WORD_BAK_PATH);
    struct stat current_st;
    bool had_current = stat(WORD_CSV_PATH, &current_st) == 0;
    if (had_current && rename(WORD_CSV_PATH, WORD_BAK_PATH) != 0) {
        ESP_LOGE(TAG, "wordbook backup rename failed");
        word_storage_rx_abort();
        return ESP_FAIL;
    }
    if (rename(WORD_TMP_PATH, WORD_CSV_PATH) != 0) {
        ESP_LOGE(TAG, "wordbook tmp rename failed");
        if (had_current) {
            rename(WORD_BAK_PATH, WORD_CSV_PATH);
        }
        word_storage_rx_abort();
        return ESP_FAIL;
    }

    if (!word_storage_reload_current_csv()) {
        ESP_LOGE(TAG, "wordbook reload failed, restoring previous csv");
        unlink(WORD_CSV_PATH);
        if (had_current) {
            rename(WORD_BAK_PATH, WORD_CSV_PATH);
            word_storage_reload_current_csv();
        }
        word_storage_rx_abort();
        return ESP_FAIL;
    }

    unlink(WORD_BAK_PATH);
    word_storage_reset_progress_after_replace();

    s_word_rx_active = false;
    s_word_rx_total = 0;
    s_word_rx_crc_expected = 0;
    s_word_rx_expected_words = 0;
    s_word_rx_received_max = 0;
    ESP_LOGI(TAG, "wordbook RX commit OK (%lu bytes, %lu words)",
             (unsigned long)st.st_size, (unsigned long)actual_words);
    return ESP_OK;
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
