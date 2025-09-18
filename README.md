# 🎭 ESP32-S3 Audio Reactive Face

Animated face display with audio reactive features, Bluetooth connectivity, and multiple expressions for ESP32-S3 Touch LCD 1.85".

## ✨ Features

- 🎵 **Audio Reactive Animation** - Real-time mouth movement 
- 😊 **4 Facial Expressions** - Neutral, happy, surprised, sleepy
- 📱 **Bluetooth Classic** - "ESP32-AudioFace" device name
- ⚡ **30 FPS Animations** - Smooth on 240x280 ST7789 display
- 💾 **SD Card Support** - Custom face storage
- 🌈 **Audio Visualization** - Color-coded level bars

## 🚀 One-Command Setup

**Copy and paste this into your terminal:**

```bash
# Clone and build (Linux/WSL)
git clone https://github.com/ramigony/g1-ESP32-S3-Touch-LCD-1.85C.git
cd g1-ESP32-S3-Touch-LCD-1.85C
chmod +x *.py *.sh
docker run --rm -v $(pwd):/project -w /project espressif/idf:v5.3.1 bash -c "idf.py set-target esp32s3 && idf.py build"
```

**Windows PowerShell:**
```powershell
git clone https://github.com/ramigony/g1-ESP32-S3-Touch-LCD-1.85C.git
cd g1-ESP32-S3-Touch-LCD-1.85C
docker run --rm -v ${PWD}:/project -w /project espressif/idf:v5.3.1 bash -c "idf.py set-target esp32s3 && idf.py build"
```

## 📱 Flash to Device

**Auto-flash (Linux/WSL):**
```bash
python3 auto_flash.py
```

**Manual flash:**
```bash
python -m esptool --chip esp32s3 -p /dev/ttyACM0 -b 460800 write_flash 0x0 build/bootloader/bootloader.bin 0x8000 build/partition_table/partition-table.bin 0x10000 build/esp32_face.bin
```

## 🔧 Hardware Setup

**ESP32-S3 Touch LCD 1.85" (Waveshare)**
- Just connect USB-C cable
- No additional wiring needed
- Optional: Insert SD card for custom faces

## 📊 Project Status

| Feature | Status |
|---------|--------|
| LCD Display | ✅ Working |
| Audio Animation | ✅ Working |
| Bluetooth Classic | ✅ Working |
| 4 Expressions | ✅ Working |
| SD Card Support | ✅ Working |
| Real-time Audio | 🔄 In Progress |

**Firmware Size:** 870KB (83% flash usage, 17% free)

## 🎯 Quick Test

1. **Clone & Build** (copy-paste command above)
2. **Flash** with `python3 auto_flash.py`
3. **Connect Bluetooth** - Look for "ESP32-AudioFace"
4. **Watch** - Face animates with audio reactive mouth movement

**That's it!** No manual configuration needed.
