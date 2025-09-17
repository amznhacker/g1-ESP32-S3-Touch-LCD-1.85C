#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <nvs_flash.h>

static const char* TAG = "ESP32_TEST";

void app_main(void) {
    ESP_LOGI(TAG, "=== ESP32-S3 Basic Test ===");
    
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    ESP_LOGI(TAG, "✅ NVS initialized");
    ESP_LOGI(TAG, "✅ ESP32-S3 is running!");
    ESP_LOGI(TAG, "✅ Serial communication works!");
    
    int counter = 0;
    while(1) {
        counter++;
        ESP_LOGI(TAG, "Heartbeat %d - System is alive!", counter);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}