// Inside DataCollector.cpp
void DataCollector::collect() {
    // No Mutex needed here if reading simple variables, 
    // though a Mutex is safer for multi-byte types.
    uint16_t currentCO2 = globalSensorPtr->getCO2();
    
    Serial.print("Data Collector retrieved: ");
    Serial.println(currentCO2);
    
    // Save to Flash or SD here...
}