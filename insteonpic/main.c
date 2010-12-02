/* Compile options:  -ml (Large code model) */
#include "maindefs.h"
#include <stdio.h>
#include <usart.h>
#include <i2c.h>
#include <timers.h>
#include <adc.h>
#include "user_interrupts.h"
#include "my_ad.h"
#include "interrupts.h"
#include "messages.h"
#include "my_uart.h"
#include "my_i2c.h"
#include "uart_thread.h"
#include "timer1_thread.h"
#include "timer0_thread.h"

#pragma config WDT = OFF
										
void init_insteon(unsigned char*);

// This program 
//   (1) prints to the UART and it reads from the UART
//   (2) it "prints" what it reads from the UART to portb (where LEDs are connected)
//   (3) it uses two timers to interrupt at different rates and drive 2 LEDs (on portb)
void main (void)
{
	char c;
	int adcVal,counter=0;
	signed char	length, adlength;
	unsigned char msgtype;
	unsigned char last_reg_recvd, action;
	unsigned char adbuffer[2];
	uart_comm uc;
	i2c_comm ic;
	unsigned char msgbuffer[MSGLEN+1];
	unsigned char insteonbuf[INSTEONLEN];
	unsigned char i;
	uart_thread_struct	uthread_data; // info for uart_lthread
	timer1_thread_struct t1thread_data; // info for timer1_lthread
	timer0_thread_struct t0thread_data; // info for timer0_lthread

	unsigned char ind = 0;
	
	initADC();
	//readADC();
	// set to run really, really fast...
	OSCCON = 0x6C; // 4 MHz
	OSCTUNEbits.PLLEN = 1; // 4x the clock speed in the previous line

	// initialize my uart recv handling code
	init_uart_recv(&uc);

	// initialize the i2c code
	init_i2c(&ic);

	// init the timer1 lthread
	init_timer1_lthread(&t1thread_data);

	// initialize message queues before enabling any interrupts
	init_queues();

	// set direction for PORTB to output
	TRISB = 0x0;
	LATB = 0x0;


	// set up PORTA for input
/*
	PORTA = 0x0;	// clear the port
	LATA = 0x0;		// clear the output latch
	ADCON1 = 0x0F;	// turn off the A2D function on these pins
	// Only for 40-pin version of this chip CMCON = 0x07;	// turn the comparator off
	TRISA = 0x0F;	// set RA3-RA0 to inputs
*/

	// initialize Timers
	OpenTimer0( TIMER_INT_ON & T0_16BIT & T0_SOURCE_INT & T0_PS_1_128);//32);
	OpenTimer1( TIMER_INT_ON & T1_PS_1_8 & T1_16BIT_RW & T1_SOURCE_INT & T1_OSC1EN_OFF & T1_SYNC_EXT_OFF);
	
	// Peripheral interrupts can have their priority set to high or low
	// enable high-priority interrupts and low-priority interrupts
	enable_interrupts();

	// Decide on the priority of the enabled peripheral interrupts
	// 0 is low, 1 is high
	// Timer1 interrupt
	IPR1bits.TMR1IP = 0;
	// USART RX interrupt
	IPR1bits.RCIP = 0;
	// I2C interrupt
	IPR1bits.SSPIP = 1;

	// configure the hardware i2c device as a slave
	i2c_configure_slave(0x8A);

	// must specifically enable the I2C interrupts
	PIE1bits.SSPIE = 1;

	// configure the hardware USART device
  	OpenUSART( USART_TX_INT_OFF & USART_RX_INT_ON & USART_ASYNCH_MODE & USART_EIGHT_BIT   & 
		USART_CONT_RX & USART_BRGH_LOW, 0xC);
		
	init_insteon(insteonbuf);

/* Junk to force an I2C interrupt in the simulator
PIR1bits.SSPIF = 1;
_asm
goto 0x08
_endasm;
*/
	printf("Hello\r\n");
	// loop forever
	// This loop is responsible for "handing off" messages to the subroutines
	// that should get them.  Although the subroutines are not threads, but
	// they can be equated with the tasks in your task diagram if you 
	// structure them properly.
  	while (1) {
		// Call a routine that blocks until either on the incoming
		// messages queues has a message (this may put the processor into
		// an idle mode
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
				case MSGT_I2C_DATA: {
					LATBbits.LATB1 = !LATBbits.LATB1;
					insteonbuf[7]=msgbuffer[2]; //on-value for insteon
					for(i=0;i<INSTLAMPS;i++) {
						insteonbuf[2] = (i==0) ? LAMP1_ADD1 : 
										(i==1) ? LAMP2_ADD1 : 0;
						insteonbuf[3] = (i==0) ? LAMP1_ADD2 : 
										(i==1) ? LAMP2_ADD2 : 0;
						insteonbuf[4] = (i==0) ? LAMP1_ADD3 : 
										(i==1) ? LAMP2_ADD3 : 0;
						uart_send(INSTEONLEN, insteonbuf);
					}
					break;
				};
				case MSGT_I2C_DBG: {
					printf("I2C Interrupt received %x: ",msgtype);
					for (i=0;i<length;i++) {
						printf(" %x",msgbuffer[i]);
					}
					break;
				};
				case MSGT_I2C_RQST: {
					printf("I2C Slave Req\r\n");
					length=5;
					LATBbits.LATB0 = !LATBbits.LATB0;
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
				case MSGT_OVERRUN: {
					LATBbits.LATB2 = !LATBbits.LATB2;
				};
				case MSGT_UART_DATA: {
					//uart_lthread(&uthread_data,msgtype,length,msgbuffer);
					switch (msgbuffer[1])
					{
						case 0x50: //receive standard Insteon message
						case 0x51: //extended message
							switch (msgbuffer[9]) //Cmd1 byte
							{
								case 0x11: //Insteon command: On
									LATBbits.LATB5 = 1;
									break;
								case 0x13: //Insteon command: off
									LATBbits.LATB5 = 0;
									break;
								case 0x19: //Status Request
									msgbuffer[0] = 0x02;
									msgbuffer[1] = 0x68; // Set Insteon ACK
									if(LATBbits.LATB5) // if LED is on (last command was insteon on)
									{
										msgbuffer[2] = insteonbuf[7]; // On-level
									}
									else
									{
										msgbuffer[2] = 0x00;
									}
									uart_send(3, msgbuffer);
									break;
							}
						break;
					}
					break;
				};
				default: {
					printf("Error: Unexpected msg in queue, type = %x\r\n",
						msgtype);
					break;
				};
			};
		}
 	}

}

void init_insteon(unsigned char* insteonbuf) {
	insteonbuf[0]=0x02;
	insteonbuf[1]=0x64; //begin All-Link
	insteonbuf[2]=0x01; //IM is master
	insteonbuf[3]=0xFF;
	uart_send(4,insteonbuf);

	insteonbuf[1] = 0x66; // Set Host Device
	insteonbuf[2] = INSTDEVCAT; // Device Category
	insteonbuf[3] = INSTSUBCAT; // Device Subcategory
	insteonbuf[4] = 0xFF;
	uart_send(5,insteonbuf);
	
	//setup for the rest of execution...
	insteonbuf[1]=0x62;	//insteon standard message
	/*insteonbuf[2]=LAMP_ADD1;
	insteonbuf[3]=LAMP_ADD2;
	insteonbuf[4]=LAMP_ADD3;*/
	insteonbuf[5]=0x00; //flags
	insteonbuf[6]=0x11; //turn ON
}