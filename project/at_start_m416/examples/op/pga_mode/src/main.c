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

/** @addtogroup 416_OP_pga_mode OP_pga_mode
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
  uint32_t index = 0, cal_flag = 0;
	
  /* initial system clock */
  system_clock_config();

  /* at board initial */
  at32_board_init();
  
  /* initial uart for printf */
  uart_print_init(115200);
	
  /* scfg/dac/gpio clock enable */
  crm_periph_clock_enable(CRM_SCFG_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
	
  /* initial gpio */
  gpio_init_struct.gpio_pins = GPIO_PINS_0 | GPIO_PINS_1;
  gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOB, &gpio_init_struct);
	
  /* initial op3 in calibration mode */
  op_set_mode(OP3, OP_CALIBRATION_MODE);
  op_set_trimming_mode(OP3, OP_TRIMMING_USER);
  op_calibration_select(OP3, OP_TRIMMING_PMOS);
  op_enable(OP3, TRUE);
  /* must delay 1ms for calibration */
  delay_ms(1);
  /* calibration pmos */
  for(index=0; index<=0x1f; index++)
  {
    op_set_triming_value(OP3, OP_TRIMMING_PMOS, index);
	/* must delay 1ms for calibration */
    delay_ms(1);
    /* calibration is successful when calout switches from 1 to 0 */		
    if(op_calout_status_get(OP3)==RESET)
    {
      cal_flag=1;
      printf("pmos calibration ok.\r\n");
    }
    
    if(cal_flag==1)
      break;
  }
  cal_flag = 0;
  op_calibration_select(OP3, OP_TRIMMING_NMOS);
  /* must delay 1ms for calibration */
  delay_ms(1);
  /* calibration nmos */
  for(index=0; index<=0x1f; index++)
  {
    op_set_triming_value(OP3, OP_TRIMMING_NMOS, index);
	/* must delay 1ms for calibration */
    delay_ms(1);
    /* calibration  is successful when calout switches from 1 to 0 */		
    if(op_calout_status_get(OP3)==RESET)
    {
      cal_flag=1;
      printf("nmos calibration ok.\r\n");
    }
    
    if(cal_flag==1)
      break;
  }
	
  /* initial op3 in programmable gain amplifier mode */
  op_set_mode(OP3, OP_FUNCTIONAL_MODE);
  op_set_functional_mode(OP3, OP_PGA_MODE);
  op_set_input_non_inverting(OP3, OP_NON_INVERTING_INPUT_INP0);
  op_set_gain(OP3, OP_PGA_GAIN_4_OR_MINUS_3);
  op_enable(OP3, TRUE);
	
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
