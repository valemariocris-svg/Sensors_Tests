
#include <Arduino.h>
#include "RGB_Led.h"
/* ============================================================================================= */
/*                                                                                               */
/*   Funzioni globali  per RGB LED  ............                                                 */
/*                                                                                               /*                                                                                               */
/* ============================================================================================= */

// // Questo include rende disponibili tutte le funzioni del core Arduino
// #include <Arduino.h>


void Clear_RGB() {
  rgbLedWrite(WS_RGB, 0, 0, 0);
}

/* ********************************************************************************************* */
void RGB_Mngr(char Colour, uint8_t Brt) {     // R G B W  Cyan Magenta Yellow
  uint8_t Brt2 = Brt/8;
  uint8_t Brtw = Brt/16;

  switch(Colour) {
    case 'R':
      rgbLedWrite(WS_RGB, 0, Brt, 0);        // Red
      break;
    
    case 'G':
      rgbLedWrite(WS_RGB, Brt, 0, 0);        // Green
      break;
    
    case 'B':
      rgbLedWrite(WS_RGB, 0, 0, Brt);        // Blue
      break;
    
    case 'C':                                // Cyan
        rgbLedWrite(WS_RGB, Brt2, 0, Brt2); 
        break;

    case 'M':
    rgbLedWrite(WS_RGB, 0, Brt2, Brt2);     // Magenta
        break;

  case 'Y':
      rgbLedWrite(WS_RGB, Brt2, Brt2, 0);   // Yellow
      break;
          
    case 'W':
    default:
      rgbLedWrite(WS_RGB, Brtw, Brtw, Brtw);  // White
      break;
  }
}

/* ********************************************************************************************* */
void RGB_LED_Set(char Colour) {     // [1]=R  [0]=G [2]=B    W  Cyan Magenta Yellow

  switch(Colour) {
    case 'R':
      rgbLedWrite(WS_RGB, 0, 9, 0);       // Red
      break;
    
    case 'G':
      rgbLedWrite(WS_RGB, 5, 0, 0);       // Green
      break;
    
    case 'B':
      rgbLedWrite(WS_RGB, 0, 0, 20);      // Blue
      break;
    
    case 'C':                             // Cyan
        rgbLedWrite(WS_RGB, 8, 0, 5); 
        break;

    case 'M':
    rgbLedWrite(WS_RGB, 0, 9, 18);       // Magenta
        break;

  case 'Y':
      rgbLedWrite(WS_RGB, 5, 10, 0);       // Yellow
      break;
          
    case 'W':                             // White
    default:
      rgbLedWrite(WS_RGB, 8, 8, 8);
      break;
  }
}

/* ********************************************************************************************* */
void RGB_Leds_Blink(char Colour) {
  if(digitalRead(Switch1)==LOW) {
    RGB_LED_Set('C');
    delay(50);
  }
  else if (digitalRead(Switch2)==LOW) {
    RGB_LED_Set('M');
    delay(50);
  }
  else {
    RGB_LED_Set(Colour);
    delay(DelayHIGH);
    Clear_RGB();
    delay(DelayLOW);
  }
}
/* ============================================================================================= */
