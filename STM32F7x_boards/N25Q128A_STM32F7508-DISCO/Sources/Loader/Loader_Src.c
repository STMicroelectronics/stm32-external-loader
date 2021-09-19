/**
  ******************************************************************************
  * @file    Loader_Src.c
  * @author  MCD Application Team
  * @brief   This file defines the operations of the external loader for
  *          N25Q128A QSPI memory of STM32F7508-DISCO.
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
  *
  ******************************************************************************
  */
#include "stm32f7xx.h"
#include "stm32f7xx_conf.h"
#include "stm32f7xx_quadspi.h"
#include "Flash_QSPI_Micron.h"
#include "stm32f7xx_rcc.h"
#include <string.h>

/* Private variables ---------------------------------------------------------*/
QUADSPI_InitTypeDef QUADSPI_InitStructure;
QUADSPI_ComConfig_InitTypeDef QUADSPI_ComConfig_InitStructure;

extern void SystemInit(void);

uint64_t Verify (uint32_t MemoryAddr, uint32_t RAMBufferAddr, uint32_t Size, uint32_t missalignement);
/**
  * @brief  System initialization.
  * @param  None
  * @retval  1      : Operation succeeded
  * @retval  0      : Operation failed
  */
int Init (void)
{ 
  GPIO_InitTypeDef GPIO_InitStructure;

  SystemInit();	
  RCC_HCLKConfig(RCC_SYSCLK_Div1);

	/*  Init structs to Zero*/
	memset( &QUADSPI_InitStructure,0,sizeof(QUADSPI_InitStructure));
	memset( &QUADSPI_ComConfig_InitStructure,0,sizeof(QUADSPI_ComConfig_InitStructure));
	
  /* Enable QUADSPI Clock*/
	RCC->AHB3ENR |= RCC_AHB3Periph_QSPI;

  /* Enable GPIO AHB1 clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC
	                       |RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE/*|RCC_AHB1Periph_GPIOF*/,ENABLE);

	/* Enable APB2 clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;

  GPIO_PinAFConfig(GPIOB,GPIO_PinSource2,9);/* CLK */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_Init(GPIOB,& GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOD, GPIO_PinSource11,9);/* 1IO0 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_Init(GPIOD,& GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOD, GPIO_PinSource12,9);/* 1IO1 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_Init(GPIOD,& GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOE, GPIO_PinSource2,9);/* 1IO2 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_Init(GPIOE,& GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOD, GPIO_PinSource13,9);/* 1IO3 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_Init(GPIOD,& GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,10);/* NCS1 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
  GPIO_Init(GPIOB,& GPIO_InitStructure);

  RCC_HCLKConfig(RCC_SYSCLK_Div1);
  QUADSPI_DeInit();

  QUADSPI_InitStructure.QUADSPI_SShift = QUADSPI_SShift_NoShift;
  QUADSPI_InitStructure.QUADSPI_Prescaler = 0x2; // 180MHz /(2+1) = 60MHz 
  QUADSPI_InitStructure.QUADSPI_CKMode = QUADSPI_CKMode_Mode0;
  QUADSPI_InitStructure.QUADSPI_CSHTime = QUADSPI_CSHTime_1Cycle;
  QUADSPI_InitStructure.QUADSPI_FSize = 24; // 2^24 = 16MB
#ifdef USE_DUAL_FLASH
	QUADSPI_InitStructure.QUADSPI_FSelect = QUADSPI_FSelect_1;
	QUADSPI_InitStructure.QUADSPI_DFlash = QUADSPI_DFlash_Enable;
#elif   defined(USE_FLASH_B)
	QUADSPI_InitStructure.QUADSPI_FSelect = QUADSPI_FSelect_2;
	QUADSPI_InitStructure.QUADSPI_DFlash = QUADSPI_DFlash_Disable;
#else
	QUADSPI_InitStructure.QUADSPI_FSelect = QUADSPI_FSelect_1;
	QUADSPI_InitStructure.QUADSPI_DFlash = QUADSPI_DFlash_Disable;
#endif /* USE_DUAL_FLASH */

  QUADSPI_Init(&QUADSPI_InitStructure);
  
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_FMode = QUADSPI_ComConfig_FMode_Indirect_Write;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_SIOOMode = QUADSPI_ComConfig_SIOOMode_Disable;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_DummyCycles = 0;//mbm 0
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ABSize = QUADSPI_ComConfig_ABSize_8bit;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ADSize = QUADSPI_ComConfig_ADSize_24bit;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_DMode = QUADSPI_ComConfig_DMode_NoData;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ADMode = QUADSPI_ComConfig_ADMode_NoAddress;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ABMode = QUADSPI_ComConfig_ABMode_NoAlternateByte;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_IMode = QUADSPI_ComConfig_IMode_1Line;
  
  QUADSPI_Cmd(ENABLE);
  QUADSPI_SetFIFOThreshold(0);
  QUADSPI_SetDataLength(0);	
	
  /* Reset memory config */
	/* Send RESET ENABLE command (0x66) to be able to reset the memory registers */
	while(QUADSPI->SR & 0x20);  /* Wait for busy flag to be cleared */
	QUADSPI->CCR = 0x2166;
	QUADSPI->AR = 0;
	QUADSPI->ABR = 0;
	QUADSPI->DLR = 0;
	__DSB(); 
	 
	/* Send RESET command (0x99) to reset the memory registers */
	while(QUADSPI->SR & 0x20);  /* Wait for busy flag to be cleared */
	QUADSPI->CCR = 0x2199;
	QUADSPI->AR = 0;
	QUADSPI->ABR = 0;
	QUADSPI->DLR = 0;
	__DSB();
	
	/* Configure QUADSPI To Memory Mapped Mode */
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_DummyCycles  = 8;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_FMode = QUADSPI_ComConfig_FMode_Memory_Mapped;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ADMode = QUADSPI_ComConfig_ADMode_1Line;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ADSize = QUADSPI_ComConfig_ADSize_24bit;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_DMode = QUADSPI_ComConfig_DMode_4Line; 
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_IMode = QUADSPI_ComConfig_IMode_1Line;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_Ins = READ_QUAD_CMD;
  QUADSPI_ComConfig_Init(&QUADSPI_ComConfig_InitStructure);
	
  return (1);
}


/**
  * @brief   Program memory.
  * @param   Address: page address
  * @param   Size   : size of data
  * @param   buffer : pointer to data buffer
  * @retval  1      : Operation succeeded
  * @retval  0      : Operation failed
  */
int Write (uint32_t Address, uint32_t Size, uint16_t* Buffer)
{ 
	uint32_t Counter = Size;
	
  QUADSPI_Valid_WriteBuffer((uint8_t*)Buffer,(Address & 0x0FFFFFFF), &Counter);

	return 1;
} 


/**
  * @brief   Sector erase.
  * @param   EraseStartAddress :  erase start address
  * @param   EraseEndAddress   :  erase end address
  * @retval  None
  */
int SectorErase (uint32_t EraseStartAddress ,uint32_t EraseEndAddress)
{      
	uint32_t BlockAddr;
	EraseStartAddress = EraseStartAddress -  EraseStartAddress % 0x10000;
		
	while (EraseEndAddress>=EraseStartAddress)
	{
		BlockAddr = EraseStartAddress & 0x0FFFFFFF;
		QUADSPI_Valid_EraseSector( BlockAddr);
    EraseStartAddress += 0x10000;
	}
 	return 1;	
}

/**
  * @brief 	 Full erase of the device 						
  * @param 	 Parallelism : 0 																		
  * @retval  1           : Operation succeeded
  * @retval  0           : Operation failed											
  */
int MassErase (uint32_t Parallelism ){
 
  QUADSPI_Valid_BulkErase();
  return 1;
}

uint32_t CheckSum(uint32_t StartAddress, uint32_t Size, uint32_t InitVal)
{
  uint8_t missalignementAddress = StartAddress%4;
  uint8_t missalignementSize = Size ;
  int cnt;
  uint32_t Val;
	
  StartAddress-=StartAddress%4;
  Size += (Size%4==0)?0:4-(Size%4);
  
  for(cnt=0; cnt<Size ; cnt+=4)
  {
    Val = *(uint32_t*)StartAddress;
    if(missalignementAddress)
    {
      switch (missalignementAddress & 0xF)
      {
        case 1:
          InitVal += (uint8_t) (Val>>8 & 0xff);
          InitVal += (uint8_t) (Val>>16 & 0xff);
          InitVal += (uint8_t) (Val>>24 & 0xff);
          missalignementAddress-=1;
          break;
        case 2:
          InitVal += (uint8_t) (Val>>16 & 0xff);
          InitVal += (uint8_t) (Val>>24 & 0xff);
          missalignementAddress-=2;
          break;
        case 3:   
          InitVal += (uint8_t) (Val>>24 & 0xff);
          missalignementAddress-=3;
          break;
      }  
    }
    else if((Size-missalignementSize)%4 && (Size-cnt) <=4)
    {
      switch ((Size-missalignementSize) & 0xF)
      {
        case 1:
          InitVal += (uint8_t) Val;
          InitVal += (uint8_t) (Val>>8 & 0xff);
          InitVal += (uint8_t) (Val>>16 & 0xff);
          missalignementSize-=1;
          break;
        case 2:
          InitVal += (uint8_t) Val;
          InitVal += (uint8_t) (Val>>8 & 0xff);
          missalignementSize-=2;
          break;
        case 3:   
          InitVal += (uint8_t) Val;
          missalignementSize-=3;
          break;
      } 
    }
    else
    {
      InitVal += (uint8_t) Val;
      InitVal += (uint8_t) (Val>>8 & 0xff);
      InitVal += (uint8_t) (Val>>16 & 0xff);
      InitVal += (uint8_t) (Val>>24 & 0xff);
    }
    StartAddress+=4;
  }
  
  return (InitVal);
}


/**
  * @brief   Verify flash memory with RAM buffer and calculates checksum value of the programmed memory
  * @param   FlashAddr     : Flash address
  * @param   RAMBufferAddr : RAM buffer address
  * @param   Size          : Size (in WORD)  
  * @param   InitVal       : Initial CRC value
  * @retval  R0            : Operation failed (address of failure)
  * @retval  R1            : Checksum value
  * @Note    Optional for all types of device
  */
uint64_t Verify (uint32_t MemoryAddr, uint32_t RAMBufferAddr, uint32_t Size, uint32_t missalignement)
{
    uint32_t InitVal = 0;
	uint32_t VerifiedData = 0;
	uint64_t checksum;
	Size*=4;

	/* Configure QUADSPI To Memory Mapped Mode */
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_DummyCycles  = 8;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_FMode = QUADSPI_ComConfig_FMode_Memory_Mapped;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ADMode = QUADSPI_ComConfig_ADMode_1Line;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_ADSize = QUADSPI_ComConfig_ADSize_24bit;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_DMode = QUADSPI_ComConfig_DMode_4Line;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_IMode = QUADSPI_ComConfig_IMode_1Line;
  QUADSPI_ComConfig_InitStructure.QUADSPI_ComConfig_Ins = READ_QUAD_CMD;
  QUADSPI_ComConfig_Init(&QUADSPI_ComConfig_InitStructure);
	checksum = CheckSum((uint32_t)MemoryAddr + (missalignement & 0xf), Size - ((missalignement >> 16) & 0xF), InitVal);  
	
  while (Size>VerifiedData)
  {
		if ( *(uint8_t*)MemoryAddr++ != *((uint8_t*)RAMBufferAddr + VerifiedData))
			return ((checksum<<32) + (MemoryAddr + VerifiedData));  
		
		VerifiedData++;  
  }
        
  return (checksum<<32);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

