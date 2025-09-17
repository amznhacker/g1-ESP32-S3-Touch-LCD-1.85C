#!/usr/bin/env python3
import os
import sys
import subprocess
import time
import glob

def find_esp32_port():
    """Auto-detect ESP32 port"""
    ports = []
    
    # Windows
    for i in range(1, 20):
        ports.append(f"COM{i}")
    
    # Linux/Mac
    ports.extend(glob.glob("/dev/ttyUSB*"))
    ports.extend(glob.glob("/dev/ttyACM*"))
    ports.extend(glob.glob("/dev/cu.usbserial*"))
    ports.extend(glob.glob("/dev/cu.SLAB_USBtoUART*"))
    
    for port in ports:
        try:
            if os.path.exists(port) or port.startswith("COM"):
                return port
        except:
            continue
    return None

def install_esptool():
    """Install esptool if not available"""
    try:
        subprocess.run(["esptool.py", "--version"], check=True, capture_output=True)
        print("✓ esptool.py found")
        return True
    except:
        print("Installing esptool...")
        try:
            subprocess.run([sys.executable, "-m", "pip", "install", "esptool"], check=True)
            return True
        except:
            print("ERROR: Could not install esptool")
            return False

def flash_esp32():
    """Flash the ESP32-S3"""
    print("ESP32-S3 Face Animation - Auto Flash")
    print("=" * 40)
    
    # Check if build files exist
    files = {
        "build/bootloader/bootloader.bin": "0x0",
        "build/partition_table/partition-table.bin": "0x8000", 
        "build/esp32_face.bin": "0x10000"
    }
    
    for file_path in files.keys():
        if not os.path.exists(file_path):
            print(f"ERROR: {file_path} not found!")
            print("Run: docker build -t esp32-face . && docker run --rm -v $(pwd):/workspace esp32-face")
            return False
    
    # Install esptool
    if not install_esptool():
        return False
    
    # Find port
    port = find_esp32_port()
    if not port:
        print("\nERROR: ESP32-S3 not detected!")
        print("\n1. Put ESP32-S3 in download mode:")
        print("   - Hold BOOT button")
        print("   - Press RESET button") 
        print("   - Release BOOT button")
        print("\n2. Try different USB cable")
        print("3. Check drivers (CP210x/CH340)")
        return False
    
    print(f"✓ Found ESP32-S3 on {port}")
    
    # Flash command
    cmd = [
        "esptool.py",
        "--chip", "esp32s3",
        "--port", port,
        "--baud", "460800",
        "write_flash"
    ]
    
    for file_path, address in files.items():
        cmd.extend([address, file_path])
    
    print(f"\nFlashing...")
    print(" ".join(cmd))
    
    try:
        result = subprocess.run(cmd, check=True)
        print("\n🎉 SUCCESS! ESP32-S3 flashed successfully!")
        print("\nConnect phone to 'ESP32_Face' Bluetooth and play music!")
        return True
    except subprocess.CalledProcessError as e:
        print(f"\n❌ FLASH FAILED: {e}")
        print("\nTry putting ESP32-S3 in download mode again")
        return False

if __name__ == "__main__":
    flash_esp32()