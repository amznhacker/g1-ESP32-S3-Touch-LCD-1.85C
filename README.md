# ESP32-S3 Visual LCD Showcase

**Interactive Visual Effects on 360x360 LCD Display - Touch-Controlled Demos**

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
2. **auto_flash.py** - Auto-detects ESP32-S3 and flashes firmware
3. **Displays interactive visual effects** on the 360x360 LCD screen
4. **Touch-controlled demos** - tap screen to cycle through effects

## 🎯 Expected Result

**Visual (LCD Screen):**
- **Rainbow Circles** - Animated colorful circles rotating around center
- **Plasma Effect** - Smooth flowing plasma-like patterns  
- **Bouncing Balls** - Physics simulation with colorful bouncing balls
- **Color Gradient** - Dynamic shifting color gradients
- Touch screen to cycle between effects

**Serial Console:**
```
ESP32-S3 Touch LCD Visual Showcase Starting...
LCD initialized successfully
Visual Demo Showcase Ready!
Running: Rainbow Circles (Mode 0) - Touch to change
```

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

## If Flash Fails

**Enter Download Mode:**
1. Hold BOOT button
2. Press RESET button  
3. Release BOOT button

**Then run:**
```bash
idf.py -p YOUR_PORT erase-flash
idf.py -p YOUR_PORT flash
```

## Troubleshooting

```bash
# Check if ESP32 is detected:
dmesg | tail -10
# Should show USB device when plugged in

# If no visual output, check LCD connections
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

**Docker = True plug-and-play! Just install Docker and run the script.**