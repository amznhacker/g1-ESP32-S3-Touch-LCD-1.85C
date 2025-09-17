#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_a2dp_api.h>
#include <esp_avrc_api.h>
#include <driver/spi_master.h>
#include <driver/gpio.h>

static const char* TAG = "ESP32_FACE";

// Display pins (adjust for your board)
#define LCD_MOSI 11
#define LCD_SCLK 12
#define LCD_CS   10
#define LCD_DC   14
#define LCD_RST  13

// Face parameters
static int eye_size = 20;
static int mouth_width = 40;
static int mouth_height = 10;
static float audio_level = 0.0;

// Simple display buffer (360x360 would be too large, use smaller)
#define DISPLAY_WIDTH 180
#define DISPLAY_HEIGHT 180
static uint16_t display_buffer[DISPLAY_WIDTH * DISPLAY_HEIGHT];

// Colors (RGB565)
#define COLOR_BLACK 0x0000
#define COLOR_WHITE 0xFFFF
#define COLOR_RED   0xF800

void draw_pixel(int x, int y, uint16_t color) {
    if (x >= 0 && x < DISPLAY_WIDTH && y >= 0 && y < DISPLAY_HEIGHT) {
        display_buffer[y * DISPLAY_WIDTH + x] = color;
    }
}

void draw_circle(int cx, int cy, int radius, uint16_t color) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x*x + y*y <= radius*radius) {
                draw_pixel(cx + x, cy + y, color);
            }
        }
    }
}

void draw_face() {
    // Clear screen
    for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++) {
        display_buffer[i] = COLOR_BLACK;
    }
    
    // Eyes - size based on audio
    int current_eye_size = eye_size * (0.5 + audio_level * 0.5);
    draw_circle(60, 60, current_eye_size, COLOR_WHITE);
    draw_circle(120, 60, current_eye_size, COLOR_WHITE);
    
    // Mouth - height based on audio level
    int current_mouth_height = mouth_height + (audio_level * 15);
    for (int i = 0; i < mouth_width; i++) {
        for (int j = 0; j < current_mouth_height; j++) {
            draw_pixel(90 - mouth_width/2 + i, 120 + j, COLOR_RED);
        }
    }
}

// Bluetooth A2DP callback
void bt_a2d_sink_data_cb(const uint8_t *data, uint32_t len) {
    // Calculate simple audio level
    int32_t sum = 0;
    for (uint32_t i = 0; i < len; i += 2) {
        int16_t sample = (data[i+1] << 8) | data[i];
        sum += abs(sample);
    }
    audio_level = (float)sum / (len/2) / 32768.0;
    if (audio_level > 1.0) audio_level = 1.0;
}

void bt_a2d_sink_state_cb(esp_a2d_connection_state_t state, esp_bd_addr_t remote_bda) {
    if (state == ESP_A2D_CONNECTION_STATE_CONNECTED) {
        ESP_LOGI(TAG, "Bluetooth connected");
    } else if (state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
        ESP_LOGI(TAG, "Bluetooth disconnected");
    }
}

void init_bluetooth() {
    esp_err_t ret = esp_bt_controller_mem_release(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(TAG, "Bluetooth controller release failed: %s", esp_err_to_name(ret));
        return;
    }

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(TAG, "Bluetooth controller init failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT);
    if (ret) {
        ESP_LOGE(TAG, "Bluetooth controller enable failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(TAG, "Bluedroid init failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(TAG, "Bluedroid enable failed: %s", esp_err_to_name(ret));
        return;
    }

    esp_a2d_register_callback(bt_a2d_sink_state_cb);
    esp_a2d_sink_register_data_callback(bt_a2d_sink_data_cb);
    esp_a2d_sink_init();

    esp_bt_dev_set_device_name("ESP32_Face");
    esp_a2d_sink_connect_last_device();
    
    ESP_LOGI(TAG, "Bluetooth A2DP sink initialized");
}

void face_task(void *pvParameters) {
    while (1) {
        draw_face();
        vTaskDelay(pdMS_TO_TICKS(50)); // 20 FPS
    }
}

extern "C" void app_main() {
    ESP_LOGI(TAG, "ESP32 Animated Face starting...");
    
    // Initialize Bluetooth
    init_bluetooth();
    
    // Create face animation task
    xTaskCreate(face_task, "face_task", 4096, NULL, 5, NULL);
    
    ESP_LOGI(TAG, "Face animation started. Connect to 'ESP32_Face' via Bluetooth");
}