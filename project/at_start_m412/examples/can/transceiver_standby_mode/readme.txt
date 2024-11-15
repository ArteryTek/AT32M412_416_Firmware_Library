/**
  **************************************************************************
  * @file     readme.txt
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board and at32-comm-ev, in this demo, 
  shows how to use the can transceiver standby mode. 
  the led4 on when mcu is in run mode and the canfd transceiver isn't in standby mode.
  the led4 off when mcu enter sleep mode and the canfd transceiver enter standby mode.
  it is wake up the canfd transceiver and mcu when received message, and ehco.
  set-up
  - can_tx(pb9)
  - can_rx(pb8)
  - can_stb(pb7)

  for more detailed information. please refer to the application note document AN0232.
