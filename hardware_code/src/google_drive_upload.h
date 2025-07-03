#ifndef GOOGLE_DRIVE_UPLOAD_H
#define GOOGLE_DRIVE_UPLOAD_H

#include <WiFiClientSecure.h>
#include <FirebaseESP32.h>
#include "esp_camera.h"
#include "Base64.h"
#include "image_processing.h"

// 🔗 External variables
extern FirebaseData firebaseData;
extern String myScript, myFoldername, myFilename, myImage;
extern uint8_t* _jpg_buf;
extern size_t _jpg_buf_len;

inline void uploadPhotoToGoogleDrive() {
  const char* myDomain = "----------------";
  String getAll = "", getBody = "";

  camera_fb_t* fb = nullptr;
  for (int retry = 0; retry < 5 && !fb; retry++) {
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("❌ Помилка отримання кадру! Спробую ще раз...");
      delay(500);
    }
  }
  if (!fb) {
    Serial.println("❌ Камера не відповідає. Перезавантажте ESP32.");
    return;
  }

  if (fb->format != PIXFORMAT_JPEG) {
    Serial.println("❌ Формат зображення не JPEG. Конвертація...");
    bool ok = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
    esp_camera_fb_return(fb);
    fb = nullptr;
    if (!ok) {
      Serial.println("❌ Помилка конвертації в JPEG!");
      return;
    }
  }

  Serial.println("✅ Фото отримано! Передаю у Google Drive...");

  WiFiClientSecure client;
  client.setInsecure();

  if (client.connect(myDomain, 443)) {
    Serial.println("✅ З'єднання успішне!");

    String imageFile = "data:image/jpeg;base64,";
    char output[base64_enc_len(3)];
    for (int i = 0; i < fb->len; i += 3) {
      int len = (i + 3 <= fb->len) ? 3 : fb->len - i;
      base64_encode(output, (char*)(fb->buf + i), len);
      imageFile += urlencode(String(output));
    }

    String Data = myFoldername + myFilename + myImage;
    client.println("POST " + myScript + " HTTP/1.1");
    client.println("Host: " + String(myDomain));
    client.println("Content-Length: " + String(Data.length() + imageFile.length()));
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Connection: keep-alive");
    client.println();

    client.print(Data);
    for (int i = 0; i < imageFile.length(); i += 1000) {
      client.print(imageFile.substring(i, i + 1000));
    }

    esp_camera_fb_return(fb);

    long startTime = millis();
    const int waitTime = 10000;
    bool state = false;

    while ((millis() - startTime) < waitTime) {
      delay(100);
      while (client.available()) {
        char c = client.read();
        if (state) getBody += String(c);
        if (c == '\n') state = true;
      }
      if (getBody.length() > 0) break;
    }

    Serial.println("📬 Відповідь: " + getBody);
    client.stop();
  } else {
    Serial.println("❌ Помилка підключення до Google Apps Script");
  }
}

#endif