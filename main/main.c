#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include "driver/ledc.h"

static const char *TAG = "SCREEN_FLASH";

// Backlight settings from original demo
#define EXAMPLE_LCD_PIN_NUM_BK_LIGHT        (5)
#define LEDC_HS_TIMER          LEDC_TIMER_0
#define LEDC_LS_MODE           LEDC_LOW_SPEED_MODE
#define LEDC_HS_CH0_CHANNEL    LEDC_CHANNEL_0
#define LEDC_ResolutionRatio   LEDC_TIMER_13_BIT
#define LEDC_MAX_Duty          ((1 << LEDC_ResolutionRatio) - 1)
#define Backlight_MAX   100

static ledc_channel_config_t ledc_channel;

void Backlight_Init(void) {
    ESP_LOGI(TAG, "Turn off LCD backlight");
    
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_ResolutionRatio,
        .freq_hz = 5000,
        .speed_mode = LEDC_LS_MODE,
        .timer_num = LEDC_HS_TIMER,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel.channel    = LEDC_HS_CH0_CHANNEL;
    ledc_channel.duty       = 0;
    ledc_channel.gpio_num   = EXAMPLE_LCD_PIN_NUM_BK_LIGHT;
    ledc_channel.speed_mode = LEDC_LS_MODE;
    ledc_channel.timer_sel  = LEDC_HS_TIMER;
    ledc_channel_config(&ledc_channel);
    ledc_fade_func_install(0);
}

void Set_Backlight(uint8_t Light) {   
    if(Light > Backlight_MAX) Light = Backlight_MAX;
    uint16_t Duty = LEDC_MAX_Duty-(81*(Backlight_MAX-Light));
    if(Light == 0) 
        Duty = 0;
    ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, Duty);
    ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);
}

void app_main(void) {
    ESP_LOGI(TAG, "ESP32-S3 Screen Flash Demo Starting...");
    
    Backlight_Init();
    
    ESP_LOGI(TAG, "Screen will flash - simulating audio reactive display");
    
    while (1) {
        // Simulate audio levels with different flash patterns
        
        // Low audio - gentle pulse
        for (int i = 10; i <= 40; i += 5) {
            Set_Backlight(i);
            vTaskDelay(pdMS_TO_TICKS(200));
        }
        for (int i = 40; i >= 10; i -= 5) {
            Set_Backlight(i);
            vTaskDelay(pdMS_TO_TICKS(200));
        }
        
        vTaskDelay(pdMS_TO_TICKS(500));
        
        // Medium audio - faster pulse
        for (int i = 20; i <= 70; i += 10) {
            Set_Backlight(i);
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        for (int i = 70; i >= 20; i -= 10) {
            Set_Backlight(i);
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        
        vTaskDelay(pdMS_TO_TICKS(500));
        
        // High audio - rapid flash
        for (int i = 30; i <= 100; i += 20) {
            Set_Backlight(i);
            vTaskDelay(pdMS_TO_TICKS(50));
        }
        for (int i = 100; i >= 30; i -= 20) {
            Set_Backlight(i);
            vTaskDelay(pdMS_TO_TICKS(50));
        }
        
        vTaskDelay(pdMS_TO_TICKS(1000));
        
        ESP_LOGI(TAG, "Flash cycle complete - simulating different audio levels");
    }
}