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

/** @addtogroup 416_PWC_deepsleep_ertc_tamper PWC_deepsleep_ertc_tamper
  * @{
  */

/**
  * @brief  configure the ertc clock source.
  * @param  none
  * @retval none
  */
void ertc_clock_config(void)
{
  /* enable the pwc clock interface */
  crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, TRUE);

  /* allow access to bpr domain */
  pwc_battery_powered_domain_access(TRUE);

  /* reset ertc domain */
  crm_battery_powered_domain_reset(TRUE);
  crm_battery_powered_domain_reset(FALSE);

  /* enable the lext osc */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_LEXT, TRUE);

  /* wait till lext is ready */
  while(crm_flag_get(CRM_LEXT_STABLE_FLAG) == RESET)
  {
  }

  /* select the ertc clock source */
  crm_ertc_clock_select(CRM_ERTC_CLOCK_LEXT);

  /* enable the ertc clock */
  crm_ertc_clock_enable(TRUE);
}

/**
  * @brief  ertc tamper configuration.
  * @param  none
  * @retval none
  */
void ertc_tamper_config(void)
{
  exint_init_type exint_init_struct;

  /* config the exint line of the ertc tamper */
  exint_init_struct.line_select   = EXINT_LINE_19;
  exint_init_struct.line_enable   = TRUE;
  exint_init_struct.line_mode     = EXINT_LINE_INTERRUPT;
  exint_init_struct.line_polarity = EXINT_TRIGGER_RISING_EDGE;
  exint_init(&exint_init_struct);

  /* enable tamper irqchannel */
  nvic_irq_enable(TAMP_STAMP_IRQn, 0, 0);

  /* disable the tamper detection */
  ertc_tamper_enable(ERTC_TAMPER_2, FALSE);

  /* clear tamper pin event(tampxf) pending flag */
  ertc_flag_clear(ERTC_TP2F_FLAG);

  /* configure the tamper trigger */
  ertc_tamper_valid_edge_set(ERTC_TAMPER_2, ERTC_TAMPER_EDGE_RISING);

  /* enable the tamper interrupt */
  ertc_interrupt_enable(ERTC_TP_INT, TRUE);

  /* enable the tamper detection */
  ertc_tamper_enable(ERTC_TAMPER_2, TRUE);
}

/**
  * @brief  system clock recover.
  * @param  none
  * @retval none
  */
void system_clock_recover(void)
{
  /* enable external high-speed crystal oscillator - hext */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_HEXT, TRUE);

  /* wait till hext is ready */
  while(crm_hext_stable_wait() == ERROR);

  /* enable pll */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_PLL, TRUE);

  /* wait till pll is ready */
  while(crm_flag_get(CRM_PLL_STABLE_FLAG) == RESET);

  /* enable auto step mode */
  crm_auto_step_mode_enable(TRUE);

  /* select pll as system clock source */
  crm_sysclk_switch(CRM_SCLK_PLL);

  /* wait till pll is used as system clock source */
  while(crm_sysclk_switch_status_get() != CRM_SCLK_PLL);
	
	/* disable auto step mode */
  crm_auto_step_mode_enable(FALSE);
}

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  ertc_clock_config();
  
  /* congfig the system clock */
  system_clock_config();

  /* init at start board */
  at32_board_init();

  /* config priority group */
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);

  /* turn on the led light */
  at32_led_on(LED2);
  at32_led_on(LED3);
  at32_led_on(LED4);

  /* config ertc or other operations of battery powered domain */
  ertc_tamper_config();
  
  while(1)
  {
    /* turn off the led light */
    at32_led_off(LED2);

    /* select system clock source as hick before ldo set */
    crm_sysclk_switch(CRM_SCLK_HICK);

    /* wait till hick is used as system clock source */
    while(crm_sysclk_switch_status_get() != CRM_SCLK_HICK)
    {
    }

    /* reduce ldo before enter deepsleep mode */
    pwc_ldo_output_voltage_set(PWC_LDO_OUTPUT_1V2);

    /* congfig the voltage regulator mode */
    pwc_voltage_regulate_set(PWC_REGULATOR_EXTRA_LOW_POWER);

    /* enter deep sleep mode */
    pwc_deep_sleep_mode_enter(PWC_DEEP_SLEEP_ENTER_WFI);

    /* turn on the led light */
    at32_led_on(LED2);
    
    /* determine if the debugging function is enabled */
    if((DEBUGMCU->ctrl & 0x00000007) != 0x00000000)
    {
      /* wait 3 LICK cycles to ensure clock stable */
      delay_us(6);
    }

    /* resume ldo before system clock source enhance */
    pwc_ldo_output_voltage_set(PWC_LDO_OUTPUT_1V3);

    /* congfig the system clock */
    system_clock_recover();
    
    delay_ms(500);
  }
}

/**
  * @}
  */

/**
  * @}
  */
