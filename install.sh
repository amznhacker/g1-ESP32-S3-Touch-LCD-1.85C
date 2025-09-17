#!/bin/bash

echo "ESP32-S3 Face - One-Line Installer"
echo "=================================="

# Check if running as root
if [ "$EUID" -eq 0 ]; then
    echo "❌ Don't run as root! Run as regular user."
    exit 1
fi

# Install Docker if not present
if ! command -v docker &> /dev/null; then
    echo "🐳 Installing Docker..."
    curl -fsSL https://get.docker.com | sh
    sudo usermod -aG docker $USER
    echo "✅ Docker installed!"
    echo "⚠️  Please logout and login again, then run:"
    echo "curl -fsSL https://raw.githubusercontent.com/amznhacker/g1-ESP32-S3-Touch-LCD-1.85C/main/install.sh | bash"
    exit 0
fi

# Check if user is in docker group
if ! groups | grep -q docker; then
    echo "⚠️  Adding user to docker group..."
    sudo usermod -aG docker $USER
    echo "Please logout and login again, then run this script."
    exit 0
fi

# Check Docker daemon
if ! docker info &> /dev/null; then
    echo "🐳 Starting Docker..."
    sudo systemctl start docker
    sudo systemctl enable docker
fi

echo "✅ Docker ready"

# Install git if needed
if ! command -v git &> /dev/null; then
    echo "📦 Installing git..."
    if command -v apt &> /dev/null; then
        sudo apt update && sudo apt install -y git
    elif command -v yum &> /dev/null; then
        sudo yum install -y git
    elif command -v pacman &> /dev/null; then
        sudo pacman -S git
    else
        echo "❌ Please install git manually"
        exit 1
    fi
fi

# Clone project
echo "📥 Cloning project..."
if [ -d "g1-ESP32-S3-Touch-LCD-1.85C" ]; then
    echo "Directory exists, updating..."
    cd g1-ESP32-S3-Touch-LCD-1.85C
    git pull
else
    git clone https://github.com/amznhacker/g1-ESP32-S3-Touch-LCD-1.85C.git
    cd g1-ESP32-S3-Touch-LCD-1.85C
fi

# Check for ESP32 device before building
echo "🔍 Checking for ESP32-S3 device..."
DEVICE_FOUND=false
for p in /dev/ttyUSB* /dev/ttyACM* /dev/cu.usbserial* /dev/cu.SLAB_USBtoUART; do
    if [ -e "$p" ]; then
        DEVICE_FOUND=true
        break
    fi
done

if [ "$DEVICE_FOUND" = false ]; then
    echo "⚠️  ESP32-S3 device not detected!"
    echo "Please connect your ESP32-S3 board via USB-C cable"
    echo "Then run: ./run.sh"
    exit 0
fi

echo "✅ ESP32 device detected"

# Make executable and run
chmod +x run.sh
./run.sh

echo ""
echo "🎉 Setup complete!"
echo "📱 Connect phone to 'ESP32_Face' Bluetooth and play music"