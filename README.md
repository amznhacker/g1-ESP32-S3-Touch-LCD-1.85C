# ESP32-S3 Animated Face Project

**Professional Bluetooth Audio Face Animation - Replaces Rainbow Screen**

## 🚀 Quick Start (Ubuntu)

```bash
git clone https://github.com/amznhacker/g1-ESP32-S3-Touch-LCD-1.85C.git
cd g1-ESP32-S3-Touch-LCD-1.85C
chmod +x *.sh
./setup.sh
python3 auto_flash.py
```

## 📋 What it does

1. **setup.sh** - Installs everything (Docker, esptool, builds firmware)
2. **auto_flash.py** - Auto-detects ESP32-S3 and flashes (replaces rainbow screen)
3. **Creates "ESP32_Face" Bluetooth device** for phone connection
4. **Real-time face animations** based on audio levels

## 🎯 Expected Result

- ❌ Rainbow screen disappears
- ✅ "ESP32_Face" Bluetooth device appears
- ✅ Connect phone and play music
- ✅ Face animations: `Face: O_O ~~~♪ (Audio: 0.45)`

## 🔧 Alternative Methods

**One-Line Install:**
```bash
curl -fsSL https://raw.githubusercontent.com/amznhacker/g1-ESP32-S3-Touch-LCD-1.85C/main/install.sh | bash
```

**Docker Method:**
```bash
./run.sh  # Uses Docker container
```

**Manual Flash:**
```bash
./flash.sh  # Direct esptool flashing
```

## Find Your Port & Monitor Serial Output

**Find ESP32 Port:**
```bash
# Ubuntu/Linux:
ls /dev/ttyUSB* /dev/ttyACM*

# Windows: Device Manager → Ports (COM & LPT) → Look for "USB Serial Port (COM#)"
```

**Monitor Serial Output (Ubuntu):**
```bash
# Method 1 - ESP-IDF monitor (recommended):
idf.py -p /dev/ttyUSB0 monitor
# Exit: Ctrl+]

# Method 2 - Screen:
screen /dev/ttyUSB0 115200
# Exit: Ctrl+A then K

# Method 3 - Minicom:
minicom -D /dev/ttyUSB0 -b 115200
# Exit: Ctrl+A then X
```

**Expected Serial Output:**
```
ESP32-S3 Touch LCD Showcase Starting...
Backlight initialized
Touch button initialized
Device Capabilities Showcase
- 1.85" 360x360 LCD with backlight control
- Touch button interaction
- Multiple visual effects
Running demo mode 0 - Touch to change
Demo: Breathing light
```

## If Flash Fails (Rainbow Screen)

**Enter Download Mode:**
1. Hold BOOT button
2. Press RESET button
3. Release BOOT button

**Then run:**
```bash
idf.py -p YOUR_PORT erase-flash
idf.py -p YOUR_PORT flash
```

## Expected Result

**Visual (LCD Screen):**
- Backlight effects cycling through 4 modes
- Touch screen to change between: Breathing → Pulse → Strobe → Brightness levels

**Serial Console:**
```
ESP32-S3 Touch LCD Showcase Starting...
Running demo mode 0 - Touch to change
Demo: Breathing light
```

**Troubleshooting:**
```bash
# Check if ESP32 is detected:
dmesg | tail -10
# Should show USB device when plugged in

# If no serial output, device might not be flashed correctly
```

## Why Docker?
- **Zero dependencies** - No ESP-IDF installation needed
- **Works everywhere** - Same environment on all machines
- **Easy maintenance** - Update container, not local tools
- **Clean system** - No toolchain pollution

## Hardware Requirements

- ESP32-S3-Touch-LCD-1.85 board
- USB-C cable
- Phone with Bluetooth

**Docker = True plug-and-play! Just install Docker and run the script.**