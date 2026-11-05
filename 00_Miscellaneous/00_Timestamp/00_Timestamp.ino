/* -----------------------------------------------------------------------------------
Con un record da 64 byte:
La tua memoria da 128Mbit (16 Megabyte) può contenere: 262.144 record.
Se logghi ogni minuto: hai 182 giorni di autonomia prima di riempire la memoria.
-------------------------------------------------------------------------------------*/


/* ----------------------------------------------------------------------------------- */
void setup() {
      Serial.begin(115200);
    delay(1000);

}

/* ----------------------------------------------------------------------------------- */
void loop() {
  // put your main code here, to run repeatedly:

}

/* ----------------------------------------------------------------------------------- */
void packDate(uint8_t* storage, int anno, int mese, int giorno, int ora, int minuto) {
    uint32_t packed = 0;
    packed |= ((anno - 2020) & 0x3F) << 20; // Max anno 2083
    packed |= (mese & 0x0F) << 16;
    packed |= (giorno & 0x1F) << 11;
    packed |= (ora & 0x1F) << 6;
    packed |= (minuto & 0x3F);
    
    memcpy(storage, &packed, 4);
}

/* 
void SensorsTask(void *pvParameters) {
    // ... setup ...
    for (;;) {
        LogRecord record = {0}; // Azzera tutto

        // Riempimento timestamp (esempio con variabili fittizie o da RTC)
        packDate(record.date, 2024, 5, 20, 14, 30);

        // Riempimento sensori
        record.temp    = mgrSHT.getTempInt();
        record.hum     = mgrSHT.getHumInt();
        record.press   = mgrBMP.getPressInt();
        record.altitud = mgrBMP.getAltInt();
        record.voc     = mgrSGP.getVOC();
        record.nox     = mgrSGP.getNOx();
        record.co2     = mgrSCD.getCO2();

        // Riempimento PM (Esempio scala 0-255)
        // record.pm_data[0] = (uint8_t)mgrSPS.getMC1p0(); ... ecc

        // SCRITTURA SU FLASH
        // flash.writeRecord((uint8_t*)&record, sizeof(record));

        // ... sleep 3s ...
    }
}
*/