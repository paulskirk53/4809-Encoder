/*
    \file   main.c
    \brief  Main file of the project.
    (c) 2020 Microchip Technology Inc. and its subsidiaries.
*/

#include "encoder.h"

//attachInterrupt(encoder_Pin,interrupt, RISING);
volatile uint16_t A_Counter = 7433; // note the variable is declared in the header file as extern. Its initial value is assigned here

int main(void)
{

  pins_disable(); // because we're using the 40 pin dip and it doesn't have these pins

  PORTC.DIRCLR = PIN2_bm; // set pins Port C2 and Port C3 as input for the encoder hardware
  PORTC.DIRCLR = PIN3_bm;
  PORTC.PIN2CTRL = PORT_PULLUPEN_bm; // enable pullup R on pins c2 and c3
  PORTC.PIN3CTRL = PORT_PULLUPEN_bm;

  //ok now we have pin PC2 and PC3 set up as #defines in .h  and pull up r's if needed - check arduino version. Now need to know how
  // to read the pins looking at the GPIO from microchip, it looks like use of PIN2_bm and PIN3_bm is used in this

  attachInterrupt(encoder_Pin, encoderInterrupt, RISING);

  char command[MAX_COMMAND_LEN]; // the string to hold incoming commands from all UARTs
  char* ptr_command;             //set up a pointer to the command string
  uint8_t index = 0;
  char c;

  /* This delay invalidates the initial noise on the TX line, after device reset. */
  _delay_ms(10);
  // USART0_init();     // will be comms with Stepper
  // USART2_init();     // will be comms with host PC - MCU Monitor
  USART1_init(); // comms with Host PC - ASCOM driver
  LED_init();

  // todo - remove the following line
  USARTx_sendString(&ASCOMSerial, "Valid commands are AZ, STA for the ASCOM usart\r\n");
  USARTx_sendString(&ASCOMSerial, "Valid commands are AZ for the Stepper usart\r\n");
  bool x;
  while (1) // the arduino loop equivalent
  {
    aggregateASCOMCommand(&ASCOMSerial, command);
    // aggregateMonitorCommand();
    // aggregateStepperCommand();

    //and next check STEPPERSerial
  }
}