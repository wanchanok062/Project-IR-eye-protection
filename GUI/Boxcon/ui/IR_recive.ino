void IR_recive()
{   decode_results results;
    if (irrecv.decode(&results))
    { // รับสัญญาณ IR และถอดรหัส

        if (results.value == 0x12345678)
        {
            digitalWrite(Relay1, HIGH);
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

    if (millis() - lastIRTime >= relayDelay)
    {
        static unsigned long previousMillis = 0;
        unsigned long currentMillis = millis();

        if (stateCountsown == 1)
        {
            if (currentMillis - previousMillis >= 1000)
            {
                Serial.println(counDown);
                char countstate[10];                 // สร้างอาร์เรย์สตริงให้เพียงพอสำหรับตัวเลข
                sprintf(countstate, "%d", counDown); // แปลงตัวแปร counDown เป็นสตริงใน buffer
                lv_label_set_text(ui_setStatus, countstate);
                lv_obj_set_style_text_color(ui_setStatus, lv_color_hex(0xFFBF00), LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_bar_set_value(ui_BarState, 15 - counDown, LV_ANIM_OFF);
                counDown = counDown - 1;

                if (counDown <= 0)
                {
                    lv_bar_set_value(ui_BarState, 0, LV_ANIM_OFF);
                    digitalWrite(Relay1, LOW);
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
}
