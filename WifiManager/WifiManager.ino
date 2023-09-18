#include <WiFiManager.h>  // https://github.com/tzapu/WiFiManager
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "time.h"

const int buttonPin = 22;  // ตั้งค่าขาของปุ่มที่คุณใช้
bool buttonPressed = false;
WiFiManager wm;
// Make GET HTTP TIME
HTTPClient http;
int httpCode;
String payload;

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 25200;
const int daylightOffset_sec = 0; 

struct tm previousTimeinfo;  // Stroe Time Befor
bool timeChanged = false;
char timeth[20];

void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  // เปรียบเทียบเวลากับเวลาก่อนหน้า
  if (timeinfo.tm_hour != previousTimeinfo.tm_hour || timeinfo.tm_min != previousTimeinfo.tm_min) {
    timeChanged = true;
    previousTimeinfo = timeinfo;
  } else {
    timeChanged = false;
  }

  // พิมพ์เวลาเฉพาะเมื่อมีการเปลี่ยนแปลง
  if (timeChanged) {
    snprintf(timeth, sizeof(timeth), "%02d:%02d\n", timeinfo.tm_hour, timeinfo.tm_min);
  }
}

void setup() {

  // put your setup code here, to run once:
  Serial.begin(115200);

  // ตั้งค่าขาปุ่มเป็นอินพุต
  pinMode(buttonPin, INPUT_PULLUP);

  //WiFiManager, Local initialization. Once its business is done, there is no need to keep it around

  // reset settings - wipe stored credentials for testing
  // these are stored by the esp library
  // wm.resetSettings();

  // Automatically connect using saved credentials,
  // if connection fails, it starts an access point with the specified name ("AutoConnectAP"),
  // if empty will auto generate SSID, if password is blank it will be an anonymous AP (wm.autoConnect())
  // then goes into a blocking loop awaiting configuration and will return success result

  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  res = wm.autoConnect("CED_Connect", "CED_config-01");  // password protected ap

  if (!res) {
    Serial.println("Failed to connect");
  } else {
    Serial.println("Connected successfully");
  }

  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
}

void loop() {
  // ตรวจสอบการกดปุ่ม
  // Serial.println(digitalRead(buttonPin));
  if (digitalRead(buttonPin) == 0) {  // ปุ่มถูกกด
    buttonPressed = true;
  } else {                // ปุ่มไม่ถูกกด
    if (buttonPressed) {  // ถ้าปุ่มถูกกดมาก่อนหน้า
      wm.resetSettings();
      Serial.println("WiFi credentials reset. Restarting...");
      ESP.restart();
    }
    buttonPressed = false;  // รีเซ็ตสถานะปุ่ม
  }
  Serial.println(timeth);
  delay(1000);
  printLocalTime();
}
