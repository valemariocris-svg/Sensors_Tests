/*
 * Copyright (c) 2025, Sensirion AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Sensirion AG nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <Arduino.h>
#include <SensirionI2cSps30.h>
#include <Wire.h>


/* ----------------------------------------------------------------------------------- */
// SensirionI2cSps30 _sps30;

static bool       SPS30_ON     = true;

/* ----------------------------------------------------------------------------------- */
#define I2C_SCL            7
#define I2C_SDA            8
#define I2C_SPEED          100000
#define SPS30_I2C_ADDR_69  0x69
#define SEC_TO_MS_FACT     1000
#define SEC_TO_MICROS_FACT 1000000


#define PRE_WAIT_STATUS    1
#define WARM_STATUS        2
#define POST_WAIT_STATUS   3

#define MSEC_BASE_TIME      3000    // 3"
#define PRE_WAIT_MSEC       1000    // 1"
#define WARM_WAIT_MSEC      15000   // 15"  Start Meas 15"  make n Meas (100ms)
#define END_WAIT_MSEC       30000   // 30'

static uint8_t  SPS30_Status;

// static char     errorMessage[64];
// static int16_t  Result;
static uint32_t Start_Millis;


void SPS30_Init();
void SPS30_Start_Cycle();
void SPS30_Tick();
void _Read_SPS30_Meas();

/* ----------------------------------------------------------------------------------- */
void setup() {

    Serial.begin(115200);
    while (!Serial) {
        delay(100);
    }
    // Wire.begin(I2C_SDA, I2C_SCL, I2C_SPEED);
    // SPS30_Init();
    Start_Millis = millis();
    // SPS30_Start_Cycle();
}

/* ----------------------------------------------------------------------------------- */
void loop() {
    // uint32_t Curr_Millis = millis(); 
    // uint32_t DeltaMillis = Curr_Millis - Start_Millis;
    // if(DeltaMillis >= END_WAIT_MSEC) {
    //     Start_Millis = Curr_Millis;
    //     SPS30_Start_Cycle();
    // }
    // SPS30_Tick(DeltaMillis);
    // delay(10);
    // uint32_t msTime_ToSleep = MSEC_BASE_TIME - (millis() - Curr_Millis);
    // Serial.printf("\nDelta millis = %d", (int) msTime_ToSleep); 
    
    // // delay(msTime_ToSleep);
    // // ---   LIGTH SLEEP   ---
    // esp_sleep_enable_timer_wakeup(msTime_ToSleep * 1000);
    // esp_light_sleep_start();
}

/* ----------------------------------------------------------------------------------- */
void SPS30_Init() {
    // SPS30_Status = PRE_WAIT_STATUS;
    // _sps30.begin(Wire, SPS30_I2C_ADDR_69);
    // _sps30.stopMeasurement();
    // int8_t serialNumber[32] = {0};
    // int8_t productType[8] = {0};

    // Result = _sps30.readSerialNumber(serialNumber, 32);
    // if(Result != 0) {
    //     Serial.printf("\nError on reading serial number: %d", Result);
    //     SPS30_ON = false;
    //     return;
    // }
    // else {
    //     Serial.print("serialNumber: ");
    //     Serial.print((const char*) serialNumber);
    //     Serial.println();
    // }

    // Result = _sps30.readProductType(productType, 8);
    // if(Result != 0) {
    //     Serial.print("productType: ");
    //     Serial.print((const char*) productType);
    //     Serial.println();
    //     SPS30_ON = false;
    //     return;
    // }
    // else {
    //     _sps30.stopMeasurement();   // goto Idle
    //     delay(100);
    // }
}

/* ----------------------------------------------------------------------------------- */
void SPS30_Start_Cycle() {
    SPS30_Status = PRE_WAIT_STATUS;
    Serial.println("\n\nRestart cycle");
}

/* ----------------------------------------------------------------------------------- */
void SPS30_Tick(uint32_t Delta_Millis) {
    if(!SPS30_ON) return;

    switch(SPS30_Status) {
        case PRE_WAIT_STATUS:
            if(Delta_Millis >= PRE_WAIT_MSEC) {
                Serial.println("SPS30 laser and fan power on");
                // _sps30.startMeasurement(SPS30_OUTPUT_FORMAT_OUTPUT_FORMAT_FLOAT);
                SPS30_Status = WARM_STATUS;
            }
            break;

        case WARM_STATUS:
            if(Delta_Millis >= WARM_WAIT_MSEC) {
                // _Read_SPS30_Meas();                 // n times with delay(100)
                SPS30_Status = POST_WAIT_STATUS;
            }
            break;
        
        case POST_WAIT_STATUS:
            // waiting for SPS30_Start_Cycle()
            break;

        default:
            break;
    }
}

/* ----------------------------------------------------------------------------------- */
void _Read_SPS30_Meas() {
    // uint16_t dataReadyFlag = 0;
    // float mc1p0 = 0;
    // float mc2p5 = 0;
    // float mc4p0 = 0;
    // float mc10p0 = 0;
    // float nc0p5 = 0;
    // float nc1p0 = 0;
    // float nc2p5 = 0;
    // float nc4p0 = 0;
    // float nc10p0 = 0;
    // float typicalParticleSize = 0;

    // for(int i=0; i<5; i++) {
    //     Result = _sps30.readDataReadyFlag(dataReadyFlag);
    //     if (Result != 0) {
    //         // Serial.print("Error trying to execute readDataReadyFlag(): ");
    //         errorToString(Result, errorMessage, sizeof errorMessage);
    //         // Serial.println(errorMessage);
    //         Serial.printf("Error on Flag Read:  %d  **  %s   **", Result, errorMessage);
    //         SPS30_ON = false;
    //         break;
    //     }
    //     else {
    //         // Serial.print("dataReadyFlag: ");
    //         // Serial.print(dataReadyFlag);
    //         // Serial.println();

    //         Result = _sps30.readMeasurementValuesFloat(mc1p0, mc2p5, mc4p0, mc10p0,
    //                                                 nc0p5, nc1p0, nc2p5, nc4p0,
    //                                                 nc10p0, typicalParticleSize);
    //         if (Result != 0) {
    //             Serial.print("Error trying to execute readMeasurementValuesFloat(): ");
    //             errorToString(Result, errorMessage, sizeof errorMessage);
    //             Serial.println(errorMessage);
    //             SPS30_ON = false;
    //             break;
    //         }
    //         else {
    //             Serial.print("\nmc2p5: ");
    //             Serial.print(mc2p5);
    //             Serial.print("\t");
    //             Serial.print("\t");
    //             Serial.print("typicalParticleSize: ");
    //             Serial.print(typicalParticleSize);
    //             delay(100);
    //         }
    //     }
    // }
    // _sps30.stopMeasurement();
}
/* =================================================================================== */
