#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <esp_timer.h>
#include <esp_random.h>
#include <driver/gpio.h>

static const char* TAG = "ESP32S3_DEMO";

// Animation state
typedef struct {
    float eye_blink;
    float mouth_smile;
    float head_tilt;
    float energy_level;
    uint32_t frame_count;
    uint64_t last_update;
} face_state_t;

static face_state_t face = {0};
static QueueHandle_t sensor_queue;

// Sensor data structure
typedef struct {
    float temperature;
    float light_level;
    bool motion_detected;
} sensor_data_t;

// Face expressions
typedef enum {
    EXPR_NEUTRAL,
    EXPR_HAPPY,
    EXPR_SURPRISED,
    EXPR_SLEEPY,
    EXPR_EXCITED
} expression_t;

static expression_t current_expression = EXPR_NEUTRAL;
static const char* expression_names[] = {"Neutral", "Happy", "Surprised", "Sleepy", "Excited"};

// Advanced animation with realistic timing
static void update_face_animation(void) {
    uint64_t now = esp_timer_get_time();
    float dt = (now - face.last_update) / 1000000.0f; // Convert to seconds
    face.last_update = now;
    face.frame_count++;
    
    // Realistic blinking pattern
    static float blink_timer = 0;
    blink_timer += dt;
    if (blink_timer > 3.0f + (esp_random() % 2000) / 1000.0f) {
        face.eye_blink = 1.0f;
        blink_timer = 0;
    } else if (face.eye_blink > 0) {
        face.eye_blink -= dt * 8.0f; // Fast blink
        if (face.eye_blink < 0) face.eye_blink = 0;
    }
    
    // Expression-based mouth animation
    switch (current_expression) {
        case EXPR_HAPPY:
            face.mouth_smile = 0.8f + 0.2f * sinf(face.frame_count * 0.1f);
            break;
        case EXPR_SURPRISED:
            face.mouth_smile = -0.5f;
            break;
        case EXPR_SLEEPY:
            face.mouth_smile = 0.1f * sinf(face.frame_count * 0.05f);
            face.eye_blink = 0.7f;
            break;
        case EXPR_EXCITED:
            face.mouth_smile = 0.9f + 0.1f * sinf(face.frame_count * 0.3f);
            face.head_tilt = 0.3f * sinf(face.frame_count * 0.2f);
            break;
        default:
            face.mouth_smile = 0.1f + 0.1f * sinf(face.frame_count * 0.08f);
            break;
    }
    
    // Energy-based head movement
    face.head_tilt += (face.energy_level - 0.5f) * dt * 0.5f;
    face.head_tilt *= 0.95f; // Damping
}

// Simulate sensor readings (would be real sensors on actual hardware)
static void sensor_task(void *pvParameters) {
    sensor_data_t data;
    
    while (1) {
        // Simulate temperature sensor (25-35°C range)
        data.temperature = 25.0f + (esp_random() % 1000) / 100.0f;
        
        // Simulate light sensor (0-100% range)
        data.light_level = (esp_random() % 100) / 100.0f;
        
        // Simulate motion detection
        data.motion_detected = (esp_random() % 100) < 10; // 10% chance
        
        xQueueSend(sensor_queue, &data, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// AI-like behavior based on sensor input
static void behavior_task(void *pvParameters) {
    sensor_data_t sensor_data;
    static uint32_t behavior_timer = 0;
    
    while (1) {
        if (xQueueReceive(sensor_queue, &sensor_data, pdMS_TO_TICKS(100))) {
            // React to environment
            if (sensor_data.motion_detected) {
                current_expression = EXPR_SURPRISED;
                face.energy_level = 0.9f;
                ESP_LOGI(TAG, "[MOTION] Motion detected! Expression: %s", expression_names[current_expression]);
            } else if (sensor_data.light_level < 0.2f) {
                current_expression = EXPR_SLEEPY;
                face.energy_level = 0.2f;
                ESP_LOGI(TAG, "[LIGHT] Low light detected! Expression: %s", expression_names[current_expression]);
            } else if (sensor_data.temperature > 30.0f) {
                current_expression = EXPR_EXCITED;
                face.energy_level = 0.8f;
                ESP_LOGI(TAG, "[TEMP] High temperature! Expression: %s", expression_names[current_expression]);
            }
            
            ESP_LOGI(TAG, "[SENSORS] Temp=%.1f°C, Light=%.0f%%, Motion=%s", 
                     sensor_data.temperature, sensor_data.light_level * 100,
                     sensor_data.motion_detected ? "YES" : "NO");
        }
        
        // Autonomous behavior changes
        behavior_timer++;
        if (behavior_timer % 100 == 0) {
            if (current_expression != EXPR_SURPRISED) {
                expression_t new_expr = (esp_random() % 4) + 1; // Random expression
                if (new_expr != current_expression) {
                    current_expression = new_expr;
                    face.energy_level = 0.3f + (esp_random() % 50) / 100.0f;
                    ESP_LOGI(TAG, "[AUTO] Autonomous change: %s", expression_names[current_expression]);
                }
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// Performance monitoring
static void performance_task(void *pvParameters) {
    uint32_t last_frame_count = 0;
    
    while (1) {
        uint32_t fps = face.frame_count - last_frame_count;
        last_frame_count = face.frame_count;
        
        size_t free_heap = esp_get_free_heap_size();
        size_t min_free_heap = esp_get_minimum_free_heap_size();
        
        ESP_LOGI(TAG, "[PERF] Performance: FPS=%lu, Heap=%zu/%zu bytes", 
                 fps, free_heap, min_free_heap);
        
        // Display face state with visual representation
        char blink_bar[11], smile_bar[11], tilt_bar[11], energy_bar[11];
        
        // Create visual bars for each parameter
        int blink_level = (int)(face.eye_blink * 10);
        int smile_level = (int)((face.mouth_smile + 1.0f) * 5); // -1 to 1 -> 0 to 10
        int tilt_level = (int)((face.head_tilt + 1.0f) * 5);   // -1 to 1 -> 0 to 10
        int energy_level = (int)(face.energy_level * 10);
        
        for (int i = 0; i < 10; i++) {
            blink_bar[i] = (i < blink_level) ? '#' : '.';
            smile_bar[i] = (i < smile_level) ? '#' : '.';
            tilt_bar[i] = (i < tilt_level) ? '#' : '.';
            energy_bar[i] = (i < energy_level) ? '#' : '.';
        }
        blink_bar[10] = smile_bar[10] = tilt_bar[10] = energy_bar[10] = '\0';
        
        ESP_LOGI(TAG, "[FACE] Face State: %s", expression_names[current_expression]);
        ESP_LOGI(TAG, "   Blink:  [%s] %.2f", blink_bar, face.eye_blink);
        ESP_LOGI(TAG, "   Smile:  [%s] %.2f", smile_bar, face.mouth_smile);
        ESP_LOGI(TAG, "   Tilt:   [%s] %.2f", tilt_bar, face.head_tilt);
        ESP_LOGI(TAG, "   Energy: [%s] %.2f", energy_bar, face.energy_level);
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// Main animation loop with simulated display rendering
static void animation_task(void *pvParameters) {
    face.last_update = esp_timer_get_time();
    
    while (1) {
        update_face_animation();
        
        // Simulate display rendering
        if (face.frame_count % 30 == 0) {
            ESP_LOGI(TAG, "[RENDER] Frame %lu: Rendering %s on 240x240 LCD", 
                     face.frame_count, expression_names[current_expression]);
        }
        
        vTaskDelay(pdMS_TO_TICKS(33)); // ~30 FPS
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "ESP32-S3 Advanced Face Animation Demo");
    ESP_LOGI(TAG, "Features: AI Behavior, Sensor Fusion, Real-time Animation");
    ESP_LOGI(TAG, "Target: 240x240 Touch LCD with 30 FPS Animation");
    ESP_LOGI(TAG, "Multi-core: Animation on Core 1, AI/Sensors on Core 0");
    
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // Create sensor data queue
    sensor_queue = xQueueCreate(10, sizeof(sensor_data_t));
    if (sensor_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create sensor queue");
        return;
    }
    
    // Initialize face state
    face.energy_level = 0.5f;
    current_expression = EXPR_NEUTRAL;
    
    ESP_LOGI(TAG, "System initialized - Starting multi-core tasks");
    
    // Create tasks on different cores for optimal performance
    BaseType_t result;
    
    result = xTaskCreatePinnedToCore(animation_task, "animation", 4096, NULL, 5, NULL, 1);
    if (result != pdPASS) ESP_LOGE(TAG, "Failed to create animation task");
    
    result = xTaskCreatePinnedToCore(sensor_task, "sensors", 2048, NULL, 3, NULL, 0);
    if (result != pdPASS) ESP_LOGE(TAG, "Failed to create sensor task");
    
    result = xTaskCreatePinnedToCore(behavior_task, "behavior", 3072, NULL, 4, NULL, 0);
    if (result != pdPASS) ESP_LOGE(TAG, "Failed to create behavior task");
    
    result = xTaskCreatePinnedToCore(performance_task, "performance", 2048, NULL, 2, NULL, 1);
    if (result != pdPASS) ESP_LOGE(TAG, "Failed to create performance task");
    
    ESP_LOGI(TAG, "Advanced face animation system active!");
    ESP_LOGI(TAG, "AI behaviors: Motion response, Light adaptation, Temperature reaction");
    ESP_LOGI(TAG, "Watch the console for real-time sensor data and performance metrics");
    ESP_LOGI(TAG, "Visual bars show real-time face parameters");
}