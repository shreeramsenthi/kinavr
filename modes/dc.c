volatile uint32_t cur_addr = 0;

/*------------------------------------------*/
/*                 Timing                   */
/*------------------------------------------*/

void timer_init () {
  TCA0_SINGLE_PER = 32000; // 16MHz clock / 500Hz sampling rate
  TCA0_SINGLE_CTRLA = TCA_SINGLE_CLKSEL_DIV4_gc | TCA_SINGLE_ENABLE_bm;
  TCA0_SINGLE_INTCTRL |= TCA_SINGLE_OVF_bm; // Enable timer interrupts on overflow on timer A
  sei();
}

ISR(TCA0_OVF_vect)
{
  //testing
  uint16_t start_time = TCA0_SINGLE_CNT;
  uart_print_byte_hex(start_time >> 8);
  uart_print_byte_hex(start_time & 0xFF);

  sensor_sample(SS_SEN0, cur_addr);
  cur_addr += 18;
  if((cur_addr % 0xFF) % 0xFC == 0) // skip last 4 bytes of each page
    cur_addr += 3;

  if(cur_addr > (max_addr - 0x12)) // if next address is within 18 bytes of end
    cli(); // clear global interrupts (stop sampling)

  //testing
  uint16_t end_time = TCA0_SINGLE_CNT;
  uart_print_byte_hex(end_time >> 8);
  uart_print_byte_hex(end_time & 0xFF);
  uart_transmit(10);
  uart_transmit(13);

  TCA0_SINGLE_INTFLAGS = TCA_SINGLE_OVF_bm;
}

/*------------------------------------------*/
/*                  Main                    */
/*------------------------------------------*/

void collect_data() {
  // Initialize communications and sensors
  uart_init(); // testing
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
