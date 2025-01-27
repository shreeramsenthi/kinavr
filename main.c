#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

const int LED_PIN = PIN4_bm;    // PORT A
const int SWITCH_PIN = PIN7_bm; // PORT A

void blink (int blinks) {
  for(int i = 0; i < blinks; i ++){
    PORTA_OUTSET = LED_PIN;
    _delay_ms(500);
    PORTA_OUTCLR = LED_PIN;
    _delay_ms(500);
  }
}

#include "com/uart.c"
#include "com/spi.c"

#include "modes/do.c"
#include "modes/dc.c"

int main () {
  // Setup master clock speed
  CPU_CCP = CCP_IOREG_gc; // Unlock protected io regs
  CLKCTRL_MCLKCTRLB = 0; // set clock speed to 16MHz

  // Hall effect switch setup
  PORTA_DIRCLR = SWITCH_PIN;
  PORTA_PIN7CTRL = PORT_PULLUPEN_bm;

  // LED setup
  PORTA_DIRSET = LED_PIN;
  PORTA_OUTSET = LED_PIN;

  while(1){
    if (~PORTA_IN & SWITCH_PIN) { // check if switch is low
      PORTA_OUTCLR = LED_PIN;
      _delay_ms(5000);
      if(PORTA_IN & SWITCH_PIN) // if switched to high
        collect_data();
      else
        output_data();
    }
    _delay_ms(1000);
  }
}
