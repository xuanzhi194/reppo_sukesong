#!/usr/bin/env python3
"""Test AISpeech directly from this computer. No proxy/web server is started."""

import argparse
import json
import os
from pathlib import Path
import queue
import sys
import threading
import time
import urllib.parse
import urllib.request
import uuid

import certifi
import websocket

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "server/ai_proxy"))
from aispeech_asr import ASRError, transcribe, validate_wav


def load_config(path):
    config = {}
    if path.is_file():
        for line in path.read_text(encoding="utf-8").splitlines():
            if line.strip() and not line.lstrip().startswith("#") and "=" in line:
                key, value = line.split("=", 1)
                config[key.strip()] = value.strip().strip("\"'")
    for key in ("AISPEECH_PRODUCT_ID", "AISPEECH_API_KEY", "AISPEECH_SERVICE"):
        if key in os.environ:
            config[key] = os.environ[key].strip()
    return config


def system_proxy_options():
    """websocket-client only reads env proxies; also honor this Mac's system HTTP proxy."""
    if urllib.request.proxy_bypass("lasr.duiopen.com"):
        return {}
    proxies = urllib.request.getproxies()
    address = proxies.get("https") or proxies.get("http")
    if not address:
        return {}
    proxy = urllib.parse.urlparse(address)
    if proxy.scheme not in ("http", "https") or not proxy.hostname:
        return {}
    options = {"http_proxy_host": proxy.hostname, "http_proxy_port": proxy.port or 80,
               "proxy_type": "http"}
    if proxy.username:
        options["http_proxy_auth"] = (urllib.parse.unquote(proxy.username),
                                       urllib.parse.unquote(proxy.password or ""))
    return options


def realtime(audio, product_id, api_key):
    validate_wav(audio)
    query = urllib.parse.urlencode({"productId": product_id, "apikey": api_key,
                                    "res": "aitranson", "lang": "cn", "device_id": uuid.uuid4().hex})
    ws = None
    receiver = None
    done = threading.Event()
    errors = queue.Queue()
    segments = {}
    codes = []

    def receive():
        try:
            while not done.is_set():
                raw = ws.recv()
                if not raw:
                    raise ASRError("realtime_closed_before_final")
                event = json.loads(raw)
                code = event.get("errno")
                codes.append(code)
                if code not in (0, 7, 8, 9):
                    raise ASRError("realtime_vendor_error_" + str(code))
                data = event.get("data") or {}
                if code in (0, 9) and isinstance(data.get("onebest"), str):
                    # rec messages are committed segments; var (8) must not be concatenated.
                    bg = data.get("bg", 0)
                    segments[bg] = data["onebest"]
                if code == 9:
                    done.set()
        except Exception as error:
            errors.put(error)
            done.set()

    try:
        ws = websocket.create_connection(
            "wss://lasr.duiopen.com/live/ws2?" + query, timeout=15,
            sslopt={"ca_certs": certifi.where()}, enable_multithread=True,
            **system_proxy_options(),
        )
        ws.send(json.dumps({"command": "start", "params": {
            "env": {"use_tprocess": 1},
            "audio": {"audioType": "wav", "sampleRate": 16000, "sampleBytes": 2, "channel": 1},
        }}))
        ack = json.loads(ws.recv())
        codes.append(ack.get("errno"))
        if ack.get("errno") != 7:
            raise ASRError("realtime_start_not_acknowledged")
        receiver = threading.Thread(target=receive, daemon=True)
        receiver.start()
        started = time.monotonic()
        # Feed WAV bytes in 40ms chunks; retain the WAV header expected by audioType=wav.
        chunk_bytes = 16000 * 2 * 40 // 1000
        for index, offset in enumerate(range(0, len(audio), chunk_bytes)):
            if done.is_set():
                if not errors.empty():
                    raise errors.get()
                raise ASRError("realtime_ended_during_upload")
            wait = started + index * 0.04 - time.monotonic()
            if wait > 0:
                time.sleep(wait)
            ws.send_binary(audio[offset:offset + chunk_bytes])
        ws.send_binary(b"")
        if not done.wait(20):
            raise ASRError("realtime_final_timeout", 504)
        if not errors.empty():
            raise errors.get()
        text = "".join(segments[key] for key in sorted(segments)).strip()
        if not text:
            raise ASRError("no_speech", 422)
        return {"provider": "aispeech", "service": "realtime", "text": text, "event_codes": codes}
    finally:
        done.set()
        if ws is not None:
            ws.close()
        if receiver is not None:
            receiver.join(timeout=2)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("wav", type=Path)
    parser.add_argument("--env", type=Path, default=ROOT / ".secrets/aispeech.env")
    parser.add_argument("--service", choices=("realtime", "sentence"))
    args = parser.parse_args()
    config = load_config(args.env)
    product_id = config.get("AISPEECH_PRODUCT_ID", "")
    api_key = config.get("AISPEECH_API_KEY", "")
    service = args.service or config.get("AISPEECH_SERVICE", "realtime")
    if not product_id or not api_key:
        parser.error("Configure AISPEECH_PRODUCT_ID/AISPEECH_API_KEY in .secrets/aispeech.env")
    if service not in ("realtime", "sentence"):
        parser.error("AISPEECH_SERVICE must be realtime or sentence")
    started = time.monotonic()
    try:
        if args.wav.stat().st_size > 5 * 1024 * 1024:
            raise ASRError("audio_too_large", 400)
        audio = args.wav.read_bytes()
        result = realtime(audio, product_id, api_key) if service == "realtime" else {
            **transcribe(audio, product_id, api_key), "service": "sentence"}
    except Exception as error:
        # Handshake/HTTP errors can embed a credential-bearing URL. Never print exception text.
        code = error.code if isinstance(error, ASRError) else type(error).__name__
        status = getattr(error, "status_code", None) or getattr(error, "upstream_status", None)
        print(json.dumps({"ok": False, "error": code, "upstream_status": status}))
        return 1
    print(json.dumps({"ok": True, "product_id": product_id,
                      "elapsed_ms": round((time.monotonic() - started) * 1000), **result},
                     ensure_ascii=False))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
