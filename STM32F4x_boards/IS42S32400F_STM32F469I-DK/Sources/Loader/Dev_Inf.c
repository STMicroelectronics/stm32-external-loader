/**
  ******************************************************************************
  * @file    Dev_Inf.c
  * @author  MCD Application Team
  * @brief   This file defines the structure containing informations about the 
  *          external flash memory IS42S32400F used by STM32CubeProgramer in 
  *          programming/erasing the device.
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

#include "Dev_Inf.h"

/* This structure containes information used by ST-LINK Utility to program and erase the device */
#if defined (__ICCARM__)
__root struct StorageInfo const StorageInfo  =  {
#else
struct StorageInfo const StorageInfo  =  {
#endif
   "IS42S32400F_STM32F469I-DK", 	      					// Device Name 
   SRAM,                   	             				        // Device Type
   0xC0000000,                						        // Device Start Address
   0x1000000,              						        // Device Size in 16 MBytes
   0x1000000,                    						// Programming Page Size 4096 Bytes
   0xFF,                       						        // Initial Content of Erased Memory
// Specify Size and Address of Sectors (view example below)   
   0x00000001, 0x01000000,     				 		        // Sector Num : 1 ,Sector Size: 16 MBytes
   0x00000000, 0x00000000,      
}; 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
