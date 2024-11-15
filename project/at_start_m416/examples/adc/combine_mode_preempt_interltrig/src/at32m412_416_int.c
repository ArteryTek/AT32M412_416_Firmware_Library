/**
  **************************************************************************
  * @file     at32m412_416_int.c
  * @brief    main interrupt service routines.
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

/* includes ------------------------------------------------------------------*/
#include "at32m412_416_int.h"

/** @addtogroup AT32M416_periph_examples
  * @{
  */

/** @addtogroup 416_ADC_combine_mode_preempt_interltrig
  * @{
  */

extern __IO uint16_t adc1_preempt_valuetab[2][3];
extern __IO uint16_t adc2_preempt_valuetab[2][3];
extern __IO uint16_t adc1_preempt_conversion_count;
extern __IO uint16_t adc2_preempt_conversion_count;
extern __IO uint32_t adc1_overflow_flag;
extern __IO uint32_t adc2_overflow_flag;
extern __IO uint32_t adc1_conversion_fail_flag;
extern __IO uint32_t adc2_conversion_fail_flag;

extern void adc_convert_recovery_process(void);

/**
  * @brief  this function handles nmi exception.
  * @param  none
  * @retval none
  */
void NMI_Handler(void)
{
}

/**
  * @brief  this function handles hard fault exception.
  * @param  none
  * @retval none
  */
void HardFault_Handler(void)
{
  /* go to infinite loop when hard fault exception occurs */
  while(1)
  {
  }
}

/**
  * @brief  this function handles memory manage exception.
  * @param  none
  * @retval none
  */
void MemManage_Handler(void)
{
  /* go to infinite loop when memory manage exception occurs */
  while(1)
  {
  }
}

/**
  * @brief  this function handles bus fault exception.
  * @param  none
  * @retval none
  */
void BusFault_Handler(void)
{
  /* go to infinite loop when bus fault exception occurs */
  while(1)
  {
  }
}

/**
  * @brief  this function handles usage fault exception.
  * @param  none
  * @retval none
  */
void UsageFault_Handler(void)
{
  /* go to infinite loop when usage fault exception occurs */
  while(1)
  {
  }
}

/**
  * @brief  this function handles svcall exception.
  * @param  none
  * @retval none
  */
void SVC_Handler(void)
{
}

/**
  * @brief  this function handles debug monitor exception.
  * @param  none
  * @retval none
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  this function handles pendsv_handler exception.
  * @param  none
  * @retval none
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  this function handles systick handler.
  * @param  none
  * @retval none
  */
void SysTick_Handler(void)
{
}

/**
  * @brief  this function handles adc1_2 handler.
  * @param  none
  * @retval none
  */
void ADC1_2_IRQHandler(void)
{
  if(adc_interrupt_flag_get(ADC1, ADC_TCF_FLAG) != RESET)
  {
    adc_flag_clear(ADC1, ADC_TCF_FLAG);
    adc1_conversion_fail_flag++;
		
    /* convert fail recovery process,ensure data accuracy */
    adc_convert_recovery_process();
  }
	
  if(adc_interrupt_flag_get(ADC1, ADC_OCCO_FLAG) != RESET)
  {
    adc_flag_clear(ADC1, ADC_OCCO_FLAG);
    adc1_overflow_flag++;
		
    /* overflow recovery process,ensure data accuracy */
    adc_convert_recovery_process();
  }
	
  if(adc_interrupt_flag_get(ADC2, ADC_TCF_FLAG) != RESET)
  {
    adc_flag_clear(ADC2, ADC_TCF_FLAG);
    adc2_conversion_fail_flag++;
		
    /* convert fail recovery process,ensure data accuracy */
    adc_convert_recovery_process();
  }
	
  if(adc_interrupt_flag_get(ADC2, ADC_OCCO_FLAG) != RESET)
  {
    adc_flag_clear(ADC2, ADC_OCCO_FLAG);
    adc2_overflow_flag++;
		
    /* overflow recovery process,ensure data accuracy */
    adc_convert_recovery_process();
  }
	
	if(adc_interrupt_flag_get(ADC1, ADC_PCCE_FLAG) != RESET)
  {
    adc_flag_clear(ADC1, ADC_PCCE_FLAG);
    if(adc1_preempt_conversion_count < 2)
    {
      adc1_preempt_valuetab[adc1_preempt_conversion_count][0] = adc_preempt_conversion_data_get(ADC1, ADC_PREEMPT_CHANNEL_1);
      adc1_preempt_valuetab[adc1_preempt_conversion_count][1] = adc_preempt_conversion_data_get(ADC1, ADC_PREEMPT_CHANNEL_2);
      adc1_preempt_valuetab[adc1_preempt_conversion_count][2] = adc_preempt_conversion_data_get(ADC1, ADC_PREEMPT_CHANNEL_3);
      adc1_preempt_conversion_count++;
    }
  }
	
  if(adc_interrupt_flag_get(ADC2, ADC_PCCE_FLAG) != RESET)
  {
    adc_flag_clear(ADC2, ADC_PCCE_FLAG);
    if(adc2_preempt_conversion_count < 2)
    {
      adc2_preempt_valuetab[adc2_preempt_conversion_count][0] = adc_preempt_conversion_data_get(ADC2, ADC_PREEMPT_CHANNEL_1);
      adc2_preempt_valuetab[adc2_preempt_conversion_count][1] = adc_preempt_conversion_data_get(ADC2, ADC_PREEMPT_CHANNEL_2);
      adc2_preempt_valuetab[adc2_preempt_conversion_count][2] = adc_preempt_conversion_data_get(ADC2, ADC_PREEMPT_CHANNEL_3);
      adc2_preempt_conversion_count++;
    }
  }
}

/**
  * @}
  */

/**
  * @}
  */

