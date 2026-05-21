#!/usr/bin/env bash
# Copy ESP-IDF build artifacts into web-flash/firmware
# Usage: ./web-flash/scripts/prepare-firmware.sh  (from project root)

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
BUILD="$ROOT/build"
DEST="$ROOT/web-flash/firmware"

declare -A MAP=(
  [bootloader.bin]=bootloader/bootloader.bin
  [partition-table.bin]=partition_table/partition-table.bin
  [ota_data_initial.bin]=ota_data_initial.bin
  [project-name.bin]=project-name.bin
  [storage.bin]=storage.bin
)

if [[ ! -d "$BUILD" ]]; then
  echo "error: run 'idf.py build' first" >&2
  exit 1
fi

mkdir -p "$DEST"

for name in "${!MAP[@]}"; do
  src="$BUILD/${MAP[$name]}"
  if [[ -f "$src" ]]; then
    cp -f "$src" "$DEST/$name"
    echo "OK $name"
  else
    echo "skip (missing): ${MAP[$name]}" >&2
  fi
done

echo ""
echo "Firmware ready: $DEST"
