#!/usr/bin/env bash
# Copy app firmware (factory partition) into web-flash/firmware for browser update.
# Usage: ./web-flash/scripts/prepare-firmware.sh  (from project root)

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
BUILD="$ROOT/build"
DEST="$ROOT/web-flash/firmware"

if [[ ! -d "$BUILD" ]]; then
  echo "error: run 'idf.py build' first" >&2
  exit 1
fi

mkdir -p "$DEST"

src="$BUILD/project-name.bin"
if [[ -f "$src" ]]; then
  cp -f "$src" "$DEST/project-name.bin"
  echo "OK project-name.bin -> $DEST"
else
  echo "error: missing $src" >&2
  exit 1
fi

echo ""
echo "App-only firmware ready: $DEST/project-name.bin (flash @ 0x10000)"
