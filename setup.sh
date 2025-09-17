#!/bin/bash

# Complete automated setup script

echo "ESP32-S3 Animated Face - Complete Setup"
echo "======================================="

# Install dependencies
echo "Installing dependencies..."
sudo apt update
sudo apt install -y docker.io docker-compose git p7zip-full wget unzip

# Start Docker
sudo systemctl start docker
sudo systemctl enable docker
sudo usermod -aG docker $USER

# Check if ESP32 is connected
if ls /dev/ttyUSB* &> /dev/null || ls /dev/ttyACM* &> /dev/null; then
    echo "✓ ESP32 device detected"
    ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null
else
    echo "⚠ Connect ESP32 board via USB"
    read -p "Press Enter when ESP32 is connected..."
fi

# Build Docker environment
echo "Building Docker environment..."
docker-compose build

echo ""
echo "Setup complete! Next steps:"
echo "1. ./flash-demo.sh    # Test official firmware"
echo "2. ./flash-face.sh    # Flash animated face"
echo "3. Connect phone to 'ESP32_Face' Bluetooth"