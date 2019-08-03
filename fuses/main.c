#if 1 // Set the MCU to use the 16MHz clock
FUSES =
  {
  //Watchdog Configuration (default is 0x00)
  .WDTCFG=WINDOW_OFF_gc | PERIOD_OFF_gc,
  //BOD Configuration (default is 0x00)
  .BODCFG=0x00,
  // This line is not default!
  //Oscillator Configuration (default is 0x02)
  .OSCCFG=FREQSEL_16MHZ_gc,
  //Apparently need for the compiler.
  .reserved_0x03=0x00,
  //TCD0 Configuration (default is 0x00)
  .TCD0CFG = 0x00,
  //System Configuration 0 (default is 0xC4)
  .SYSCFG0 = CRCSRC_NOCRC_gc| RSTPINCFG_UPDI_gc,
  //System Configuration 1 (default is 0x07)
  .SYSCFG1 = SUT_64MS_gc,
  //Application Code Section End (default is 0x00)
  .APPEND = 0x00,
  //Boot Section End (default is 0x00)
  .BOOTEND = 0x00,
  };
#endif
