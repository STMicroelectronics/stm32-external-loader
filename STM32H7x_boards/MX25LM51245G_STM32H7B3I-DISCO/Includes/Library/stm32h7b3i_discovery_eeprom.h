/**
  ******************************************************************************
  * @file    stm32h7b3i_discovery_eeprom.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for
  *          the stm32h7b3i_discovery_eeprom.c firmware driver.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
#ifndef STM32H7B3I_DISCO_EEPROM_H
#define STM32H7B3I_DISCO_EEPROM_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7b3i_discovery_conf.h"
#include "stm32h7b3i_discovery_errno.h"
#include "../Components/m24lr64/m24lr64.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32H7B3I_DISCO
  * @{
  */

/** @addtogroup STM32H7B3I_DISCO_EEPROM
  * @brief This file includes the I2C EEPROM driver of STM32H7B3I_DISCO board.
  * @{
  */

/** @defgroup STM32H7B3I_DISCO_EEPROM_Exported_Types Exported Types
  * @{
  */
/**
  * @}
  */

/** @defgroup STM32H7B3I_DISCO_EEPROM_Exported_Constants Exported Constants
  * @{
  */
#define EEPROM_INSTANCES_NBR       1U

/* EEPROM hardware address and page size */
#define EEPROM_PAGESIZE             4U
#define EEPROM_MAX_SIZE             0x2000U

#define EEPROM_I2C_ADDRESS_A01           0xA0U
#define EEPROM_I2C_ADDRESS_A02           0xA6U

/**
  * @}
  */
/** @addtogroup STM32H7B3I_DISCO_EEPROM_Exported_Variables
  * @{
  */
extern void  *Eeprom_CompObj;
/**
  * @}
  */

/** @defgroup STM32H7B3I_DISCO_EEPROM_Exported_FunctionsPrototypes Exported Functions Prototypes
  * @{
  */
int32_t BSP_EEPROM_Init(uint32_t Instance);
int32_t BSP_EEPROM_DeInit(uint32_t Instance);
int32_t BSP_EEPROM_WritePage(uint32_t Instance, uint8_t* pBuffer, uint32_t PageNbr);
int32_t BSP_EEPROM_ReadPage(uint32_t Instance, uint8_t* pBuffer, uint32_t PageNbr);
int32_t BSP_EEPROM_WriteBuffer(uint32_t Instance, uint8_t* pBuffer, uint32_t WriteAddr, uint32_t NbrOfBytes);
int32_t BSP_EEPROM_ReadBuffer(uint32_t Instance, uint8_t* pBuffer, uint32_t ReadAddr, uint32_t NbrOfBytes);
int32_t BSP_EEPROM_IsDeviceReady(uint32_t Instance);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32H7B3I_DISCO_EEPROM_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
