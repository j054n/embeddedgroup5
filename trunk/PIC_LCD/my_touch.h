#ifndef MY_TOUCH_H
#define MY_TOUCH_H
#include "maindefs.h"
#define Y_PLUS LATAbits.LATA5
#define Y_MINUS LATBbits.LATB1
#define X_PLUS LATBbits.LATB2
#define X_MINUS LATBbits.LATB3
#define TOUCH_WIDTH 0xf0
unsigned char getYLoc(void);
unsigned char isTouched(void);
#endif