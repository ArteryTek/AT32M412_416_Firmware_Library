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

/** @addtogroup 412_OP_input_switch OP_input_switch
  * @{
  */


/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  gpio_init_type gpio_init_struct = {0};
  tmr_output_config_type tmr_oc_init_structure;
  uint32_t index = 0, cal_flag = 0;
	
  /* initial system clock */
  system_clock_config();

  /* at board initial */
  at32_board_init();
  
  /* initial uart for printf */
  uart_print_init(115200);
	
  /* scfg/dac/gpio/tmr1 clock enable */
  crm_periph_clock_enable(CRM_SCFG_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_DAC_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_TMR1_PERIPH_CLOCK, TRUE);
	
  /* initial gpio */
  gpio_init_struct.gpio_pins = GPIO_PINS_1 | GPIO_PINS_2;
  gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOA, &gpio_init_struct);
	
  /* initial dac1 */
  dac_reference_voltage_select(DAC1_SELECT, DAC_VDDA);
  dac_trigger_select(DAC1_SELECT, DAC_SOFTWARE_TRIGGER);
  dac_trigger_enable(DAC1_SELECT, TRUE);
  dac_output_enable(DAC1_SELECT, TRUE);
  dac_enable(DAC1_SELECT,TRUE);
	
  /* initial tmr1 cc6 */
  tmr_base_init(TMR1, system_core_clock/10000, 20);
  tmr_cnt_dir_set(TMR1, TMR_COUNT_UP);
  tmr_clock_source_div_set(TMR1, TMR_CLOCK_DIV1);
  
  tmr_output_default_para_init(&tmr_oc_init_structure);
  tmr_oc_init_structure.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;
  tmr_oc_init_structure.oc_idle_state = FALSE;
  tmr_oc_init_structure.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_oc_init_structure.oc_output_state = TRUE;
  tmr_output_channel_config(TMR1, TMR_SELECT_CHANNEL_6, &tmr_oc_init_structure);
  tmr_channel_value_set(TMR1, TMR_SELECT_CHANNEL_6, system_core_clock/10000/2);
  tmr_output_channel_buffer_enable(TMR1, TMR_SELECT_CHANNEL_6, TRUE);
  tmr_output_enable(TMR1, TRUE);	
  
  /* initial op1 in calibration mode */
  op_set_mode(OP1, OP_CALIBRATION_MODE);
  op_set_trimming_mode(OP1, OP_TRIMMING_USER);
  op_calibration_select(OP1, OP_TRIMMING_PMOS);
  op_enable(OP1, TRUE);
  /* must delay 1ms for calibration */
  delay_ms(1);
  /* calibration pmos */
  for(index=0; index<=0x1f; index++)
  {
    op_set_triming_value(OP1, OP_TRIMMING_PMOS, index);
	/* must delay 1ms for calibration */
    delay_ms(1);
    /* calibration is successful when calout switches from 1 to 0 */		
    if(op_calout_status_get(OP1)==RESET)
    {
      cal_flag=1;
      printf("pmos calibration ok.\r\n");
    }
    
    if(cal_flag==1)
      break;
  }
  cal_flag = 0;
  op_calibration_select(OP1, OP_TRIMMING_NMOS);
  /* must delay 1ms for calibration */
  delay_ms(1);
  /* calibration nmos */
  for(index=0; index<=0x1f; index++)
  {
    op_set_triming_value(OP1, OP_TRIMMING_NMOS, index);
	/* must delay 1ms for calibration */
    delay_ms(1);
    /* calibration  is successful when calout switches from 1 to 0 */		
    if(op_calout_status_get(OP1)==RESET)
    {
      cal_flag=1;
      printf("nmos calibration ok.\r\n");
    }
    
    if(cal_flag==1)
      break;
  }
	
  /* initial op1 */
  op_set_mode(OP1, OP_FUNCTIONAL_MODE);
  op_set_functional_mode(OP1, OP_FOLLOWER_MODE);
  /* default selection of the non inverting input: dac1 output */
  op_set_input_non_inverting(OP1, OP_NON_INVERTING_INPUT_INP3);
  /* secondary selection of the non inverting input */
  op_input_non_inverting_switch(OP1, OP_NON_INVERTING_INPUT_INP0);
  op_input_mux_enable(OP1, TRUE);	
  op_enable(OP1, TRUE);
	
  /* enable tmr1 */
  tmr_counter_enable(TMR1, TRUE);

  while(1)
  {
    /* dac generate a voltage waveform */
    delay_us(100);
    dac_1_data_set(0x10);
    dac_software_trigger_generate(DAC1_SELECT);
    delay_us(100);
    dac_1_data_set(0x0);
    dac_software_trigger_generate(DAC1_SELECT);
  }
}

/**
  * @}
  */

/**
  * @}
  */
