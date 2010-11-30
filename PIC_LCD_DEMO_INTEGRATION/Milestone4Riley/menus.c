#include "my_lcd.h"
#include "my_spi.h"
#include "menus.h"
#include <stdio.h>
#include <string.h>
#include <spi.h>
#include "messages.h"
#include "debug_strings.h"
unsigned char lastOpt=0, menuDepth=0, selected=0,chosenName=0, chosenState=0;
static accounts *storeList;

void displayInitScreen(void)
{
	
	char init_msg[]="Initializing\0";
	// initSPIHeader();
	LCD_INVERT=0;
	drawString(init_msg,1);
//	CloseSPI1();
}
void displayAccounts(accounts *acctList, unsigned char highlightLine)
{
	
	char init_msg[16];
	//initSPIHeader();
//	LCD_INVERT=1;
	setPos(0,0);
	setStartLine(0);
	storeList=acctList;
	init_msg[0]=0;
	strcpy(init_msg,acctList->acctL[0]);
	drawString(init_msg,1);
	init_msg[0]=0;
	strcpy(init_msg,acctList->acctL[1]);
	drawString(init_msg,2);
	init_msg[0]=0;
	strcpy(init_msg,acctList->acctL[2]);
	drawString(init_msg,3);
	init_msg[0]=0;
	strcpy(init_msg,acctList->acctL[3]);
	drawString(init_msg,4);
	if(highlightLine>0 && highlightLine <=4)
	{
		LCD_INVERT=1;
		strcpy(init_msg,acctList->acctL[highlightLine-1]);
		drawString(init_msg,highlightLine);	
		LCD_INVERT=0;	
	}
//	CloseSPI1();
}
void displayOptions(unsigned char highlightLine)
{
	setPos(0,0);
	setStartLine(0);
	drawString(optL[0],1);
	drawString(optL[1],2);
	drawString(optL[2],3);
	drawString(optL[3],4);
	if(highlightLine>0 && highlightLine <=4)
	{
		LCD_INVERT=1;
		drawString(optL[highlightLine-1],highlightLine);	
		LCD_INVERT=0;	
	}
}
void displayDebug(void)
{
	char printUser[]="User  ", printOpt[]="Opt   ", blank[]=" ";
	unsigned char buffer[3]={MSGT_USER_CHOICE,chosenName,chosenState};
	
	ToMainHigh_sendmsg(3,MSGT_USER_CHOICE,(void *) buffer);
	setPos(0,0);
	setStartLine(0);
	printUser[5]=(char)chosenName+0x40;
	printOpt[5]=(char)chosenState+0x40;
	drawString(printUser,1);
	drawString(printOpt,2);
	drawString(blank,3);
	drawString(blank,4);
}
void highlightOpt(unsigned char lineNum)
{
	
}
void selectOption(unsigned char menuOpt)
{
	char init2_msg[16];
	printSPIHeader(GPIOA,RESET_OFF|0x3); //choose both Chip Selects
	printSPIHeader(GPIOB,0b00111111); //turn display ON
	glcdStrobe(0x3);
//	initSPI();
	//clearScreen();
	if(menuOpt>0)
	{
		if(menuOpt!=lastOpt)
		{	
			lastOpt=menuOpt;
		}
		else if(lastOpt>0&&menuOpt>0)
		{
			switch(menuDepth)
			{			
				case 0:
				{
					//draw menu options
					if(selected>0)
					{
						chosenName=menuOpt;
						displayOptions(0);
						menuDepth=1;
						lastOpt=0;
						selected=0;
					}
					else
					{
						displayAccounts(storeList, menuOpt);
						selected=1;
					}
					break;
				};
				case 1:
				{
					if(selected>0)
					{
					//display choices
					chosenState=menuOpt;
					menuDepth=2;
					lastOpt=0;
					selected=0;
					}
					else
					{
						
						displayOptions(menuOpt);
						selected=1;
					}
					break;
				};
				case 2://reset
				{
					displayDebug();
					
					menuDepth=0;
					lastOpt=0;
					selected=0;
					break;
				};
				default:
				{
					displayAccounts(storeList,0);
					menuDepth=0;
					break;
				};	
			}
		}
	}
	else 
	{
		lastOpt=0;
		if(menuDepth==0)
		{
			displayAccounts(storeList,0);
		}
		else if(menuDepth==1)
		{	
			displayOptions(0);
		}else if(menuDepth==2)
		{
			displayDebug();	
		}
	
//	printSPIHeader(GPIOA,RESET_ON|0x3); //choose both Chip Selects
//	glcdStrobe(0x3|RESET_ON);
//	printSPIHeader(GPIOA,RESET_OFF|0x3); //choose both Chip Selects
//	setStartLine(0x3|RESET_OFF);
//printSPIHeader(GPIOB,0b00111111); //turn display ON
//	glcdStrobe(0x3);
//	setPos(0,0);
//	setStartLine(0);
	}

//	CloseSPI1();
}