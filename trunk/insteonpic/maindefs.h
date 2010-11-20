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

#define LAMP_ADD1 0x12
#define LAMP_ADD2 0x0A
#define LAMP_ADD3 0xC6
#define INSTLINKGRP 0xFF // Insteon group used for linking
#define INSTDEVCAT 0x01 // Insteon device category used by PIC
#define INSTSUBCAT 0x01 // Insteon device subcategory
#define INSTEONLEN 8

#endif

