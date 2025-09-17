# ESP32-S3 Animated Face Project

**Professional Bluetooth Audio Face Animation - Zero Dependencies Setup**

## Ultra-Lazy Setup (One Command Does Everything)

**Ubuntu/Linux:**
```bash
curl -fsSL https://raw.githubusercontent.com/amznhacker/g1-ESP32-S3-Touch-LCD-1.85C/main/install.sh | bash
```

**Windows:**
```cmd
powershell -c "irm https://raw.githubusercontent.com/amznhacker/g1-ESP32-S3-Touch-LCD-1.85C/main/install.bat | iex"
```

## Manual Setup (If You Want Control)

**Windows:**
```cmd
git clone https://github.com/amznhacker/g1-ESP32-S3-Touch-LCD-1.85C.git
cd g1-ESP32-S3-Touch-LCD-1.85C
run.bat
```

**Linux/Mac:**
```bash
git clone https://github.com/amznhacker/g1-ESP32-S3-Touch-LCD-1.85C.git
cd g1-ESP32-S3-Touch-LCD-1.85C
./run.sh
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