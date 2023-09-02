#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

const uint16_t kRecvPin = 4;  // GPIO 4 (D2) สำหรับรับสัญญาณ IR
#define Ralay1 D8
IRrecv irrecv(kRecvPin);
int stateCountsown = 0;
int counDown = 15;

unsigned long lastIRTime = 0;
const unsigned long relayDelay = 15000;

void setup() {
  Serial.begin(115200);
  irrecv.enableIRIn();  // เปิดใช้งานรับสัญญาณ IR
  pinMode(Ralay1, OUTPUT);
}

void loop() {
  decode_results results;
  if (irrecv.decode(&results)) {  // รับสัญญาณ IR และถอดรหัส

    if (results.value == 0x12345678) {
      digitalWrite(Ralay1, HIGH);  // เปิดรีเลย์
      Serial.println("Relay ON");
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
        counDown = counDown - 1;

        if (counDown <= 0) {
          digitalWrite(Ralay1, LOW);
          Serial.println("Relay : OFF");
          stateCountsown = 0;
          lastIRTime = 0;
          counDown = 15;
        }
        previousMillis = currentMillis;
      }
    }
  }
}
