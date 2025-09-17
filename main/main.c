#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <driver/gpio.h>

static const char* TAG = "ESP32_TEST";

// Test GPIO (backlight pin)
#define TEST_GPIO 15

void app_main(void) {
    ESP_LOGI(TAG, "=== ESP32-S3 GPIO Test ===");
    
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // Configure GPIO
    gpio_config_t gpio_conf = {
        .pin_bit_mask = (1ULL << TEST_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&gpio_conf);
    
    ESP_LOGI(TAG, "✅ GPIO %d configured as output", TEST_GPIO);
    ESP_LOGI(TAG, "✅ Watch for LCD backlight blinking!");
    
    int counter = 0;
    bool led_state = false;
    
    while(1) {
        counter++;
        led_state = !led_state;
        
        gpio_set_level(TEST_GPIO, led_state);
        
        ESP_LOGI(TAG, "GPIO %d = %s (Count: %d)", 
                 TEST_GPIO, led_state ? "HIGH" : "LOW", counter);
        
        vTaskDelay(pdMS_TO_TICKS(500)); // Blink every 0.5 seconds
    }
}