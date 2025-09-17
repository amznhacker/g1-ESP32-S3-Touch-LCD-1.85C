# ESP32-S3 Animated Face Project

Complete setup for ESP32-S3-Touch-LCD-1.85 with animated face responding to phone audio.

## Hardware Setup
1. **ESP32-S3-Touch-LCD-1.85** development board
2. **2030 speaker 8Ω 2W** - connect to board
3. **TF card** - insert into board (format as FAT32)
4. **USB Type-C cable** - for programming

## One-Command Setup

```bash
# Make scripts executable
chmod +x *.sh

# 1. Setup everything automatically
./setup.sh

# 2. If setup fails, build manually:
docker build -t esp32-face .

# 3. Flash animated face to ESP32
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



## Troubleshooting

### Permission Denied
```bash
# Option 1: Fix permissions
chmod +x *.sh && ./setup.sh

# Option 2: Run with bash directly
bash setup.sh
bash flash-face.sh
```

### Docker Compose Issues
```bash
# If docker-compose fails, use plain Docker:
docker build -t esp32-face .
# Then skip to flash-face.sh
```

### Missing Firmware
```bash
# Skip demo, go directly to custom face:
./flash-face.sh
# Demo firmware is optional - custom face works without it
```

### Flash Issues
```bash
# If flashing fails or rainbow screen appears:
# 1. Hold BOOT button
# 2. Press RESET button  
# 3. Release RESET, then BOOT
# 4. Erase flash completely:
docker run --rm --privileged -v /dev:/dev -v $(pwd):/workspace --workdir /workspace esp32-face \
    bash -c "source /opt/esp-idf/export.sh && esptool.py --chip esp32s3 --port /dev/ttyACM0 erase_flash"
# 5. Try ./flash-face.sh again
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
├── main/
│   ├── main.cpp            # Animated face code
│   └── CMakeLists.txt      # Build configuration
├── CMakeLists.txt          # Project configuration
├── docker-compose.yml      # Container orchestration
├── Dockerfile              # Container definition
└── README.md               # This file
```