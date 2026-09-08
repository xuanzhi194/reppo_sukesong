/* Dino Run record persistence. No LVGL, worker task or generated UI dependency. */
#include "game_score_storage.h"
#include "esp_log.h"
#include "nvs.h"
#include <stdbool.h>

#define GAME_SCORE_NVS_NS "DinoGame"
#define GAME_SCORE_NVS_KEY "best"
#define GAME_SCORE_RETRY_MS 5000u

static const char *TAG = "DINO_SCORE";
static uint32_t s_best;
static uint32_t s_persisted;
static uint32_t s_last_failure;
static bool s_loaded;
static bool s_retry_pending;

static esp_err_t score_load(void)
{
    nvs_handle_t handle;
    uint32_t stored = 0;
    esp_err_t err = nvs_open(GAME_SCORE_NVS_NS, NVS_READONLY, &handle);
    if (err == ESP_OK) {
        err = nvs_get_u32(handle, GAME_SCORE_NVS_KEY, &stored);
        nvs_close(handle);
    }
    if (err == ESP_ERR_NVS_NOT_FOUND) stored = 0;
    else if (err != ESP_OK) return err;

    s_persisted = stored;
    if (stored > s_best) s_best = stored;
    s_loaded = true;
    return ESP_OK;
}

static void score_retry(esp_err_t err, uint32_t now_ms)
{
    s_retry_pending = true;
    s_last_failure = now_ms;
    ESP_LOGW(TAG, "Record NVS error: %s; will retry", esp_err_to_name(err));
}

void game_score_storage_init(void)
{
    s_best = s_persisted = s_last_failure = 0;
    s_loaded = s_retry_pending = false;
    esp_err_t err = score_load();
    if (err != ESP_OK) score_retry(err, 0);
}

uint32_t game_score_storage_best(void)
{
    return s_best;
}

void game_score_storage_offer(uint32_t best)
{
    if (best > s_best) s_best = best;
}

void game_score_storage_poll(uint32_t now_ms)
{
    if (s_retry_pending && now_ms - s_last_failure < GAME_SCORE_RETRY_MS) return;
    s_retry_pending = false;
    if (!s_loaded) {
        /* A read error must never let a lower score overwrite an existing record. */
        esp_err_t err = score_load();
        if (err != ESP_OK) {
            score_retry(err, now_ms);
            return;
        }
    }
    if (s_best <= s_persisted) return;

    nvs_handle_t handle;
    esp_err_t err = nvs_open(GAME_SCORE_NVS_NS, NVS_READWRITE, &handle);
    if (err == ESP_OK) {
        err = nvs_set_u32(handle, GAME_SCORE_NVS_KEY, s_best);
        if (err == ESP_OK) err = nvs_commit(handle);
        nvs_close(handle);
    }
    if (err != ESP_OK) {
        score_retry(err, now_ms);
        return;
    }
    /* Only a successful commit acknowledges the pending record. */
    s_persisted = s_best;
}
