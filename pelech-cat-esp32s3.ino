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
bool heatingEnabled[10] = {false}; 

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
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0, 4);
  tft.setTextColor(TFT_WHITE);
  tft.println("Inteligentny pelech");

  // Zobrazenie obsadenosti
  for (int i = 0; i < 10; i++) {
    tft.setCursor(0, 60 + i * 30, 2); 
    tft.print("Miesto " + String(i + 1) + ": ");
    if (bedOccupied[i]) {
      tft.setTextColor(TFT_GREEN);
      tft.println("Obsadené");
    } else {
      tft.setTextColor(TFT_RED);
      tft.println("Volné");
    }
  }

  // Zobrazenie režimu "nerušiť"
  for (int i = 0; i < 10; i++) {
    tft.setCursor(250, 60 + i * 30, 2); 
    if (doNotDisturb[i]) {
      tft.setTextColor(TFT_YELLOW);
      tft.println("NR"); // Zobrazenie "NR" pre režim "nerušiť"
    } else {
      tft.setTextColor(TFT_WHITE);
      tft.println("  "); // Zobrazenie dvoch medzier pre zarovnanie
    }
  }

  // Zobrazenie informácií o kŕmení
  tft.setCursor(0, 400, 2); 
  tft.setTextColor(TFT_WHITE);
  tft.println("Dalsie krmenie:");
  tft.setCursor(0, 430, 2);
  if (manualFeedSet) {
    tft.println(manualFeedInfo);
  } else {
    tft.println(feedTimes[currentFeeding] + " " + String(feedGrams[currentFeeding]) + "g");
  }
}

void setup() {
  // Inicializácia sériovej komunikácie
  Serial.begin(115200);

  // Inicializácia TFT displeja
  tft.init();
  tft.setRotation(3); 
  tft.fillScreen(TFT_BLACK);

  // Pripojenie k WiFi sieti
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Pripájanie k WiFi..");
  }
  Serial.println(WiFi.localIP());

  // Inicializácia SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("Chyba pri inicializácii SPIFFS");
    return;
  }

  // Nastavenie pinov pre senzory, vyhrievacie elementy a tlačidlá
  for (int i = 0; i < 10; i++) {
    pinMode(sensorPins[i], INPUT);
    pinMode(heaterPins[i], OUTPUT);
    pinMode(buttonPins[i], INPUT_PULLUP); 
  }

  // Definovanie endpointu pre hlavnú stránku
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });

  // Endpoint pre získanie stavu obsadenosti
  server.on("/occupancy", HTTP_GET, [](AsyncWebServerRequest *request){
    String occupancy = "";
    for (int i = 0; i < 10; i++) {
      occupancy += bedOccupied[i] ? "1" : "0";
    }
    request->send(200, "text/plain", occupancy);
  });

  // Endpoint pre získanie stavu režimu "nerušiť"
  server.on("/dnd", HTTP_GET, [](AsyncWebServerRequest *request){
    String dnd = "";
    for (int i = 0; i < 10; i++) {
      dnd += doNotDisturb[i] ? "1" : "0";
    }
    request->send(200, "text/plain", dnd);
  });

  // Endpoint pre získanie informácií o kŕmení
  server.on("/feeding", HTTP_GET, [](AsyncWebServerRequest *request){
    String feedingInfo = manualFeedSet ? manualFeedInfo : (currentFeedTime + " " + String(currentFeedGrams) + "g");
    request->send(200, "text/plain", feedingInfo);
  });

  // Endpoint pre manuálne nastavenie kŕmenia
  server.on("/setManualFeed", HTTP_POST, [](AsyncWebServerRequest *request){
    if (request->hasParam("feedInfo", true)) {
      manualFeedInfo = request->getParam("feedInfo", true)->value();
      manualFeedSet = true;
      request->send(200, "text/plain", "Manualne krmenie nastavene");
    } else {
      request->send(400, "text/plain", "Chýba parameter 'feedInfo'");
    }
  });

  // Endpointy pre ovládanie
  server.on("/occupancy/:index", HTTP_GET, [](AsyncWebServerRequest *request){
    int index = request->pathArg("index").toInt();
    if (index >= 0 && index < 10) {
      bedOccupied[index] = !bedOccupied[index];
      request->send(200, "text/plain", "Obsadenosť prepnutie");
    } else {
      request->send(400, "text/plain", "Neplatný index");
    }
  });

  server.on("/heating/:index", HTTP_GET, [](AsyncWebServerRequest *request){
    int index = request->pathArg("index").toInt();
    if (index >= 0 && index < 10) {
      heatingEnabled[index] = !heatingEnabled[index]; // Prepnutie stavu vyhrievania
      digitalWrite(heaterPins[index], heatingEnabled[index]); // Zapnutie/vypnutie vyhrievania
      request->send(200, "text/plain", "Vyhrievanie prepnutie");
    } else {
      request->send(400, "text/plain", "Neplatný index");
    }
  });

  server.on("/heating/:index", HTTP_GET, [](AsyncWebServerRequest *request){
    int index = request->pathArg("index").toInt();
    if (index >= 0 && index < 10) {
      heatingEnabled[index] = !heatingEnabled[index]; // Prepnutie stavu vyhrievania
      digitalWrite(heaterPins[index], heatingEnabled[index]); // Zapnutie/vypnutie vyhrievania
