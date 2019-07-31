const uint32_t max_addr = 0x3FFFFF; // Address to count up to. 3F for 32Mb

void output_data () {
  // Setup Communications
  uart_init();
  spi_init();

  // Set up counters
  uint32_t addr = 0; // We will iterate through addresses

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

  while(addr <= max_addr) { // Repeat until all addresses output
    char data = spi_transfer(0); // Read in next byte
    uart_print_byte_hex(data); //Print byte
    addr++; // increment address

    // Drop line every 18 bytes, or one sample
    if((addr % 0xFF) % 18 == 0) {
      //uart_transmit(10); // for live view only
      uart_transmit(13);
    }
    if((addr % 0xFF) % 0xFC == 0){ // skip last 4 bytes of each page
      addr += 3;
      spi_transfer(0); // Ignore next 4 bytes
      spi_transfer(0); //
      spi_transfer(0); //
      spi_transfer(0); //
      uart_transmit(13);
    }
  }

  // End read command
  flash_end_rw();
}
