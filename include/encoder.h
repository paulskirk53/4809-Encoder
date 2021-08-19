#pragma once;

#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#include <stdlib.h>
#include <arduino.h>

#define F_CPU 3333333
#define USART1_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)
#define MAX_COMMAND_LEN		16
#define INIT_DELAY			10	// Delay to invalidate the after-reset noise on the PC0 pin (TX) 

#define ASCOMSerial   USART1      // give the USARTS meaningful names
#define MONITORSerial USART0      //this will be a ftdi
#define STEPPERSerial USART2      //this is serial to serial hardwired connectors

#define encoder_Pin PIN_PC2            // pin used for encoder interrupts
// Pin PC3 is also used for the encoder input - see local declaration in the interrupt routine


//global variable declarations - if these vars are used in other files such as main.cpp or encoder.cpp, they must be declared here.

extern volatile uint16_t A_Counter;     // Initialise this to 10253 / (360 / 261) - the position of due west - 261 for the dome when the scope is at 270. 
extern long Azimuth;

//of the chars which appear at ASCOMSerial, the following  list defines those which are valid.
const char validset[18] {'A','Z','S','T','0','1','2','3','4','5','6','7','8','9','.','+','-','\0'};  
bool validchar(char c);
void encoder();
void encoderInterrupt();
void locate_String(char* command);
void USART1_init(void);
void USARTx_sendChar(USART_t* usart_x, char c);
void USARTx_sendString(USART_t* usart_x , char *str);
char USARTx_readChar(USART_t* usart);
void LED_on(void);
void LED_off(void);
void LED_init(void);
void executeCommand(char *command);
void pins_disable(void);
