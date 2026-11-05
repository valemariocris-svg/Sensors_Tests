#include <Wire.h>
#include <Adafruit_BMP3XX.h>

// Definisci i pin I2C che stai usando
#define I2C_SCL_PIN 1 
#define I2C_SDA_PIN 5

// Crea un oggetto per il sensore
Adafruit_BMP3XX bmp;

void setup() {
  delay(100);
  Serial.begin(115200);
  delay(500);

// CONFIGURAZIONE PORTA I2C: Assegna SDA e SCL ai pin GPIO scelti
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN); 

  // Inizializza il sensore. 
  // Usa "BMP390_I2C_ADDRESS" se hai una scheda breakout con questo indirizzo.
  // La libreria rileverà automaticamente se il chip è un BMP388, BMP390, etc.
  for(int i=0; i<10; i++) { 
    if (!bmp.begin_I2C()) {
      Serial.println("Errore: Impossibile trovare il sensore BMP390. Controlla i collegamenti!");
      delay(10000);
    }
    else break;
  }

  Serial.println("Sensore BMP390 trovato!");

  // --- CONFIGURAZIONE OPZIONALE (per migliorare la precisione) ---
  // Queste impostazioni ottimizzano la risoluzione del sensore e possono ridurre il rumore.

  // Imposta l'oversampling per la temperatura e la pressione
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);

  // Imposta il filtro IIR (Infinite Impulse Response) per ridurre il rumore
  // BMP3_IIR_FILTER_COEFF_3 è un buon compromesso
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);

  // Imposta la velocità di campionamento
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);
}

void loop() {
  // Controlla se i dati del sensore sono disponibili
  if (!bmp.performReading()) {
    Serial.println("Errore: Impossibile leggere i dati dal sensore BMP390.");
    delay(1000);
    return;
  }

  // Stampa i valori letti
  float Altitude = bmp.readAltitude(1019);   // Sea Level = 1013.25
  Serial.printf("BMP390 Temp: %.1f *C   Press: %.0f   Altitud (~): %.0f", bmp.temperature, bmp.pressure/100., Altitude);

  // Per calcolare l'altitudine, devi fornire la pressione al livello del mare.
  // Qui usiamo un valore standard di 1013.25 hPa come riferimento.
  // Puoi aggiornare questo valore con la pressione barometrica locale per maggiore precisione.
  // Serial.print("Altitudine approssimativa: ");
  // Serial.print(bmp.readAltitude(1013.25));
  // Serial.println(" metri");

  Serial.println("");
  
  delay(3000); // Leggi ogni secondo
}
