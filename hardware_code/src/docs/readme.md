# 🐾 Smart Pet Feeder — ESP32-S3 Firmware

This is a modular firmware system for a smart pet feeder powered by the ESP32-S3 microcontroller. It supports automated and manual feeding, weight measurement, presence detection, photo capture, cloud logging, and a local OLED display. The project integrates with Firebase and Google Drive for remote control, monitoring, and image storage.

---

## 📂 Project Folder: `src/`  

This folder contains all logic and modules required to run the feeder firmware.

---

## 🚀 Features

- 📶 Wi-Fi auto-connection
- ☁️ Firebase integration with real-time updates
- 🕒 NTP time sync (UTC+3 for Kyiv)
- ⚖️ Weight measurement using HX711
- 📏 Obstacle detection (HC-SR04)
- 📸 Remote photo capture via Firebase trigger
- 🖥️ OLED UI cycling through time, feed schedule, weight
- 🍽️ Automated feeding via servo (scheduled or manual)
- ☁️ Photo upload to Google Drive via Apps Script
- 📢 Push notifications (inactivity & missed feeding)

---

## 📁 Folder Structure

<table>
  <thead>
    <tr>
      <th style="text-align:left;">File / Module</th>
      <th style="text-align:left;">Description</th>
    </tr>
  </thead>
  <tbody>
    <tr><td><code>main.cpp / main.ino</code></td><td>Primary firmware logic: contains <code>setup()</code> and <code>loop()</code>, coordinates all modules</td></tr>
    <tr><td><code>includes.h</code></td><td>Central header that includes all libraries and project modules</td></tr>
    <tr><td><code>wifi_setup.h</code></td><td>Connects to Wi-Fi network and optionally initializes EEPROM</td></tr>
    <tr><td><code>firebase_setup.h</code></td><td>Sets up Firebase configuration: host, authentication token, auto-reconnect</td></tr>
    <tr><td><code>camera_setup.h</code></td><td>Configures ESP32-S3 camera (resolution, pin mapping, frame format)</td></tr>
    <tr><td><code>camera_index.h</code></td><td>Embedded HTML interface for ESP32-CAM video streaming (for use with web server)</td></tr>
    <tr><td><code>camera_pins.h</code></td><td>Defines pinout mappings for the ESP32-S3 camera module</td></tr>
    <tr><td><code>servo_scale_sonic.h</code></td><td>Manages servo motor feeding mechanism, ultrasonic distance sensor (HC-SR04), and HX711 scale</td></tr>
    <tr><td><code>display_oled.h</code></td><td>Controls rendering to OLED display; shows time, weight, schedule</td></tr>
    <tr><td><code>google_drive_upload.h</code></td><td>Captures a photo, encodes it as Base64, and uploads via Google Apps Script</td></tr>
    <tr><td><code>image_processing.h</code></td><td>Utility functions for Base64 URL encoding and JPEG frame formatting</td></tr>
    <tr><td><code>utils.h</code></td><td>Time updates (NTP), feed failure checks, and inactivity detection logic</td></tr>
    <tr><td><code>camera_action.h</code></td><td>Listens to Firebase for remote camera capture triggers (<code>/cameraControl/action</code>)</td></tr>
    <tr><td><code>Base64.h</code></td><td>Header for Base64 encoding helper used in photo uploads</td></tr>
    <tr><td><code>Base64.cpp</code></td><td>Implements custom base64 encoding functionality (for use with constrained devices)</td></tr>
    <tr><td><code>app_httpd.cpp</code> <em>(optional)</em></td><td>Starts an ESP32-CAM HTTP web server for live video streaming (if streaming mode enabled)</td></tr>
    <tr><td><code>partitions.csv</code></td><td>Defines memory partition layout for the ESP32 (e.g. firmware size, OTA, SPIFFS)</td></tr>
    <tr><td><code>ci.json</code></td><td>Build configuration or CI/CD metadata (e.g. PlatformIO or GitHub Actions settings)</td></tr>
  </tbody>
</table>

---

## ⚙️ Hardware Requirements

- ESP32-S3 board (e.g., ESP32-S3-EYE) with PSRAM
- HX711 + load cell
- HC-SR04 ultrasonic sensor
- Servo motor
- 0.96" or 1.3" OLED display (SSD1306)
- Camera module (OV2640)
- Internet connection with access to Firebase and Google Scripts

---

## 🧠 Core Workflow

1. Connect to Wi-Fi
2. Initialize Firebase + time sync
3. Schedule feeding via Firebase (`/servo/scheduled_time`)
4. Allow real-time override (`/servo/state`)
5. Display feed + time info on OLED
6. Measure distance and weight
7. Send alerts to Firebase if:
   - No approach in over an hour
   - No feeding detected in 24h
8. On `/cameraControl/action` trigger → take photo → upload to Google Drive

---

## 🔑 Setup Guide

1. Edit:
   - `wifi_setup.h`: Wi-Fi SSID/password
   - `firebase_setup.h`: Firebase host & token
   - `google_drive_upload.h`: Script URL & keys
2. Open in [Arduino IDE] or PlatformIO
3. Select board: **ESP32-S3 Dev Module**
4. Flash to your device
5. Watch serial monitor for logs

---

## ✅ Dependencies

- `FirebaseESP32`
- `Adafruit_SSD1306`
- `HX711_ADC`
- `ESP32Servo`
- `NTPClient`
- `WiFi` / `Wire`
- `Base64` (local file)

---

## 📌 Notes

- Ensure `CAMERA_MODEL_ESP32S3_EYE` is defined before including `camera_pins.h`
- Use secure OTA updates or serial flashing as preferred
- For large JPEG uploads: ensure PSRAM is enabled and stable

---

## 📸 Drive Upload Sample

Photos are uploaded as base64-encoded JPEGs to your Google Apps Script endpoint for storage in Drive. Example script should decode and store images in a named folder with timestamps.

---
