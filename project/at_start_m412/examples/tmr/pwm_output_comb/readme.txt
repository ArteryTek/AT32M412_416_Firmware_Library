/**
  **************************************************************************
  * @file     readme.txt
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board, in this demo, shows how to configure
  the tmr1 peripheral to generate combined pwm signals.

  in this demo, channels 1 and 2 are config to combined pwm mode a and b, for 
  comparison channes 3 and 4 are config to pwm mode a and b.
  for channel 1: output is the logical OR between channel 3 and channel 4 output.
  for channel 2: output is the logical AND between channel 3 and channel 4 output.
  
  #define CH5_COMBINED_MODE is used to enable combined 3-phase pwm mode, enable 
  this function, channel 1, 2 and 3 output will be logical AND with channel 5.
  
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