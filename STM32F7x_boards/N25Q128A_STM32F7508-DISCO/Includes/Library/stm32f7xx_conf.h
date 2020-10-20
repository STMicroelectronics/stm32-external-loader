/**
  ******************************************************************************
  * @file    stm32f7xx_conf.h 
  * @author  MCD Application Team
  * @brief   Library configuration file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __stm32f7XX_CONF_H
#define __stm32f7XX_CONF_H

/* Includes ------------------------------------------------------------------*/
/* Comment the line below to disable peripheral header file inclusion */
//#include "stm32f7xx_adc.h"
//#include "stm32f7xx_can.h"
//#include "stm32f7xx_crc.h"
//#include "stm32f7xx_comp.h"
//#include "stm32f7xx_dac.h"
//#include "stm32f7xx_dbgmcu.h"
//#include "stm32f7xx_dma.h"
//#include "stm32f7xx_exti.h"
//#include "stm32f7xx_flash.h"
#include "stm32f7xx_gpio.h"
//#include "stm32f7xx_syscfg.h"
//#include "stm32f7xx_i2c.h"
//#include "stm32f7xx_iwdg.h"
//#include "stm32f7xx_pwr.h"
#include "stm32f7xx_rcc.h"
//#include "stm32f7xx_spi.h"
//#include "stm32f7xx_tim.h"
//#include "stm32f7xx_usart.h"
//#include "stm32f7xx_wwdg.h"
#include "stm32f7xx_quadspi.h"
//#include "misc.h"  /* High level functions for NVIC and SysTick (add-on to CMSIS functions) */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Uncomment the line below to expanse the "assert_param" macro in the 
   Standard Peripheral Library drivers code */
/* #define USE_FULL_ASSERT    1 */

/* Exported macro ------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT

/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function which reports 
  *         the name of the source file and the source line number of the call 
  *         that failed. If expr is true, it returns no value.
  * @retval None
  */
  #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */

#endif /* __stm32f7XX_CONF_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
