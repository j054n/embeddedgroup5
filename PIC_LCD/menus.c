#include "my_lcd.h"
#include "my_spi.h"
#include "menus.h"
#include <stdio.h>
#include <string.h>
#include <spi.h>
#include "debug_strings.h"
unsigned char lastOpt=0, menuDepth=0, chosenName=0, chosenState=0;
static accounts *storeList;
void displayInitScreen(void)
{
	
	char init_msg[]="Initializing\0";
	// initSPIHeader();
	LCD_INVERT=0;
	drawString(init_msg,1);
//	CloseSPI1();
}
void displayAccounts(accounts *acctList)
{
	
	char init_msg[16];
	//initSPIHeader();
//	LCD_INVERT=1;
	setPos(0,0);
	setStartLine(0);
	storeList=acctList;
	strcpy(init_msg,acctList->acctL[0]);
	drawString(init_msg,1);
	strcpy(init_msg,acctList->acctL[1]);
	drawString(init_msg,2);
	strcpy(init_msg,acctList->acctL[2]);
	drawString(init_msg,3);
	strcpy(init_msg,acctList->acctL[3]);
	drawString(init_msg,4);
//	CloseSPI1();
}
void displayOptions(void)
{
	setPos(0,0);
	setStartLine(0);
	drawString(optL[0],1);
	drawString(optL[1],2);
	drawString(optL[2],3);
	drawString(optL[3],4);
}
void displayDebug(void)
{
	char printUser[]="User  ", printOpt[]="Opt   ", blank[]=" ";
	setPos(0,0);
	setStartLine(0);
	printUser[5]=(char)chosenName+0x40;
	printOpt[5]=(char)chosenState+0x40;
	drawString(printUser,1);
	drawString(printOpt,2);
	drawString(blank,3);
	drawString(blank,4);
}
void selectOption(unsigned char menuOpt)
{
	char init2_msg[16];
//	initSPI();
	if(menuOpt>0)
	{
		if(menuOpt!=lastOpt)
		{	
			if(menuDepth==0 )
			{
				LCD_INVERT=1;
				strcpy(init2_msg,storeList->acctL[menuOpt-1]);
				drawString(init2_msg,menuOpt);
				LCD_INVERT=0;
				if(lastOpt>0)
				{
				strcpy(init2_msg,storeList->acctL[lastOpt-1]);
				drawString(init2_msg,lastOpt);
				}
			//	menuDepth=1;
			}
			else if(menuDepth==1)
			{
				//redraw menu options
				LCD_INVERT=1;
				drawString(optL[menuOpt-1],menuOpt);
				LCD_INVERT=0;
				if(lastOpt>0)
				{
				drawString(optL[lastOpt-1],lastOpt);
				}
			//	menuDepth=2;
			}
			else if(lastOpt==0)		
			{
			//	displayAccounts(storeList);
			}
				lastOpt=menuOpt;
		}
		else if(lastOpt>0&&menuOpt>0)
		{
			switch(menuDepth)
			{			
				case 0:
				{
					//draw menu options
					chosenName=menuOpt;
					displayOptions();
					menuDepth=1;
					break;
				};
				case 1:
				{
					//display choices
					chosenState=menuOpt;
					menuDepth=2;
					break;
				};
				case 2://reset
				{
					displayDebug();
					menuDepth=3;
					break;
				};
				default:
				{
				displayAccounts(storeList);
				menuDepth=0;
				break;
				};	
			}
			lastOpt=0;
		}
	}

//	CloseSPI1();
}