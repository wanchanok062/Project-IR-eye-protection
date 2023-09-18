// import library
#include <lvgl.h>  //this library used for create display TFT
#include <TFT_eSPI.h>
#include <ui.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <WiFiManager.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "time.h"

const uint16_t kRecvPin = 14;  // GPIO 4 (D2) สำหรับรับสัญญาณ IR
const int buttonPin = 22;      // ตั้งค่าขาของปุ่มที่คุณใช้
#define Ralay1 21
IRrecv irrecv(kRecvPin);
int stateCountsown = 0;
int counDown = 15;
bool buttonPressed = false;  //button for reset wifi manager
WiFiManager wm;


// -------- Time get ---------
HTTPClient http;
int httpCode;
String payload;

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 25200;
const int daylightOffset_sec = 0;

struct tm previousTimeinfo;  // Stroe Time Befor
bool timeChanged = false;
char timeth[20];
//-------- end ---------------

//------------- function time get -------------
void printLocalTime() {
  delay(1000);
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  if (timeinfo.tm_hour != previousTimeinfo.tm_hour || timeinfo.tm_min != previousTimeinfo.tm_min) {
    timeChanged = true;
    previousTimeinfo = timeinfo;
  } else {
    timeChanged = false;
  }
  if (timeChanged) {
    snprintf(timeth, sizeof(timeth), "%02d:%02d\n", timeinfo.tm_hour, timeinfo.tm_min);
  }
}
// ------------- end -------------

unsigned long lastIRTime = 0;
const unsigned long relayDelay = 15000;

/*Don't forget to set Sketchbook location in File/Preferencesto the path of your UI project (the parent foder of this INO file)*/

/*Change to your screen resolution*/
static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 240;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * screenHeight / 10];

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */

#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char *buf) {
  Serial.printf(buf);
  Serial.flush();
}
#endif

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t *)&color_p->full, w * h, true);
  tft.endWrite();

  lv_disp_flush_ready(disp);
}




void setup() {
  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  
  Serial.begin(115200); /* prepare for possible serial debug */
  pinMode(buttonPin, INPUT_PULLUP);
  irrecv.enableIRIn();  // เปิดใช้งานรับสัญญาณ IR
  pinMode(Ralay1, OUTPUT);
  String LVGL_Arduino = "Hello Arduino! ";
  LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

  Serial.println(LVGL_Arduino);
  Serial.println("I am LVGL_Arduino");

  lv_init();

#if LV_USE_LOG != 0
  lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif

  tft.begin();        /* TFT init */
  tft.setRotation(3); /* Landscape orientation, flipped */

  lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * screenHeight / 10);

  /*Change the following line to your display resolution*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /*Initialize the (dummy) input device driver*/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  lv_indev_drv_register(&indev_drv);
  //----- wifi manager -----
  bool res;
  res = wm.autoConnect("CED_Connect", "CED_config-01");

  if (!res) {
    Serial.println("Failed to connect");
  } else {
    Serial.println("Connected successfully");
  }
  // ----- end wifi manager setup -----

  // ----- get the time ------
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
  // ----- end get the time -----

  ui_init();
  Serial.println("Setup done");
}

void loop() {
  lv_label_set_text(ui_Label1, String(WiFi.SSID()).c_str());
  decode_results results;
  if (irrecv.decode(&results)) {  // รับสัญญาณ IR และถอดรหัส

    if (results.value == 0x12345678) {
      digitalWrite(Ralay1, HIGH);  // เปิดรีเลย์
      Serial.println("Relay ON");
      lv_label_set_text(ui_setStatus, "ON");
      lv_obj_set_style_text_color(ui_setStatus, lv_color_hex(0x0FD10E), LV_PART_MAIN | LV_STATE_DEFAULT);
      lv_bar_set_value(ui_BarState, 0, LV_ANIM_OFF);
      lastIRTime = millis();
      Serial.println(lastIRTime);
      stateCountsown = 1;
      counDown = 15;
    }
    irrecv.resume();
  }

  if (millis() - lastIRTime >= relayDelay) {
    static unsigned long previousMillis = 0;
    unsigned long currentMillis = millis();


    if (stateCountsown == 1) {
      if (currentMillis - previousMillis >= 1000) {
        Serial.println(counDown);
        char countstate[10];                  // สร้างอาร์เรย์สตริงให้เพียงพอสำหรับตัวเลข
        sprintf(countstate, "%d", counDown);  // แปลงตัวแปร counDown เป็นสตริงใน buffer
        lv_label_set_text(ui_setStatus, countstate);
        lv_obj_set_style_text_color(ui_setStatus, lv_color_hex(0xFFBF00), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_bar_set_value(ui_BarState, 15 - counDown, LV_ANIM_OFF);
        counDown = counDown - 1;

        if (counDown <= 0) {
          lv_bar_set_value(ui_BarState, 0, LV_ANIM_OFF);
          digitalWrite(Ralay1, LOW);
          Serial.println("Relay : OFF");
          lv_label_set_text(ui_setStatus, "OFF");
          lv_obj_set_style_text_color(ui_setStatus, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
          stateCountsown = 0;
          lastIRTime = 0;
          counDown = 15;
        }
        previousMillis = currentMillis;
      }
    }
  }
  // ----- button wifi manager -----
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
  // ----- end button wifi manager -----
  lv_label_set_text(ui_setTime, timeth);
  printLocalTime();
  lv_timer_handler(); /* let the GUI do its work */
  delay(5);
}
