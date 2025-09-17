#!/bin/bash

echo "ESP32-S3 Flash Recovery"
echo "======================"
echo "1. Hold BOOT button"
echo "2. Press RESET button"
echo "3. Release RESET, then BOOT"
read -p "Press Enter when ready..."

# Detect port
PORT="/dev/ttyACM0"
if [ -e /dev/ttyUSB0 ]; then
    PORT="/dev/ttyUSB0"
fi

echo "Erasing flash on $PORT..."
docker run --rm --privileged \
    -v /dev:/dev \
    -v $(pwd):/workspace \
    -w /workspace \
    esp32-face \
    bash -c "source /opt/esp-idf/export.sh && esptool.py --chip esp32s3 --port $PORT erase_flash"

echo "Flash erased! Run ./flash-face.sh to reflash."