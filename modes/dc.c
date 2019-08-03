volatile uint32_t cur_addr = 0;

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
  sensor_sample(SS_SEN0, cur_addr);
  cur_addr += 18;
  if((cur_addr % 0xFF) % 0xFC == 0) // skip last 4 bytes of each page
    cur_addr += 3;

  if(cur_addr > (max_addr - 0x12)) // if next address is within 18 bytes of end
    cli(); // clear global interrupts (stop sampling)

  TCA0_SINGLE_INTFLAGS = TCA_SINGLE_OVF_bm;
}

/*------------------------------------------*/
/*                  Main                    */
/*------------------------------------------*/

void collect_data() {
  // Initialize communications and sensors
  uart_init(); // testing
  spi_init();

  // Update clock speed
  CPU_CCP = CCP_IOREG_gc; // Unlock protected io regs
  CLKCTRL_MCLKCTRLB = 0; // set clock speed to 20MHz
  USART0_BAUD = uart_baudrate * 6;// set baud rate
  while(1)
    uart_transmit('E');

  //sensor_init(SS_SEN0);

  //// Setup memory
  //flash_chip_erase ();
  //flash_wait_for_ready();

  //while(!(~PORTA_IN & PIN6_bm));

  // Start sampling
  timer_init();
  while(1);
}
