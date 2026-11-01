#include "Signals.h"

Signals::Signals() 
    : m_lastLedToggle(0), m_ledState(false),
      m_beepRemaining(0), m_beepOnTime(0), m_beepOffTime(0),
      m_lastBuzzerToggle(0), m_buzzerState(false), m_isBeeping(false) {}

void Signals::begin() {
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
}

void Signals::stopSound() {
    m_isBeeping = false;
    m_beepRemaining = 0;
    m_buzzerState = false;
    digitalWrite(BUZZER_PIN, LOW);
}

void Signals::playSound(SoundType type) {
    switch (type) {
        case SOUND_START:
            m_beepRemaining = 1;
            m_beepOnTime = 300;
            m_beepOffTime = 50;
            break;
        case SOUND_STOP:
            m_beepRemaining = 2;
            m_beepOnTime = 100;
            m_beepOffTime = 100;
            break;
        case SOUND_LIMIT:
            m_beepRemaining = 3;
            m_beepOnTime = 100;
            m_beepOffTime = 100;
            break;
        case SOUND_ERROR:
            // 30 bipova x (250ms ON + 250ms OFF) = tačno 15 sekundi zvučnog signala!
            m_beepRemaining = 30;
            m_beepOnTime = 250;
            m_beepOffTime = 250;
            break;
        default:
            m_beepRemaining = 0;
            break;
    }
    m_isBeeping = (m_beepRemaining > 0);
    m_buzzerState = true;
    digitalWrite(BUZZER_PIN, HIGH);
    m_lastBuzzerToggle = millis();
}

void Signals::update(MotorState state) {
    unsigned long now = millis();

    // 1. LED lampa: kod MOTOR_ERROR gori stalno, kod MOTOR_STOP je ugašena, u radu treperi
    if (state == MOTOR_ERROR) {
        digitalWrite(LED_PIN, HIGH);
    } else if (state == MOTOR_STOP) {
        digitalWrite(LED_PIN, LOW);
    } else {
        unsigned long interval = (state == MOTOR_OPEN) ? 800 : 300;
        if (now - m_lastLedToggle >= interval) {
            m_lastLedToggle = now;
            m_ledState = !m_ledState;
            digitalWrite(LED_PIN, m_ledState ? HIGH : LOW);
        }
    }

    // 2. Neblokirajući Buzzer
    if (m_isBeeping) {
        if (m_buzzerState && (now - m_lastBuzzerToggle >= m_beepOnTime)) {
            digitalWrite(BUZZER_PIN, LOW);
            m_buzzerState = false;
            m_lastBuzzerToggle = now;
            m_beepRemaining--;
            if (m_beepRemaining <= 0) {
                m_isBeeping = false;
            }
        } else if (!m_buzzerState && (now - m_lastBuzzerToggle >= m_beepOffTime)) {
            digitalWrite(BUZZER_PIN, HIGH);
            m_buzzerState = true;
            m_lastBuzzerToggle = now;
        }
    } else {
        digitalWrite(BUZZER_PIN, LOW);
    }
}
