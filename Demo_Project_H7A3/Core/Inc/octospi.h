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

//MT25TL256 memory parameters (single die specs):
#define MEMORY_FLASH_SIZE	(16UL << 20)	// 128 Mib => 16MiB
//#define MEMORY_BLOCK_SIZE	(?UL << 10)   	// ? sectors of ?KiB
#define MEMORY_SECTOR_SIZE	(4UL << 10)   	// 4096 subsectors of 4KiB
#define MEMORY_PAGE_SIZE	256     		// 65536 pages of 256B
#define SECTORS_COUNT (MEMORY_FLASH_SIZE / MEMORY_SECTOR_SIZE)

//MT25TL256 memory parameters (twin die specs):
#define MEMORY_DUAL_FLASH_SIZE (MEMORY_FLASH_SIZE*2)
#define MEMORY_DUAL_PAGE_SIZE (MEMORY_PAGE_SIZE*2)
#define MEMORY_DUAL_SECTOR_SIZE (MEMORY_SECTOR_SIZE*2)






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

