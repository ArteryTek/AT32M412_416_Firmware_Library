/**
  **************************************************************************
  * @file     readme.txt
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board and at32-comm-ev, in this demo, 
  shows how to use the can communication mode. every 1s transmit one message
  and the led4 blink, if receive a message, led2 blink(message id is 0x400) or led3
  blink(message id is not equal to 0x400).
  set-up
  - can_tx(pb9)
  - can_rx(pb8)
  - can_stb(pb7)

  for more detailed information. please refer to the application note document AN0232.
