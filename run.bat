@echo off
echo ESP32-S3 Face - Docker Setup
echo =============================

REM Check Docker
docker --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ❌ ERROR: Docker not found!
    echo Install Docker Desktop: https://desktop.docker.com/win/main/amd64/Docker%20Desktop%20Installer.exe
    pause
    exit /b 1
)

REM Check if Docker is running
docker info >nul 2>&1
if %errorlevel% neq 0 (
    echo ❌ ERROR: Docker not running!
    echo Start Docker Desktop and try again
    pause
    exit /b 1
)

echo ✅ Docker found and running

REM Check for ESP32 device in Device Manager
echo 🔍 Checking for ESP32-S3 device...
set DEVICE_FOUND=false
set PORT=

REM Look for common ESP32 COM ports
for /f "tokens=2 delims==" %%i in ('wmic path Win32_SerialPort where "Description like '%%USB%%' or Description like '%%Serial%%'" get DeviceID /format:value 2^>nul ^| find "="') do (
    set PORT=%%i
    set DEVICE_FOUND=true
    goto found
)

:found
if "%DEVICE_FOUND%"=="false" (
    echo ⚠️  WARNING: ESP32-S3 device not detected!
    echo Please:
    echo 1. Connect ESP32-S3 board via USB-C
    echo 2. Check Device Manager ^> Ports ^(COM ^& LPT^)
    echo 3. Install drivers if needed
    echo.
    set /p continue="Continue anyway? (y/n): "
    if /i not "%continue%"=="y" exit /b 1
    set /p PORT="Enter COM port (e.g., COM3): "
) else (
    echo ✅ ESP32 device found at %PORT%
)

echo 🔨 Building ESP-IDF container...
docker build -t esp32-face .
if %errorlevel% neq 0 (
    echo ❌ Container build failed!
    pause
    exit /b 1
)

echo 🔨 Building project...
docker run --rm -v "%cd%:/workspace" esp32-face bash -c "cd /workspace && . /opt/esp-idf/export.sh && idf.py set-target esp32s3 && idf.py build"
if %errorlevel% neq 0 (
    echo ❌ Project build failed!
    pause
    exit /b 1
)

echo ✅ Build successful!

echo 📡 Flashing to %PORT%...
docker run --rm -v "%cd%:/workspace" --device=%PORT% esp32-face bash -c "cd /workspace && . /opt/esp-idf/export.sh && idf.py -p /dev/ttyUSB0 flash"

if %errorlevel% equ 0 (
    echo.
    echo 🎉 SUCCESS! ESP32_Face Bluetooth device ready!
    echo 📱 Connect your phone to 'ESP32_Face' and play music
    echo 👀 Watch console for face animations
    echo.
    set /p monitor="Monitor serial output? (y/n): "
    if /i "%monitor%"=="y" (
        docker run --rm -it -v "%cd%:/workspace" --device=%PORT% esp32-face bash -c "cd /workspace && . /opt/esp-idf/export.sh && idf.py -p /dev/ttyUSB0 monitor"
    )
) else (
    echo.
    echo ❌ FLASH FAILED!
    echo Try download mode:
    echo 1. Hold BOOT button
    echo 2. Press RESET button
    echo 3. Release BOOT button
    echo 4. Run this script again
)

pause