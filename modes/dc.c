volatile uint32_t cur_addr = 0;

/*------------------------------------------*/
/*                 Timing                   */
/*------------------------------------------*/

void timer_init () {
  TCA0_SINGLE_PER = 40000; // 16MHz clock / 400Hz sampling rate
  TCA0_SINGLE_CTRLA = TCA_SINGLE_ENABLE_bm;
  TCA0_SINGLE_INTCTRL |= TCA_SINGLE_OVF_bm; // Enable timer interrupts on overflow on timer A
  sei();
}

ISR(TCA0_OVF_vect)
{
    TCA0_SINGLE_INTFLAGS = TCA_SINGLE_OVF_bm;

    sensor_sample(SS_SEN0, cur_addr);
    cur_addr += 18;
    if((cur_addr % 0x100) % 0xFC == 0) // skip last 4 bytes of each page
      cur_addr += 4;

    if(cur_addr > (max_addr - 0x12)) { // if next address is within 18 bytes of end
      TCA0_SINGLE_CTRLA = 0; // stop sampling
      PORTA_OUTCLR = LED_PIN;
    }

    if(cur_addr == 0x1c90) { // After one second at 400Hz
      TCA0_SINGLE_CTRLA = 0x00; // Equivalent to TCA0_SINGLE_DISABLE_bm;
      blink(3); // Wait three seconds without recording
      PORTA_OUTSET = LED_PIN; // Turn on LED for run time
      TCA0_SINGLE_CTRLA = TCA_SINGLE_ENABLE_bm;
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

  // Setup memory
  flash_chip_erase ();
  flash_wait_for_ready();

  PORTA_OUTSET = LED_PIN; // Indicate erase is done
  while(PORTA_IN & SWITCH_PIN); // Wait until switch is toggled back low

  blink(4); // count off to time calibration

  // Start sampling
  timer_init();
  while(1);
}
