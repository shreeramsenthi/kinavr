const uint32_t max_addr = 0x3FFFFF; // Address to count up to 3F for 32Mb

void output_data () {
  // Indicate mode
  PORTA_OUTSET = LED_PIN;

  // Setup Communications
  uart_init();
  spi_init();

  // Set up counters
  uint32_t addr = 0; // We will iterate through addresses
  uint8_t FF_counter = 0;

  // Start Read
  flash_start_read(addr);

  // Indicate start of print
  //uart_transmit(10); // For new lines on live view, comment out for saving
  uart_transmit(13); // New line
  uart_transmit(83); // S
  uart_transmit(84); // T
  uart_transmit(65); // A
  uart_transmit(82); // R
  uart_transmit(84); // T
  //uart_transmit(10); // For live view only
  uart_transmit(13); // New line

  while(addr <= max_addr && FF_counter < 18) { // Repeat until all addresses output or a line of FF's are found
    char data = spi_transfer(0); // Read in next byte
	if(data == 0xFF)
		FF_counter++;
	else
		FF_counter = 0;
    uart_print_byte_hex(data); //Print byte
    addr++; // increment address

    // Drop line every 18 bytes, or one sample
    if((addr % 0x100) % 18 == 0) {
      //uart_transmit(10); // for live view only
      uart_transmit(13);
    }
    if((addr % 0x100) % 0xFC == 0){ // skip last 4 bytes of each page
      addr += 4;
      spi_transfer(0); // Ignore next 4 bytes
      spi_transfer(0); //
      spi_transfer(0); //
      spi_transfer(0); //
    }
  }
  
  // End read command
  flash_end_rw();
  
  // Indicate end
  PORTA_OUTCLR = LED_PIN;
  uart_transmit(13); // New line
  uart_transmit('D');
  uart_transmit('O');
  uart_transmit('N');
  uart_transmit('E');
  uart_transmit(13); // New line
  
  // Don't return to main()
  while(1);
}
