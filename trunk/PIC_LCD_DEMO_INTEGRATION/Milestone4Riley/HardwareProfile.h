/********************************************************************
* FileName:		HardwareProfile.h
* Dependencies:    
* Processor:	PIC18, PIC24, PIC32, dsPIC30, dsPIC33
*               tested with 18F4620, dsPIC33FJ256GP710	
* Complier:     Microchip C18 v3.04 or higher
*				Microchip C30 v2.03 or higher	
*               Microchip C32 v1.02 or higher	
* Company:		Microchip Technology, Inc.
*
* Copyright and Disclaimer Notice
*
* Copyright © 2007-2010 Microchip Technology Inc.  All rights reserved.
*
* Microchip licenses to you the right to use, modify, copy and distribute 
* Software only when embedded on a Microchip microcontroller or digital 
* signal controller and used with a Microchip radio frequency transceiver, 
* which are integrated into your product or third party product (pursuant 
* to the terms in the accompanying license agreement).   
*
* You should refer to the license agreement accompanying this Software for 
* additional information regarding your rights and obligations.
*
* SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY 
* KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY 
* WARRANTY OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A 
* PARTICULAR PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE 
* LIABLE OR OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, 
* CONTRIBUTION, BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY 
* DIRECT OR INDIRECT DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO 
* ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, 
* LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF SUBSTITUTE GOODS, 
* TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT 
* NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*
*********************************************************************
* File Description:
*
*  This file defines functions used for demo board hardware
*
* Change History:
*  Rev   Date         Author    Description
*  1.0   2/15/2009    yfy       Initial revision
*  2.0   4/15/2009    yfy       MiMAC and MiApp revision
********************************************************************/

#ifndef _HARDWARE_PROFILE_H
    #define _HARDWARE_PROFILE_H
    
    #include "GenericTypeDefs.h"
    #include "ConfigApp.h"
    
    /*********************************************************************/
    // Choose one of the demo board that support the demo out of the box
    /*********************************************************************/
	#define PIC18F27J13

    // there are three ways to use NVM to store data: External EPROM, Data EEPROM and 
    // programming space, with following definitions:
    //  #define USE_EXTERNAL_EEPROM
    //  #define USE_DATA_EEPROM
    #define USE_PROGRAMMING_SPACE  
    // Each demo board has defined the method of using NVM, as
    // required by Network Freezer feature.
    

        
//***********************************************************************
    #if defined(PIC18F27J13)
        #define CLOCK_FREQ          16000000
        
        // Transceiver Configuration
        #define RFIF                INTCONbits.INT0IF
        #define RFIE                INTCONbits.INT0IE
        #define PHY_CS              LATBbits.LATB5
        #define PHY_CS_TRIS         TRISBbits.TRISB5
        #define PHY_RESETn          LATCbits.LATC7
        #define PHY_RESETn_TRIS     TRISCbits.TRISC7
        
        #define PUSH_BUTTON_1       PORTBbits.RB5
        #define PUSH_BUTTON_2       PORTBbits.RB4
        #define LED_1               LATCbits.LATC0
        #define LED_2               LATCbits.LATC1
		#define LED_3				LATCbits.LATC2
        
        #define BUTTON_1_TRIS       TRISBbits.TRISB4
        #define BUTTON_2_TRIS       TRISBbits.TRISB4
        #define LED_1_TRIS          TRISAbits.TRISA0
        #define LED_2_TRIS          TRISAbits.TRISA1
        
        #define RF_INT_PIN          PORTBbits.RB0
        #define RF_INT_TRIS         TRISBbits.TRISB0
        
        #define SPI_SDI             PORTCbits.RC4               
        #define SDI_TRIS            TRISCbits.TRISC4
        #define SPI_SDO             LATCbits.LATC5               
        #define SDO_TRIS            TRISCbits.TRISC5
        #define SPI_SCK             LATCbits.LATC3               
        #define SCK_TRIS            TRISCbits.TRISC3
        
        #define TMRL                TMR0L
    #endif
//**************************************************************************



    
    // Following definition is for delay functionality
    #if defined(__18CXX)
        #define GetInstructionClock()	(CLOCK_FREQ/4)
    #elif defined(__C30__) 
        #define GetInstructionClock()	(CLOCK_FREQ/2)
    #elif defined(__PIC32MX__)
        #define GetInstructionClock()	(CLOCK_FREQ)
    #endif

    BYTE ButtonPressed(void);
    void BoardInit(void);

    void LCDDisplay(char *, BYTE, BOOL);
    void LCDTRXCount(BYTE, BYTE);

    
#endif
