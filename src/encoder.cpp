#include "encoder.h"

void pins_disable(void)
{
    //These pins are disabled following advice in the 4809 datasheet
  PORTC.PIN7CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTC.PIN6CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTB.PIN5CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTB.PIN4CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTB.PIN3CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTB.PIN2CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTB.PIN1CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTB.PIN0CTRL = PORT_ISC_INPUT_DISABLE_gc;
  
}

void USART1_init(void)
{
    USART1.BAUD = (uint16_t)USART1_BAUD_RATE(9600);

    USART1.CTRLB |= USART_RXEN_bm | USART_TXEN_bm;
	
	PORTC.DIR |= PIN0_bm;
	PORTC.DIR &= ~PIN1_bm;
}

void USARTx_sendChar(USART_t* usart_x, const char c)
{
    while (!(usart_x->STATUS & USART_DREIF_bm))
    {
        ;    
    }
    usart_x->TXDATAL = c;
}

void USARTx_sendString(USART_t* usart_x ,  char *str)
{
    for(size_t i = 0; i < strlen(str); i++)
    {
        USARTx_sendChar(usart_x, str[i]);      
    }
}

char USARTx_readChar(USART_t* usart)
{
    while (!(usart->STATUS & USART_RXCIF_bm))
    {
        ;
    }
    return usart->RXDATAL;
}

void LED_on(void)
{
    
PORTA.OUT |= PIN7_bm;    // positive logic
}

void LED_off(void)
{
    
    PORTA.OUT &= ~PIN7_bm;    // positive logic
}


void LED_init(void)
{
    PORTA.DIR |= PIN7_bm;
	LED_off();
}

void locate_String(char* command)
{
  
  char * pch;
  pch = strstr (command,"STA");  //set the pointer to the position of the occurrence of STA (the sync to azimuth command)
  if (pch != NULL)
    strncpy (pch,"   ",3);       // copy 3 spaces into where STA used to be (this is so that the strtod function can work - only leading white space is ignored)
  
}

bool validchar(char c)
{
  for (int i=0; i<15; i++)   // check if the char is a member of the validset
    {
      if(c==validset[i])
      {
        return true;
      }
    }
}

void executeCommand(char *command)
{

// assuming 'command' is populated from any of the three active UARTs
// will need to ensure the protocol used is unique across the UARTs

    if(strcmp( "AZ", command ) == 0)     // = 0 means that AZ is equal to 
    {
        LED_on();
        USARTx_sendString(&ASCOMSerial, "Azimuth = 250.00 HAL\r\n");    // this is from ASCOMSerial, we'll send the azimuth value to the ASCOMSerial - call a fn to action this
    }
    else if (strcmp("STA", command) <= 0) // '<=' used here because of the way the function compares S1 and S2 it means STA is recognised as an initial substring of command
    {
      LED_off();                 // this could be useful indicator of valid command received
        
      USARTx_sendString(&ASCOMSerial, "  STA received HAL.\r\n");    // this is from ASCOMSerial, we'll set the encoder to this value todo - remove this line
      locate_String(command);                   // locates 'STA' and copies in spaces instead - required for strtol 

      uint16_t azval = strtol(command,'\0',10) ;   // alternatively ascii to long 
      if ( (azval >=1) && (azval <= 360) )         // TODO REMEMBER az zero is not allowed because of the way the arduino function worked - this may no longer apply
        {
            // convert the long to string and print it back as a test
            char text[30] = "                             ";        //note 29 spaces in string
            sprintf(text,"%d",azval);
            //to_string(azval);
            USARTx_sendString(&ASCOMSerial, "the az converted back is "); 
            USARTx_sendString(&ASCOMSerial, text);                          // "Val is valid HAL -  \r\n");
            USARTx_sendString(&ASCOMSerial, " \r\n");
           // this is the integer to ascii function itoa
           // USARTx_sendString(&USART1, val_as_char);

        }
        else{
            USARTx_sendString(&ASCOMSerial, " Val is not valid HAL. here it is: \r\n");
            USARTx_sendString(&ASCOMSerial, command);
        }

         //if the conversion is a valid double, check  0<= double <= 360, then if valid,  - call a fn to action the STA999.99
    }
    //if (/* condition from  UART - 0 or 1  */ )
  //  {
  //       code to validate and process commands from next uart
   // }
    
    
    else 
    {
         USARTx_sendString(&ASCOMSerial, "Unrecognised string HAL, type STA999.99 or AZ \r\n");
    }
}


void encoder()
{
  //Encoder:


  if (A_Counter < 1)
  {
    A_Counter =  A_Counter + 10253;     // set the counter floor value
  }

  if (A_Counter > 10253)   // set the counter ceiling value
  {
    A_Counter = A_Counter -  10253;
  }

  Azimuth = (A_Counter) / 28;    // 28.481 is 10253 (counts for 25.6 revs) / 360 (degrees)
  // i.e number of ticks per degree

  // some error checking
  if (Azimuth < 1)
  {
    Azimuth = 1;
  }

  if (Azimuth > 36000)
  {
    Azimuth = 36000;
  }


}  // end void encoder

void encoderInterrupt()               // Interrupt function
{

  char A_Phase , B_Phase;      //describe the encoder input wires
  //check the phase pins - A_Phase is portC pin2, B_Phase is pin3

 A_Phase = bit_is_set(PORTC.IN, PIN2_bm); //This will return a 0 if the bit is clear, and non-zero if the bit is set.
  if (A_Phase > 0)
    {
      A_Phase=1;
    }

 B_Phase = bit_is_set(PORTC.IN, PIN3_bm);    // pin3 bit mask
  if (B_Phase > 0)
    {
      B_Phase=1;
    }
  //i = digitalRead( B_PHASE);
  //j = digitalRead( A_PHASE);
  if (A_Phase == B_Phase)
  {
    A_Counter -= 1;
  }

  else
  {
    A_Counter += 1;   // increment the counter which represents increasing dome angle
  }  // end else clause
}  // end void interrupt