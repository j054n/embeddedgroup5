/* Compile options:  -ml (Large code model) */
//*********************************************************************
//Filename: main.c
//Author: William Newell
//Date: 11 November 2010
//Purpose: To demonstrate the receiving of packets over MiWi using message
//queues in the framework
//*********************************************************************
#include "maindefs.h"
#include <stdio.h>
#include <usart.h>
#include <i2c.h>
#include <timers.h>
#include "user_interrupts.h"
#include "interrupts.h"
#include <adc.h>
#include <delays.h>
#include "messages.h"
#include "my_uart.h"
#include "my_i2c.h"
#include "uart_thread.h"
#include "my_spi.h"
#include "my_lcd.h"
#include "my_touch.h"
#include "debug_strings.h"
#include "menus.h"
#include "timer0_thread.h"

#include "WirelessProtocols/P2P/P2P.h"


#include "ConfigApp.h"
#include "HardwareProfile.h"
#include "..\Microchip\Include\WirelessProtocols\MCHP_API.h"


#define LIGHT   0x01
#define SWITCH  0x02
#define MIWICS LATBbits.LATB5

#if ADDITIONAL_NODE_ID_SIZE > 0
    BYTE AdditionalNodeID[ADDITIONAL_NODE_ID_SIZE] = {LIGHT};
#endif

BYTE myChannel = 20;
  
   

#if defined(__18CXX)
void main(void)
#else
int main(void)
#endif
{   
	int j;
    BYTE i;
    BYTE TxSynCount = 0;
    BYTE TxSynCount2 = 0;
    BYTE TxNum = 0;
    BYTE RxNum = 0;
	BYTE foo = 0;
	int value;
	int display, prevdisplay;
	signed char	length, mwlength;
	unsigned char msgtype;
	unsigned char last_reg_recvd, action;
	unsigned char mwbuffer[1];
unsigned int counter=0,curval=0;

//char toWrite[]={'a','b','c','\0'}, toWrite2[]="OVERW", toWrite3[]="APPLE", toWrite4[]="ADDA";

//signed char	length, adlength;
//unsigned char	msgtype;
//unsigned char last_reg_recvd, action;
//unsigned char adbuffer[2];
//accounts acctList;
//unsigned char nameList[4][16]={"MARK JONES", "APPLE", "PAUL PLASSMAN"};
//uart_comm uc;
//i2c_comm ic;
accounts *acctList;
unsigned char msgbuffer[MSGLEN+1];
//unsigned char i;
//uart_thread_struct	uthread_data; // info for uart_lthread
//timer1_thread_struct t1thread_data; // info for timer1_lthread
timer0_thread_struct t0thread_data; // info for timer0_lthread
unsigned char data;
TRISB = 0b00000011;
TRISA = 0b00100000;
TRISC=0b00010000;
MIWICS=1;

strcpy(acctList->acctL[0],acctL[0]);
strcpy(acctList->acctL[1],acctL[1]);
strcpy(acctList->acctL[2],acctL[2]);
strcpy(acctList->acctL[3],acctL[3]);

	glcdInit();
	displayInitScreen();
	displayAccounts(acctList,0);
    
    /*******************************************************************/
    // Initialize the system
    /*******************************************************************/
    BoardInit();      
   // initADC();
	init_queues();
    LED_1 = 0;
    LED_2 = 0;

	OSCCON = 0x6C; // 4 MHz
	OSCTUNEbits.PLLEN = 1; // 4x the clock speed in the previous line
    /*******************************************************************/
    // Initialize Microchip proprietary protocol. Which protocol to use
    // depends on the configuration in ConfigApp.h
    /*******************************************************************/
    /*******************************************************************/
    // Function MiApp_ProtocolInit initialize the protocol stack. The
    // only input parameter indicates if previous network configuration
    // should be restored. In this simple example, we assume that the 
    // network starts from scratch.
    /*******************************************************************/
    MiApp_ProtocolInit(FALSE);

    // Set default channel
    MiApp_SetChannel(myChannel);

    /*******************************************************************/
    // Function MiApp_ConnectionMode defines the connection mode. The
    // possible connection modes are:
    //  ENABLE_ALL_CONN:    Enable all kinds of connection
    //  ENABLE_PREV_CONN:   Only allow connection already exists in 
    //                      connection table
    //  ENABL_ACTIVE_SCAN_RSP:  Allow response to Active scan
    //  DISABLE_ALL_CONN:   Disable all connections. 
    /*******************************************************************/
    MiApp_ConnectionMode(ENABLE_ALL_CONN);

    /*******************************************************************/
    // Function MiApp_EstablishConnection try to establish a new 
    // connection with peer device. 
    // The first parameter is the index to the active scan result, 
    //      which is acquired by discovery process (active scan). If 
    //      the value of the index is 0xFF, try to establish a 
    //      connection with any peer.
    // The second parameter is the mode to establish connection, 
    //      either direct or indirect. Direct mode means connection 
    //      within the radio range; indirect mode means connection 
    //      may or may not in the radio range. 
    /*******************************************************************/
    #ifdef ENABLE_HAND_SHAKE
        i = MiApp_EstablishConnection(0xFF, CONN_MODE_DIRECT);
    #endif
    

    MiApp_StartConnection(START_CONN_DIRECT, 10, 0);


    /*******************************************************************/
    // Function DumpConnection is used to print out the content of the
    //  Connection Entry on the hyperterminal. It may be useful in 
    //  the debugging phase.
    // The only parameter of this function is the index of the  
    //  Connection Entry. The value of 0xFF means to print out all
    //  valid Connection Entry; otherwise, the Connection Entry
    //  of the input index will be printed out.
    /*******************************************************************/
	init_queues();
	OpenTimer0( TIMER_INT_ON & T0_16BIT & T0_SOURCE_INT & T0_PS_1_4);
//	OpenTimer1( TIMER_INT_ON & T1_PS_1_8 & T1_16BIT_RW  & T1_OSC1EN_OFF & T1_SYNC_EXT_OFF,0);
	
	// Peripheral interrupts can have their priority set to high or low
	// enable high-priority interrupts and low-priority interrupts
	enable_interrupts();
	while(1)
    {

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
				//refresh screen
				selectOption(0);
				break;
				};
				case MSGT_PHOTO_READ_1:{
				//send photoresistor val 1
				break;
				};
				case MSGT_PHOTO_READ_2:{
				//send photoresistor val 2
				break;
				};
				case MSGT_USER_CHOICE:{	
				break;
				};
				case MSGT_MIWI_DATA:{
				  /*******************************************************************/
        // Function MiApp_MessageAvailable returns a boolean to indicate if 
        // a packet has been received by the transceiver. If a packet has 
        // been received, all information will be stored in the rxFrame, 
        // structure of RECEIVED_MESSAGE.
        /*******************************************************************/
    	readADC(&value);
		P2PTasks();

		if(value<0x200) LED_3 = 1;
		else LED_3 = 0;	
   	        
			//test to see what the payload of the packet is, if it's a P, that is a poll command, if it's 
			//and L, this is an LCD information command.
		
			//receive message from miwi message queue if available	
		   mwlength = MWQueue_recvmsg(1,&msgtype,(void *)&mwbuffer);

		 	 if((mwlength==MSGQUEUE_EMPTY) || (length==MSGBUFFER_TOOSMALL))
	   	 	 {
		 	 }
		 	 else
		 	 {					
			
				if (mwbuffer[0]=='P') {
			
				 MiApp_FlushTx();
				 for(j = 0; j < 8; j++) {
					MiApp_WriteData((BYTE) j*2);
					}
               	  TxSynCount2++;
              	   MiApp_BroadcastPacket(FALSE);
					LED_1 ^= 1;
				}
				else if (mwbuffer[0]=='L') {
				
					LED_2 ^= 1;
				}
					MiApp_DiscardMessage();
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
//					timer1_lthread(&t1thread_data,msgtype,length,msgbuffer);
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
	}
}