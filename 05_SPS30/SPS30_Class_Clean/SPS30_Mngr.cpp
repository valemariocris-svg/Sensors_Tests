/* ------------------------------------------------------------------------- */
#include <Arduino.h>
#include <SensirionI2cSps30.h>
#include <Constants.h>
#include "SPS30.h" 


/* =====================  class   SPS30_Mngr   ============================= */
SPS30_Mngr::SPS30_Mngr()  {}


/* ------------------------------------------------------------------------- */
/*                  on error   set     _SPS30_Ready = false;                 */
/* ------------------------------------------------------------------------- */
void SPS30_Mngr::Init(TwoWire &IIC_Wire) {
    snprintf(_buffer, sizeof(_buffer), "%s", 
              "\n\n\n\n***   Restart Meteo Station   ***");
    _PrintStr(_buffer);
    _Print_Count = 0;
    _Sensirion_sps30.begin(IIC_Wire, SPS30_I2C_ADDRESS);
    _SPS30_Ready = true;
    _status      = PRE_WAIT;
    _Sensirion_sps30.begin(Wire, SPS30_I2C_ADDRESS);

    _print_Ser_Prodt();
    
    Serial.println("Stop measurement\n");
    _stop_Measure();
}


/* ------------------------------------------------------------------------- */
void SPS30_Mngr::Next_Step(uint32_t DeltaMillis) {
    if(!_SPS30_Ready) return;

    switch (_status) {
        case PRE_WAIT:
            if(_Print_Count++ == 0) {
                snprintf(_buffer, sizeof(_buffer), "%s", "\nPRE_WAIT");
                _PrintStr(_buffer);
            }
            else {
                snprintf(_buffer, sizeof(_buffer), "%s", " ."); 
                _PrintStr(_buffer);  
            }
            if (DeltaMillis >= _PRE_WAIT_MILLIS) {
                _status = WARM_READ;
                _start_measure();
                _Print_Count = 0;
            }
            break;

        case WARM_READ:
            // Waiting for WARM  then  get_measures 
            if (DeltaMillis >= (_PRE_WAIT_MILLIS + _WARM_DURATION)) {
                snprintf(_buffer, sizeof(_buffer), "\n...READ"); 
                _PrintStr(_buffer);
                _get_measures();
                _status = POST_WAIT;
            }
            else {
                snprintf(_buffer, sizeof(_buffer), "\nWARM"); 
                _PrintStr(_buffer);
            }  
            break;

        case POST_WAIT:
            // Waiting for Cycle_Restart
            snprintf(_buffer, sizeof(_buffer), "\nPOST_WAIT"); 
            _PrintStr(_buffer);
            break;

        default:
            break;
    }
}

/* ------------------------------------------------------------------------- */
void SPS30_Mngr::Cycle_Restart() { 
    _status = PRE_WAIT;
    snprintf(_buffer, sizeof(_buffer), "%s", "\n------------------\n");
    _PrintStr(_buffer);
}

/* ------------------------------------------------------------------------- */
/*                  on error   set     _SPS30_Ready = false;                 */
/* ------------------------------------------------------------------------- */
void SPS30_Mngr::_start_measure() {
    int Result;
    char errorMessage[64];

    Result = _Sensirion_sps30.startMeasurement(SPS30_OUTPUT_FORMAT_OUTPUT_FORMAT_FLOAT);
    if (Result != 0) {
        errorToString(Result, errorMessage, sizeof errorMessage);
        Serial.printf("Error on Flag Read:  %d  **  %s   **", Result, errorMessage);
        _SPS30_Ready = false;
    }
    else {
        _PrintStr(_buffer);
    }
}

/* ------------------------------------------------------------------------- */
/*                  on error   set     _SPS30_Ready = false;                 */
/* ------------------------------------------------------------------------- */
void SPS30_Mngr::_get_measures() {
    int Result;
    char errorMessage[64];

    uint16_t dataReadyFlag = 0;
    float        mc1p0, mc2p5, mc4p0, mc10p0,
          nc0p5, nc1p0, nc2p5, nc4p0, nc10p0,
          typicalParticleSize;

    Result = _Sensirion_sps30.readDataReadyFlag(dataReadyFlag);
    if (Result != 0) {
        errorToString(Result, errorMessage, sizeof errorMessage);
        Serial.printf("Error on Flag Read:  %d  **  %s   **", Result, errorMessage);
        _SPS30_Ready = false;
    }
    else {
        Serial.printf("\ndataready flag: %d", dataReadyFlag);
        for(int i=0; i<4; i++) {
            Result = _Sensirion_sps30.readMeasurementValuesFloat(mc1p0, mc2p5, mc4p0, mc10p0,
                                                    nc0p5, nc1p0, nc2p5, nc4p0,
                                                    nc10p0, typicalParticleSize);
            if (Result != 0) {
                Serial.print("Error trying to execute readMeasurementValuesFloat(): ");
                errorToString(Result, errorMessage, sizeof errorMessage);
                Serial.println(errorMessage);
                _SPS30_Ready = false;
                break;
            }
            else {
                Serial.printf("\nmc2p5: %f  TypSize: %f", mc2p5, typicalParticleSize);
                delay(100);
            }
        }
    }
    _stop_Measure();
}

/* ------------------------------------------------------------------------- */
/*                  on error   set     _SPS30_Ready = false;                 */
/* ------------------------------------------------------------------------- */
void SPS30_Mngr::_stop_Measure() {
    int Result;
    Result = _Sensirion_sps30.stopMeasurement();
    if(Result !=0 ) {
        snprintf(_buffer, sizeof(_buffer), "%s %d", "\Error on stop measurement: ", Result);
        _PrintStr(_buffer);
        _SPS30_Ready = false;
    }
}



void SPS30_Mngr::_Clean_Count() {
    // prefs.begin("sps30_data", false);

    // Controlliamo se il sistema è mai stato inizializzato
    // if (!prefs.isKey("init")) {
    //     Serial.println("SPS30: Primo avvio assoluto. Inizializzo memoria...");
    //     prefs.putUInt("cicli", 0);
    //     // prefs.putBool("init", true); // Da ora in poi questa chiave esisterà
    // }

    // uint16_t n = prefs.getUInt("cicli", 0);
    // Serial.printf("SPS30: Cicli memorizzati all'avvio: %d\n", n);
    
    // prefs.end();
}

/* ------------------------------------------------------------------------- */
/*                  on error   set     _SPS30_Ready = false;                 */
/* ------------------------------------------------------------------------- */
void SPS30_Mngr::_print_Ser_Prodt() {
    int Result = 180;
    int8_t serialNumber[32] = {0};
    int8_t productType[8]   = {0};

    while(Result != 0) {
        Result = _Sensirion_sps30.readSerialNumber(serialNumber, 32);
        if(Result == 0) {
            Serial.print("\nserialNumber: ");
            Serial.print((const char*) serialNumber);
        }
        else {
            Serial.printf("\nError on reading serial number: %d", Result);
            // _SPS30_Ready = false;
            // return;
        }
        delay(200);
    }

    Result = 180;
    while(Result != 0) {
        Result = _Sensirion_sps30.readProductType(productType, 8);
        if(Result == 0) {
            Serial.print("\nproductType: ");
            Serial.print((const char*) productType);
            Serial.println();
        }

        else {
            Serial.printf("\nError on reading product type: %d", Result);
            // _SPS30_Ready = false;
            // return;
        }
        delay(200);
    }
 }

/* --------------------------------------------------------------------------------- */
void SPS30_Mngr::_Clean_fan_manager() {
    // 1. Apri lo spazio di memoria "sps30_data" in modalità lettura/scrittura
    _prefs.begin("sps30_data", false);

    // 2. Leggi il valore attuale (se non esiste, restituisce 0)
    uint16_t nCicli = _prefs.getUInt("cicli", 0);
    nCicli++;

    if (nCicli >= 336) {
        Serial.println("SPS30: Soglia settimanale raggiunta. Avvio pulizia...");
        _Sensirion_sps30.startFanCleaning();
        nCicli = 0; // Reset dopo la pulizia
    }

    // 3. Salva il nuovo valore e chiudi
    _prefs.putUInt("cicli", nCicli);
    _prefs.end();
    
    Serial.printf("SPS30: Ciclo %d di 336 salvato in memoria.\n", nCicli);
}


/* --------------------------------------------------------------------------------- */
String _Convert_int8_To_String(int* Number) {
    int* pNum = Number;
    String hexStr = "";
    for (int i = 0; i < 32; i++) {
        char buf[3];
        sprintf(buf, "%02X", *pNum++);
        hexStr += buf;
    }
    Serial.println(hexStr);
    return hexStr;
}

/* --------------------------------------------------------------------------------- */
void SPS30_Mngr::_PrintStr(String str) {
    if(_printEnabd) {
        Serial.print(str);
        // delay(80);
    }
}


/* --------------------------------------------------------------------------------- */
/*                                                                                   */
SPS30_Mngr Sps30_mngr; /*                                                            */
/*                                                                                   */       
/* --------------------------------------------------------------------------------- */