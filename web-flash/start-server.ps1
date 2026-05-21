# Reppo 烧录页本地服务器（PowerShell）
$ErrorActionPreference = "Stop"
Set-Location $PSScriptRoot

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Reppo 固件烧录 - 本地服务器" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "请勿关闭本窗口。浏览器将打开 http://localhost:8080" -ForegroundColor Yellow
Write-Host ""

if (-not (Get-Command python -ErrorAction SilentlyContinue)) {
    Write-Host "[错误] 未找到 python，请安装 Python 3。" -ForegroundColor Red
    Read-Host "按 Enter 退出"
    exit 1
}

Start-Sleep -Seconds 1
Start-Process "http://localhost:8080/"

python -m http.server 8080
