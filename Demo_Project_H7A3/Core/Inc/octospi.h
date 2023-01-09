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

#define SECTORS_COUNT (MEMORY_FLASH_SIZE / MEMORY_SECTOR_SIZE)


//MT25TL256 commands
#define WRITE_ENABLE_CMD 0x06				// WRITE ENABLE
#define READ_STATUS_REG_CMD 0x05			// READ STATUS REGISTER
#define WRITE_STATUS_REG_CMD 0x01			// WRITE STATUS REGISTER
#define SECTOR_ERASE_CMD 0x20 				// 4KiB SUBSECTOR ERASE
#define CHIP_ERASE_CMD 0xC7 				// 128Mib BULK ERASE (for each die, so 256Mib)
#define QUAD_IN_FAST_PROG_CMD 0x38 			// EXTENDED QUAD INPUT FAST PROGRAM
//#define READ_CONFIGURATION_REG_CMD 0x15 	// page 50 table 24?
//#define QUAD_READ_IO_CMD 0xEC 				// ?
#define QUAD_OUT_FAST_READ_CMD 0x6B 		// QUAD OUTPUT FAST READ
#define QPI_ENABLE_CMD 0x35 				// ENTER QUAD INPUT/OUTPUT MODE
#define DISABLE_QIP_MODE 0xf5 				// RESET QUAD INPUT/OUTPUT MODE
#define RESET_ENABLE_CMD 0x66 				// RESET ENABLE
#define RESET_EXECUTE_CMD 0x99 				// RESET MEMORY


#define DUMMY_CLOCK_CYCLES_READ_QUAD 10  // which one is this? sometimes 0,4,8 or 10

/* USER CODE END Private defines */

void MX_OCTOSPI1_Init(void);

/* USER CODE BEGIN Prototypes */

uint8_t CSP_QUADSPI_Init(void);
uint8_t CSP_QSPI_EraseSector(uint32_t EraseStartAddress ,uint32_t EraseEndAddress);
uint8_t CSP_QSPI_WriteMemory(const uint8_t* buffer, uint32_t address, uint32_t buffer_size);
uint8_t CSP_QSPI_EnableMemoryMappedMode(void);
uint8_t CSP_QSPI_Erase_Chip(void);

extern OSPI_HandleTypeDef hospi1;

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __OCTOSPI_H__ */

