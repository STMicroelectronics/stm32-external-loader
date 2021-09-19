/**
  ******************************************************************************
  * @file    Loader_Src.c
  * @author  MCD Application Team
  * @brief   This file defines the operations of the external loader for
  *          M29W128GL memory of STM32L476I-EVAL.
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
#include <string.h>
#include "Loader_Src.h"
#pragma section = ".bss"

/* Private functions ---------------------------------------------------------*/
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


/** Description :
  * Initilize the MCU Clock, the GPIO Pins corresponding to the 
  * device and initilize the FSMC with the chosen configuration 
  * Inputs 	:	
  * 				 None 		
  * outputs 	:					
  * 				"1" 			: Operation succeeded	
  * 				"0" 			: Operation failure	
  * Note: Mandatory for all types of device 
  */


int Init (void)
{  
  /*  Init structs to Zero*/
    char *   startadd =  __section_begin(".bss");
    uint32_t size =  __section_size(".bss");
    memset(startadd,0,size);
 
    HAL_Init();
  
  /* Configure the system clock to 80 MHz */
  SystemClock_Config();
  
  /* NOR device configuration */ 
  BSP_NOR_Init();

  return 1;
}


/** Description :	
  * Write data to the device	 	
  * Inputs :		
  * 				Address 	: Write location  
  * 				Size 		: Length in bytes 
  * 				buffer 		: Address where to get the data to write
  * outputs :							
  * 				"1" 			: Operation succeeded		
  * 				"0" 			: Operation failure	
  * Info :							
  * Align and memory size (32/16/8 bits) is handled in this function 
  * Note : Mandatory for all types except SRAM and PSRAM	
  */
KeepInCompilation int Write (uint32_t Address, uint32_t Size, uint16_t* Buffer)
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

/** Description :
  * Full erase of the device 
  * Inputs :	
  *  				None 	
  * outputs :					
  * 				"1" : Operation succeeded	
  * 				"0" : Operation failure	
  * Info :	
  * Note : Not Mandatory for SRAM PSRAM and NOR_FLASH		
*/
KeepInCompilation int MassErase (void)
{ 
  if (BSP_NOR_Erase_Chip()==0)
    return 1;
  else
    return 0;
}

/** Description :
  * Erase a full sector in the device 
  * Inputs :				
  * 				SectrorAddress	: Start of sector 	
  * outputs :						
  * 				"1" : Operation succeeded	
  * 				"0" : Operation failure		
  * Note : Not Mandatory for SRAM PSRAM and NOR_FLASH		
  */
KeepInCompilation int SectorErase (uint32_t EraseStartAddress ,uint32_t EraseEndAddress)
{  
  uint32_t BlockAddr;

  EraseStartAddress = EraseStartAddress -  EraseStartAddress%0x20000;
  while (EraseEndAddress>=EraseStartAddress)
  {
    BlockAddr = EraseStartAddress - StartAddresse;
    if (BSP_NOR_Erase_Block(BlockAddr)!=0)
      return 1;
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
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 80000000
  *            HCLK(Hz)                       = 80000000
  * @ note REVA depency, need AHBCLK div 2 to perform correctly LCD access
  *            AHB Prescaler                  = 2
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 1
  *            PLL_N                          = 20
  *            PLL_P                          = 7
  *            PLL_Q                          = 4
  *            PLL_R                          = 2
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* Enable HSE Oscillator and activate PLL with HSE as source   */
  /* (Default MSI Oscillator enabled at system reset remains ON) */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLP = 7;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  /* REVA depency, need AHBCLK div 2 to perform correctly LCD access */
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
}

#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line)
{
  while (1)
  {}
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics 2021 *****END OF FILE****/
