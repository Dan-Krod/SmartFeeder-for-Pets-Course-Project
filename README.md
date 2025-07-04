# 🐾 SMARTFEEDER for Pets — Course Project

An intelligent, IoT-enabled pet feeder system based on the **ESP32-S3** microcontroller. This embedded solution combines automatic food dispensing, cloud-based image storage, real-time monitoring, and an onboard user interface.

---

## 📌 Overview

**SMARTFEEDER** is an automated pet management system designed for university coursework. It leverages a combination of embedded hardware and software integrations to achieve the following:

- Dispense measured amounts of food via a servo-driven mechanism
- Capture and upload feeding images to **Google Drive**
- Remotely update feeding schedules and commands via **Firebase Realtime Database**
- Display current system status and diagnostics on an **OLED screen**
- Offer a responsive **web UI**, hosted directly on the ESP32 for local control
- Utilize distance sensing and weight measurement for pet interaction detection and food quantification

---

## 📁 Folder Structure

```
SMARTFEEDER-FOR-PETS-COU/
├── hardware_code/
│   ├── diagrams/                     # Logical diagrams (basic & detailed)
│   └── src/
│       ├── docs/   
|       |   └── readme.md            # Firmware-specific description 
|       |
│       ├── app_httpd.cpp            # ESP32 web server implementation
│       ├── Base64.cpp/.h            # Base64 encoding/decoding
│       ├── camera_*                 # Camera configs, routing, pins
│       ├── ci.json                  # Continuous integration (if used)
│       ├── display_oled.h           # OLED display rendering logic
│       ├── firebase_setup.h         # Firebase DB setup and auth
│       ├── google_drive_upload.h    # Image upload to Google Drive
│       ├── image_processing.h       # Optional image filters or processing
│       ├── includes.h               # Centralized header for shared includes
│       ├── main.ino                 # Main firmware logic
│       ├── partitions.csv           # ESP32 partition scheme
│       ├── servo_scale_sonic.h      # Servo + ultrasonic + weight handling
│       ├── utils.h                  # Helper functions
│       └── wifi_setup.h             # Network credentials and connection
│                      
│
├── schematic/
│   ├── schematic.png                # Hardware connection diagram
│   └── readme.md                    # Schematic description
│
├── web_interface/
│   ├── images/                      # UI images/icons
│   ├── index.html                   # Frontend interface
│   ├── app.js                       # UI logic and requests to ESP32
|   ├── styles.css                   # Styles for frontend interface
│   └── readme.md                    # Web interface description
│
└── README.md                        # This documentation
```

---

## ✨ Core Features

<table>
  <thead>
    <tr>
      <th>Module</th>
      <th>Description</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>📷 Camera Capture</td>
      <td>Live photo taken by ESP32-S3’s camera module</td>
    </tr>
    <tr>
      <td>☁️ Cloud Sync</td>
      <td>Image upload to Google Drive with user API token</td>
    </tr>
    <tr>
      <td>🔥 Firebase Integration</td>
      <td>Real-time feeding control, system status, and activity sync</td>
    </tr>
    <tr>
      <td>📟 OLED Interface</td>
      <td>Displays Wi-Fi state, feeding confirmation, drive upload status</td>
    </tr>
    <tr>
      <td>⚙️ Servo Mechanism</td>
      <td>Controls food dispensing quantity</td>
    </tr>
    <tr>
      <td>⚖️ Sensor System</td>
      <td>Load cell and ultrasonic distance sensor detect pet presence and food load</td>
    </tr>
    <tr>
      <td>🧠 Autonomous Logic</td>
      <td>No host PC needed — all processing handled by ESP32</td>
    </tr>
    <tr>
      <td>🌐 Local Web UI</td>
      <td>Lightweight HTML/JS UI served from flash for manual override</td>
    </tr>
  </tbody>
</table>

---

## 🛠 Installation & Deployment

### 📋 Requirements

- Hardware:
  - ESP32-S3 module (with camera support)
  - SG90 Servo Motor
  - HX711 Load Cell Amplifier + ~10kg Load Sensor
  - HC-SR04 Ultrasonic Sensor
  - 128x64 OLED (SSD1306)
  - USB-C power or battery supply

- Software:
  - [Arduino IDE](https://www.arduino.cc/en/software) or [PlatformIO](https://platformio.org/)
  - Board Package: `esp32` by Espressif
  - Required Libraries:
    - [`Firebase ESP Client`](https://github.com/mobizt/Firebase-ESP-Client)
    - [`ESPAsyncWebServer`](https://github.com/me-no-dev/ESPAsyncWebServer)
    - [`Adafruit SSD1306`](https://github.com/adafruit/Adafruit_SSD1306)
    - `ArduinoJson`, `Adafruit GFX`, `AsyncTCP`, Load cell amplifier (HX711), Ultrasonic sensor (HC-SR04), Servo motor and others if specified in `docs/`

### ⚙️ Setup Steps

1. Configure:
   - WiFi: Edit `wifi_setup.h`
   - Firebase: Set `API_KEY`, `DATABASE_URL`, and `USER_EMAIL` in `firebase_setup.h`
   - Google Drive: Paste OAuth2 credentials in `google_drive_upload.h`
2. Build and flash `main.ino` to ESP32-S3 via USB
3. Monitor the Serial console for device IP or errors
4. Access local IP via browser to use web interface

---

## 🧪 Testing & Debugging

- On first boot, verify serial output for correct WiFi connection
- Use `Firebase Console` to check log entries or feeding updates
- Test ultrasonic response and load cell feedback via OLED or Serial
- Attempt image capture and confirm upload in linked Drive folder
- **Feed Your Pet** 😸

---

## 👤 Author

**[Dan Krod]**   
Academic Year: 2025  
Email: [you@example.com]

---

## 📜 License

This project is submitted for educational purposes and is not licensed for commercial redistribution. Please contact the author if you wish to extend or reuse any part of the design.