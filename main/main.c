#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <math.h>
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"

static const char *TAG = "ESP32_LCD_SHOWCASE";

// LCD Configuration (ST77916 360x360)
#define LCD_HOST SPI2_HOST
#define LCD_PIXEL_CLOCK_HZ (80 * 1000 * 1000)
#define LCD_BK_LIGHT_ON_LEVEL 1
#define LCD_BK_LIGHT_OFF_LEVEL !LCD_BK_LIGHT_ON_LEVEL
#define PIN_NUM_MOSI 11
#define PIN_NUM_CLK 12
#define PIN_NUM_CS 10
#define PIN_NUM_DC 14
#define PIN_NUM_RST 13
#define PIN_NUM_BK_LIGHT 5
#define LCD_H_RES 360
#define LCD_V_RES 360

// Touch button
#define TOUCH_PIN 0

static esp_lcd_panel_handle_t panel_handle = NULL;
static bool touch_pressed = false;
static int demo_mode = 0;
static uint16_t *lcd_buffer = NULL;

// Color definitions (RGB565)
#define COLOR_BLACK   0x0000
#define COLOR_WHITE   0xFFFF
#define COLOR_RED     0xF800
#define COLOR_GREEN   0x07E0
#define COLOR_BLUE    0x001F
#define COLOR_YELLOW  0xFFE0
#define COLOR_CYAN    0x07FF
#define COLOR_MAGENTA 0xF81F
#define COLOR_ORANGE  0xFD20
#define COLOR_PURPLE  0x8010

void lcd_init(void) {
    ESP_LOGI(TAG, "Initialize SPI bus");
    spi_bus_config_t buscfg = {
        .sclk_io_num = PIN_NUM_CLK,
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = LCD_H_RES * 80 * sizeof(uint16_t),
    };
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));

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

    ESP_LOGI(TAG, "Install ST77916 panel driver");
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = PIN_NUM_RST,
        .rgb_endian = LCD_RGB_ENDIAN_BGR,
        .bits_per_pixel = 16,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_st77916(io_handle, &panel_config, &panel_handle));

    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, true));
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, true, false));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

    // Initialize backlight
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << PIN_NUM_BK_LIGHT
    };
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));
    gpio_set_level(PIN_NUM_BK_LIGHT, LCD_BK_LIGHT_ON_LEVEL);

    // Allocate LCD buffer
    lcd_buffer = heap_caps_malloc(LCD_H_RES * LCD_V_RES * sizeof(uint16_t), MALLOC_CAP_DMA);
    if (!lcd_buffer) {
        ESP_LOGE(TAG, "Failed to allocate LCD buffer");
        return;
    }

    ESP_LOGI(TAG, "LCD initialized successfully");
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

void fill_screen(uint16_t color) {
    for (int i = 0; i < LCD_H_RES * LCD_V_RES; i++) {
        lcd_buffer[i] = color;
    }
    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, LCD_H_RES, LCD_V_RES, lcd_buffer);
}

void draw_circle(int cx, int cy, int radius, uint16_t color) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x*x + y*y <= radius*radius) {
                int px = cx + x;
                int py = cy + y;
                if (px >= 0 && px < LCD_H_RES && py >= 0 && py < LCD_V_RES) {
                    lcd_buffer[py * LCD_H_RES + px] = color;
                }
            }
        }
    }
}

void draw_text_simple(int x, int y, const char* text, uint16_t color) {
    // Simple 8x8 pixel font simulation
    int len = strlen(text);
    for (int i = 0; i < len; i++) {
        for (int py = 0; py < 8; py++) {
            for (int px = 0; px < 6; px++) {
                int screen_x = x + i * 8 + px;
                int screen_y = y + py;
                if (screen_x < LCD_H_RES && screen_y < LCD_V_RES) {
                    // Simple pattern for letters
                    if ((px == 1 || px == 4) && (py == 1 || py == 6)) {
                        lcd_buffer[screen_y * LCD_H_RES + screen_x] = color;
                    }
                }
            }
        }
    }
}

void demo_rainbow_circles(void) {
    ESP_LOGI(TAG, "Demo: Rainbow Circles");
    static int frame = 0;
    
    fill_screen(COLOR_BLACK);
    
    uint16_t colors[] = {COLOR_RED, COLOR_ORANGE, COLOR_YELLOW, COLOR_GREEN, COLOR_CYAN, COLOR_BLUE, COLOR_PURPLE, COLOR_MAGENTA};
    
    for (int i = 0; i < 8; i++) {
        int angle = (frame + i * 45) % 360;
        int cx = LCD_H_RES/2 + (int)(cos(angle * M_PI / 180) * 80);
        int cy = LCD_V_RES/2 + (int)(sin(angle * M_PI / 180) * 80);
        draw_circle(cx, cy, 20 + (frame % 20), colors[i]);
    }
    
    draw_text_simple(120, 20, "RAINBOW CIRCLES", COLOR_WHITE);
    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, LCD_H_RES, LCD_V_RES, lcd_buffer);
    
    frame += 5;
    if (read_touch()) return;
    vTaskDelay(pdMS_TO_TICKS(50));
}

void demo_plasma_effect(void) {
    ESP_LOGI(TAG, "Demo: Plasma Effect");
    static int time_offset = 0;
    
    for (int y = 0; y < LCD_V_RES; y++) {
        for (int x = 0; x < LCD_H_RES; x++) {
            float dx = x - LCD_H_RES/2;
            float dy = y - LCD_V_RES/2;
            float dist = sqrt(dx*dx + dy*dy);
            
            float plasma = sin(dist * 0.1 + time_offset * 0.1) + 
                          sin(x * 0.05 + time_offset * 0.08) + 
                          sin(y * 0.03 + time_offset * 0.12);
            
            int color_index = (int)((plasma + 3) * 42) % 256;
            uint16_t color = (color_index << 8) | (color_index >> 1) | ((255-color_index) >> 3);
            
            lcd_buffer[y * LCD_H_RES + x] = color;
        }
    }
    
    draw_text_simple(130, 20, "PLASMA EFFECT", COLOR_WHITE);
    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, LCD_H_RES, LCD_V_RES, lcd_buffer);
    
    time_offset += 2;
    if (read_touch()) return;
    vTaskDelay(pdMS_TO_TICKS(30));
}

void demo_bouncing_balls(void) {
    ESP_LOGI(TAG, "Demo: Bouncing Balls");
    static float ball_x[5] = {50, 100, 150, 200, 250};
    static float ball_y[5] = {50, 100, 150, 200, 100};
    static float vel_x[5] = {2, -1.5, 3, -2.5, 1};
    static float vel_y[5] = {1.5, 2, -1, 2, -2.5};
    uint16_t ball_colors[5] = {COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_YELLOW, COLOR_CYAN};
    
    fill_screen(COLOR_BLACK);
    
    for (int i = 0; i < 5; i++) {
        ball_x[i] += vel_x[i];
        ball_y[i] += vel_y[i];
        
        if (ball_x[i] <= 15 || ball_x[i] >= LCD_H_RES - 15) vel_x[i] *= -1;
        if (ball_y[i] <= 15 || ball_y[i] >= LCD_V_RES - 15) vel_y[i] *= -1;
        
        draw_circle((int)ball_x[i], (int)ball_y[i], 15, ball_colors[i]);
    }
    
    draw_text_simple(120, 20, "BOUNCING BALLS", COLOR_WHITE);
    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, LCD_H_RES, LCD_V_RES, lcd_buffer);
    
    if (read_touch()) return;
    vTaskDelay(pdMS_TO_TICKS(30));
}

void demo_color_gradient(void) {
    ESP_LOGI(TAG, "Demo: Color Gradient");
    static int gradient_offset = 0;
    
    for (int y = 0; y < LCD_V_RES; y++) {
        for (int x = 0; x < LCD_H_RES; x++) {
            int r = (x + gradient_offset) % 256;
            int g = (y + gradient_offset) % 256;
            int b = ((x + y + gradient_offset) / 2) % 256;
            
            uint16_t color = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
            lcd_buffer[y * LCD_H_RES + x] = color;
        }
    }
    
    draw_text_simple(120, 20, "COLOR GRADIENT", COLOR_BLACK);
    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, LCD_H_RES, LCD_V_RES, lcd_buffer);
    
    gradient_offset += 3;
    if (read_touch()) return;
    vTaskDelay(pdMS_TO_TICKS(50));
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
                demo_rainbow_circles();
                break;
            case 1:
                demo_plasma_effect();
                break;
            case 2:
                demo_bouncing_balls();
                break;
            case 3:
                demo_color_gradient();
                break;
        }
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "ESP32-S3 Touch LCD Visual Showcase Starting...");
    
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // Initialize hardware
    lcd_init();
    touch_init();
    
    ESP_LOGI(TAG, "Visual Demo Showcase Ready!");
    ESP_LOGI(TAG, "- 1.85\" 360x360 Full Color LCD Display");
    ESP_LOGI(TAG, "- Real-time Graphics Rendering");
    ESP_LOGI(TAG, "- Touch Interaction");
    ESP_LOGI(TAG, "- Multiple Visual Effects");
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "Touch the screen to cycle through demos:");
    ESP_LOGI(TAG, "0: Rainbow Circles");
    ESP_LOGI(TAG, "1: Plasma Effect");
    ESP_LOGI(TAG, "2: Bouncing Balls");
    ESP_LOGI(TAG, "3: Color Gradient");
    
    // Start showcase
    xTaskCreate(showcase_task, "showcase", 4096, NULL, 5, NULL);
    
    // Status updates
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(5000));
        const char* demo_names[] = {"Rainbow Circles", "Plasma Effect", "Bouncing Balls", "Color Gradient"};
        ESP_LOGI(TAG, "Running: %s (Mode %d) - Touch to change", demo_names[demo_mode], demo_mode);
    }
}