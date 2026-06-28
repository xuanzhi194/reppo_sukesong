# Copy ESP-IDF build artifacts into web-flash/firmware for browser flashing.
# Run from project root:  .\web-flash\scripts\prepare-firmware.ps1

$ErrorActionPreference = "Stop"
$WebFlash = Split-Path $PSScriptRoot -Parent
$Root = Split-Path $WebFlash -Parent
$Build = Join-Path $Root "build"
$Dest = Join-Path $WebFlash "firmware"

$map = @{
    "bootloader.bin"         = "bootloader\bootloader.bin"
    "partition-table.bin"    = "partition_table\partition-table.bin"
    "project-name.bin"       = "project-name.bin"
    "storage.bin"            = "storage.bin"
}

if (-not (Test-Path $Build)) {
    Write-Error "Build folder not found. Run 'idf.py build' first."
}

New-Item -ItemType Directory -Force -Path $Dest | Out-Null

foreach ($entry in $map.GetEnumerator()) {
    $src = Join-Path $Build $entry.Value
    $dst = Join-Path $Dest $entry.Key
    if (-not (Test-Path $src)) {
        Write-Warning "Skip (missing): $($entry.Value)"
        continue
    }
    Copy-Item -Force $src $dst
    Write-Host "OK $($entry.Key)"
}

Write-Host ""
Write-Host "Firmware ready in: $Dest"
Write-Host "Serve web-flash over HTTPS or localhost, then open index.html"
