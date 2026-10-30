#!/usr/bin/env python3
"""Reppo proxy for DeepSeek summaries and AISpeech sentence recognition."""

import os
import httpx
from flask import Flask, request, jsonify
from aispeech_asr import ASRError, MAX_AUDIO_BYTES, transcribe


def _clean_env(val):
    if val is None:
        return ""
    return val.strip().strip("\r\n").strip('"').strip("'")


def _load_dotenv(path=".env"):
    """简单读取 .env（不依赖 python-dotenv）"""
    if not os.path.isfile(path):
        return
    with open(path, encoding="utf-8") as f:
        for line in f:
            line = line.strip().strip("\r")
            if not line or line.startswith("#") or "=" not in line:
                continue
            key, val = line.split("=", 1)
            key = _clean_env(key).lstrip("\ufeff")
            val = _clean_env(val)
            if key and key not in os.environ:
                os.environ[key] = val


_load_dotenv(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".env"))

app = Flask(__name__)

DEEPSEEK_URL = os.environ.get(
    "DEEPSEEK_URL", "https://api.deepseek.com/chat/completions"
)
DEEPSEEK_API_KEY = _clean_env(os.environ.get("DEEPSEEK_API_KEY", ""))
DEVICE_TOKEN = _clean_env(os.environ.get("DEVICE_TOKEN", "reppo-dev-token-change-me"))
DEEPSEEK_MODEL = _clean_env(os.environ.get("DEEPSEEK_MODEL", "deepseek-v4-flash")) or "deepseek-v4-flash"
MAX_TOKENS = int(os.environ.get("MAX_TOKENS", "800"))
AISPEECH_PRODUCT_ID = _clean_env(os.environ.get("AISPEECH_PRODUCT_ID", ""))
AISPEECH_API_KEY = _clean_env(os.environ.get("AISPEECH_API_KEY", ""))
AISPEECH_LANG = _clean_env(os.environ.get("AISPEECH_LANG", "ce")) or "ce"


def _auth_ok() -> bool:
    auth = (request.headers.get("Authorization") or "").strip()
    if not auth.lower().startswith("bearer "):
        return False
    token = auth[7:].strip()
    return token == DEVICE_TOKEN


@app.get("/health")
def health():
    return jsonify({
        "ok": True,
        "device_token_len": len(DEVICE_TOKEN),
        "deepseek_key_set": bool(DEEPSEEK_API_KEY),
        "aispeech_configured": bool(AISPEECH_PRODUCT_ID and AISPEECH_API_KEY),
    })


@app.post("/v1/asr/transcribe")
def asr_transcribe():
    if not _auth_ok():
        return jsonify({"error": "unauthorized"}), 401
    if request.mimetype not in ("audio/wav", "audio/x-wav", "application/octet-stream"):
        return jsonify({"error": "send_raw_wav_with_content_type_audio_wav"}), 415
    if request.content_length is not None and request.content_length > MAX_AUDIO_BYTES:
        return jsonify({"error": "audio_too_large"}), 413
    audio = request.stream.read(MAX_AUDIO_BYTES + 1)
    if len(audio) > MAX_AUDIO_BYTES:
        return jsonify({"error": "audio_too_large"}), 413
    try:
        return jsonify(transcribe(audio, AISPEECH_PRODUCT_ID, AISPEECH_API_KEY, AISPEECH_LANG))
    except ASRError as error:
        payload = {"error": error.code, "provider": "aispeech"}
        if error.upstream_status is not None:
            payload["upstream_status"] = error.upstream_status
        return jsonify(payload), error.status


@app.post("/v1/ai/summary")
def ai_summary():
    if not _auth_ok():
        return jsonify({"error": "unauthorized"}), 401
    if not DEEPSEEK_API_KEY:
        return jsonify({"error": "server missing DEEPSEEK_API_KEY"}), 500

    body = request.get_json(silent=True) or {}
    prompt = body.get("prompt", "")
    model = body.get("model") or DEEPSEEK_MODEL
    if not prompt:
        return jsonify({"error": "prompt required"}), 400

    payload = {
        "model": model,
        "messages": [{"role": "user", "content": prompt}],
        "max_tokens": MAX_TOKENS,
        "stream": False,
        "thinking": {"type": "disabled"},
    }
    headers = {
        "Authorization": f"Bearer {DEEPSEEK_API_KEY}",
        "Content-Type": "application/json",
    }

    try:
        with httpx.Client(timeout=60.0) as client:
            r = client.post(DEEPSEEK_URL, json=payload, headers=headers)
    except httpx.HTTPError as e:
        return jsonify({"error": f"upstream request failed: {e}"}), 502

    if r.status_code != 200:
        return jsonify({"error": "upstream error", "detail": r.text[:500]}), 502

    data = r.json()
    content = ""
    try:
        msg = data["choices"][0]["message"]
        content = msg.get("content") or msg.get("reasoning_content") or ""
    except (KeyError, IndexError, TypeError):
        return jsonify({"error": "bad upstream response"}), 502

    if not content.strip():
        return jsonify({"error": "empty upstream content"}), 502

    return jsonify({"content": content})


if __name__ == "__main__":
    host = os.environ.get("HOST", "0.0.0.0")
    port = int(os.environ.get("PORT", "8080"))
    app.run(host=host, port=port)
