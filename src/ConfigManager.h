#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "Config.h"

struct SystemSettings {
    bool autoCloseEnabled;
    int autoCloseMinutes;
    bool toggleMode;
    unsigned long pedestrianOpenMs;
    unsigned long pedestrianWaitMs;
    MotionProfile openProfile;
    MotionProfile closeProfile;
};

class ConfigManager {
public:
    static void setDefaults(SystemSettings& settings) {
        settings.autoCloseEnabled = true;
        settings.autoCloseMinutes = 2;
        settings.toggleMode = true;
        settings.pedestrianOpenMs = 4500;
        settings.pedestrianWaitMs = 15000;
        settings.openProfile = DEFAULT_OPEN_PROFILE;
        settings.closeProfile = DEFAULT_CLOSE_PROFILE;
    }

    static bool load(SystemSettings& settings) {
        setDefaults(settings);

        if (!LittleFS.begin()) {
            Serial.println("LittleFS nije montiran, koristim podrazumevana podesavanja.");
            return false;
        }

        if (!LittleFS.exists("/config.json")) {
            return true;
        }

        File file = LittleFS.open("/config.json", "r");
        if (!file) return false;

        DynamicJsonDocument doc(768);
        DeserializationError error = deserializeJson(doc, file);
        file.close();

        if (error) return false;

        settings.autoCloseEnabled = doc["autoCloseEnabled"] | true;
        settings.autoCloseMinutes = doc["autoCloseMinutes"] | 2;
        settings.toggleMode       = doc["toggleMode"] | true;
        settings.pedestrianOpenMs = doc["pedestrianOpenMs"] | 4500;
        settings.pedestrianWaitMs = doc["pedestrianWaitMs"] | 15000;

        JsonObject o = doc["open"];
        if (!o.isNull()) {
            settings.openProfile.maxSpeed    = o["maxSpeed"] | DEFAULT_OPEN_PROFILE.maxSpeed;
            settings.openProfile.minSpeed    = o["minSpeed"] | DEFAULT_OPEN_PROFILE.minSpeed;
            settings.openProfile.accelTime   = o["accelTime"] | DEFAULT_OPEN_PROFILE.accelTime;
            settings.openProfile.runTime     = o["runTime"] | DEFAULT_OPEN_PROFILE.runTime;
            settings.openProfile.speedStep   = o["speedStep"] | DEFAULT_OPEN_PROFILE.speedStep;
            settings.openProfile.stepDelay   = o["stepDelay"] | DEFAULT_OPEN_PROFILE.stepDelay;
            settings.openProfile.maxTimeout  = o["maxTimeout"] | DEFAULT_OPEN_PROFILE.maxTimeout;
        }

        JsonObject c = doc["close"];
        if (!c.isNull()) {
            settings.closeProfile.maxSpeed   = c["maxSpeed"] | DEFAULT_CLOSE_PROFILE.maxSpeed;
            settings.closeProfile.minSpeed   = c["minSpeed"] | DEFAULT_CLOSE_PROFILE.minSpeed;
            settings.closeProfile.accelTime  = c["accelTime"] | DEFAULT_CLOSE_PROFILE.accelTime;
            settings.closeProfile.runTime    = c["runTime"] | DEFAULT_CLOSE_PROFILE.runTime;
            settings.closeProfile.speedStep  = c["speedStep"] | DEFAULT_CLOSE_PROFILE.speedStep;
            settings.closeProfile.stepDelay  = c["stepDelay"] | DEFAULT_CLOSE_PROFILE.stepDelay;
            settings.closeProfile.maxTimeout = c["maxTimeout"] | DEFAULT_CLOSE_PROFILE.maxTimeout;
        }

        return true;
    }

    static bool save(const SystemSettings& settings) {
        if (!LittleFS.begin()) {
            LittleFS.format();
            if (!LittleFS.begin()) return false;
        }

        DynamicJsonDocument doc(768);
        doc["autoCloseEnabled"] = settings.autoCloseEnabled;
        doc["autoCloseMinutes"] = settings.autoCloseMinutes;
        doc["toggleMode"]       = settings.toggleMode;
        doc["pedestrianOpenMs"] = settings.pedestrianOpenMs;
        doc["pedestrianWaitMs"] = settings.pedestrianWaitMs;

        JsonObject o = doc.createNestedObject("open");
        o["maxSpeed"]   = settings.openProfile.maxSpeed;
        o["minSpeed"]   = settings.openProfile.minSpeed;
        o["accelTime"]  = settings.openProfile.accelTime;
        o["runTime"]    = settings.openProfile.runTime;
        o["speedStep"]  = settings.openProfile.speedStep;
        o["stepDelay"]  = settings.openProfile.stepDelay;
        o["maxTimeout"] = settings.openProfile.maxTimeout;

        JsonObject c = doc.createNestedObject("close");
        c["maxSpeed"]   = settings.closeProfile.maxSpeed;
        c["minSpeed"]   = settings.closeProfile.minSpeed;
        c["accelTime"]  = settings.closeProfile.accelTime;
        c["runTime"]    = settings.closeProfile.runTime;
        c["speedStep"]  = settings.closeProfile.speedStep;
        c["stepDelay"]  = settings.closeProfile.stepDelay;
        c["maxTimeout"] = settings.closeProfile.maxTimeout;

        File file = LittleFS.open("/config.json", "w");
        if (!file) return false;

        serializeJson(doc, file);
        file.close();
        return true;
    }
};

#endif
