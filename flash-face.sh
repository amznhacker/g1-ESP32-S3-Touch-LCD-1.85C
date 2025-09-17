#!/bin/bash

# Flash animated face firmware

echo "Building and Flashing Animated Face"
echo "==================================="

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

# Check if container exists, if not build it
if ! docker images esp32-face | grep -q esp32-face; then
    echo "Building Docker image..."
    docker build -t esp32-face .
fi

# Build and flash
echo "Building animated face firmware..."
docker run --rm --privileged -v /dev:/dev -v $(pwd):/workspace --workdir /workspace esp32-face \
    bash -c "source /opt/esp-idf/export.sh && idf.py set-target esp32s3 && idf.py build && idf.py -p $PORT flash"

echo ""
echo "Flashing complete!"
echo "1. Press RESET button on ESP32"
echo "2. Pair phone with 'ESP32_Face' Bluetooth"
echo "3. Play audio - watch face animate!"