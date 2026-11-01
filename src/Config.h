#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <ESP8266WiFi.h>

#define FW_VERSION "v5.1-modular"

// --- MREŽNA PODEŠAVANJA (STATIČKA IP) ---
#define USE_STATIC_IP true
const IPAddress STATIC_IP(192, 168, 1, 40);
const IPAddress GATEWAY(192, 168, 1, 1);
const IPAddress SUBNET(255, 255, 255, 0);
const IPAddress DNS_SERVER(192, 168, 1, 1);

// --- I2C PINOVI I ADRESE ---
#define SDA_PIN 4               // D2 (GPIO4)
#define SCL_PIN 5               // D1 (GPIO5)
#define PCF8574_ADDRESS 0x20    // I2C adresa expandera
#define OLED_ADDRESS    0x3C    // I2C adresa SSD1306 OLED-a
#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   64

// --- BTS7960 KONTROLER MOTORA ---
#define RPWM_PIN 2              // D3 (GPIO2) - Desni PWM (Zatvaranje)
#define LPWM_PIN 0              // D4 (GPIO0) - Levi PWM (Otvaranje)
#define R_EN_PIN 12             // D6 (GPIO12) - Enable Desni
#define L_EN_PIN 14             // D5 (GPIO14) - Enable Levi

// --- SIGNALIZACIJA ---
#define LED_PIN    13           // D7 (GPIO13)
#define BUZZER_PIN 15           // D8 (GPIO15)

// --- PCF8574 PINOVI (INPUTI P0-P7) ---
#define BUTTON_WALL      0      // P0 (1. pin) - Fizički taster na zidu
#define RF_PEDESTRIAN    1      // P1 (2. pin) - RF Kanal C (Pešački prolaz)
#define LIMIT_OPEN       2      // P2 (3. pin) - Limit prekidač otvoreno
#define LIMIT_CLOSE      3      // P3 (4. pin) - Limit prekidač zatvoreno
#define RF_OPEN          4      // P4 (5. pin) - RF Kanal A (Otvori)
#define RF_CLOSE         5      // P5 (6. pin) - RF Kanal B (Zatvori)
#define RF_STOP          6      // P6 (7. pin) - RF Kanal D (Stop)
#define SAFETY_BEAM      7      // P7 (8. pin) - Fotoćelija (NC kontakt)

// --- AKTIVACIJA DODATNIH HARDVERSKIH PINOVA ---
// Postavljeno na false za bezbedno testiranje dok fizički ne povežeš žice.
// Kada povežeš taster, RF kanal C ili fotoćeliju, samo promeni u true!
#define ENABLE_WALL_BUTTON   false   // P0 (1. pin) isključen
#define ENABLE_RF_PEDESTRIAN false   // P1 (2. pin) isključen
#define ENABLE_SAFETY_BEAM   false   // P7 (fotoćelija) isključena

// --- PROFILI KRETANJA (REŠENJE ZA MOTOR BRISAČA) ---
struct MotionProfile {
    int maxSpeed;               // Maksimalna PWM brzina (0-255)
    int minSpeed;               // Brzina pri usporavanju na kraju (0-255)
    unsigned long accelTime;    // Vreme ubrzavanja (ms)
    unsigned long runTime;      // Vreme pod punom brzinom pre usporavanja (ms)
    int speedStep;              // Korak promene PWM-a
    unsigned long stepDelay;    // Pauza između koraka ubrzanja (ms)
    unsigned long maxTimeout;   // Sigurnosni timeout (ms)
};

const MotionProfile DEFAULT_OPEN_PROFILE = {
    .maxSpeed    = 210,
    .minSpeed    = 80,
    .accelTime   = 4000,
    .runTime     = 11000,
    .speedStep   = 15,
    .stepDelay   = 80,
    .maxTimeout  = 28000
};

const MotionProfile DEFAULT_CLOSE_PROFILE = {
    .maxSpeed    = 175,
    .minSpeed    = 70,
    .accelTime   = 3000,
    .runTime     = 8500,
    .speedStep   = 20,
    .stepDelay   = 70,
    .maxTimeout  = 22000
};

#endif
