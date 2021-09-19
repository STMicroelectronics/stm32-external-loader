/**
  ******************************************************************************
  * @file    Dev_Inf.c
  * @author  MCD Application Team
  * @brief   This file defines the structure containing informations about the 
  *          external flash memory M29W128GL used by STM32CubeProgramer in 
  *          programming/erasing the device.
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
#include "Dev_Inf.h"

/* This structure containes information used by ST-LINK Utility to program and erase the device */
#if defined (__ICCARM__)
__root struct StorageInfo const StorageInfo  =  {
#else
struct StorageInfo const StorageInfo  =  {
#endif
   "M29W128GL_STM32H743I-EVAL", 		   	// Device Name + version number
   NOR_FLASH,                     	         	// Device Type
   0x60000000,                                   // Device Start Address
   0x1000000,                 					 // Device Size in Bytes 16MBytes
   0x00000010,                 					 // Programming Page Size 16Bytes
   0xFF,                       					 // Initial Content of Erased Memory
// Specify Size and Address of Sectors (view example below)
   0x00000080, 0x00020000,     				 // Sector Num : 128 ,Sector Size: 128KBytes 
   0x00000000, 0x00000000,
};


