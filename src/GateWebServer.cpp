#include "GateWebServer.h"
#include "WebPage.h"

GateWebServer::GateWebServer() 
    : m_server(80), m_webSocket(81), m_hasPendingSave(false) {
    // Bez alokacije Stringova u globalnom konstruktoru pre setup()!
}

bool GateWebServer::begin(const SystemSettings& initialSettings) {
    m_settings = initialSettings;
    m_lastStatusJson = "{\"motorStatus\":\"STOP\",\"speed\":0,\"limitOpen\":false,\"limitClose\":false}";

    // HTTP rute
    m_server.on("/", HTTP_GET, [this]() { handleRoot(); });
    m_server.on("/style.css", HTTP_GET, [this]() { handleCSS(); });
    m_server.on("/favicon.ico", HTTP_GET, [this]() { m_server.send(204); });
    m_server.on("/api/status", HTTP_GET, [this]() { handleStatus(); });
    m_server.on("/api/control", HTTP_POST, [this]() { handleControl(); });
    m_server.on("/api/settings", HTTP_GET, [this]() { handleGetSettings(); });

    m_server.begin();

    // WebSockets start na portu 81
    m_webSocket.begin();
    m_webSocket.onEvent([this](uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
        onWebSocketEvent(num, type, payload, length);
    });

    return true;
}

void GateWebServer::handleClient() {
    m_server.handleClient();
    m_webSocket.loop();

    if (m_pendingCmd.length() > 0) {
        String cmd = m_pendingCmd;
        m_pendingCmd = "";
        if (m_cmdCb) m_cmdCb(cmd);
    }

    if (m_hasPendingSave) {
        m_hasPendingSave = false;
        if (m_saveCb) m_saveCb(m_settings);
    }
}

void GateWebServer::handleRoot() {
    // Šalje stranicu direktno iz PROGMEM Flash-a (0 bajtova RAM-a, radi čak i bez LittleFS uploada!)
    m_server.send_P(200, "text/html", INDEX_HTML);
}

void GateWebServer::handleCSS() {
    m_server.send(200, "text/css", "");
}

void GateWebServer::handleStatus() {
    m_server.send(200, "application/json", m_lastStatusJson);
}

void GateWebServer::handleControl() {
    if (!m_server.hasArg("plain")) {
        m_server.send(400, "application/json", "{\"success\":false}");
        return;
    }
    String body = m_server.arg("plain");
    parseIncomingJson(body.c_str(), body.length());
    m_server.send(200, "application/json", "{\"success\":true}");
}

void GateWebServer::handleGetSettings() {
    DynamicJsonDocument doc(768);
    doc["autoCloseEnabled"]   = m_settings.autoCloseEnabled;
    doc["autoCloseMinutes"]   = m_settings.autoCloseMinutes;
    doc["toggleMode"]         = m_settings.toggleMode;
    doc["pedestrianOpenTime"] = (float)m_settings.pedestrianOpenMs / 1000.0;
    doc["pedestrianWaitTime"] = (float)m_settings.pedestrianWaitMs / 1000.0;

    JsonObject o = doc.createNestedObject("open");
    o["maxSpeed"]   = m_settings.openProfile.maxSpeed;
    o["minSpeed"]   = m_settings.openProfile.minSpeed;
    o["accelTime"]  = (float)m_settings.openProfile.accelTime / 1000.0;
    o["runTime"]    = (float)m_settings.openProfile.runTime / 1000.0;
    o["maxTimeout"] = m_settings.openProfile.maxTimeout / 1000;

    JsonObject c = doc.createNestedObject("close");
    c["maxSpeed"]   = m_settings.closeProfile.maxSpeed;
    c["minSpeed"]   = m_settings.closeProfile.minSpeed;
    c["accelTime"]  = (float)m_settings.closeProfile.accelTime / 1000.0;
    c["runTime"]    = (float)m_settings.closeProfile.runTime / 1000.0;
    c["maxTimeout"] = m_settings.closeProfile.maxTimeout / 1000;

    String res;
    serializeJson(doc, res);
    m_server.send(200, "application/json", res);
}

void GateWebServer::onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    if (type == WStype_CONNECTED) {
        if (m_lastStatusJson.length() > 0) {
            m_webSocket.sendTXT(num, m_lastStatusJson);
        }
    } else if (type == WStype_TEXT) {
        parseIncomingJson((const char*)payload, length);
    }
}

void GateWebServer::parseIncomingJson(const char* json, size_t len) {
    DynamicJsonDocument doc(768);
    if (deserializeJson(doc, json, len)) return;

    if (doc.containsKey("command")) {
        m_pendingCmd = doc["command"].as<String>();
    } else if (doc.containsKey("saveSettings")) {
        m_settings.autoCloseEnabled = doc["autoCloseEnabled"].as<bool>();
        m_settings.autoCloseMinutes = doc["autoCloseMinutes"].as<int>();
        m_settings.toggleMode       = doc["toggleMode"].as<bool>();
        if (doc.containsKey("pedestrianOpenTime")) {
            m_settings.pedestrianOpenMs = (unsigned long)(doc["pedestrianOpenTime"].as<float>() * 1000);
        }
        if (doc.containsKey("pedestrianWaitTime")) {
            m_settings.pedestrianWaitMs = (unsigned long)(doc["pedestrianWaitTime"].as<float>() * 1000);
        }

        JsonObject o = doc["open"];
        if (!o.isNull()) {
            m_settings.openProfile.maxSpeed   = o["maxSpeed"].as<int>();
            m_settings.openProfile.minSpeed   = o["minSpeed"].as<int>();
            m_settings.openProfile.accelTime  = (unsigned long)(o["accelTime"].as<float>() * 1000);
            m_settings.openProfile.runTime    = (unsigned long)(o["runTime"].as<float>() * 1000);
            m_settings.openProfile.maxTimeout = (unsigned long)(o["maxTimeout"].as<float>() * 1000);
        }

        JsonObject c = doc["close"];
        if (!c.isNull()) {
            m_settings.closeProfile.maxSpeed   = c["maxSpeed"].as<int>();
            m_settings.closeProfile.minSpeed   = c["minSpeed"].as<int>();
            m_settings.closeProfile.accelTime  = (unsigned long)(c["accelTime"].as<float>() * 1000);
            m_settings.closeProfile.runTime    = (unsigned long)(c["runTime"].as<float>() * 1000);
            m_settings.closeProfile.maxTimeout = (unsigned long)(c["maxTimeout"].as<float>() * 1000);
        }

        m_hasPendingSave = true;
    }
}

void GateWebServer::broadcastStatus(MotorState state, int speed, bool limitOpen, bool limitClose, 
                                    bool autoClose, int autoCloseMinutes, unsigned long remainingSec,
                                    bool toggleMode, const char* message) {
    DynamicJsonDocument doc(384);
    doc["version"] = FW_VERSION;
    switch (state) {
        case MOTOR_OPEN:  doc["motorStatus"] = "OTVARAM"; break;
        case MOTOR_CLOSE: doc["motorStatus"] = "ZATVARAM"; break;
        case MOTOR_ERROR: doc["motorStatus"] = "ERROR"; break;
        default:          doc["motorStatus"] = "STOP"; break;
    }

    doc["speed"] = speed;
    doc["limitOpen"] = limitOpen;
    doc["limitClose"] = limitClose;
    if (autoClose) {
        if (remainingSec > 0) {
            doc["autoClose"] = String(remainingSec) + "s";
        } else {
            doc["autoClose"] = String(autoCloseMinutes) + "m";
        }
    } else {
        doc["autoClose"] = "OFF";
    }
    doc["autoCloseValue"] = autoClose ? String(autoCloseMinutes) : "off";
    doc["toggleMode"] = toggleMode;
    doc["lastMessage"] = message;

    m_lastStatusJson = "";
    serializeJson(doc, m_lastStatusJson);
    m_webSocket.broadcastTXT(m_lastStatusJson);
}
