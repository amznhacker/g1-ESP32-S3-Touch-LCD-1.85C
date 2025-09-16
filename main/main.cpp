#include <Arduino.h>
#include <lvgl.h>
#include <TFT_eSPI.h>
#include <driver/i2s.h>
#include <BluetoothA2DPSink.h>

TFT_eSPI tft = TFT_eSPI();
BluetoothA2DPSink a2dp_sink;

// Face parameters
int eye_size = 20;
int mouth_width = 40;
int mouth_height = 10;
float audio_level = 0;

// Audio callback
void audio_data_callback(const uint8_t *data, uint32_t length) {
    // Calculate audio level (simplified RMS)
    int32_t sum = 0;
    for(int i = 0; i < length; i += 2) {
        int16_t sample = (data[i+1] << 8) | data[i];
        sum += abs(sample);
    }
    audio_level = (float)sum / (length/2) / 32768.0;
}

void draw_face() {
    tft.fillScreen(TFT_BLACK);
    
    // Eyes - blink based on audio
    int current_eye_size = eye_size * (0.5 + audio_level * 0.5);
    tft.fillCircle(120, 120, current_eye_size, TFT_WHITE);
    tft.fillCircle(240, 120, current_eye_size, TFT_WHITE);
    
    // Mouth - open based on audio level
    int current_mouth_height = mouth_height + (audio_level * 30);
    tft.fillEllipse(180, 220, mouth_width, current_mouth_height, TFT_RED);
}

void setup() {
    Serial.begin(115200);
    
    // Initialize display
    tft.init();
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);
    
    // Initialize Bluetooth A2DP
    a2dp_sink.set_stream_reader(audio_data_callback);
    a2dp_sink.start("ESP32_Face");
}

void loop() {
    draw_face();
    delay(50); // 20 FPS
}