/**
  ******************************************************************************
  * @file    stm32l4r9i_eval_ospi_nor.h
  * @author  MCD Application Team
  * @version V0.0.2
  * @date    23-December-2016
  * @brief   This file contains the common defines and functions prototypes for
  *          the stm32l4r9i_eval_ospi_nor.c driver.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32L4R9I_EVAL_OSPI_NOR_H
#define __STM32L4R9I_EVAL_OSPI_NOR_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "mx25lm51245g.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L4R9I_EVAL
  * @{
  */

/** @addtogroup STM32L4R9I_EVAL_OSPI_NOR
  * @{
  */

/* Exported constants --------------------------------------------------------*/ 
/** @defgroup STM32L4R9I_EVAL_OSPI_NOR_Exported_Constants Exported Constants
  * @{
  */
/* OSPI Error codes */
#define OSPI_NOR_OK            ((uint8_t)0x00)
#define OSPI_NOR_ERROR         ((uint8_t)0x01)
#define OSPI_NOR_BUSY          ((uint8_t)0x02)
#define OSPI_NOR_NOT_SUPPORTED ((uint8_t)0x04)
#define OSPI_NOR_SUSPENDED     ((uint8_t)0x08)

/**
  * @}
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup STM32L4R9I_EVAL_OSPI_NOR_Exported_Types Exported Types
  * @{
  */
/* OSPI Info */
typedef struct {
  uint32_t FlashSize;          /*!< Size of the flash */
  uint32_t EraseSectorSize;    /*!< Size of sectors for the erase operation */
  uint32_t EraseSectorsNumber; /*!< Number of sectors for the erase operation */
  uint32_t ProgPageSize;       /*!< Size of pages for the program operation */
  uint32_t ProgPagesNumber;    /*!< Number of pages for the program operation */
} OSPI_NOR_Info;

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup STM32L4R9I_EVAL_OSPI_NOR_Exported_Functions Exported Functions
  * @{
  */
uint8_t BSP_OSPI_NOR_Init                  (void);
uint8_t BSP_OSPI_NOR_DeInit                (void);
uint8_t BSP_OSPI_NOR_Read                  (uint8_t* pData, uint32_t ReadAddr, uint32_t Size);
uint8_t BSP_OSPI_NOR_Write                 (uint8_t* pData, uint32_t WriteAddr, uint32_t Size);
uint8_t BSP_OSPI_NOR_Erase_Block           (uint32_t BlockAddress);
uint8_t BSP_OSPI_NOR_Erase_Sector          (uint32_t Sector);
uint8_t BSP_OSPI_NOR_Erase_Chip            (void);
uint8_t BSP_OSPI_NOR_GetStatus             (void);
uint8_t BSP_OSPI_NOR_GetInfo               (OSPI_NOR_Info* pInfo);
uint8_t BSP_OSPI_NOR_EnableMemoryMappedMode(void);
uint8_t BSP_OSPI_NOR_EnableWriteMemoryMappedMode(void);
uint8_t BSP_OSPI_NOR_SuspendErase          (void);
uint8_t BSP_OSPI_NOR_ResumeErase           (void);
uint8_t BSP_OSPI_NOR_EnterDeepPowerDown    (void);
uint8_t BSP_OSPI_NOR_LeaveDeepPowerDown    (void);

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

#endif /* __STM32L4R9I_EVAL_OSPI_NOR_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
