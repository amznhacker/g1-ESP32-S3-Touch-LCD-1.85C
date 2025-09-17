#!/bin/bash

# Erase ESP32-S3 flash completely

echo "Erasing ESP32-S3 Flash"
echo "======================"

# Detect ESP32 port
PORT=""
if [ -e /dev/ttyUSB0 ]; then
    PORT="/dev/ttyUSB0"
elif [ -e /dev/ttyACM0 ]; then
    PORT="/dev/ttyACM0"
else
    echo "Error: ESP32 not found. Connect via USB and try again."
    exit 1
fi

echo "Using port: $PORT"
echo "Hold BOOT button, press RESET, release RESET, then release BOOT"
read -p "Press Enter when ESP32 is in download mode..."

# Erase flash
docker run --rm --privileged -v /dev:/dev -v $(pwd):/workspace --workdir /workspace esp32-face \
    bash -c "source /opt/esp-idf/export.sh && esptool.py --chip esp32s3 --port $PORT erase_flash"

echo "Flash erased! Now run ./flash-face.sh"