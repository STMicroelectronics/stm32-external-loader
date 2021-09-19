/**
  ******************************************************************************
  * @file    b_u585i_iot02_conf_template.h
  * @author  MCD Application Team
  * @brief   configuration file.
  *          This file should be copied to the application folder and renamed
  *          to b_u585i_iot02_conf.h
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
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef B_U585I_IOT02_CONF_H
#define B_U585I_IOT02_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"

/* COM define */
#define USE_BSP_COM_FEATURE                  1U
#define USE_COM_LOG                          0U

/* IRQ priorities */
#define BSP_BUTTON_USER_IT_PRIORITY          0x0FUL

/* I2C1 and I2C2 Frequencies in Hz  */
#define BUS_I2C1_FREQUENCY                   100000UL /* Frequency of I2C1 = 100 KHz*/
#define BUS_I2C2_FREQUENCY                   100000UL /* Frequency of I2C2 = 100 KHz*/

#ifdef __cplusplus
}
#endif

#endif /* B_U585I_IOT02_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
