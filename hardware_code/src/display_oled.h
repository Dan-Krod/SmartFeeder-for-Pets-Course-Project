#ifndef DISPLAY_OLED_H
#define DISPLAY_OLED_H

#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <FirebaseESP32.h>
#include <NTPClient.h>

extern Adafruit_SSD1306 display;
extern NTPClient timeClient;
extern FirebaseData firebaseData;
extern String scheduledTime;
extern float oled_dist;

extern int displayMode;
extern unsigned long lastDisplayUpdate;

inline void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
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

#endif