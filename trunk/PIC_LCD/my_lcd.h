#ifndef MY_LCD_H
#define MY_LCD_H

#define LCD_XADDR_CMD  0b10111000
#define LCD_XADDR_CONT 0b00110000
#define LCD_XADDR_MASK 0b00000111
#define LCD_YADDR_CMD  0b01000000

#define LCD_YADDR_MASK 0b00111111
#define LCD_E_MASK	0b11101111
#define LCD_ENABLE	0b00010000
#define LCD_DISABLE	0x0
#define RESET_OFF	0b00100000
#define RESET_ON	0x0
#define RS_ON		0b00000100
#define RS_OFF		0x0
#define RW_ON		0b00001000
#define RW_OFF		0x0

#define LCD_ENDMSG 0x30
#define LCD_DATA_SEND 0b00101000
#define CS1 0x2
#define CS2 0x1
#define CS_OFF 0x0
#define CS_BOTH 0x3
#define TWO_US 24
#define CMD 0x42
#define OCMD 0x40
#define GPIOA 0x12
#define GPIOB 0x13
#define IOCON 0x0A
#define IODIRA 0x00
#define IODIRB 0x01
#define OLATB 0x1A
#define OLATA 0x0A
#define READ 0b00001100
#define STAT_READ 0b00000100
#define LCDCS LATBbits.LATB4
#define STARTLINE 0b11000000
#define GPPUB 0x16
#define GPPUA 0x06
#define LARGE_FONT 0x1
#define SMALL_FONT 0x2

void glcdStrobe(unsigned char cs);
void glcdInit(void);
void initSPIHeader(void);
void clearScreen(void);
void setYAddr(unsigned char yaddr);
unsigned char setPos(unsigned char x, unsigned char y);
void setStartLine(unsigned char pos);
void setXAddr(unsigned char xaddr, unsigned char cs);
void printSPIHeader(unsigned char lat, unsigned char data);
void writePixelByte(unsigned char bytePage, unsigned char cs);
void writePixel(unsigned char x, unsigned char y, unsigned char state);
void drawSquare(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2);
void drawChar(char toDraw, unsigned char size,unsigned char cs);
void drawString(char str[], unsigned char size, unsigned char line);
unsigned char readLCD(void);
unsigned char spird(void);

#endif