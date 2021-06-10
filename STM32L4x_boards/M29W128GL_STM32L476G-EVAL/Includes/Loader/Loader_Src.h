/**
  ******************************************************************************
  * @file    Loader_Src.h
  * @author  MCD Application Team
  * @brief   Header file of Loader_Src.c
  *           
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#ifndef __LOADER_SRC_H
#define __LOADER_SRC_H

/* Includes ------------------------------------------------------------------*/
#define StartAddresse 0x64000000
#include "stm32l4xx_hal.h"
#include "stm32l476g_eval_nor.h"

#define TIMEOUT 5000U
#define KeepInCompilation __root

/* Private function prototypes -----------------------------------------------*/
int Init ();
KeepInCompilation int Write (uint32_t Address, uint32_t Size, uint16_t* Buffer);
KeepInCompilation int SectorErase (uint32_t EraseStartAddress ,uint32_t EraseEndAddress);
KeepInCompilation uint64_t Verify (uint32_t MemoryAddr, uint32_t RAMBufferAddr, uint32_t Size, uint32_t missalignement);
//KeepInCompilation HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority);
KeepInCompilation int MassErase (void);
void SystemClock_Config(void);



#endif /* __LOADER_SRC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
