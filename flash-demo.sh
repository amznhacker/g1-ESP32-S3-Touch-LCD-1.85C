#!/bin/bash

# Flash official demo firmware

echo "Flashing Official Demo Firmware"
echo "==============================="

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

# Check if firmware exists
if [ -f "Firmware/firmware.bin" ]; then
    echo "Flashing official firmware..."
    docker run --rm --privileged -v /dev:/dev -v $(pwd):/workspace --workdir /workspace esp32-face \
        esptool.py --chip esp32s3 --port $PORT --baud 921600 write_flash -z 0x0 Firmware/firmware.bin
    echo "Done! Press RESET button on ESP32"
else
    echo "Error: Firmware/firmware.bin not found"
    echo "Download demos from: https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-1.85#Resources"
fi