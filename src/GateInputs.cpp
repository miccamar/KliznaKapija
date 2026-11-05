#include "GateInputs.h"

GateInputs::GateInputs() 
    : m_limitOpenState(false), m_limitCloseState(false), 
      m_safetyState(false), m_toggleMode(true), m_lastRecoverTime(0) {
    for (int i = 0; i < 8; i++) {
        m_buttonStates[i] = false;
        m_lastButtonStates[i] = false;
        m_lastDebounceTime[i] = 0;
    }
}

void GateInputs::recoverI2C() {
    if (millis() - m_lastRecoverTime < 1000) return; // Ne češće od 1s
    m_lastRecoverTime = millis();

    // 9 SCL clock impulsa za oslobađanje zaglavljene I2C linije (I2C bus recovery standard)
    pinMode(SDA_PIN, INPUT_PULLUP);
    pinMode(SCL_PIN, OUTPUT);
    for (int i = 0; i < 9; i++) {
        digitalWrite(SCL_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(SCL_PIN, LOW);
        delayMicroseconds(10);
    }
    pinMode(SCL_PIN, INPUT_PULLUP);

    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(100000); // 100 kHz standardna brzina, maksimalno otporna na šum
    Wire.setTimeout(100);  // 100ms timeout sprečava blokadu koda

    Wire.beginTransmission(PCF8574_ADDRESS);
    Wire.write(0xFF);
    Wire.endTransmission();
}

bool GateInputs::begin() {
    recoverI2C();
    Wire.beginTransmission(PCF8574_ADDRESS);
    return (Wire.endTransmission() == 0);
}

uint8_t GateInputs::readPCF() {
    // 1. Proveri da li čip odgovara
    Wire.beginTransmission(PCF8574_ADDRESS);
    uint8_t err = Wire.endTransmission();
    if (err != 0) {
        recoverI2C();
        return 0xFF; // Sigurno stanje: svi tasteri otpušteni
    }

    // 2. Zatraži 1 bajt
    uint8_t n = Wire.requestFrom((uint8_t)PCF8574_ADDRESS, (uint8_t)1);
    if (n == 0 || !Wire.available()) {
        recoverI2C();
        return 0xFF;
    }

    uint8_t raw = Wire.read();

    // 3. SANITY CHECK: Nemoguće stanje
    // Pin 2 (Limit Otvoreno) i Pin 3 (Limit Zatvoreno) NE MOGU oba biti 0 (aktivna) u isto vreme!
    // Ako su oba 0, desio se I2C šum / bus latchup.
    bool limOpenActive  = !((raw >> 2) & 1);
    bool limCloseActive = !((raw >> 3) & 1);
    if (limOpenActive && limCloseActive) {
        recoverI2C();
        return 0xFF;
    }

    return raw;
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
