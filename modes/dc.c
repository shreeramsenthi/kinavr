volatile uint32_t cur_addr = 0;
volatile uint32_t cur_sample = 0;
const uint16_t samples_per_axis = 100;

/*------------------------------------------*/
/*                 Timing                   */
/*------------------------------------------*/

void timer_init () {
  TCA0_SINGLE_PER = 10000; // 16MHz clock / DIV2 prescaler / 800Hz sampling rate
  TCA0_SINGLE_CTRLA = TCA_SINGLE_ENABLE_bm | TCA_SINGLE_CLKSEL_DIV2_gc;
  TCA0_SINGLE_INTCTRL |= TCA_SINGLE_OVF_bm; // Enable timer interrupts on overflow on timer A
  sei();
}

ISR(TCA0_OVF_vect)
{
    TCA0_SINGLE_INTFLAGS = TCA_SINGLE_OVF_bm;

    sensor_sample(SS_SEN0, cur_addr);
    cur_addr += 18;
    sensor_sample(SS_SEN1, cur_addr);
    cur_addr += 18;
	cur_sample++;
	
    if((cur_addr % 0x100) % 0xFC == 0) // skip last 4 bytes of each page
      cur_addr += 4;

    if(cur_addr > (max_addr - 0x12)) { // if next address is within 18 bytes of end
      TCA0_SINGLE_CTRLA = 0; // stop sampling
      PORTA_OUTCLR = LED_PIN;
    }

    if(cur_sample == samples_per_axis) { // Pause after first calib set, continue directly after second
      TCA0_SINGLE_CTRLA = 0x00; // Equivalent to TCA0_SINGLE_DISABLE_bm;
	  
	  PORTA_OUTSET = LED_PIN; // Indicate end of calib sampling
	  while(~PORTA_IN & SWITCH_PIN); // Wait until switch is toggled back high
	  
	  PORTA_OUTCLR = LED_PIN;
	  _delay_ms(5000); // To remove magnet
      
	  PORTA_OUTSET = LED_PIN; // Turn on LED for run time
      TCA0_SINGLE_CTRLA = TCA_SINGLE_ENABLE_bm | TCA_SINGLE_CLKSEL_DIV2_gc;
    }
}

/*------------------------------------------*/
/*                  Main                    */
/*------------------------------------------*/

void collect_data() {
  // Indicate mode
  blink(2);

  // Initialize communications and sensors
  spi_init();

  sensor_init(SS_SEN0);
  sensor_init(SS_SEN1);

  // Setup memory
  flash_chip_erase ();
  flash_wait_for_ready();

  PORTA_OUTSET = LED_PIN; // Indicate erase is done
  while(PORTA_IN & SWITCH_PIN); // Wait until switch is toggled back low

  PORTA_OUTCLR = LED_PIN;
  _delay_ms(5000); // To remove magnet
  
  // Start sampling
  timer_init();
  while(1);
}
