/* Compile options:  -ml (Large code model) */
#include "maindefs.h"
#include <stdio.h>
#include <usart.h>
#include <i2c.h>
#include <timers.h>
#include "user_interrupts.h"
#include "interrupts.h"
#include <adc.h>
#include <delays.h>
#include "messages.h"
#include "my_uart.h"
#include "my_i2c.h"
#include "uart_thread.h"
#include "my_spi.h"
#include "my_lcd.h"
#include "touch_defs.h"

#include "timer0_thread.h"

//#pragma config WDT = OFF

#define MIWICS LATBbits.LATB4




void main (void)
{

unsigned int counter=0,curval=0, adcVal=0;
unsigned char data;
TRISB = 0b00000011;
TRISA = 0x0;
TRISC=0b00010000;
//LATCbits.LATC0=1;
//LATCbits.LATC1=1;
//LATCbits.LATC2=1;
//while(1){}

//initSPIHeader();
glcdInit();
//while(1)
//printf("s");
while(1){}
/*while(1)
{


curval=counter%4;
counter++;
if(curval==0)
{

	writePixelByte(0xf5,CS1);
}
else
{
	writePixelByte(0x0,CS2);
}

/*if(curval==0)
{
	data=0b01010101;
	printSPIHeader(OLATA, data);
}
else if(curval==1)
{
	data=0b10101010;
	printSPIHeader(OLATA, data);
}
else if(curval==2)
{
	data=0b01010101;
	printSPIHeader(OLATB, data);
}
else
{
	data=0b10101010;
	printSPIHeader(OLATB, data);
	counter=0;
}

X_PLUS=1;//touchscreen voltage
X_MINUS=1;
Y_PLUS=0;
//Y_MINUS=1;
//LATB=LATB|0xf;
readADC2(&adcVal); //detect a touch

if(adcVal>0x300) //touch threshold
	{

		X_PLUS=1;//touchscreen voltage
		X_MINUS=0;
		readADC2(&adcVal); //read a touch Y location
		if(adcVal <0x0f0)
		{
			LATCbits.LATC0=1;
			LATCbits.LATC1=0;
			LATCbits.LATC2=0;
		}
		else if(adcVal<0x1f0)
		{
			LATCbits.LATC0=0;
			LATCbits.LATC1=1;
			LATCbits.LATC2=0;
		}
		else if(adcVal<0x2f0)
		{
			LATCbits.LATC0=1;
			LATCbits.LATC1=1;
			LATCbits.LATC2=0;
		}
		else 
		{
			LATCbits.LATC0=0;
			LATCbits.LATC1=0;
			LATCbits.LATC2=1;
		}
	}else
	{
		LATCbits.LATC0=0;
		LATCbits.LATC1=0;
		LATCbits.LATC2=0;
	}

}*/

}