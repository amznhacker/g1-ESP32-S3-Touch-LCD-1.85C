#!/usr/bin/env python3
"""
ESP32-S3 Audio Face - Complete Setup & Flash Tool
Handles build, flash, and diagnostics with error recovery
"""

import subprocess
import sys
import os
import time
import glob

def run_cmd(cmd, description="", check_output=False):
    """Run command with error handling"""
    print(f"\n🔄 {description}")
    print(f"   Command: {cmd}")
    
    try:
        if check_output:
            result = subprocess.run(cmd, shell=True, capture_output=True, text=True, timeout=300)
            if result.returncode != 0:
                print(f"❌ Failed: {result.stderr}")
                return False, result.stderr
            return True, result.stdout
        else:
            result = subprocess.run(cmd, shell=True, timeout=300)
            return result.returncode == 0, ""
    except subprocess.TimeoutExpired:
        print("❌ Command timed out (5 minutes)")
        return False, "Timeout"
    except Exception as e:
        print(f"❌ Error: {e}")
        return False, str(e)

def check_prerequisites():
    """Check Docker and system requirements"""
    print("🔍 Checking prerequisites...")
    
    # Check Docker
    success, _ = run_cmd("docker --version", "Checking Docker", True)
    if not success:
        print("❌ Docker not found. Install Docker first:")
        print("   Ubuntu: sudo apt update && sudo apt install docker.io")
        print("   Windows: Download Docker Desktop")
        return False
    
    # Check Docker daemon
    success, _ = run_cmd("docker ps", "Testing Docker daemon", True)
    if not success:
        print("❌ Docker daemon not running. Start Docker service:")
        print("   Ubuntu: sudo systemctl start docker")
        print("   Windows: Start Docker Desktop")
        return False
    
    print("✅ Prerequisites OK")
    return True

def build_firmware():
    """Build ESP32-S3 firmware with Docker"""
    print("\n🏗️  Building ESP32-S3 firmware...")
    
    # Set target and build
    cmd = 'docker run --rm -v $(pwd):/project -w /project espressif/idf:v5.3.1 bash -c "idf.py set-target esp32s3 && idf.py build"'
    success, output = run_cmd(cmd, "Building firmware (this may take 5+ minutes)")
    
    if not success:
        print("❌ Build failed. Common fixes:")
        print("   1. Clean build: rm -rf build/ sdkconfig")
        print("   2. Check Docker permissions: sudo usermod -aG docker $USER")
        print("   3. Restart Docker service")
        return False
    
    # Verify build output
    if not os.path.exists("build/esp32_face.bin"):
        print("❌ Firmware binary not generated")
        return False
    
    size = os.path.getsize("build/esp32_face.bin")
    print(f"✅ Build successful: {size//1024}KB firmware generated")
    return True

def find_esp32_device():
    """Auto-detect ESP32-S3 device"""
    print("\n🔍 Detecting ESP32-S3 device...")
    
    # Common ESP32 device patterns
    patterns = ["/dev/ttyUSB*", "/dev/ttyACM*", "/dev/cu.usbserial*", "/dev/cu.SLAB*"]
    devices = []
    
    for pattern in patterns:
        devices.extend(glob.glob(pattern))
    
    if not devices:
        print("❌ No ESP32 device found. Check:")
        print("   1. USB cable connected")
        print("   2. ESP32-S3 powered on")
        print("   3. Driver installed (CP210x/CH340)")
        print("   4. Try different USB port")
        return None
    
    if len(devices) == 1:
        print(f"✅ Found device: {devices[0]}")
        return devices[0]
    
    print(f"⚠️  Multiple devices found: {devices}")
    print("Using first device. Disconnect others if flashing fails.")
    return devices[0]

def flash_firmware(device=None):
    """Flash firmware to ESP32-S3"""
    if not device:
        device = find_esp32_device()
        if not device:
            return False
    
    print(f"\n⚡ Flashing to {device}...")
    
    # Flash command
    cmd = f"python3 -m esptool --chip esp32s3 -p {device} -b 460800 --before default_reset --after hard_reset write_flash --flash_mode dio --flash_size 2MB --flash_freq 80m 0x0 build/bootloader/bootloader.bin 0x8000 build/partition_table/partition-table.bin 0x10000 build/esp32_face.bin"
    
    success, output = run_cmd(cmd, "Flashing firmware")
    
    if not success:
        print("❌ Flash failed. Try:")
        print("   1. Hold BOOT button while connecting USB")
        print("   2. Press RESET button after connecting")
        print("   3. Try lower baud rate: -b 115200")
        print("   4. Check device permissions: sudo chmod 666 /dev/ttyUSB0")
        return False
    
    print("✅ Flash successful!")
    print("\n🎉 Setup complete! Your ESP32-S3 should now show the animated face.")
    print("   Bluetooth device: 'ESP32-AudioFace'")
    return True

def main():
    """Main setup process"""
    print("🎭 ESP32-S3 Audio Face - Complete Setup")
    print("=" * 50)
    
    if not check_prerequisites():
        sys.exit(1)
    
    if not build_firmware():
        sys.exit(1)
    
    if not flash_firmware():
        sys.exit(1)
    
    print("\n🚀 All done! Connect to 'ESP32-AudioFace' via Bluetooth to test.")

if __name__ == "__main__":
    main()
