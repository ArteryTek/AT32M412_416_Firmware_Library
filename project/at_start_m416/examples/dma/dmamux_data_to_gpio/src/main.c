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

/** @addtogroup AT32M416_periph_examples
  * @{
  */

/** @addtogroup 416_DMA_data_to_gpio DMA_data_to_gpio
  * @{
  */

gpio_init_type  gpio_init_struct = {0};
dma_init_type dma_init_struct;

#define BUFFER_SIZE                      16
uint16_t src_buffer[BUFFER_SIZE] = {0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008,
                                    0x0009, 0x000a, 0x000b, 0x000c, 0x000d, 0x000e, 0x000f, 0x0010};

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  /* initial system clock */
  system_clock_config();

  /* at board initial */
  at32_board_init();

  /* enable dma1/gpiob/tmr3 clock */
  crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_TMR3_PERIPH_CLOCK, TRUE);

  /* config gpiob pin for output mode */
  gpio_init_struct.gpio_pins = GPIO_PINS_ALL;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOB, &gpio_init_struct);

  tmr_base_init(TMR3, 0xFF, 0);
  tmr_cnt_dir_set(TMR3, TMR_COUNT_UP);

  /* enable tmr3 overflow dma request */
  tmr_dma_request_enable(TMR3, TMR_OVERFLOW_DMA_REQUEST, TRUE);

  /* dma1 channel1 configuration */
  dma_reset(DMA1_CHANNEL1);
  dma_init_struct.buffer_size = BUFFER_SIZE;
  dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
  dma_init_struct.memory_base_addr = (uint32_t)src_buffer;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_base_addr = (uint32_t)&GPIOB->odt;
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_MEDIUM;
  dma_init_struct.loop_mode_enable = FALSE;
  dma_init(DMA1_CHANNEL1, &dma_init_struct);

  /* enable transfer full data interrupt */
  dma_interrupt_enable(DMA1_CHANNEL1, DMA_FDT_INT, TRUE);

  /* DMA1 channel1 interrupt nvic init */
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  nvic_irq_enable(DMA1_Channel1_IRQn, 1, 0);

  /* TMR3 dmamux function enable */
  dmamux_enable(DMA1, TRUE);
  dmamux_init(DMA1MUX_CHANNEL1, DMAMUX_DMAREQ_ID_TMR3_OVERFLOW);

  /* enable dma channel */
  dma_channel_enable(DMA1_CHANNEL1, TRUE);

  /* enable TMR3 */
  tmr_counter_enable(TMR3, TRUE);

  while(1)
  {
  }
}

/**
  * @}
  */

/**
  * @}
  */


