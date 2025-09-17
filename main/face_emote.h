#pragma once

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <lvgl.h>
#include <math.h>

// Face emotions
typedef enum {
    FACE_NEUTRAL,
    FACE_HAPPY,
    FACE_EXCITED,
    FACE_SLEEPY,
    FACE_SURPRISED,
    FACE_WINK
} face_emotion_t;

// Audio level thresholds
#define AUDIO_THRESHOLD_LOW     0.1f
#define AUDIO_THRESHOLD_MID     0.3f
#define AUDIO_THRESHOLD_HIGH    0.6f

// Face parameters
typedef struct {
    int eye_left_x, eye_left_y;
    int eye_right_x, eye_right_y;
    int eye_width, eye_height;
    int mouth_x, mouth_y;
    int mouth_width, mouth_height;
    face_emotion_t emotion;
    float audio_level;
    bool is_speaking;
} face_state_t;

// Function declarations
void face_emote_init(void);
void face_update_emotion(face_emotion_t emotion);
void face_update_audio_level(float level);
void face_draw(lv_obj_t *parent);
void face_animate_speaking(bool speaking);
void face_blink_animation(void);