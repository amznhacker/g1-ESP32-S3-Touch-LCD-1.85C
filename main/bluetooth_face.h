#pragma once

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_a2dp_api.h>
#include <esp_avrc_api.h>
#include <esp_gap_bt_api.h>
#include "face_emote.h"

// Bluetooth device name
#define BT_DEVICE_NAME "ESP32_Face"

// Audio analysis parameters
#define AUDIO_SAMPLE_RATE 44100
#define AUDIO_BUFFER_SIZE 1024
#define AUDIO_ANALYSIS_WINDOW 512

// Function declarations
void bluetooth_face_init(void);
void bluetooth_start_discoverable(void);
void bluetooth_stop_discoverable(void);
void audio_analyze_and_emote(const uint8_t *data, uint32_t len);
float calculate_audio_level(const int16_t *samples, size_t count);
void bluetooth_connection_callback(esp_a2dp_connection_state_t state, esp_bd_addr_t *bd_addr);
void bluetooth_audio_callback(const uint8_t *data, uint32_t len);