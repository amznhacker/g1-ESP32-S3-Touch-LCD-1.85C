#!/bin/bash

echo "ESP32-S3 Animated Face - Build & Flash"
echo "====================================="

# Build Docker image if needed
if ! docker images esp32-face | grep -q esp32-face; then
    echo "Building ESP-IDF environment..."
    docker build -t esp32-face .
fi

# Detect port
PORT="/dev/ttyACM0"
if [ -e /dev/ttyUSB0 ]; then
    PORT="/dev/ttyUSB0"
fi

echo "Using port: $PORT"
echo "Building and flashing..."

# Build and flash in one command
docker run --rm --privileged \
    -v /dev:/dev \
    -v $(pwd):/workspace \
    -w /workspace \
    esp32-face \
    bash -c "source /opt/esp-idf/export.sh && \
             idf.py set-target esp32s3 && \
             idf.py build && \
             idf.py -p $PORT flash monitor"

echo "Done! Connect phone to 'ESP32_Face' Bluetooth and play audio."