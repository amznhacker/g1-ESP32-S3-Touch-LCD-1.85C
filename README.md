# ESP32-S3 Animated Face Project

**Professional Bluetooth Audio Face Animation - Zero Dependencies Setup**

## Option 1: Docker (Recommended - True Plug & Play)

**Windows:**
```cmd
# Install Docker Desktop: https://desktop.docker.com/win/main/amd64/Docker%20Desktop%20Installer.exe
git clone <this-repo>
cd g1-ESP32-S3-Touch-LCD-1.85C
run.bat
```

**Linux/Mac:**
```bash
# Install Docker: curl -fsSL https://get.docker.com | sh
git clone <this-repo>
cd g1-ESP32-S3-Touch-LCD-1.85C
chmod +x run.sh
./run.sh
```

## Option 2: Native ESP-IDF (Advanced Users)

**Windows:**
```cmd
# Download ESP-IDF installer: https://dl.espressif.com/dl/esp-idf-tools-setup-online-5.3.1.exe
# Open "ESP-IDF 5.3 CMD" from Start Menu
windows-install.bat
```

**Linux/Mac:**
```bash
# Install ESP-IDF first, then:
./recovery.sh
```

## Find Your Port

**Windows:** Device Manager → Ports (COM & LPT) → Look for "USB Serial Port (COM#)"

**Linux/Mac:** 
```bash
ls /dev/tty*USB* /dev/tty*ACM*
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

After successful flash:
1. Device creates "ESP32_Face" Bluetooth
2. Connect phone to "ESP32_Face"
3. Play music
4. Console shows: `Face: O_O ~~~♪ (Audio: 0.45)`

## Troubleshooting

**Build Error:** Run `idf.py fullclean` then try again

**Port Error:** Check USB cable, try different port

**Permission Error (Linux):** `sudo usermod -a -G dialout $USER` then logout/login

## Hardware Requirements

- ESP32-S3-Touch-LCD-1.85 board
- USB-C cable
- Phone with Bluetooth

## Why Docker?
- **Zero dependencies** - No ESP-IDF installation needed
- **Works everywhere** - Same environment on all machines
- **Easy maintenance** - Update container, not local tools
- **Clean system** - No toolchain pollution

## Project Structure

```
g1-ESP32-S3-Touch-LCD-1.85C/
├── main/
│   ├── main.c              # Bluetooth A2DP + Face Animation
│   └── CMakeLists.txt      # Component config
├── Dockerfile              # ESP-IDF container
├── run.bat                 # Windows Docker runner
├── run.sh                  # Linux/Mac Docker runner
├── CMakeLists.txt          # Project config
├── sdkconfig.defaults      # ESP32-S3 + Bluetooth settings
└── README.md              # This file
```

**Docker = True plug-and-play! Just install Docker and run the script.**