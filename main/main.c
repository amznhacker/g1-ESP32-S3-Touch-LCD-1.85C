#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <nvs_flash.h>

static const char* TAG = "ESP32_FACE";

// Simple face animation variables
static int animation_counter = 0;

// Face animation task
static void face_animation_task(void *pvParameters) {
    int blink_counter = 0;
    
    while (1) {
        blink_counter++;
        animation_counter++;
        
        // Simple console-based "face" animation
        if (blink_counter % 20 == 0) {
            if (animation_counter % 100 < 50) {
                ESP_LOGI(TAG, "Face: ^_^ (Happy - Frame %d)", animation_counter);
            } else {
                ESP_LOGI(TAG, "Face: -_- (Sleepy - Frame %d)", animation_counter);
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "ESP32-S3 Face Animation Starting...");
    
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    ESP_LOGI(TAG, "System initialized. Starting face animation...");
    
    // Start face animation task
    xTaskCreate(face_animation_task, "face_anim", 4096, NULL, 5, NULL);
    
    ESP_LOGI(TAG, "Face animation started! Watch the console for animated faces.");
    
    // Keep main task alive
    while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}