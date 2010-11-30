#include "my_spi.h"

void initSPI(void)
{
   
	TRISC=0b00010000;
	RESET=0;
	RESET=1;
	
	OpenSPI1(SPI_FOSC_16, MODE_00, SMPMID);	
}
