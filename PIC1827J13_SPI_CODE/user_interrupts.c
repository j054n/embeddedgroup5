// This is where the "user" interrupts handlers should go
// The *must* be declared in "user_interrupts.h"

#include "maindefs.h"
#include <timers.h>
#include "user_interrupts.h"
#include "messages.h"

#include <adc.h>

void initADC()
{
/*	OpenADC(ADC_FOSC_8 & ADC_RIGHT_JUST & ADC_0_TAD,
		ADC_CH0 & ADC_CH1 & ADC_CH12 & ADC_CH9  & ADC_CH10 &
		ADC_INT_OFF & ADC_REF_VDD_VSS, ADC_TRIG_CCP2,0b1111010001111100);
	// Use SetChanADC(ADC_CH1) to look at sensor channel
	SetChanADC(ADC_CH0);*/
//	Delay10TCYx( 50 );
}
void readADC(int *value) //function reads from the ADC and returns a value 0x0 to 0x03FF
{
	ConvertADC(); // Start conversion
	while( BusyADC() ); // Wait for ADC conversion
	(*value) = ReadADC(); // Read result and put in temp
	Delay1KTCYx(1);  // wait a bit...
}
void readADC2(int *value) //function reads from the ADC and returns a value 0x0 to 0x03FF
{
	OpenADC(ADC_FOSC_16 & ADC_RIGHT_JUST & ADC_0_TAD,
		ADC_CH10 &  ADC_CH9&
		ADC_INT_OFF & ADC_REF_VDD_VSS, ADC_TRIG_CCP2,0b1111010001111100);
	SetChanADC(ADC_CH10);
	ConvertADC(); // Start conversion
	while( BusyADC() ); // Wait for ADC conversion
	(*value) = ReadADC(); // Read result and put in temp
	Delay1KTCYx(1);  // wait a bit...

}
// A function called by the interrupt handler
// This one does the action I wanted for this program on a timer0 interrupt
void timer0_int_handler()
{
	unsigned int val;
	int	length, msgtype;

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
	unsigned int result;
	//updates the ADC values
//	readADC(&adcVal);
	// read the timer and then send an empty message to main()
	result = ReadTimer1();
	ToMainLow_sendmsg(0,MSGT_TIMER1,(void *) 0);
	
	// reset the timer
	WriteTimer1(0);
}