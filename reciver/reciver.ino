#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

const uint16_t kRecvPin = 4;  // GPIO 4 (D2) สำหรับรับสัญญาณ IR
#define Ralay1 D8
IRrecv irrecv(kRecvPin);


void setup() {
  Serial.begin(115200);
  irrecv.enableIRIn();  // เปิดใช้งานรับสัญญาณ IR
  pinMode(Ralay1, OUTPUT);
}

void loop() {
  decode_results results;
  if (irrecv.decode(&results)) {  // รับสัญญาณ IR และถอดรหัส
    Serial.print("Received IR code: 0x");
    Serial.println(results.value, HEX);
    // รับสัญญาณ IR ถัดไป
    
    if (results.value == 0x12345678) {
      // digitalWrite(Ralay1, HIGH);  // เปิดรีเลย์
      Serial.println("Relay ON");
      delay(100);
    }
    irrecv.resume();
  } 
}
