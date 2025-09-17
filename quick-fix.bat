@echo off
echo ESP32-S3 Quick Fix Script
echo =========================

cd /d "%~dp0"

echo Cleaning build...
if exist build rmdir /s /q build
if exist sdkconfig del sdkconfig

echo Setting ESP32-S3 target...
idf.py set-target esp32s3

echo Building project...
idf.py build

if %errorlevel% neq 0 (
    echo.
    echo BUILD FAILED - Try this:
    echo 1. Check ESP-IDF is installed: idf.py --version
    echo 2. Run: idf.py fullclean
    echo 3. Run this script again
    pause
    exit /b 1
)

echo.
echo BUILD SUCCESS!
echo.
echo Next steps:
echo 1. Connect ESP32-S3 board via USB
echo 2. Check Device Manager for COM port
echo 3. Run: idf.py -p COM3 flash
echo.
echo If flash fails (rainbow screen):
echo 1. Hold BOOT button
echo 2. Press RESET button
echo 3. Release BOOT button
echo 4. Run: idf.py -p COM3 erase-flash
echo 5. Run: idf.py -p COM3 flash
echo.
echo After success: Connect phone to 'ESP32_Face' Bluetooth
echo.
pause