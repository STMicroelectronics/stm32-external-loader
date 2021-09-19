/**
  ******************************************************************************
  * @file    Flash_QSPI_Micron.h
  * @author  MCD Application Team
  * @brief   header file of Flash_QSPI_Micro.c 
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
#ifndef __FLASH_QSPI_MICRON_H
#define __FLASH_QSPI_MICRON_H

/* Includes ------------------------------------------------------------------*/
//#include "stm32f7xx_quadspi_valid.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define WREN_CMD                     0x06
#define READ_SR_CMD                  0x05
#define READ_CMD                     0x03
#define FAST_READ_CMD                0x0B
#define FAST_READ_DDR_CMD            0x0D
#define FAST_READ_DUAL_DDR_CMD       0xBD
#define FAST_READ_QUAD_DDR_CMD       0xED
#define READ_QUAD_CMD                0x6B
#define READ_QUAD_IO_CMD             0xEB
#define READ_DUAL_CMD                0x3B
#define PAGE_PROGRAM_CMD             0x02
#define PAGE_PROGRAM_QUAD_CMD        0x32
#define PAGE_PROGRAM_DUAL_CMD        0xA2
#define SECTOR_ERASE_CMD             0xD8
#define BULK_ERASE_CMD               0xC7
#define ENTER_QUAD_CMD               0x35
#define EXIT_QUAD_CMD                0xF5
#define WRITE_V_CONF_CMD             0x81
#define READ_V_CONF_CMD              0x85
#define ENTER_4B_ADDR_CMD            0xB7
#define EXIT_4B_ADDR_CMD             0xE9

#define RSTEN_CMD                    0x66
#define RST_CMD                      0x99

#define QSPI_PAGESIZE                0x100

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void QUADSPI_Valid_WriteEnable_PS(void);
void QUADSPI_Valid_WriteEnable_PH(void);
void QUADSPI_Valid_QUADEnable(void);
void QUADSPI_Valid_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint32_t* NumByteToWrite);
void QUADSPI_Valid_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint32_t* NumByteToWrite);
void QUADSPI_Valid_EraseSector(uint32_t SectorAddr);
void QUADSPI_Valid_XIP_Enable(void);
void QUADSPI_Valid_BulkErase(void);
void QUADSPI_Valid_4Byte_Address(FunctionalState NewState);
void QUADSPI_Valid_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint32_t* NumByteToRead);
void QSPI_SendInstruction(uint8_t Instruction);
#endif /* __FLASH_QSPI_MICRON_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
