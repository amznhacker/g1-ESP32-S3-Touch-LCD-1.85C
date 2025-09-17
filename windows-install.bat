@echo off
echo ESP32-S3 Face - Windows Auto-Installer
echo ======================================

echo Step 1: Checking if ESP-IDF is installed...
idf.py --version >nul 2>&1
if %errorlevel% equ 0 (
    echo ESP-IDF found! Skipping installation.
    goto build
)

echo ESP-IDF not found. Please install it:
echo.
echo 1. Download: https://dl.espressif.com/dl/esp-idf-tools-setup-online-5.3.1.exe
echo 2. Run the installer (installs everything automatically)
echo 3. Open "ESP-IDF 5.3 CMD" from Start Menu
echo 4. Run this script again
echo.
pause
exit /b 1

:build
echo Step 2: Building ESP32-S3 Face project...
cd /d "%~dp0"

if exist build rmdir /s /q build
if exist sdkconfig del sdkconfig

idf.py set-target esp32s3
idf.py build

if %errorlevel% neq 0 (
    echo BUILD FAILED!
    pause
    exit /b 1
)

echo.
echo BUILD SUCCESS!
echo.
echo Step 3: Connect ESP32-S3 board and find COM port in Device Manager
echo Then run: idf.py -p COM3 flash
echo.
echo After flash: Connect phone to "ESP32_Face" Bluetooth
echo.
pause