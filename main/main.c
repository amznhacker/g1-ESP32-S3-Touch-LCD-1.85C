#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include "driver/ledc.h"

static const char *TAG = "SCREEN_FLASH";

#define BACKLIGHT_PIN 5
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_DUTY_RES LEDC_TIMER_13_BIT
#define LEDC_FREQUENCY 5000

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
    
    ESP_LOGI(TAG, "LCD Backlight initialized");
}

static void set_backlight(uint8_t brightness) {
    uint32_t duty = (8191 * brightness) / 100;
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
}

void app_main(void) {
    ESP_LOGI(TAG, "Screen Flash Demo Starting...");
    
    init_backlight();
    
    ESP_LOGI(TAG, "Screen will flash in patterns");
    
    while (1) {
        // Flash pattern
        for (int i = 0; i <= 100; i += 10) {
            set_backlight(i);
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        for (int i = 100; i >= 0; i -= 10) {
            set_backlight(i);
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}