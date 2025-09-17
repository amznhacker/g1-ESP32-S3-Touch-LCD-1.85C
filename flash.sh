#!/bin/bash

echo "🔥 ESP32-S3 Face Animation Flash Script"
echo "======================================="

# Check if build files exist
if [ ! -f "build/esp32_face.bin" ]; then
    echo "❌ Build files not found! Run setup first:"
    echo "./setup.sh"
    exit 1
fi

echo "✓ Build files found"

# Try to auto-detect port
PORT=""
for p in /dev/ttyUSB0 /dev/ttyUSB1 /dev/ttyACM0 /dev/ttyACM1; do
    if [ -e "$p" ] 2>/dev/null; then
        PORT="$p"
        echo "✓ Found device on $PORT"
        break
    fi
done

if [ -z "$PORT" ]; then
    echo "⚠️  No device auto-detected"
    echo ""
    echo "1. Put ESP32-S3 in download mode:"
    echo "   - Hold BOOT button"
    echo "   - Press RESET button"
    echo "   - Release BOOT button"
    echo ""
    echo "2. Enter your port manually:"
    read -p "Port (e.g., /dev/ttyUSB0): " PORT
fi

echo ""
echo "🚀 Flashing ESP32-S3 on $PORT..."

# Flash command
~/.local/bin/esptool.py --chip esp32s3 --port "$PORT" --baud 460800 write_flash \
    0x0 build/bootloader/bootloader.bin \
    0x8000 build/partition_table/partition-table.bin \
    0x10000 build/esp32_face.bin

if [ $? -eq 0 ]; then
    echo ""
    echo "🎉 SUCCESS! ESP32-S3 flashed successfully!"
    echo ""
    echo "Connect phone to 'ESP32_Face' Bluetooth and play music!"
    echo "Face animations will appear in serial output"
else
    echo ""
    echo "❌ FLASH FAILED!"
    echo "Try putting ESP32-S3 in download mode again"
fi