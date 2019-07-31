#include <avr/io.h>
#include <avr/interrupt.h>

#include "com/uart.c"
#include "com/spi.c"

#include "modes/dc.c"
#include "modes/do.c"

const int DC_BTN = PIN5_bm; // PORT A
const int DO_BTN = PIN6_bm; // PORT A

int main () {
  // Mode button setup
  PORTA_DIRCLR = DO_BTN | DC_BTN;
  PORTA_PIN5CTRL = PORT_PULLUPEN_bm;
  PORTA_PIN6CTRL = PORT_PULLUPEN_bm;

  // Testing
  uart_init();
  uart_transmit('X');
  uart_transmit(10);
  uart_transmit(13);

  while(1){
    if (~PORTA_IN & DC_BTN) { // Button press grounds the pin, pull-up resistor is enabled otherwise
      uart_transmit('C');
      //collect_data();
    }
    if (~PORTA_IN & DO_BTN) {
      uart_transmit('O');
      //output_data();
    }
  }
}
