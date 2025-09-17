# ESP32-S3 Screen Flash Fix

## What was fixed:
- Added LCD backlight initialization using LEDC driver
- Screen now flashes based on audio levels from Bluetooth A2DP
- Removed unicode characters for cross-platform compatibility

## How it works:
- **No connection**: Screen dim (5% brightness)
- **Connected, no audio**: Medium brightness (30%)
- **Low audio (0.05-0.1)**: 40% brightness
- **Medium audio (0.1-0.3)**: 70% brightness  
- **High audio (0.3+)**: 100% brightness (full flash)

## Hardware:
- Uses GPIO 5 for backlight control (LEDC PWM)
- 13-bit resolution, 5kHz frequency

## Build & Flash:
```bash
./run.bat  # Windows with Docker
```

## Test:
```bash
python test_flash.py  # Validates flash logic
```