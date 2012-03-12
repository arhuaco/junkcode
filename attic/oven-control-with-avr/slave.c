/* http://www.captain.at/electronic-atmega16-serial-port.php */

/*********************************************
* Chip type           : ATmega16
* Clock frequency     : 4,000000 MHz
*********************************************/

/* There are small --innocuous-- bugs in this file
 * Be ware when using this code in other context */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <avr/iom16.h>
#include <util/delay.h>

#include "avrlib/a2d.c"

#define F_OSC 4000000		           /* oscillator-frequency in Hz */
#define UART_BAUD_RATE 9600
#define UART_BAUD_CALC(UART_BAUD_RATE,F_OSC) ((F_OSC)/((UART_BAUD_RATE)*16l)-1)

/* Simple I/O routines */

void usart_putc(unsigned char c) {
  // wait until UDR ready
  while(!(UCSRA & (1 << UDRE)));
   UDR = c;    // send character
}

void uart_puts (char *s) {
  while (*s) 
    usart_putc(*s), s++;
}

void uintprint(unsigned int x)
{
  char stack[10];
  int size = 0;

  while(x >0)
    stack[size++] = x % 10,  x /= 10;

  if (size)
    while (size > 0)
    {
      usart_putc('0' + stack[--size]);
    }
  else
   usart_putc('0');
}

/* Initialize the serial console...
 * I don't like this code */

void init(void) {
  // set baud rate
  UBRRH = (uint8_t)(UART_BAUD_CALC(UART_BAUD_RATE,F_OSC)>>8);
  UBRRL = (uint8_t)UART_BAUD_CALC(UART_BAUD_RATE,F_OSC);
  // Enable receiver and transmitter; enable RX interrupt
  UCSRB = (1 << RXEN) | (1 << TXEN) | (1 << RXCIE);
  //asynchronous 8N1
  UCSRC = (1 << URSEL) | (3 << UCSZ0);
}


/* state of the blinking led
 * Let's create a blinking let with simple counters... */

#define MAX_BLINKER_LED  32766
#define MAX_COMMAND_LED (MAX_BLINKER_LED / 8)
uint16_t blink_led_count = MAX_BLINKER_LED;
uint16_t command_led_count = MAX_BLINKER_LED;
uint8_t direction = 1;

/* state of the controller */

volatile char do_command = 0;
volatile unsigned char the_command = 0;

uint8_t actual_channel  = 0;

/* useful definitions */
#define PORTD_CLEAR(PDX) (PORTD &= ~(1 << PDX))
#define PORTD_SET(PDX) (PORTD |= 1 << PDX)

#define PORTC_CLEAR(PCX) (PORTC &= ~(1 << PCX))
#define PORTC_SET(PCX) (PORTC |= 1 << PCX)

/* Commands */

#define VERBOSE 0

void
process_command (void)
{
  unsigned char c = the_command;
  char *s ="$";

  switch (c)
  {
    case '0': case '1':         /* select channel */

      actual_channel = c - '0';

      if (actual_channel == 0)
        s = "0 sel";
      else if (actual_channel == 1)
        s = "1 sel"; 
      break;

    case '>':               /* ping */
      s = "<";
     if (!VERBOSE)
       uart_puts("<\n\r");
      break;

    case 'r': case 'R':     /* read */
      uintprint(a2dConvert10bit(actual_channel));
     if (!VERBOSE)
       uart_puts("\n\r");
      s = 0;
      break;

    case '+':   /* set */

      if (actual_channel == 0)
        PORTD_SET(PD6), s = "0 on"; /* actuator 0 */
      else if (actual_channel == 1)
        PORTD_SET(PD7), s = "1 on"; /* actuator 0 */

      break;

    case '-':  /* clear */

      if (actual_channel == 0)
        PORTD_CLEAR(PD6), s = "0 off"; /* actuator 0 */
      else if (actual_channel == 1)
        PORTD_CLEAR(PD7), s = "1 off"; /* actuator 0 */
      break;

    default:              /* Invalid command */
      s = "?";
   }

   if (s && VERBOSE)
     uart_puts(s);
   if (VERBOSE)
     uart_puts("\n\r");
}


// INTERRUPT can be interrupted, SIGNAL can't be interrupted
// USART RX interrupt
SIGNAL (SIG_UART_RECV)
{ 
  cli();
  the_command = UDR;
  do_command = 1;

  command_led_count = MAX_COMMAND_LED;
  PORTD_SET(PD4); /* command led on */
}



int main(void)
{
  init(); // init USART

  a2dInit(); // Analog-Digital converter initialization
  a2dSetReference(ADC_REFERENCE_256V);   // Internal 2.56V Voltage Reference with external capacitor at AREF pin
  a2dSetPrescaler(ADC_PRESCALE_DIV128);  // Slowest/more accurate ADC (avrlib/a2d.h)

  // enable pins as outputs
  DDRD |= (1<<PD7); /* actuator 1 */
  DDRD |= (1<<PD6); /* actuator 0 */
  DDRD |= (1<<PD5); /* blinker */
  DDRD |= (1<<PD4); /* command received */

  DDRC |= (1<<PC0); /* first actuator */

  while (1)
  {

   cli(); // disable interrupts

    if (do_command)
      process_command(), do_command = 0;
  
   sei();  // enable interrupts

    /* blinker */
    if (blink_led_count == MAX_BLINKER_LED)
      direction = -1, PORTD_SET(PD5); /* on */
    else if (blink_led_count == -MAX_BLINKER_LED)
      direction = 1, PORTD_CLEAR(PD5); /* off */

    blink_led_count += direction;

    /* command led countdown */
    if (command_led_count > 0)
      -- command_led_count;
    if (command_led_count == 1)
      PORTD_CLEAR(PD4); /* off */
  }

  return 0;
}
