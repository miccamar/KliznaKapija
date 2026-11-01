#include "GateDisplay.h"

GateDisplay::GateDisplay() 
    : m_oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1), m_lastUpdate(0) {
    strncpy(m_message, "Spremno", sizeof(m_message));
}

bool GateDisplay::begin() {
    if (!m_oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
        return false;
    }
    showBootStep("KLIZNA KAPIJA", "Pokretanje...");
    return true;
}

void GateDisplay::showBootStep(const char* line1, const char* line2) {
    m_oled.clearDisplay();
    m_oled.setTextColor(SSD1306_WHITE);
    m_oled.setTextSize(1);
    m_oled.setCursor(0, 12);
    m_oled.println(line1);
    if (line2 && line2[0] != '\0') {
        m_oled.setCursor(0, 32);
        m_oled.println(line2);
    }
    m_oled.display();
}

void GateDisplay::setMessage(const char* msg) {
    strncpy(m_message, msg, sizeof(m_message) - 1);
    m_message[sizeof(m_message) - 1] = '\0';
}

uint8_t GateDisplay::getWifiQuality() {
    int rssi = WiFi.RSSI();
    if (rssi <= -100) return 0;
    if (rssi >= -50) return 100;
    return uint8_t(2 * (rssi + 100));
}

void GateDisplay::update(MotorState state, int speed, bool limitOpen, bool limitClose, bool autoClose, unsigned long remainingAutoCloseSec) {
    if (m_lastUpdate != 0 && (millis() - m_lastUpdate < 350)) return;
    m_lastUpdate = millis();

    m_oled.clearDisplay();

    m_oled.setTextSize(1);
    m_oled.setCursor(0, 0);
    m_oled.print(WiFi.localIP().toString());
    
    m_oled.setCursor(95, 0);
    m_oled.print(getWifiQuality());
    m_oled.print("%");

    m_oled.drawLine(0, 10, 127, 10, SSD1306_WHITE);

    m_oled.setCursor(0, 14);
    m_oled.print("Motor: ");
    switch (state) {
        case MOTOR_OPEN:  m_oled.print("OTVARANJE"); break;
        case MOTOR_CLOSE: m_oled.print("ZATVARANJE"); break;
        case MOTOR_ERROR: m_oled.print("GRESKA!"); break;
        default:          m_oled.print("STOP"); break;
    }

    m_oled.setCursor(0, 26);
    m_oled.print("Brzina: ");
    m_oled.print(speed);
    m_oled.print(" PWM");

    m_oled.setCursor(0, 38);
    m_oled.print("Lim: O:[");
    m_oled.print(limitOpen ? "X" : " ");
    m_oled.print("] Z:[");
    m_oled.print(limitClose ? "X" : " ");
    m_oled.print("]");

    m_oled.setCursor(0, 50);
    if (autoClose && remainingAutoCloseSec > 0) {
        m_oled.print("AutoCls: ");
        m_oled.print(remainingAutoCloseSec);
        m_oled.print("s");
    } else {
        m_oled.print(m_message);
    }

    m_oled.display();
}
