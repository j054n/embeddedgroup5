#ifndef MY_TOUCH_H
#define MY_TOUCH_H
#include "maindefs.h"
#define Y_PLUS LATAbits.LATA5
#define Y_MINUS LATBbits.LATB1
#define X_PLUS LATBbits.LATB2
#define X_MINUS LATBbits.LATB3
#define LOW_AREA 0x12f
#define LOW_MID_AREA 0x210
#define HIGH_MID_AREA 0x2d0
#define HIGH_AREA 0x3ff
unsigned char getYLoc(void);
unsigned char isTouched(void);
#endif