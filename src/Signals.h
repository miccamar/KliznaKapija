#ifndef SIGNALS_H
#define SIGNALS_H

#include <Arduino.h>
#include "Config.h"
#include "MotorController.h"

enum SoundType {
    SOUND_NONE,
    SOUND_START,
    SOUND_STOP,
    SOUND_LIMIT,
    SOUND_ERROR
};

class Signals {
public:
    Signals();
    void begin();
    void update(MotorState state);
    void playSound(SoundType type);
    void stopSound();

private:
    unsigned long m_lastLedToggle;
    bool m_ledState;

    int m_beepRemaining;
    unsigned long m_beepOnTime;
    unsigned long m_beepOffTime;
    unsigned long m_lastBuzzerToggle;
    bool m_buzzerState;
    bool m_isBeeping;
};

#endif
