# ESP32-S3 Animated Face Project

Complete setup for ESP32-S3-Touch-LCD-1.85 with animated face responding to phone audio.

## Hardware Setup
1. **ESP32-S3-Touch-LCD-1.85** development board
2. **2030 speaker 8Ω 2W** - connect to board
3. **TF card** - insert into board (format as FAT32)
4. **USB Type-C cable** - for programming

## One-Command Setup

```bash
# 1. Setup everything automatically
./setup.sh

# 2. Flash animated face to ESP32
./flash-face.sh

# 3. Connect phone to "ESP32_Face" Bluetooth and play audio
```

## Manual Steps (if needed)

### Test Official Firmware First
```bash
# Flash official demo to verify hardware
./flash-demo.sh
```

### Build Custom Face
```bash
# Build and flash animated face
./flash-face.sh
```



## Arduino IDE Setup (Alternative)

### 1. Install Arduino IDE
- Download from arduino.cc
- Install ESP32 board package v3.0.2+

### 2. Install Libraries
- **LVGL v8.3.10** (offline install)
- **ESP32-audioI2S-master v2.0.0** (offline install)

### 3. Board Settings
- Board: ESP32S3 Dev Module
- Partition: 16M Flash (3MB APP/9.9MB FATFS)
- USB CDC: Enabled

### 4. Upload Code
- Open main/main.cpp in Arduino IDE
- Select correct COM port
- Upload (if fails, hold BOOT + press RESET)

## Troubleshooting

### Flash Issues
```bash
# If flashing fails:
# 1. Hold BOOT button
# 2. Press RESET button  
# 3. Release RESET, then BOOT
# 4. Try ./flash-face.sh again
```

### No Bluetooth Connection
```bash
# Check if face is running:
# 1. Press RESET on ESP32
# 2. Look for "ESP32_Face" in phone Bluetooth
# 3. Pair and play audio
```

## Project Files
```
├── setup.sh                # Complete automated setup
├── flash-demo.sh           # Flash official firmware
├── flash-face.sh           # Flash animated face
├── main/main.cpp           # Animated face code
├── Dockerfile              # Container definition
└── README.md               # This file
```