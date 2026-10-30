#!/usr/bin/env python3
"""Send one WAV to the reppo proxy; the proxy alone holds AISpeech credentials."""

import argparse
import json
import os
from pathlib import Path
import sys
import time
import urllib.error
import urllib.request


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("wav", type=Path, help="16kHz, 16bit, mono PCM WAV, up to 60s")
    parser.add_argument("--url", default="http://127.0.0.1:8080/v1/asr/transcribe")
    args = parser.parse_args()
    token = os.environ.get("DEVICE_TOKEN", "").strip()
    if not token:
        parser.error("Set DEVICE_TOKEN to match the proxy's .env (not AISPEECH_API_KEY).")
    if not args.wav.is_file():
        parser.error("WAV file does not exist")
    if args.wav.stat().st_size > 5 * 1024 * 1024:
        parser.error("WAV must be <= 5MB")
    req = urllib.request.Request(args.url, data=args.wav.read_bytes(), method="POST", headers={
        "Authorization": f"Bearer {token}", "Content-Type": "audio/wav",
    })
    started = time.monotonic()
    try:
        with urllib.request.urlopen(req, timeout=45) as response:
            body = json.load(response)
    except urllib.error.HTTPError as error:
        try:
            body = json.loads(error.read(8192))
        except (ValueError, UnicodeError):
            body = {"error": "non_json_response_check_proxy_or_nginx"}
        print(json.dumps({"http_status": error.code, "result": body}, ensure_ascii=False))
        return 1
    except (OSError, ValueError) as error:
        print(f"Request failed ({type(error).__name__}); check URL, connectivity and proxy service.", file=sys.stderr)
        return 1
    if not isinstance(body, dict) or body.get("provider") != "aispeech" or not body.get("text"):
        print("Response does not contain a successful AISpeech transcript.", file=sys.stderr)
        return 1
    print(json.dumps({"elapsed_ms": round((time.monotonic() - started) * 1000), **body}, ensure_ascii=False))
    return 0


if __name__ == "__main__":
    sys.exit(main())
