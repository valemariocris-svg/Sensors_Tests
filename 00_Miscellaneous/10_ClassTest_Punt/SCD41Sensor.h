#ifndef SCD41_SENSOR_H
#define SCD41_SENSOR_H

#include <Arduino.h>
#include <SensirionI2cScd4x.h>
#include <Wire.h>

// Sensor States
enum SensorState {
    STATE_INIT,
    STATE_START_MEASURE,
    STATE_WAITING,
    STATE_READ_DATA,
    STATE_IDLE
};

class SCD41Sensor {
public:
    SCD41Sensor();
    bool begin();
    
    // State Machine logic
    void updateStateMachine();

    // Data Collector Accessors (Getters)
    uint16_t getCO2() const { return _lastCO2; }
    float getTemp() const { return _lastTemp; }

private:
    SensirionI2CScd4x _driver;
    SensorState _currentState;
    
    // Private storage
    uint16_t _lastCO2;
    float _lastTemp;
};
#endif