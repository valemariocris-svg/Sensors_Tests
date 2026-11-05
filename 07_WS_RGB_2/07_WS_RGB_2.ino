/* ============================================================================================= */
/*                   for WS2812        On  ESP32S3 Zero                                          */
/*   Demonstrates usage of onboard RGB LED on some ESP dev boards.                               */
/*   Calling digitalWrite(WS_RGB, HIGH) will use hidden RGB driver.                              */
/*   RGBLedWrite demonstrates control of each channel:                                           */
/*                                                                                               */
/*   void rgbLedWrite(uint8_t pin, uint8_t red_val, uint8_t green_val, uint8_t blue_val)         */
/*                                                                                               */
/*   WARNING: After using digitalWrite to drive RGB LED it will be impossible                    */
/*   to drive the same pin with normal HIGH/LOW level                                            */
/*   a better test for output print and pulse RGB  is contained  in  "Echo_Test.ino"             */
/* ============================================================================================= */
#include "RGB_Led.h"

const uint32_t SERIAL_SPEED  = 115200;


/* ********************************************************************************************* */
void setup() {
  // WS_RGB = 21    // Hardware connected on Zero
  Clear_RGB();
  Serial.begin(SERIAL_SPEED);
  delay(500);
  pinMode(Switch1, INPUT_PULLUP);
  pinMode(Switch2, INPUT_PULLUP);
}

/* ********************************************************************************************* */
void loop() {
  RGB_Leds_Blink('R');
  RGB_Leds_Blink('G');
  RGB_Leds_Blink('B');

  // RGB_Leds_Blink('C');
  // RGB_Leds_Blink('M');
  // RGB_Leds_Blink('Y');
  RGB_Leds_Blink('W');
}
/* ============================================================================================= */
