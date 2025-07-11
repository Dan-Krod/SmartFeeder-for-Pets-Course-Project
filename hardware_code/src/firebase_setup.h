#ifndef FIREBASE_SETUP_H
#define FIREBASE_SETUP_H

#include <FirebaseESP32.h>

// 🔐 Replace with your own credentials
#define FIREBASE_HOST "----------------------------------------"
#define FIREBASE_AUTH "----------------------------------------"

// Configuration objects
FirebaseConfig firebase_config;
FirebaseAuth auth;
FirebaseData firebaseData;

inline void initFirebase() {
  firebase_config.host = FIREBASE_HOST;
  firebase_config.signer.tokens.legacy_token = FIREBASE_AUTH;

  Firebase.begin(&firebase_config, &auth);
  Firebase.reconnectWiFi(true);

  Serial.println("✅ Firebase is initialised!");
}

#endif