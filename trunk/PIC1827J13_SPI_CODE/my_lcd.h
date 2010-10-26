#ifndef LCD_DEFS_H
#define LCD_DEFS_H

#define LCD_XADDR_CMD  0b10111000
#define LCD_XADDR_CONT 0b00110000
#define LCD_XADDR_MASK 0b00001111
#define LCD_E_MASK 0b11101111
#define LCD_ENDMSG 0x30
#define LCD_DATA_SEND 0b00101000
#define CS1 0x2
#define CS2 0x1
#define TWO_US 24
#define CMD 0x42
#define OCMD 0x40
#define GPIOA 0x12
#define GPIOB 0x13
#define IOCON 0x0A
#define IODIRA 0x00
#define IODIRB 0x10
#define OLATB 0x1A
#define OLATA 0x0A
#define LCDCS LATBbits.LATB5

void glcdStrobe(void);
void glcdInit(void);
void initSPIHeader(void);
void setYAddr(unsigned char yaddr);
void setXAddr(unsigned char xaddr, unsigned char cs);
void printSPIHeader(unsigned char lat, unsigned char data);
void writePixelByte(unsigned char bytePage, unsigned char cs);
#endif