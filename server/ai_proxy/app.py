#!/usr/bin/env python3
"""Reppo AI proxy: ESP32 -> ECS -> DeepSeek. DeepSeek API key stays on server only."""

import os
import httpx
from flask import Flask, request, jsonify


def _load_dotenv(path=".env"):
    """简单读取 .env（不依赖 python-dotenv）"""
    if not os.path.isfile(path):
        return
    with open(path, encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#") or "=" not in line:
                continue
            key, val = line.split("=", 1)
            key = key.strip()
            val = val.strip().strip('"').strip("'")
            if key and key not in os.environ:
                os.environ[key] = val


_load_dotenv(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".env"))

app = Flask(__name__)

DEEPSEEK_URL = os.environ.get(
    "DEEPSEEK_URL", "https://api.deepseek.com/chat/completions"
)
DEEPSEEK_API_KEY = os.environ.get("DEEPSEEK_API_KEY", "")
DEVICE_TOKEN = os.environ.get("DEVICE_TOKEN", "reppo-dev-token-change-me")
DEEPSEEK_MODEL = os.environ.get("DEEPSEEK_MODEL", "deepseek-v4-pro")
MAX_TOKENS = int(os.environ.get("MAX_TOKENS", "800"))


def _auth_ok() -> bool:
    auth = request.headers.get("Authorization", "")
    return auth == f"Bearer {DEVICE_TOKEN}"


@app.get("/health")
def health():
    return jsonify({"ok": True})


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
