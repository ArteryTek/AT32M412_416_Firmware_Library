/**
  **************************************************************************
  * @file     readme.txt
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board, in this demo, shows how to use
  combine_mode(ordinary simultaneous mode only).
  the trigger source is: tmr1trgout2,,and use dma mode 1 transfer conversion data
  the convert data as follow:
  - adccom_ordinary_valuetab[n][0] ---> adc1_channel_2
  - adccom_ordinary_valuetab[n][1] ---> adc2_channel_1
  
  - adccom_ordinary_valuetab[n][2] ---> adc1_channel_4
  - adccom_ordinary_valuetab[n][3] ---> adc2_channel_3

  - adccom_ordinary_valuetab[n][4] ---> adc1_channel_6
  - adccom_ordinary_valuetab[n][5] ---> adc2_channel_5

  for more detailed information. please refer to the application note document AN0180.
