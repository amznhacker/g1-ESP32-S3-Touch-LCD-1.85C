#!/bin/bash

echo "ESP32-S3 Face - One-Line Installer"
echo "=================================="

# Install Docker if not present
if ! command -v docker &> /dev/null; then
    echo "Installing Docker..."
    curl -fsSL https://get.docker.com | sh
    sudo usermod -aG docker $USER
    echo "Docker installed. Please logout and login again, then run this script."
    exit 0
fi

# Clone project
echo "Cloning project..."
git clone https://github.com/amznhacker/g1-ESP32-S3-Touch-LCD-1.85C.git
cd g1-ESP32-S3-Touch-LCD-1.85C

# Make executable and run
chmod +x run.sh
./run.sh

echo ""
echo "Done! Connect phone to 'ESP32_Face' Bluetooth and play music."