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

/** @addtogroup 416_ADC_op_as_adc_channel ADC_op_as_adc_channel
  * @{
  */

#define DMA_BUFFER_SIZE                  4

__IO uint16_t adc1_ordinary_valuetab[4] = {0};
__IO uint16_t dma_trans_complete_flag = 0;
__IO uint32_t adc1_overflow_flag = 0;
__IO uint32_t adc1_conversion_fail_flag = 0;
__IO uint16_t adc_opa_value[4] = {0};

static void gpio_config(void);
static void dma_config(void);
static void adc_config(void);
void op_channel_config(uint16_t channel);

/**
  * @brief  gpio configuration.
  * @param  none
  * @retval none
  */
static void gpio_config(void)
{
  gpio_init_type gpio_initstructure;
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

  gpio_default_para_init(&gpio_initstructure);

  /* config adc pin as analog input mode */
  gpio_initstructure.gpio_mode = GPIO_MODE_ANALOG;
  gpio_initstructure.gpio_pins = GPIO_PINS_1 | GPIO_PINS_7;
  gpio_init(GPIOA, &gpio_initstructure);
	
  gpio_initstructure.gpio_pins = GPIO_PINS_0 | GPIO_PINS_13;
  gpio_init(GPIOB, &gpio_initstructure);
}

/**
  * @brief  dma configuration.
  * @param  none
  * @retval none
  */
static void dma_config(void)
{
  dma_init_type dma_init_struct;
  crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
  nvic_irq_enable(DMA1_Channel1_IRQn, 0, 0);

  dma_reset(DMA1_CHANNEL1);
  dma_default_para_init(&dma_init_struct);
  dma_init_struct.buffer_size = DMA_BUFFER_SIZE;
  dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;
  dma_init_struct.memory_base_addr = (uint32_t)adc1_ordinary_valuetab;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_base_addr = (uint32_t)&(ADC1->odt);
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_HIGH;
  dma_init_struct.loop_mode_enable = TRUE;
  dma_init(DMA1_CHANNEL1, &dma_init_struct);

  dmamux_enable(DMA1, TRUE);
  dmamux_init(DMA1MUX_CHANNEL1, DMAMUX_DMAREQ_ID_ADC1);

  /* enable dma transfer complete interrupt */
  dma_interrupt_enable(DMA1_CHANNEL1, DMA_FDT_INT, TRUE);
  dma_channel_enable(DMA1_CHANNEL1, TRUE);
}

/**
  * @brief  adc configuration.
  * @param  none
  * @retval none
  */
static void adc_config(void)
{
  adc_common_config_type adc_common_struct;
  adc_base_config_type adc_base_struct;
  crm_periph_clock_enable(CRM_ADC1_PERIPH_CLOCK, TRUE);
  nvic_irq_enable(ADC1_2_IRQn, 0, 0);

  adc_common_default_para_init(&adc_common_struct);

  /* config combine mode */
  adc_common_struct.combine_mode = ADC_INDEPENDENT_MODE;

  /* config division,adcclk is division by hclk */
  adc_common_struct.div = ADC_HCLK_DIV_6;

  /* config common dma mode,it's not useful in independent mode */
  adc_common_struct.common_dma_mode = ADC_COMMON_DMAMODE_DISABLE;

  /* config common dma request repeat */
  adc_common_struct.common_dma_request_repeat_state = FALSE;

  /* config adjacent adc sampling interval,it's useful for ordinary shifting mode */
  adc_common_struct.sampling_interval = ADC_SAMPLING_INTERVAL_4CYCLES;

  /* config inner temperature sensor and vintrv */
  adc_common_struct.tempervintrv_state = FALSE;
  adc_common_config(&adc_common_struct);

  adc_base_default_para_init(&adc_base_struct);

  adc_base_struct.sequence_mode = TRUE;
  adc_base_struct.repeat_mode = FALSE;
  adc_base_struct.data_align = ADC_RIGHT_ALIGNMENT;
  adc_base_struct.ordinary_channel_length = 4;
  adc_base_config(ADC1, &adc_base_struct);
  adc_resolution_set(ADC1, ADC_RESOLUTION_12B);

  /* config ordinary channel */
  adc_ordinary_channel_set(ADC1, ADC_CHANNEL_19, 1, ADC_SAMPLETIME_41_5);
  adc_ordinary_channel_set(ADC1, ADC_CHANNEL_20, 2, ADC_SAMPLETIME_41_5);
  adc_ordinary_channel_set(ADC1, ADC_CHANNEL_21, 3, ADC_SAMPLETIME_41_5);
  adc_ordinary_channel_set(ADC1, ADC_CHANNEL_22, 4, ADC_SAMPLETIME_41_5);

  /* config ordinary trigger source and trigger edge */
  adc_ordinary_conversion_trigger_set(ADC1, ADC_ORDINARY_TRIG_TMR1CH1, ADC_ORDINARY_TRIG_EDGE_NONE);

  /* config dma mode,it's not useful when common dma mode is use */
  adc_dma_mode_enable(ADC1, TRUE);

  /* config dma request repeat,it's not useful when common dma mode is use */
  adc_dma_request_repeat_enable(ADC1, TRUE);

  /* enable adc overflow interrupt */
  adc_interrupt_enable(ADC1, ADC_OCCO_INT, TRUE);
	
  /* enable adc trigger convert fail interrupt */
  adc_interrupt_enable(ADC1, ADC_TCF_INT, TRUE);
	
  /* enable adc trigger conversion fail auto conversion abort */
  adc_convert_fail_auto_abort_enable(ADC1, TRUE);

  /* adc enable */
  adc_enable(ADC1, TRUE);
  while(adc_flag_get(ADC1, ADC_RDY_FLAG) == RESET);

  /* adc calibration */
  adc_calibration_init(ADC1);
  while(adc_calibration_init_status_get(ADC1));
  adc_calibration_start(ADC1);
  while(adc_calibration_status_get(ADC1));
}

/**
  * @brief  op_channel config with 8 multiple gain.
  * @param  channel: op to be configuration
  * @retval none
  */
void op_channel_config(uint16_t channel)
{
  uint32_t index = 0;
  uint16_t data = 0;
  uint16_t flag = 0;
  op_type *opa_channel_sel;
  switch(channel)
  {
    case 1: 
      opa_channel_sel = OP1;
    break;
    case 2: 
      opa_channel_sel = OP2;
    break;
    case 3: 
      opa_channel_sel = OP3;
    break;
    case 4: 
      opa_channel_sel = OP4;
    break;
    default:break;
  }
  dma_trans_complete_flag = 0;
  op_set_mode(opa_channel_sel, OP_CALIBRATION_MODE);
  printf("PMOS Calibration begin:\r\n");
	
	/* op internal output adc */
  op_internal_output_select(opa_channel_sel, OP_INTERNAL_OUTPUT_ADC);
  
  op_set_trimming_mode(opa_channel_sel, OP_TRIMMING_USER);

  op_calibration_select(opa_channel_sel, OP_TRIMMING_PMOS);
  op_enable(opa_channel_sel, TRUE);
  delay_us(5);
  for(index=0; index<=0x1f; index++)
  {
    op_set_triming_value(opa_channel_sel, OP_TRIMMING_PMOS, index);
    delay_us(5);    
    
    /* adc trigger */
    adc_ordinary_software_trigger_enable(ADC1, TRUE);
    while(dma_trans_complete_flag == 0);
    switch(channel)
    {
      case 1: 
        data = adc_opa_value[0];
      break;
      case 2: 
        data = adc_opa_value[1];
      break;
      case 3: 
        data = adc_opa_value[2];
      break;
      case 4: 
        data = adc_opa_value[3];
      break;
      default:break;
    }
    dma_trans_complete_flag = 0;
    if(data==0)
    {
      /*calibration is successful when calout switches from 1 to 0.*/
      printf("PMOS Calibration OK\r\n");
      
      flag =1;
      break;
    }
    if(flag==1)
      break;
  }
  flag = 0;
  op_calibration_select(opa_channel_sel, OP_TRIMMING_NMOS);
  printf("NMOS Calibration begin:\r\n");
  delay_us(5);
  for(index=0; index<=0x1f; index++)
  {
    op_set_triming_value(opa_channel_sel, OP_TRIMMING_NMOS, index);
    delay_us(5);
		
    /* adc trigger */
    adc_ordinary_software_trigger_enable(ADC1, TRUE);
    while(dma_trans_complete_flag == 0);
    switch(channel)
    {
      case 1: 
        data = adc_opa_value[0];
      break;
      case 2: 
        data = adc_opa_value[1];
      break;
      case 3: 
        data = adc_opa_value[2];
      break;
      case 4: 
        data = adc_opa_value[3];
      break;
      default:break;
    }
    while(data==0)
    {
      printf("NMOS Calibration OK\r\n");
      op_enable(opa_channel_sel, FALSE);
      op_set_mode(opa_channel_sel, OP_FUNCTIONAL_MODE);
      op_set_functional_mode(opa_channel_sel, OP_PGA_MODE);
      op_set_input_non_inverting(opa_channel_sel, OP_NON_INVERTING_INPUT_INP0);
      op_set_gain(opa_channel_sel, OP_PGA_GAIN_8_OR_MINUS_7);
      op_enable(opa_channel_sel, TRUE); 
      flag =1;
      break;
    }
    dma_trans_complete_flag = 0;
    if(flag==1)
      break;
  }
  dma_trans_complete_flag = 0;
}

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  __IO uint32_t index = 0;
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);

  /* config the system clock */
  system_clock_config();

  /* init at start board */
  at32_board_init();
  at32_led_off(LED2);
  at32_led_off(LED3);
  at32_led_off(LED4);
  uart_print_init(115200);
	printf("op_as_adc_channel \r\n");
  gpio_config();
  dma_config();
  adc_config();
  crm_periph_clock_enable(CRM_SCFG_PERIPH_CLOCK, TRUE);
  op_channel_config(1);
  op_channel_config(2);
  op_channel_config(3);
  op_channel_config(4);
  while(1)
  {
    adc_ordinary_software_trigger_enable(ADC1, TRUE);
    delay_ms(100);
    if((dma_trans_complete_flag == 0) || (adc1_overflow_flag != 0) || (adc1_conversion_fail_flag != 0))
    {
      /* printf flag when error occur */
      at32_led_on(LED3);
      at32_led_on(LED4);
      printf("error occur\r\n");
      printf("adc1_overflow_flag = %d\r\n",adc1_overflow_flag);
      printf("dma_trans_complete_flag = %d\r\n",dma_trans_complete_flag);
      printf("adc1_conversion_fail_flag = %d\r\n",adc1_conversion_fail_flag);
    }
    else
    {
      /* printf data when conversion end without error */
      printf("conversion end without error\r\n");
      printf("adc1_ordinary_valuetab[0] = 0x%x\r\n", adc1_ordinary_valuetab[0]);
      printf("adc1_ordinary_valuetab[1] = 0x%x\r\n", adc1_ordinary_valuetab[1]);
      printf("adc1_ordinary_valuetab[2] = 0x%x\r\n", adc1_ordinary_valuetab[2]);
      printf("adc1_ordinary_valuetab[3] = 0x%x\r\n", adc1_ordinary_valuetab[3]);
      printf("\r\n");
    }
    dma_trans_complete_flag = 0;
    at32_led_toggle(LED2);
    delay_ms(1000);
  }
}

/**
  * @}
  */

/**
  * @}
  */
