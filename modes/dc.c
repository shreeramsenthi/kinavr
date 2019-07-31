
/*------------------------------------------*/
/*                 Timing                   */
/*------------------------------------------*/

void timer_init () {
  TCA0_SINGLE_PER = 3255; // 3.33MHz / 1024 = 3255 for 1 second period
  TCA0_SINGLE_CTRLA |= (TCA_SINGLE_CLKSEL_DIV1024_gc) | (TCA_SINGLE_ENABLE_bm);
  TCA0_SINGLE_INTCTRL |= (TCA_SINGLE_OVF_bm); // Enable timer interrupts on overflow on timer A
  sei();
}

ISR(TCA0_OVF_vect)
{

  TCA0_SINGLE_INTFLAGS = TCA_SINGLE_OVF_bm;
}

/*------------------------------------------*/
/*                  Main                    */
/*------------------------------------------*/

void collect_data() {
  // Timer Initialization
  timer_init();

  while(1);
}
