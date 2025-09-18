#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_bt_device.h>
#include <esp_gap_bt_api.h>
#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_vendor.h>
#include <esp_lcd_panel_ops.h>
#include <esp_vfs_fat.h>
#include <sdmmc_cmd.h>

static const char* TAG = "AUDIO_FACE";

// LCD pins for Waveshare ESP32-S3 Touch LCD 1.85"
#define LCD_HOST    SPI2_HOST
#define LCD_PIXEL_CLOCK_HZ     (20 * 1000 * 1000)
#define PIN_NUM_SCLK           12
#define PIN_NUM_MOSI           11
#define PIN_NUM_LCD_DC         8
#define PIN_NUM_LCD_RST        14
#define PIN_NUM_LCD_CS         10
#define PIN_NUM_BK_LIGHT       9
#define LCD_H_RES              240
#define LCD_V_RES              280

// SD Card pins (if available)
#define PIN_NUM_SD_MISO        13
#define PIN_NUM_SD_MOSI        11
#define PIN_NUM_SD_CLK         12
#define PIN_NUM_SD_CS          10

// Colors
#define COLOR_BLACK   0x0000
#define COLOR_WHITE   0xFFFF
#define COLOR_RED     0xF800
#define COLOR_BLUE    0x001F
#define COLOR_GREEN   0x07E0
#define COLOR_YELLOW  0xFFE0
#define COLOR_SKIN    0xFDB8
#define COLOR_MOUTH   0xF800

static esp_lcd_panel_handle_t panel_handle = NULL;

// Audio reactive face state
typedef struct {
    float mouth_open;     // 0.0 = closed, 1.0 = wide open
    float audio_level;    // Current audio level (simulated)
    bool bt_connected;    // Bluetooth connection status
    uint32_t frame_count;
    int expression;       // 0=neutral, 1=happy, 2=surprised, 3=sleepy
} audio_face_t;

static audio_face_t face = {0};

// Initialize LCD
static void lcd_init(void)
{
    spi_bus_config_t buscfg = {
        .sclk_io_num = PIN_NUM_SCLK,
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = LCD_H_RES * 80 * sizeof(uint16_t),
    };
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));

    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = PIN_NUM_LCD_DC,
        .cs_gpio_num = PIN_NUM_LCD_CS,
        .pclk_hz = LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .spi_mode = 0,
        .trans_queue_depth = 10,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &io_handle));

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = PIN_NUM_LCD_RST,
        .rgb_endian = LCD_RGB_ENDIAN_BGR,
        .bits_per_pixel = 16,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, true));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

    // Backlight on
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << PIN_NUM_BK_LIGHT
    };
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));
    gpio_set_level(PIN_NUM_BK_LIGHT, 1);
}

// Draw filled rectangle
static void draw_rect(int x, int y, int w, int h, uint16_t color)
{
    uint16_t *buffer = malloc(w * h * sizeof(uint16_t));
    for (int i = 0; i < w * h; i++) {
        buffer[i] = color;
    }
    esp_lcd_panel_draw_bitmap(panel_handle, x, y, x + w, y + h, buffer);
    free(buffer);
}

// Draw circle
static void draw_circle(int cx, int cy, int r, uint16_t color)
{
    for (int y = -r; y <= r; y++) {
        for (int x = -r; x <= r; x++) {
            if (x*x + y*y <= r*r) {
                uint16_t pixel = color;
                esp_lcd_panel_draw_bitmap(panel_handle, cx + x, cy + y, cx + x + 1, cy + y + 1, &pixel);
            }
        }
    }
}

// Draw audio reactive face
static void draw_audio_face(void)
{
    // Clear screen with gradient background
    for (int y = 0; y < LCD_V_RES; y++) {
        uint16_t bg_color = (y * 31 / LCD_V_RES) << 11; // Blue gradient
        draw_rect(0, y, LCD_H_RES, 1, bg_color);
    }
    
    // Face circle
    draw_circle(120, 140, 80, COLOR_SKIN);
    
    // Eyes based on expression
    int eye_y = 120;
    int eye_size = 12;
    
    switch (face.expression) {
        case 1: // Happy - smaller eyes
            eye_size = 8;
            break;
        case 2: // Surprised - bigger eyes
            eye_size = 16;
            break;
        case 3: // Sleepy - half closed
            eye_y = 125;
            eye_size = 6;
            break;
        default: // Neutral
            break;
    }
    
    draw_circle(100, eye_y, eye_size, COLOR_WHITE);
    draw_circle(140, eye_y, eye_size, COLOR_WHITE);
    draw_circle(100, eye_y, eye_size/2, COLOR_BLACK);
    draw_circle(140, eye_y, eye_size/2, COLOR_BLACK);
    
    // Audio reactive mouth
    int mouth_width = 20 + (int)(face.mouth_open * 40);  // 20-60 pixels wide
    int mouth_height = 8 + (int)(face.mouth_open * 20);  // 8-28 pixels tall
    int mouth_y = 160;
    
    // Different mouth shapes based on expression
    uint16_t mouth_color = COLOR_MOUTH;
    switch (face.expression) {
        case 1: // Happy - curved smile
            mouth_color = COLOR_RED;
            for (int x = -mouth_width/2; x <= mouth_width/2; x++) {
                int curve_y = mouth_y + (x*x) / (mouth_width/4);
                draw_rect(120 + x, curve_y, 1, 4, mouth_color);
            }
            break;
        case 2: // Surprised - O shape
            draw_circle(120, mouth_y, mouth_width/4, mouth_color);
            draw_circle(120, mouth_y, mouth_width/6, COLOR_BLACK);
            break;
        case 3: // Sleepy - small line
            draw_rect(120 - 10, mouth_y, 20, 2, mouth_color);
            break;
        default: // Neutral - oval
            for (int y = 0; y < mouth_height; y++) {
                int line_width = (int)(mouth_width * sinf(M_PI * y / mouth_height));
                int line_x = 120 - line_width/2;
                draw_rect(line_x, mouth_y + y, line_width, 1, mouth_color);
            }
            break;
    }
    
    // Audio level indicator bars
    for (int i = 0; i < 10; i++) {
        int bar_height = (int)(face.audio_level * 50 * (i + 1) / 10);
        uint16_t bar_color = (i < 3) ? COLOR_GREEN : (i < 7) ? COLOR_YELLOW : COLOR_RED;
        if (bar_height > 0) {
            draw_rect(10 + i * 8, 50 - bar_height, 6, bar_height, bar_color);
        }
    }
    
    // Connection status indicator
    if (face.bt_connected) {
        draw_circle(20, 20, 8, COLOR_BLUE);  // Blue dot = connected
        // Bluetooth symbol (simplified)
        draw_rect(18, 15, 2, 10, COLOR_WHITE);
        draw_rect(16, 17, 6, 2, COLOR_WHITE);
        draw_rect(16, 21, 6, 2, COLOR_WHITE);
    } else {
        draw_circle(20, 20, 8, COLOR_RED);   // Red dot = disconnected
    }
    
    // Frame counter
    char frame_text[32];
    snprintf(frame_text, sizeof(frame_text), "Frame: %lu", face.frame_count);
    // Simple text display would need a font library, so we'll skip for now
}

// Simulate audio input (replace with real Bluetooth A2DP later)
static void simulate_audio(void)
{
    static float phase = 0;
    phase += 0.1f;
    
    // Simulate music with varying intensity
    face.audio_level = (sinf(phase) + 1.0f) * 0.5f * (0.5f + 0.5f * sinf(phase * 0.3f));
    face.mouth_open = face.audio_level * 0.8f + 0.2f * sinf(phase * 2.0f);
    
    // Change expression occasionally
    if ((int)(phase * 10) % 100 == 0) {
        face.expression = (face.expression + 1) % 4;
    }
}

// Initialize Bluetooth (simplified for now)
static void bt_init(void)
{
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));
    
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT));
    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());
    
    // Set device name (using deprecated but working API for now)
    esp_bt_dev_set_device_name("ESP32-AudioFace");
    
    face.bt_connected = true; // Simulate connection for demo
    
    ESP_LOGI(TAG, "Bluetooth initialized - Device: ESP32-AudioFace");
}

// Initialize SD card (if available)
static void sd_init(void)
{
    ESP_LOGI(TAG, "Initializing SD card");
    
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    
    sdmmc_card_t *card;
    const char mount_point[] = "/sdcard";
    
    ESP_LOGI(TAG, "Using SPI peripheral");
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_SD_MOSI,
        .miso_io_num = PIN_NUM_SD_MISO,
        .sclk_io_num = PIN_NUM_SD_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    
    esp_err_t ret = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize bus.");
        return;
    }
    
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_SD_CS;
    slot_config.host_id = host.slot;
    
    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);
    
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s).", esp_err_to_name(ret));
        }
        return;
    }
    
    ESP_LOGI(TAG, "SD card mounted successfully");
    sdmmc_card_print_info(stdout, card);
}

// Animation task
static void animation_task(void *pvParameters)
{
    while (1) {
        face.frame_count++;
        
        // Simulate audio input
        simulate_audio();
        
        // Draw the face
        draw_audio_face();
        
        // Log status every 60 frames (2 seconds at 30 FPS)
        if (face.frame_count % 60 == 0) {
            ESP_LOGI(TAG, "Frame %lu | Expression: %d | Audio: %.2f | Mouth: %.2f", 
                     face.frame_count, face.expression, face.audio_level, face.mouth_open);
        }
        
        vTaskDelay(pdMS_TO_TICKS(33)); // ~30 FPS
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "ESP32-S3 Audio Reactive Face Starting...");
    
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // Initialize LCD
    lcd_init();
    ESP_LOGI(TAG, "LCD initialized - 240x280 ST7789");
    
    // Initialize SD card (optional)
    sd_init();
    
    // Initialize Bluetooth
    bt_init();
    
    // Show initial face
    draw_audio_face();
    
    // Start animation
    xTaskCreate(animation_task, "animation", 8192, NULL, 5, NULL);
    
    ESP_LOGI(TAG, "Audio Reactive Face Ready!");
    ESP_LOGI(TAG, "- Simulated audio reactive mouth");
    ESP_LOGI(TAG, "- Multiple facial expressions");
    ESP_LOGI(TAG, "- Audio level visualization");
    ESP_LOGI(TAG, "- Bluetooth ready for future A2DP integration");
}