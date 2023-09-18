void set_up_wifiManager() {
  bool res;
  res = wm.autoConnect("CED_Connect", "CED_config-01");

  if (!res) {
    Serial.println("Failed to connect");
  } else {
    Serial.println("Connected successfully");
  }
}

void set_up_getTime() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
}

void button_wifi_manager() {
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
}
