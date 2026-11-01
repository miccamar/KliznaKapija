#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ArduinoOTA.h>
#include "Config.h"
#include "ConfigManager.h"
#include "MotorController.h"
#include "GateInputs.h"
#include "Signals.h"
#include "GateDisplay.h"
#include "GateWebServer.h"

MotorController motor;
GateInputs      inputs;
Signals         signals;
GateDisplay     displayOLED;
GateWebServer   webServer;
SystemSettings  settings;

unsigned long gateFullyOpenedTime = 0;
bool gateIsFullyOpen = false;
unsigned long lastStatusBroadcast = 0;

// Tajmer za automatski reset ERROR stanja nakon 15 sekundi
unsigned long errorStartTime = 0;
const unsigned long ERROR_DURATION_MS = 15000; // 15 sekundi

// Promenljive za pešački mod
bool isPedestrianMode = false;
unsigned long pedestrianStartTime = 0;
unsigned long pedestrianStopTime = 0;

MotorState lastDirection = MOTOR_CLOSE;

void notifyWeb() {
    unsigned long remainingSec = 0;
    if (settings.autoCloseEnabled && gateIsFullyOpen && motor.getState() == MOTOR_STOP) {
        unsigned long elapsed = millis() - gateFullyOpenedTime;
        unsigned long totalMs = (unsigned long)settings.autoCloseMinutes * 60000;
        if (elapsed < totalMs) remainingSec = (totalMs - elapsed) / 1000;
    }

    const char* msg = (motor.getState() == MOTOR_ERROR) ? "GRESKA: Timeout (15s)" : "Spremno";
    webServer.broadcastStatus(motor.getState(), motor.getCurrentSpeed(),
                              inputs.isLimitOpen(), inputs.isLimitClose(),
                              settings.autoCloseEnabled, settings.autoCloseMinutes,
                              remainingSec, inputs.getToggleMode(), msg);
}

// Funkcija koja potpuno gasi ERROR, lampu i zvučni signal
void clearErrorState() {
    isPedestrianMode = false;
    motor.clearError();
    signals.stopSound();
    signals.playSound(SOUND_STOP);
    displayOLED.setMessage("Greska resetovana");
    notifyWeb();
}

void executeOpen() {
    if (motor.getState() == MOTOR_ERROR) {
        clearErrorState();
        return;
    }
    if (inputs.isLimitOpen()) {
        displayOLED.setMessage("Vec otvoreno!");
        signals.playSound(SOUND_ERROR);
        return;
    }
    isPedestrianMode = false;
    motor.open();
    gateIsFullyOpen = false;
    signals.playSound(SOUND_START);
    displayOLED.setMessage("Otvaranje...");
    notifyWeb();
}

void executeClose() {
    if (motor.getState() == MOTOR_ERROR) {
        clearErrorState();
        return;
    }
    if (inputs.isLimitClose()) {
        displayOLED.setMessage("Vec zatvoreno!");
        signals.playSound(SOUND_ERROR);
        return;
    }
    isPedestrianMode = false;
    motor.close();
    gateIsFullyOpen = false;
    signals.playSound(SOUND_START);
    displayOLED.setMessage("Zatvaranje...");
    notifyWeb();
}

void executeStop(bool isError = false) {
    isPedestrianMode = false;
    if (isError) {
        errorStartTime = millis();
        motor.stop(true);
        signals.playSound(SOUND_ERROR); // Pišti 15 sekundi, lampa gori
        displayOLED.setMessage("Greska Timeout!");
    } else {
        if (motor.getState() == MOTOR_ERROR) {
            clearErrorState();
            return;
        }
        motor.stop(false);
        signals.stopSound();
        signals.playSound(SOUND_STOP);
        displayOLED.setMessage("Zaustavljeno");
    }
    notifyWeb();
}

void executePedestrian() {
    if (motor.getState() == MOTOR_ERROR) {
        clearErrorState();
        return;
    }
    if (motor.getState() != MOTOR_STOP) {
        executeStop(false);
        return;
    }
    if (inputs.isLimitOpen()) {
        displayOLED.setMessage("Vec otvoreno!");
        return;
    }

    isPedestrianMode = true;
    pedestrianStartTime = millis();
    pedestrianStopTime = 0;

    motor.open();
    signals.playSound(SOUND_START);
    displayOLED.setMessage("Pesacki mod...");
    notifyWeb();
}

void handleWallButton() {
    if (motor.getState() == MOTOR_ERROR) {
        clearErrorState();
        return;
    }

    if (!inputs.getToggleMode()) {
        signals.playSound(SOUND_STOP);
        displayOLED.setMessage("Taster zakljucan!");
        notifyWeb();
        return;
    }

    isPedestrianMode = false;

    if (motor.getState() == MOTOR_OPEN || motor.getState() == MOTOR_CLOSE) {
        lastDirection = motor.getState();
        executeStop(false);
        return;
    }

    if (inputs.isLimitOpen()) {
        executeClose();
        lastDirection = MOTOR_CLOSE;
    } else if (inputs.isLimitClose()) {
        executeOpen();
        lastDirection = MOTOR_OPEN;
    } else {
        if (lastDirection == MOTOR_OPEN) {
            executeClose();
            lastDirection = MOTOR_CLOSE;
        } else {
            executeOpen();
            lastDirection = MOTOR_OPEN;
        }
    }
}

void setup() {
    motor.begin();
    signals.begin();

    Serial.begin(115200);
    Wire.begin(SDA_PIN, SCL_PIN);

    displayOLED.begin();
    displayOLED.showBootStep("1. WiFi spajanje...", "Molim sacekajte");

    #if USE_STATIC_IP
    WiFi.config(STATIC_IP, GATEWAY, SUBNET, DNS_SERVER);
    #endif

    WiFiManager wm;
    wm.setConfigPortalTimeout(120);
    wm.autoConnect("AutoGateAP");

    String ipStr = WiFi.localIP().toString();
    Serial.println("Povezan na WiFi! IP: " + ipStr);
    displayOLED.showBootStep("2. WiFi Povezan!", ("IP: " + ipStr).c_str());
    delay(2000);

    ConfigManager::load(settings);
    motor.setOpenProfile(settings.openProfile);
    motor.setCloseProfile(settings.closeProfile);

    inputs.begin();
    inputs.setToggleMode(settings.toggleMode);

    ArduinoOTA.setHostname("klizna-kapija");
    ArduinoOTA.onStart([]() { motor.stop(false); });
    ArduinoOTA.begin();

    webServer.begin(settings);
    webServer.setCommandCallback([](const String& cmd) {
        // Ako je u ERROR stanju, bilo koje dugme na Webu odmah resetuje grešku!
        if (motor.getState() == MOTOR_ERROR) {
            clearErrorState();
            return;
        }
        if (cmd == "open") executeOpen();
        else if (cmd == "close") executeClose();
        else if (cmd == "pedestrian") executePedestrian();
        else if (cmd == "stop") executeStop(false);
    });

    webServer.setSettingsSaveCallback([](const SystemSettings& newSettings) {
        settings = newSettings;
        ConfigManager::save(settings);
        motor.setOpenProfile(settings.openProfile);
        motor.setCloseProfile(settings.closeProfile);
        inputs.setToggleMode(settings.toggleMode);
        webServer.updateSettings(settings);
        notifyWeb();
    });

    displayOLED.setMessage("Spremno");
    displayOLED.update(MOTOR_STOP, 0, inputs.isLimitOpen(), inputs.isLimitClose(), settings.autoCloseEnabled, 0);
    notifyWeb();
}

void loop() {
    ArduinoOTA.handle();

    // 0. Automatski reset ERROR stanja nakon 15 sekundi
    if (motor.getState() == MOTOR_ERROR) {
        if (millis() - errorStartTime >= ERROR_DURATION_MS) {
            clearErrorState();
        }
    }

    // 1. Fotoćelija (aktivna samo ako je ENABLE_SAFETY_BEAM == true)
    if (inputs.isSafetyBeamBlocked()) {
        if (motor.getState() == MOTOR_CLOSE) {
            executeStop(false);
            signals.playSound(SOUND_LIMIT);
            displayOLED.setMessage("Prepreka! Revers");
            notifyWeb();

            delay(400);

            executeOpen();
            lastDirection = MOTOR_OPEN;
        }

        if (settings.autoCloseEnabled && gateIsFullyOpen) {
            gateFullyOpenedTime = millis();
        }
    }

    // 2. Tasteri i RF daljinski
    ButtonEvent evt = inputs.update();
    if (evt != EVENT_NONE) {
        // Ako je sistem u ERROR stanju, pritisak na BILO KOJU tipku odmah poništava grešku!
        if (motor.getState() == MOTOR_ERROR && 
            (evt == EVENT_WALL_BUTTON || evt == EVENT_RF_OPEN || evt == EVENT_RF_CLOSE || 
             evt == EVENT_RF_PEDESTRIAN || evt == EVENT_RF_STOP)) {
            clearErrorState();
        } else {
            switch (evt) {
                case EVENT_WALL_BUTTON:
                    handleWallButton();
                    break;

                case EVENT_RF_OPEN:
                    executeOpen();
                    lastDirection = MOTOR_OPEN;
                    break;

                case EVENT_RF_CLOSE:
                    executeClose();
                    lastDirection = MOTOR_CLOSE;
                    break;

                case EVENT_RF_STOP:
                    executeStop(false);
                    break;

                case EVENT_RF_PEDESTRIAN:
                    executePedestrian();
                    break;

                case EVENT_LIMIT_OPEN_HIT:
                    if (motor.getState() == MOTOR_OPEN) {
                        executeStop(false);
                        signals.playSound(SOUND_LIMIT);
                        gateIsFullyOpen = true;
                        gateFullyOpenedTime = millis();
                        lastDirection = MOTOR_OPEN;
                    }
                    break;

                case EVENT_LIMIT_CLOSE_HIT:
                    if (motor.getState() == MOTOR_CLOSE) {
                        executeStop(false);
                        signals.playSound(SOUND_LIMIT);
                        gateIsFullyOpen = false;
                        lastDirection = MOTOR_CLOSE;
                    }
                    break;

                default:
                    break;
            }
        }
        notifyWeb();
    }

    if (motor.getState() == MOTOR_OPEN && inputs.isLimitOpen()) executeStop(false);
    if (motor.getState() == MOTOR_CLOSE && inputs.isLimitClose()) executeStop(false);

    // 3. Logika pešačkog tajmera
    if (isPedestrianMode) {
        if (motor.getState() == MOTOR_OPEN && (millis() - pedestrianStartTime >= settings.pedestrianOpenMs)) {
            motor.stop(false);
            pedestrianStopTime = millis();
            displayOLED.setMessage("Pesacki ceka...");
            notifyWeb();
        } else if (motor.getState() == MOTOR_STOP && pedestrianStopTime > 0) {
            if (millis() - pedestrianStopTime >= settings.pedestrianWaitMs) {
                isPedestrianMode = false;
                executeClose();
                lastDirection = MOTOR_CLOSE;
            }
        }
    }

    // 4. Kretanje motora i rampe
    MotorState stBefore = motor.getState();
    int spdBefore = motor.getCurrentSpeed();
    motor.update();

    if (motor.isTimedOut() && motor.getState() != MOTOR_ERROR) {
        executeStop(true);
    }

    if (motor.getState() != stBefore || abs(motor.getCurrentSpeed() - spdBefore) >= 5) {
        notifyWeb();
    }

    // 5. Auto-close
    unsigned long remainingSec = 0;
    if (settings.autoCloseEnabled && gateIsFullyOpen && motor.getState() == MOTOR_STOP) {
        unsigned long elapsed = millis() - gateFullyOpenedTime;
        unsigned long totalMs = (unsigned long)settings.autoCloseMinutes * 60000;
        if (elapsed < totalMs) {
            remainingSec = (totalMs - elapsed) / 1000;
        } else {
            gateIsFullyOpen = false;
            executeClose();
        }
    }

    if (millis() - lastStatusBroadcast >= 1000) {
        lastStatusBroadcast = millis();
        notifyWeb();
    }

    // 6. Signali, displej i web server
    signals.update(motor.getState());
    displayOLED.update(motor.getState(), motor.getCurrentSpeed(),
                       inputs.isLimitOpen(), inputs.isLimitClose(),
                       settings.autoCloseEnabled, remainingSec);

    webServer.handleClient();
}
