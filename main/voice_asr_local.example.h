#pragma once

/* Vendor productId/apiKey belong in server/ai_proxy/.env, NEVER here. */
#define VOICE_ASR_PROVIDER VOICE_ASR_AISPEECH
/* First verify AISpeech with 0; use 1 later to enable the existing Qwen backup. */
#define VOICE_ASR_ALLOW_QWEN_FALLBACK 0
#define VOICE_ASR_PROXY_URL "https://YOUR_API_DOMAIN/v1/asr/transcribe"
#define VOICE_ASR_DEVICE_TOKEN "REPLACE_WITH_YOUR_SERVER_DEVICE_TOKEN"
