/* =================================================================================== */
#include <Arduino.h>
// #include "driver/rtc_io.h" // Libreria fondamentale per gestire i pin in Deep Sleep

#define MICRO_A_SEC 1000000ULL    // unsigned long long  64 bit   1.000.000
#define WAKEUP_PIN GPIO_NUM_5 
#define LedPIN     13
#define Switch_In  5

// Variabile salvata nella memoria RTC
RTC_DATA_ATTR int totalCycles = 0;    // n.u.
boolean           Stay_Active = false;

/* ----------------------------------------------------------------------------------- */
void print_wakeup_reason() {          // n.u.
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  switch(wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("SVEGLIA: Premuto pulsante su D5!"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("SVEGLIA: Timer scaduto."); break;
    default : Serial.println("SVEGLIA: Reset/Power-on."); break;
  }
}

/* ----------------------------------------------------------------------------------- */
boolean Check_WakeUp_FromSwitch() {
  if( esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0) return true;
  return false;
}

/* ----------------------------------------------------------------------------------- */
boolean Check_Switch_Closed() {
  if(digitalRead(Switch_In) == LOW) {
    delay(20);
    if(digitalRead(Switch_In) == LOW) return true;
    else                              return false;
  }
  else return false;
}

/* ----------------------------------------------------------------------------------- */
boolean Check_Status() {
  if(!Check_Switch_Closed()) return false;
  uint16_t Count = 0;
  while(Check_Switch_Closed()) { 
    delay(10); if(++Count>500) 
    return false;          // Switch always on (> 5sec)
  }
  Stay_Active = !Stay_Active;
  return true;            // Change status requested          
}

/* ----------------------------------------------------------------------------------- */
void setup() {
  pinMode(LedPIN,    OUTPUT);
  pinMode(Switch_In, INPUT_PULLUP);
  if(Check_Switch_Closed()) {
    while(Check_Switch_Closed());
    Stay_Active = true;  
  }
}

/* ----------------------------------------------------------------------------------- */
/* the "Stay_Active" at the first reset is false, so                                   */
/* The sequence is Active-->Ligth Sleep-->Deep Sleep                                   */
/* pulsing the Switch_In during Active if !Active --> Stay only active                 */
/* pulsing the Switch_In during Active if Active  --> Start immediatly Ligth, Deep ..  */
/* pulsing the Switch_In at wakeup from ligth goto to Active Only                      */
/* ----------------------------------------------------------------------------------- */
void loop() {
  digitalWrite(LedPIN, HIGH);     // 2sec Active
  for(int i=0; i<20; i++) {
    if(Check_Status()) break;     // change status immediatly
    delay(100);
  }
  digitalWrite(LedPIN, LOW);

  if(Stay_Active) {
    delay(500);
  }
  else {
    // ---   LIGTH SLEEP   ---
    esp_sleep_enable_timer_wakeup(3 * MICRO_A_SEC);
    esp_light_sleep_start();
    // --- risveglio   ---
    Check_Status();               // to go immediatly on Active
  }
  if(!Stay_Active) {
    digitalWrite(LedPIN, HIGH);
    delay(20);
    digitalWrite(LedPIN, LOW);
    // ---   DEEP SLEEP   ---
    esp_sleep_enable_timer_wakeup(3 * MICRO_A_SEC);
    esp_deep_sleep_start();
    // al risveglio fa reset
  }  
}
/* =================================================================================== */