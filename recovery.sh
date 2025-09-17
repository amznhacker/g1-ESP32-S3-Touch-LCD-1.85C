#!/bin/bash

echo "ESP32-S3 Recovery Script"
echo "======================="

cd "$(dirname "$0")"

if [ ! -f "main/main.c" ]; then
    echo "ERROR: main/main.c not found"
    exit 1
fi

echo "Cleaning build..."
rm -rf build sdkconfig

echo "Setting ESP32-S3 target..."
idf.py set-target esp32s3

echo "Building project..."
idf.py build

if [ $? -ne 0 ]; then
    echo ""
    echo "BUILD FAILED - Try this:"
    echo "1. Check ESP-IDF: idf.py --version"
    echo "2. Run: idf.py fullclean"
    echo "3. Run this script again"
    exit 1
fi

echo ""
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
    echo "No port found. Connect ESP32-S3 and enter port:"
    read -p "Port (e.g., /dev/ttyUSB0): " PORT
fi

echo "Flashing to $PORT..."
idf.py -p "$PORT" flash

if [ $? -eq 0 ]; then
    echo ""
    echo "SUCCESS! Connect phone to 'ESP32_Face' Bluetooth"
    echo ""
    read -p "Monitor output? (y/n): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        idf.py -p "$PORT" monitor
    fi
else
    echo ""
    echo "FLASH FAILED - Enter download mode:"
    echo "1. Hold BOOT button"
    echo "2. Press RESET button"
    echo "3. Release BOOT button"
    echo "4. Run: idf.py -p $PORT erase-flash"
    echo "5. Run: idf.py -p $PORT flash"
fi