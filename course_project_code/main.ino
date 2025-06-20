#include "esp_camera.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include <FirebaseESP32.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP32Servo.h>
#include <HX711_ADC.h>
#include <EEPROM.h>
#include "Base64.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define CAMERA_MODEL_ESP32S3_EYE // Has PSRAM
#include "camera_pins.h"

// Wi-Fi 
const char* ssid = "--------";
const char* password = "--------";

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Firebase
#define FIREBASE_HOST "----------------------------------------"
#define FIREBASE_AUTH "----------------------------------------"

FirebaseConfig firebase_config;
FirebaseAuth auth;
FirebaseData firebaseData;

// Google Drive
String myScript = "-------------------------------------"; // Вставте URL свого скрипту
String myFoldername = "&myFoldername=ESP32-CAM";
String myFilename = "&myFilename=ESP32-CAM.jpg";
String myImage = "&myFile=";

uint8_t *_jpg_buf = NULL;
size_t _jpg_buf_len = 0;

// Час
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 10800, 60000); // Київ UTC+3

// Серво
Servo myServo;
bool isActive = false;
String scheduledTime = "";
bool hasTriggeredMinute = false;

// HC-SR04
const int trigPin = 1;
const int echoPin = 2;
unsigned long obstacleStartTime = 0;
bool obstacleDetected = false;
bool obstacleWasLongEnough = false;

// Активність
unsigned long lastActivityTime = 0;
const unsigned long INACTIVITY_THRESHOLD = 3600000; 
bool inactivityNotified = false;

// HX711
const int HX711_dout = 47;
const int HX711_sck = 45;
HX711_ADC LoadCell(HX711_dout, HX711_sck);
const int calVal_eepromAdress = 0;
bool weightMeasurementInProgress = false;
unsigned long weightStartTime = 0;
float weightSum = 0;
int weightCount = 0;

// Аварійне реагування
unsigned long lastFeedTime = 0;
const unsigned long FEED_INTERVAL_THRESHOLD = 86400000; 
bool feedIssueNotified = false;

unsigned long lastPollTime = 0;
const unsigned long POLL_INTERVAL = 5000; 

// Камера-сервер
void startCameraServer();
void setupLedFlash(int pin);

bool photoUploaded = false;

unsigned long lastCheck = 0;

static bool photoInProgress = false;

// Глобальні змінні 
unsigned long lastPhotoUploadTime = 0; 
const unsigned long PHOTO_UPLOAD_INTERVAL = 30000; 

unsigned long lastDisplayUpdate = 0;
int displayMode = 0;

float oled_dist = 0.0;

void setup() {
  Serial.begin(115200);
  
  // Wi-Fi
  WiFi.begin(ssid, password);
  EEPROM.begin(512);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\n✅ Wi-Fi підключено!");

  // OLED
  Wire.begin(40, 39); 
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  // Camera init
  camera_config_t camera_config;
  camera_config.ledc_channel = LEDC_CHANNEL_0;
  camera_config.ledc_timer = LEDC_TIMER_0;
  camera_config.pin_d0 = Y2_GPIO_NUM;
  camera_config.pin_d1 = Y3_GPIO_NUM;
  camera_config.pin_d2 = Y4_GPIO_NUM;
  camera_config.pin_d3 = Y5_GPIO_NUM;
  camera_config.pin_d4 = Y6_GPIO_NUM;
  camera_config.pin_d5 = Y7_GPIO_NUM;
  camera_config.pin_d6 = Y8_GPIO_NUM;
  camera_config.pin_d7 = Y9_GPIO_NUM;
  camera_config.pin_xclk = XCLK_GPIO_NUM;
  camera_config.pin_pclk = PCLK_GPIO_NUM;
  camera_config.pin_vsync = VSYNC_GPIO_NUM;
  camera_config.pin_href = HREF_GPIO_NUM;
  camera_config.pin_sccb_sda = SIOD_GPIO_NUM;
  camera_config.pin_sccb_scl = SIOC_GPIO_NUM;
  camera_config.pin_pwdn = PWDN_GPIO_NUM;
  camera_config.pin_reset = RESET_GPIO_NUM;
  camera_config.xclk_freq_hz = 10000000;
  camera_config.pixel_format = PIXFORMAT_JPEG;
  camera_config.frame_size = FRAMESIZE_HVGA;
  camera_config.jpeg_quality = 10;
  camera_config.fb_count = 1;

  if (esp_camera_init(&camera_config) != ESP_OK) {
    Serial.println("❌ Помилка ініціалізації камери!");
    return;
  }
  Serial.println("✅ Камера готова до роботи!");

  startCameraServer();

  Serial.print("📷 Camera Ready! Open 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to view.");

  // Firebase
  firebase_config.host = FIREBASE_HOST;
  firebase_config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&firebase_config, &auth);
  Firebase.reconnectWiFi(true);

  // NTP
  timeClient.begin();

  // Серво
  myServo.attach(21);
  myServo.write(0);

  // HC-SR04
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // HX711
  LoadCell.begin();
  LoadCell.start(5000, true);
  float savedCalibrationValue;
  EEPROM.get(calVal_eepromAdress, savedCalibrationValue);
  LoadCell.setCalFactor(savedCalibrationValue > 0 ? savedCalibrationValue : 115.0);
  while (!LoadCell.update());
  LoadCell.tare();

  lastActivityTime = millis();
}

void loop() {
  // Time
  timeClient.update();
  String now = timeClient.getFormattedTime().substring(0, 5);
  Serial.println("🕒 Поточний час: [" + now + "]");

  // Planned time
  if (Firebase.getString(firebaseData, "/servo/scheduled_time")) {
    scheduledTime = firebaseData.to<String>().substring(0, 5);
    scheduledTime.trim();
    Serial.println("📌 Запланований час: [" + scheduledTime + "]");
  }

  // Servo planned feed
  if (scheduledTime == now && !hasTriggeredMinute) {
    myServo.write(90);
    delay(500);
    myServo.write(0);
    lastFeedTime = millis(); 
    feedIssueNotified = false; 
    hasTriggeredMinute = true;
  }
  if (scheduledTime != now) hasTriggeredMinute = false;

  if (Firebase.getInt(firebaseData, "/servo/state")) {
    isActive = firebaseData.to<int>() == 1;
  }

  if (isActive) {
    myServo.write(90);
    delay(500);
    myServo.write(0);
    lastFeedTime = millis(); 
    feedIssueNotified = false; 
    Firebase.setInt(firebaseData, "/servo/state", 0);

    String feedTimestamp = timeClient.getFormattedTime();
    String path = "/manual_feedings/" + String(millis()); 

    Firebase.setString(firebaseData, path + "/time", feedTimestamp);
    Firebase.setString(firebaseData, path + "/message", "Ручне годування запущено!");

    Serial.println("🍽️ Ручне годування записано у Firebase!");
  }

  checkObstacle();
  checkWeightMeasurement();
  checkInactivity();
  checkFeedingIssue(); 

  checkFirebaseAction();

  if (millis() - lastDisplayUpdate > 5000) {
      lastDisplayUpdate = millis();
      displayMode = (displayMode + 1) % 2; 
      updateDisplay();
  }

  delay(200);
}

// OLED
void updateDisplay() {
    display.clearDisplay();
    display.setTextSize(1.2);
    display.setTextColor(WHITE);

    switch (displayMode) {
      case 0:  
        display.setCursor(5, 5);
        display.println("Time: " + timeClient.getFormattedTime());  
        display.setCursor(5, 20);
        display.println("Planned: " + scheduledTime);  
        break;  

      case 1: 
        display.setCursor(5, 5);
        display.println("Distance: " + String(oled_dist) + " sm");  

        float lastWeight = 0.0;
        if (Firebase.getFloat(firebaseData, "/weight_sensor/weight/last_measurement")) {
          lastWeight = firebaseData.to<float>();
        }

        display.setCursor(5, 20);
        display.println("Weight: " + String(lastWeight) + " g");  
        break;  
    } 

    display.display();
}

// Weight
void checkWeightMeasurement() {
  int activate;
  if (Firebase.getInt(firebaseData, "/weight_sensor/weight/activate")) {
    activate = firebaseData.to<int>();
    if (activate == 1 && !weightMeasurementInProgress) {
      weightMeasurementInProgress = true;
      weightStartTime = millis();
      weightSum = 0;
      weightCount = 0;
      Serial.println("⚖️ Початок зважування протягом 10 секунд...");
    }
  }

  if (weightMeasurementInProgress) {
    if (LoadCell.update()) {
      float value = LoadCell.getData();
      weightSum += value;
      weightCount++;
    }

    if (millis() - weightStartTime > 10000) {
      float averageWeight = weightCount > 0 ? weightSum / weightCount : 0;
      String timeNow = timeClient.getFormattedTime();
      String path = "/weight_sensor/weight_logs/" + String(millis());

      Firebase.setFloat(firebaseData, "/weight_sensor/weight/last_measurement", averageWeight);
      Firebase.setFloat(firebaseData, path + "/weight", averageWeight);
      Firebase.setString(firebaseData, path + "/time", timeNow);
      Firebase.setInt(firebaseData, "/weight_sensor/weight/activate", 0);

      Serial.println("✅ Середня вага: " + String(averageWeight));
      weightMeasurementInProgress = false;
    }
  }
}

// Рhoto notification
void checkFirebaseAction() {
  if (photoInProgress) return;  

  if (Firebase.getBool(firebaseData, "/cameraControl/action") && firebaseData.boolData()) {
    Serial.println("📸 Отримано запит на фото");

    photoInProgress = true;

    uploadPhotoToGoogleDrive();  

    Firebase.setBool(firebaseData, "/cameraControl/action", false);

    photoInProgress = false;
  }
}

// Obstacle
void checkObstacle() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2;
  
  oled_dist = distance;

  Serial.print("📏 Відстань: ");
  Serial.print(distance);
  Serial.println(" см");

  if (distance > 0 && distance <= 30.0) {
    if (!obstacleDetected) {
      obstacleStartTime = millis();
      obstacleDetected = true;
      obstacleWasLongEnough = false;
      photoUploaded = false; 
    } else {
      unsigned long obstacleDuration = millis() - obstacleStartTime;

      if (obstacleDuration >= 5000 && !obstacleWasLongEnough) {
        obstacleWasLongEnough = true;
        logObstacle(obstacleDuration / 1000);
      }
    }
  } else { 
    obstacleDetected = false;
    obstacleWasLongEnough = false;
    photoUploaded = false; 
  }
}

// Log obctacle
void logObstacle(int durationSeconds) {
  String timestamp = timeClient.getFormattedTime();
  String path = "/obstacle_logs/" + String(millis());

  Firebase.setString(firebaseData, path + "/time", timestamp);
  Firebase.setInt(firebaseData, path + "/duration", durationSeconds);

  if (durationSeconds >= 5) {
    Firebase.setBool(firebaseData, "/notifications/animal_detected/active", true);
    Firebase.setString(firebaseData, "/notifications/animal_detected/time", timestamp);
    Firebase.setString(firebaseData, "/notifications/animal_detected/message", "🐾 Ваша тварина прийшла! Хочете глянути");
  }

  Serial.println("🧱 Перешкода записана у Firebase. Тривалість: " + String(durationSeconds) + " с");
}

// Inactivity (additional)
void checkInactivity() {
  if ((millis() - lastActivityTime >= INACTIVITY_THRESHOLD) && !inactivityNotified) {
    String timestamp = timeClient.getFormattedTime();
    Firebase.setString(firebaseData, "/notifications/no_activity/time", timestamp);
    Firebase.setString(firebaseData, "/notifications/no_activity/message", "⚠️ Тварина не підходила понад годину!");

    Serial.println("⚠️ УВАГА: Тварина не підходила понад годину!");
    inactivityNotified = true;
  }
}

// Feeding issue (additional)
void checkFeedingIssue() {
  if ((millis() - lastFeedTime >= FEED_INTERVAL_THRESHOLD) && !feedIssueNotified) {
    String timestamp = timeClient.getFormattedTime();
    Firebase.setString(firebaseData, "/notifications/feed_issue/time", timestamp);
    Firebase.setString(firebaseData, "/notifications/feed_issue/message", "🚨 Корм не подавався понад 24 години!");

    Serial.println("🚨 АВАРІЯ: Корм не подавався понад 24 години!");
    feedIssueNotified = true;
  }
}

// Google Apps Script
void uploadPhotoToGoogleDrive() {
  const char* myDomain = "script.google.com";
  String getAll="", getBody = "";

  camera_fb_t * fb = NULL;
  int retry = 5; // К-сть спроб кадру

  while (retry-- && !fb) {
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("❌ Помилка отримання кадру! Спробую ще раз...");
      delay(500);
    }
  }

  if (!fb) {
    Serial.println("❌ Камера все ще не відповідає. Перезавантажте ESP32.");
    return;
  }

  if (fb->format != PIXFORMAT_JPEG) {
    Serial.println("❌ Формат зображення не JPEG. Виконую конвертацію...");
    bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
    esp_camera_fb_return(fb);
    fb = NULL;
    if (!jpeg_converted) {
      Serial.println("❌ Помилка конвертації в JPEG!");
      return;
    }
  }

  Serial.println("✅ Фото отримано! Передаю у Google Drive...");
  
  WiFiClientSecure client_tcp;
  client_tcp.setInsecure();

  if (client_tcp.connect(myDomain, 443)) {
    Serial.println("✅ З'єднання успішне!");

    char *input = (char *)fb->buf;
    char output[base64_enc_len(3)];
    String imageFile = "data:image/jpeg;base64,";
    Serial.println("Передача фото: " + imageFile);
    Serial.println("Base64 довжина: " + String(imageFile.length()));
    for (int i = 0; i < fb->len; i += 3) {
      int len = (i + 3 <= fb->len) ? 3 : fb->len - i;
      base64_encode(output, (char*)(fb->buf + i), len);
      imageFile += urlencode(String(output));
    }

    String Data = "---------------------------------";

    client_tcp.println("POST " + myScript + " HTTP/1.1");
    client_tcp.println("Host: " + String(myDomain));
    client_tcp.println("Content-Length: " + String(Data.length() + imageFile.length()));
    client_tcp.println("Content-Type: application/x-www-form-urlencoded");
    client_tcp.println("Connection: keep-alive");
    client_tcp.println();
    
    client_tcp.print(Data);
    for (int Index = 0; Index < imageFile.length(); Index += 1000) {
      client_tcp.print(imageFile.substring(Index, Index + 1000));
    }
    esp_camera_fb_return(fb);

    int waitTime = 10000;
    long startTime = millis();
    boolean state = false;

    while ((startTime + waitTime) > millis()) {
      Serial.print(".");
      delay(100);
      while (client_tcp.available()) {
        char c = client_tcp.read();
        if (state) getBody += String(c);
        if (c == '\n') {
          if (getAll.length() == 0) state = true;
          getAll = "";
        } else if (c != '\r') {
          getAll += String(c);
        }
        startTime = millis();
      }
      if (getBody.length() > 0) break;
    }
    client_tcp.stop();
    Serial.println(getBody);
  } else {
    Serial.println("❌ Помилка підключення до Google Apps Script");
  }
}

// Кодування URL
String urlencode(String str) {
  String encodedString = "";
  char c;
  char code0;
  char code1;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == ' ') {
      encodedString += '+';
    } else if (isalnum(c)) {
      encodedString += c;
    } else {
      code1 = (c & 0xf) + '0';
      if ((c & 0xf) > 9) {
        code1 = (c & 0xf) - 10 + 'A';
      }
      c = (c >> 4) & 0xf;
      code0 = c + '0';
      if (c > 9) {
        code0 = c - 10 + 'A';
      }
      encodedString += '%';
      encodedString += code0;
      encodedString += code1;
    }
  }
  return encodedString;
}