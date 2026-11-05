#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "SCD41Sensor.h"

// DECLARATION: We promise these exist in the .cpp/.ino files
extern SCD41Sensor* globalSensorPtr;
extern SemaphoreHandle_t i2cMutex;

#endif