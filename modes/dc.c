volatile uint32_t cur_addr = 0;

/*------------------------------------------*/
/*                 Timing                   */
/*------------------------------------------*/

void timer_init () {
  TCA0_SINGLE_PER = 10000; // 16MHz clock / 1.6kHz sampling rate
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

    if(cur_addr > (max_addr - 0x12)) // if next address is within 18 bytes of end
      TCA0_SINGLE_CTRLA = 0; // stop sampling
    }
}

/*------------------------------------------*/
/*                  Main                    */
/*------------------------------------------*/

void collect_data() {
  // Initialize communications and sensors
  spi_init();

  sensor_init(SS_SEN0);

  // Setup memory
  flash_chip_erase ();
  flash_wait_for_ready();

  while(!(~PORTA_IN & PIN6_bm));

  // Start sampling
  timer_init();
  while(1);
}
