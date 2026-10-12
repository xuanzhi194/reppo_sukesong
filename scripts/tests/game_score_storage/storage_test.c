/* Production storage adapter against transactional, fault-injectable NVS.
 * NVS survives game_score_storage_init(); that call simulates a reboot of RAM. */
#include "game_score_storage.h"
#include "nvs.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static struct {
    bool namespace_exists;
    bool key_exists;
    uint32_t committed;
    bool open;
    nvs_open_mode_t mode;
    bool staged;
    uint32_t pending;
    unsigned opens, reads, sets, commits, closes;
    unsigned fail_open, fail_read, fail_set, fail_commit;
    esp_err_t read_error;
} flash;

const char *esp_err_to_name(esp_err_t error)
{
    (void)error;
    return "injected error";
}

void test_esp_log(const char *tag, const char *format, ...)
{
    (void)tag;
    (void)format;
}

esp_err_t nvs_open(const char *name, nvs_open_mode_t mode, nvs_handle_t *handle)
{
    assert(strcmp(name, "DinoGame") == 0);
    assert(!flash.open);
    ++flash.opens;
    if (flash.fail_open) {
        --flash.fail_open;
        return ESP_FAIL;
    }
    if (mode == NVS_READONLY && !flash.namespace_exists) return ESP_ERR_NVS_NOT_FOUND;
    assert(mode == NVS_READONLY || mode == NVS_READWRITE);
    flash.open = true;
    flash.mode = mode;
    if (mode == NVS_READWRITE) flash.namespace_exists = true;
    *handle = 1;
    return ESP_OK;
}

esp_err_t nvs_get_u32(nvs_handle_t handle, const char *key, uint32_t *value)
{
    assert(flash.open && handle == 1);
    if (strcmp(key, "tilt") == 0) {
        return ESP_ERR_NVS_NOT_FOUND;
    }
    assert(strcmp(key, "best") == 0);
    ++flash.reads;
    if (flash.fail_read) {
        --flash.fail_read;
        return flash.read_error;
    }
    if (!flash.key_exists) return ESP_ERR_NVS_NOT_FOUND;
    *value = flash.committed;
    return ESP_OK;
}

esp_err_t nvs_set_u32(nvs_handle_t handle, const char *key, uint32_t value)
{
    assert(flash.open && handle == 1 && flash.mode == NVS_READWRITE);
    assert(strcmp(key, "best") == 0);
    ++flash.sets;
    if (flash.fail_set) {
        --flash.fail_set;
        return ESP_ERR_NVS_NOT_ENOUGH_SPACE;
    }
    flash.staged = true;
    flash.pending = value;
    return ESP_OK;
}

esp_err_t nvs_commit(nvs_handle_t handle)
{
    assert(flash.open && handle == 1 && flash.mode == NVS_READWRITE);
    assert(flash.staged);
    ++flash.commits;
    if (flash.fail_commit) {
        --flash.fail_commit;
        return ESP_FAIL;
    }
    flash.committed = flash.pending;
    flash.key_exists = true;
    flash.staged = false;
    return ESP_OK;
}

void nvs_close(nvs_handle_t handle)
{
    assert(flash.open && handle == 1);
    ++flash.closes;
    flash.open = false;
    flash.staged = false;
}

static void seed(uint32_t value)
{
    memset(&flash, 0, sizeof(flash));
    flash.namespace_exists = true;
    flash.key_exists = true;
    flash.committed = value;
    flash.read_error = ESP_FAIL;
}

static void reboot_restore(void)
{
    seed(37);
    game_score_storage_init();
    assert(game_score_storage_best() == 37);
    game_score_storage_offer(1234);
    game_score_storage_poll(0);
    assert(flash.committed == 1234 && flash.commits == 1);
    game_score_storage_init();
    assert(game_score_storage_best() == 1234);
    game_score_storage_poll(30000);
    assert(flash.commits == 1);
}

static void lower_and_equal_do_not_write(void)
{
    seed(444);
    game_score_storage_init();
    game_score_storage_offer(0);
    game_score_storage_offer(443);
    game_score_storage_offer(444);
    game_score_storage_poll(0);
    game_score_storage_poll(100000);
    assert(game_score_storage_best() == 444);
    assert(flash.sets == 0 && flash.commits == 0);
}

static void offer_only_updates_ram(void)
{
    seed(11);
    game_score_storage_init();
    unsigned initial_opens = flash.opens;
    for (uint32_t i = 12; i <= 1200; ++i) game_score_storage_offer(i);
    game_score_storage_offer(40);
    assert(game_score_storage_best() == 1200);
    assert(flash.opens == initial_opens && flash.sets == 0 && flash.committed == 11);
    game_score_storage_poll(1);
    assert(flash.committed == 1200 && flash.commits == 1);
}

static void missing_namespace_is_zero(void)
{
    memset(&flash, 0, sizeof(flash));
    game_score_storage_init();
    assert(game_score_storage_best() == 0 && !flash.namespace_exists);
    game_score_storage_poll(0);
    assert(flash.sets == 0);
    game_score_storage_offer(9);
    game_score_storage_poll(0);
    assert(flash.committed == 9 && flash.commits == 1);
}

static void missing_key_is_zero(void)
{
    seed(0);
    flash.key_exists = false;
    game_score_storage_init();
    assert(game_score_storage_best() == 0);
    game_score_storage_offer(20);
    game_score_storage_poll(0);
    assert(flash.committed == 20 && flash.commits == 1);
}

static void write_failure(unsigned *counter)
{
    seed(100);
    game_score_storage_init();
    game_score_storage_offer(200);
    *counter = 1;
    game_score_storage_poll(100);
    assert(!flash.open && flash.committed == 100);
    assert(game_score_storage_best() == 200);
    unsigned attempted_opens = flash.opens;
    game_score_storage_poll(5099);
    assert(flash.opens == attempted_opens && flash.committed == 100);
    game_score_storage_offer(333);
    game_score_storage_poll(5100);
    assert(flash.committed == 333 && !flash.open);
    unsigned successful_commits = flash.commits;
    game_score_storage_poll(90000);
    assert(flash.commits == successful_commits);
    game_score_storage_init();
    assert(game_score_storage_best() == 333);
}

static void open_failure_retries(void) { write_failure(&flash.fail_open); }
static void set_failure_retries(void) { write_failure(&flash.fail_set); }
static void commit_failure_retries(void) { write_failure(&flash.fail_commit); }

static void initial_load_failure(bool fail_open, esp_err_t read_error)
{
    seed(900);
    flash.fail_open = fail_open ? 1 : 0;
    flash.fail_read = fail_open ? 0 : 1;
    flash.read_error = read_error;
    game_score_storage_init();
    assert(!flash.open);
    game_score_storage_offer(100);
    unsigned initial_opens = flash.opens;
    game_score_storage_poll(0);
    game_score_storage_poll(4999);
    assert(flash.opens == initial_opens && flash.sets == 0 && flash.committed == 900);
    game_score_storage_poll(5000);
    assert(game_score_storage_best() == 900 && flash.sets == 0);
    game_score_storage_offer(999);
    game_score_storage_poll(5001);
    assert(flash.committed == 999 && flash.commits == 1);
}

static void initial_open_failure_preserves_record(void) { initial_load_failure(true, ESP_FAIL); }
static void initial_read_failure_preserves_record(void) { initial_load_failure(false, ESP_FAIL); }
static void type_mismatch_preserves_record(void) { initial_load_failure(false, ESP_ERR_NVS_TYPE_MISMATCH); }

static void repeated_load_failure_never_writes(void)
{
    seed(5000);
    flash.fail_read = 3;
    game_score_storage_init();
    game_score_storage_offer(7000);
    game_score_storage_poll(5000);
    game_score_storage_poll(10000);
    assert(flash.sets == 0 && flash.committed == 5000 && !flash.open);
    game_score_storage_poll(14999);
    assert(flash.reads == 3);
    game_score_storage_poll(15000);
    /* A successful read may flush the pending value in this or the next poll. */
    game_score_storage_poll(15001);
    assert(flash.committed == 7000 && flash.commits == 1);
}

static void tick_wrap_keeps_backoff(void)
{
    seed(10);
    game_score_storage_init();
    game_score_storage_offer(99);
    flash.fail_commit = 1;
    uint32_t failure_tick = UINT32_MAX - 2000;
    game_score_storage_poll(failure_tick);
    unsigned attempted_opens = flash.opens;
    game_score_storage_poll(failure_tick + 4999U);
    assert(flash.opens == attempted_opens && flash.committed == 10);
    game_score_storage_poll(failure_tick + 5000U);
    assert(flash.committed == 99 && flash.commits == 2);
}

static void full_u32_score_roundtrips(void)
{
    seed(1);
    game_score_storage_init();
    game_score_storage_offer(UINT32_MAX);
    game_score_storage_poll(0);
    game_score_storage_init();
    assert(game_score_storage_best() == UINT32_MAX);
    game_score_storage_offer(10);
    game_score_storage_poll(50000);
    assert(flash.committed == UINT32_MAX && flash.commits == 1);
}

int main(int argc, char **argv)
{
    static const struct { const char *name; void (*run)(void); } tests[] = {
        {"reboot_restore", reboot_restore},
        {"lower_and_equal_do_not_write", lower_and_equal_do_not_write},
        {"offer_only_updates_ram", offer_only_updates_ram},
        {"missing_namespace_is_zero", missing_namespace_is_zero},
        {"missing_key_is_zero", missing_key_is_zero},
        {"open_failure_retries", open_failure_retries},
        {"set_failure_retries", set_failure_retries},
        {"commit_failure_retries", commit_failure_retries},
        {"initial_open_failure_preserves_record", initial_open_failure_preserves_record},
        {"initial_read_failure_preserves_record", initial_read_failure_preserves_record},
        {"type_mismatch_preserves_record", type_mismatch_preserves_record},
        {"repeated_load_failure_never_writes", repeated_load_failure_never_writes},
        {"tick_wrap_keeps_backoff", tick_wrap_keeps_backoff},
        {"full_u32_score_roundtrips", full_u32_score_roundtrips},
    };
    assert(argc == 2);
    for (unsigned i = 0; i < sizeof(tests) / sizeof(tests[0]); ++i) {
        if (strcmp(argv[1], tests[i].name) == 0) {
            tests[i].run();
            assert(!flash.open);
            printf("%s: PASS\n", tests[i].name);
            return 0;
        }
    }
    fprintf(stderr, "unknown test: %s\n", argv[1]);
    return 1;
}
