#include "SCD41Sensor.h"
#include "Globals.h"

SCD41Sensor::SCD41Sensor() : _currentState(STATE_INIT), _lastCO2(0), _lastTemp(0.0) {}

bool SCD41Sensor::begin() {
    Wire.begin(11, 10, 100000);
    _driver.begin(Wire, 0x69);
    return (_driver.stopPeriodicMeasurement() == 0);
}

void SCD41Sensor::updateStateMachine() {
    switch (_currentState) {
        case STATE_INIT:
            // Setup sensor parameters if needed
            _currentState = STATE_START_MEASURE;
            break;

        case STATE_START_MEASURE:
            if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
                _driver.measureSingleShot(); // SCD41 Single Shot command
                xSemaphoreGive(i2cMutex);
                _currentState = STATE_WAITING;
            }
            break;

        case STATE_WAITING:
            // SCD41 needs ~5000ms for a single shot measurement
            vTaskDelay(pdMS_TO_TICKS(5000)); 
            _currentState = STATE_READ_DATA;
            break;

        case STATE_READ_DATA:
            if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
                uint16_t ready = 0;
                _driver.getDataReadyStatus(ready);
                
                if (ready) {
                    _driver.readMeasurement(_lastCO2, _lastTemp, _lastTemp);
                }
                xSemaphoreGive(i2cMutex);
                _currentState = STATE_IDLE;
            }
            break;

        case STATE_IDLE:
            // Wait 55 seconds to complete a 1-minute cycle
            vTaskDelay(pdMS_TO_TICKS(55000));
            _currentState = STATE_START_MEASURE;
            break;
    }
}