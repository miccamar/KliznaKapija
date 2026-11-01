#include "MotorController.h"

MotorController::MotorController() 
    : m_state(MOTOR_STOP), m_currentSpeed(0), m_targetSpeed(0),
      m_startTime(0), m_lastSpeedStepTime(0), m_timedOut(false),
      m_openProf(DEFAULT_OPEN_PROFILE), m_closeProf(DEFAULT_CLOSE_PROFILE) {}

void MotorController::begin() {
    pinMode(RPWM_PIN, OUTPUT);
    pinMode(LPWM_PIN, OUTPUT);
    pinMode(R_EN_PIN, OUTPUT);
    pinMode(L_EN_PIN, OUTPUT);

    digitalWrite(RPWM_PIN, LOW);
    digitalWrite(LPWM_PIN, LOW);
    digitalWrite(R_EN_PIN, HIGH);
    digitalWrite(L_EN_PIN, HIGH);
}

void MotorController::open() {
    m_state = MOTOR_OPEN;
    m_startTime = millis();
    m_currentSpeed = 0;
    m_targetSpeed = 0;
    m_timedOut = false;
    applyPWM(0);
}

void MotorController::close() {
    m_state = MOTOR_CLOSE;
    m_startTime = millis();
    m_currentSpeed = 0;
    m_targetSpeed = 0;
    m_timedOut = false;
    applyPWM(0);
}

void MotorController::stop(bool isError) {
    m_state = isError ? MOTOR_ERROR : MOTOR_STOP;
    m_timedOut = false; // Uvek poništi flag da ne bi ponovo okidao u loop-u!
    m_currentSpeed = 0;
    m_targetSpeed = 0;
    applyPWM(0);
}

void MotorController::clearError() {
    m_state = MOTOR_STOP;
    m_timedOut = false;
    m_currentSpeed = 0;
    m_targetSpeed = 0;
    applyPWM(0);
}

void MotorController::applyPWM(int speed) {
    speed = constrain(speed, 0, 255);
    if (m_state == MOTOR_OPEN) {
        digitalWrite(RPWM_PIN, LOW);
        analogWrite(LPWM_PIN, speed);
    } else if (m_state == MOTOR_CLOSE) {
        digitalWrite(LPWM_PIN, LOW);
        analogWrite(RPWM_PIN, speed);
    } else {
        digitalWrite(RPWM_PIN, LOW);
        digitalWrite(LPWM_PIN, LOW);
    }
}

void MotorController::calculateTargetSpeed() {
    const MotionProfile& prof = (m_state == MOTOR_OPEN) ? m_openProf : m_closeProf;
    unsigned long elapsed = millis() - m_startTime;

    if (elapsed > prof.maxTimeout) {
        m_timedOut = true;
        applyPWM(0);
        return;
    }

    if (elapsed <= prof.accelTime) {
        m_targetSpeed = map(elapsed, 0, prof.accelTime, prof.minSpeed, prof.maxSpeed);
    } else if (elapsed <= (prof.accelTime + prof.runTime)) {
        m_targetSpeed = prof.maxSpeed;
    } else {
        m_targetSpeed = prof.minSpeed;
    }
}

void MotorController::update() {
    if (m_state == MOTOR_STOP || m_state == MOTOR_ERROR) {
        return;
    }

    calculateTargetSpeed();
    if (m_timedOut) {
        return;
    }

    const MotionProfile& prof = (m_state == MOTOR_OPEN) ? m_openProf : m_closeProf;

    if (millis() - m_lastSpeedStepTime >= prof.stepDelay) {
        m_lastSpeedStepTime = millis();
        if (m_currentSpeed < m_targetSpeed) {
            m_currentSpeed = min(m_currentSpeed + prof.speedStep, m_targetSpeed);
            applyPWM(m_currentSpeed);
        } else if (m_currentSpeed > m_targetSpeed) {
            m_currentSpeed = max(m_currentSpeed - prof.speedStep, m_targetSpeed);
            applyPWM(m_currentSpeed);
        }
    }
}
