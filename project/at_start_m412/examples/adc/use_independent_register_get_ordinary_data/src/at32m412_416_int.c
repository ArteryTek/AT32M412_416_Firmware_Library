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

/** @addtogroup AT32M412_periph_examples
  * @{
  */

/** @addtogroup 412_ADC_use_independent_register_get_ordinary_data
  * @{
  */

extern __IO uint16_t adc1_ordinary_valuetab[3];
extern __IO uint16_t adc1_ordinary_conversion_flag;
extern __IO uint32_t adc1_overflow_flag;
extern __IO uint32_t adc1_conversion_fail_flag;

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
  }
  if(adc_interrupt_flag_get(ADC1, ADC_OCCO_FLAG) != RESET)
  {
    adc_flag_clear(ADC1, ADC_OCCO_FLAG);
    adc1_overflow_flag++;
  }
  if(adc_interrupt_flag_get(ADC1, ADC_OCCE_FLAG) != RESET)
  {
    adc_flag_clear(ADC1, ADC_OCCE_FLAG);
    adc1_ordinary_conversion_flag++;
    adc1_ordinary_valuetab[0] = adc_common_ordinary_data_get(ADC1, 1);
    adc1_ordinary_valuetab[1] = adc_common_ordinary_data_get(ADC1, 2);
    adc1_ordinary_valuetab[2] = adc_common_ordinary_data_get(ADC1, 3);
  }
}

/**
  * @}
  */

/**
  * @}
  */
