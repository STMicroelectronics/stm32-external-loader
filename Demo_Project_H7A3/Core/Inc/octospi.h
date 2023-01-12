/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    octospi.h
  * @brief   This file contains all the function prototypes for
  *          the octospi.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __OCTOSPI_H__
#define __OCTOSPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern OSPI_HandleTypeDef hospi1;

/* USER CODE BEGIN Private defines */

//MT25TL256 memory parameters
#define MEMORY_FLASH_SIZE	(32UL << 20)	// 256 Mib => 32MiB
#define MEMORY_BLOCK_SIZE	(64UL << 10)   	// 512 sectors of 64KiB
#define MEMORY_SECTOR_SIZE	(4UL << 10)   	// 8192 subsectors of 4KiB
#define MEMORY_PAGE_SIZE	256     		// 131072 pages of 256B

#define MEMORY_DUAL_PAGE_SIZE (MEMORY_PAGE_SIZE*2)
#define MEMORY_DUAL_SECTOR_SIZE (MEMORY_SECTOR_SIZE*2)

#define SECTORS_COUNT (MEMORY_FLASH_SIZE / MEMORY_SECTOR_SIZE)




/* USER CODE END Private defines */

void MX_OCTOSPI1_Init(void);

/* USER CODE BEGIN Prototypes */

uint8_t CSP_QUADSPI_Init(void);
uint8_t CSP_QSPI_EraseSector(uint32_t EraseStartAddress ,uint32_t EraseEndAddress);
uint8_t CSP_QSPI_WriteMemory(const uint8_t* buffer, uint32_t address, uint32_t buffer_size);
uint8_t CSP_QSPI_ReadMemory(uint8_t* buffer, uint32_t address, uint32_t buffer_size);
uint8_t CSP_QSPI_EnableMemoryMappedMode(void);
uint8_t CSP_QSPI_Erase_Chip(void);

extern OSPI_HandleTypeDef hospi1;

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __OCTOSPI_H__ */

