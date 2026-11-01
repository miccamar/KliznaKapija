#include "GateInputs.h"

GateInputs::GateInputs() 
    : m_limitOpenState(false), m_limitCloseState(false), 
      m_safetyState(false), m_toggleMode(true) {
    for (int i = 0; i < 8; i++) {
        m_buttonStates[i] = false;
        m_lastButtonStates[i] = false;
        m_lastDebounceTime[i] = 0;
    }
}

bool GateInputs::begin() {
    Wire.beginTransmission(PCF8574_ADDRESS);
    if (Wire.endTransmission() == 0) {
        Wire.beginTransmission(PCF8574_ADDRESS);
        Wire.write(0xFF);
        Wire.endTransmission();
        return true;
    }
    return false;
}

uint8_t GateInputs::readPCF() {
    Wire.requestFrom((uint8_t)PCF8574_ADDRESS, (uint8_t)1);
    return Wire.available() ? Wire.read() : 0xFF;
}

ButtonEvent GateInputs::update() {
    uint8_t raw = readPCF();
    ButtonEvent detectedEvent = EVENT_NONE;

    for (int i = 0; i < 8; i++) {
        #if !ENABLE_WALL_BUTTON
        if (i == 0) continue;
        #endif

        #if !ENABLE_RF_PEDESTRIAN
        if (i == 1) continue;
        #endif

        #if !ENABLE_SAFETY_BEAM
        if (i == 7) continue;
        #endif

        bool pressed = !((raw >> i) & 1);

        if (pressed != m_lastButtonStates[i]) {
            m_lastDebounceTime[i] = millis();
        }

        if ((millis() - m_lastDebounceTime[i]) > m_debounceDelay) {
            if (pressed != m_buttonStates[i]) {
                m_buttonStates[i] = pressed;

                if (m_buttonStates[i]) {
                    switch (i) {
                        #if ENABLE_WALL_BUTTON
                        case 0: detectedEvent = EVENT_WALL_BUTTON; break;
                        #endif
                        #if ENABLE_RF_PEDESTRIAN
                        case 1: detectedEvent = EVENT_RF_PEDESTRIAN; break;
                        #endif
                        case 2: detectedEvent = EVENT_LIMIT_OPEN_HIT; break;
                        case 3: detectedEvent = EVENT_LIMIT_CLOSE_HIT; break;
                        case 4: detectedEvent = EVENT_RF_OPEN; break;
                        case 5: detectedEvent = EVENT_RF_CLOSE; break;
                        case 6: detectedEvent = EVENT_RF_STOP; break;
                        #if ENABLE_SAFETY_BEAM
                        case 7: detectedEvent = EVENT_SAFETY_TRIGGERED; break;
                        #endif
                        default: break;
                    }
                }
            }
        }
        m_lastButtonStates[i] = pressed;
    }

    m_limitOpenState  = m_buttonStates[2];
    m_limitCloseState = m_buttonStates[3];

    #if ENABLE_SAFETY_BEAM
    m_safetyState = ((raw >> 7) & 1);
    #else
    m_safetyState = false;
    #endif

    return detectedEvent;
}
