#include "voice_asr.h"
#include "voice_asr_config.h"
#include "esp_http_client.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static int qwen_calls, proxy_calls, cleaned, status = 200;
static bool complete = true, qwen_ok = true, read_error;
static size_t offset, written;
static int payload_length;
static const uint8_t wav[] = "RIFF-test-audio";
static const char *response = "{\"text\":\"准备答辩25分钟\",\"provider\":\"aispeech\"}";

esp_http_client_handle_t esp_http_client_init(const esp_http_client_config_t *cfg)
{
    assert(strcmp(cfg->url, "https://proxy.test/v1/asr/transcribe") == 0);
    assert(cfg->timeout_ms == 45000);
    proxy_calls++;
    offset = written = 0;
    return &proxy_calls;
}
int esp_http_client_set_header(esp_http_client_handle_t client, const char *key, const char *value)
{
    (void)client;
    if (strcmp(key, "Authorization") == 0) assert(strcmp(value, "Bearer test-token") == 0);
    else { assert(strcmp(key, "Content-Type") == 0); assert(strcmp(value, "audio/wav") == 0); }
    return 0;
}
int esp_http_client_open(esp_http_client_handle_t client, int length)
{ (void)client; payload_length = length; return 0; }
int esp_http_client_write(esp_http_client_handle_t client, const char *data, int length)
{
    (void)client;
    int n = length < 3 ? length : 3; /* Exercise partial writes. */
    assert(memcmp(data, wav + written, (size_t)n) == 0);
    written += (size_t)n;
    return n;
}
int64_t esp_http_client_fetch_headers(esp_http_client_handle_t client)
{ (void)client; assert(written == (size_t)payload_length); return 0; /* chunked response */ }
int esp_http_client_get_status_code(esp_http_client_handle_t client) { (void)client; return status; }
int esp_http_client_read(esp_http_client_handle_t client, char *data, int length)
{
    (void)client;
    if (read_error) return -1;
    size_t n = strlen(response) - offset;
    if (n > 3) n = 3; /* Exercise split UTF-8 bytes and JSON tokens. */
    if (n > (size_t)length) n = (size_t)length;
    memcpy(data, response + offset, n);
    offset += n;
    return (int)n;
}
bool esp_http_client_is_complete_data_received(esp_http_client_handle_t client)
{ (void)client; return complete; }
int esp_http_client_close(esp_http_client_handle_t client) { (void)client; return 0; }
int esp_http_client_cleanup(esp_http_client_handle_t client) { (void)client; cleaned++; return 0; }

static bool qwen(const uint8_t *audio, size_t length, char *out, size_t cap)
{
    assert(audio == wav && length == sizeof(wav)); /* Must reuse the original recording. */
    qwen_calls++;
    snprintf(out, cap, "%s", qwen_ok ? "qwen-result" : "partial-failed-result");
    return qwen_ok;
}

static void failed_primary(void)
{
    char out[128] = "stale";
    int before = qwen_calls;
    bool ok = voice_asr_recognize(wav, sizeof(wav), out, sizeof(out), qwen);
    if (VOICE_ASR_ALLOW_QWEN_FALLBACK) {
        assert(ok && strcmp(out, "qwen-result") == 0);
        assert(qwen_calls == before + 1);
    } else {
        assert(!ok && !out[0] && qwen_calls == before);
    }
    assert(cleaned == proxy_calls);
}

int main(void)
{
    char out[128] = "stale";
    assert(voice_asr_recognize(wav, sizeof(wav), out, sizeof(out), qwen));
    if (VOICE_ASR_PROVIDER == VOICE_ASR_QWEN) {
        assert(strcmp(out, "qwen-result") == 0 && proxy_calls == 0 && qwen_calls == 1);
    } else {
        assert(strcmp(out, "准备答辩25分钟") == 0 && qwen_calls == 0 && proxy_calls == 1);
        assert(cleaned == proxy_calls);
        const char *bad[] = {"not-json", "{\"text\":\"\",\"provider\":\"aispeech\"}",
                             "{\"text\":\"wrong\",\"provider\":\"qwen\"}", "{\"error\":\"failed\"}"};
        for (size_t i = 0; i < sizeof(bad) / sizeof(bad[0]); i++) { response = bad[i]; failed_primary(); }
        response = "{\"text\":\"ok\",\"provider\":\"aispeech\"}";
        status = 401; failed_primary(); status = 200;
        complete = false; failed_primary(); complete = true;
        read_error = true; failed_primary(); read_error = false;
    }
    qwen_ok = false;
    status = 502;
    assert(!voice_asr_recognize(wav, sizeof(wav), out, sizeof(out), qwen) && !out[0]);
    strcpy(out, "stale");
    assert(!voice_asr_recognize(NULL, 0, out, sizeof(out), qwen) && !out[0]);
    assert(!voice_asr_recognize(wav, sizeof(wav), NULL, 0, qwen));
    assert(cleaned == proxy_calls);
    puts("ASR routing, failure handling and resource cleanup passed");
    return 0;
}
