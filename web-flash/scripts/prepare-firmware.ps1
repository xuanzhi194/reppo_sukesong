# Copy app firmware (factory partition) into web-flash/firmware for browser update.
# Run from project root:  .\web-flash\scripts\prepare-firmware.ps1

$ErrorActionPreference = "Stop"
$WebFlash = Split-Path $PSScriptRoot -Parent
$Root = Split-Path $WebFlash -Parent
$Build = Join-Path $Root "build"
$Dest = Join-Path $WebFlash "firmware"

if (-not (Test-Path $Build)) {
    Write-Error "Build folder not found. Run 'idf.py build' first."
}

New-Item -ItemType Directory -Force -Path $Dest | Out-Null

$src = Join-Path $Build "project-name.bin"
$dst = Join-Path $Dest "project-name.bin"

if (-not (Test-Path $src)) {
    Write-Error "Missing $src"
}

Copy-Item -Force $src $dst
Write-Host "OK project-name.bin -> $Dest"
Write-Host ""
Write-Host "App-only firmware ready (flash @ 0x10000)"
