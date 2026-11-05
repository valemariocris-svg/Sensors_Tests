#include <stdint.h>
#ifndef RGB_LED_H                                                   
#define RGB_LED_H
const uint8_t Switch1 = 5;
const uint8_t Switch2 = 6;

const uint32_t  DelayHIGH = 400;
const uint32_t  DelayLOW  = 100;

extern void Clear_RGB();
extern void RGB_Mngr(char Colour, uint8_t Brt);
extern void RGB_LED_Set(char Colour);
extern void RGB_Leds_Blink(char Colour);
#endif