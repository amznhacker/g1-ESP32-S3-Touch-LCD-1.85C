#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <driver/gpio.h>

static const char* TAG = "ESP32_TEST";

// Test multiple possible backlight pins
int test_pins[] = {15, 38, 45, 48, 21, 14, 2, 4, 16, 17};
int num_pins = sizeof(test_pins) / sizeof(test_pins[0]);

void app_main(void) {
    ESP_LOGI(TAG, "=== ESP32-S3 Multi-GPIO Test ===");
    
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // Configure all test pins as output
    for (int i = 0; i < num_pins; i++) {
        gpio_config_t gpio_conf = {
            .pin_bit_mask = (1ULL << test_pins[i]),
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE
        };
        gpio_config(&gpio_conf);
        gpio_set_level(test_pins[i], 0); // Start LOW
    }
    
    ESP_LOGI(TAG, "✅ Testing pins: 15,38,45,48,21,14,2,4,16,17");
    ESP_LOGI(TAG, "✅ Watch LCD for brightness changes!");
    
    int counter = 0;
    
    while(1) {
        counter++;
        
        // Test each pin one by one
        for (int i = 0; i < num_pins; i++) {
            // Turn all pins OFF first
            for (int j = 0; j < num_pins; j++) {
                gpio_set_level(test_pins[j], 0);
            }
            
            // Turn current pin ON
            gpio_set_level(test_pins[i], 1);
            
            ESP_LOGI(TAG, "Testing GPIO %d = HIGH (others LOW) - Round %d", 
                     test_pins[i], counter);
            
            vTaskDelay(pdMS_TO_TICKS(1000)); // Hold for 1 second
        }
        
        // All pins OFF for 1 second
        for (int j = 0; j < num_pins; j++) {
            gpio_set_level(test_pins[j], 0);
        }
        ESP_LOGI(TAG, "All pins LOW - Round %d complete", counter);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}