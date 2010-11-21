#ifndef __maindefs
#define __maindefs

#include <p18lf27j13.h>

// Message type definitions
#define MSGT_TIMER0 10
#define MSGT_TIMER1 11
#define MSGT_MAIN1 20
#define	MSGT_OVERRUN 30
#define MSGT_UART_DATA 31
#define MSGT_I2C_DBG 41
#define	MSGT_I2C_DATA 40
#define MSGT_I2C_RQST 42
#define MSGT_I2C_MASTER_SEND_COMPLETE 43
#define MSGT_I2C_MASTER_SEND_FAILED 44
#define MSGT_I2C_MASTER_RECV_COMPLETE 45
#define MSGT_I2C_MASTER_RECV_FAILED 46
#define MSGT_LCD_TOUCH 50
#define MSGT_LCD_AREA1 51
#define MSGT_LCD_AREA2 52
#define MSGT_LCD_AREA3 53
#define MSGT_LCD_AREA4 54
#define MSGT_LCD_NOTOUCH 55
#define DEBUG_LED1 			LATCbits.LATC0
#define DEBUG_LED2			LATCbits.LATC1
#define DEBUG_LED3			LATCbits.LATC2
#define MSGT_PHOTO_READ_1 60
#define MSGT_PHOTO_READ_2 61
#define MSGT_USER_CHOICE 70
#define MSGT_MIWI_DATA 80
#endif

