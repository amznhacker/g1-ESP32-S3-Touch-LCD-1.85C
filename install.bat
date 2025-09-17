@echo off
echo ESP32-S3 Face - One-Click Installer
echo ===================================

REM Check if running as admin (not recommended)
net session >nul 2>&1
if %errorlevel% equ 0 (
    echo ❌ Don't run as Administrator! Run as regular user.
    pause
    exit /b 1
)

REM Check Docker
docker --version >nul 2>&1
if %errorlevel% neq 0 (
    echo 🐳 Docker not found!
    echo Please install Docker Desktop:
    echo https://desktop.docker.com/win/main/amd64/Docker%20Desktop%20Installer.exe
    echo.
    echo After installation, restart and run this script again.
    pause
    exit /b 1
)

REM Check if Docker is running
docker info >nul 2>&1
if %errorlevel% neq 0 (
    echo 🐳 Starting Docker Desktop...
    start "" "C:\Program Files\Docker\Docker\Docker Desktop.exe"
    echo Waiting for Docker to start...
    timeout /t 10 /nobreak >nul
    docker info >nul 2>&1
    if %errorlevel% neq 0 (
        echo ❌ Docker failed to start. Please start Docker Desktop manually.
        pause
        exit /b 1
    )
)

echo ✅ Docker ready

REM Check Git
git --version >nul 2>&1
if %errorlevel% neq 0 (
    echo 📦 Installing Git...
    winget install Git.Git
    if %errorlevel% neq 0 (
        echo ❌ Git installation failed. Please install manually:
        echo https://git-scm.com/download/win
        pause
        exit /b 1
    )
)

echo ✅ Git ready

REM Clone project
echo 📥 Cloning project...
if exist "g1-ESP32-S3-Touch-LCD-1.85C" (
    echo Directory exists, updating...
    cd g1-ESP32-S3-Touch-LCD-1.85C
    git pull
) else (
    git clone https://github.com/amznhacker/g1-ESP32-S3-Touch-LCD-1.85C.git
    cd g1-ESP32-S3-Touch-LCD-1.85C
)

REM Check for ESP32 device
echo 🔍 Checking for ESP32-S3 device...
set DEVICE_FOUND=false
for /f "tokens=2 delims==" %%i in ('wmic path Win32_SerialPort where "Description like '%%USB%%'" get DeviceID /format:value 2^>nul ^| find "="') do (
    set DEVICE_FOUND=true
    goto found
)

:found
if "%DEVICE_FOUND%"=="false" (
    echo ⚠️  ESP32-S3 device not detected!
    echo Please:
    echo 1. Connect ESP32-S3 board via USB-C cable
    echo 2. Check Device Manager ^> Ports ^(COM ^& LPT^)
    echo 3. Install drivers if needed
    echo.
    echo Then run: run.bat
    pause
    exit /b 0
)

echo ✅ ESP32 device detected

REM Run setup
echo 🚀 Running setup...
call run.bat

echo.
echo 🎉 Setup complete!
echo 📱 Connect phone to 'ESP32_Face' Bluetooth and play music
pause