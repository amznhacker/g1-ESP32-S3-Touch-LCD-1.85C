#!/bin/bash

# ESP32-S3 Animated Face - Complete Setup Script

echo "ESP32-S3 Animated Face - Setup"
echo "=============================="

# Check if running on Ubuntu/Linux
if ! command -v apt &> /dev/null; then
    echo "This script is designed for Ubuntu/Debian systems"
    echo "Please install Docker manually on your system"
    exit 1
fi

# Install Docker if not present
if ! command -v docker &> /dev/null; then
    echo "Installing Docker..."
    sudo apt update
    sudo apt install -y docker.io
    sudo systemctl start docker
    sudo systemctl enable docker
    sudo usermod -aG docker $USER
    echo "⚠️  Please log out and back in for Docker permissions to take effect"
fi

# Check Docker is running
if ! docker info &> /dev/null; then
    echo "Starting Docker service..."
    sudo systemctl start docker
fi

# Install git if needed
if ! command -v git &> /dev/null; then
    echo "Installing git..."
    sudo apt install -y git
fi

# Check USB device access
echo "Checking for ESP32 device..."
if ls /dev/ttyUSB* /dev/ttyACM* &> /dev/null; then
    echo "✅ USB serial device found:"
    ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null
else
    echo "⚠️  No USB serial device found"
    echo "   Connect your ESP32-S3 board via USB-C cable"
fi

# Build Docker environment
echo "Building ESP-IDF environment (this may take a few minutes)..."
if docker build -t esp32-face .; then
    echo "✅ Docker environment ready"
else
    echo "❌ Docker build failed"
    exit 1
fi

echo ""
echo "Setup complete! Next steps:"
echo "1. Connect ESP32-S3 board via USB-C"
echo "2. Run: ./flash-face.sh"
echo "3. Connect phone to 'ESP32_Face' Bluetooth"
echo "4. Play audio and watch the console output"