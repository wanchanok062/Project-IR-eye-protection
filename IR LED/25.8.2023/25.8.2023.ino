#include <Wire.h>
#include <MPU6050.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#define IrLedPin 3

IRsend irsend(IrLedPin);

unsigned long previousMillis = 0;
const long interval = 3000;  // 0.25 sec
bool ledState = LOW;

MPU6050 mpu;


void sendIRCode() {
  // ส่งรหัส IR ของตัวเองที่คุณได้รับจากการบันทึก
  // ตัวอย่าง: คำสั่งส่งรหัส IR ที่มีความยาว 32 บิต
  uint32_t customIRCode = 0x12345678;
  delay(500);
  irsend.sendNEC(customIRCode, 32);
}

void setup() {
  Wire.begin(0, 2);  // Initialize I2C communication on GPIO 0 (SDA) and GPIO 2 (SCL)
  Serial.begin(9600);
  mpu.initialize();
  pinMode(IrLedPin, OUTPUT);
  digitalWrite(IrLedPin, 0);

  // if (mpu.testConnection()) {
  //   // Serial.println("MPU6050 connection successful");
  // } else {
  //   // Serial.println("MPU6050 connection failed");
  //   while (1)
  //     ;
  // }
}

void loop() {
  // sendIRCode();
  // int16_t ax, ay, az;
  int16_t gx, gy, gz;

  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  float accelerationX = (float)ax / 16384.0; 

  unsigned long currentMillis = millis();

  // Serial.println(accelerationX);
  if (accelerationX <= 0.4) {
    digitalWrite(IrLedPin, LOW);
  } else {
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      ledState = !ledState;
      // digitalWrite(IrLedPin, ledState);
      sendIRCode();  // ส่งรหัส IR เมื่อค่า accelerationX มากกว่า 0.50
      delay(500);
      digitalWrite(IrLedPin, LOW);
    }
  }
}
