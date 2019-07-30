const int MOSI = PIN1_bm; // PORT A
const int SCK = PIN3_bm; // PORT A
const int SS_MEM = PIN0_bm; // PORT B
const int SS_SEN0 = PIN0_bm; // PORT C
const int SS_SEN1 = PIN1_bm; // PORT C
const int SS_SEN2 = PIN2_bm; // PORT C
const int SS_SEN3 = PIN3_bm; // PORT C
const int SS_SEN = PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm; // PORT C

/*------------------------------------------*/
/*               Low Level                  */
/*------------------------------------------*/

void spi_init() {
  // Setup output pins, drive SS lines high
  PORTA_DIRSET = MOSI | SCK;

  PORTB_DIRSET = SS_MEM;
  PORTB_OUTSET = SS_MEM;

  PORTC_DIRSET = SS_SEN;
  PORTC_OUTSET = SS_SEN;

  SPI0_CTRLA = SPI_MASTER_bm | SPI_CLK2X_bm | SPI_ENABLE_bm; // Enable SPI in fast master mode
}

unsigned char spi_transfer (unsigned char data) {
  SPI0_DATA = data; // Place data in transfer register
  while(!(SPI0_INTFLAGS & SPI_IF_bm)); // Wait until SPI Interrupt Flag is set
  return(SPI0_DATA);
}

/*------------------------------------------*/
/*             SPI Functions                */
/*------------------------------------------*/

void flash_send_address (unsigned int addr) { //Sends 3 byte addresses over SPI
  // Convert address to 3 bytes and send
  spi_transfer((addr >> 16) & 0xFF);
  spi_transfer((addr >> 8) & 0xFF);
  spi_transfer(addr & 0xFF);
}

void flash_wait_for_ready ()
{
  PORTB_OUTCLR = SS_MEM; // Set SS low
  spi_transfer(0x05); // Read status register 1 command
  while((spi_transfer(0) & 1)); // Keep reading until least bit = 0, indicates not chip isn't busy
  PORTB_OUTSET = SS_MEM; // Set SS High
}

void flash_write_enable () {
  PORTB_OUTCLR = SS_MEM; // Set SS low
  spi_transfer(0x06); //Write enable
  PORTB_OUTSET = SS_MEM; // Set SS High
}

// For external use
void flash_start_read (unsigned int addr) {
  flash_wait_for_ready();

  PORTB_OUTCLR = SS_MEM;   // Set SS low
  spi_transfer(0x03); // Read Command
  flash_send_address(addr);
}

void flash_end_read () {
  PORTB_OUTSET = SS_MEM;   // Set SS high
}

void flash_write_single (unsigned int addr, unsigned char data){
  flash_wait_for_ready();
  flash_write_enable();

  PORTB_OUTCLR = SS_MEM; // Set SS low
  spi_transfer(0x02); // Write command
  flash_send_address(addr);
  spi_transfer(data);
  PORTB_OUTSET = SS_MEM; // End Write
}

void flash_write_sample (unsigned int addr){
  flash_wait_for_ready();
  flash_write_enable();

  PORTB_OUTCLR = SS_MEM; // Set SS low
  spi_transfer(0x02); // Write command
  flash_send_address(addr);
}

void flash_block_erase (unsigned int addr){
  flash_wait_for_ready();
  flash_write_enable();

  PORTB_OUTCLR = SS_MEM; // Set SS low
  spi_transfer(0x52); // Block Erase command
  flash_send_address(addr);
  PORTB_OUTSET = SS_MEM; // End Write
}

void flash_chip_erase (){
  flash_wait_for_ready();
  flash_write_enable();

  PORTB_OUTCLR = SS_MEM; // Set SS low
  spi_transfer(0xC7); // Chip Erase command
  PORTB_OUTSET = SS_MEM; // End Write
}
