/* Compile options:  -ml (Large code model) */
#include "maindefs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <usart.h>
#include <timers.h>
#include "user_interrupts.h"
#include "interrupts.h"
#include <adc.h>
#include <delays.h>
#include "messages.h"
#include "my_uart.h"
#include "uart_thread.h"
#include "my_spi.h"
#include "my_lcd.h"
#include "my_touch.h"
#include "debug_strings.h"
#include "menus.h"
#include "timer0_thread.h"
#include "timer1_thread.h"

//#pragma config WDT = OFF

#define MIWICS LATBbits.LATB5




void main (void)
{

unsigned int counter=0,curval=0;
char c;
//char toWrite[]={'a','b','c','\0'}, toWrite2[]="OVERW", toWrite3[]="APPLE", toWrite4[]="ADDA";

signed char	length, adlength;
unsigned char	msgtype;
unsigned char last_reg_recvd, action;
unsigned char adbuffer[2];
//accounts acctList;
//unsigned char nameList[4][16]={"MARK JONES", "APPLE", "PAUL PLASSMAN"};
//uart_comm uc;
//i2c_comm ic;
accounts *acctList;
unsigned char msgbuffer[MSGLEN+1];
unsigned char i;
//uart_thread_struct	uthread_data; // info for uart_lthread
timer1_thread_struct t1thread_data; // info for timer1_lthread
timer0_thread_struct t0thread_data; // info for timer0_lthread
unsigned char data;
TRISB = 0b00000011;
TRISA = 0b00100000;
TRISC=0b00010000;
MIWICS=1;
//acctList.acct1=(char*)malloc(16);
//acctList.acct2=(char*)malloc(16);
//acctList.acct3=(char*)malloc(16);
//acctList.acct4=(char*)malloc(16);
strcpy(acctList->acctL[0],acctL[0]);
strcpy(acctList->acctL[1],acctL[1]);
strcpy(acctList->acctL[2],acctL[2]);
strcpy(acctList->acctL[3],acctL[3]);




//acctList.acct4="asdfasdfasdf";
glcdInit();
displayInitScreen();
displayAccounts(acctList,0);
//list=&acctList;
//acctListSize=4;
	OSCCON = 0x6C; // 4 MHz
	OSCTUNEbits.PLLEN = 1; // 4x the clock speed in the previous line
	
	// initialize my uart recv handling code
//	init_uart_recv(&uc);

	// initialize the i2c code
//	init_i2c(&ic);

	// init the timer1 lthread
	//init_timer1_lthread(&t1thread_data);

	// initialize message queues before enabling any interrupts
	init_queues();

	// set direction for PORTB to output
//	TRISB = 0x0;
//	LATB = 0x0;


	// set up PORTA for input
/*
	PORTA = 0x0;	// clear the port
	LATA = 0x0;		// clear the output latch
	ADCON1 = 0x0F;	// turn off the A2D function on these pins
	// Only for 40-pin version of this chip CMCON = 0x07;	// turn the comparator off
	TRISA = 0x0F;	// set RA3-RA0 to inputs
*/
//displayInitScreen();
	// initialize Timers
	OpenTimer0( TIMER_INT_ON & T0_16BIT & T0_SOURCE_INT & T0_PS_1_4);
//	OpenTimer1( TIMER_INT_ON & T1_PS_1_8 & T1_16BIT_RW  & T1_OSC1EN_OFF & T1_SYNC_EXT_OFF,0);
	
	// Peripheral interrupts can have their priority set to high or low
	// enable high-priority interrupts and low-priority interrupts
	enable_interrupts();

	// Decide on the priority of the enabled peripheral interrupts
	// 0 is low, 1 is high
	// Timer1 interrupt
//	IPR1bits.TMR1IP = 0;
	// USART RX interrupt
//	IPR1bits.RCIP = 0;
	// I2C interrupt
//	IPR1bits.SSPIP = 1;

	// configure the hardware i2c device as a slave
//	i2c_configure_slave(0x8A);

	// must specifically enable the I2C interrupts
//	PIE1bits.SSPIE = 1;

	// configure the hardware USART device
  	/*OpenUSART( USART_TX_INT_OFF & USART_RX_INT_ON & USART_ASYNCH_MODE & USART_EIGHT_BIT   & 
		USART_CONT_RX & USART_BRGH_LOW, 0x19);*/
while(1)
{
//displayInitScreen();
	block_on_To_msgqueues();

		// At this point, one or both of the queues has a message.  It 
		// makes sense to check the high-priority messages first -- in fact,
		// you may only want to check the low-priority messages when there
		// is not a high priority message.  That is a design decision and
		// I haven't done it here.
		length = ToMainHigh_recvmsg(MSGLEN,&msgtype,(void *) msgbuffer);
		
		if (length < 0) {
			// no message, check the error code to see if it is concern
			if (length != MSGQUEUE_EMPTY) {
				printf("Error: Bad high priority receive, code = %x\r\n",
					length);
			}
		} else {
			switch (msgtype) {
				case MSGT_TIMER0: {
					timer0_lthread(&t0thread_data,msgtype,length,msgbuffer);
					break;
				};
				case MSGT_I2C_DATA:
				case MSGT_I2C_DBG: {
					printf("I2C Interrupt received %x: ",msgtype);
					for (i=0;i<length;i++) {
						printf(" %x",msgbuffer[i]);
					}
					//LATBbits.LATB0 = !LATBbits.LATB0;
				//	LATB = msgbuffer[2];
					//LATB = msgtype;
					//LATB=0x01;
					//printf("\r\n");
					// keep track of the first byte received for later use
					//last_reg_recvd = msgbuffer[0];
					//action=msgbuffer[7];
					//msgbuffer[0]=0xff;
					//msgbuffer[1]=0xff;
					//msgbuffer[2]=0xff;
					//msgbuffer[3]=0xff;
					//start_i2c_slave_reply(4,msgbuffer);
					break;
				};
				case MSGT_I2C_RQST: {
					printf("I2C Slave Req\r\n");
					length=2;
				/*	if(counter==0)
					{
						msgbuffer[0]=0x55;
						msgbuffer[1]=0x55;
						counter++;
					}else
					{
						msgbuffer[0]=0x22;
						msgbuffer[1]=0x22;
						counter=0;
					}*/
				/*	adlength = 	ADQueue_recvmsg(2,0x55,(void *)&adbuffer);
					if((adlength==MSGQUEUE_EMPTY) || (adlength==MSGBUFFER_TOOSMALL))
					{
						msgbuffer[0]=0xff;
						msgbuffer[1]=0xff;
						//msgbuffer[2]=0xff;
					}
					else
					{					
						msgbuffer[0]=adbuffer[0];
						msgbuffer[1]=adbuffer[1];
					//	msgbuffer[0]=0x22;
					//	msgbuffer[1]=0x22;
					}*/
				//	LATB=msgbuffer[0];
								//printf("XXX: type: %x ADC: %x MsgB1: %x MsgB2: %x\r\n",msgtype,value,msgbuffer[0],msgbuffer[1]);
								//break;
					//		}
					//}
//					start_i2c_slave_reply(length,msgbuffer);
					break;
				};
				case MSGT_LCD_AREA1:{
				
//displayInitScreen();
				selectOption(1);
				break;
				};				
				case MSGT_LCD_AREA2:{
//displayInitScreen();
				selectOption(2);
				break;				
				};
				case MSGT_LCD_AREA3:{
//displayInitScreen();
				selectOption(3);
				break;
				};

				case MSGT_LCD_AREA4:{
//displayInitScreen();
				selectOption(4);
			//	initSPIHeader();
				break;
				};
				case MSGT_LCD_TOUCH:{
				//do nothing atm
				
				break;
				};
				case MSGT_LCD_NOTOUCH:{	
//displayInitScreen();
				selectOption(0);
				break;
				};
				
				default: {
					printf("Error: Unexpected msg in queue, type = %x\r\n",
						msgtype);
					break;
				};
			};
		}

		// Check the low priority queue
		length = ToMainLow_recvmsg(MSGLEN,&msgtype,(void *) msgbuffer);
		if (length < 0) {
			// no message, check the error code to see if it is concern
			if (length != MSGQUEUE_EMPTY) {
				printf("Error: Bad low priority receive, code = %x\r\n",
					length);
			}
		} else {
			switch (msgtype) {
				case MSGT_TIMER1: {
					timer1_lthread(&t1thread_data,msgtype,length,msgbuffer);
					break;
				};
				case MSGT_OVERRUN:
			//	case MSGT_UART_DATA: {
			//		uart_lthread(&uthread_data,msgtype,length,msgbuffer);
			//		break;
			//	};
				default: {
			//		printf("Error: Unexpected msg in queue, type = %x\r\n",
			//			msgtype);
					break;
				};
			};
		}
/*curval=counter%4;
counter++;
if(curval==0)
{

	writePixelByte(0xf5,CS1);
}
else
{
	writePixelByte(0x0,CS2);
}*/

/*if(curval==0)
{
	data=0b01010101;
	printSPIHeader(OLATA, data);
}
else if(curval==1)
{
	data=0b10101010;
	printSPIHeader(OLATA, data);
}
else if(curval==2)
{
	data=0b01010101;
	printSPIHeader(OLATB, data);
}
else
{
	data=0b10101010;
	printSPIHeader(OLATB, data);
	counter=0;
}

X_PLUS=1;//touchscreen voltage
X_MINUS=1;
Y_PLUS=0;
//Y_MINUS=1;
//LATB=LATB|0xf;
readADC2(&adcVal); //detect a touch

if(adcVal>0x300) //touch threshold
	{

		X_PLUS=1;//touchscreen voltage
		X_MINUS=0;
		readADC2(&adcVal); //read a touch Y location
		if(adcVal <0x0f0)
		{
			LATCbits.LATC0=1;
			LATCbits.LATC1=0;
			LATCbits.LATC2=0;
		}
		else if(adcVal<0x1f0)
		{
			LATCbits.LATC0=0;
			LATCbits.LATC1=1;
			LATCbits.LATC2=0;
		}
		else if(adcVal<0x2f0)
		{
			LATCbits.LATC0=1;
			LATCbits.LATC1=1;
			LATCbits.LATC2=0;
		}
		else 
		{
			LATCbits.LATC0=0;
			LATCbits.LATC1=0;
			LATCbits.LATC2=1;
		}
	}else
	{
		LATCbits.LATC0=0;
		LATCbits.LATC1=0;
		LATCbits.LATC2=0;
	}*/

}

}