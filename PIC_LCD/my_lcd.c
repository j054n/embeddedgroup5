#include "my_lcd.h"
#include "my_spi.h"
#include <spi.h>
#include <delays.h>
void initSPIHeader(void)
{
	RESET=0;
	Delay10TCYx(50);
	RESET=1;

	initSPI();
	LCDCS=0;
	WriteSPI1(CMD);
	WriteSPI1(IOCON);
	WriteSPI1(0x28);
	LCDCS=1;
/*		LCDCS=0;
	WriteSPI1(OCMD);
	WriteSPI1(GPPUB);
	WriteSPI1(0xff);
	LCDCS=1;
	WriteSPI1(OCMD);
	WriteSPI1(GPPUA);
	WriteSPI1(0xff);
	LCDCS=1;
	LCDCS=0;
	WriteSPI1(OCMD);
	WriteSPI1(OLATB);
	WriteSPI1(0xff);
	LCDCS=1;
	LCDCS=0;
	WriteSPI1(OCMD);
	WriteSPI1(OLATA);
	WriteSPI1(0xff);
	LCDCS=1;
	LCDCS=0;*/

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

/*	LCDCS=0;
	WriteSPI1(OCMD);
	WriteSPI1(GPIOA);
	WriteSPI1(0xff);
	LCDCS=1;
	LCDCS=0;
	WriteSPI1(OCMD);
	WriteSPI1(GPIOB);
	WriteSPI1(0xff);
	LCDCS=1;*/

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
	
	printSPIHeader(GPIOA,RESET_OFF| cs); //write out LCD X address control with CS
	Delay10TCYx(TWO_US);
	printSPIHeader(GPIOB,xaddr|LCD_YADDR_CMD); //write out LCD x address
	Delay10TCYx(TWO_US);

	glcdStrobe(cs);
	
}
void setYAddr(unsigned char yaddr)
{
	
	printSPIHeader(GPIOA,RESET_OFF); //write out LCD X address control with CS
	Delay10TCYx(TWO_US);
	printSPIHeader(GPIOB,yaddr|LCD_XADDR_CMD); //write out LCD x address
	Delay10TCYx(TWO_US);

	glcdStrobe(0x3);
}
void glcdStrobe(unsigned char cs)
{

	printSPIHeader(GPIOA,RESET_OFF|LCD_ENABLE|cs); //high, enable
	Delay10TCYx(5 );
	printSPIHeader(GPIOA,RESET_OFF|LCD_DISABLE|cs);//low, disable
}
void glcdInit(void)
{
	int temp=0;
	initSPIHeader();
	Delay1TCY();
	printSPIHeader(GPIOA,RESET_ON|0x3); //choose both Chip Selects
	glcdStrobe(0x3);
	printSPIHeader(GPIOA,RESET_OFF|0x3); //choose both Chip Selects
	setStartLine(0x0);
	printSPIHeader(GPIOB,0b00111111); //turn display ON
	glcdStrobe(0x3);
	printSPIHeader(GPIOB,0xff); //turn display ON
//	temp=readLCD();
//	if(temp>0)
//		LATCbits.LATC0=1;

	clearScreen();
//	printSPIHeader(OLATB,0b00111111); //turn display ON
//	glcdStrobe(0x3);
//	printSPIHeader(OLATB,0b11000000);
//	glcdStrobe(CS_BOTH);
}
unsigned char setPos(unsigned char x, unsigned char y)
{
	unsigned char cs=0x0;
	
	setYAddr(y);
	if(cs>63)
	{
		cs=CS2;
		setXAddr(x-64,CS2);
	}
	else
	{
		cs=CS1;
		setXAddr(x,CS1);
	}
	return cs;
}
void clearScreen()
{
	int i=0;
	unsigned char x,y;
	for(y=0; y<8; y++)
	{
		
		for(x=0; x<128; x++)
		{
			setPos(x,y);
			Delay100TCYx(100);
			if(x<64)
				writePixelByte(0x0f,CS1);
			else
				writePixelByte(0x0f,CS2);
		}
	}	
	setPos(0,0);
}
void writePixelByte(unsigned char bp, unsigned char cs)
{
	unsigned char cs_r=cs&0b00000011;
	printSPIHeader(GPIOA,RESET_OFF|RS_ON| cs_r); // write command
	Delay10TCYx(5);
	printSPIHeader(GPIOB,bp);	//write data shit0,CS1
   // printSPIHeader(OLATA,bytePage);
//			LATCbits.LATC0=1;
	glcdStrobe(cs);
	printSPIHeader(GPIOA,RESET_OFF);
}
void setStartLine(unsigned char pos)
{
		printSPIHeader(GPIOA,RESET_OFF| 0x3); // 
		printSPIHeader(GPIOB, 0b11000000|pos);
		glcdStrobe(0x3);
}
unsigned char readLCD()
{
	unsigned char toSend=0;
	LCDCS=0;
	WriteSPI1(OCMD);
	WriteSPI1(IODIRB);
	WriteSPI1(0xff); //set all IO B pins to inputs
	LCDCS=1;
//	printSPIHeader(OLATA,RESET_OFF|RS_ON|RW_ON);  //tell LCD screen to read status
//	glcdStrobe(0x3);
	printSPIHeader(GPIOA,RESET_OFF|RS_OFF|RW_ON|CS1);  //tell LCD screen to read status
	glcdStrobe(0x1);
	Delay10TCYx(10);
 	do{
	LCDCS=0;
	WriteSPI1(OCMD|0x01); //read
	WriteSPI1(GPIOB);
	while(!DataRdySPI1());
	toSend=ReadSPI1();
	LCDCS=1;
	}while(toSend&0x80);
	
	return toSend;
}