#include "my_lcd.h"
#include "my_spi.h"
#include "menus.h"
#include <stdio.h>
#include <string.h>
#include <spi.h>
#include "messages.h"
//#include "debug_strings.h"
unsigned char lastOpt=0, menuDepth=0, selected=0,chosenName=0, chosenState=0;
static struct menu *head, *options,*current;
//struct menu acct1, acct2, acct3;
char acctL[4][16]={"Ben","Daniel Logan","Ty Davis","Riley Newell"};
char optL[4][16]={"Reading light","Sleeping light","TV light","MAX light"};
//static accounts *storeList;

void displayInitScreen(void)
{
	
	char init_msg[]="Initializing\0";
	current=head;
	// initSPIHeader();
	LCD_INVERT=0;
	drawString(init_msg,1);
//	CloseSPI1();
}
void displayAccounts(unsigned char highlightLine)
{
	
	char init_msg[16];
//struct	menu *test=head.next[0];
	int menuOptR=5, i=0,opts=0;//test->menuOpt;
	//initSPIHeader();
//	LCD_INVERT=1;
	setPos(0,0);
	setStartLine(0);
	//storeList=acctList;
	opts=head->numOptions;
	for(i=0; i<opts&&i<4; i++)
	{
	init_msg[0]=0;
	menuOptR=head->next[i]->menuOpt;

	strcpy(init_msg,acctL[menuOptR-1]);
//	init_msg[1];
	drawString(init_msg,i+1);
	}
	if(highlightLine>0 && highlightLine <=4)
	{
		LCD_INVERT=1;
		init_msg[0]=0;
		strcpy(init_msg,acctL[head->next[highlightLine-1]->menuOpt-1]);
		drawString(init_msg,highlightLine);	
		LCD_INVERT=0;	
	}
//	CloseSPI1();
}
void displayOptions(unsigned char highlightLine)
{
	char init_msg[16];
//struct	menu *test=head.next[0];
	int menuOptR=5, i=0,opts=0;//test->menuOpt;
	//initSPIHeader();
//	LCD_INVERT=1;
	setPos(0,0);
	setStartLine(0);
	//storeList=acctList;
	opts=options->numOptions;
	for(i=0; i<opts&&i<4; i++)
	{
	init_msg[0]=0;
	menuOptR=options->next[i]->menuOpt;

	strcpy(init_msg,optL[menuOptR-1]);
//	init_msg[1];
	drawString(init_msg,i+1);
	}
	if(highlightLine>0 && highlightLine <=4)
	{
		LCD_INVERT=1;
		init_msg[0]=0;
		strcpy(init_msg,optL[options->next[highlightLine-1]->menuOpt-1]);
		drawString(optL[options->next[highlightLine-1]->menuOpt-1],highlightLine);	
		LCD_INVERT=0;	
	}
}
void displayDebug(unsigned char user, unsigned char opt)
{
	char printUser[]="User  ", printOpt[]="Opt   ", blank[]=" ";	
	setPos(0,0);
	setStartLine(0);
	printUser[5]=(char)user+0x40;
	printOpt[5]=(char)opt+0x40;
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
	unsigned char buffer[2];
	printSPIHeader(GPIOA,RESET_OFF|0x3); //choose both Chip Selects
	printSPIHeader(GPIOB,0b00111111); //turn display ON
	glcdStrobe(0x3);
	
	if(menuOpt>0&&lastOpt==menuOpt)
	{
		if(current==head)
		{
			displayAccounts(0);
			current=current->next[menuOpt-1];
		}
		else if(current==options)
		{
			displayOptions(0);
			current=current->next[menuOpt-1];
		}
		else
		{
			if(current->parent==head)
			{
				displayAccounts(menuOpt);
			//	if(current->numOptions==1)
			//	{
			//		current=current->next[0];
			//	}
				chosenName=menuOpt;
				lastOpt=0;
			}else
			if(current->parent==options)
			{
				displayOptions(menuOpt);
				lastOpt=0;
				buffer[0]=chosenName;
				buffer[1]=menuOpt;
				ToMainHigh_sendmsg(2,MSGT_USER_CHOICE,(void *) buffer);
			//	current=head;
			}else{
				displayAccounts(0);
			}
			if(current->numOptions>=1)
				{
					current=current->next[0];
				}
		//	current=current->next[0];
		}
	}
	else 
		{
			/*if(current==head->next[0] || current==head->next[1] 
			|| current==head->next[2] || current==head->next[3]||current==head)
			{
				displayAccounts(0);
			}else
			if(current==options->next[0] || current==options->next[1] 
			|| current==options->next[2] || current==options->next[3]||current==options)
			{
				displayOptions(0);
			}*/
			
			if(current==head||current->parent==head)
			{
				displayAccounts(0);
			}else
			if(current==options||current->parent==options)
			{
				displayOptions(0);
			}else
			{
				current=head;
			}
			lastOpt=menuOpt;
		}
}

  void buildDebugGraph(struct menu *acct1, struct menu *acct2,struct menu *acct3,struct menu *acct4,
					   struct menu *opt1, struct menu *opt2,struct menu *opt3,struct menu *opt4)
{
    char  test[]="test";
	char test2[16];
//	int i;
	head->numOptions=0;
	options->numOptions=0;
	acct1->menuOpt=1;
	acct1->numOptions=0;
	acct1->parent=head;
	acct2->menuOpt=2;
	acct2->numOptions=0;
	acct2->parent=head;
	acct3->menuOpt=3;
	acct3->numOptions=0;
	acct3->parent=head;
	acct4->menuOpt=4;
	acct4->numOptions=0;
	acct4->parent=head;
	opt1->menuOpt=1;
	opt1->numOptions=0;
	opt1->parent=options;
	opt2->menuOpt=2;
	opt2->numOptions=0;
	opt2->parent=options;
	opt3->menuOpt=3;
	opt3->numOptions=0;
	opt3->parent=options;
	opt4->menuOpt=4;
	opt4->numOptions=0;
	opt4->parent=options;
	
//	i = acct1.menuOpt;


	add(opt1, head);
	add(opt2, head);
	add(opt3, head);
	add(opt4, head);
	add(options, opt1);
	add(options, opt2);
	add(options, opt3);
	add(options, opt4);
	add(acct1,options);
	add(acct2,options);
	add(acct3,options);
	add(acct4,options);
	add(head, acct1);
	add(head, acct2);
	add(head, acct3);
	add(head, acct4);

}


void add(struct menu *headL, struct menu *toAdd)
{
	headL->numOptions++;
	headL->next[headL->numOptions-1]=toAdd;
}
void setHeadOpts(struct menu *headP, struct menu *optionsP)
{
	head=headP;
	options=optionsP;
}