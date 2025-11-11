/**
  **************************************************************************
  * @file     main.c
  * @brief    main program
  **************************************************************************
  *
  * Copyright (c) 2025, Artery Technology, All rights reserved.
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

/** @addtogroup 416_TMR_brake_bidrectional TMR_brake_bidrectional
  * @{
  */

gpio_init_type  gpio_init_struct = {0};
tmr_output_config_type  tmr_output_struct;
crm_clocks_freq_type crm_clocks_freq_struct = {0};
tmr_brkdt_config_type tmr_brkdt_config_struct = {0};
tmr_brkin_config_type tmr_brkin_config_struct;

uint16_t timerperiod = 0;
uint16_t channel1pulse = 0, channel2pulse = 0, channel3pulse = 0, channel4pulse = 0;

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  system_clock_config();

  at32_board_init();
	
  uart_print_init(115200);

  /* get system clock */
  crm_clocks_freq_get(&crm_clocks_freq_struct);

  /* turn led2/led3/led4 on */
  at32_led_on(LED2);
  at32_led_on(LED3);
  at32_led_on(LED4);

  /* enable tmr1/gpioa/gpiob clock */
  crm_periph_clock_enable(CRM_TMR1_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

  /* timer1 output pin configuration */
  gpio_init_struct.gpio_pins = GPIO_PINS_8;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOA, &gpio_init_struct);

  gpio_init_struct.gpio_pins = GPIO_PINS_6;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOA, &gpio_init_struct);

  gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE6, GPIO_MUX_1);
  gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE8, GPIO_MUX_1);

  /* compute the value to be set in arr regiter to generate signal frequency at 100 khz */
  timerperiod = ((crm_clocks_freq_struct.apb2_freq) / 100000 ) - 1;
  /* compute ccr1 value to generate a duty cycle at 50% for channel 1 */
  channel1pulse = (uint16_t) (((uint32_t) 5 * (timerperiod - 1)) / 10);

  tmr_base_init(TMR1, timerperiod, 0);
  tmr_cnt_dir_set(TMR1, TMR_COUNT_UP);

  /* channel 1 Configuration in output mode */
  tmr_output_default_para_init(&tmr_output_struct);
  tmr_output_struct.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_B;
  tmr_output_struct.oc_output_state = TRUE;
  tmr_output_struct.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct.oc_idle_state = TRUE;
  tmr_output_channel_config(TMR1, TMR_SELECT_CHANNEL_1, &tmr_output_struct);
  tmr_channel_value_set(TMR1, TMR_SELECT_CHANNEL_1, channel1pulse);

  /* automatic output enable, stop, dead time and lock configuration*/
  tmr_brkdt_default_para_init(&tmr_brkdt_config_struct);
  tmr_brkdt_config_struct.brk_enable = TRUE;
  tmr_brkdt_config_struct.auto_output_enable = TRUE;
  tmr_brkdt_config_struct.deadtime = 0;
  tmr_brkdt_config_struct.fcsodis_state = TRUE;
  tmr_brkdt_config_struct.fcsoen_state = TRUE;
  tmr_brkdt_config_struct.brk_polarity = TMR_BRK_INPUT_ACTIVE_LOW;
  tmr_brkdt_config_struct.wp_level = TMR_WP_OFF;
  tmr_brkdt_config(TMR1, &tmr_brkdt_config_struct);

  tmr_brk_bidir_enable(TMR1, TMR_BRK_SELECT_1, TRUE);

  tmr_brkin_config_struct.enable = TRUE;
  tmr_brkin_config_struct.polarity = TMR_BRKIN_SRC_POL_HIGH;
  tmr_brkin_config_struct.source = TMR_BRKIN_SOURCE_BRKIN;
  tmr_brkin_config_struct.brk_sel = TMR_BRK_SELECT_1;
  tmr_brk_input_config(TMR1, &tmr_brkin_config_struct);

  /* tmr1 output enable */
  tmr_output_enable(TMR1, TRUE);
  /* enable tmr1 */
  tmr_counter_enable(TMR1, TRUE);

  while(1)
  {
    if(at32_button_press() == USER_BUTTON)
    {
      tmr_brk_release(TMR1, TMR_BRK_SELECT_1);
      tmr_flag_clear(TMR1, TMR_BRK_FLAG);
      while(tmr_brk_release_check(TMR1, TMR_BRK_SELECT_1) == TMR_BRK_UNRELEASED);
      printf("The brakes output have been released!\r\n");
    }
  }
}

/**
  * @}
  */

/**
  * @}
  */
