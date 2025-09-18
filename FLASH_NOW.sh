#!/bin/bash

echo "🎵 ESP32-S3 Audio Reactive Face Flasher 🎵"
echo "=========================================="

# Check if firmware exists
if [ ! -f "build/esp32_face.bin" ]; then
    echo "❌ Firmware not found. Building first..."
    docker run --rm -v $PWD:/project -w /project espressif/idf:v5.3.1 idf.py build
fi

echo "📱 Connect your ESP32-S3 via USB and press Enter..."
read

# Auto-detect port
PORT=$(ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null | head -1)
if [ -z "$PORT" ]; then
    echo "❌ No USB device found. Check connection."
    exit 1
fi

echo "🔌 Found device: $PORT"
echo "⚡ Flashing audio reactive face firmware..."

python3 -m esptool --chip esp32s3 -p $PORT -b 460800 \
    --before default_reset --after hard_reset write_flash \
    --flash_mode dio --flash_size 2MB --flash_freq 80m \
    0x0 build/bootloader/bootloader.bin \
    0x8000 build/partition_table/partition-table.bin \
    0x10000 build/esp32_face.bin

if [ $? -eq 0 ]; then
    echo ""
    echo "✅ Flash successful!"
    echo "🎉 Your ESP32-S3 Audio Reactive Face is ready!"
    echo ""
    echo "Next steps:"
    echo "1. Power on your device"
    echo "2. Look for 'ESP32-AudioFace' in Bluetooth settings"
    echo "3. Enjoy the animated face! 😊"
else
    echo "❌ Flash failed. Check connections and try again."
fi