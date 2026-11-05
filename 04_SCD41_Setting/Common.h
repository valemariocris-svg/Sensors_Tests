#ifndef COMMON_H
#define COMMON_H

#include <Arduino.h>
#include <Wire.h>
#include <SensirionI2cScd4x.h>
#include <SensirionI2cSht4x.h>
#include <Adafruit_BMP3XX.h>
#include "esp_timer.h"
#include "esp_sleep.h"
#include <EEPROM.h>

/* ---------------------------------------------------------------------------------------- */
#define I2C_SCL_PIN 1 
#define I2C_SDA_PIN 5
#define TEST_PIN    10

#define SHT45_I2C_ADDRESS  0x44
#define BMP390_I2C_ADDRESS 0x77
#define SGP4x_I2C_ADDRESS  0x59
#define SCD41_I2C_ADDRESS  0x62
#define SPS30_I2C_ADDRESS  0x69

#define indexSHT45        0
#define indexBMP390       1
#define indexSCD41        2
#define indexSGP41        3
#define indexSPS30        4

#define ERR_NONE         0     
#define UNKNOWN_ERR      0xFF  
#define ERR_I2C_COMM     0x01  

#define TOT_SAMPLES       8
#define EEPROM_SIZE       (TOT_SAMPLES * 8)
#define INIT_MEAS_TO_SKIP 6
#define WAIT_SEC_X_MEAS   15

const int64_t CYCLE_TIME_US = 1000000;

// SOLO DICHIARAZIONI (senza assegnare il valore qui!)
extern bool PRINT_RESULT;
extern bool READ_MEAS_ENABLE;
extern bool SLEEP_ENABLED;

enum TestState {
    STATE_START_MEASURE,
    STATE_WAIT_MEASUREMENT
};

extern float buffer_co2[TOT_SAMPLES];

extern TestState test_state;

extern SensirionI2cSht4x sht45;
extern Adafruit_BMP3XX   bmp390;
extern SensirionI2cScd4x scd41;

namespace SensorNames {
    static const char* Names[] = {
        "SHT45  Te-Um ",
        "BMP390 Pres. ",
        "SCD41  CO2   ",
        "SGP41  VOC   ",
        "SPS30  PM    "
    };
    static const int Count = sizeof(Names) / sizeof(Names[0]);
}

uint16_t SHT45_begin();
uint16_t BMP390_begin();
uint16_t SCD41_begin();

void set_RGB_colors(uint8_t valRed, uint8_t valGreen, uint8_t valBlue);
void PrintUint64(uint64_t value, int sensIx, uint16_t Diagnostic, const char* Str);
void save_samples_InEeprom();
void leggiEStampaTestFiltro();

#endif // COMMON_H