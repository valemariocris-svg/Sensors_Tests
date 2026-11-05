#include "Class.h"


// Creiamo un'istanza della classe chiamata 'mioOggetto'
// Passiamo 500 come ParInit (che finirà in _PrivVar)
ClassExample mioOggetto(500);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("--- Test Classe ---");
  
  // Possiamo accedere alla variabile pubblica
  mioOggetto.PubVar = 10;
  
  // Chiamiamo la funzione pubblica
  mioOggetto.PubFunzione();
  
  // NOTA: mioOggetto._PrivVar o mioOggetto._PrivFunzione() 
  // causerebbero un errore di compilazione perché sono private.
}

void loop() {
  // Il loop rimane pulito
}
