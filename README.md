# cat-bed-2
smart cat bed 2

# Inteligentný pelech pre mačky

Tento projekt implementuje inteligentný pelech pre mačky s nasledujúcimi funkciami:

* **Vyhrievané miesta:** 10 vyhrievaných miest pre mačky s detekciou obsadenosti pomocou senzorov.
* **Režim "nerušiť":** Možnosť deaktivovať vyhrievanie pre jednotlivé miesta pomocou tlačidiel.
* **Displej:** Zobrazenie informácií o obsadenosti, režime "nerušiť" a kŕmení na TFT displeji Waveshare ESP32-S3-Touch-LCD-7.
* **Webové rozhranie:** Zobrazenie informácií o pelechu a možnosť manuálneho nastavenia informácií o kŕmení.

## Hardvér

* ESP32-S3-Touch-LCD-7
* 10x kapacitný senzor TTP223
* 10x vyhrievací element (fólia alebo kábel)
* 10x tlačidlo
* Napájací adaptér 12V/5A
* Napäťový regulátor LM7805
* Prepojovacie káble

## Softvér

* Arduino IDE
* Knižnice pre ESP32-S3 a TFT displej
* Kód `pelech-cat-esp32s3.ino`
* Webové rozhranie `index.html`

## Zapojenie

Zapojte komponenty podľa schémy zapojenia.

## Spustenie

1. Nahrajte kód `pelech-cat-esp32s3.ino` na ESP32-S3-Touch-LCD-7.
2. Nahrajte webové rozhranie `index.html` do SPIFFS na ESP32-S3-Touch-LCD-7.
3. Pripojte ESP32-S3-Touch-LCD-7 k WiFi sieti.
4. Otvorte webové rozhranie v prehliadači.

## Použitie

* Na webovom rozhraní môžete sledovať obsadenosť miest, režim "nerušiť" a informácie o kŕmení.
* Môžete manuálne nastaviť informácie o kŕmení na webovom rozhraní.
* Tlačidlami na pelechu môžete aktivovať/deaktivovať režim "nerušiť" pre jednotlivé miesta.
