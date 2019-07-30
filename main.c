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

  TCA0_SINGLE_INTFLAGS = TCA_SINGLE_OVF_bm;
}

int main () {
  //// Timer Initialization
  //timer_init();
  //sei();

  output_data();

  while(1);
}
