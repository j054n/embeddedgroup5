#include "my_lcd.h"
#include "my_spi.h"
#include <spi.h>
#include <delays.h>
void initSPIHeader(void)
{
	initSPI();
	LCDCS=0;
	WriteSPI1(OCMD);
	WriteSPI1(IOCON);
	WriteSPI1(0xA0);
	LCDCS=1;
	
	LCDCS=0;
	WriteSPI1(OCMD);
	WriteSPI1(IODIRB);
	WriteSPI1(0x00);
	LCDCS=1;

	LCDCS=0;
	WriteSPI1(OCMD);
	WriteSPI1(IODIRA);
	WriteSPI1(0x00);
	LCDCS=1;
}
void printSPIHeader(unsigned char lat, unsigned char data)
{
//	Delay1KTCYx(5);
	LCDCS=0;
	WriteSPI1(OCMD);
	WriteSPI1(lat);
	WriteSPI1(data); //CS#1 select (video memory), E On (enable)

	LCDCS=1;
//	Delay1KTCYx(5);
}

void setXAddr(unsigned char xaddr, unsigned char cs)
{
	unsigned char cs_r=cs&0b00000011;
	printSPIHeader(OLATA,LCD_XADDR_CONT|cs_r); //write out LCD X address control with CS
	Delay10TCYx(TWO_US);
	printSPIHeader(OLATB,(xaddr&LCD_XADDR_MASK)|LCD_XADDR_CMD); //write out LCD x address
	Delay10TCYx(TWO_US);
	printSPIHeader(OLATA,LCD_XADDR_CONT|cs_r|LCD_ENDMSG);
	Delay10TCYx(TWO_US);
	
}
void setYAddr(unsigned char yaddr)
{
}
void glcdStrobe(void)
{
	printSPIHeader(OLATA,0b00110000); //high, enable
	Delay10TCYx(2);
	printSPIHeader(OLATA,0b00100000);//low, disable
}
void glcdInit(void)
{
	initSPIHeader();
	Delay1TCY();
	printSPIHeader(OLATA,0b00100000);//unreset
	Delay10TCYx(8);
	printSPIHeader(OLATA,0b00100000|0x3);
	printSPIHeader(OLATB,0b00111111); //turn display ON
	glcdStrobe();
	printSPIHeader(OLATB,0b11000000);
	glcdStrobe();
}
void writePixelByte(unsigned char bp, unsigned char cs)
{
	unsigned char cs_r=cs&0b00000011;
	printSPIHeader(OLATA,0b00100100);
	printSPIHeader(OLATB,bp);	//write data shit
   // printSPIHeader(OLATA,bytePage);
//			LATCbits.LATC0=1;
	glcdStrobe();
/*	printSPIHeader(OLATA,LCD_DATA_SEND|cs_r); 
	Delay10TCYx(TWO_US);
	printSPIHeader(OLATB,bytePage);
	printSPIHeader(OLATA,LCD_DATA_SEND|cs_r|LCD_ENDMSG|0x10);
	
	Delay10TCYx(TWO_US);

	printSPIHeader(OLATA,LCD_DATA_SEND|cs_r); //write out LCD X address control with CS

	printSPIHeader(OLATA,LCD_DATA_SEND|cs_r|LCD_ENDMSG|0x10);
	Delay10TCYx(TWO_US);*/
}
