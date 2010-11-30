//************************************************************************
//Filename: HardwareProfile.c
//Author: Microchip/modified by William Newell
//Date: 2 November 2010
//Purpose: Milestone 4 code that defines which PIC will be used,  this was
//changed for the PIC18LF27J13 which was NOT one of the original supported
//devices.
//*************************************************************************
#include "SystemProfile.h"
#include "Compiler.h"
#include "Console.h"
//#include "WirelessProtocols\LCDBlocking.h"
#include "TimeDelay.h"
#include "HardwareProfile.h"


//***************************************************************************

//my custom line for the 27J13, this PIC does NOT have EERPOM so all of the
//values must be changed in software and written in programming space for our
//PIC

#if defined(__18F27J13)
 #pragma config OSC = INTOSC, WDTEN = OFF, XINST = ON, WDTPS = 2048, PLLDIV = 2
//****************************************************************************
#endif


#define DEBOUNCE_TIME 0x00003FFF

BOOL PUSH_BUTTON_pressed;
TICK PUSH_BUTTON_press_time;

/*********************************************************************
 * Function:        void BoardInit( void )
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    Board is initialized for P2P usage
 *
 * Overview:        This function configures the board 
 *
 * Note:            This routine needs to be called before the function 
 *                  to initialize P2P stack or any other function that
 *                  operates on the stack
 ********************************************************************/

void BoardInit(void)
{

//************************************************************************************************
    #if defined(PIC18F27J13)
        WDTCONbits.SWDTEN = 0; //disable WDT
        
        // Switches S2 and S3 are on RB5 and RB4 respectively. We want interrupt-on-change
        INTCON = 0x00;
        


		
        // There is no external pull-up resistors on S2 and S3. We will use internal pull-ups.
        // The MRF24J40 is using INT0 for interrupts
        // Enable PORTB internal pullups
        INTCON2 = 0x00;
        INTCON3 = 0x00;
    

		// Set internal oscillator to 4Mhz
		OSCCON = 0x6C;
		
		TRISB = 0b00000011;
		TRISA = 0b00100000;
		TRISC = 0b00010000;
//		LATA = 0x00;
//		TRISA =  0x01;
        // Make PORTB as input - this is the RESET default
		//changed
  //      TRISB = 0xdf;
    
        // Set PORTC control signal direction and initial states
        // disable chip select
		
   //     LATC = 0xf8;
    
        // Set the SPI module for use by Stack
    //    TRISC = 0x50;
    
        // Set the SPI module
        #if defined(HARDWARE_SPI)
            SSPSTAT = 0xC0;
            SSPCON1 = 0x20;
        #endif
        
        // D1 and D2 are on RA0 and RA1 respectively, and CS of TC77 is on RA2.
        // Make PORTA as digital I/O.
        // The TC77 temp sensor CS is on RA2.
        ADCON1 = 0x0F;
    
        
        PHY_CS = 1;             //deselect the MRF24J40
        PHY_CS_TRIS = 0;        //make chip select an output   
        
        RFIF = 0;               //clear the interrupt flag
    
        RCONbits.IPEN = 1;
        
        INTCON2bits.INTEDG0 = 0;
        
        INTCONbits.GIEH = 1;
//************************************************************************************************
    #endif
}


/*********************************************************************
 * Function:        void LCDTRXCount(BYTE txCount, BYTE rxCount)
 *
 * PreCondition:    LCD has been initialized
 *
 * Input:           txCount - the total number of transmitted messages
 *                  rxCount - the total number of received messages
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function display the total numbers of TX and
 *                  RX messages on the LCD, if applicable.
 *
 * Note:            This routine is only effective if Explorer16 or
 *                  PIC18 Explorer demo boards are used
 ********************************************************************/
void LCDTRXCount(BYTE txCount, BYTE rxCount)
{
    #if defined(EXPLORER16) || defined(PIC18_EXPLORER) || (defined(EIGHT_BIT_WIRELESS_BOARD) && defined(SENSOR_PORT_LCD))
        LCDErase();
        #if defined(PIC18_EXPLORER) || defined(EIGHT_BIT_WIRELESS_BOARD)
            sprintf((char *)LCDText, (far rom char*)"TX Messages: %3d", txCount);
            sprintf((char *)&(LCDText[16]), (far rom char*)"RX Messages: %3d", rxCount);
        #else
            sprintf((char *)LCDText, (const char*)"TX Messages: %d", txCount);
            sprintf((char *)&(LCDText[16]), (const char*)"RX Messages: %d", rxCount);
        #endif
        LCDUpdate();    
    #endif
}



/*********************************************************************
 * Function:        void LCDDisplay(char *text, BYTE value, BOOL delay)
 *
 * PreCondition:    LCD has been initialized
 *
 * Input:           text - text message to be displayed on LCD
 *                  value - the text message allows up to one byte 
 *                          of variable in the message
 *                  delay - whether need to display the message for
 *                          2 second without change
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function display the text message on the LCD, 
 *                  including up to one BYTE variable, if applicable.
 *
 * Note:            This routine is only effective if Explorer16 or
 *                  PIC18 Explorer demo boards are used
 ********************************************************************/
void LCDDisplay(char *text, BYTE value, BOOL delay)
{
    #if defined(EXPLORER16) || defined(PIC18_EXPLORER) || (defined(EIGHT_BIT_WIRELESS_BOARD) && defined(SENSOR_PORT_LCD))
        LCDErase();

        #if defined(PIC18_EXPLORER) || defined(EIGHT_BIT_WIRELESS_BOARD)
            sprintf((char *)LCDText, (far rom char*)text, value); 
        #elif defined(EXPLORER16)
            sprintf((char *)LCDText, (const char*)text, value); 
        #endif
        LCDUpdate();
        
        // display the message for 2 seconds
        if( delay )
        {
            BYTE i;
            for(i = 0; i < 8; i++)
            {
                DelayMs(250);
            }
        }
    #endif
}


/*********************************************************************
 * Function:        BYTE ButtonPressed(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          Byte to indicate which button has been pressed. 
 *                  Return 0 if no button pressed.
 *
 * Side Effects:    
 *
 * Overview:        This function check if any button has been pressed
 *
 * Note:            
 ********************************************************************/

#if defined(EIGHT_BIT_WIRELESS_BOARD) && defined(SENSOR_PORT_UART) && defined(SENSOR_PORT_LCD)
    void I2CTask(void);
#endif    

#if defined(__18CXX)
    void UserInterruptHandler(void)
    {
        #if defined(EIGHT_BIT_WIRELESS_BOARD) && defined(SENSOR_PORT_UART) && defined(SENSOR_PORT_LCD)
            if( PIR3bits.SSP2IF )
            {
                PIR3bits.SSP2IF = 0;
                I2CTask();
                DelayMs(5);
            }   
        #endif 
    }              
#endif
