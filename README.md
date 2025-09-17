# ESP32-S3 Animated Face Project

Complete setup for ESP32-S3-Touch-LCD-1.85 with animated face responding to phone audio via Bluetooth.

## Hardware Requirements

### Essential Components
1. **ESP32-S3-Touch-LCD-1.85** development board (1.85 inch, 360×360 resolution)
2. **2030 speaker 8Ω 2W** - connect to board audio pins
3. **TF card** - insert into board (format as FAT32)
4. **USB Type-C cable** - for programming and power
5. **Phone with Bluetooth** - for audio streaming

### Board Specifications
- **Chip**: ESP32-S3-WROOM-1-N16R8 (16MB Flash, 8MB PSRAM)
- **Display**: 1.85" IPS LCD, 360×360 pixels, ST7789 driver
- **Touch**: CST816S capacitive touch controller
- **Audio**: I2S DAC output, onboard speaker connector
- **Connectivity**: Wi-Fi 802.11 b/g/n, Bluetooth 5.0
- **Sensors**: QMI8658 6-axis IMU, PCF85063 RTC
- **Storage**: TF card slot, 16MB onboard Flash

## Quick Start

### Prerequisites (Ubuntu/Linux)
```bash
# Install Docker if not already installed
sudo apt update
sudo apt install docker.io
sudo systemctl start docker
sudo usermod -aG docker $USER
# Log out and back in for group changes

# Install git if needed
sudo apt install git
```

### Setup and Flash
```bash
# 1. Clone repository
git clone https://github.com/amznhacker/g1-ESP32-S3-Touch-LCD-1.85C.git
cd g1-ESP32-S3-Touch-LCD-1.85C

# 2. Make scripts executable
chmod +x *.sh

# 3. Connect ESP32-S3 board via USB-C cable

# 4. Build and flash firmware
./flash-face.sh

# 5. Connect phone to "ESP32_Face" Bluetooth device
# 6. Play audio and watch console for face animations
```

## How It Works

### Bluetooth Audio Reception
- Creates Bluetooth A2DP sink named "ESP32_Face"
- Receives stereo audio stream from paired phone
- Analyzes audio levels in real-time
- Responds to music, speech, and sound effects

### Face Animation System
- **Eyes**: Blink and change size based on audio intensity
- **Mouth**: Opens wider during loud sounds/music
- **Expression**: Happy face during audio, sleepy when quiet
- **Console Output**: Real-time face status and audio levels

### Audio Response Patterns
- `Face: O_O ~~~♪` - Active audio detected
- `Face: -_- (Quiet)` - Silent or low audio
- Audio level: 0.0 (silent) to 1.0 (maximum)

## Troubleshooting

### Rainbow Screen Recovery
If you see a rainbow/corrupted display:
```bash
# 1. Put ESP32 in download mode:
#    - Hold BOOT button
#    - Press RESET button
#    - Release RESET, then BOOT

# 2. Erase corrupted firmware
./erase-flash.sh

# 3. Reflash working firmware
./flash-face.sh
```

### Connection Issues
```bash
# Check USB connection
ls /dev/ttyUSB* /dev/ttyACM*

# If no device found:
# - Try different USB cable
# - Check USB port
# - Install USB drivers if needed

# Force download mode:
# Hold BOOT + press RESET + release RESET + release BOOT
```

### Build Problems
```bash
# Clean build
docker system prune -f
docker build --no-cache -t esp32-face .
./flash-face.sh

# Check Docker permissions
sudo usermod -aG docker $USER
# Log out and back in
```

### Bluetooth Pairing
1. Flash firmware successfully
2. Press RESET button on ESP32
3. Wait 10-15 seconds for Bluetooth to initialize
4. Look for "ESP32_Face" in phone's Bluetooth settings
5. Pair and connect
6. Play audio from any app (music, YouTube, etc.)

## Development Environment

### Docker Container
- **Base**: Ubuntu 22.04
- **ESP-IDF**: v5.3.1
- **Target**: ESP32-S3
- **Toolchain**: Xtensa GCC 13.2.0

### Project Structure
```
├── flash-face.sh           # Build and flash script
├── erase-flash.sh          # Recovery/erase script  
├── main/
│   ├── main.c              # Main application code
│   └── CMakeLists.txt      # Component build config
├── CMakeLists.txt          # Project build config
├── sdkconfig.defaults      # ESP32-S3 configuration
├── Dockerfile              # ESP-IDF environment
└── README.md               # This documentation
```

### Configuration Details
- **Bluetooth**: Classic BT + A2DP sink enabled
- **Partition**: Single app (no OTA)
- **Log Level**: Info (shows face animations)
- **Stack Size**: 8KB main task
- **Target**: ESP32-S3 (auto-configured)

## Hardware Connections

### Audio Output
- **Speaker**: Connect 2030 8Ω 2W speaker to onboard connector
- **I2S DAC**: Built-in audio processing
- **Volume**: Controlled via Bluetooth device volume

### Power and Programming
- **USB-C**: Programming, power, and serial monitor
- **Battery**: Optional 3.7V Li-Po via MX1.25 connector
- **Power LED**: Indicates board power status

### Display and Touch
- **LCD**: 1.85" ST7789 controller (360×360)
- **Touch**: CST816S capacitive touch (currently unused)
- **Backlight**: PWM controlled brightness

## Based on Official Documentation

This project follows the [Waveshare ESP32-S3-Touch-LCD-1.85](https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-1.85) official specifications and uses ESP-IDF framework as recommended for professional development.

### Key Features Implemented
- ✅ Bluetooth A2DP audio sink
- ✅ Real-time audio level detection  
- ✅ Face animation system
- ✅ ESP32-S3 optimized build
- ✅ Recovery and troubleshooting tools
- 🔄 LCD display integration (future enhancement)
- 🔄 Touch interface (future enhancement)
- 🔄 IMU-based gestures (future enhancement)

## Support

For hardware issues, refer to the [official Waveshare documentation](https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-1.85).

For software issues, check the troubleshooting section above or create an issue in this repository.