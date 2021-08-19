/*
    \file   main.c
    \brief  Main file of the project.
    (c) 2020 Microchip Technology Inc. and its subsidiaries.
*/

#include "encoder.h"

//attachInterrupt(encoder_Pin,interrupt, RISING); 
  volatile uint16_t A_Counter = 7433;         // note the variable is declared in the header file as extern. Its initial value is assigned here


int main(void)
{

    pins_disable();                  // because we're using the 40 pin dip and it doesn't have these pins

    PORTC.DIRCLR   = PIN2_bm;                // set pins Port C2 and Port C3 as input for the encoder hardware
    PORTC.DIRCLR   = PIN3_bm;
    PORTC.PIN2CTRL = PORT_PULLUPEN_bm;    // enable pullup R on pins c2 and c3
    PORTC.PIN3CTRL = PORT_PULLUPEN_bm;

    //ok now we have pin PC2 and PC3 set up as #defines in .h  and pull up r's if needed - check arduino version. Now need to know how
    // to read the pins looking at the GPIO from microchip, it looks like use of PIN2_bm and PIN3_bm is used in this

attachInterrupt(encoder_Pin, encoderInterrupt, RISING);



    char command[MAX_COMMAND_LEN];   // the string to hold incoming commands from all UARTs
    

    uint8_t index = 0;
    char c;
	
	/* This delay invalidates the initial noise on the TX line, after device reset. */
    _delay_ms(10);
	  // USART0_init();     // will be comms with Stepper
    // USART2_init();     // will be comms with host PC - MCU Monitor
    USART1_init();        // comms with Host PC - ASCOM driver
    LED_init();

	// todo - remove the following line
    USARTx_sendString( &ASCOMSerial, "Valid commands are AZ and STA.\r\n");
    bool x;
    while (1)
    {

      if ((ASCOMSerial.STATUS & USART_RXCIF_bm))   //if there's a char available...Valid strings of chars are: AZ# (caps) and STA followed by 1 to 6 chars like e.g. 1.00# , 342.00#

        {
    
          c = USARTx_readChar(&ASCOMSerial);       // read the char 
          if(c != '\n' && c != '\r' && c != '#')   // # marks end of command string
            {
                 
                if (validchar(c)) 
                {
                  command[index++] = c;                // stuff the validated char into the command array 
                }        
              if(index > MAX_COMMAND_LEN)
              {
                  index = 0;
              }
           }
        
          if(c == '#')                     //if it's the protocol termination char '#', add a null to end of string
          {
              command[index] = '\0';
              index = 0;
              // use strcomp function to validate either STA or AZ
              // then execute the command is either to transmit the azimuth values to the ascom driver OR sync the encoder with the azimuth value supplied
              executeCommand(command);      //instead of executeCommand this could be validate Command - only sync and Az are valid - check.
          }
        }
      
      // now ceck MONOITORSerial
      
      //and next check STEPPERSerial
    }
}