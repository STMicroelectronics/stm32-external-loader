/**
  ******************************************************************************
  * @file    Flash_QSPI_Micron.c
  * @author  MCD Application Team  
  * @brief   This file contains the required functions to perform writE, erase
  *          and read operations on Micron flash
  *
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
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx.h"
#include "Flash_QSPI_Micron.h"
#include "stm32f7xx_quadspi.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern QUADSPI_InitTypeDef QUADSPI_InitStructure;
extern QUADSPI_ComConfig_InitTypeDef QUADSPI_ComConfig_InitStructure;
__IO uint8_t tmp_variable;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/* Description :																			*/
/* send command to the nor flash  		*/
/* Inputs 	:																				*/
/* 			Instruction  																		*/
/* outputs 	:																				*/
/* 				 None 																		*/
void QSPI_SendInstruction(uint8_t Instruction)
{
 QUADSPI_ComConfig_InitTypeDef QUADSPI_ComConfig_InitStructure;
	
  QUADSPI_Valid_WriteEnable_PH();
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_DDRMode = QUADSPI_ComConfig_DDRMode_Disable ;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_SIOOMode = QUADSPI_ComConfig_SIOOMode_Disable ;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_DummyCycles = 0 ;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_FMode = QUADSPI_ComConfig_FMode_Indirect_Write;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ABSize = QUADSPI_ComConfig_ABSize_8bit;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ABMode = QUADSPI_ComConfig_ABMode_NoAlternateByte ;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ADSize = QUADSPI_ComConfig_ADSize_24bit;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_DMode = QUADSPI_ComConfig_DMode_NoData;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ADMode = QUADSPI_ComConfig_ADMode_NoAddress;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_IMode = QUADSPI_ComConfig_IMode_1Line ;
  
  QUADSPI_SetFIFOThreshold(0);
  QUADSPI_SetDataLength(0);
  
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_Ins = Instruction ;
  QUADSPI_ComConfig_Init(&QUADSPI_ComConfig_InitStructure);
  
  while(QUADSPI_GetFlagStatus(QUADSPI_FLAG_BUSY)==SET);

}

/**
  * @brief  Erases specified flash memory sector.  
  * @param  SectorAddr: Address of the sector that will be erased.  
  *         This parameter can be any value of between 0x00000000 and 0xFFFFFFFF
  * @retval None
  */

void QUADSPI_Valid_EraseSector(uint32_t SectorAddr)
{
  
  QUADSPI_Valid_WriteEnable_PH();
  
  /* Send Erase Sector Command */
  QUADSPI_SetDataLength(0);
  QUADSPI_ComConfig_StructInit(&QUADSPI_ComConfig_InitStructure); 
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_IMode = QUADSPI_ComConfig_IMode_1Line;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ADMode = QUADSPI_ComConfig_ADMode_1Line;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ADSize = QUADSPI_ComConfig_ADSize_24bit;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_FMode = QUADSPI_ComConfig_FMode_Indirect_Read;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_DMode = QUADSPI_ComConfig_DMode_NoData;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_Ins = SECTOR_ERASE_CMD;

  QUADSPI_ComConfig_Init(&QUADSPI_ComConfig_InitStructure); 
  /* Set sector address to erase */
  QUADSPI_SetAddress(SectorAddr);
  
  /* Reconfigure Quadspi to automatic polling mode to wait for end of erase */
  QUADSPI_Valid_WriteEnable_PH();
  QUADSPI_SetDataLength(0x00);
  QUADSPI_AutoPollingMode_Config(0x00,0x01,QUADSPI_PMM_AND);
  QUADSPI_AutoPollingModeStopCmd(ENABLE);
  QUADSPI_SetDataLength(0);
  QUADSPI_ClearFlag(QUADSPI_FLAG_SM);
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_FMode = QUADSPI_ComConfig_FMode_Auto_Polling;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ADMode = QUADSPI_ComConfig_ADMode_NoAddress;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_DMode = QUADSPI_ComConfig_DMode_1Line;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_Ins = READ_SR_CMD;
  QUADSPI_ComConfig_Init(&QUADSPI_ComConfig_InitStructure); /* Read SR1 Command */
  
  /* Wait for end of Erase */
  while(QUADSPI_GetFlagStatus(QUADSPI_FLAG_SM)==RESET);
  QUADSPI_ClearFlag(QUADSPI_FLAG_SM);
  while(QUADSPI_GetFlagStatus(QUADSPI_FLAG_BUSY)==SET);
}

/**
  * @brief  Mass Erase of Flash Memory   
  * @param  None
  *         
  * @retval None
  */
void QUADSPI_Valid_BulkErase(void)
{
  
  QUADSPI_Valid_WriteEnable_PH();
  
  /* Send Erase Sector Command */
  QUADSPI_SetDataLength(0);
  QUADSPI_ComConfig_StructInit(&QUADSPI_ComConfig_InitStructure); 
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_IMode = QUADSPI_ComConfig_IMode_1Line;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ADMode = QUADSPI_ComConfig_ADMode_NoAddress;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ADSize = QUADSPI_ComConfig_ADSize_24bit;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_FMode = QUADSPI_ComConfig_FMode_Indirect_Read;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_DMode = QUADSPI_ComConfig_DMode_NoData;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_Ins = BULK_ERASE_CMD;

  QUADSPI_ComConfig_Init(&QUADSPI_ComConfig_InitStructure);
  
  /* Reconfigure Quadspi to automatic polling mode to wait for end of erase */
  QUADSPI_Valid_WriteEnable_PH();
  QUADSPI_SetDataLength(0x00);
  QUADSPI_AutoPollingMode_Config(0x00,0x01,QUADSPI_PMM_AND);
  QUADSPI_AutoPollingModeStopCmd(ENABLE);
  QUADSPI_SetDataLength(0);
  QUADSPI_ClearFlag(QUADSPI_FLAG_SM);
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_FMode = QUADSPI_ComConfig_FMode_Auto_Polling;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ADMode = QUADSPI_ComConfig_ADMode_NoAddress;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_DMode = QUADSPI_ComConfig_DMode_1Line;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_Ins = READ_SR_CMD;
  QUADSPI_ComConfig_Init(&QUADSPI_ComConfig_InitStructure); /* Read SR1 Command */
  
  /* Wait for end of Erase */
  while(QUADSPI_GetFlagStatus(QUADSPI_FLAG_SM)==RESET);
  QUADSPI_ClearFlag(QUADSPI_FLAG_SM);
  while(QUADSPI_GetFlagStatus(QUADSPI_FLAG_BUSY)==SET);
}

/**
  * @brief  Write the specified buffer to the flash memory  
  * @param  pBuffer: pointer to the buffer that will be written in the flash  
  * @param  WriteAddr: The address where the data will be written in flash  
  * @param  NumByteToWrite: The number of bytes to be will be written in flash
  * @retval None
  */
void QUADSPI_Valid_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint32_t* NumByteToWrite)
{
  uint32_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
  uint32_t   QSPI_DataNum = 0;
  
  Addr = WriteAddr % QSPI_PAGESIZE;
  count = QSPI_PAGESIZE - Addr;
  NumOfPage =  (*NumByteToWrite) / QSPI_PAGESIZE;
  NumOfSingle = (*NumByteToWrite) % QSPI_PAGESIZE;
  
  QUADSPI_Valid_4Byte_Address(ENABLE);/* Enable 4 Byte Addressing Mode */

  if (Addr == 0) /*!< WriteAddr is QSPI_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < QSPI_PAGESIZE */
    {
      QSPI_DataNum = (*NumByteToWrite);      
      QUADSPI_Valid_WritePage(pBuffer, WriteAddr, &QSPI_DataNum);
    }
    else /*!< NumByteToWrite > QSPI_PAGESIZE */
    {
      while (NumOfPage--)
      {
        QSPI_DataNum = QSPI_PAGESIZE;
        QUADSPI_Valid_WritePage(pBuffer, WriteAddr, &QSPI_DataNum);
        WriteAddr +=  QSPI_PAGESIZE;
        pBuffer += QSPI_PAGESIZE;
      }
      
      QSPI_DataNum = NumOfSingle;
      QUADSPI_Valid_WritePage(pBuffer, WriteAddr, &QSPI_DataNum);
    }
  }
  else /*!< WriteAddr is not QSPI_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < QSPI_PAGESIZE */
    {
      if (NumOfSingle > count) /*!< (NumByteToWrite + WriteAddr) > QSPI_PAGESIZE */
      {
        temp = NumOfSingle - count;
        QSPI_DataNum = count;
        QUADSPI_Valid_WritePage(pBuffer, WriteAddr, &QSPI_DataNum);
        WriteAddr +=  count;
        pBuffer += count;
        
        QSPI_DataNum = temp;
        QUADSPI_Valid_WritePage(pBuffer, WriteAddr, &QSPI_DataNum);
      }
      else
      {
        QSPI_DataNum = (*NumByteToWrite); 
        QUADSPI_Valid_WritePage(pBuffer, WriteAddr, &QSPI_DataNum);
      }
    }
    else /*!< NumByteToWrite > QSPI_PAGESIZE */
    {
      (*NumByteToWrite) -= count;
      NumOfPage =  (*NumByteToWrite) / QSPI_PAGESIZE;
      NumOfSingle = (*NumByteToWrite) % QSPI_PAGESIZE;

      QSPI_DataNum = count;
        
      QUADSPI_Valid_WritePage(pBuffer, WriteAddr, &QSPI_DataNum);
      WriteAddr +=  count;
      pBuffer += count;

      while (NumOfPage--)
      {
        QSPI_DataNum = QSPI_PAGESIZE;
        
        QUADSPI_Valid_WritePage(pBuffer, WriteAddr, &QSPI_DataNum);
        WriteAddr +=  QSPI_PAGESIZE;
        pBuffer += QSPI_PAGESIZE;
      }

      if (NumOfSingle != 0)
      {
        QSPI_DataNum = NumOfSingle;
        
        QUADSPI_Valid_WritePage(pBuffer, WriteAddr, &QSPI_DataNum);
      }
    }
  }
  
  QUADSPI_Valid_4Byte_Address(DISABLE);/* Disable 4 Byte Addressing Mode */
}

/**
  * @brief  Write the specified buffer to the flash memory  
  * @param  pBuffer: pointer to the buffer that will be written in the flash  
  * @param  WriteAddr: The address where the data will be written in flash  
  * @param  NumByteToWrite: The number of bytes to be will be written in flash
  *         This parameter must be inferior to Flash memroy page size
  * @retval None
  */
void QUADSPI_Valid_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint32_t* NumByteToWrite)
{
  if (* NumByteToWrite == 0)
  return;
  
  QUADSPI_Valid_WriteEnable_PH();
  
   /* Writing Sequence -------------------------------------------------------*/

  QUADSPI_SetDataLength(*NumByteToWrite - 1);
  QUADSPI_SetAddress(WriteAddr);
  QUADSPI_ComConfig_StructInit(&QUADSPI_ComConfig_InitStructure); 
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_IMode = QUADSPI_ComConfig_IMode_1Line;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_FMode = QUADSPI_ComConfig_FMode_Indirect_Write;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ADMode = QUADSPI_ComConfig_ADMode_1Line;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ADSize = QUADSPI_ComConfig_ADSize_24bit; /* Enable 4 Byte Addressing Mode */ //MBM 32
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_DMode = QUADSPI_ComConfig_DMode_4Line;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_Ins = PAGE_PROGRAM_QUAD_CMD;
  QUADSPI_ComConfig_Init(&QUADSPI_ComConfig_InitStructure); /* Write Buffer to the Flash */
  
  while((* NumByteToWrite)--)
  {
    while(!(QUADSPI_GetFlagStatus(QUADSPI_FLAG_FT)));
    QUADSPI_SendData8(* pBuffer++);
  }
  (* NumByteToWrite) = 0;
  
  while(QUADSPI_GetFlagStatus(QUADSPI_FLAG_BUSY)==SET);
  while(QUADSPI_GetFlagStatus(QUADSPI_FLAG_TC)==RESET);
  QUADSPI_ClearFlag(QUADSPI_FLAG_TC);
  
  QUADSPI_SetDataLength(0x00);
  QUADSPI_AutoPollingMode_Config(0x00,0x01,QUADSPI_PMM_AND);
  QUADSPI_AutoPollingModeStopCmd(ENABLE);
  QUADSPI_SetDataLength(0);

  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_FMode = QUADSPI_ComConfig_FMode_Auto_Polling;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ADMode = QUADSPI_ComConfig_ADMode_NoAddress;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_DMode = QUADSPI_ComConfig_DMode_1Line;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_Ins = READ_SR_CMD;
  QUADSPI_ComConfig_Init(&QUADSPI_ComConfig_InitStructure); 
  
  while(QUADSPI_GetFlagStatus(QUADSPI_FLAG_SM)==RESET);
  QUADSPI_ClearFlag(QUADSPI_FLAG_SM);
  while(QUADSPI_GetFlagStatus(QUADSPI_FLAG_BUSY)==SET);
}

/**
  * @brief  Read the specified buffer from the flash memory  
  * @param  pBuffer: pointer to the buffer that will be written in the flash  
  * @param  ReadAddr: The address where the data will be written in flash  
  * @param  NumByteToWrite: The number of bytes to be will be written in flash
  *         This parameter must be inferior to Flash memroy page size
  * @retval None
  */
void QUADSPI_Valid_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint32_t* NumByteToRead)
{
 
  QUADSPI_Valid_4Byte_Address(ENABLE);/* Enable 4 Byte Addressing Mode */
   /* Writing Sequence -------------------------------------------------------*/

  QUADSPI_ComConfig_StructInit(&QUADSPI_ComConfig_InitStructure); 
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_DummyCycles = 10;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_IMode = QUADSPI_ComConfig_IMode_1Line;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_FMode = QUADSPI_ComConfig_FMode_Indirect_Read;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ADMode = QUADSPI_ComConfig_ADMode_4Line;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ADSize = QUADSPI_ComConfig_ADSize_32bit;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_DMode = QUADSPI_ComConfig_DMode_4Line;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_Ins = READ_QUAD_IO_CMD;
  QUADSPI_ComConfig_Init(&QUADSPI_ComConfig_InitStructure); /* Write Buffer to the Flash */
  
  QUADSPI_SetDataLength(*NumByteToRead - 1);
  QUADSPI_SetAddress(ReadAddr);
  
  while(QUADSPI_GetFlagStatus(QUADSPI_FLAG_BUSY))
  {
    if(QUADSPI_GetFlagStatus(QUADSPI_FLAG_FT))
      * pBuffer++ = QUADSPI_ReceiveData8();
    (* NumByteToRead) --;
  }
  
  while(QUADSPI_GetFlagStatus(QUADSPI_FLAG_BUSY)==SET);
  while(QUADSPI_GetFlagStatus(QUADSPI_FLAG_TC)==RESET);
  QUADSPI_ClearFlag(QUADSPI_FLAG_TC);
  
  QUADSPI_Valid_4Byte_Address(DISABLE);/* Disable 4 Byte Addressing Mode */
}

/**
  * @brief  Enable write operations on the Flash memory (the polling on status
   *        register id done by software)
  * @param  None
  * @retval None
  */

void QUADSPI_Valid_WriteEnable_PS(void)
{
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_FMode = QUADSPI_ComConfig_FMode_Indirect_Write;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_SIOOMode = QUADSPI_ComConfig_SIOOMode_Disable;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_DummyCycles = 0;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ABSize = QUADSPI_ComConfig_ABSize_8bit;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ADSize = QUADSPI_ComConfig_ADSize_24bit;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_DMode = QUADSPI_ComConfig_DMode_NoData;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ADMode = QUADSPI_ComConfig_ADMode_NoAddress;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ABMode = QUADSPI_ComConfig_ABMode_NoAlternateByte;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_IMode = QUADSPI_ComConfig_IMode_1Line;
  
  QUADSPI_SetFIFOThreshold(0);
  QUADSPI_SetDataLength(0);
  
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_Ins = WREN_CMD ; 
  QUADSPI_ComConfig_Init(&QUADSPI_ComConfig_InitStructure);
  while(QUADSPI_GetFlagStatus(QUADSPI_FLAG_BUSY)==SET);
  
  QUADSPI_SetDataLength(0xFFFFFFFF);
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_FMode = QUADSPI_ComConfig_FMode_Indirect_Read;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_DMode = QUADSPI_ComConfig_DMode_1Line;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_Ins = READ_SR_CMD;
  QUADSPI_ComConfig_Init(&QUADSPI_ComConfig_InitStructure); 
    while(!(tmp_variable & 0x02))/* Wait until WEL flag is set */
    {
      if(QUADSPI_GetFlagStatus(QUADSPI_FLAG_FT));
      tmp_variable = QUADSPI_ReceiveData8();
    }
  QUADSPI_AbortRequest();
  while(QUADSPI_GetFlagStatus(QUADSPI_FLAG_BUSY)==SET);
}


/**
  * @brief  Enable write operations on the Flash memory (the polling on status
   *        register id done by hardware)
  * @param  None
  * @retval None
  */

void QUADSPI_Valid_WriteEnable_PH(void)
{	
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_FMode = QUADSPI_ComConfig_FMode_Indirect_Write;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_SIOOMode = QUADSPI_ComConfig_SIOOMode_Disable;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_DummyCycles = 0;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ABSize = QUADSPI_ComConfig_ABSize_8bit;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ADSize = QUADSPI_ComConfig_ADSize_24bit;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_DMode = QUADSPI_ComConfig_DMode_NoData;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ADMode = QUADSPI_ComConfig_ADMode_NoAddress;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ABMode = QUADSPI_ComConfig_ABMode_NoAlternateByte;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_IMode = QUADSPI_ComConfig_IMode_1Line;
  
  QUADSPI_SetFIFOThreshold(0);
  QUADSPI_SetDataLength(0);
  
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_Ins = WREN_CMD ; 
  QUADSPI_ComConfig_Init(&QUADSPI_ComConfig_InitStructure);
  while(QUADSPI_GetFlagStatus(QUADSPI_FLAG_BUSY)==SET);
  
  QUADSPI_AutoPollingMode_Config(0x02,0x02,QUADSPI_PMM_AND);
  QUADSPI_AutoPollingModeStopCmd(ENABLE);
  QUADSPI_SetDataLength(0x00);

  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_FMode = QUADSPI_ComConfig_FMode_Auto_Polling;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ADMode = QUADSPI_ComConfig_ADMode_NoAddress;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_DMode = QUADSPI_ComConfig_DMode_1Line;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_Ins = READ_SR_CMD;
  QUADSPI_ComConfig_Init(&QUADSPI_ComConfig_InitStructure); 
  
  while(QUADSPI_GetFlagStatus(QUADSPI_FLAG_SM)==RESET);
  QUADSPI_ClearFlag(QUADSPI_FLAG_SM);
  QUADSPI_ClearFlag(QUADSPI_FLAG_TC);
  while(QUADSPI_GetFlagStatus(QUADSPI_FLAG_BUSY)==SET);
}

/**
  * @brief  Enable XIP (Execute In Place) mode in the flash memory
  * @param  None
  * @retval None
  */

void QUADSPI_Valid_XIP_Enable(void)
{
  QUADSPI_Valid_WriteEnable_PH();
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_FMode = QUADSPI_ComConfig_FMode_Indirect_Write;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ABSize = QUADSPI_ComConfig_ABSize_8bit;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ADSize = QUADSPI_ComConfig_ADSize_24bit;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_DMode = QUADSPI_ComConfig_DMode_1Line;
  
  QUADSPI_SetFIFOThreshold(0);
  QUADSPI_SetDataLength(0);
  
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_Ins = WRITE_V_CONF_CMD ;
  QUADSPI_ComConfig_Init(&QUADSPI_ComConfig_InitStructure);
  
  QUADSPI_SendData8(0xF3);
  
  while(QUADSPI_GetFlagStatus(QUADSPI_FLAG_BUSY)==SET);
  
  QUADSPI_SetDataLength(0x00);
  QUADSPI_AutoPollingMode_Config(0x00,0x08,QUADSPI_PMM_AND);
  QUADSPI_AutoPollingModeStopCmd(ENABLE);
  QUADSPI_SetDataLength(0);

  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_FMode = QUADSPI_ComConfig_FMode_Auto_Polling;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ADMode = QUADSPI_ComConfig_ADMode_NoAddress;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_DMode = QUADSPI_ComConfig_DMode_1Line;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_Ins = READ_V_CONF_CMD;
  QUADSPI_ComConfig_Init(&QUADSPI_ComConfig_InitStructure); 
  
  while(QUADSPI_GetFlagStatus(QUADSPI_FLAG_SM)==RESET);
  QUADSPI_ClearFlag(QUADSPI_FLAG_SM);
  while(QUADSPI_GetFlagStatus(QUADSPI_FLAG_BUSY)==SET);
}

/**
  * @brief  Enables or Disables 4 Byte Addressing Mode
  * @param  NewState: new state of the Micron Flash 4 Byte Address Mode. 
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */

void QUADSPI_Valid_4Byte_Address(FunctionalState NewState)
{
  
  QUADSPI_Valid_WriteEnable_PH();
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_FMode = QUADSPI_ComConfig_FMode_Indirect_Write;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ABSize = QUADSPI_ComConfig_ABSize_8bit;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ADSize = QUADSPI_ComConfig_ADSize_24bit;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_DMode = QUADSPI_ComConfig_DMode_NoData;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ADMode = QUADSPI_ComConfig_ADMode_NoAddress;
  
  QUADSPI_SetFIFOThreshold(0);
  QUADSPI_SetDataLength(0);
  
    if (NewState != DISABLE)
  {
    QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_Ins = ENTER_4B_ADDR_CMD ;
  }
  else
  {
    QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_Ins = EXIT_4B_ADDR_CMD ;
  }
  QUADSPI_ComConfig_Init(&QUADSPI_ComConfig_InitStructure);
  
  while(QUADSPI_GetFlagStatus(QUADSPI_FLAG_BUSY)==SET);

}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
