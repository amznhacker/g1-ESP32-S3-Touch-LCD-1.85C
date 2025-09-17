@echo off
echo ESP32-S3 Face - Docker Setup (Windows)
echo ====================================

REM Check if Docker is running
docker --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: Docker not found!
    echo Install Docker Desktop: https://desktop.docker.com/win/main/amd64/Docker%20Desktop%20Installer.exe
    pause
    exit /b 1
)

echo Building ESP-IDF container...
docker build -t esp32-face .

echo Building project...
docker run --rm -v "%cd%:/workspace" esp32-face bash -c "cd /workspace && . /opt/esp-idf/export.sh && idf.py set-target esp32s3 && idf.py build"

if %errorlevel% neq 0 (
    echo BUILD FAILED!
    pause
    exit /b 1
)

echo BUILD SUCCESS!
echo.
echo Connect ESP32-S3 board and find COM port in Device Manager
echo Then run: docker run --rm -v "%cd%:/workspace" --device=COM3 esp32-face bash -c "cd /workspace && . /opt/esp-idf/export.sh && idf.py -p /dev/ttyUSB0 flash"
echo.
pause