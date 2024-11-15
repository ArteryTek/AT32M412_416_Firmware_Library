/**
  **************************************************************************
  * @file     readme.txt
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board, in this demo, shows how to configure
  the tmr1 peripheral to detect the brake bidirectional input signs.
  
  when the brake input detects low level, the pwm output will be turned off
  and the brake pin will output low level, until the software writes BRKRLS
  to release the brake output, and restart the brake protection circuit.
  
  set-up
  connect the tmr1 pins to an oscilloscope to monitor the waveforms:
  tmr1_brk - pa6
  tmr1_ch1 - pa8
  
  for more detailed information. please refer to the application note document AN0236.