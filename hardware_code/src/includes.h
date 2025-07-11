#ifndef INCLUDES_H
#define INCLUDES_H

// Systemic
#include <Arduino.h>
#include "esp_camera.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// Time, Firebase, Cloud
#include <FirebaseESP32.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Sensors and components
#include <ESP32Servo.h>
#include <HX711_ADC.h>
#include <EEPROM.h>
#include <Wire.h>

// OLED
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Basic utils
#include "Base64.h"

// User modules
#include "wifi_setup.h"
#include "camera_setup.h"
#include "display_oled.h"
#include "firebase_setup.h"
#include "servo_scale_sonic.h"
#include "google_drive_upload.h"
#include "utils.h"
#include "camera_action.h"

#define CAMERA_MODEL_ESP32S3_EYE  // Has PSRAM
#include "camera_pins.h"

#endif