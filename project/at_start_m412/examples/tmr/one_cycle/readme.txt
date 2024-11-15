/**
  **************************************************************************
  * @file     readme.txt
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board, in this demo, pa9 connect to a
  key, press the key then pa8 output one pulse.
  
  #define RETRIGGER_OCM_MODE used to enable re-trigger one cycle mode, in
  this mode, during counting, the counter can be triggered again.
  
  
  set-up
  connect the tmr1 pins to an oscilloscope to monitor the waveforms:
  - tmr1_ch1 - pa8: output
  - tmr1_ch2 - pa9: trigger input

  for more detailed information. please refer to the application note document AN0236.