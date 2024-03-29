#include "my_touch.h"
#include "user_interrupts.h"
#include <adc.h>
unsigned char getYLoc(void)
{
	unsigned char toSend=0;
	int adcVal=0;
	X_PLUS=1;//touchscreen voltage
	X_MINUS=0;
	readADC_CH(&adcVal,ADC_CH10); //read a touch Y location
/*	if(adcVal>0xf)
		return (adcVal/0xff)+1;
	else
		return 0;*/

if(adcVal<0xf)
	return 0;
else if(adcVal<LOW_AREA)
	return 1;
else if(adcVal<LOW_MID_AREA)
	return 2;
else if(adcVal<HIGH_MID_AREA)
	return 3;
else
	return 4;
		/*if(adcVal <0x0f0)
		{
			return 1;
		}
		else if(adcVal<0x1f0)
		{
			return 2;
		}
		else if(adcVal<0x2f0)
		{
			return 3;
		}
		else 
		{
			return 4;
		}
	
			
		return toSend;*/
}

unsigned char isTouched(void)
{
	int adcVal=0;
	X_PLUS=1;//touchscreen voltage
	X_MINUS=1;
	Y_PLUS=0;
	//Y_MINUS=1;
	//LATB=LATB|0xf;
	readADC_CH(&adcVal, ADC_CH10 ); //detect a touch

	if(adcVal>0x300) //touch threshold
		return 1;
	else
		return 0;
}