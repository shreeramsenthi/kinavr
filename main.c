#include <avr/io.h>
#include <avr/interrupt.h>

#include "com/uart.c"
#include "com/spi.c"

#include "modes/dc.c"
#include "modes/do.c"

void timer_init () {
  TCA0_SINGLE_PER = 3255; // 3.33MHz / 1024 = 3255 for 1 second period
  TCA0_SINGLE_CTRLA |= (TCA_SINGLE_CLKSEL_DIV1024_gc) | (TCA_SINGLE_ENABLE_bm);
  TCA0_SINGLE_INTCTRL |= (TCA_SINGLE_OVF_bm); // Enable timer interrupts on overflow on timer A
}

ISR(TCA0_OVF_vect)
{
  uart_transmit('H');
  uart_transmit('I');
  uart_transmit(10);
  uart_transmit(13);

  TCA0_SINGLE_INTFLAGS = TCA_SINGLE_OVF_bm;
}

int main () {
  uart_init();
  timer_init();
  sei();

  uart_transmit('S');

  while(1);
}
