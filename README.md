# ESP32-S3 Animated Face Project

Complete setup for ESP32-S3-Touch-LCD-1.85 with animated face responding to phone audio.

## Hardware Setup
1. **ESP32-S3-Touch-LCD-1.85** development board
2. **2030 speaker 8Ω 2W** - connect to board
3. **TF card** - insert into board (format as FAT32)
4. **USB Type-C cable** - for programming

## Quick Start (Ubuntu Docker)

### 1. Download Official Demos (Optional)
```bash
# Install required tools
sudo apt update && sudo apt install -y p7zip-full wget unzip

# Download from Waveshare wiki Resources section:
# Visit: https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-1.85#Resources
# Click "ESP32-S3-Touch-LCD-1.85 Demo" link to download

# OR skip demos and go directly to step 2 for custom face code
```

### 2. Setup Environment
```bash
# Build Docker environment
docker-compose up --build

# Enter container
docker exec -it esp32-dev bash
source /opt/esp-idf/export.sh
```

### 3. Flash Test Firmware (Optional)
```bash
# Skip if demo download failed - go directly to step 4
# If demo available:
esptool.py --chip esp32s3 --port /dev/ttyUSB0 --baud 921600 write_flash -z 0x0 ESP32-S3-Touch-LCD-1.85-Demo/Firmware/firmware.bin
```

### 4. Flash Custom Face Code
```bash
# Build and flash animated face
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

### Manual Demo Download
If you need official demos:
1. Visit [Waveshare Wiki](https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-1.85#Resources)
2. Click "ESP32-S3-Touch-LCD-1.85 Demo" under Resources section
3. Extract downloaded file
4. Use libraries: LVGL v8.3.10, ESP32-audioI2S-master v2.0.0

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

### Download Issues
```bash
# Official demos have broken download links
# Skip to custom animated face code:
./quick-start.sh
# OR manually:
docker-compose up --build -d
docker exec -it esp32-dev bash -c "source /opt/esp-idf/export.sh && ./flash.sh"
```

### Can't Connect/Flash
1. Hold **BOOT** button
2. Press **RESET** button
3. Release **RESET**, then **BOOT**
4. Try flashing again

### No Display Output
- Check TF card is FAT32 formatted
- Verify speaker connections
- Press RESET after flashing

### Bluetooth Connection
1. Pair phone with "ESP32_Face"
2. Play audio from phone
3. Face animates with sound levels

## File Structure
```
├── Dockerfile              # Ubuntu environment
├── docker-compose.yml      # Container setup
├── platformio.ini          # ESP32-S3 config
├── main/
│   └── main.cpp            # Animated face code
└── README.md               # This file
```