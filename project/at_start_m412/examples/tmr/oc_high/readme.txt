/**
  **************************************************************************
  * @file     readme.txt
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board, in this demo, shows how to configur
  the tmr peripheral in output high mode with the corresponding interrupt requests
  for each channel.

  tmr4 configuration:
  tmr4clk = system_core_clock / 8,
  the objective is to get tmr4 counter clock at 1 khz:
  - prescaler = (tim2clk / tmr4 counter clock) - 1
  and generate 4 signals with 4 different delays:
  tmr4_ch1 delay = ccr1_val/tmr4 counter clock = 1000 ms
  tmr4_ch2 delay = ccr2_val/tmr4 counter clock = 500 ms
  tmr4_ch3 delay = ccr3_val/tmr4 counter clock = 250 ms
  tmr4_ch4 delay = ccr4_val/tmr4 counter clock = 125 ms

  connect the following pins to an oscilloscope to monitor the different waveforms:
  - pb6
  - pb7
  - pb8
  - pb9

  for more detailed information. please refer to the application note document AN0236.
