#ifndef UTILS_H
#define UTILS_H

#include <FirebaseESP32.h>
#include <NTPClient.h>

// 🔗 External variables
extern FirebaseData firebaseData;
extern NTPClient timeClient;

extern String scheduledTime;
extern unsigned long lastActivityTime;
extern bool inactivityNotified;
extern const unsigned long INACTIVITY_THRESHOLD;

extern unsigned long lastFeedTime;
extern bool feedIssueNotified;
extern const unsigned long FEED_INTERVAL_THRESHOLD;

// 🕒 Update and display the current time
inline String updateAndPrintTime() {
  timeClient.update();
  String now = timeClient.getFormattedTime().substring(0, 5);
  Serial.println("🕒 Поточний час: [" + now + "]");
  return now;
}

// 📌 Get scheduled feeding times from Firebase
inline void fetchScheduledTime() {
  if (Firebase.getString(firebaseData, "/servo/scheduled_time")) {
    scheduledTime = firebaseData.to<String>().substring(0, 5);
    scheduledTime.trim();
    Serial.println("📌 Запланований час: [" + scheduledTime + "]");
  }
}

// ⚠️ Checking for inactivity
inline void checkInactivity() {
  if ((millis() - lastActivityTime >= INACTIVITY_THRESHOLD) && !inactivityNotified) {
    String timestamp = timeClient.getFormattedTime();
    Firebase.setString(firebaseData, "/notifications/no_activity/time", timestamp);
    Firebase.setString(firebaseData, "/notifications/no_activity/message", "⚠️ Тварина не підходила понад годину!");
    Serial.println("⚠️ УВАГА: Тварина не підходила понад годину!");
    inactivityNotified = true;
  }
}

// 🚨 Checking for a delay in feeding
inline void checkFeedingIssue() {
  if ((millis() - lastFeedTime >= FEED_INTERVAL_THRESHOLD) && !feedIssueNotified) {
    String timestamp = timeClient.getFormattedTime();
    Firebase.setString(firebaseData, "/notifications/feed_issue/time", timestamp);
    Firebase.setString(firebaseData, "/notifications/feed_issue/message", "🚨 Корм не подавався понад 24 години!");
    Serial.println("🚨 АВАРІЯ: Корм не подавався понад 24 години!");
    feedIssueNotified = true;
  }
}

#endif