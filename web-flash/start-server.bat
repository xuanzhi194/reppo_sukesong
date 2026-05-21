@echo off
chcp 65001 >nul
cd /d "%~dp0"

echo ========================================
echo   Reppo 固件烧录 - 本地服务器
echo ========================================
echo.
echo 正在启动，请勿关闭本窗口（关闭后网页会打不开）
echo 浏览器将自动打开: http://localhost:8080
echo.

where python >nul 2>&1
if errorlevel 1 (
    echo [错误] 未找到 python，请先安装 Python 3 或使用 ESP-IDF 终端。
    pause
    exit /b 1
)

timeout /t 1 /nobreak >nul
start "" "http://localhost:8080/"

python -m http.server 8080
if errorlevel 1 (
    echo.
    echo [错误] 8080 端口可能被占用。请关闭占用程序后重试。
    pause
    exit /b 1
)

pause
