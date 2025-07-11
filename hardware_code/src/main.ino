#include "includes.h"

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
String myScript = "-------------------------------------"; 
String myFoldername = "&myFoldername=ESP32-CAM";
String myFilename = "&myFilename=ESP32-CAM.jpg";
String myImage = "&myFile=";

uint8_t *_jpg_buf = NULL;
size_t _jpg_buf_len = 0;

// Час
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 10800, 60000); 

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

// Emergency response
unsigned long lastFeedTime = 0;
const unsigned long FEED_INTERVAL_THRESHOLD = 86400000; 
bool feedIssueNotified = false;

unsigned long lastPollTime = 0;
const unsigned long POLL_INTERVAL = 5000; 

// Camera server
void startCameraServer();
void setupLedFlash(int pin);

bool photoUploaded = false;

unsigned long lastCheck = 0;

static bool photoInProgress = false;

// Global variables
unsigned long lastPhotoUploadTime = 0; 
const unsigned long PHOTO_UPLOAD_INTERVAL = 30000; 

unsigned long lastDisplayUpdate = 0;
int displayMode = 0;

float oled_dist = 0.0;

void setup() {
  Serial.begin(115200);
  
  // Wi-Fi
  initWiFi();

  // OLED
  Wire.begin(40, 39); 
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  // Camera init
  if (!setupCamera()) return;

  startCameraServer();

  Serial.print("📷 Camera Ready! Open 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to view.");

  // Firebase
  initFirebase();

  // NTP
  timeClient.begin();

  // Servo + HC-SR04 + HX711
  initServoWeightUltrasonic();

  lastActivityTime = millis();
}

void loop() {
  // Time
  String now = updateAndPrintTime();

  // Planned time
  fetchScheduledTime();

  handleScheduledFeeding();
  checkManualFeeding(isActive);
  checkWeightMeasurement();
  checkObstacle();

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