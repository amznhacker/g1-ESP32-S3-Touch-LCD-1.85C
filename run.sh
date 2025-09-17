#!/bin/bash

echo "ESP32-S3 Face - Docker Setup"
echo "============================"

# Check Docker
if ! command -v docker &> /dev/null; then
    echo "❌ ERROR: Docker not found!"
    echo "Install: curl -fsSL https://get.docker.com | sh"
    exit 1
fi

# Check if Docker daemon is running
if ! docker info &> /dev/null; then
    echo "❌ ERROR: Docker daemon not running!"
    echo "Start Docker service: sudo systemctl start docker"
    exit 1
fi

echo "✅ Docker found and running"

# Check for ESP32-S3 device
echo "🔍 Checking for ESP32-S3 device..."
PORT=""
DEVICE_FOUND=false

for p in /dev/ttyUSB* /dev/ttyACM* /dev/cu.usbserial* /dev/cu.SLAB_USBtoUART; do
    if [ -e "$p" ]; then
        # Check if it's likely an ESP32 device
        if lsusb 2>/dev/null | grep -i "espressif\|silicon labs\|ch340\|cp210" &> /dev/null; then
            PORT="$p"
            DEVICE_FOUND=true
            break
        fi
    fi
done

if [ "$DEVICE_FOUND" = false ]; then
    echo "⚠️  WARNING: ESP32-S3 device not detected!"
    echo "Please:"
    echo "1. Connect ESP32-S3 board via USB-C"
    echo "2. Check cable (data cable, not charge-only)"
    echo "3. Install drivers if needed"
    read -p "Continue anyway? (y/n): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
    read -p "Enter port manually (e.g., /dev/ttyUSB0): " PORT
else
    echo "✅ ESP32 device found at $PORT"
fi

echo "🔨 Building ESP-IDF container..."
docker build -t esp32-face . || { echo "❌ Container build failed!"; exit 1; }

echo "🔨 Building project..."
docker run --rm -v "$(pwd):/workspace" esp32-face bash -c "cd /workspace && . /opt/esp-idf/export.sh && idf.py set-target esp32s3 && idf.py build" || { echo "❌ Project build failed!"; exit 1; }

echo "✅ Build successful!"

echo "📡 Flashing to $PORT..."
docker run --rm -v "$(pwd):/workspace" --device="$PORT" esp32-face bash -c "cd /workspace && . /opt/esp-idf/export.sh && idf.py -p /dev/ttyUSB0 flash"

if [ $? -eq 0 ]; then
    echo ""
    echo "🎉 SUCCESS! ESP32_Face Bluetooth device ready!"
    echo "📱 Connect your phone to 'ESP32_Face' and play music"
    echo "👀 Watch console for face animations"
    
    read -p "Monitor serial output? (y/n): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        docker run --rm -it -v "$(pwd):/workspace" --device="$PORT" esp32-face bash -c "cd /workspace && . /opt/esp-idf/export.sh && idf.py -p /dev/ttyUSB0 monitor"
    fi
else
    echo ""
    echo "❌ FLASH FAILED!"
    echo "Try download mode:"
    echo "1. Hold BOOT button"
    echo "2. Press RESET button"
    echo "3. Release BOOT button"
    echo "4. Run this script again"
fi