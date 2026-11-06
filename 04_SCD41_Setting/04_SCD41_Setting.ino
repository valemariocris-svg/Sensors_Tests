/* ---------------------------------------------------------------------------------------- */
#include "Common.h"

/* ---------------------------------------------------------------------------------------- */





/* ---------------------------------------------------------------------------------------- */
uint64_t previous_millis = 0;
int64_t current_time;
int64_t previous_time;
int64_t cycle_time;
int64_t active_time;
int64_t sleep_time;
int16_t seconds_counter = 0;

/* ---------------------------------------------------------------------------------------- */
uint16_t uintCO2;
float    t_reference,   rh_reference, pres, fCO2;
float    t_scd41,       rh_scd41;
float    currentOffset, t_offset_actual;

int16_t  buffer_index = 0;

int16_t measures_count   = 0;
int16_t first_meas_Count = 0;


void set_RGB_colors(uint8_t valRed, uint8_t valGreen, uint8_t valBlue);
void PrintUint64(uint64_t value, int sensIx, uint16_t Diagnostic, const char* Str);
void Read_CO2_Measure();
void Wait_start_Measure();
void Wait_read_Measure();

/* -------------------------------------------------------------------------------------------------------------------- */
void setup() {
    Serial.begin(115200);
    while (!Serial) {
        delay(100);
    }
    set_RGB_colors(0, 0, 24);   // Blue first measure to skip, Green: reading, Red: end of nn cycles
    delay(4000);
    pinMode(TEST_PIN, OUTPUT);
    Serial.println("\n\n\n------ Calibrazione SCD41 in Single-Shot 5/11 3 files ------");

    if(PRINT_RESULT) {
        leggiEStampaTestFiltro();
        set_RGB_colors(0, 0, 24);
    }
    if(!READ_MEAS_ENABLE) {
        while(1) delay(50);
    }

    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

    if(SHT45_begin() !=0) {
        set_RGB_colors(24, 0, 0);
        while(1) delay(50);  
    }

    if(BMP390_begin() !=0) {
        set_RGB_colors(24, 0, 0);
        while(1) delay(50);  
    }

    if(SCD41_begin() !=0) {
        set_RGB_colors(24, 0, 0);
        while(1) delay(50);  
    }
    previous_millis = millis();

}

/* -------------------------------------------------------------------------------------------------------------------- */
void loop() {
    digitalWrite(TEST_PIN, HIGH);
    current_time  = esp_timer_get_time();
    cycle_time    = current_time - previous_time;
    previous_time = current_time;

    // switch (test_state) {
    //     case STATE_START_MEASURE:
    //         Wait_start_Measure();
    //         break;

    //     case STATE_WAIT_MEASUREMENT: 
    //         Wait_read_Measure();
    //         break;

    //     default:
    //         test_state = STATE_START_MEASURE;
    //         seconds_counter = 0;
    //         break;
    // }

    //     // if(++first_meas_Count == INIT_MEAS_TO_SKIP) {
    //     //     test_state = STATE_WAIT_MEASUREMENT;
    //     //     measures_count = 0;
    //     // }
    //     // else measures_count++;


    //     active_time = esp_timer_get_time() - current_time;

        // Calcola il tempo rimanente per completare il ciclo di  CYCLE_TIME_US 
        sleep_time = CYCLE_TIME_US - active_time;
        if(sleep_time > CYCLE_TIME_US) sleep_time = CYCLE_TIME_US;
        if(sleep_time < 2 ) sleep_time = CYCLE_TIME_US;


        if(++seconds_counter >= WAIT_SEC_X_MEAS) {     
            set_RGB_colors(24, 0, 0);
            while(1) delay(50);
        }





        if(SLEEP_ENABLED) {
             // 3. Configura il timer di risveglio per il tempo residuo
            esp_sleep_enable_timer_wakeup((uint64_t) sleep_time);
            digitalWrite(TEST_PIN, LOW);
            // 4. Entra in Light Sleep (la CPU si ferma, i registri e la RAM sono preservati)
            esp_light_sleep_start();
        }
        else  {
            digitalWrite(TEST_PIN, LOW);
            delayMicroseconds((uint64_t) sleep_time);
            Serial.printf("\nDelay 1000");
        }
        // il ciclo for ricomincia automaticamente da capo
}


/* -------------------------------------------------------------------------------------------------------------------- */
void Wait_start_Measure() {
    // Attesa lettura misura / Agg.to pressione
    if(seconds_counter > WAIT_SEC_X_MEAS) {
        // Aggiorniamo la pressione ambiente sul BMP390 e la passiamo all'SCD41
        if (bmp390.performReading()) {
            pres                 = (float) bmp390.pressure;
            uint32_t pressure_pa = (uint32_t) pres;
            scd41.setAmbientPressure(pressure_pa); // Compensazione di pressione attiva
        }

        // Avviamo la misura a singolo colpo sull'SCD41
        int16_t error = scd41.measureSingleShot();
        if (error == 0) {
            // Serial.println("\n[SCD41] Misura Single-Shot avviata. Attesa completamento (~5s)...");
            test_state = STATE_WAIT_MEASUREMENT;
        } else {
            Serial.println("[SCD41] Errore nell'avvio della misura single shot!");
        }
    }
}


/* -------------------------------------------------------------------------------------------------------------------- */
void Wait_read_Measure() {
    // Verifichiamo con getDataReadyStatus se il sensore ha completato i 5 secondi di elaborazione
    bool data_ready = false;
    int16_t error = scd41.getDataReadyStatus(data_ready);
    Serial.printf("\nerror=%d    ready=%d", error, data_ready);

    if (error == 0 && data_ready) {
        // Dati pronti! Leggiamo l'SCD41
        if (scd41.readMeasurement(uintCO2, t_scd41, rh_scd41) == 0) {
            fCO2 = (float) uintCO2;
            // Leggiamo il riferimento pulito dall'SHT45
            sht45.measureHighPrecision(t_reference, rh_reference);

            // Leggiamo la pressione attuale per il log
            float pressure_hpa = bmp390.pressure / 100.0;

            // F. Recuperiamo l'offset attuale per la formula
            currentOffset = 0.0;
            scd41.getTemperatureOffset(currentOffset);

            // Calcolo della formula ufficiale Sensirion:
            // Toffset_actual = TSCD4x - TReference + Toffset_previous
            t_offset_actual = t_scd41 - t_reference + currentOffset;

            // Stampa del report sul monitor seriale
            //             Co2    t_Scd41  t_reference  rh_reference   currentOffset   t_offset_actual
            if(first_meas_Count >= INIT_MEAS_TO_SKIP) {
                buffer_co2[buffer_index++] = fCO2; 
                set_RGB_colors(0, 14, 0); 
            }
            Serial.printf("\n%03i  %u   %.2f   %.2f   %.0f    %.2f    %.2f", 
                            buffer_index, uintCO2, t_scd41, t_reference, rh_reference, currentOffset, t_offset_actual);   

            test_state = STATE_START_MEASURE;
            if(++first_meas_Count == INIT_MEAS_TO_SKIP) measures_count = 0;
            else measures_count++;     
        }
    }
}


/* -------------------------------------------------------------------------------------------------------------------- */
void Read_CO2_Measure() {
//     unsigned long current_millis = millis();
 
//     switch (test_state) {
//         case STATE_START_MEASURE: {
//             // Ogni xx secondi avviamo una singola misura
//             if (current_millis - previous_millis >= CYCLE_MS) {
//                 previous_millis = current_millis;

//                 // A. Aggiorniamo la pressione ambiente sul BMP390 e la passiamo all'SCD41
//                 if (bmp390.performReading()) {
//                     pres                 = (float) bmp390.pressure;
//                     uint32_t pressure_pa = (uint32_t) pres;
//                     scd41.setAmbientPressure(pressure_pa); // Compensazione di pressione attiva
//                 }

//                 // B. Avviamo la misura a singolo colpo sull'SCD41
//                 int16_t error = scd41.measureSingleShot();
//                 if (error == 0) {
//                     // Serial.println("\n[SCD41] Misura Single-Shot avviata. Attesa completamento (~5s)...");
//                     test_state = STATE_WAIT_MEASUREMENT;
//                 } else {
//                     Serial.println("[SCD41] Errore nell'avvio della misura single shot!");
//                 }
//             }
//             break;
//         }

//         case STATE_WAIT_MEASUREMENT: {
//             // Verifichiamo con getDataReadyStatus se il sensore ha completato i 5 secondi di elaborazione
//             bool data_ready = false;
//             int16_t error = scd41.getDataReadyStatus(data_ready);

//             if (error == 0 && data_ready) {
//                 // C. Dati pronti! Leggiamo l'SCD41
       
//                 if (scd41.readMeasurement(uintCO2, t_scd41, rh_scd41) == 0) {
//                     fCO2 = (float) uintCO2;
//                     // D. Leggiamo il riferimento pulito dall'SHT45
//                     sht45.measureHighPrecision(t_reference, rh_reference);

//                     // E. Leggiamo la pressione attuale per il log
//                     float pressure_hpa = bmp390.pressure / 100.0;

//                     // F. Recuperiamo l'offset attuale per la formula
//                     currentOffset = 0.0;
//                     scd41.getTemperatureOffset(currentOffset);

//                     // G. Calcolo della formula ufficiale Sensirion:
//                     // Toffset_actual = TSCD4x - TReference + Toffset_previous
//                     t_offset_actual = t_scd41 - t_reference + currentOffset;

//                     // H. Stampa del report sul monitor seriale
//                     //             Co2    t_Scd41  t_reference  rh_reference   currentOffset   t_offset_actual
//                     if(first_meas_Count >= INIT_MEAS_TO_SKIP) {
//                         buffer_co2[buffer_index++] = fCO2; 
//                         set_RGB_colors(0, 14, 0); 
//                     }
//                     Serial.printf("\n%03i  %u   %.2f   %.2f   %.0f    %.2f    %.2f", 
//                                     buffer_index, uintCO2, t_scd41, t_reference, rh_reference, currentOffset, t_offset_actual);        
                    
//                     // Serial.println("\n==========================================");
//                     // Serial.print("Pressione Ambientale (BMP390): "); Serial.print(pressure_hpa, 2); Serial.println(" hPa");
//                     // Serial.print("Temperatura Reale (SHT45 - Ref): "); Serial.print(t_reference, 2); Serial.println(" °C");
//                     // Serial.print("CO2 (SCD41 Single-Shot): "); Serial.print(co2); Serial.println(" ppm");
//                     // Serial.print("Temperatura Interna (SCD41): "); Serial.print(t_scd41, 2); Serial.println(" °C");
//                     // Serial.println("------------------------------------------");
//                     // Serial.print("Offset Attuale in Uso: "); Serial.print(currentOffset, 2); Serial.println(" °C");
//                     // Serial.print("==> NUOVO OFFSET CALCOLATO: "); Serial.print(t_offset_actual, 2); Serial.println(" °C");
//                     // Serial.println("==========================================");

//                     /*
//                        === COME SALVARE IL NUOVO OFFSET IN EEPROM ===
//                        Quando il sistema è a regime (dopo ~30 minuti nel box chiuso) e il 
//                        valore calcolato è stabile, decommenta questo blocco per scriverlo permanentemente:
                       
//                        Serial.println("Scrittura del nuovo offset nella EEPROM...");
//                        scd41.stopPeriodicMeasurement(); // Necessario per andare in idle prima di scrivere
//                        scd41.setTemperatureOffset(t_offset_actual);
//                        scd41.persistSettings();
//                        Serial.println("Salvato con successo! Riavvia la scheda.");
//                        while(true); // Blocca l'esecuzione
//                     */
//                 }

//                 // Torniamo in attesa del prossimo intervallo di 15s
//                 test_state = STATE_START_MEASURE;
//                 previous_millis = millis(); // Riavvia il timer dei 15s dal completamento
//                 if(++first_meas_Count == INIT_MEAS_TO_SKIP) measures_count = 0;
//                 else measures_count++;
//             }
//             break;
//         }
//     }
}

