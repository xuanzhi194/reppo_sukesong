#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef bool (*voice_asr_backend_t)(const uint8_t *wav, size_t wav_len, char *out, size_t cap);

/* Select primary backend; optionally retry the SAME recording with the existing Qwen backend. */
bool voice_asr_recognize(const uint8_t *wav, size_t wav_len, char *out, size_t cap,
                         voice_asr_backend_t qwen_backend);
