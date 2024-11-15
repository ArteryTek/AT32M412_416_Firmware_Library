/**
  **************************************************************************
  * @file     readme.txt
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board, in this demo, shows how to configure
  the tmr1 peripheral to generate asymmetric pwm signals.

  in this demo, channels 1 and 2 are config to asymmetric pwm mode a, for comparison
  channes 3 and 4 are config to pwm mode a.
  for channel 1: the rising edge is synchronized with the rising edge of channel 4,
  and the falling edge is synchronized with the falling edge of channel 3.
  for channel 2: the rising edge is synchronized with the rising edge of channel 3,
  and the falling edge is synchronized with the falling edge of channel 4.
  
  #define INDEPENDENT_ASYM is used to enable independent asymmetric pwm mode.
  
  set-up
  connect the tmr1 pins to an oscilloscope to monitor the different waveforms:
  - tmr1_ch1  --->  pa8
  - tmr1_ch1c --->  pa7
  - tmr1_ch2  --->  pa9
  - tmr1_ch2c --->  pb0
  - tmr1_ch3  --->  pa10
  - tmr1_ch3c --->  pb1
  - tmr1_ch4  --->  pa11
  - tmr1_ch4c --->  pb4

  for more detailed information. please refer to the application note document AN0236.