#ifndef GATE_INPUTS_H
#define GATE_INPUTS_H

#include <Arduino.h>
#include <Wire.h>
#include "Config.h"

enum ButtonEvent {
    EVENT_NONE,
    EVENT_WALL_BUTTON,
    EVENT_RF_OPEN,
    EVENT_RF_CLOSE,
    EVENT_RF_PEDESTRIAN,
    EVENT_RF_STOP,
    EVENT_LIMIT_OPEN_HIT,
    EVENT_LIMIT_CLOSE_HIT,
    EVENT_SAFETY_TRIGGERED
};

class GateInputs {
public:
    GateInputs();
    bool begin();
    ButtonEvent update();
    void recoverI2C();

    bool isLimitOpen() const { return m_limitOpenState; }
    bool isLimitClose() const { return m_limitCloseState; }
    bool isSafetyBeamBlocked() const { return m_safetyState; }

    bool getToggleMode() const { return m_toggleMode; }
    void setToggleMode(bool enabled) { m_toggleMode = enabled; }

private:
    uint8_t readPCF();

    bool m_buttonStates[8];
    bool m_lastButtonStates[8];
    unsigned long m_lastDebounceTime[8];
    const unsigned long m_debounceDelay = 50;

    bool m_limitOpenState;
    bool m_limitCloseState;
    bool m_safetyState;
    bool m_toggleMode;
    unsigned long m_lastRecoverTime;
};

#endif
