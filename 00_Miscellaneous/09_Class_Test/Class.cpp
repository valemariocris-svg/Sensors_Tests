#include <Arduino.h>
#include "Class.h"

// Costruttore: inizializziamo la variabile privata con il valore passato
ClassExample::ClassExample(int ParInit) {
  _PrivVar = ParInit;
  PubVar = 0; // Inizializziamo anche la pubblica a un valore di default
}

// Implementazione della funzione pubblica
void ClassExample::PubFunzione() {
  Serial.println("Esecuzione di PubFunzione...");
  
  // Chiamiamo internamente la funzione privata
  _PrivFunzione();
  
  Serial.print("Valore aggiornato di PubVar: ");
  Serial.println(PubVar);
}

// Implementazione della funzione privata
void ClassExample::_PrivFunzione() {
  Serial.println("Esecuzione di _PrivFunzione (Nascosta)...");
  
  // Modifichiamo le variabili
  PubVar = _PrivVar * 2; 
}
