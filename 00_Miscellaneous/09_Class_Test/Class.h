#ifndef CLASS_EXAMPLE_H
#define CLASS_EXAMPLE_H

#include <Arduino.h>

class ClassExample {
  public:
    // Costruttore: serve per inizializzare il parametro ParInit
    ClassExample(int ParInit);

    // Funzione pubblica accessibile dal file .ino
    void PubFunzione();

    // Variabile pubblica
    int PubVar;

  private:
    // Funzione privata: può essere chiamata solo dall'interno della classe
    void _PrivFunzione();

    // Variabile privata (convenzione con underscore)
    int _PrivVar;
};
#endif