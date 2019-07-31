#include <avr/io.h>
#include <avr/interrupt.h>

#include "com/uart.c"
#include "com/spi.c"

#include "modes/dc.c"
#include "modes/do.c"

const int DO_BTN = PIN5_bm; // PORT A
const int DC_BTN = PIN6_bm; // PORT A

ISR (PORTA_PORT_vect) {
  uart_print_byte_hex(PORTA_INTFLAGS);
  PORTA_PIN5CTRL = 0;
  PORTA_PIN6CTRL = 0;
  PORTA_INTFLAGS |= DO_BTN | DC_BTN;
}

int main () {
  // Mode button setup
  PORTA_DIRCLR = DO_BTN | DC_BTN;
  PORTA_PIN5CTRL = PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;
  PORTA_PIN6CTRL = PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;
  sei();

  // Testing
  uart_init();
  uart_transmit('X');
  uart_transmit(10);
  uart_transmit(13);

  //output_data();
  //collect_data();

  while(1);
}
