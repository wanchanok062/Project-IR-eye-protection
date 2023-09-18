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