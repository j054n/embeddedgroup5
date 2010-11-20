#include "my_ad.h"
#include "messages.h"
#include <adc.h>

void initADC()
{
	OpenADC(ADC_FOSC_8 & ADC_RIGHT_JUST & ADC_0_TAD,
		ADC_CH0 & ADC_CH1 &
		ADC_INT_OFF & ADC_VREFPLUS_VDD & 
		ADC_VREFMINUS_VSS, 0b1011);
	// Use SetChanADC(ADC_CH1) to look at sensor channel
	SetChanADC(ADC_CH1);
	Delay10TCYx( 50 );
}
/*void initADC()
{
	
	OpenADC(ADC_FOSC_8 & ADC_RIGHT_JUST & ADC_0_TAD,
		ADC_CH0 & ADC_CH1 &
		ADC_INT_OFF & ADC_REF_VDD_VSS, ADC_TRIG_CCP2, 0b11111000);
	// Use SetChanADC(ADC_CH1) to look at sensor channel
	SetChanADC(ADC_CH0);
	Delay10TCYx( 50 );
	
}*/
void readADC() //function reads from the ADC and returns a value 0x0 to 0x03FF
{
	unsigned char buffer[2];
	unsigned char mtype=0x55;
	int value;
	ConvertADC(); // Start conversion
	while( BusyADC() ); // Wait for ADC conversion
	value = ReadADC(); // Read result and put in temp
	Delay1KTCYx(1);  // wait a bit...

	buffer[0]=(value>>8)&0xff;
	buffer[1]=value&0xff;

	ADQueue_sendmsg(2,&mtype,(void *)buffer);
}