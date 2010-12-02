#ifndef __user_interrupts
#define __user_interrupts

// interrupts defined by the "user" and that are called from
// interrupts.c -- the "user" needs to insert these into
// interrupts.c because it, of course, doesn't know which
// interrupt handlers you would like to call

//int adcVVV=0;
//My example program uses these two timer interrupts
extern unsigned char LOC_FLAG, AD_DATA_FLAG;
void timer0_int_handler(void);

void timer1_int_handler(void);
void readADC(int *value);
void readADC2(int *value);
void readADC_CH(int *value, unsigned char channel);
void initADC(void);
void ad_int_handler(void);
// include the handler from my uart code
#include "my_uart.h"

#endif
