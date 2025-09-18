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

**Ubuntu (SSH keys configured):**

```bash
git clone git@github.com:amznhacker/g1-ESP32-S3-Touch-LCD-1.85C.git
cd g1-ESP32-S3-Touch-LCD-1.85C
python3 setup.py
```

**WSL/Windows (HTTPS):**
```bash
git clone https://github.com/amznhacker/g1-ESP32-S3-Touch-LCD-1.85C.git
cd g1-ESP32-S3-Touch-LCD-1.85C
python3 setup.py
```

**That's it!** The setup script will:
- ✅ Check Docker and prerequisites
- 🏗️ Build ESP32-S3 firmware (5+ minutes)
- 🔍 Auto-detect your ESP32-S3 device
- ⚡ Flash firmware automatically
- 🛠️ Show fixes for any issues

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

1. **Clone & Run** `python3 setup.py`
2. **Connect Bluetooth** - Look for "ESP32-AudioFace"
3. **Watch** - Face animates with audio reactive mouth movement

## 🛠️ Manual Commands (if needed)

**Build only:**
```bash
docker run --rm -v $(pwd):/project -w /project espressif/idf:v5.3.1 bash -c "idf.py set-target esp32s3 && idf.py build"
```

**Flash only:**
```bash
python3 auto_flash.py
```
