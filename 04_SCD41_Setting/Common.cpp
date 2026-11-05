#include "Common.h"

SensirionI2cSht4x sht45;
Adafruit_BMP3XX   bmp390;
SensirionI2cScd4x scd41;

// DEFINIZIONE REALE delle variabili globali (con i valori iniziali)
bool PRINT_RESULT        = true;
bool READ_MEAS_ENABLE    = true;
bool SLEEP_ENABLED       = false;
TestState test_state     = STATE_START_MEASURE;

float buffer_co2[TOT_SAMPLES]; // Il vettore che conterrà le tue 50 letture
uint32_t serialNumber;
uint16_t error;

/* -------------------------------------------------------------------------------------------------------------------- */
uint16_t SHT45_begin() {
    Wire.beginTransmission(SHT45_I2C_ADDRESS);

    error = Wire.endTransmission();
    if(error) { 
        PrintUint64(0, indexSHT45, error, "begin");
        return error;   
    }
    else {
        error =   sht45.serialNumber(serialNumber);
      if(error) {
          PrintUint64(0, indexSHT45, error, "begin ReadSN");
          return error;
      }
      else      PrintUint64((uint64_t) serialNumber, indexSHT45, 0, "");
    }
    return 0;
 }

 /* -------------------------------------------------------------------------------------------------------------------- */
uint16_t BMP390_begin() {
    if (!bmp390.begin_I2C() ) {
       Serial.println("\nErrore begin  BMP390!");
       return 1;
    }
    else {
        // bmp390.setTemperatureOversampling(BMP3X_OVERSAMPLING_8X);
        // bmp390.setPressureOversampling(BMP3X_OVERSAMPLING_16X);
        // bmp390.setIIRFilterCoeff(BMP3X_IIR_FILTER_COEFF_3);
        // bmp390.setOutputDataRate(BMP3X_ODR_50_HZ);
        Serial.printf("\nBMP390 inizializzato.");
    }
}

/* -------------------------------------------------------------------------------------------------------------------- */
uint16_t SCD41_begin() {
    scd41.begin(Wire, SCD41_I2C_ADDRESS);
    
    // Assicuriamoci che parta in idle fermando eventuali misure precedenti
    error = scd41.stopPeriodicMeasurement();
    delay(100);

    uint64_t serialNumber_64;
    uint8_t result = 0;
    if (error) {
        for(int i=0; i<3; i++) {
            scd41.wakeUp();
            delay(100);
            error = scd41.stopPeriodicMeasurement();
            if (error) {
                result++;
            }
            else {
                result = 0;
                break;
            }
        }
        if(result > 0) {
            Serial.printf("\n\nThree errors on trying stopPeriodicMeasurement\nSTOP!!!");
            while(1)  delay(50);
        }
    }
    error = scd41.getSerialNumber(serialNumber_64);
    if (error) {
        PrintUint64(0, indexSCD41, error, "error on get S/N\nSTOP!!!"); 
        return 1;
    }
    PrintUint64(serialNumber, indexSCD41, 0, "");

    float currentOffset = 0.0;
    if (scd41.getTemperatureOffset(currentOffset) == 0) {
        Serial.print("Offset di temperatura attuale (Toffset_previous): ");
        Serial.print(currentOffset);
        Serial.println(" °C");
    }

    Serial.printf("\nInizio ciclo di calibrazione Single-Shot ..");
    Serial.printf("\n--------------------------------------------------------");
    return 0;
}


/* -------------------------------------------------------------------------------------------------------------------- */
#define ESP32 1
// uint8_t  ESP32 = 1;  // 1=Waveshare,  2=compatibile  3=ESP32-Nano
/* ------------------------------------LOW-------------------- */
void set_RGB_colors(uint8_t valRed, uint8_t valGreen, uint8_t valBlue) {
  switch (ESP32) {
    case  1:
      rgbLedWrite(WS_RGB, valRed, valGreen, valBlue);
      break;
    case  2:
      rgbLedWrite(WS_RGB, valGreen, valRed, valBlue);
      break;
    case  3:
      digitalWrite(14, valRed);
      digitalWrite(14, valGreen);
      digitalWrite(14, valBlue);
      break;
    default:
      break;
  }
}

/* -------------------------------------------------------------------------------------------------------------------- */
void save_samples_InEeprom() {
    EEPROM.begin(EEPROM_SIZE);

    // Salva l'intero array buffer_co2 a partire dall'indirizzo 0 della EEPROM
    EEPROM.put(0, buffer_co2);

    // Su ESP32 il commit è obbligatorio per scrivere realmente i dati in Flash
    if (EEPROM.commit()) {
        Serial.println("\n[EEPROM] salvate le misure nella memoria persistente!");
    } else {
        Serial.println("\n[EEPROM] ERRORE: Scrittura EEPROM fallita!");
    }
    
    EEPROM.end();
}

/* -------------------------------------------------------------------------------------------------------------------- */
void leggiEStampaTestFiltro() {
    
    EEPROM.begin(EEPROM_SIZE);
    // Preleva l'intero array dalla EEPROM dall'indirizzo 0
    EEPROM.get(0, buffer_co2);
    EEPROM.end();

    // --- A. Calcolo Min, Max, Media ---
    uint32_t somma = 0;
    uint16_t min_co2 = buffer_co2[0];
    uint16_t max_co2 = buffer_co2[0];

    for (int i = 0; i < TOT_SAMPLES; i++) {
        somma += buffer_co2[i];
        if (buffer_co2[i] < min_co2) min_co2 = buffer_co2[i];
        if (buffer_co2[i] > max_co2) max_co2 = buffer_co2[i];
    }
    float media = (float)somma / TOT_SAMPLES;

    Serial.println("\n========================================");
    Serial.println("       ANALISI DATI LETTI DA EEPROM     ");
    Serial.println("========================================");
    Serial.print("Valore Minimo : "); Serial.println(min_co2);
    Serial.print("Valore Massimo : "); Serial.println(max_co2);
    Serial.print("Valore Medio   : "); Serial.println(media, 1);
    Serial.println("----------------------------------------");

    // --- B. Test del Filtro a Media Mobile Esponenziale (EMA) ---
    // NOTA: Puoi modificare questo coefficiente alpha (da 0.05 a 0.8) 
    // e riavviare/ricalibrare per vedere subito l'andamento sul Monitor Seriale!
    float alpha = 0.2f; 
    // float co2_filtrata = buffer_co2[0]; // Inizializziamo il filtro con il primo campione
    
    float co2_filtrata = 605.0;
    Serial.println("Indice |  Grezzo (ppm)  |  Filtrato (ppm)");
    Serial.println("----------------------------------------");
    for (int i = 0; i < TOT_SAMPLES; i++) {
        // Formula del filtro EMA: y[k] = alpha * x[k] + (1 - alpha) * y[k-1]
        co2_filtrata = (alpha * buffer_co2[i]) + ((1.0f - alpha) * co2_filtrata);

        // Stampa tabulare per un controllo visivo immediato
        Serial.print("  ");
        if (i < 10) Serial.print("0");
        Serial.print(i);
        Serial.print("   |      ");
        Serial.print(buffer_co2[i]);
        Serial.print("     |      ");
        Serial.println(co2_filtrata, 1);
    }
    Serial.println("========================================\n");
}

/* -------------------------------------------------------------------------------------------------------------------- */
void PrintUint64(uint64_t value, int sensIx, uint16_t Diagnostic, const char* Str) {
    char errorMessage[64];
    uint16_t Most = (value >> 32) & 0xFFFF;
    uint16_t most = (value >> 16) & 0xFFFF;
    uint16_t less =  value  & 0xFFFF;

    const char* Name = SensorNames::Names[sensIx];
    if(Diagnostic == 0 ) {
        if( value == 0) {
            Serial.printf("\n%s   OK  %s", Name, Str); }
        else {
        Serial.printf("\n%s   OK  S/N :  0x%04X.%04X.%04X", Name, Most, most, less); }
    }
    else if(Diagnostic == UNKNOWN_ERR) { 
        Serial.printf("\n%s  NOK Errore : %s  %s", Name, "?? ", Str); }
    
    else { 
        errorToString(Diagnostic, errorMessage, sizeof errorMessage);
        Serial.printf("\n%s   NOK Errore: 0x%04X  %s   %s", Name, Diagnostic, errorMessage, Str);
    }
}