#ifndef SERVO_SCALE_SONIC_H
#define SERVO_SCALE_SONIC_H

#include <ESP32Servo.h>
#include <HX711_ADC.h>
#include <EEPROM.h>
#include <FirebaseESP32.h>
#include <NTPClient.h>

extern Servo myServo;
extern HX711_ADC LoadCell;
extern int calVal_eepromAdress;
extern FirebaseData firebaseData;
extern NTPClient timeClient;
extern String scheduledTime;
extern bool hasTriggeredMinute;

extern int trigPin, echoPin;
extern float oled_dist;

extern unsigned long lastFeedTime;
extern bool feedIssueNotified;

extern bool weightMeasurementInProgress;
extern unsigned long weightStartTime;
extern float weightSum;
extern int weightCount;

// 🟢 Initialising components
inline void initServoWeightUltrasonic() {
  // Servo
  myServo.attach(21);
  myServo.write(0);

  // HX711
  LoadCell.begin();
  LoadCell.start(5000, true);
  float savedCalibrationValue;
  EEPROM.get(calVal_eepromAdress, savedCalibrationValue);
  LoadCell.setCalFactor(savedCalibrationValue > 0 ? savedCalibrationValue : 115.0);
  while (!LoadCell.update());
  LoadCell.tare();

  // HC-SR04
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

// 🕒 Handling planned feeding
inline void handleScheduledFeeding() {
  String now = timeClient.getFormattedTime().substring(0, 5);

  if (scheduledTime == now && !hasTriggeredMinute) {
    myServo.write(90);
    delay(500);
    myServo.write(0);
    lastFeedTime = millis();
    feedIssueNotified = false;
    hasTriggeredMinute = true;
  }
  if (scheduledTime != now) hasTriggeredMinute = false;
}

// ⚖️ Weighing processing
inline void checkWeightMeasurement() {
  if (Firebase.getInt(firebaseData, "/weight_sensor/weight/activate")) {
    int activate = firebaseData.to<int>();
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
      float avgWeight = weightCount > 0 ? weightSum / weightCount : 0;
      String timeNow = timeClient.getFormattedTime();
      String path = "/weight_sensor/weight_logs/" + String(millis());

      Firebase.setFloat(firebaseData, "/weight_sensor/weight/last_measurement", avgWeight);
      Firebase.setFloat(firebaseData, path + "/weight", avgWeight);
      Firebase.setString(firebaseData, path + "/time", timeNow);
      Firebase.setInt(firebaseData, "/weight_sensor/weight/activate", 0);

      Serial.println("✅ Середня вага: " + String(avgWeight));
      weightMeasurementInProgress = false;
    }
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

// 🧪 Manual feeding test
inline void checkManualFeeding(bool& isActive) {
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

    String timestamp = timeClient.getFormattedTime();
    String path = "/manual_feedings/" + String(millis());

    Firebase.setString(firebaseData, path + "/time", timestamp);
    Firebase.setString(firebaseData, path + "/message", "Ручне годування запущено!");
    Serial.println("🍽️ Ручне годування записано у Firebase!");
  }
}

#endif