#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <driver/gpio.h>

static const char* TAG = "ESP32_TEST";

// CORRECT pins from official demo
#define LCD_BL_PIN    5   // Backlight - from ST77916.h
#define LCD_RST_PIN   -1  // Reset (controlled by EXIO2)
#define LCD_CS_PIN    21  // Chip Select
#define LCD_SCK_PIN   40  // SPI Clock

void app_main(void) {
    ESP_LOGI(TAG, "=== ESP32-S3 CORRECT LCD Test ===");
    
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // Configure backlight pin (GPIO 5)
    gpio_config_t gpio_conf = {
        .pin_bit_mask = (1ULL << LCD_BL_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&gpio_conf);
    
    ESP_LOGI(TAG, "✅ OFFICIAL DEMO PINS:");
    ESP_LOGI(TAG, "✅ Backlight: GPIO %d", LCD_BL_PIN);
    ESP_LOGI(TAG, "✅ CS: GPIO %d", LCD_CS_PIN);
    ESP_LOGI(TAG, "✅ SCK: GPIO %d", LCD_SCK_PIN);
    ESP_LOGI(TAG, "✅ Watch LCD brightness change NOW!");
    
    int counter = 0;
    bool backlight_on = false;
    
    while(1) {
        counter++;
        backlight_on = !backlight_on;
        
        gpio_set_level(LCD_BL_PIN, backlight_on ? 1 : 0);
        
        ESP_LOGI(TAG, "LCD Backlight GPIO %d = %s (Test %d)", 
                 LCD_BL_PIN, backlight_on ? "BRIGHT" : "DIM", counter);
        
        vTaskDelay(pdMS_TO_TICKS(1000)); // 1 second on/off
    }
}