const uint8_t tx_pin = PIN2_bm; // Port B
const uint16_t uart_baudrate = 1388; //Specific to 3.33MHz

void uart_init (){
  // Set up Tx pin
  PORTB_DIRSET = tx_pin;
  PORTB_OUTSET = tx_pin;

  // Enable UART
  USART0_BAUD = uart_baudrate;// set baud rate
  USART0_CTRLB = USART_TXEN_bm; // Enable Tx Pin
  USART0_CTRLC = USART_SBMODE_2BIT_gc | USART_CHSIZE_8BIT_gc; // Set 2 stop bits and 8bit messages
}

void uart_transmit (unsigned char data){
  while(!(USART0_STATUS & USART_DREIF_bm)); // Wait for old data to finish transmitting
  USART0_TXDATAL = data; // put data byte into the transfer register
}

void uart_print_byte_hex (unsigned char data) {
  uint8_t msd = data / 16 + 48; // First digit in hex, shifted to 0 in ASCII
  uint8_t lsd = data % 16 + 48; // Second digit

  if (msd > 57) // shift values from A-F to ASCII A
    msd += 7;
  if (lsd > 57)
    lsd += 7;

  uart_transmit(msd);
  uart_transmit(lsd);
}
