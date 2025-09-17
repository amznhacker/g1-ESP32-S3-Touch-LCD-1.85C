#!/bin/bash

# Dependency Check Script

echo "ESP32-S3 Development Environment Check"
echo "====================================="

# Check Docker
if command -v docker &> /dev/null; then
    echo "✓ Docker installed: $(docker --version)"
else
    echo "✗ Docker not found. Installing..."
    sudo apt update
    sudo apt install -y docker.io
    sudo systemctl start docker
    sudo systemctl enable docker
    sudo usermod -aG docker $USER
    echo "⚠ Please log out and back in for Docker group changes"
fi

# Check Docker Compose
if command -v docker-compose &> /dev/null; then
    echo "✓ Docker Compose installed: $(docker-compose --version)"
elif command -v docker compose &> /dev/null; then
    echo "✓ Docker Compose (plugin) installed"
else
    echo "✗ Docker Compose not found. Installing..."
    sudo apt install -y docker-compose
fi

# Check USB devices
if ls /dev/ttyUSB* &> /dev/null || ls /dev/ttyACM* &> /dev/null; then
    echo "✓ USB serial devices found:"
    ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null
else
    echo "⚠ No USB serial devices found. Connect ESP32 board."
fi

# Check git
if command -v git &> /dev/null; then
    echo "✓ Git installed"
else
    echo "✗ Git not found. Installing..."
    sudo apt install -y git
fi

echo ""
echo "Ready to proceed with: ./quick-start.sh"