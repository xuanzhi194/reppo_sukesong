/* Dino / Tilt Orb record persistence. No LVGL, worker task or generated UI. */
#include "game_score_storage.h"
#include "esp_log.h"
#include "nvs.h"
#include <stdbool.h>

#define GAME_SCORE_NVS_NS "DinoGame"
#define GAME_SCORE_DINO_KEY "best"
#define GAME_SCORE_TILT_KEY "tilt"
#define GAME_SCORE_RETRY_MS 5000u

typedef struct {
    uint32_t best;
    uint32_t persisted;
    bool loaded;
    const char *key;
} score_slot_t;

static const char *TAG = "DINO_SCORE";
static score_slot_t s_dino = { .key = GAME_SCORE_DINO_KEY };
static score_slot_t s_tilt = { .key = GAME_SCORE_TILT_KEY };
static uint32_t s_last_failure;
static bool s_retry_pending;

static bool slots_ready(void)
{
    return s_dino.loaded && s_tilt.loaded;
}

static esp_err_t slot_read(nvs_handle_t handle, score_slot_t *slot)
{
    uint32_t stored = 0;
    esp_err_t err = nvs_get_u32(handle, slot->key, &stored);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        stored = 0;
        err = ESP_OK;
    }
    if (err != ESP_OK) return err;
    slot->persisted = stored;
    if (stored > slot->best) slot->best = stored;
    slot->loaded = true;
    return ESP_OK;
}

static esp_err_t score_load(void)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(GAME_SCORE_NVS_NS, NVS_READONLY, &handle);
    if (err == ESP_OK) {
        err = slot_read(handle, &s_dino);
        if (err == ESP_OK) err = slot_read(handle, &s_tilt);
        nvs_close(handle);
    }
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        s_dino.persisted = 0;
        s_tilt.persisted = 0;
        s_dino.loaded = s_tilt.loaded = true;
        return ESP_OK;
    }
    return err;
}

static void score_retry(esp_err_t err, uint32_t now_ms)
{
    s_retry_pending = true;
    s_last_failure = now_ms;
    ESP_LOGW(TAG, "Record NVS error: %s; will retry", esp_err_to_name(err));
}

void game_score_storage_init(void)
{
    s_dino.best = s_dino.persisted = 0;
    s_tilt.best = s_tilt.persisted = 0;
    s_dino.loaded = s_tilt.loaded = false;
    s_last_failure = 0;
    s_retry_pending = false;
    esp_err_t err = score_load();
    if (err != ESP_OK) score_retry(err, 0);
}

uint32_t game_score_storage_best(void)
{
    return s_dino.best;
}

uint32_t game_score_storage_tilt_best(void)
{
    return s_tilt.best;
}

void game_score_storage_offer(uint32_t best)
{
    if (best > s_dino.best) s_dino.best = best;
}

void game_score_storage_tilt_offer(uint32_t best)
{
    if (best > s_tilt.best) s_tilt.best = best;
}

void game_score_storage_poll(uint32_t now_ms)
{
    if (s_retry_pending && now_ms - s_last_failure < GAME_SCORE_RETRY_MS) return;
    s_retry_pending = false;
    if (!slots_ready()) {
        /* A read error must never let a lower score overwrite an existing record. */
        esp_err_t err = score_load();
        if (err != ESP_OK) {
            score_retry(err, now_ms);
            return;
        }
    }
    if (s_dino.best <= s_dino.persisted && s_tilt.best <= s_tilt.persisted) return;

    nvs_handle_t handle;
    esp_err_t err = nvs_open(GAME_SCORE_NVS_NS, NVS_READWRITE, &handle);
    if (err == ESP_OK) {
        if (s_dino.best > s_dino.persisted) {
            err = nvs_set_u32(handle, s_dino.key, s_dino.best);
        }
        if (err == ESP_OK && s_tilt.best > s_tilt.persisted) {
            err = nvs_set_u32(handle, s_tilt.key, s_tilt.best);
        }
        if (err == ESP_OK) err = nvs_commit(handle);
        nvs_close(handle);
    }
    if (err != ESP_OK) {
        score_retry(err, now_ms);
        return;
    }
    s_dino.persisted = s_dino.best;
    s_tilt.persisted = s_tilt.best;
}
