#!/bin/bash

# Legacy Docker run for older systems without Docker Compose

echo "Building ESP32 container..."
docker build -t esp32-face .

echo "Running container (legacy mode)..."
docker run -it --rm --privileged \
  -v /dev:/dev \
  -v $(pwd):/workspace \
  --workdir /workspace \
  --name esp32-dev \
  esp32-face bash -c "source /opt/esp-idf/export.sh && bash"