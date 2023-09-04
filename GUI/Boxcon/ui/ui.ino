#include <lvgl.h>
#include <TFT_eSPI.h>
#include <ui.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

const uint16_t kRecvPin = 14;  // GPIO 4 (D2) สำหรับรับสัญญาณ IR
#define Ralay1 21
IRrecv irrecv(kRecvPin);
int stateCountsown = 0;
int counDown = 15;

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

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
  uint16_t touchX = 0, touchY = 0;

  bool touched = false;  //tft.getTouch( &touchX, &touchY, 600 );

  if (!touched) {
    data->state = LV_INDEV_STATE_REL;
  } else {
    data->state = LV_INDEV_STATE_PR;

    /*Set the coordinates*/
    data->point.x = touchX;
    data->point.y = touchY;

    Serial.print("Data x ");
    Serial.println(touchX);

    Serial.print("Data y ");
    Serial.println(touchY);
  }
}

void setup() {
  Serial.begin(115200); /* prepare for possible serial debug */
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

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
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
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);


  ui_init();

  Serial.println("Setup done");
}

void loop() {

  decode_results results;
  if (irrecv.decode(&results)) {  // รับสัญญาณ IR และถอดรหัส

    if (results.value == 0x12345678) {
      digitalWrite(Ralay1, HIGH);  // เปิดรีเลย์
      Serial.println("Relay ON");
      lv_label_set_text(ui_setStatus, "ON");
      lv_obj_set_style_text_color(ui_setStatus, lv_color_hex(0x0FD10E), LV_PART_MAIN | LV_STATE_DEFAULT);
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

  lv_timer_handler(); /* let the GUI do its work */
  delay(5);
}
