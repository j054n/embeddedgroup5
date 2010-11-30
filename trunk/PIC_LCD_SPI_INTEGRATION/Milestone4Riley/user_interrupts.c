// This is where the "user" interrupts handlers should go
// The *must* be declared in "user_interrupts.h"

#include "maindefs.h"
#include <timers.h>
#include "user_interrupts.h"
#include "messages.h"
#include "my_touch.h"
#include <adc.h>
#include <delays.h>
#include "my_lcd.h"
unsigned char LOC_FLAG=0, TIMER0_COUNT=0;
void initADC()
{
}
void readADC(int *value) //function reads from the ADC and returns a value 0x0 to 0x03FF
{
	ConvertADC(); // Start conversion
	while( BusyADC() ); // Wait for ADC conversion
	(*value) = ReadADC(); // Read result and put in temp
	Delay1KTCYx(1);  // wait a bit...
}
void readADC_CH(int *value, unsigned char ADC_CH) //function reads from the ADC and returns a value 0x0 to 0x03FF
{
	OpenADC(ADC_FOSC_16 & ADC_RIGHT_JUST & ADC_0_TAD,
		ADC_CH &  
		ADC_INT_OFF & ADC_REF_VDD_VSS, ADC_TRIG_CCP2,0xff);
	SetChanADC(ADC_CH);
	ConvertADC(); // Start conversion
	while( BusyADC() ); // Wait for ADC conversion
	(*value) = ReadADC(); // Read result and put in temp
	Delay1KTCYx(1);  // wait a bit...

}
void readADC2(int *value)
{
	readADC_CH(value,ADC_CH10);
}
// A function called by the interrupt handler
// This one does the action I wanted for this program on a timer0 interrupt
void timer0_int_handler()
{
	unsigned int val;
	unsigned char buffer[2];
	int	length, msgtype;
	if(LOC_FLAG>0&&LCD_WRITING==0){
		switch(getYLoc()){
		case 1:
				ToMainHigh_sendmsg(2,MSGT_LCD_AREA1,(void *) buffer);
		break;
		case 2:
				ToMainHigh_sendmsg(2,MSGT_LCD_AREA2,(void *) buffer);
		break;
		case 3:
				ToMainHigh_sendmsg(2,MSGT_LCD_AREA3,(void *) buffer);
		break;
		case 4:
				ToMainHigh_sendmsg(2,MSGT_LCD_AREA4,(void *) buffer);
		break;
		default:
				ToMainHigh_sendmsg(2,MSGT_LCD_NOTOUCH,(void *) buffer);
		break;
		}
		
	}	else if(LCD_WRITING==0&&TIMER0_COUNT>=3) //refresh every 3rd trigger
	{
		ToMainHigh_sendmsg(2,MSGT_LCD_NOTOUCH,(void *) buffer);
		TIMER0_COUNT=0;
	}
	TIMER0_COUNT++;
 	LOC_FLAG=isTouched();
	readADC_CH(&val, ADC_CH0);
	buffer[0]=val;
	readADC_CH(&val, ADC_CH1);
	buffer[1]=val;
	ToMainHigh_sendmsg(2,MSGT_PHOTO_READ,(void *) buffer);
	// toggle an LED
    //LATBbits.LATB0 = !LATBbits.LATB0; 
	// reset the timer
	WriteTimer0(0);
	// try to receive a message and, if we get one, echo it back
	length = FromMainHigh_recvmsg(sizeof(val),&msgtype,(void *)&val);
	if (length == sizeof(val)) {
		ToMainHigh_sendmsg(sizeof(val),MSGT_TIMER0,(void *) &val);
	} 
}

// A function called by the interrupt handler
// This one does the action I wanted for this program on a timer1 interrupt
void timer1_int_handler()
{
	unsigned int result,val;
	//updates the ADC values

	// read the timer and then send an empty message to main()
	result = ReadTimer1();
	ToMainLow_sendmsg(0,MSGT_TIMER1,(void *) 0);
	
	// reset the timer
	WriteTimer1(0);
}