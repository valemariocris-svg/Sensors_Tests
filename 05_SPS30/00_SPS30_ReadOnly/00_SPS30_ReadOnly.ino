/* ----------------------------------------------------------------------------------- */
/*

*/


#include <Arduino.h>
#include <Wire.h>
#include <SensirionI2cSps30.h>      // SPS30

// Factor to convert sec in microseconds
#define uS_TO_S_FACTOR 1000000ULL  
#define TIME_TO_SLEEP_S 60          // seconds

#define LedPIN             13
#define TestPIN            4
#define Switch1_PIN        5
#define Switch2_PIN        6

#define I2C_SCL            1
#define I2C_SDA            5
#define SPS30_I2C_ADDRESS  0x69

// Prototipi
bool Init_SPS30();
boolean Check_Switch_Closed();
void ReadMeasures();

// Variabili globali
boolean   SPS30_Connected = true;
uint32_t  LedPause       = 500;
uint32_t  autoCleaningInterval;

int16_t  errorRes;
int8_t   serialNumber[32];
String   SerialNum_String = "";


/* ----------------------------------------------------------------------------------- */
// data structure of sensors values to be saved on flash
struct __attribute__((packed)) Meas_Packet {
    // 1. TIMESTAMP (4 byte)
    uint8_t  date[4]; 

    // 2. AMBIENTE (14 byte)
    int16_t temp;      // SHT45 (x100)
    int16_t umid;      // SHT45 (x100)
    int16_t press;     // BMP390 (offset 900, x10)
    int16_t altitud;   // BMP390 (metri)
    int16_t voc;       // SGP41 (indice)
    int16_t nox;       // SGP41 (indice)
    int16_t co2;       // SCD41 (ppm)

    // 3. SPS30 PARTICOLATO (18 byte)
    // Usiamo int16_t per mantenere la precisione x10 o x1 (es. 50.5 ug/m3 -> 505)
    int16_t pm_mc_1p0, pm_mc_2p5, pm_mc_4p0, pm_mc_10p0; 
    int16_t pm_nc_0p5, pm_nc_1p0, pm_nc_2p5, pm_nc_4p0, pm_nc_10p0;

    // 4. BATTERIA / POWER (8 byte)
    int16_t v_bat;     // INA228 (mV)
    int16_t i_bat;     // INA228 (mA)
    int16_t mah_cons;  // INA228 (mAh)
    int16_t pwr_res1;  // Libero

    // 5. ESPANSIONE (20 byte)
    int16_t disponibili[10]; 
}; // TOTALE: 64 Byte

/* ----------------------------------------------------------------------------------- */
/*             the packet of measures to be saved in Flash (after averages)            */
/* ----------------------------------------------------------------------------------- */
Meas_Packet Meas;

/* ----------------------------------------------------------------------------------- */
/*       the actual measures values for averaging & to create the Meas packet         */
/* ----------------------------------------------------------------------------------- */
uint16_t mc1p0, mc2p5, mc4p0, mc10p0, 
         nc0p5, nc1p0, nc2p5, nc4p0, nc10p0, 
         typicalParticleSize;

SensirionI2cSps30 _sps30;



/* ----------------------------------------------------------------------------------- */
void setup() {
    pinMode(LedPIN,  OUTPUT);
    pinMode(TestPIN, OUTPUT);
    pinMode(Switch1_PIN, INPUT_PULLUP);
    pinMode(Switch2_PIN, INPUT_PULLUP);
    digitalWrite(LedPIN, HIGH);
    digitalWrite(TestPIN, HIGH);

    Serial.begin(115200);
    delay(500);
    // Serial.println("SETUP\n");

    // // 1) Inizializzazione  Wire  ed  SPS30
    if(!Init_SPS30()) {
        LedPause = 50;
        SPS30_Connected = false;
    }
}

/* ----------------------------------------------------------------------------------- */
void loop() { 

    digitalWrite(LedPIN, HIGH);
    digitalWrite(TestPIN, HIGH);
    delay(LedPause);
    digitalWrite(LedPIN, LOW);
    digitalWrite(TestPIN, LOW);
    delay(LedPause);

    if(Check_Switch_Closed()) {
        Init_SPS30();
        while(Check_Switch_Closed());
    }
}


/* =================================================================================== */
boolean Check_Switch_Closed() {
  if(digitalRead(Switch1_PIN) == LOW) {
    delay(10);
    if(digitalRead(Switch1_PIN) == LOW) return true;
    else                              return false;
  }
  else return false;
}

/* ----------------------------------------------------------------------------------- */
bool Init_SPS30() {
    if(!Wire.begin(I2C_SDA, I2C_SCL, 100000)) {
        Serial.println("Wire.begin NOT OK!");
        return false;
    }
    _sps30.begin(Wire, SPS30_I2C_ADDRESS);
    errorRes = _sps30.readAutoCleaningInterval(autoCleaningInterval);
    if(errorRes !=0) {
        Serial.printf("\nSPS30 NOT responding  ***   Err: %5d   ***", errorRes);
        return false;
    }
    else {
        // Serial.printf("\nSPS30 cleaning interval = %6d n", (int) autoCleaningInterval);
        // Serial.print("\nAutocleaning inteval (s) = ");
        int intAutoClean = (int) autoCleaningInterval;
        int Days = intAutoClean/(3600*24);
        Serial.printf("\nautocleanig interval:  s=%d   days=%d\n", intAutoClean, Days);
        return true;
    }
}

/* ----------------------------------------------------------------------------------- */
void Read_Measures() {
    int16_t ReadError = _sps30.readMeasurementValuesUint16(mc1p0, mc2p5, mc4p0, mc10p0, 
                                                        nc0p5, nc1p0, nc2p5, nc4p0, nc10p0, 
                                                        typicalParticleSize);
    if(ReadError == 0) {
        Serial.printf("\n%5d  %5d  %5d  %5d  %5d  %5d %5d  %5d %5d  %5d", 
                         mc1p0, mc2p5, mc4p0, mc10p0,nc0p5, nc1p0, nc2p5, nc4p0, nc10p0, 
                                                        typicalParticleSize);
    }
    else{
        Serial.printf("\nRead ERROR : %d", ReadError);
    }
}
/* =================================================================================== */




//   // 2) Avvio misura
//   sps30_start_measurement();
//   delay(2000); // Breve pausa per stabilizzare la ventola

//   // 3) Ciclo lettura PM
//   struct sps30_measurement m;
//   uint16_t ret = sps30_read_measurement(&m);

//   if (ret == 0) {
//     Serial.printf("PM1.0: %.2f | PM2.5: %.2f | PM10: %.2f\n", m.mc_1p0, m.mc_2p5, m.mc_10p0);
//     Serial.printf("Dim. Tipica: %.2f\n", m.typical_particle_size);
//     ReadCount++;
//   } else {
//     Serial.println("Errore durante la lettura.");
//   }

//   // 4) Controllo pulizia ventola (ogni 30 letture)
//   if (ReadCount >= 30) {
//     Serial.println("Avvio pulizia manuale ventola...");
//     sps30_start_fan_cleaning();
//     delay(10000); // La pulizia dura circa 10 secondi
//     ReadCount = 0;
//   }

//   // 5) Preparazione allo Sleep
//   Serial.println("SPS30 in Sleep mode.");
//   sps30_stop_measurement();
//   sps30_sleep();

//   Serial.println("ESP32 in Light Sleep per 5 minuti...");
//   Serial.flush(); // Assicura che i messaggi seriali siano inviati prima dello sleep

//   // Configurazione Light Sleep
//   esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP_S * uS_TO_S_FACTOR);
//   esp_light_sleep_start();

//   // Al risveglio il codice riparte da qui nel loop()
//   Serial.println("ESP32 Sveglio!");

/* ----------------------------------------------------------------------------------- */
