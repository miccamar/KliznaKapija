#ifndef GATE_WEB_SERVER_H
#define GATE_WEB_SERVER_H

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "Config.h"
#include "MotorController.h"
#include "ConfigManager.h"

typedef std::function<void(const String&)> CommandCallback;
typedef std::function<void(const SystemSettings&)> SettingsSaveCallback;

class GateWebServer {
public:
    GateWebServer();
    bool begin(const SystemSettings& initialSettings);
    void handleClient();

    void setCommandCallback(CommandCallback cb) { m_cmdCb = cb; }
    void setSettingsSaveCallback(SettingsSaveCallback cb) { m_saveCb = cb; }

    void broadcastStatus(MotorState state, int speed, bool limitOpen, bool limitClose, 
                         bool autoClose, int autoCloseMinutes, unsigned long remainingSec,
                         bool toggleMode, const char* message);

    void updateSettings(const SystemSettings& settings) { m_settings = settings; }

private:
    ESP8266WebServer m_server;
    WebSocketsServer m_webSocket;
    CommandCallback m_cmdCb;
    SettingsSaveCallback m_saveCb;
    SystemSettings m_settings;
    String m_lastStatusJson;

    String m_pendingCmd;
    bool m_hasPendingSave;

    void handleRoot();
    void handleCSS();
    void handleStatus();
    void handleControl();
    void handleGetSettings();
    void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
    void parseIncomingJson(const char* json, size_t len);
};

#endif
