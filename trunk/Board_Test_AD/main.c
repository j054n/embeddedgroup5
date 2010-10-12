/////////////////////////////////////////////
// Program to test the A/Ds on the 4534 demo
// board.  Channel 0 is connected to the pot.
// Channel 1 is connected to a header where a
// sensor can be hooked up.
// The LEDs display is based on the value read
// from the A/D.
//////////////////////////////////////////////
/*
#include <p18cxxx.h>
#include <portb.h>
#include <delays.h>
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

void readADC(int *value)
{
	ConvertADC(); // Start conversion
	while( BusyADC() ); // Wait for ADC conversion
	(*value) = ReadADC(); // Read result and put in temp
	Delay1KTCYx(1);  // wait a bit...
}

void stopADC()
{
	CloseADC(); // Disable A/D converter
}

void main(void)
{
	int value;

	// Set internal oscillator to 4Mhz
	OSCCON = 0x6C;

	// Set PORT B as digital outputs for the LEDs
	LATB = 0x00;
	TRISB = 0x00;

	// Initialize the A/D
	initADC();

	// Flash the LEDs
	LATB = 0x3F;
	Delay10KTCYx(50);
	LATB = 0x00;

	// Loop forever while reading the A/D
	while(1) {

			// read A/D
			readADC(&value);

			// Map the 10 bit value to an LED display
			if(value<0x07F)
				LATB = 0x00;
			else if (value<0x100)
				LATB = 0x01;
			else if (value<0x17F)
				LATB = 0x03;
			else if (value<0x200)
				LATB = 0x07;
			else if (value<0x27F)
				LATB = 0x0F;
			else if (value<0x300)
				LATB = 0x1F;
			else 
				LATB = 0x3F;

	}

}
*/

/////////////////////////////////////////////
//	Program to read from the sensor channel on the PIC board
//   and output values read from there on the LEDs.
// 	 Also, the program will communicate to the S3E board through 2 pins
//   RC3 and RC4, input and output. (Can only send high and low value
//   so it is not sending send serial data, rather it is just updating 1 bit
//   for the other micro processor to see)
//////////////////////////////////////////////

#include <p18cxxx.h>
//#include <portb.h>
#include <delays.h>
#include <adc.h>

void initADC(void)
{
	OpenADC(ADC_FOSC_8 & ADC_RIGHT_JUST & ADC_0_TAD,
		ADC_CH0 & ADC_CH1 &
		ADC_INT_OFF & ADC_REF_VDD_VSS, ADC_TRIG_CCP2, 0b11111000);
	// Use SetChanADC(ADC_CH1) to look at sensor channel
	SetChanADC(ADC_CH0);
	Delay10TCYx( 50 );
}

void readADC(int *value)
{
	ConvertADC(); // Start conversion
	while( BusyADC() ); // Wait for ADC conversion
	(*value) = ReadADC(); // Read result and put in temp
	Delay1KTCYx(1);  // wait a bit...
}

void stopADC()
{
	CloseADC(); // Disable A/D converter
}

void main(void)
{
	int value;
	int display, prevdisplay;

	// Set internal oscillator to 4Mhz
	OSCCON = 0x6C;
	
	LATA = 0x00;
	TRISA = 0x01;

	// Set PORT B as digital outputs for the LEDs
	LATB = 0x00;
	TRISB = 0xff;

	// Set PORT C as input and output.
	LATC = 0x00;	
	TRISC = 0xFE;  // Set PORT C to read from all except RC4
				   // Write out on RC4
//printf("1\n");

	// Initialize the A/D
	initADC();

	// Flash the LEDs
	LATC = 0x01;
	//printf("1\n");
	Delay10KTCYx(50);
	LATC = 0x00;
	//printf("1\n");
	Delay10KTCYx(50);
	//LATC = 0x01;

	// Loop forever while reading the A/D and communicating
	while(1) {
		display = 0;
		
		// Set the middle red LED (0x10 on Port B) to output what is read off of
		// RC3 pin on the PIC. 
		//display+=((PORTC&0x08)<<1);

		//Read A/D
		readADC(&value);

		if(value<0x200) LATC = 0x00;
		else LATC = 0x01;	

		//LATC = 0x01;
			
		// Update the lower 4 LEDs (Port B's lower 4 bits (0xF)) with a value read
		// from the sensor.
		/*if(value<0x08F)
			display =(display|0x0);
		else if (value<0x100)
			display =(display|0x01);
		else if (value<0x180)
			display =(display|0x03);
		else if (value<0x200)
			display =(display|0x07);
		else if (value<0x280)
			display =(display|0x0F);
		LATB=display;*/

		/*if(value<0x1ff)
			display = 0x0;
		else if(value<0x230)
			display = prevdisplay;
		else
			display = 0x0F;
		LATB=display;
		prevdisplay=display;*/

		// Output on the RC4 pin whatever value is also giong to the 4th LED.
			// This information will be read by the S3E Board.
		//PORTC=((display&0x8)<<1); 
	}

}