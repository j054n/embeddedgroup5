#ifndef MENUS_H
#define MENUS_H
typedef struct accounts {
//	char acct1[16];
//	char acct2[16];
//	char acct3[16];
//	char acct4[16];
	char acctL[4][16];
} accounts;

//extern unsigned char LCD_INVERT;

void displayInitScreen(void);
void displayAccounts(accounts *acctList, unsigned char highlightLine);
void displayOptions(unsigned char highlightLine);
void selectOption(unsigned char menuOpt);

#endif