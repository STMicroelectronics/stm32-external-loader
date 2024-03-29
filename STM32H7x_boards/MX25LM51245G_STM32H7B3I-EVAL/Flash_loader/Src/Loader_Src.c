/**
  ******************************************************************************
  * @file    Loader_Src.c
  * @author  MCD Application Team
  * @brief   This file defines the operations of the external loader for
  *          mx25lm51245g OSPI memory of STM32H7B3I-EVAL.
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
#pragma section=".bss"

/* Private variables ---------------------------------------------------------*/
BSP_OSPI_NOR_Init_t Flash;  

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
  for (i=0; i<0x1000; i++);
}

/** @defgroup STM32H7B3I_Eval_OSPI_Private_Functions Private Functions
  * @{
  */
/**
  * @brief  System initialization.
  * @param  None
  * @retval  1      : Operation succeeded
  * @retval  0      : Operation failed
  */

int Init()
{
  int32_t result=0;

 /*  Init structs to Zero*/
  char *   startadd =  __section_begin(".bss");
  uint32_t size =  __section_size(".bss");
  memset(startadd,0,size);

  /*  Init system*/
   SystemInit(); 
   HAL_Init();
   
  /*  define InterfaceMode and transferRate*/
   Flash.InterfaceMode = BSP_OSPI_NOR_OPI_MODE;
   Flash.TransferRate  = BSP_OSPI_NOR_DTR_TRANSFER;
   
  /* Configure the system clock  */
   SystemClock_Config();
   
  /*Initialaize OSPI*/
   if(BSP_OSPI_NOR_Init(0,&Flash) !=0)
    return 0;
   
  /*Configure the QSPI in memory-mapped mode*/ 
   result = BSP_OSPI_NOR_EnableMemoryMappedMode(0); 
   
   if(result!=0)
     return result;
  
  /*Enable Interrupts*/
   __enable_irq();

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
KeepInCompilation int Write (uint32_t Address, uint32_t Size, uint8_t* buffer)
{
    
    /*Disable Interrupts*/
    __disable_irq();
    Address = Address & 0x0fffffff;  
    
    /*Deinitialaize QSPI*/
    if(BSP_OSPI_NOR_DeInit(0)!=0)
     return 0;
   
   /*Initialaize QSPI*/
   if( BSP_OSPI_NOR_Init(0,&Flash)!=0)
     return 0;
   
    /*Writes an amount of data to the QSPI memory.*/
    if( BSP_OSPI_NOR_Write(0,buffer,Address, Size)!=0)
     return 0;
   
    /*Enable Interrupts*/
     __enable_irq();
     
   return 1;
}

/**
  * @brief 	 Full erase of the device 						
  * @param 	 Parallelism : 0 																		
  * @retval  1           : Operation succeeded
  * @retval  0           : Operation failed											
  */
KeepInCompilation  int MassErase (uint32_t Parallelism ){
  
    /*Disable Interrupts*/
     __disable_irq();
     
    /*Deinitialaize QSPI*/
     if(BSP_OSPI_NOR_DeInit(0) != 0)
       return 0;
     
    /*Initialaize QSPI*/
     if(BSP_OSPI_NOR_Init(0,&Flash) != 0 )
       return 0;
      
    /*Erases the entire OSPI memory*/
     BSP_OSPI_NOR_Erase_Chip(0);
     
    /*Reads current status of the OSPI memory*/
     while (BSP_OSPI_NOR_GetStatus(0)!=0);
  return 1;  
}

/**
  * @brief   Sector erase.
  * @param   EraseStartAddress :  erase start address
  * @param   EraseEndAddress   :  erase end address
  * @retval  None
  */
KeepInCompilation int SectorErase (uint32_t EraseStartAddress ,uint32_t EraseEndAddress)
{ 
   /*Disable Interrupts*/
    __disable_irq();
    
    uint32_t BlockAddr;
    EraseStartAddress = EraseStartAddress & 0x0FFFFFFF;  
    EraseEndAddress &= 0x0FFFFFFF;
    EraseStartAddress = EraseStartAddress -  EraseStartAddress % 0x10000;
    
   /*Deinitialaize QSPI*/
    if(BSP_OSPI_NOR_DeInit(0) != 0)
      return 0;
     
   /*Initialaize QSPI*/
    if(BSP_OSPI_NOR_Init(0,&Flash) != 0 )
      return 0;
    
    while (EraseEndAddress>=EraseStartAddress)
    {
      BlockAddr = EraseStartAddress;
      /*Erases the specified block of the OSPI memory*/
       BSP_OSPI_NOR_Erase_Block(0,BlockAddr,  MX25LM51245G_ERASE_64K);
      /*Reads current status of the OSPI memory*/
      while (BSP_OSPI_NOR_GetStatus(0)!=0);
        EraseStartAddress+=0x10000;
    }
    
    /*Enable Interrupts*/
     __enable_irq();
    
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
          
    StartAddress-=StartAddress%4;
    Size += (Size%4==0)?0:4-(Size%4);
    
    for(cnt=0; cnt<Size ; cnt+=4)
    {
      Val = *(uint32_t*)StartAddress;
      if(missalignementAddress)
      {
        switch (missalignementAddress)
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
        switch (Size-missalignementSize)
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

KeepInCompilation uint64_t Verify (uint32_t MemoryAddr, uint32_t RAMBufferAddr, uint32_t Size, uint32_t missalignement)
{
    uint32_t VerifiedData = 0, InitVal = 0;
    uint64_t checksum;
    Size*=4;

    BSP_OSPI_NOR_EnableMemoryMappedMode(0);
    checksum = CheckSum((uint32_t)MemoryAddr + (missalignement & 0xf), Size - ((missalignement >> 16) & 0xF), InitVal);
    while (Size>VerifiedData)
    {
      if ( *(uint8_t*)MemoryAddr++ != *((uint8_t*)RAMBufferAddr + VerifiedData))
        return ((checksum<<32) + (MemoryAddr + VerifiedData));  
     
      VerifiedData++;  
    }
          
    return (checksum<<32);
}
/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 280000000 (CPU Clock)
  *            HCLK(Hz)                       = 280000000 (Bus matrix and AHBs Clock)
  *            AHB Prescaler                  = 1
  *            CD APB3 Prescaler              = 2 (APB3 Clock  140MHz)
  *            CD APB1 Prescaler              = 2 (APB1 Clock  140MHz)
  *            CD APB2 Prescaler              = 2 (APB2 Clock  140MHz)
  *            SRD APB4 Prescaler             = 2 (APB4 Clock  140MHz)
  *            HSE Frequency(Hz)              = 24000000
  *            PLL_M                          = 12
  *            PLL_N                          = 280
  *            PLL_P                          = 2
  *            PLL_Q                          = 2
  *            PLL_R                          = 2
  *            VDD(V)                         = 3.3
  *            Flash Latency(WS)              = 6
  * @param  None
  * @retval None
  */

static void SystemClock_Config(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;
    HAL_StatusTypeDef ret = HAL_OK;

    /*!< Supply configuration update enable */
    HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);

    /* The voltage scaling allows optimizing the power consumption when the device is
    clocked below the maximum system frequency, to update the voltage scaling value
    regarding system frequency refer to product datasheet.
    */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

    while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

    /* Enable HSE Oscillator and activate PLL with HSE as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
    RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

    RCC_OscInitStruct.PLL.PLLM = 12;
    RCC_OscInitStruct.PLL.PLLN = 280;
    RCC_OscInitStruct.PLL.PLLFRACN = 0;
    RCC_OscInitStruct.PLL.PLLP = 2;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLQ = 2;

    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_1;
    ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
    if(ret != HAL_OK)
    {
      while(1);
    }

    /* Select PLL as system clock source and configure  bus clocks dividers */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_D1PCLK1 | RCC_CLOCKTYPE_PCLK1 | \
      RCC_CLOCKTYPE_PCLK2  | RCC_CLOCKTYPE_D3PCLK1);

    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.SYSCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;
    ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6);
    if(ret != HAL_OK)
    {
      while(1);
    }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
