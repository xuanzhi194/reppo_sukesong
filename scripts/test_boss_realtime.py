"""Opt-in cloud smoke test; sends only a supplied WAV, never records the mic.
DASHSCOPE_API_KEY env preferred. --use-firmware-key reads the existing key without printing it.
Requires websocket-client. WAV must be mono 16-bit 16kHz.
"""
import argparse
import base64
import json
import os
from pathlib import Path
import re
import threading
import time
import wave
import websocket
import ssl
import certifi

p = argparse.ArgumentParser()
p.add_argument('wav', type=Path)
p.add_argument('--use-firmware-key', action='store_true')
p.add_argument('--threshold', type=float, default=0.2)
p.add_argument('--silence-ms', type=int, default=1100)
p.add_argument('--language', choices=['zh','auto'], default='zh')
p.add_argument('--show-partials', action='store_true')
a = p.parse_args()
key = os.environ.get('DASHSCOPE_API_KEY', '')
if not key and a.use_firmware_key:
    source = (Path(__file__).resolve().parents[1] / 'main/voice_todo.c').read_text()
    match = re.search(r'DASHSCOPE_KEY\s*=\s*"([^"]+)"', source)
    if match:
        key = match.group(1)
if not key:
    raise SystemExit('Set DASHSCOPE_API_KEY or explicitly opt in to the existing firmware key')
with wave.open(str(a.wav)) as f:
    assert (f.getframerate(), f.getnchannels(), f.getsampwidth()) == (16000, 1, 2)
    pcm = f.readframes(f.getnframes()) + bytes(48000)
try:
    ws = websocket.create_connection('wss://dashscope.aliyuncs.com/api-ws/v1/realtime?model=qwen3-asr-flash-realtime',
                                     header={'Authorization': 'Bearer ' + key}, timeout=15, sslopt={'ca_certs': certifi.where(), 'cert_reqs': ssl.CERT_REQUIRED})
    ws.send(json.dumps({'event_id': 'config', 'type': 'session.update', 'session': {
        'input_audio_format': 'pcm', 'sample_rate': 16000,
        'input_audio_transcription': {'language': 'zh'} if a.language == 'zh' else {},
        'turn_detection': {'type': 'server_vad', 'threshold': a.threshold, 'silence_duration_ms': a.silence_ms}}}))
    while True:
        event = json.loads(ws.recv())
        if event['type'] == 'error':
            raise RuntimeError('session rejected: ' + str(event.get('error', {}).get('code', 'unknown')))
        if event['type'] == 'session.updated':
            print('PASS WebSocket authentication and PCM16 session configuration', flush=True)
            break
    def send_audio():
        for i in range(0, len(pcm), 3200):
            ws.send(json.dumps({'event_id': 'a' + str(i), 'type': 'input_audio_buffer.append',
                                'audio': base64.b64encode(pcm[i:i+3200]).decode()}))
            time.sleep(.1)
        ws.send(json.dumps({'event_id': 'end', 'type': 'session.finish'}))
    sender = threading.Thread(target=send_audio)
    sender.start()
    partials, finals = 0, []
    deadline = time.monotonic() + 30
    while time.monotonic() < deadline:
        event = json.loads(ws.recv())
        if event['type'] == 'conversation.item.input_audio_transcription.text':
            partials += 1
            if a.show_partials:
                print('PARTIAL', event.get('text', '') + event.get('stash', ''), flush=True)
        elif event['type'] == 'conversation.item.input_audio_transcription.completed':
            finals.append(event.get('transcript', ''))
        elif event['type'] == 'error':
            raise RuntimeError('ASR error: ' + str(event.get('error', {}).get('code', 'unknown')))
        elif event['type'] == 'session.finished':
            break
    sender.join(timeout=5)
    ws.close()
    print('Partial events:', partials)
    print('Final transcripts:', json.dumps(finals, ensure_ascii=False))
    if not finals:
        raise RuntimeError('No final transcript received')
except Exception as e:
    # Never emit connection repr, headers, or raw exception payloads.
    print('FAIL', type(e).__name__)
    raise SystemExit(1)
