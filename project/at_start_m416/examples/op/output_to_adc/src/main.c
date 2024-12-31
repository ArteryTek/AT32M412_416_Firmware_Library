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

/** @addtogroup 416_OP_output_to_adc OP_output_to_adc
  * @{
  */


/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  gpio_init_type gpio_init_struct;
  adc_base_config_type adc_base_struct;
  adc_common_config_type adc_common_struct;
  uint32_t index = 0, cal_flag = 0;
  uint32_t adc_data = 0;
	
  /* initial system clock */
  system_clock_config();

  /* at board initial */
  at32_board_init();
  
  /* initial uart for printf */
  uart_print_init(115200);
	
  /* scfg/dac/gpio clock enable */
  crm_periph_clock_enable(CRM_SCFG_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_ADC1_PERIPH_CLOCK, TRUE);
	
  /* initial gpio */
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_pins = GPIO_PINS_7;
  gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOA, &gpio_init_struct);
	
  /* config combine mode */
  adc_common_struct.combine_mode = ADC_INDEPENDENT_MODE;

  /* config division,adcclk is division by hclk */
  adc_common_struct.div = ADC_HCLK_DIV_5;

  /* config common dma mode,it's not useful in independent mode */
  adc_common_struct.common_dma_mode = ADC_COMMON_DMAMODE_DISABLE;

  /* config common dma request repeat */
  adc_common_struct.common_dma_request_repeat_state = FALSE;

  /* config adjacent adc sampling interval,it's useful for ordinary shifting mode */
  adc_common_struct.sampling_interval = ADC_SAMPLING_INTERVAL_5CYCLES;

  /* config inner temperature sensor and vintrv */
  adc_common_struct.tempervintrv_state = FALSE;
  adc_common_config(&adc_common_struct);
	
  adc_base_default_para_init(&adc_base_struct);
  adc_base_struct.sequence_mode = TRUE;
  adc_base_struct.repeat_mode = FALSE;
  adc_base_struct.data_align = ADC_RIGHT_ALIGNMENT;
  adc_base_struct.ordinary_channel_length = 1;
  adc_base_config(ADC1, &adc_base_struct);
  adc_resolution_set(ADC1, ADC_RESOLUTION_12B);
  adc_ordinary_channel_set(ADC1, ADC_CHANNEL_20, 1, ADC_SAMPLETIME_41_5);
	
  /* adc enable */
  adc_enable(ADC1, TRUE);
  while(adc_flag_get(ADC1, ADC_RDY_FLAG) == RESET);

  /* adc calibration */
  adc_calibration_init(ADC1);
  while(adc_calibration_init_status_get(ADC1));
  adc_calibration_start(ADC1);
  while(adc_calibration_status_get(ADC1));
	
  /* op2 out connect to adc1 channel21 */
  op_internal_output_select(OP2, OP_INTERNAL_OUTPUT_ADC);
	
  /* initial OP2 in calibration mode */
  op_set_mode(OP2, OP_CALIBRATION_MODE);
  op_set_trimming_mode(OP2, OP_TRIMMING_USER);
  op_calibration_select(OP2, OP_TRIMMING_PMOS);
  op_enable(OP2, TRUE);
  /* must delay 1ms for calibration */
  delay_ms(1);
  /* calibration pmos */
  for(index=0; index<=0x1f; index++)
  {
    op_set_triming_value(OP2, OP_TRIMMING_PMOS, index);
    /* must delay 1ms for calibration */
    delay_ms(1);
    adc_ordinary_software_trigger_enable(ADC1, TRUE);
    while(!adc_flag_get(ADC1, ADC_OCCE_FLAG));
    adc_data = adc_ordinary_conversion_data_get(ADC1);
    /* calibration is successful when adc conversion data is close to the minimum adc output */		
    if(adc_data<64)
    {
      cal_flag=1;
		   	printf("pmos calibration ok.\r\n");
    }
    
    if(cal_flag==1)
      break;
  }
  cal_flag = 0;
  op_calibration_select(OP2, OP_TRIMMING_NMOS);
  /* must delay 1ms for calibration */
  delay_ms(1);
  /* calibration nmos */
  for(index=0; index<=0x1f; index++)
  {
    op_set_triming_value(OP2, OP_TRIMMING_NMOS, index);
    /* must delay 1ms for calibration */
    delay_ms(1);
	  	adc_ordinary_software_trigger_enable(ADC1, TRUE);
    while(!adc_flag_get(ADC1, ADC_OCCE_FLAG));
    adc_data = adc_ordinary_conversion_data_get(ADC1);
    /* calibration is successful when adc conversion data is close to the minimum adc output */			
    if(adc_data<64)
    {
      cal_flag=1;
      printf("nmos calibration ok.\r\n");
    }
    
    if(cal_flag==1)
      break;
  }

  /* initial op2 in programmable gain amplifier mode */
  op_enable(OP2, FALSE);
  op_set_mode(OP2, OP_FUNCTIONAL_MODE);
  op_set_functional_mode(OP2, OP_PGA_MODE);
  op_set_input_non_inverting(OP2, OP_NON_INVERTING_INPUT_INP0);
  op_set_gain(OP2, OP_PGA_GAIN_4_OR_MINUS_3);
  op_enable(OP2, TRUE);
	
  while(1)
  {
    delay_ms(500);
    adc_ordinary_software_trigger_enable(ADC1, TRUE);
    while(!adc_flag_get(ADC1, ADC_OCCE_FLAG));
    adc_data = adc_ordinary_conversion_data_get(ADC1);
    printf("adc value: %fv \r\n",3.3/4096*adc_data);
  }
}

/**
  * @}
  */

/**
  * @}
  */
