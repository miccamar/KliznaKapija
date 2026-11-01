# 🏠 Pametna Klizna Kapija (ESP8266 NodeMCU + BTS7960)

Kompletan sistem za automatizaciju i kontrolu klizne kapije zasnovan na **ESP8266 (NodeMCU v2)**, drajveru motora **BTS7960 (43A H-Bridge)** i I2C ekspanderu **PCF8574**.

Rešava problem asimetrije brzina motora automobilskih brisača (različita brzina pri otvaranju i zatvaranju usled fabričkog pomaka četkica) putem **nezavisnih profila ubrzanja, kretanja i usporavanja (Soft-Start & Soft-Stop)**.

---

## ✨ Ključne Funkcije

* **Nezavisni profili za motor brisača (Soft-Start / Soft-Stop):**
  * Zasebno podešavanje maksimalne brzine (PWM), brzine pri kraju, vremena zaleta, trajanja pune brzine i timeout-a za otvaranje i zatvaranje.
* **Moderni Web Interfejs (WebSockets na portu 81):**
  * Instant dvosmerna komunikacija bez kašnjenja i opterećenja procesora.
  * Mobilni interfejs prilagođen telefonu — sve komande na jednom ekranu bez skrolovanja.
  * Tamna i svetla tema (prekidač ☀️/🌙).
* **Pešački mod (Pedestrian Mode):**
  * Delimično otvaranje kapije (~1m / npr. 4.5s), zadržavanje 15s i automatsko zatvaranje (dostupno preko Web-a i RF kanala C).
* **Zaključavanje zidnog tastera:**
  * Mogućnost softverskog isključivanja/zaključavanja fizičkog tastera na zidu radi bezbednosti (npr. noću ili tokom odsustva).
* **Koračni mod na jednom tasteru (Step-by-Step):**
  * Jedan taster radi ciklus: *Otvori ➔ Stop ➔ Zatvori ➔ Stop*.
* **Višestruka sigurnost:**
  * Podrška za fotoćeliju (NC kontakt) sa trenutnim zaustavljanjem, pauzom od 400ms za smirenje inercije i automatskim reversom u otvaranje.
  * Sigurnosni timeout za svaki smer.
  * Pametni alarm za grešku: pišti i svetli 15 sekundi uz automatski reset (ili trenutni reset pritiskom na bilo koje dugme).
* **Dual-Environment OTA & USB flešovanje:**
  * Odvojena okruženja `nodemcu_usb` i `nodemcu_ota` u `platformio.ini` za flešovanje kablom ili bežično kroz WiFi.
* **Trajna memorija (LittleFS / Flash):**
  * Sva podešavanja (vremena, brzine, auto-close) pamte se u `/config.json` i ne gube se pri nestanku struje.

---

## 🛠️ Hardverska Arhitektura

| Komponenta | Namena | Povezivanje na ESP8266 |
| :--- | :--- | :--- |
| **NodeMCU v2 (ESP-12E)** | Glavni mikrokontroler | Centralna jedinica |
| **BTS7960 (43A)** | H-most drajver motora | D3 (RPWM), D4 (LPWM), D5 (L_EN), D6 (R_EN) |
| **PCF8574 (I2C)** | 8-bitni ekspander ulaza | D1 (SCL / GPIO5), D2 (SDA / GPIO4) |
| **SSD1306 (128x64)** | OLED ekran statusa | D1 (SCL), D2 (SDA) |
| **LED Lampa** | Svetlosna signalizacija | D7 (GPIO13) |
| **Buzzer** | Zvučni signali | D8 (GPIO15) |

### 📌 Raspodela pinova na PCF8574 ekspanderu:
* **P0:** Fizički zidni taster (Koračni mod)
* **P1:** RF daljinski Kanal C (Pešački mod)
* **P2:** Limit prekidač - Otvoreno
* **P3:** Limit prekidač - Zatvoreno
* **P4:** RF daljinski Kanal A (Otvori)
* **P5:** RF daljinski Kanal B (Zatvori)
* **P6:** RF daljinski Kanal D (Stop)
* **P7:** Sigurnosna fotoćelija (NC kontakt)

---

## 🚀 Instalacija i Pokretanje (PlatformIO)

1. Klonirajte repozitorijum:
   ```bash
   git clone https://github.com/miccamar/KliznaKapija.git
   cd KliznaKapija
   ```
2. Otvorite projekat u **VS Code** (PlatformIO IDE).
3. Podesite statičku IP adresu (ako je koristite) u `src/Config.h`.
4. Izaberite željeno okruženje:
   * **`env:nodemcu_usb`** – za flešovanje preko USB kabla.
   * **`env:nodemcu_ota`** – za bežično flešovanje preko WiFi mreže.
5. Kliknite **Upload**.

---

## 📄 Licenca
Ovaj projekat je otvorenog koda (Open Source) pod **MIT licencom** — slobodan za korišćenje, modifikaciju i deljenje.
