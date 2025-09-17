#!/bin/bash

echo "ESP32-S3 Face - Docker Setup (Linux/Mac)"
echo "========================================"

# Check Docker
if ! command -v docker &> /dev/null; then
    echo "ERROR: Docker not found!"
    echo "Install: curl -fsSL https://get.docker.com | sh"
    exit 1
fi

echo "Building ESP-IDF container..."
docker build -t esp32-face .

echo "Building project..."
docker run --rm -v "$(pwd):/workspace" esp32-face bash -c "cd /workspace && . /opt/esp-idf/export.sh && idf.py set-target esp32s3 && idf.py build"

if [ $? -ne 0 ]; then
    echo "BUILD FAILED!"
    exit 1
fi

echo "BUILD SUCCESS!"
echo ""

# Auto-detect port
PORT=""
for p in /dev/ttyUSB0 /dev/ttyACM0 /dev/cu.usbserial* /dev/cu.SLAB_USBtoUART; do
    if [ -e "$p" ]; then
        PORT="$p"
        break
    fi
done

if [ -z "$PORT" ]; then
    echo "Connect ESP32-S3 board and enter port:"
    read -p "Port (e.g., /dev/ttyUSB0): " PORT
fi

echo "Flashing to $PORT..."
docker run --rm -v "$(pwd):/workspace" --device="$PORT" esp32-face bash -c "cd /workspace && . /opt/esp-idf/export.sh && idf.py -p /dev/ttyUSB0 flash"

if [ $? -eq 0 ]; then
    echo ""
    echo "SUCCESS! Connect phone to 'ESP32_Face' Bluetooth"
else
    echo "FLASH FAILED - Try download mode (hold BOOT, press RESET)"
fi