#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <Arduino.h>
#include "Config.h"

enum MotorState {
    MOTOR_STOP,
    MOTOR_OPEN,
    MOTOR_CLOSE,
    MOTOR_ERROR
};

class MotorController {
public:
    MotorController();
    void begin();
    
    void open();
    void close();
    void stop(bool isError = false);
    
    void update();

    MotorState getState() const { return m_state; }
    int getCurrentSpeed() const { return m_currentSpeed; }
    bool isTimedOut() const { return m_timedOut; }
    void clearError();

    void setOpenProfile(const MotionProfile& prof) { m_openProf = prof; }
    void setCloseProfile(const MotionProfile& prof) { m_closeProf = prof; }
    MotionProfile getOpenProfile() const { return m_openProf; }
    MotionProfile getCloseProfile() const { return m_closeProf; }

private:
    MotorState m_state;
    int m_currentSpeed;
    int m_targetSpeed;
    
    unsigned long m_startTime;
    unsigned long m_lastSpeedStepTime;
    bool m_timedOut;

    MotionProfile m_openProf;
    MotionProfile m_closeProf;

    void applyPWM(int speed);
    void calculateTargetSpeed();
};

#endif
