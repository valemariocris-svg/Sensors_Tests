#include <Arduino.h>

/**
 * Esempio di formattazione stringhe con sprintf e snprintf
 * per ESP32-S3.
 */

void setup() {
  Serial.begin(115200);
  delay(1000);


/* ---------------------------------------------------------------------------------------------------
  // Variabili di test
  int      temperatura  = -5;
  uint     uptime       = 3600;
  uint8_t  indirizzoI2C = 0x6B;
  float    batteria     = 4.16845;
  String   stato        = "ATTIVO";
  stato += "  Aggiunto";

  char buffer[128];

  // Sintassi: sprintf(destinazione, "formato", variabili...);   per doble   usare sempre .1f  .2f  .3f
  
  snprintf(buffer, sizeof(buffer), "\n\nLOG: \nStato = %s\nTemp  =%4d C\nUptime=%u s \nI2C   =  0x%02X   %x\nBatteria: %.1f Volt", 
          stato.c_str(), 
          temperatura, 
          uptime, 
          indirizzoI2C,
          indirizzoI2C,
          batteria
          );
  Serial.println(buffer);

  // 3. Esempio con Float (precisione controllata)
  // %.2f significa "2 cifre dopo la virgola"
  // sprintf(buffer, "Batteria: %.2f Volt", batteria);
  // Serial.println(buffer);

  // 4. CONSIGLIO: snprintf (versione sicura)
  // snprintf impedisce di "uscire" dal buffer se la stringa è troppo lunga
  // snprintf(buffer, sizeof(buffer), "Valore HEX minuscolo: %x", indirizzoI2C);
  // Serial.println(buffer);

------------------------------------------------------------------------------------ */


/**
 * Esempio di conversione int8_t array in String
 * per Serial Numbers di sensori.
 */

  // Array di test (32 byte)
  int8_t rawSerial[32] = {0x00, 0x1A, 0xFF, 0x4B, 0x22}; // ... resto a zero

  // --- OPZIONE A: Conversione in HEX (Standard per ID Hardware) ---
  String hexSN = "";
  for (int i = 0; i < 32; i++) {
    char buf[3];
    // sprintf(buf, "%02X", (int8_t)rawSerial[i]);
    hexSN += buf;
  }
  
  Serial.print("Esadecimale: ");
  Serial.println(hexSN);


  // --- OPZIONE B: Conversione in Testo (Se contiene ASCII) ---
  // Esempio con testo leggibile
  int8_t asciiSerial[32] = "SENS-S3-99B2-X"; 
  
  // Metodo sicuro con snprintf per evitare overflow
  char txtBuffer[33]; 
  // snprintf(txtBuffer, sizeof(txtBuffer), "%s", (char*)asciiSerial);
  String textSN = String(txtBuffer);

  // Serial.print("Testuale: ");
  // Serial.println(textSN);
}

void loop() {
  // Niente nel loop
}
