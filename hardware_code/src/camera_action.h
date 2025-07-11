#ifndef CAMERA_ACTION_H
#define CAMERA_ACTION_H

#include <FirebaseESP32.h>

extern FirebaseData firebaseData;
extern bool photoInProgress;

void uploadPhotoToGoogleDrive();

inline void checkFirebaseAction() {
  if (photoInProgress) return;

  if (Firebase.getBool(firebaseData, "/cameraControl/action") && firebaseData.boolData()) {
    Serial.println("📸 Отримано запит на фото");

    photoInProgress = true;

    uploadPhotoToGoogleDrive();

    Firebase.setBool(firebaseData, "/cameraControl/action", false);

    photoInProgress = false;
  }
}

#endif