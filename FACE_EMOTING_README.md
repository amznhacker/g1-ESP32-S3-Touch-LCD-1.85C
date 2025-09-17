# ESP32-S3 Bluetooth Face Animation

## 🎭 What's New

This enhanced firmware transforms your ESP32-S3 Touch LCD into an **animated face that emotes based on Bluetooth audio**!

### ✨ Features

- **🎵 Real-time Audio Analysis**: Face reacts to music volume and intensity
- **📱 Bluetooth A2DP Sink**: Connect any phone/device as "ESP32_Face"
- **😊 Emotional States**: 4 different face emotions based on audio levels
- **🔊 Speaker Integration**: Uses existing PCM5101 audio driver
- **📺 Console Animations**: Watch face emotions in real-time via serial monitor

## 🎯 Face Emotions

| Audio Level | Emotion | Display |
|-------------|---------|---------|
| No Connection | 😴 Sleeping | `Face: 😴 Sleeping (No Bluetooth)` |
| Connected, No Audio | 😊 Awake | `Face: 😊 Awake (Connected)` |
| Low-Mid Audio | 😮 Speaking | `Face: 😮 Speaking ♪ (Audio: 0.15)` |
| High Audio | 🤩 Excited | `Face: 🤩 EXCITED! ♪♪♪ (Audio: 0.65)` |

## 🚀 Quick Start

### 1. Flash the Firmware
```bash
# Ubuntu/Linux
./setup.sh
python3 auto_flash.py

# Or use existing flash method
./flash.sh
```

### 2. Connect via Bluetooth
1. **Enable Bluetooth** on your phone
2. **Search for devices** - look for "**ESP32_Face**"
3. **Connect** to ESP32_Face
4. **Play music** from any app (Spotify, YouTube, etc.)

### 3. Watch the Magic! 🎪
- **Console Output**: Open serial monitor to see face emotions
- **Speaker Audio**: Music plays through the built-in speaker
- **Real-time Reactions**: Face changes based on audio intensity

## 📊 Console Output Example

```
🚀 ESP32-S3 Bluetooth Face Animation Starting...
🎯 ESP32_Face Bluetooth initialized
📱 Device name: ESP32_Face
🔍 Now discoverable - connect your phone!
✨ ESP32_Face is ready!

🔄 Bluetooth CONNECTING...
✅ Bluetooth CONNECTED!
🎵 Play music to see face animations!
Face: 😊 Awake (Connected)
Face: 😮 Speaking ♪ (Audio: 0.12)
Face: 🤩 EXCITED! ♪♪♪ (Audio: 0.58)
Face: 😮 Speaking ♪ (Audio: 0.23)
```

## 🔧 Technical Details

### Audio Analysis
- **Sample Rate**: 44.1kHz stereo
- **Analysis Window**: 256 samples
- **Update Rate**: Real-time (every audio packet)
- **Level Calculation**: RMS (Root Mean Square) for accurate volume detection

### Bluetooth Configuration
- **Profile**: A2DP Sink (Advanced Audio Distribution Profile)
- **Codec**: SBC (standard Bluetooth audio codec)
- **Device Name**: "ESP32_Face"
- **Discoverable**: Always when powered on

### Face State Thresholds
```c
// Audio level thresholds
if (audio_level > 0.3f)      -> EXCITED 🤩
else if (audio_level > 0.05f) -> SPEAKING 😮  
else if (connected)          -> AWAKE 😊
else                         -> SLEEPING 😴
```

## 🎨 Customization

### Change Device Name
Edit in `main.c`:
```c
esp_bt_dev_set_device_name("YourCustomName");
```

### Adjust Audio Sensitivity
Modify thresholds in `update_face_state()`:
```c
if (audio_level > 0.2f) {        // Lower = more sensitive
    new_state = FACE_EXCITED;
}
```

### Add More Emotions
Extend the `simple_face_state_t` enum and add cases in `update_face_state()`.

## 🛠 Hardware Requirements

- **ESP32-S3-Touch-LCD-1.85** board
- **Built-in speaker** (PCM5101 driver)
- **Bluetooth antenna** (integrated)
- **Power supply** (USB-C or battery)

## 📱 Compatible Devices

Works with any Bluetooth-enabled device:
- **📱 Smartphones**: iPhone, Android
- **💻 Computers**: Windows, Mac, Linux
- **🎵 Music Players**: iPod, dedicated MP3 players
- **📺 Smart TVs**: Most modern smart TVs

## 🔍 Troubleshooting

### Can't Find "ESP32_Face"
1. Check serial monitor for "Now discoverable" message
2. Reset ESP32 and wait 10 seconds
3. Clear Bluetooth cache on phone

### No Audio Through Speaker
1. Verify connection in serial monitor
2. Check volume on phone (should be >50%)
3. Try different music app

### Face Not Reacting
1. Ensure music is actually playing
2. Check audio levels in console: `Audio: 0.xxx`
3. Try louder music or adjust sensitivity

## 🎉 What's Next?

This is just the beginning! Future enhancements could include:

- **🖥 Visual Face Display**: Actual animated face on LCD screen
- **🎨 Custom Animations**: Blinking, winking, mouth movements
- **🎵 Beat Detection**: Sync animations to music beats
- **📊 Spectrum Analysis**: Different emotions for different frequencies
- **🎮 Interactive Features**: Touch controls, gesture recognition

## 🤝 Contributing

Want to add more features? The code is modular and easy to extend:

- `main.c`: Core Bluetooth and face logic
- `face_emote.*`: Advanced face animation system (ready for LCD integration)
- `bluetooth_face.*`: Full-featured Bluetooth audio analysis

## 📄 License

This project builds upon the original ESP32-S3-Touch-LCD demo and adds Bluetooth face animation capabilities.

---

**🎭 Enjoy your new animated ESP32 face! 🎭**

*Connect, play music, and watch your ESP32 come to life!*