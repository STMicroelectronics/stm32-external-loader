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
#include "stm32h743xx.h"
#include "STM32H7FMC.h"
#include "stm32h7xx_hal_nor.h"
#include "stm32h7xxfmc.h"

#define TIMEOUT 5000U
#define KeepInCompilation __root

/* Private function prototypes -----------------------------------------------*/
int Init (void);
KeepInCompilation int SectorErase (uint32_t EraseStartAddress ,uint32_t EraseEndAddress);
KeepInCompilation int Write (uint32_t Address, uint32_t Size, uint16_t* Buffer);;
static void MPU_Config(void);


#endif /* __LOADER_SRC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/