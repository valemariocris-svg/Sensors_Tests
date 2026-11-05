#include "DataCollector.h"
#include "Globals.h"

void DataCollector::processData() {
    // Safety check: Ensure the pointer has been initialized
    if (globalSensorPtr != nullptr) {
        if (globalSensorPtr->readSensor()) {
            Serial.print("CO2 Level: ");
            Serial.println(globalSensorPtr->getCO2());
        }
    } else {
        Serial.println("Error: Sensor object not initialized yet!");
    }
}