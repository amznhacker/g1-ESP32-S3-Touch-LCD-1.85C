# ESP32-S3 Audio Reactive Face

An animated face that reacts to audio with multiple expressions, displayed on a 1.85" LCD screen.

## Features

- **Audio Reactive Mouth** - Mouth opens/closes with music (simulated, ready for Bluetooth A2DP)
- **Multiple Expressions** - 4 different facial expressions (neutral, happy, surprised, sleepy)
- **Audio Visualization** - Real-time audio level bars with color coding
- **Bluetooth Ready** - Foundation for connecting to phone audio
- **SD Card Support** - Ready for storing custom face images
- **30 FPS Animation** - Smooth real-time graphics

## Hardware

- **ESP32-S3** microcontroller
- **Waveshare ESP32-S3 Touch LCD 1.85"** (240x280 ST7789)
- **SD Card** (optional)

## Quick Start

1. **Flash the firmware:**
   ```bash
   ./FLASH_NOW.sh
   ```

2. **Power on** - Face starts animating immediately

3. **Connect Bluetooth** - Look for "ESP32-AudioFace" in phone settings

## What You'll See

- Animated face with changing expressions
- Audio level bars (green/yellow/red)
- Bluetooth status indicator (blue=connected, red=waiting)
- Smooth mouth animation reacting to simulated audio

## Current Status

✅ **Working:** Face animation, expressions, LCD display, Bluetooth foundation
🔄 **Next:** Real Bluetooth A2DP audio streaming integration

## Build Requirements

- Docker (for ESP-IDF environment)
- USB connection to ESP32-S3

## File Structure

```
├── main/main.c          # Audio reactive face code
├── build/               # Compiled firmware
├── FLASH_NOW.sh         # Flash script
└── README.md           # This file
```

## Technical Details

- **Display:** ST7789 240x280 LCD via SPI
- **Animation:** 30 FPS with expression cycling
- **Audio:** Simulated reactive mouth (ready for real audio)
- **Memory:** 870KB firmware, 76% flash free
- **Bluetooth:** Classic BT stack initialized

Ready to flash and enjoy your audio reactive face! 🎵😊