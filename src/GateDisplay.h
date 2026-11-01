#ifndef GATE_DISPLAY_H
#define GATE_DISPLAY_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include "Config.h"
#include "MotorController.h"

class GateDisplay {
public:
    GateDisplay();
    bool begin();
    
    void showBootStep(const char* line1, const char* line2 = "");
    void update(MotorState state, int speed, bool limitOpen, bool limitClose, bool autoClose, unsigned long remainingAutoCloseSec);
    void setMessage(const char* msg);

private:
    Adafruit_SSD1306 m_oled;
    char m_message[32];
    unsigned long m_lastUpdate;
    uint8_t getWifiQuality();
};

#endif
