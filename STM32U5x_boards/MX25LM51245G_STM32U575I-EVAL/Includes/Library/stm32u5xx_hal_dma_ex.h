/**
  ******************************************************************************
  * @file    stm32u5xx_hal_dma_ex.h
  * @author  MCD Application Team
  * @brief   Header file of DMA HAL extension module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2019 STMicroelectronics.
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
#ifndef STM32U5xx_HAL_DMA_EX_H
#define STM32U5xx_HAL_DMA_EX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal_def.h"

/** @addtogroup STM32U5xx_HAL_Driver
  * @{
  */

/** @addtogroup DMAEx
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @defgroup DMAEx_Exported_Functions DMAEx Exported Functions
  * @brief    DMAEx Exported functions
  * @{
  */

/** @defgroup DMA_Exported_Functions_Group1 Linked-List operation functions
  * @brief    Linked-list operation functions
  * @{
  */
HAL_StatusTypeDef HAL_DMAEx_LinkedList_CreateNode(DMA_HandleTypeDef *hdma, DMA_LinkNodeTypeDef *pNode, DMA_LinkNodeConfTypeDef *pNodeConfig);
HAL_StatusTypeDef HAL_DMAEx_LinkedList_AddNode(DMA_HandleTypeDef *hdma, DMA_LinkNodeTypeDef *pNewNode, DMA_LinkNodeTypeDef *pPrevNode);
HAL_StatusTypeDef HAL_DMAEx_LinkedList_RemoveNode(DMA_HandleTypeDef *hdma, DMA_LinkNodeTypeDef *pNode);
HAL_StatusTypeDef HAL_DMAEx_LinkedList_EnableCircularMode(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMAEx_LinkedList_DisableCircularMode(DMA_HandleTypeDef *hdma);
/**
  * @}
  */

/** @defgroup DMA_Exported_Functions_Group2 Suspend and Resume operation functions
  * @brief    Suspend and Resume operation functions
  * @{
  */
HAL_StatusTypeDef HAL_DMAEx_Suspend(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMAEx_Suspend_IT(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMAEx_Resume(DMA_HandleTypeDef *hdma);
/**
  * @}
  */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup DMAEx_Private_Macros DMA Private Macros
  * @brief    DMAEx private macros
  * @{
  */


/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/
/** @defgroup DMAEx_Private_Functions DMAEx Private Functions
  * @brief    DMAEx Private functions
  * @{
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

#endif /* STM32U5xx_HAL_DMA_EX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
