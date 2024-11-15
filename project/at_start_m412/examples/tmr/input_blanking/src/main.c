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

/** @addtogroup 412_TMR_input_blanking TMR_input_blanking
  * @{
  */

#define  ENABLE_BLANKING

gpio_init_type  gpio_init_struct = {0};
crm_clocks_freq_type crm_clocks_freq_struct = {0};

void gpio_config(void)
{
  gpio_init_type gpio_init_struct;
  
  /* enable gpioa clock */
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  
  gpio_init_struct.gpio_pins = GPIO_PINS_0 | GPIO_PINS_1 | GPIO_PINS_2 | GPIO_PINS_3| GPIO_PINS_4;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOA, &gpio_init_struct);
}

void tmr_blk_config(void)
{
  tmr_input_config_type   tmr_ic_init_struct;
  tmr_blk_config_type     tmr_blk_init_struct;
  tmr_output_config_type  tmr_oc_init_struct;

  /* enable tmr3/tmr10/gpioa clock */
  crm_periph_clock_enable(CRM_TMR3_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_TMR10_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  
  tmr_reset(TMR3);
  tmr_reset(TMR10);

  /*------------------------ config input_tmr tmr3 ------------------------*/
  /* tmr3_ic2(pa7) input pin Configuration */
  gpio_init_struct.gpio_pins = GPIO_PINS_7;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOA, &gpio_init_struct);
  gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE7, GPIO_MUX_2);
  
  tmr_base_init(TMR3, 0xFFFF, (crm_clocks_freq_struct.ahb_freq / 1000000) - 1);
  tmr_cnt_dir_set(TMR3, TMR_COUNT_UP);

  tmr_ic_init_struct.input_channel_select = TMR_SELECT_CHANNEL_2;
  tmr_ic_init_struct.input_mapped_select = TMR_CC_CHANNEL_MAPPED_DIRECT;
  tmr_ic_init_struct.input_polarity_select = TMR_INPUT_BOTH_EDGE;
  tmr_ic_init_struct.input_filter_value = 0x00;
  tmr_input_channel_init(TMR3, &tmr_ic_init_struct, TMR_CHANNEL_INPUT_DIV_1);

#ifdef  ENABLE_BLANKING
  tmr_blk_default_para_init(&tmr_blk_init_struct);
  tmr_blk_init_struct.input_channel_select = TMR_SELECT_CHANNEL_2;
  tmr_blk_init_struct.input_blk_state = TMR_BLK_STATE_HIGH;  /* tmr input channel state is high when blanking active */
  tmr_blk_init_struct.blk_source_select = TMR_BLK_SOURCE_3;  /* tmr blanking source from tmr10_oc1 */
  tmr_blk_init_struct.blk_source_polarity = TMR_BLK_SOURCE_ACTIVE_HIGH;  /* tmr blanking source active high */
  tmr_blk_init_struct.blk_enable = TRUE;
  tmr_input_channel_blk_config(TMR3, &tmr_blk_init_struct);
#endif

  nvic_irq_enable(TMR3_GLOBAL_IRQn, 0, 0);
  tmr_interrupt_enable(TMR3, TMR_C2_INT, TRUE);

  tmr_counter_enable(TMR3, TRUE);

  /*------------------------ config oc_tmr tmr10 ------------------------*/
  /* tmr10_oc1(pa12) output pin Configuration */
  gpio_init_struct.gpio_pins = GPIO_PINS_12;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOA, &gpio_init_struct);
  gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE12, GPIO_MUX_3);
  
  tmr_base_init(TMR10, 1000-1, (crm_clocks_freq_struct.ahb_freq / 10000) - 1); 
  tmr_cnt_dir_set(TMR10, TMR_COUNT_UP);

  tmr_oc_init_struct.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;
  tmr_oc_init_struct.oc_idle_state = FALSE;
  tmr_oc_init_struct.occ_idle_state = FALSE;
  tmr_oc_init_struct.oc_polarity = TMR_OUTPUT_ACTIVE_LOW;
  tmr_oc_init_struct.occ_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_oc_init_struct.oc_output_state = TRUE;
  tmr_oc_init_struct.occ_output_state = TRUE;
  tmr_output_channel_config(TMR10, TMR_SELECT_CHANNEL_1, &tmr_oc_init_struct);
  tmr_channel_value_set(TMR10, TMR_SELECT_CHANNEL_1, 500);
  tmr_output_channel_buffer_enable(TMR10, TMR_SELECT_CHANNEL_1, TRUE);

  tmr_counter_enable(TMR10, TRUE);
  tmr_output_enable(TMR10, TRUE);
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

  /* get system clock */
  crm_clocks_freq_get(&crm_clocks_freq_struct);
  
  /* used for test */
  gpio_config();
  
  /* config tmr3 input blanking and tmr10 blanking source */
  tmr_blk_config();
  
  /* turn led4 on */
  at32_led_on(LED4);

  while(1)
  {
    delay_ms(10);
   
    /* connect simulation external signal(pa0) to tmr3_ic2(pa7) */
    gpio_bits_toggle(GPIOA, GPIO_PINS_0);  
  }
}

/**
  * @}
  */

/**
  * @}
  */
