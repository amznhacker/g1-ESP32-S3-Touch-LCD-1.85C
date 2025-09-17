#!/bin/bash

# Quick Start Script - Skip official demos, go straight to animated face

echo "ESP32-S3 Animated Face - Quick Start"
echo "===================================="

# Install dependencies
sudo apt update
sudo apt install -y git python3 python3-pip

# Build Docker environment
echo "Building Docker environment..."
docker-compose up --build -d

# Flash animated face code
echo "Flashing animated face code..."
docker exec esp32-dev bash -c "source /opt/esp-idf/export.sh && ./flash.sh"

echo "Done! Connect your phone to 'ESP32_Face' Bluetooth device"