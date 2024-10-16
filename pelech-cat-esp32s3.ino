#include <WiFi.h>
#include "SPIFFS.h"
#include <ESPAsyncWebServer.h>
#include <TFT_eSPI.h> 

// Nahraď s tvojim SSID a heslom WiFi siete
const char* ssid = "NAZOV_WIFI_SIETE";
const char* password = "HESLO_WIFI_SIETE";

// Vytvorenie objektu pre TFT displej
TFT_eSPI tft = TFT_eSPI();

// Vytvorenie webového servera
AsyncWebServer server(80);

// Premenné pre uloženie dát 
bool bedOccupied[10] = {false};
bool doNotDisturb[10] = {false};

// Definovanie pinov pre senzory, vyhrievacie elementy a tlačidlá
const int sensorPins[10] = {2, 4, 6, 8, 10, 12, 14, 16, 18, 20};
const int heaterPins[10] = {3, 5, 7, 9, 11, 13, 15, 17, 19, 21};
const int buttonPins[10] = {22, 24, 26, 28, 30, 32, 34, 36, 38, 39};

// Počet kŕmení za deň
const int NUM_FEEDINGS = 5;

// Pole pre časy kŕmenia
String feedTimes[NUM_FEEDINGS] = {"7:00", "12:00", "17:00", "20:00", "22:00"};

// Pole pre množstvá krmiva (v gramoch)
int feedGrams[NUM_FEEDINGS] = {40, 20, 30, 10, 40};

// Index aktuálneho kŕmenia
int currentFeeding = 0;

// Premenné pre manuálne nastavenie kŕmenia
String manualFeedInfo = ""; 
bool manualFeedSet = false;

// Funkcia pre zobrazenie informácií na displeji
void displayInfo() {
  // ... (kód pre zobrazenie informácií na displeji) ...
}

void setup() {
  // ... (kód pre inicializáciu) ...

  // Definovanie endpointu pre hlavnú stránku
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });

  // ... (ostatné endpointy) ...

  // Spustenie webového servera
  server.begin();
}

void loop() {
  // ... (kód pre čítanie senzorov, ovládanie vyhrievania a režimu "nerušiť") ...

  // Zobrazenie informácií na displeji
  displayInfo();

  // Zvýšenie indexu aktuálneho kŕmenia
  currentFeeding = (currentFeeding + 1) % NUM_FEEDINGS;

  // Odoslanie dát na ESP32 cez sériovú komunikáciu
  String data = "o"; // Začiatok reťazca s označením "o" pre obsadenosť
  for (int i = 0; i < 10; i++) {
    data += bedOccupied[i] ? "1" : "0"; 
  }
  data += ";d"; // Označenie "d" pre "do not disturb"
  for (int i = 0; i < 10; i++) {
    data += doNotDisturb[i] ? "1" : "0";
  }
  data += ";f"; // Označenie "f" pre feeding (kŕmenie)
  if (manualFeedSet) {
    data += manualFeedInfo;
  } else {
    data += feedTimes[currentFeeding] + ";" + String(feedGrams[currentFeeding]);
  }
  Serial.println(data); // Odoslanie dát cez sériový port

  // Čakanie 1 sekundu
  delay(1000);
}
