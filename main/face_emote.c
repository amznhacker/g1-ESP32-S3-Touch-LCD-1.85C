#include "face_emote.h"

static const char *TAG = "FACE_EMOTE";
static face_state_t face_state;
static lv_obj_t *face_canvas = NULL;
static lv_timer_t *blink_timer = NULL;
static lv_timer_t *audio_timer = NULL;

// Colors
static lv_color_t face_bg_color;
static lv_color_t eye_color;
static lv_color_t mouth_color;

// Initialize face emoting system
void face_emote_init(void) {
    // Initialize face state
    face_state.eye_left_x = 60;
    face_state.eye_left_y = 80;
    face_state.eye_right_x = 120;
    face_state.eye_right_y = 80;
    face_state.eye_width = 25;
    face_state.eye_height = 25;
    face_state.mouth_x = 90;
    face_state.mouth_y = 130;
    face_state.mouth_width = 40;
    face_state.mouth_height = 20;
    face_state.emotion = FACE_NEUTRAL;
    face_state.audio_level = 0.0f;
    face_state.is_speaking = false;

    // Initialize colors
    face_bg_color = lv_color_hex(0x000000);  // Black background
    eye_color = lv_color_hex(0x00FF00);      // Green eyes
    mouth_color = lv_color_hex(0xFF0000);    // Red mouth

    ESP_LOGI(TAG, "Face emoting system initialized");
}

// Update face emotion
void face_update_emotion(face_emotion_t emotion) {
    face_state.emotion = emotion;
    
    switch (emotion) {
        case FACE_HAPPY:
            face_state.eye_height = 20;  // Slightly squinted
            face_state.mouth_height = 25;
            mouth_color = lv_color_hex(0xFF4444);  // Bright red smile
            break;
            
        case FACE_EXCITED:
            face_state.eye_width = 30;   // Wide eyes
            face_state.eye_height = 30;
            face_state.mouth_width = 50;
            face_state.mouth_height = 30;
            eye_color = lv_color_hex(0x00FFFF);    // Cyan eyes
            mouth_color = lv_color_hex(0xFF0088);  // Pink mouth
            break;
            
        case FACE_SLEEPY:
            face_state.eye_height = 8;   // Droopy eyes
            face_state.mouth_width = 20;
            face_state.mouth_height = 10;
            eye_color = lv_color_hex(0x0088FF);    // Blue eyes
            mouth_color = lv_color_hex(0x888888);  // Gray mouth
            break;
            
        case FACE_SURPRISED:
            face_state.eye_width = 35;   // Very wide eyes
            face_state.eye_height = 35;
            face_state.mouth_width = 15;
            face_state.mouth_height = 25;
            eye_color = lv_color_hex(0xFFFF00);    // Yellow eyes
            mouth_color = lv_color_hex(0xFF8800);  // Orange mouth
            break;
            
        case FACE_WINK:
            // Will be handled in draw function
            eye_color = lv_color_hex(0xFF00FF);    // Magenta eyes
            mouth_color = lv_color_hex(0x00FF88);  // Green mouth
            break;
            
        default: // FACE_NEUTRAL
            face_state.eye_width = 25;
            face_state.eye_height = 25;
            face_state.mouth_width = 40;
            face_state.mouth_height = 20;
            eye_color = lv_color_hex(0x00FF00);    // Green eyes
            mouth_color = lv_color_hex(0xFF0000);  // Red mouth
            break;
    }
    
    ESP_LOGI(TAG, "Face emotion updated to: %d", emotion);
}

// Update audio level and trigger appropriate emotion
void face_update_audio_level(float level) {
    face_state.audio_level = level;
    
    // Determine emotion based on audio level
    if (level > AUDIO_THRESHOLD_HIGH) {
        face_update_emotion(FACE_EXCITED);
        face_state.is_speaking = true;
    } else if (level > AUDIO_THRESHOLD_MID) {
        face_update_emotion(FACE_HAPPY);
        face_state.is_speaking = true;
    } else if (level > AUDIO_THRESHOLD_LOW) {
        face_update_emotion(FACE_NEUTRAL);
        face_state.is_speaking = true;
    } else {
        face_update_emotion(FACE_SLEEPY);
        face_state.is_speaking = false;
    }
    
    ESP_LOGI(TAG, "Audio level: %.2f, Speaking: %s", level, face_state.is_speaking ? "Yes" : "No");
}

// Draw the face on canvas
static void draw_face_on_canvas(lv_obj_t *canvas) {
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    
    // Clear canvas
    lv_canvas_fill_bg(canvas, face_bg_color, LV_OPA_COVER);
    
    // Draw eyes
    rect_dsc.bg_color = eye_color;
    rect_dsc.radius = face_state.eye_width / 2;
    
    if (face_state.emotion == FACE_WINK) {
        // Left eye closed (wink)
        lv_area_t left_eye_area = {
            face_state.eye_left_x - face_state.eye_width/2,
            face_state.eye_left_y - 2,
            face_state.eye_left_x + face_state.eye_width/2,
            face_state.eye_left_y + 2
        };
        lv_canvas_draw_rect(canvas, &left_eye_area, &rect_dsc);
        
        // Right eye normal
        lv_area_t right_eye_area = {
            face_state.eye_right_x - face_state.eye_width/2,
            face_state.eye_right_y - face_state.eye_height/2,
            face_state.eye_right_x + face_state.eye_width/2,
            face_state.eye_right_y + face_state.eye_height/2
        };
        lv_canvas_draw_rect(canvas, &right_eye_area, &rect_dsc);
    } else {
        // Both eyes normal
        lv_area_t left_eye_area = {
            face_state.eye_left_x - face_state.eye_width/2,
            face_state.eye_left_y - face_state.eye_height/2,
            face_state.eye_left_x + face_state.eye_width/2,
            face_state.eye_left_y + face_state.eye_height/2
        };
        lv_canvas_draw_rect(canvas, &left_eye_area, &rect_dsc);
        
        lv_area_t right_eye_area = {
            face_state.eye_right_x - face_state.eye_width/2,
            face_state.eye_right_y - face_state.eye_height/2,
            face_state.eye_right_x + face_state.eye_width/2,
            face_state.eye_right_y + face_state.eye_height/2
        };
        lv_canvas_draw_rect(canvas, &right_eye_area, &rect_dsc);
    }
    
    // Draw mouth
    rect_dsc.bg_color = mouth_color;
    rect_dsc.radius = face_state.mouth_height / 2;
    
    // Add speaking animation (mouth movement)
    int mouth_offset = 0;
    if (face_state.is_speaking) {
        mouth_offset = (int)(sin(lv_tick_get() / 100.0) * 5);
    }
    
    lv_area_t mouth_area = {
        face_state.mouth_x - face_state.mouth_width/2,
        face_state.mouth_y - face_state.mouth_height/2 + mouth_offset,
        face_state.mouth_x + face_state.mouth_width/2,
        face_state.mouth_y + face_state.mouth_height/2 + mouth_offset
    };
    lv_canvas_draw_rect(canvas, &mouth_area, &rect_dsc);
    
    // Add audio level indicator
    if (face_state.audio_level > 0.05f) {
        char audio_text[32];
        snprintf(audio_text, sizeof(audio_text), "♪ %.2f", face_state.audio_level);
        
        lv_draw_label_dsc_t label_dsc;
        lv_draw_label_dsc_init(&label_dsc);
        label_dsc.color = lv_color_hex(0xFFFFFF);
        
        lv_area_t text_area = {10, 10, 150, 30};
        lv_canvas_draw_text(canvas, &text_area, &label_dsc, audio_text);
    }
}

// Create and draw face
void face_draw(lv_obj_t *parent) {
    if (face_canvas == NULL) {
        // Create canvas
        face_canvas = lv_canvas_create(parent);
        lv_obj_set_size(face_canvas, 200, 200);
        lv_obj_center(face_canvas);
        
        // Create buffer for canvas
        static lv_color_t canvas_buf[200 * 200];
        lv_canvas_set_buffer(face_canvas, canvas_buf, 200, 200, LV_IMG_CF_TRUE_COLOR);
    }
    
    draw_face_on_canvas(face_canvas);
}

// Animate speaking
void face_animate_speaking(bool speaking) {
    face_state.is_speaking = speaking;
    if (speaking) {
        ESP_LOGI(TAG, "Face: O_O ~~~♪ (Audio: %.2f)", face_state.audio_level);
    }
}

// Blink animation callback
static void blink_timer_cb(lv_timer_t *timer) {
    static bool is_blinking = false;
    static uint32_t blink_start = 0;
    
    if (!is_blinking && (lv_tick_get() - blink_start > 3000)) {  // Blink every 3 seconds
        is_blinking = true;
        blink_start = lv_tick_get();
        
        // Temporarily change to wink
        face_emotion_t current_emotion = face_state.emotion;
        face_update_emotion(FACE_WINK);
        
        // Restore after 200ms
        lv_timer_t *restore_timer = lv_timer_create([](lv_timer_t *t) {
            face_update_emotion((face_emotion_t)(uintptr_t)t->user_data);
            lv_timer_del(t);
        }, 200, (void*)(uintptr_t)current_emotion);
        lv_timer_set_repeat_count(restore_timer, 1);
        
        is_blinking = false;
    }
}

// Audio monitoring callback
static void audio_timer_cb(lv_timer_t *timer) {
    // Simulate audio level (in real implementation, get from audio driver)
    static float audio_sim = 0.0f;
    static int audio_direction = 1;
    
    // Simple audio simulation for demo
    audio_sim += audio_direction * 0.05f;
    if (audio_sim > 0.8f || audio_sim < 0.0f) {
        audio_direction *= -1;
    }
    
    face_update_audio_level(audio_sim);
    
    // Redraw face
    if (face_canvas) {
        draw_face_on_canvas(face_canvas);
    }
}

// Start blink animation
void face_blink_animation(void) {
    if (blink_timer == NULL) {
        blink_timer = lv_timer_create(blink_timer_cb, 100, NULL);
    }
    
    if (audio_timer == NULL) {
        audio_timer = lv_timer_create(audio_timer_cb, 50, NULL);  // 20 FPS
    }
    
    ESP_LOGI(TAG, "Face animations started");
}