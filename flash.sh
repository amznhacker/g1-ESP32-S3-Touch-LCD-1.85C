#!/bin/bash

# ESP32-S3 Flash Script
# Run inside Docker container

echo "ESP32-S3 Touch LCD Flash Script"
echo "================================"

# Check if device is connected
if [ ! -e /dev/ttyUSB0 ]; then
    echo "Error: ESP32 not found on /dev/ttyUSB0"
    echo "Try: /dev/ttyACM0 or check connection"
    exit 1
fi

# Source ESP-IDF environment
source /opt/esp-idf/export.sh

echo "1. Erasing flash..."
esptool.py --chip esp32s3 --port /dev/ttyUSB0 erase_flash

echo "2. Building project..."
idf.py build

echo "3. Flashing firmware..."
idf.py -p /dev/ttyUSB0 flash

echo "4. Starting monitor (Ctrl+] to exit)..."
idf.py -p /dev/ttyUSB0 monitor