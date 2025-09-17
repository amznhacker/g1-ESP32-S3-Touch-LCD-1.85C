#include "face_emote.h"
#include "bluetooth_face.h"

// Integration with existing demo
// This file shows how to integrate face emoting with the existing LVGL demo

static lv_obj_t *face_tab = NULL;
static lv_obj_t *bt_status_label = NULL;

// Create face tab in existing tabview
void create_face_tab(lv_obj_t *tabview) {
    // Add face tab to existing tabview
    face_tab = lv_tabview_add_tab(tabview, "Face");
    
    // Set dark background for face
    lv_obj_set_style_bg_color(face_tab, lv_color_hex(0x000000), 0);
    
    // Create Bluetooth status label
    bt_status_label = lv_label_create(face_tab);
    lv_label_set_text(bt_status_label, "ESP32_Face\nBluetooth: Waiting...");
    lv_obj_set_style_text_color(bt_status_label, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_text_align(bt_status_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(bt_status_label, LV_ALIGN_TOP_MID, 0, 10);
    
    // Initialize face emoting
    face_emote_init();
    face_draw(face_tab);
    face_blink_animation();
    
    // Initialize Bluetooth
    bluetooth_face_init();
    bluetooth_start_discoverable();
}

// Update Bluetooth status in UI
void update_bluetooth_status(const char *status) {
    if (bt_status_label) {
        char text[100];
        snprintf(text, sizeof(text), "ESP32_Face\nBluetooth: %s", status);
        lv_label_set_text(bt_status_label, text);
    }
}

// Integration callbacks for demo
void face_demo_bt_connected(void) {
    update_bluetooth_status("Connected ✓");
    face_update_emotion(FACE_HAPPY);
}

void face_demo_bt_disconnected(void) {
    update_bluetooth_status("Waiting...");
    face_update_emotion(FACE_SLEEPY);
}

void face_demo_audio_level(float level) {
    face_update_audio_level(level);
    
    // Update status with audio info
    if (level > 0.05f) {
        char status[50];
        snprintf(status, sizeof(status), "Playing ♪ %.2f", level);
        update_bluetooth_status(status);
    }
}