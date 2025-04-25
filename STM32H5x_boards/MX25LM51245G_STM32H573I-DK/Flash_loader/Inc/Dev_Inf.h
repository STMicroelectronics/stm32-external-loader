/**
  ******************************************************************************
  * @file    Dev_Inf.h
  * @author  ST/GPAM Application Team
  * @brief   header file of Dev_Inf.c
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
**/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DEV_INF_H
#define DEV_INF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Device Type Definitions ---------------------------------------------------*/
#define MCU_FLASH 1U
#define NAND_FLASH 2U
#define NOR_FLASH 3U
#define SRAM 4U
#define PSRAM 5U
#define PC_CARD 6U
#define SPI_FLASH 7U
#define I2C_FLASH 8U
#define SDRAM 9U
#define I2C_EEPROM 10U

#define SECTOR_NUM 10U /* Max Number of Sector types */

/**
  * @brief  Structure representing device sectors.
  */
struct DeviceSectors
{
    unsigned long SectorNum;  /**< Number of Sectors */
    unsigned long SectorSize; /**< Sector Size in Bytes */
};

/**
  * @brief  Structure representing storage information.
  */
struct StorageInfo
{
    char DeviceName[100];                     /**< Device Name and Description */
    unsigned short DeviceType;                /**< Device Type: ONCHIP, EXT8BIT, EXT16BIT, ... */
    unsigned long DeviceStartAddress;         /**< Default Device Start Address */
    unsigned long DeviceSize;                 /**< Total Size of Device */
    unsigned long PageSize;                   /**< Programming Page Size */
    unsigned char EraseValue;                 /**< Content of Erased Memory */
    struct DeviceSectors sectors[SECTOR_NUM]; /**< Array of device sectors */
};

/* External declaration of StorageInfo */
extern const struct StorageInfo StorageInfo;

#ifdef __cplusplus
}
#endif

#endif /* DEV_INF_H */
