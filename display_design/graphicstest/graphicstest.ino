#include <TFT_eSPI.h>
#include <SPI.h>
TFT_eSPI tft = TFT_eSPI();

void setup() {
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_WHITE);
  
  // Set text color
  tft.setTextColor(TFT_BLACK);
  
  // Set text size
  tft.setTextSize(3);
  
  // Display "hello world" at coordinates (50, 50)
  tft.setCursor(10, 10);
  tft.println("Detil");
}

void loop() {
  // Leave loop() empty to display "hello world" only once.
}
