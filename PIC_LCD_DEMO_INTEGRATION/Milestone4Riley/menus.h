#ifndef MENUS_H
#define MENUS_H
//struct menu;
struct menu {

	int menuOpt;
	unsigned char numOptions;
	struct menu *parent;
    struct menu *next[4];
};

//extern unsigned char LCD_INVERT;

extern char acctL[4][16];
void displayInitScreen(void);
void displayAccounts(unsigned char highlightLine);
void displayOptions(unsigned char highlightLine);
void displayDebug(unsigned char user, unsigned char opt);
void selectOption(unsigned char menuOpt);
void add(struct menu *headL, struct menu *toAdd);
void buildDebugGraph(struct menu *acct1, struct menu *acct2,struct menu *acct3,struct menu *acct4,struct menu *opt1, struct menu *opt2,struct menu *opt3,struct menu *opt4);
void setHeadOpts(struct menu *head, struct menu *options);
#endif