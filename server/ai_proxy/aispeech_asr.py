"""AISpeech sentence ASR. Vendor credentials never leave this server."""

import io
import json
import uuid
import wave

import httpx

ENDPOINT = "https://lasr.duiopen.com/lasr-sentence-api/v2/sentence"
MAX_AUDIO_BYTES = 5 * 1024 * 1024
LANGUAGES = {"cn", "en", "ce", "sichuantone-mix", "cantonese-mix"}


class ASRError(Exception):
    def __init__(self, code, status=502, upstream_status=None):
        super().__init__(code)
        self.code = code
        self.status = status
        self.upstream_status = upstream_status


def validate_wav(audio):
    if not audio or len(audio) > MAX_AUDIO_BYTES:
        raise ASRError("audio_empty_or_too_large", 400)
    try:
        with wave.open(io.BytesIO(audio), "rb") as wav:
            if (wav.getframerate(), wav.getsampwidth(), wav.getnchannels(), wav.getcomptype()) != (
                16000, 2, 1, "NONE"
            ):
                raise ASRError("expected_16khz_16bit_mono_pcm_wav", 400)
            frames = wav.getnframes()
            if not 0 < frames <= 60 * 16000:
                raise ASRError("audio_duration_must_be_0_to_60_seconds", 400)
            if len(wav.readframes(frames)) != frames * 2:
                raise ASRError("truncated_wav", 400)
    except (wave.Error, EOFError, OSError, ValueError):
        raise ASRError("invalid_wav", 400) from None


def transcribe(audio, product_id, api_key, lang="ce"):
    if not product_id or not api_key:
        raise ASRError("aispeech_not_configured", 503)
    if lang not in LANGUAGES:
        raise ASRError("invalid_aispeech_lang", 503)
    validate_wav(audio)
    params = {
        "request_id": str(uuid.uuid4()),
        "audio": {"audio_type": "wav", "sample_rate": 16000, "channel": 1, "sample_bytes": 2},
        "asr": {"lang": lang, "use_vad": True, "use_post": True, "use_itn": True},
    }
    try:
        with httpx.Client(timeout=httpx.Timeout(20.0, connect=5.0)) as client:
            response = client.post(
                ENDPOINT,
                params={"productId": product_id, "apiKey": api_key},
                data={"params": json.dumps(params)},
                files={"file": ("recording.wav", audio, "audio/wav")},
            )
    except httpx.TimeoutException:
        raise ASRError("aispeech_timeout", 504) from None
    except httpx.HTTPError:
        # Exception URLs can contain apiKey. Never return or log the exception.
        raise ASRError("aispeech_network_error") from None
    if response.status_code != 200:
        code = {401: "aispeech_auth_failed", 403: "aispeech_quota_or_rate_limit"}.get(
            response.status_code, "aispeech_http_error"
        )
        raise ASRError(code, upstream_status=response.status_code)
    try:
        body = response.json()
    except ValueError:
        raise ASRError("aispeech_invalid_json") from None
    if not isinstance(body, dict) or type(body.get("errno")) is not int:
        raise ASRError("aispeech_bad_response")
    if body["errno"] != 0:
        raise ASRError("aispeech_recognition_failed")
    data = body.get("data")
    results = data.get("result") if isinstance(data, dict) else None
    if not isinstance(results, list) or any(
        not isinstance(item, dict) or not isinstance(item.get("onebest"), str) for item in results
    ):
        raise ASRError("aispeech_bad_response")
    text = "".join(item["onebest"] for item in results).strip()
    if not text:
        raise ASRError("no_speech", 422)
    # Bound the response for the ESP32's fixed JSON receive buffer, including escaped Unicode.
    if len(text) > 1000:
        raise ASRError("aispeech_transcript_too_long")
    return {"text": text, "provider": "aispeech"}
