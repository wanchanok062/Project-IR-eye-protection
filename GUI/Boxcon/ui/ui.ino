#include <lvgl.h>             //this library used for create display TFT.
#include <TFT_eSPI.h>         //this library for PSI Port.
#include <ui.h>               //this library used for create display GUI.
#include <IRremoteESP8266.h>  //this library used for recive IR.
#include <IRrecv.h>           //this library used for recive IR.
#include <IRutils.h>          //this library used for recive IR.
#include <WiFiManager.h>      //this library used for manage WIFI.
#include <WiFi.h>             //this library used for manage WIFI.
#include <HTTPClient.h>       // this library used for HTTP protocol.
#include <ArduinoJson.h>      // this library used for Json.
#include "time.h"             // this library used for time.
#define TFT_GREY 0x5AEB

//----- IR -----
const uint16_t kRecvPin = 14;  // GPIO 4 (D2) สำหรับรับสัญญาณ IR
#define Relay1 21
IRrecv irrecv(kRecvPin);
//----- End IR -----
int stateCountsown = 0;
int counDown = 15;
bool buttonPressed = false;  //button for reset wifi manager
WiFiManager wm;
const int buttonPin = 22;  // ตั้งค่าขาของปุ่มที่คุณใช้

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
  pinMode(Relay1, OUTPUT);
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
  set_up_wifiManager();
  set_up_getTime();
  ui_init();
  Serial.println("Setup done");
}

void loop() {
  lv_label_set_text(ui_Label1, String(WiFi.SSID()).c_str());
  lv_label_set_text(ui_setTime, timeth);
  IR_recive();
  button_wifi_manager();
  printLocalTime();
  lv_timer_handler(); /* let the GUI do its work */
  delay(5);

}