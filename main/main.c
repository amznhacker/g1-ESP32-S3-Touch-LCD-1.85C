#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_a2dp_api.h>
#include <esp_gap_bt_api.h>
#include <math.h>
#include "driver/ledc.h"

static const char *TAG = "ESP32_FACE";

// LCD Backlight pins and settings
#define BACKLIGHT_PIN 5
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_DUTY_RES LEDC_TIMER_13_BIT
#define LEDC_FREQUENCY 5000

// Simple face state
typedef enum {
    FACE_SLEEPING,    // No connection
    FACE_AWAKE,       // Connected but no audio
    FACE_SPEAKING,    // Audio playing
    FACE_EXCITED      // High audio level
} simple_face_state_t;

static simple_face_state_t current_face_state = FACE_SLEEPING;
static bool bt_connected = false;
static float audio_level = 0.0f;
static bool screen_initialized = false;

// Simple audio analysis
static float analyze_audio_simple(const uint8_t *data, uint32_t len) {
    if (len < 4) return 0.0f;
    
    const int16_t *samples = (const int16_t *)data;
    size_t sample_count = len / 4; // Stereo 16-bit
    
    float sum = 0.0f;
    for (size_t i = 0; i < sample_count && i < 256; i++) {
        // Mix stereo to mono and normalize
        float sample = ((float)samples[i*2] + (float)samples[i*2+1]) / 65536.0f;
        sum += fabsf(sample);
    }
    
    return sum / fminf(sample_count, 256.0f);
}

// Initialize LCD backlight
static void init_backlight(void) {
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_DUTY_RES,
        .freq_hz = LEDC_FREQUENCY,
        .speed_mode = LEDC_MODE,
        .timer_num = LEDC_TIMER,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t ledc_channel = {
        .channel = LEDC_CHANNEL,
        .duty = 0,
        .gpio_num = BACKLIGHT_PIN,
        .speed_mode = LEDC_MODE,
        .timer_sel = LEDC_TIMER,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    
    screen_initialized = true;
    ESP_LOGI(TAG, "LCD Backlight initialized");
}

// Set backlight brightness (0-100)
static void set_backlight(uint8_t brightness) {
    if (!screen_initialized) return;
    
    uint32_t duty = (8191 * brightness) / 100; // Convert to 13-bit duty cycle
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
}

// Flash screen based on audio level
static void flash_screen(float level) {
    if (!screen_initialized) return;
    
    uint8_t brightness;
    if (level > 0.3f) {
        brightness = 100; // Full brightness for high audio
    } else if (level > 0.1f) {
        brightness = 70;  // Medium brightness
    } else if (level > 0.05f) {
        brightness = 40;  // Low brightness
    } else {
        brightness = 10;  // Dim when no audio
    }
    
    set_backlight(brightness);
}

// Update face state based on audio
static void update_face_state(void) {
    simple_face_state_t new_state;
    
    if (!bt_connected) {
        new_state = FACE_SLEEPING;
        set_backlight(5); // Very dim when sleeping
    } else if (audio_level > 0.3f) {
        new_state = FACE_EXCITED;
        flash_screen(audio_level);
    } else if (audio_level > 0.05f) {
        new_state = FACE_SPEAKING;
        flash_screen(audio_level);
    } else {
        new_state = FACE_AWAKE;
        set_backlight(30); // Medium brightness when awake
    }
    
    if (new_state != current_face_state) {
        current_face_state = new_state;
        
        switch (current_face_state) {
            case FACE_SLEEPING:
                ESP_LOGI(TAG, "Sleeping (No Bluetooth)");
                break;
            case FACE_AWAKE:
                ESP_LOGI(TAG, "Awake (Connected)");
                break;
            case FACE_SPEAKING:
                ESP_LOGI(TAG, "Speaking (Audio: %.2f)", audio_level);
                break;
            case FACE_EXCITED:
                ESP_LOGI(TAG, "EXCITED! (Audio: %.2f)", audio_level);
                break;
        }
    }
}

// A2DP audio data callback
static void bt_a2d_sink_data_cb(const uint8_t *data, uint32_t len) {
    if (bt_connected && data && len > 0) {
        audio_level = analyze_audio_simple(data, len);
        update_face_state();
    }
}

// A2DP callback
static void bt_a2d_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param) {
    switch (event) {
        case ESP_A2D_CONNECTION_STATE_EVT:
            switch (param->conn_stat.state) {
                case ESP_A2D_CONNECTION_STATE_DISCONNECTED:
                    bt_connected = false;
                    audio_level = 0.0f;
                    ESP_LOGI(TAG, "Bluetooth DISCONNECTED");
                    break;
                case ESP_A2D_CONNECTION_STATE_CONNECTED:
                    bt_connected = true;
                    ESP_LOGI(TAG, "Bluetooth CONNECTED!");
                    ESP_LOGI(TAG, "Play music to see screen flash!");
                    break;
                default:
                    break;
            }
            update_face_state();
            break;
        default:
            break;
    }
}

// Initialize Bluetooth A2DP sink
static void bluetooth_init(void) {
    // Initialize Bluetooth controller
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT));
    
    // Initialize Bluedroid
    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());
    
    // Set device name
    esp_bt_gap_set_device_name("ESP32_Face");
    
    // Initialize A2DP sink
    esp_a2d_register_callback(bt_a2d_cb);
    esp_a2d_sink_register_data_callback(bt_a2d_sink_data_cb);
    esp_a2d_sink_init();
    
    // Set discoverable and connectable
    esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
    
    ESP_LOGI(TAG, "ESP32_Face Bluetooth initialized");
    ESP_LOGI(TAG, "Device name: ESP32_Face");
    ESP_LOGI(TAG, "Now discoverable - connect your phone!");
}

// Face animation task
static void face_animation_task(void *param) {
    while (1) {
        // Decay audio level gradually
        if (audio_level > 0.01f) {
            audio_level *= 0.95f;
            update_face_state();
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "ESP32-S3 Bluetooth Face Animation Starting...");
    
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // Initialize LCD backlight for screen flashing
    init_backlight();
    
    // Initialize Bluetooth
    bluetooth_init();
    
    // Start face animation task
    xTaskCreate(face_animation_task, "face_anim", 2048, NULL, 5, NULL);
    
    ESP_LOGI(TAG, "ESP32_Face is ready!");
    ESP_LOGI(TAG, "Instructions:");
    ESP_LOGI(TAG, "1. Connect phone to 'ESP32_Face' via Bluetooth");
    ESP_LOGI(TAG, "2. Play music to see screen flash with audio");
    ESP_LOGI(TAG, "3. Watch console for status!");
    
    // Main loop
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        
        // Periodic status update
        static int status_counter = 0;
        if (++status_counter >= 10) { // Every 10 seconds
            status_counter = 0;
            ESP_LOGI(TAG, "Status: %s, Audio: %.3f", 
                     bt_connected ? "Connected" : "Waiting for connection", 
                     audio_level);
        }
    }
}