#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <driver/gpio.h>

static const char* TAG = "ESP32_FACE";

// Simple GPIO test - blink backlight to verify hardware
#define PIN_NUM_BK_LIGHT 15

void app_main(void) {
    ESP_LOGI(TAG, "ESP32-S3 LCD Test Starting...");
    
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // Configure backlight pin
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << PIN_NUM_BK_LIGHT
    };
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));
    
    ESP_LOGI(TAG, "Blinking backlight to test LCD...");
    
    // Blink backlight to test if LCD responds
    int counter = 0;
    while(1) {
        counter++;
        
        // Turn backlight on/off every second
        if (counter % 10 == 0) {
            gpio_set_level(PIN_NUM_BK_LIGHT, 1);
            ESP_LOGI(TAG, "Backlight ON - Frame %d", counter/10);
        } else if (counter % 10 == 5) {
            gpio_set_level(PIN_NUM_BK_LIGHT, 0);
            ESP_LOGI(TAG, "Backlight OFF");
        }
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}