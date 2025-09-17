#include "bluetooth_face.h"
#include <math.h>
#include <string.h>

static const char *TAG = "BT_FACE";
static bool bt_connected = false;
static float current_audio_level = 0.0f;

// Audio analysis buffer
static int16_t audio_buffer[AUDIO_BUFFER_SIZE];
static size_t buffer_index = 0;

// A2DP callback for audio data
static void bt_a2d_sink_data_cb(const uint8_t *data, uint32_t len) {
    bluetooth_audio_callback(data, len);
}

// A2DP callback for connection state
static void bt_a2d_sink_state_cb(esp_a2dp_connection_state_t state, void *param) {
    esp_a2dp_cb_param_t *a2d = (esp_a2dp_cb_param_t *)(param);
    bluetooth_connection_callback(state, &a2d->conn_stat.remote_bda);
}

// GAP callback for device discovery
static void bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param) {
    switch (event) {
        case ESP_BT_GAP_DISC_RES_EVT:
            ESP_LOGI(TAG, "Device discovered");
            break;
        case ESP_BT_GAP_DISC_STATE_CHANGED_EVT:
            if (param->disc_st_chg.state == ESP_BT_GAP_DISCOVERY_STOPPED) {
                ESP_LOGI(TAG, "Discovery stopped");
            }
            break;
        default:
            break;
    }
}

// Initialize Bluetooth face system
void bluetooth_face_init(void) {
    esp_err_t ret;
    
    // Initialize face emoting system
    face_emote_init();
    
    // Initialize Bluetooth controller
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Bluetooth controller init failed: %s", esp_err_to_name(ret));
        return;
    }
    
    ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Bluetooth controller enable failed: %s", esp_err_to_name(ret));
        return;
    }
    
    // Initialize Bluedroid
    ret = esp_bluedroid_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Bluedroid init failed: %s", esp_err_to_name(ret));
        return;
    }
    
    ret = esp_bluedroid_enable();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Bluedroid enable failed: %s", esp_err_to_name(ret));
        return;
    }
    
    // Register GAP callback
    esp_bt_gap_register_callback(bt_gap_cb);
    
    // Initialize A2DP sink
    esp_a2d_register_callback(bt_a2d_sink_state_cb);
    esp_a2d_sink_register_data_callback(bt_a2d_sink_data_cb);
    esp_a2d_sink_init();
    
    // Set device name
    esp_bt_dev_set_device_name(BT_DEVICE_NAME);
    
    ESP_LOGI(TAG, "Bluetooth face system initialized");
}

// Start Bluetooth discoverable mode
void bluetooth_start_discoverable(void) {
    // Set discoverable and connectable mode
    esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
    
    ESP_LOGI(TAG, "Bluetooth device '%s' is now discoverable", BT_DEVICE_NAME);
    ESP_LOGI(TAG, "Connect your phone to see face animations!");
}

// Stop Bluetooth discoverable mode
void bluetooth_stop_discoverable(void) {
    esp_bt_gap_set_scan_mode(ESP_BT_NON_CONNECTABLE, ESP_BT_NON_DISCOVERABLE);
    ESP_LOGI(TAG, "Bluetooth discoverable mode stopped");
}

// Calculate audio level from samples
float calculate_audio_level(const int16_t *samples, size_t count) {
    if (count == 0) return 0.0f;
    
    float sum = 0.0f;
    for (size_t i = 0; i < count; i++) {
        float sample = (float)samples[i] / 32768.0f;  // Normalize to -1.0 to 1.0
        sum += sample * sample;  // RMS calculation
    }
    
    float rms = sqrtf(sum / count);
    return fminf(rms, 1.0f);  // Clamp to max 1.0
}

// Analyze audio data and update face emotions
void audio_analyze_and_emote(const uint8_t *data, uint32_t len) {
    // Convert bytes to 16-bit samples (assuming stereo 16-bit PCM)
    size_t sample_count = len / 4;  // 4 bytes per stereo sample (2 channels * 2 bytes)
    const int16_t *samples = (const int16_t *)data;
    
    // Mix stereo to mono and store in buffer
    for (size_t i = 0; i < sample_count && buffer_index < AUDIO_BUFFER_SIZE; i++) {
        // Mix left and right channels
        int32_t mixed = ((int32_t)samples[i * 2] + (int32_t)samples[i * 2 + 1]) / 2;
        audio_buffer[buffer_index++] = (int16_t)mixed;
    }
    
    // Analyze when buffer is full
    if (buffer_index >= AUDIO_ANALYSIS_WINDOW) {
        current_audio_level = calculate_audio_level(audio_buffer, AUDIO_ANALYSIS_WINDOW);
        
        // Update face emotion based on audio level
        face_update_audio_level(current_audio_level);
        face_animate_speaking(current_audio_level > 0.05f);
        
        // Shift buffer for continuous analysis
        memmove(audio_buffer, audio_buffer + AUDIO_ANALYSIS_WINDOW/2, 
                (AUDIO_BUFFER_SIZE - AUDIO_ANALYSIS_WINDOW/2) * sizeof(int16_t));
        buffer_index -= AUDIO_ANALYSIS_WINDOW/2;
        
        ESP_LOGD(TAG, "Audio level: %.3f", current_audio_level);
    }
}

// Bluetooth connection state callback
void bluetooth_connection_callback(esp_a2dp_connection_state_t state, esp_bd_addr_t *bd_addr) {
    switch (state) {
        case ESP_A2DP_CONNECTION_STATE_DISCONNECTED:
            bt_connected = false;
            face_update_emotion(FACE_SLEEPY);
            ESP_LOGI(TAG, "Bluetooth disconnected - Face going to sleep");
            break;
            
        case ESP_A2DP_CONNECTION_STATE_CONNECTING:
            face_update_emotion(FACE_SURPRISED);
            ESP_LOGI(TAG, "Bluetooth connecting - Face surprised!");
            break;
            
        case ESP_A2DP_CONNECTION_STATE_CONNECTED:
            bt_connected = true;
            face_update_emotion(FACE_HAPPY);
            ESP_LOGI(TAG, "Bluetooth connected - Face is happy!");
            ESP_LOGI(TAG, "Start playing music to see face animations!");
            break;
            
        case ESP_A2DP_CONNECTION_STATE_DISCONNECTING:
            face_update_emotion(FACE_NEUTRAL);
            ESP_LOGI(TAG, "Bluetooth disconnecting");
            break;
            
        default:
            break;
    }
}

// Bluetooth audio data callback
void bluetooth_audio_callback(const uint8_t *data, uint32_t len) {
    if (bt_connected && data && len > 0) {
        audio_analyze_and_emote(data, len);
    }
}