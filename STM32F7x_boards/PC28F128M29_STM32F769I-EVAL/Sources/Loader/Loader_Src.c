/**
  ******************************************************************************
  * @file    Loader_Src.c
  * @author  MCD Application Team
  * @brief   This file defines the operations of the external loader for
  *          PC28F128M29 memory of STM32F769I-EVAL.
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

#include "Loader_Src.h"
#include <string.h>
#pragma section = ".bss"

/* Private function prototypes -----------------------------------------------*/

KeepInCompilation HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{ 
  return HAL_OK;
}
 
uint32_t HAL_GetTick(void)
{
  return 1;
}

void HAL_Delay(uint32_t Delay)
{
  int i=0;
  for (i=0; i<0x100000; i++);
}
/**
  * @brief  System initialization.
  * @param  None
  * @retval  1      : Operation succeeded
  * @retval  0      : Operation failed
  */
int Init (void)
{ 
  /*  Init structs to Zero*/
  char *   startadd =  __section_begin(".bss");
  uint32_t size =  __section_size(".bss");
  memset(startadd,0,size);
  
  /*  Init system*/
  SystemInit();
  HAL_Init();
  
  /* Configure the system clock */
  SystemClock_Config();
  
 /* NOR device configuration */
  BSP_NOR_Init();
  
  return 1;
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
  uint32_t InternalAddr = Address - StartAddresse;
  uint32_t WritedData = 0;
  uint32_t Counter = 0;    
  uint16_t TmpBuffer = 0x00000000;
  
  if (InternalAddr%2 != 0)
  {
    BSP_NOR_ReadData((InternalAddr - InternalAddr%2), &TmpBuffer, 1);
    
    for (Counter =0; (Counter<(2-InternalAddr%2))&&(Counter<Size); Counter++)
      *((uint8_t*)(&TmpBuffer)+InternalAddr%2+Counter) = * ((uint8_t*)Buffer+Counter);
    
    if (BSP_NOR_WriteData((InternalAddr - InternalAddr%2), &TmpBuffer, 1) != 0)
      return 0;
    
    WritedData += Counter;
  }
  
  if (Size-WritedData >= 2)
  {
    if (BSP_NOR_WriteData(InternalAddr+WritedData, (uint16_t*)((uint8_t*)Buffer+WritedData), ((Size-WritedData)/2))!=0)
      return 0;

    WritedData += (((Size-WritedData)/2)*2);
  }
  
  if (WritedData < Size)
  {
    BSP_NOR_ReadData(InternalAddr+WritedData, &TmpBuffer, 1);

    for (Counter =0; Counter<(Size-WritedData); Counter++)
      *((uint8_t*)(&TmpBuffer)+Counter) = *((uint8_t*)Buffer+WritedData+Counter);
    
    if (BSP_NOR_WriteData(InternalAddr+WritedData, &TmpBuffer, 1)!=0)
      return 0;
  }

  return 1;
} 


/**
  * @brief   Erase full chip.
  * @param   None
  * @retval  1      : Operation succeeded
  * @retval  0      : Operation failed
  */
int MassErase (void)
{
  if (BSP_NOR_Erase_Chip()==0)
    return 1;
  else
    return 0;
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
  EraseStartAddress = EraseStartAddress -  EraseStartAddress%0x20000;
  while (EraseEndAddress>=EraseStartAddress)
  {
    BlockAddr = EraseStartAddress - StartAddresse;
    if (BSP_NOR_Erase_Block(BlockAddr)!=0)
      return 0;
    EraseStartAddress+=0x20000;
  }
  return 1;	
}

   
/**
  * Description :
  * Calculates checksum value of the memory zone
  * Inputs    :
  *      StartAddress  : Flash start address
  *      Size          : Size (in WORD)  
  *      InitVal       : Initial CRC value
  * outputs   :
  *     R0             : Checksum value
  * Note: Optional for all types of device
  */
uint32_t CheckSum(uint32_t StartAddress, uint32_t Size, uint32_t InitVal)
{
  uint8_t missalignementAddress = StartAddress%4;
  uint8_t missalignementSize = Size ;
  int cnt;
  uint32_t Val;
  uint16_t value;
	
  StartAddress-=StartAddress%4;
  Size += (Size%4==0)?0:4-(Size%4);
  
  for(cnt=0; cnt<Size ; cnt+=4)
  {
    BSP_NOR_ReadData(StartAddress - StartAddresse, &value,1);
    Val = value;
    BSP_NOR_ReadData(StartAddress - StartAddresse + 2, &value, 1);
    Val+= value<<16;
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
  * Description :
  * Verify flash memory with RAM buffer and calculates checksum value of
  * the programmed memory
  * Inputs    :
  *      FlashAddr     : Flash address
  *      RAMBufferAddr : RAM buffer address
  *      Size          : Size (in WORD)  
  *      InitVal       : Initial CRC value
  * outputs   :
  *     R0             : Operation failed (address of failure)
  *     R1             : Checksum value
  * Note: Optional for all types of device
  */
uint64_t Verify (uint32_t MemoryAddr, uint32_t RAMBufferAddr, uint32_t Size, uint32_t missalignement)
{
  uint32_t InternalAddr = MemoryAddr - StartAddresse;
  uint32_t VerifiedData = 0, InitVal = 0;
  uint16_t TmpBuffer = 0x0000;
  uint64_t checksum;
  Size*=4;
	
  checksum = CheckSum((uint32_t)MemoryAddr + (missalignement & 0xf), Size - ((missalignement >> 16) & 0xF), InitVal);
  if (InternalAddr%2 != 0)
  {
    BSP_NOR_ReadData((InternalAddr - InternalAddr%2), &TmpBuffer, 1);
        
    if ((uint8_t)(TmpBuffer>>8) != (*(uint8_t*)RAMBufferAddr))
      return ((checksum<<32) +  MemoryAddr);
    VerifiedData++;    
  }
     
  while ((Size-VerifiedData)>1)
  {
    BSP_NOR_ReadData(InternalAddr+VerifiedData, &TmpBuffer, 1);
       
    if ((TmpBuffer&0x00FF) != (*((uint8_t*)RAMBufferAddr+VerifiedData)))
      return ((checksum<<32) + MemoryAddr+VerifiedData);
        
    VerifiedData++;
        
    if ((uint8_t)(TmpBuffer>>8) != (*((uint8_t*)RAMBufferAddr+VerifiedData)))
      return ((checksum<<32) + MemoryAddr+VerifiedData);
        
    VerifiedData++;
  }
     
  if ((Size-VerifiedData) != 0)
  {
    BSP_NOR_ReadData(InternalAddr+VerifiedData, &TmpBuffer, 1);
       
    if ((uint8_t)(TmpBuffer&0x00FF) != (*((uint8_t*)RAMBufferAddr+VerifiedData)))
      return ((checksum<<32) + MemoryAddr+VerifiedData);
  }

  return (checksum<<32);
}
/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 216000000
  *            HCLK(Hz)                       = 216000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 432
  *            PLL_P                          = 2
  *            PLL_Q                          = 9
  *            PLL_R                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 7
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef  ret = HAL_OK;
  
  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;  
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  RCC_OscInitStruct.PLL.PLLR = 7;
  
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
  
  /* Activate the OverDrive to reach the 216 MHz Frequency */  
  ret = HAL_PWREx_EnableOverDrive();
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2; 
  
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }  
}

/************************ (C) COPYRIGHT STMicroelectronics 2021 *****END OF FILE****/