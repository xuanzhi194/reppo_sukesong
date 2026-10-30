#pragma once

#define VOICE_ASR_QWEN 0
#define VOICE_ASR_AISPEECH 1

/* Copy voice_asr_local.example.h to voice_asr_local.h to override these defaults. */
#if __has_include("voice_asr_local.h")
#include "voice_asr_local.h"
#endif

#ifndef VOICE_ASR_PROVIDER
#define VOICE_ASR_PROVIDER VOICE_ASR_QWEN
#endif
#ifndef VOICE_ASR_ALLOW_QWEN_FALLBACK
#define VOICE_ASR_ALLOW_QWEN_FALLBACK 1
#endif
#ifndef VOICE_ASR_PROXY_URL
#define VOICE_ASR_PROXY_URL ""
#endif
#ifndef VOICE_ASR_DEVICE_TOKEN
#define VOICE_ASR_DEVICE_TOKEN ""
#endif
#ifndef VOICE_ASR_PROXY_TIMEOUT_MS
#define VOICE_ASR_PROXY_TIMEOUT_MS 45000
#endif

#if VOICE_ASR_PROVIDER != VOICE_ASR_QWEN && VOICE_ASR_PROVIDER != VOICE_ASR_AISPEECH
#error "Invalid VOICE_ASR_PROVIDER"
#endif
