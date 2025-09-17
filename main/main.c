#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_bt_device.h>
#include <esp_a2dp_api.h>
#include <esp_avrc_api.h>

static const char* TAG = "ESP32_FACE";

// Simple face animation variables
static float audio_level = 0.0;
static int animation_counter = 0;

// Bluetooth A2DP callback for audio data
static void bt_a2d_sink_data_cb(const uint8_t *data, uint32_t len) {
    // Calculate simple audio level from PCM data
    int32_t sum = 0;
    for (uint32_t i = 0; i < len && i < 1024; i += 2) {
        int16_t sample = (data[i+1] << 8) | data[i];
        sum += abs(sample);
    }
    audio_level = (float)sum / 512.0 / 32768.0;
    if (audio_level > 1.0) audio_level = 1.0;
    
    // Simple animation based on audio
    animation_counter++;
    if (animation_counter % 10 == 0) {
        ESP_LOGI(TAG, "Audio Level: %.2f", audio_level);
    }
}

// Bluetooth A2DP callback
static void bt_a2d_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param) {
    switch (event) {
        case ESP_A2D_CONNECTION_STATE_EVT:
            if (param->conn_stat.state == ESP_A2D_CONNECTION_STATE_CONNECTED) {
                ESP_LOGI(TAG, "Bluetooth A2DP Connected");
            } else if (param->conn_stat.state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
                ESP_LOGI(TAG, "Bluetooth A2DP Disconnected");
            }
            break;
        default:
            break;
    }
}

// Initialize Bluetooth A2DP
static void init_bluetooth(void) {
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));
    
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT));
    
    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());
    
    esp_a2d_register_callback(bt_a2d_cb);
    esp_a2d_sink_register_data_callback(bt_a2d_sink_data_cb);
    ESP_ERROR_CHECK(esp_a2d_sink_init());
    
    ESP_ERROR_CHECK(esp_bt_dev_set_device_name("ESP32_Face"));
    
    ESP_LOGI(TAG, "Bluetooth initialized. Device name: ESP32_Face");
}

// Face animation task
static void face_animation_task(void *pvParameters) {
    int blink_counter = 0;
    
    while (1) {
        blink_counter++;
        
        // Simple console-based "face" animation
        if (blink_counter % 20 == 0) {
            if (audio_level > 0.1) {
                ESP_LOGI(TAG, "Face: O_O  ~~~♪ (Audio: %.2f)", audio_level);
            } else {
                ESP_LOGI(TAG, "Face: -_-  (Quiet)");
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "ESP32-S3 Animated Face Starting...");
    
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // Initialize Bluetooth
    init_bluetooth();
    
    // Start face animation task
    xTaskCreate(face_animation_task, "face_anim", 4096, NULL, 5, NULL);
    
    ESP_LOGI(TAG, "System ready! Connect your phone to 'ESP32_Face' and play audio.");
}