#!/bin/bash

echo "🚀 ESP32-S3 Face Animation - Complete Setup"
echo "==========================================="
echo ""

# Check if running on Ubuntu/Debian
if ! command -v apt &> /dev/null; then
    echo "❌ This script requires Ubuntu/Debian with apt package manager"
    exit 1
fi

echo "📦 Installing dependencies..."
sudo apt update -qq
sudo apt install -y python3 python3-pip python3-venv git curl docker.io

echo "🐳 Setting up Docker..."
sudo usermod -aG docker $USER
sudo systemctl start docker
sudo systemctl enable docker

echo "🔧 Installing esptool..."
python3 -m pip install --user esptool

echo "🏗️  Building ESP-IDF container..."
sudo docker build -t esp32-face . -q

echo "⚙️  Building firmware..."
sudo docker run --rm -v "$(pwd):/workspace" esp32-face bash -c "cd /workspace && . /opt/esp-idf/export.sh && idf.py build" > build.log 2>&1

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
else
    echo "❌ Build failed! Check build.log"
    exit 1
fi

echo ""
echo "🎉 SETUP COMPLETE!"
echo ""
echo "Next steps:"
echo "1. Connect ESP32-S3 via USB"
echo "2. Run: python3 auto_flash.py"
echo ""
echo "The rainbow screen will be replaced with Bluetooth face animation!"