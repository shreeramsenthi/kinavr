#include "mpu9250.h"

const uint8_t MOSI = PIN1_bm; // PORT A
const uint8_t SCK = PIN3_bm; // PORT A
const uint8_t SS_MEM = PIN0_bm; // PORT B
const uint8_t SS_SEN0 = PIN0_bm; // PORT C
const uint8_t SS_SEN1 = PIN1_bm; // PORT C
const uint8_t SS_SEN2 = PIN2_bm; // PORT C
const uint8_t SS_SEN3 = PIN3_bm; // PORT C
const uint8_t SS_SEN = PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm; // PORT C

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

  SPI0_CTRLB = 3; // SPI MODE 3
  SPI0_CTRLA = SPI_MASTER_bm |
               SPI_PRESC_DIV16_gc |
               SPI_ENABLE_bm; // Enable SPI in fast master mode
}

unsigned char spi_transfer (unsigned char data) {
  SPI0_DATA = data; // Place data in transfer register
  while(!(SPI0_INTFLAGS & SPI_IF_bm)); // Wait until SPI Interrupt Flag is set
  return(SPI0_DATA);
}

/*------------------------------------------*/
/*            Flash Functions               */
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

void flash_start_write (unsigned int addr){
  flash_wait_for_ready();
  flash_write_enable();

  PORTB_OUTCLR = SS_MEM; // Set SS low
  spi_transfer(0x02); // Write command
  flash_send_address(addr);
}

void flash_end_rw () {
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

/*------------------------------------------*/
/*            Sensor Functions              */
/*------------------------------------------*/

uint8_t mpu_reg_transfer (uint8_t SS, uint8_t reg, uint8_t val) {
  PORTC_OUTCLR = SS;
  spi_transfer(reg);
  uint8_t data = spi_transfer(val);
  PORTC_OUTSET = SS;
  return(data);
}

void mpu_write_register (uint8_t SS, uint8_t addr, uint8_t data) {
  mpu_reg_transfer(SS, addr, data);
  _delay_ms(10);
}

void mpu_start_read (uint8_t SS, uint8_t addr) {
  PORTC_OUTCLR = SS;
  spi_transfer(addr | 0x80); // Strip bank info, add read flag
}

void mpu_end_read (uint8_t SS) {
  PORTC_OUTSET = SS;
}

void mag_write_reg (uint8_t SS, uint8_t reg, uint8_t data) {
  mpu_write_register(SS, MPU9250_I2C_SLV0_ADDR, 0x0C); // Set i2c Address to write
  mpu_write_register(SS, MPU9250_I2C_SLV0_REG, reg); // Set register address
  mpu_write_register(SS, MPU9250_I2C_SLV0_DO, data); // Set value to write
}

void mag_start_read (uint8_t SS, uint8_t reg) {
  mpu_write_register(SS, MPU9250_I2C_SLV0_ADDR, 0x0C | 0x80); // Set i2c Address to read
  mpu_write_register(SS, MPU9250_I2C_SLV0_REG, reg); // Set register address
  mpu_write_register(SS, MPU9250_I2C_SLV0_DO, 0xFF); // Set value to write
}

void sensor_init (uint8_t SS) {
  // Reset Sensor
  mpu_write_register(SS, MPU9250_PWR_MGMT_1, MPU9250_BIT_H_RESET);
  _delay_ms(100);
  mpu_write_register(SS, MPU9250_USER_CTRL, MPU9250_BIT_I2C_IF_DIS); // SPI mode
  mpu_write_register(SS, MPU9250_PWR_MGMT_1, 0x00); // Clear sleep mode
  mpu_write_register(SS, MPU9250_PWR_MGMT_2, 0x00); // Ensure Accel + Gyro are enabled

  // Set Accel/Gyro resolution and LP Filter
  mpu_write_register(SS, MPU9250_ACCEL_CONFIG, 0x00); // 2G scale
  mpu_write_register(SS, MPU9250_GYRO_CONFIG, 0x00); // Enable LP filter and 250 scale (may be disabling LP filter, unclear)

  // Set up I2C Master
  mpu_write_register(SS, MPU9250_INT_PIN_CFG, 0x20); // Disable bypass and latch INT pin
  mpu_write_register(SS, MPU9250_I2C_MST_CTRL, 0x0D); // set I2C master clock at 400kHz
  mpu_write_register(SS, MPU9250_I2C_MST_DELAY_CTRL, 0x01); // Set magnetometer delay
  mpu_write_register(SS, MPU9250_USER_CTRL, MPU9250_BIT_I2C_IF_DIS | MPU9250_BIT_I2C_MST_EN); // Enable I2C master mode

  // Reset Mag
  mag_write_reg(SS, REG_AK8963_CNTL2, BIT_AK8963_CNTL2_SRST); // Reset magnetometer
  mpu_write_register(SS, MPU9250_I2C_SLV0_CTRL, 0x80); // Enable SLV0 and no read
  _delay_ms(100);
  mag_write_reg(SS, REG_AK8963_CNTL1, 0b0110); // Continous mode, 100Hz
  mpu_write_register(SS, MPU9250_I2C_SLV0_CTRL, 0x80); // Enable SLV0 and no read

  // Prep mag for reading
  mpu_write_register(SS, MPU9250_I2C_SLV0_CTRL, 0x87); // Enable SLV0 and 7 bytes of read
  mag_start_read(SS, 0x03); // data register on the mag
}

void sensor_sample (uint8_t SS, uint32_t addr) {
  uint8_t sample[18];

  // Read in data from sensor
  mpu_start_read(SS, MPU9250_ACCEL_XOUT_H);
  for (int i = 0; i < 6; i++)
    sample[i] = spi_transfer(0x00);
  spi_transfer(0x00); // Disregard two bytes of temperature
  spi_transfer(0x00);
  for (int i = 6; i < 18; i++)
    sample[i] = spi_transfer(0x00);
  mpu_end_read(SS);

  // Write to memory
  flash_start_write(addr);
  for(int i = 0; i < 18; i++)
    spi_transfer(sample[i]);
  flash_end_rw();
}
