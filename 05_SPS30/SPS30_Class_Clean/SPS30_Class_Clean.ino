/* ----------------------------------------------------------------------------------- */
#include <Arduino.h>
#include <SensirionI2cSps30.h>
#include <Wire.h>
#include "Constants.h"
#include "SPS30.h"

#define I2C_SDA       3
#define I2C_SCL       4
#define I2C_SPEED     100000

/* ----------------------------------------------------------------------------------- */ 
const uint32_t  END_WAIT_MSEC = 30000;    // 180000 (30')
static uint32_t Start_Millis;

/* ----------------------------------------------------------------------------------- */
void setup() {
    Serial.begin(115200);
    delay(1000);

    Start_Millis = millis();
    Wire.begin(I2C_SDA, I2C_SCL, I2C_SPEED);

    //Sht45_mngr.Init();
    //Bmp390_mngr.Init();
    //Sgp4x.Init();
    //Scd41_mngr.Init();
    Sps30_mngr.Init(Wire);
}

/* ----------------------------------------------------------------------------------- */
void loop() {
    uint32_t Curr_Millis = millis(); 
    uint32_t DeltaMillis = Curr_Millis - Start_Millis;
    uint32_t msTime_ToSleep;

    delay(100);
    if(DeltaMillis >= END_WAIT_MSEC) {
        Start_Millis = Curr_Millis;
        Sps30_mngr.Cycle_Restart();
    }

    Sps30_mngr.Next_Step(DeltaMillis);

    delay(1400);
    msTime_ToSleep = MSEC_BASE_TIME - (millis() - Curr_Millis);
    // ---   LIGTH SLEEP   ---
    // esp_sleep_enable_timer_wakeup(msTime_ToSleep * 1000);   // Time in microseconds
    // esp_light_sleep_start();
    delay(msTime_ToSleep);
}
