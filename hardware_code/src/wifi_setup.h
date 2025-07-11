#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H

#include <WiFi.h>

// Wi-Fi credentials
const char* ssid = "--------";
const char* password = "--------";

void initWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Підключення до Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Підключено до Wi-Fi!");
  Serial.print("IP адреса: ");
  Serial.println(WiFi.localIP());
}

#endif
