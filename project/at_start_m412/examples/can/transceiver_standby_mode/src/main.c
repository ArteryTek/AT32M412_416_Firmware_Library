/**
  **************************************************************************
  * @file     main.c
  * @brief    main program
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */

#include "at32m412_416_board.h"
#include "at32m412_416_clock.h"

/** @addtogroup AT32M412_periph_examples
  * @{
  */

/** @addtogroup 412_CAN_transceiver_standby_mode CAN_transceiver_standby_mode
  * @{
  */

/* macro definition about some parameters of can bit time */
#define BITTIME_DIV                      1
#define AC_BTS1_SIZE                     135
#define AC_BTS2_SIZE                     45
#define AC_RSAW_SIZE                     45
/* the calculation process of can_clk is as follows:
   can_clk = can_pclk/BITTIME_DIV = 180M/1 = 180M; 
*/
/* the calculation process of nominal bit rate is as follows: 
   ac_baudrate = can_clk/(AC_BTS1_SIZE + AC_BTS2_SIZE) = 180M/(135 + 45) = 1M;
   ac_samplepoint = AC_BTS1_SIZE/(AC_BTS1_SIZE + AC_BTS2_SIZE) = 135/180 = 75%;
*/

can_rxbuf_type can_rxbuf_struct;

/**
  *  @brief  can gpio config
  *  @param  none
  *  @retval none
  */
static void can_gpio_config(void)
{
  gpio_init_type gpio_init_struct;

  /* enable the gpio clock */
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

  gpio_default_para_init(&gpio_init_struct);

  /* configure the can tx, rx, stb pin */
  /* can_stb connect to the transceiver <stby> pin, default output low, keep the transceiver 
     running in normal mode */   
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins = GPIO_PINS_9 | GPIO_PINS_8 | GPIO_PINS_7;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOB, &gpio_init_struct);

  gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE9, GPIO_MUX_9);
  gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE8, GPIO_MUX_9);
  gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE7, GPIO_MUX_9);
}

/**
  *  @brief  can configiguration.
  *  @param  none
  *  @retval the result of can_configuration
  *          this parameter can be one of the following values:
  *          SUCCESS or ERROR
  */
error_status can_configuration(void)
{
  can_bittime_type can_bittime_struct;
  
  /* as specified in CAN protocol, the maximum allowable oscillator tolerance is 1%. 
     The HICK accuracy does not meet the clock requirements in CAN protocol. to guarantee normal 
     communication, it is recommended to use HEXT as the system clock source. */
  if(crm_flag_get(CRM_HEXT_STABLE_FLAG) != SET)
  {
    return ERROR;
  }

  /* enable the can clock */
  crm_periph_clock_enable(CRM_CAN1_PERIPH_CLOCK, TRUE);
  crm_can_clock_select(CRM_CAN1, CRM_CAN_CLOCK_SOURCE_PCLK);

  can_software_reset(CAN1, TRUE);

  /* set can bit time */
  can_bittime_default_para_init(&can_bittime_struct);
  can_bittime_struct.bittime_div = BITTIME_DIV;
  can_bittime_struct.ac_bts1_size = AC_BTS1_SIZE;
  can_bittime_struct.ac_bts2_size = AC_BTS2_SIZE;
  can_bittime_struct.ac_rsaw_size = AC_RSAW_SIZE;
  can_bittime_set(CAN1, &can_bittime_struct);

  can_stb_transmit_mode_set(CAN1, CAN_STB_TRANSMIT_BY_FIFO);    

  can_software_reset(CAN1, FALSE);

  can_retransmission_limit_set(CAN1, CAN_RE_TRANS_TIMES_UNLIMIT);
  can_rearbitration_limit_set(CAN1, CAN_RE_ARBI_TIMES_UNLIMIT);
  can_rxbuf_warning_set(CAN1, 3);
  can_mode_set(CAN1, CAN_MODE_COMMUNICATE);

  /* can interrupt config */
  nvic_irq_enable(CAN1_RX_IRQn, 0, 0);
  nvic_irq_enable(CAN1_ERR_IRQn, 0, 0);
  can_interrupt_enable(CAN1, CAN_RIE_INT, TRUE);
  can_interrupt_enable(CAN1, CAN_BEIE_INT, TRUE);
  
  return SUCCESS;
}

/**
  *  @brief  can transmit data
  *  @param  none
  *  @retval none
  */
static void can_transmit_data(void)
{
  can_txbuf_type  can_txbuf_struct = {0};

  /* write the primary transmit buffer */
  can_txbuf_struct.id = 0x400;
  can_txbuf_struct.id_type = CAN_ID_STANDARD;
  can_txbuf_struct.frame_type = CAN_FRAME_DATA;
  can_txbuf_struct.data_length = CAN_DLC_BYTES_8;
  can_txbuf_struct.data[0] = 0x00;
  can_txbuf_struct.data[1] = 0x11;
  can_txbuf_struct.data[2] = 0x22;
  can_txbuf_struct.data[3] = 0x33;
  can_txbuf_struct.data[4] = 0x44;
  can_txbuf_struct.data[5] = 0x55;
  can_txbuf_struct.data[6] = 0x66;
  can_txbuf_struct.data[7] = 0x77;
  while(can_txbuf_write(CAN1, CAN_TXBUF_PTB, &can_txbuf_struct) != SUCCESS);
  
  /* transmit the primary transmit buffer */
  can_txbuf_transmit(CAN1, CAN_TRANSMIT_PTB);
}

/**
  *  @brief  can echo data
  *  @param  can receive buffer structure
  *  @retval none
  */
static void can_echo_data(const can_rxbuf_type* can_rxbuf_struct)
{
  can_txbuf_type  can_txbuf_struct = {0};
  uint8_t i;

  /* write the secondary transmit buffer */
  can_txbuf_struct.id = can_rxbuf_struct->id;
  can_txbuf_struct.id_type = can_rxbuf_struct->id_type;
  can_txbuf_struct.frame_type = can_rxbuf_struct->frame_type;
  can_txbuf_struct.data_length = can_rxbuf_struct->data_length;
  for(i=0; i<can_rxbuf_struct->data_length; i++)
  {
    can_txbuf_struct.data[i] = can_rxbuf_struct->data[i];
  }
  while(can_txbuf_write(CAN1, CAN_TXBUF_STB, &can_txbuf_struct) != SUCCESS);
  
  /* transmit all frames of the secondary transmit buffer */
  can_txbuf_transmit(CAN1, CAN_TRANSMIT_STB_ALL);
}

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  system_clock_config();
  at32_board_init();
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  
  /* turn on the led light */
  at32_led_on(LED4);

  /* enable pwc clock */
  crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, TRUE);

  can_gpio_config();
  if(can_configuration() == ERROR)
  {
    /* CAN clock initialization error */
    while(1)
    {
    }
  }
  
  can_transmit_data();
  while(1)
  {
    /* enable the can transceiver standby mode */
    while(can_standby_mode_enable(CAN1, TRUE) != SUCCESS);
    
    at32_led_off(LED4);
    /* enter sleep mode */
    pwc_sleep_mode_enter(PWC_SLEEP_ENTER_WFI);
    at32_led_on(LED4);
    
    while(can_rxbuf_read(CAN1, &can_rxbuf_struct) != ERROR)
    {
      can_echo_data(&can_rxbuf_struct);
    }
  }
}

/**
  * @}
  */

/**
  * @}
  */
