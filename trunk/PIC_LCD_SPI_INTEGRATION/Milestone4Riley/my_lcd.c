#include "my_lcd.h"
#include "my_spi.h"
#include "my_fonts.h"
#include <spi.h>
#include <delays.h>
#include "HardwareProfile.h"
unsigned char LCD_INVERT=0, LCD_WRITING=0;
void initSPIHeader(void)
{
	RESET=0;
	Delay10TCYx(5);
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
	Delay10TCYx(5);
	
	LCDCS=0;
	WriteSPI1(OCMD);
	WriteSPI1(lat);
	WriteSPI1(data); //CS#1 select (video memory), E On (enable)

	LCDCS=1;
//	Delay1KTCYx(5);
}
void glcdInit(void)
{
	int temp=0;
    //char toWrite[]="HELLO WORLD", toWrite2[]="OVERWRITE", toWrite3[]="APPLE SUCKS", toWrite4[]="ADDADFASDFS";
	initSPIHeader();
	Delay1TCY();
	printSPIHeader(GPIOA,RESET_ON|0x3); //choose both Chip Selects
	glcdStrobe(0x3|RESET_ON);
	printSPIHeader(GPIOA,RESET_OFF|0x3); //choose both Chip Selects
	setStartLine(0x3|RESET_OFF);
	printSPIHeader(GPIOB,0b00111111); //turn display ON
	glcdStrobe(0x3);
	//printSPIHeader(GPIOB,0xff); //Set data high, ready to wait for status byte
	setStartLine(0);
	clearScreen();
	
}
void setXAddr(unsigned char xaddr, unsigned char cs)
{
	unsigned char cs_r=cs&0b00000011;
	
	printSPIHeader(GPIOA,RESET_OFF| cs_r); //write out LCD X address control with CS
//	Delay10TCYx(TWO_US);
	printSPIHeader(GPIOB,xaddr|LCD_YADDR_CMD); //write out LCD x address
//	Delay10TCYx(TWO_US);

	glcdStrobe(cs_r);
	
}
void setYAddr(unsigned char yaddr)
{
	
	printSPIHeader(GPIOA,RESET_OFF|0x3); //write out LCD X address control with CS
//	Delay10TCYx(TWO_US);
	printSPIHeader(GPIOB,yaddr|LCD_XADDR_CMD); //write out LCD x address
//	Delay10TCYx(TWO_US);

	glcdStrobe(0x3);
}
void glcdStrobe(unsigned char cs)
{
	//Delay10TCYx(2 );
	printSPIHeader(GPIOA,RESET_OFF|LCD_ENABLE|cs); //high, enable
	Delay10TCYx(2 );
	printSPIHeader(GPIOA,RESET_OFF|LCD_DISABLE|cs);//low, disable
}

unsigned char spird()
{
	SSPBUF=0x1;
	while (!SSPSTATbits.BF);
	return SSPBUF;
}
unsigned char setPos(unsigned char x, unsigned char y)
{
	unsigned char cs=0x0;
	
	setYAddr(y);
	if(x<64)
	{
		cs=CS1;
		setXAddr(x,CS1);
		setXAddr(0,CS2);
	}
	else
	{
		cs=CS2;
		setXAddr(0,CS1);
		setXAddr(x-64,CS2);
	}
	return cs;
}

// Writes all 0's to the LCD to clear out old DRAM data
void clearScreen()
{
	int i=0;
	unsigned char x,y;
	for(y=0; y<8; y++)
	{
		setPos(0,y);
		for(x=0; x<128; x++)
		{
			if(x<64)
				writePixelByte(0x00,CS1);
			else
				writePixelByte(0x00,CS2);
		}
	}	
	setPos(0,0);
}
//writes a byte out to the LCD screen at whatever current location
void writePixelByte(unsigned char bp, unsigned char cs)
{
	unsigned char cs_r=cs&0b00000011, tempStat;
//	tempStat=readLCD(); //check LCD status byte
		
	printSPIHeader(GPIOA,RESET_OFF|RS_ON| cs_r); // write command
	//Delay10TCYx(5);
	if(LCD_INVERT>0)
	{
		printSPIHeader(GPIOB,~bp);	//write data shit0,CS1
	}else
	{
		printSPIHeader(GPIOB,bp);
	}
   // printSPIHeader(OLATA,bytePage);
//			LATCbits.LATC0=1;
	glcdStrobe(cs_r|RESET_OFF|RS_ON);
//	printSPIHeader(GPIOA,RESET_OFF);
}
void setStartLine(unsigned char pos)
{
		printSPIHeader(GPIOA,RESET_OFF| 0x3); 
		printSPIHeader(GPIOB, 0b11000000|pos); //lcd start line command+data
		glcdStrobe(0x3);
}
unsigned char readLCD()
{
	unsigned char toSend=0;
	printSPIHeader(IODIRB,0xff);
//	printSPIHeader(OLATA,RESET_OFF|RS_ON|RW_ON);  //tell LCD screen to read status
//	glcdStrobe(0x3);
	printSPIHeader(GPIOA,RESET_OFF|RS_OFF|RW_ON);  //tell LCD screen to read status
	glcdStrobe(0x3|RESET_OFF|RS_OFF|RW_ON); //dummy strobe
	//glcdStrobe(0x3|RESET_OFF|RS_OFF|RW_ON); // real read
	//Delay10TCYx(10);

	LCDCS=0;
	WriteSPI1(OCMD|0x01); //read
	WriteSPI1(GPIOB);

	while(!DataRdySPI1());
	toSend=ReadSPI1();
	LCDCS=1;

	printSPIHeader(IODIRB,0x0);	
	return toSend;
}

void writePixel(unsigned char x, unsigned char y, unsigned char state)
{
	unsigned char ypage=y/8, ypos=y%8, dataWrite=(state&0x1)<<ypos;
	
	writePixelByte(dataWrite,setPos(x,y)); 
}
void drawSquare(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2)
{

	int i;
	for(i=0; i<(x2-x1); i++) //x direction lines
	{
		writePixel(x1,y1+i,1);
	//	writePixel(x1+i,y2,1);	
	}
/*	for(i=0; i<(y2-y1); i++) //y direction lines
	{
		writePixel(x1,y1+i,1);
		writePixel(x2,y2+i,1);	
	}*/
}





//line ranges from 1-4
void drawString(char *str, unsigned char line)
{

	unsigned char i=0, cs=CS1, valid=1, tempRFIE=RFIE;
	RFIE=0;
	LCD_WRITING=1;
	if(line>0 && line <=4) 
		setPos(0,(line-1)*2);
	i=0;//breakpoint
	for (i=0; i<16; i++)
	{	
		if(i<8)
		{
			cs=CS1;
		}
		else
		{
			cs=CS2;
		}
		if((str[i]<'A'|| (str[i]>'Z' && str[i]<'a')|| str[i]>'z') && str[i]!=' ')
		{
			valid=0;
		}
		if(valid>0)
		{
			drawChar(str[i], LARGE_FONT ,cs);
		}
		else
		{
			drawChar(' ',LARGE_FONT,cs);//clear out the remainder of the line	
		}
	}
	LCD_WRITING=0;
	RFIE=tempRFIE;
}
void drawChar(char toDraw, unsigned char size, unsigned char cs)
{
	unsigned char i=0, charIndex=0, temps;
//	unsigned char **font;
	
	if((toDraw>='A' && toDraw <='Z'))
	{
//	temps=readLCD();
	//	if(size==LARGE_FONT)
	//	{
	//		font=FONT6x8_capLetter;
	//		width=6;
	//	}else 
	//	{
	//		font=FONT5x6_capLetter;
	//		width=4;
	//	}
		charIndex=toDraw-65;
		//write space
		writePixelByte(0x0,cs);
		for( i=0; i<FONT_WIDTH;i++)
		{		
			writePixelByte(FONT6x8_capLetter[charIndex][i],cs);
		}
		writePixelByte(0x0,cs); //write space
	}
	else if(toDraw >= 'a' && toDraw <='z')
	{
		charIndex=toDraw-97;
		//write space
		writePixelByte(0x0,cs);
		for( i=0; i<FONT_WIDTH;i++)
		{		
			writePixelByte(FONT6x8_letter[charIndex][i],cs);
		}
		writePixelByte(0x0,cs); //write space
	}	
	else if(toDraw=' ')
	{
		for(i=0; i<FONT_WIDTH+2; i++)
		{
			writePixelByte(0x0,cs);	
		}
	}	
}
