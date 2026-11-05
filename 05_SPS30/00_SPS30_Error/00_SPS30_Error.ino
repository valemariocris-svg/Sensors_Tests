
#include <Arduino.h>
bool Init_SPS30() {
    delay(3000); // <<<<<<< ==============================
    // return true;
}
/*
Quando il compilatore incontra una funzione che promette di restituire un valore ma non lo fa, 
il processore (specialmente l'ESP32 che è molto sensibile) va in crash o entra in un loop
infinito perché, uscendo dalla funzione, cerca di recuperare un valore dal registro
che non è stato popolato, "sporcando" lo stack di memoria o il registro del Program Counter (PC).
Ecco perché il comportamento cambia in base alla durata del delay: 
cambia semplicemente il modo in cui la memoria viene gestita in quel microsecondo, 
ma l'errore di fondo rimane.

Vai su File > Impostazioni (o Preferences).
Cerca Avvisi del compilatore (Compiler Warnings).
Impostalo su "Tutti" (All).
In questo modo, quando scrivi una funzione bool o int senza il return, 
nella console nera in basso vedrai apparire un avviso giallo: 
warning: no return statement in function returning non-void [-Wreturn-type]
*/



/* ----------------------------------------------------------------------------------- */
void setup() {
    Serial.begin(115200);
    delay(100);
    Serial.println("SETUP\n");
    delay(1000);

    Init_SPS30();   // <<<<<<< ==============================
    Serial.println("END SETUP");
}
/* ----------------------------------------------------------------------------------- */
void loop() { 
    delay(2000);
    Serial.println("LOOP HERE");
}
/* ----------------------------------------------------------------------------------- */



/*
#include <Wire.h>
// #include <SensirionI2cSps30.h>      // SPS30

// // Factor to convert sec in microseconds
// #define uS_TO_S_FACTOR 1000000ULL  
// #define TIME_TO_SLEEP_S 60          // seconds

// #define LedPIN             13
// #define TestPIN            4
// #define Switch1_PIN        5
// #define Switch2_PIN        6

// #define I2C_SCL            7
// #define I2C_SDA            8
// #define SPS30_I2C_ADDRESS  0x69

// // Variabili globali
// uint16_t  ReadCount      = 0;
// uint8_t   Wire_Begin     = 0;
// uint8_t   Sps30_Err_Code = 0;
// uint8_t   Clean_Err_Code = 0;
// boolean   Res_Printed    = false;
// uint32_t  LedPause       = 500;

// uint16_t errorRes;
// int8_t   serialNumber[32];
// String   SerialNum_String = "";

// SensirionI2cSps30 sps30;

*/