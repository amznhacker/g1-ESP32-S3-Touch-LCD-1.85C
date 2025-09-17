#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_vendor.h>
#include <esp_lcd_panel_ops.h>

static const char* TAG = "ESP32_FACE";

// LCD Configuration for ESP32-S3-Touch-LCD-1.85
#define LCD_HOST    SPI2_HOST
#define LCD_DMA_CH  SPI_DMA_CH_AUTO
#define LCD_PIXEL_CLOCK_HZ     (20 * 1000 * 1000)
#define LCD_BK_LIGHT_ON_LEVEL  1
#define LCD_BK_LIGHT_OFF_LEVEL !LCD_BK_LIGHT_ON_LEVEL
#define PIN_NUM_MOSI 11
#define PIN_NUM_CLK  12
#define PIN_NUM_CS   10
#define PIN_NUM_DC   13
#define PIN_NUM_RST  14
#define PIN_NUM_BK_LIGHT 15
#define LCD_H_RES 360
#define LCD_V_RES 360

static esp_lcd_panel_handle_t panel_handle = NULL;
static int animation_counter = 0;

// Simple face patterns (360x360 pixels, but we'll draw simple shapes)
static void draw_face(bool happy) {
    // Clear screen (black)
    uint16_t *buffer = malloc(LCD_H_RES * LCD_V_RES * sizeof(uint16_t));
    memset(buffer, 0, LCD_H_RES * LCD_V_RES * sizeof(uint16_t));
    
    // Draw simple face with colored pixels
    int center_x = LCD_H_RES / 2;
    int center_y = LCD_V_RES / 2;
    
    // Eyes (white circles)
    for (int y = center_y - 80; y < center_y - 40; y++) {
        for (int x = center_x - 60; x < center_x - 20; x++) {
            if ((x - (center_x - 40)) * (x - (center_x - 40)) + (y - (center_y - 60)) * (y - (center_y - 60)) < 400) {
                buffer[y * LCD_H_RES + x] = 0xFFFF; // White
            }
        }
        for (int x = center_x + 20; x < center_x + 60; x++) {
            if ((x - (center_x + 40)) * (x - (center_x + 40)) + (y - (center_y - 60)) * (y - (center_y - 60)) < 400) {
                buffer[y * LCD_H_RES + x] = 0xFFFF; // White
            }
        }
    }
    
    // Mouth
    if (happy) {
        // Happy mouth (smile arc)
        for (int x = center_x - 40; x < center_x + 40; x++) {
            int y = center_y + 20 + (x - center_x) * (x - center_x) / 80;
            if (y < LCD_V_RES) {
                buffer[y * LCD_H_RES + x] = 0xF800; // Red
            }
        }
    } else {
        // Sleepy mouth (horizontal line)
        for (int x = center_x - 30; x < center_x + 30; x++) {
            int y = center_y + 40;
            if (y < LCD_V_RES) {
                buffer[y * LCD_H_RES + x] = 0x07E0; // Green
            }
        }
    }
    
    // Send to LCD
    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, LCD_H_RES, LCD_V_RES, buffer);
    free(buffer);
}

static void init_lcd(void) {
    ESP_LOGI(TAG, "Initialize SPI bus");
    spi_bus_config_t buscfg = {
        .sclk_io_num = PIN_NUM_CLK,
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = LCD_H_RES * 80 * sizeof(uint16_t),
    };
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, LCD_DMA_CH));

    ESP_LOGI(TAG, "Install panel IO");
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = PIN_NUM_DC,
        .cs_gpio_num = PIN_NUM_CS,
        .pclk_hz = LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .spi_mode = 0,
        .trans_queue_depth = 10,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &io_handle));

    ESP_LOGI(TAG, "Install LCD driver");
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = PIN_NUM_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,
        .bits_per_pixel = 16,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, true));
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, true, false));

    // Turn on backlight
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << PIN_NUM_BK_LIGHT
    };
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));
    gpio_set_level(PIN_NUM_BK_LIGHT, LCD_BK_LIGHT_ON_LEVEL);

    ESP_LOGI(TAG, "LCD initialized successfully");
}

// Face animation task
static void face_animation_task(void *pvParameters) {
    int blink_counter = 0;
    
    while (1) {
        blink_counter++;
        animation_counter++;
        
        bool happy = (animation_counter % 100 < 50);
        
        // Update LCD display
        draw_face(happy);
        
        // Also log to console
        if (blink_counter % 20 == 0) {
            if (happy) {
                ESP_LOGI(TAG, "Face: ^_^ (Happy - Frame %d)", animation_counter);
            } else {
                ESP_LOGI(TAG, "Face: -_- (Sleepy - Frame %d)", animation_counter);
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "ESP32-S3 Face Animation with LCD Starting...");
    
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // Initialize LCD
    init_lcd();
    
    ESP_LOGI(TAG, "System initialized. Starting face animation...");
    
    // Start face animation task
    xTaskCreate(face_animation_task, "face_anim", 8192, NULL, 5, NULL);
    
    ESP_LOGI(TAG, "Face animation started! Watch the LCD screen for animated faces.");
    
    // Keep main task alive
    while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}