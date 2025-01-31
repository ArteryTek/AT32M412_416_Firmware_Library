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

/** @addtogroup 412_CMP_cycle_by_cycle_control CMP_cycle_by_cycle_control
  * @{
  */

/**
  * @brief  configures cmp
  * @param  none
  * @retval none
  */
void cmp_config(void)
{
  cmp_init_type cmp_init_struct;
  gpio_init_type gpio_init_struct;

  /* gpioa peripheral clock enable */
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

  /* configure pa1: pa1 is used as cmp1 non inveting input */
  gpio_init_struct.gpio_pins = GPIO_PINS_1;
  gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOA, &gpio_init_struct);

  /* configure dac1: dac1 is used as cmp1 inveting input */
  crm_periph_clock_enable(CRM_DAC_PERIPH_CLOCK, TRUE);
  dac_1_data_set(0x27);
  dac_enable(DAC1_SELECT, TRUE);
  
  gpio_init_struct.gpio_pins = GPIO_PINS_0;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init(GPIOA, &gpio_init_struct);

  gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE0, GPIO_MUX_12);

  /* cmp peripheral clock enable */
  crm_periph_clock_enable(CRM_SCFG_PERIPH_CLOCK, TRUE);

  /* cmp1 init: pa1 is used cmp1 inverting input */
  cmp_default_para_init(&cmp_init_struct);
  cmp_init_struct.cmp_non_inverting = CMP_NON_INVERTING_INP1;
  cmp_init_struct.cmp_inverting = CMP_INVERTING_INM2;
  cmp_init_struct.cmp_polarity = CMP_POL_NON_INVERTING;
  cmp_init_struct.cmp_speed = CMP_SPEED_FAST;
  cmp_init_struct.cmp_hysteresis = CMP_HYSTERESIS_NONE;
  cmp_init(CMP1_SELECTION, &cmp_init_struct);

  /* enable cmp1 */
  cmp_enable(CMP1_SELECTION, TRUE);
}

/**
  * @brief  configures tmr1_ch1 as input
  * @param  none
  * @retval none
  */
void tmr1_init(void)
{
  gpio_init_type gpio_init_struct;
  tmr_output_config_type tmr_output_structure;

  crm_periph_clock_enable(CRM_TMR1_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

  gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE8, GPIO_MUX_1);

  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_pins = GPIO_PINS_8;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOA, &gpio_init_struct);

  /* tmr1 time base configuration */
  tmr_base_init(TMR1, 9999, 0);
  tmr_cnt_dir_set(TMR1, TMR_COUNT_UP);
  tmr_clock_source_div_set(TMR1, TMR_CLOCK_DIV1);

  /* channel 4 configuration in pwm mode */
  tmr_output_default_para_init(&tmr_output_structure);
  
  tmr_output_structure.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;
  tmr_output_structure.oc_idle_state = FALSE;
  tmr_output_structure.occ_idle_state = FALSE;
  tmr_output_structure.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_structure.occ_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_structure.oc_output_state = TRUE;
  tmr_output_structure.occ_output_state = FALSE;
  tmr_output_channel_config(TMR1, TMR_SELECT_CHANNEL_1, &tmr_output_structure);
  tmr_channel_value_set(TMR1, TMR_SELECT_CHANNEL_1, 5000);
    
  tmr_coraw_clear_set(TMR1, TMR_CORAW_CLR_CMP1);
  
  tmr_output_channel_switch_set(TMR1, TMR_SELECT_CHANNEL_1, TRUE);
  
  /* tmr1 output enable */
  tmr_output_enable(TMR1, TRUE);
  
  /* tmr1 counter enable */
  tmr_counter_enable(TMR1, TRUE);
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

  /* cmp1 configuration */
  cmp_config();

  /* tmr1 configuration in input capture mode */
  tmr1_init();

  while (1)
  {
  }
}

/**
  * @}
  */

/**
  * @}
  */
