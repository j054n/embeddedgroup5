#ifndef __maindefs
#define __maindefs

#include <p18f2680.h>

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
#define MSGT_SPI_RECV 50
#define MSGT_SPI_SEND 51
#define MSGT_SPI_REQ  52
#define MSGT_TOUCH_DETECT 60
#define MSGT_AD_READ 70
#define MSGT_LIGHT1_VAL 71
#define MSGT_LIGHT2_VAL 72
#define MSGT_CONTACT_VAL 73
#define MSGT_MIC_VAL 74


#endif

