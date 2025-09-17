#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include "driver/ledc.h"
#include "driver/gpio.h"

static const char *TAG = "ESP32_SHOWCASE";

// Backlight control (from original demo)
#define BACKLIGHT_PIN 5
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_DUTY_RES LEDC_TIMER_13_BIT
#define LEDC_FREQUENCY 5000
#define LEDC_MAX_DUTY ((1 << LEDC_DUTY_RES) - 1)

// Touch button (from schematic)
#define TOUCH_PIN 0

static ledc_channel_config_t ledc_channel;
static bool touch_pressed = false;
static int demo_mode = 0;

void backlight_init(void) {
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_DUTY_RES,
        .freq_hz = LEDC_FREQUENCY,
        .speed_mode = LEDC_MODE,
        .timer_num = LEDC_TIMER,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel.channel = LEDC_CHANNEL;
    ledc_channel.duty = 0;
    ledc_channel.gpio_num = BACKLIGHT_PIN;
    ledc_channel.speed_mode = LEDC_MODE;
    ledc_channel.timer_sel = LEDC_TIMER;
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    
    ESP_LOGI(TAG, "Backlight initialized");
}

void set_backlight(uint8_t brightness) {
    if (brightness > 100) brightness = 100;
    uint32_t duty = (LEDC_MAX_DUTY * brightness) / 100;
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
}

void touch_init(void) {
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << TOUCH_PIN),
        .pull_down_en = 0,
        .pull_up_en = 1,
    };
    gpio_config(&io_conf);
    ESP_LOGI(TAG, "Touch button initialized");
}

bool read_touch(void) {
    return !gpio_get_level(TOUCH_PIN); // Active low
}

void demo_breathing(void) {
    ESP_LOGI(TAG, "Demo: Breathing light");
    for (int i = 0; i <= 100; i += 2) {
        set_backlight(i);
        vTaskDelay(pdMS_TO_TICKS(30));
        if (read_touch()) return;
    }
    for (int i = 100; i >= 0; i -= 2) {
        set_backlight(i);
        vTaskDelay(pdMS_TO_TICKS(30));
        if (read_touch()) return;
    }
}

void demo_pulse(void) {
    ESP_LOGI(TAG, "Demo: Pulse effect");
    for (int cycle = 0; cycle < 3; cycle++) {
        set_backlight(100);
        vTaskDelay(pdMS_TO_TICKS(200));
        set_backlight(20);
        vTaskDelay(pdMS_TO_TICKS(200));
        if (read_touch()) return;
    }
}

void demo_strobe(void) {
    ESP_LOGI(TAG, "Demo: Strobe light");
    for (int i = 0; i < 10; i++) {
        set_backlight(100);
        vTaskDelay(pdMS_TO_TICKS(50));
        set_backlight(0);
        vTaskDelay(pdMS_TO_TICKS(50));
        if (read_touch()) return;
    }
}

void demo_fade_levels(void) {
    ESP_LOGI(TAG, "Demo: Brightness levels");
    uint8_t levels[] = {10, 30, 50, 70, 90, 100};
    for (int i = 0; i < 6; i++) {
        set_backlight(levels[i]);
        ESP_LOGI(TAG, "Brightness: %d%%", levels[i]);
        vTaskDelay(pdMS_TO_TICKS(800));
        if (read_touch()) return;
    }
}

void showcase_task(void *param) {
    while (1) {
        // Check for touch to cycle demos
        if (read_touch() && !touch_pressed) {
            touch_pressed = true;
            demo_mode = (demo_mode + 1) % 4;
            ESP_LOGI(TAG, "Switched to demo mode %d", demo_mode);
            vTaskDelay(pdMS_TO_TICKS(300)); // Debounce
        } else if (!read_touch()) {
            touch_pressed = false;
        }

        // Run current demo
        switch (demo_mode) {
            case 0:
                demo_breathing();
                break;
            case 1:
                demo_pulse();
                break;
            case 2:
                demo_strobe();
                break;
            case 3:
                demo_fade_levels();
                break;
        }
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "ESP32-S3 Touch LCD Showcase Starting...");
    
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // Initialize hardware
    backlight_init();
    touch_init();
    
    ESP_LOGI(TAG, "Device Capabilities Showcase");
    ESP_LOGI(TAG, "- 1.85\" 360x360 LCD with backlight control");
    ESP_LOGI(TAG, "- Touch button interaction");
    ESP_LOGI(TAG, "- Multiple visual effects");
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "Touch the screen to cycle through demos:");
    ESP_LOGI(TAG, "0: Breathing light");
    ESP_LOGI(TAG, "1: Pulse effect");
    ESP_LOGI(TAG, "2: Strobe light");
    ESP_LOGI(TAG, "3: Brightness levels");
    
    // Start showcase
    xTaskCreate(showcase_task, "showcase", 4096, NULL, 5, NULL);
    
    // Status updates
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(5000));
        ESP_LOGI(TAG, "Running demo mode %d - Touch to change", demo_mode);
    }
}