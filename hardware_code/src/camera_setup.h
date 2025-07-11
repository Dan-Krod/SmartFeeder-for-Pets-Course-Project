#ifndef CAMERA_SETUP_H
#define CAMERA_SETUP_H

#include "esp_camera.h"
#include "camera_pins.h"

bool initCamera() {
  camera_config_t config = {
    .ledc_channel = LEDC_CHANNEL_0,
    .ledc_timer = LEDC_TIMER_0,
    .pin_d0 = Y2_GPIO_NUM,
    .pin_d1 = Y3_GPIO_NUM,
    .pin_d2 = Y4_GPIO_NUM,
    .pin_d3 = Y5_GPIO_NUM,
    .pin_d4 = Y6_GPIO_NUM,
    .pin_d5 = Y7_GPIO_NUM,
    .pin_d6 = Y8_GPIO_NUM,
    .pin_d7 = Y9_GPIO_NUM,
    .pin_xclk = XCLK_GPIO_NUM,
    .pin_pclk = PCLK_GPIO_NUM,
    .pin_vsync = VSYNC_GPIO_NUM,
    .pin_href = HREF_GPIO_NUM,
    .pin_sccb_sda = SIOD_GPIO_NUM,
    .pin_sccb_scl = SIOC_GPIO_NUM,
    .pin_pwdn = PWDN_GPIO_NUM,
    .pin_reset = RESET_GPIO_NUM,
    .xclk_freq_hz = 10000000,
    .pixel_format = PIXFORMAT_JPEG,
    .frame_size = FRAMESIZE_HVGA,
    .jpeg_quality = 10,
    .fb_count = 1
  };

  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("❌ Camera init failed!");
    return false;
  }

  Serial.println("✅ Camera initialized.");
  return true;
}

#endif 