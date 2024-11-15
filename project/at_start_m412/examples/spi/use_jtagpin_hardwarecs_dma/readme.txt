/**
  **************************************************************************
  * @file     readme.txt
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board, in this demo, shows how to use
  dma recieve data. spi2 use jtag pin as spi pin,and config spi in hardware
  cs mode. after download the code,please disconnect the pa13 and pa14 from
  downloader to make sure communication is ok.
  the pins connection as follow:
  - spi1                     spi2 
    pa4(cs)        <--->     pa15(cs)
    pa5(sck)       <--->     pb3 (sck)
    pa6(miso)      <--->     pa13(miso)
    pa7(mosi)      <--->     pa14(mosi)

  for more detailed information. please refer to the application note document AN0102.
