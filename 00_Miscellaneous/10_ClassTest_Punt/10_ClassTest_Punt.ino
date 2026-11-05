#include "Globals.h"
#include "SCD41Sensor.h"

// DEFINITION: Create the actual Mutex handle
SemaphoreHandle_t i2cMutex = nullptr;

void setup() {
    Serial.begin(115200);
    delay(1000);

    // 1. Create the Mutex
    i2cMutex = xSemaphoreCreateMutex();

    // 2. Instantiate the Sensor and assign it to the Global Pointer
    globalSensorPtr = new SCD41Sensor();

    // 3. Initialize Hardware
    if (globalSensorPtr->begin()) {
        Serial.println("System Initialized Successfully.");
    }

    // 4. Create RTOS Tasks
    if (i2cMutex != NULL && globalSensorPtr != nullptr) {
        xTaskCreatePinnedToCore(TaskReadSensor, "ReadTask", 4096, NULL, 1, NULL, 1);
        xTaskCreatePinnedToCore(TaskSerialDisplay, "SerialTask", 4096, NULL, 1, NULL, 0);
    }
}

void loop() {
    // RTOS handles everything; delete the default loop task
    vTaskDelete(NULL);
}

// --- TASK: READS THE SENSOR ---
void TaskReadSensor(void *pvParameters) {
    for (;;) {
        if (xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {
            globalSensorPtr->readSensor();
            xSemaphoreGive(i2cMutex);
            Serial.println("[TaskRead] Sensor updated.");
        }
        vTaskDelay(pdMS_TO_TICKS(5000)); // Every 5 seconds
    }
}

// --- TASK: DISPLAYS DATA ON SERIAL ---
void TaskSerialDisplay(void *pvParameters) {
    for (;;) {
        if (xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {
            uint16_t co2 = globalSensorPtr->getCO2();
            Serial.print("[TaskSerial] Current CO2: ");
            Serial.println(co2);
            xSemaphoreGive(i2cMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(10000)); // Every 10 seconds
    }
}