/**
  ******************************************************************************
  * @file    stm32l476g_eval_nor.c
  * @author  MCD Application Team
  * @brief   This file includes a standard driver for the M29W128GL NOR 
  *          memories mounted on STM32L476G-EVAL board.
  @verbatim
  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================  
  [..] 
   (#) This driver is used to drive the M29W128GL NOR flash external 
       memory mounted on STM32L476G-EVAL evaluation board.
       
   (#) This driver does not need a specific component driver for the NOR device
       to be included with.

   (#) Initialization steps:
       (++) Initialize the NOR external memory using the BSP_NOR_Init() function. This 
            function includes the MSP layer hardware resources initialization and the
            FMC controller configuration to interface with the external NOR memory.
  
   (#) NOR flash operations
       (++) NOR external memory can be accessed with read/write operations once it is
            initialized.
            Read/write operation can be performed with AHB access using the functions
            BSP_NOR_ReadData()/BSP_NOR_WriteData(). The BSP_NOR_WriteData() performs write operation
            of an amount of data by unit (halfword). You can also perform a program data
            operation of an amount of data using the function BSP_NOR_ProgramData().
       (++) The function BSP_NOR_Read_ID() returns the chip IDs stored in the structure 
            "NOR_IDTypeDef". (see the NOR IDs in the memory data sheet)
       (++) Perform erase block operation using the function BSP_NOR_Erase_Block() and by
            specifying the block address. You can perform an erase operation of the whole 
            chip by calling the function BSP_NOR_Erase_Chip(). 
       (++) After other operations, the function BSP_NOR_ReturnToReadMode() allows the NOR 
            flash to return to read mode to perform read operations on it. 
  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32l476g_eval_nor.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L476G_EVAL
  * @{
  */ 
  
/** @defgroup STM32L476G_EVAL_NOR STM32L476G-EVAL NOR
  * @{
  */ 


/* Private variables ---------------------------------------------------------*/

/** @defgroup STM32L476G_EVAL_NOR_Private_Variables Private Variables
  * @{
  */       
static NOR_HandleTypeDef norHandle = {0};
static FMC_NORSRAM_TimingTypeDef Timing = {0};

/**
  * @}
  */ 



/* Private functions ---------------------------------------------------------*/
    
/** @defgroup STM32L476G_EVAL_NOR_Private_Functions Private Functions
  * @{
  */ 
static void NOR_MspInit(void);

/**
  * @}
  */
    
/** @defgroup STM32L476G_EVAL_NOR_Exported_Functions Exported Functions
  * @{
  */ 

/**
  * @brief  Initializes the NOR device.
  * @retval NOR memory status
  */
uint8_t BSP_NOR_Init(void)
{ 
  
  norHandle.Instance  = FMC_NORSRAM_DEVICE;
  norHandle.Extended  = FMC_NORSRAM_EXTENDED_DEVICE;
  
  /* NOR device configuration */  
  Timing.AddressSetupTime       = 3;
  Timing.AddressHoldTime        = 1;
  Timing.DataSetupTime          = 4;
  Timing.BusTurnAroundDuration  = 0;
  Timing.CLKDivision            = 2;
  Timing.DataLatency            = 2;
  Timing.AccessMode             = FMC_ACCESS_MODE_A;
  
  norHandle.Init.NSBank             = FMC_NORSRAM_BANK2;
  norHandle.Init.DataAddressMux     = FMC_DATA_ADDRESS_MUX_DISABLE;
  norHandle.Init.MemoryType         = FMC_MEMORY_TYPE_NOR;
  norHandle.Init.MemoryDataWidth    = NOR_MEMORY_WIDTH;
  norHandle.Init.BurstAccessMode    = NOR_BURSTACCESS;
  norHandle.Init.WaitSignalPolarity = FMC_WAIT_SIGNAL_POLARITY_LOW;
  norHandle.Init.WaitSignalActive   = FMC_WAIT_TIMING_BEFORE_WS;
  norHandle.Init.WriteOperation     = FMC_WRITE_OPERATION_ENABLE;
  norHandle.Init.WaitSignal         = FMC_WAIT_SIGNAL_DISABLE;
  norHandle.Init.ExtendedMode       = FMC_EXTENDED_MODE_DISABLE;
  norHandle.Init.AsynchronousWait   = FMC_ASYNCHRONOUS_WAIT_DISABLE;
  norHandle.Init.WriteBurst         = NOR_WRITEBURST;
  norHandle.Init.ContinuousClock    = FMC_CONTINUOUS_CLOCK_SYNC_ONLY;
  norHandle.Init.WriteFifo          = FMC_WRITE_FIFO_DISABLE;
  norHandle.Init.PageSize           = FMC_PAGE_SIZE_NONE;

  /* NOR controller initialization */
  NOR_MspInit();
  
  if(HAL_NOR_Init(&norHandle, &Timing, &Timing) != HAL_OK)
  {
    return NOR_STATUS_ERROR;
  }
  else
  {
    return NOR_STATUS_OK;
  }
}

/**
  * @brief  Reads an amount of data from the NOR device.
  * @param  uwStartAddress: Read start address
  * @param  pData: Pointer to data to be read
  * @param  uwDataSize: Size of data to read    
  * @retval NOR memory status
  */
uint8_t BSP_NOR_ReadData(uint32_t uwStartAddress, uint16_t* pData, uint32_t uwDataSize)
{
  if(HAL_NOR_ReadBuffer(&norHandle, NOR_DEVICE_ADDR + uwStartAddress, pData, uwDataSize) != HAL_OK)
  {
    return NOR_STATUS_ERROR;
  }
  else
  {
    return NOR_STATUS_OK;
  }
}

/**
  * @brief  Returns the NOR memory to read mode.
  * @retval None
  */
void BSP_NOR_ReturnToReadMode(void)
{
   HAL_NOR_ReturnToReadMode(&norHandle);
}

/**
  * @brief  Writes an amount of data to the NOR device.
  * @param  uwStartAddress: Write start address
  * @param  pData: Pointer to data to be written
  * @param  uwDataSize: Size of data to write    
  * @retval NOR memory status
  */
uint8_t BSP_NOR_WriteData(uint32_t uwStartAddress, uint16_t* pData, uint32_t uwDataSize)
{
  uint32_t index = uwDataSize;
  
  while(index > 0)
  {
    /* Write data to NOR */
    HAL_NOR_Program(&norHandle, (uint32_t *)(NOR_DEVICE_ADDR + uwStartAddress), pData);
    
    /* Read NOR device status */
    if(HAL_NOR_GetStatus(&norHandle, NOR_DEVICE_ADDR, PROGRAM_TIMEOUT) != HAL_NOR_STATUS_SUCCESS)
    {
      return NOR_STATUS_ERROR;
    }
    
    /* Update the counters */
    index--;
    uwStartAddress += 2;
    pData++; 
  }
  
  return NOR_STATUS_OK;
}

/**
  * @brief  Programs an amount of data to the NOR device.
  * @param  uwStartAddress: Write start address
  * @param  pData: Pointer to data to be written
  * @param  uwDataSize: Size of data to write    
  * @retval NOR memory status
  */
uint8_t BSP_NOR_ProgramData(uint32_t uwStartAddress, uint16_t* pData, uint32_t uwDataSize)
{
  /* Send NOR program buffer operation */
  HAL_NOR_ProgramBuffer(&norHandle, uwStartAddress, pData, uwDataSize);
  
  /* Return the NOR memory status */
  if(HAL_NOR_GetStatus(&norHandle, NOR_DEVICE_ADDR, PROGRAM_TIMEOUT) != HAL_NOR_STATUS_SUCCESS)
  {
    return NOR_STATUS_ERROR;
  }
  else
  {
    return NOR_STATUS_OK;
  }
}

/**
  * @brief  Erases the specified block of the NOR device. 
  * @param  BlockAddress: Block address to erase  
  * @retval NOR memory status
  */
uint8_t BSP_NOR_Erase_Block(uint32_t BlockAddress)
{
  /* Send NOR erase block operation */
  HAL_NOR_Erase_Block(&norHandle, BlockAddress, NOR_DEVICE_ADDR);
  
  /* Return the NOR memory status */  
  if(HAL_NOR_GetStatus(&norHandle, NOR_DEVICE_ADDR, BLOCKERASE_TIMEOUT) != HAL_NOR_STATUS_SUCCESS)
  {
    return NOR_STATUS_ERROR;
  }
  else
  {
    return NOR_STATUS_OK;
  }
}

/**
  * @brief  Erases the entire NOR chip.
  * @retval NOR memory status
  */
uint8_t BSP_NOR_Erase_Chip(void)
{
  /* Send NOR Erase chip operation */
  HAL_NOR_Erase_Chip(&norHandle, NOR_DEVICE_ADDR);
  
  /* Return the NOR memory status */
  if(HAL_NOR_GetStatus(&norHandle, NOR_DEVICE_ADDR, CHIPERASE_TIMEOUT) != HAL_NOR_STATUS_SUCCESS)
  {
    return NOR_STATUS_ERROR;
  }
  else
  {
    return NOR_STATUS_OK;
  } 
}

/**
  * @brief  Reads NOR flash IDs.
  * @param  pNOR_ID : Pointer to NOR ID structure
  * @retval NOR memory status
  */
uint8_t BSP_NOR_Read_ID(NOR_IDTypeDef *pNOR_ID)
{
  if(HAL_NOR_Read_ID(&norHandle, pNOR_ID) != HAL_OK)
  {
    return NOR_STATUS_ERROR;
  }
  else
  {
    return NOR_STATUS_OK;
  }
}

/**
  * @}
  */

/** @addtogroup STM32L476G_EVAL_NOR_Private_Functions 
  * @{
  */ 

/**
  * @brief  Initializes the NOR MSP.
  * @retval None
  */
static void NOR_MspInit(void)
{
  GPIO_InitTypeDef GPIO_Init_Structure;

  /* Enable FMC clock */
  __HAL_RCC_FMC_CLK_ENABLE();

  /* Enable GPIOs clock */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWREx_EnableVddIO2();
  
  /* Common GPIO configuration */
  GPIO_Init_Structure.Mode      = GPIO_MODE_AF_PP;
  GPIO_Init_Structure.Pull      = GPIO_PULLDOWN;
  GPIO_Init_Structure.Speed     = GPIO_SPEED_HIGH;
  GPIO_Init_Structure.Alternate = GPIO_AF12_FMC;
  
  /*## Data Bus #######*/
  /* GPIOD configuration */
  GPIO_Init_Structure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 |
                              GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOD, &GPIO_Init_Structure);

  /* GPIOE configuration */  
  GPIO_Init_Structure.Pin   = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |
                              GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 |
                              GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOE, &GPIO_Init_Structure);
  
  /*## Address Bus #######*/
  /* GPIOF configuration */  
  GPIO_Init_Structure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                              GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_12 | GPIO_PIN_13 |
                              GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOF, &GPIO_Init_Structure);

  /* GPIOG configuration */  
  GPIO_Init_Structure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 |
                              GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
  HAL_GPIO_Init(GPIOG, &GPIO_Init_Structure);
  
  /* GPIOD configuration */
  GPIO_Init_Structure.Pin   = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13;
  HAL_GPIO_Init(GPIOD, &GPIO_Init_Structure);

  /* GPIOE configuration */
  GPIO_Init_Structure.Pin   = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 |
                              GPIO_PIN_6 | GPIO_PIN_2;
  HAL_GPIO_Init(GPIOE, &GPIO_Init_Structure);

  /*## NOE and NWE configuration #######*/ 
  GPIO_Init_Structure.Pin = GPIO_PIN_4 |GPIO_PIN_5;
  HAL_GPIO_Init(GPIOD, &GPIO_Init_Structure);
  
  /*## LCD_DO, NBL1 configuration #######*/
  GPIO_Init_Structure.Pin = GPIO_PIN_6;
  HAL_GPIO_Init(GPIOA, &GPIO_Init_Structure);

  /*## NE3 configuration #######*/
  GPIO_Init_Structure.Mode      = GPIO_MODE_OUTPUT_PP;
  GPIO_Init_Structure.Pull      = GPIO_PULLUP;
  GPIO_Init_Structure.Speed     = GPIO_SPEED_HIGH;
  GPIO_Init_Structure.Pin = GPIO_PIN_10;
  HAL_GPIO_Init(GPIOG, &GPIO_Init_Structure);
  
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_SET);

  /*## NE2/NCE3 configuration ######*/
  GPIO_Init_Structure.Mode      = GPIO_MODE_AF_PP;
  GPIO_Init_Structure.Pull      = GPIO_PULLDOWN;
  GPIO_Init_Structure.Speed     = GPIO_SPEED_HIGH;
  GPIO_Init_Structure.Alternate = GPIO_AF12_FMC;
  GPIO_Init_Structure.Pin = GPIO_PIN_9;
  HAL_GPIO_Init(GPIOG, &GPIO_Init_Structure);

  /*## Configure PD6 for NOR memory Ready/Busy signal ######*/
  GPIO_Init_Structure.Pin       = GPIO_PIN_6;
  HAL_GPIO_Init(GPIOD, &GPIO_Init_Structure);
}

/**
  * @brief  NOR BSP Wait for Ready/Busy signal.
  * @param  hnor: Pointer to NOR handle
  * @param  Timeout: Timeout duration  
  * @retval None
  */
void HAL_NOR_MspWait(NOR_HandleTypeDef *hnor, uint32_t Timeout)
{
#if defined(NOR_READY_SB10)
  uint32_t timeout = Timeout;
  
  /* Polling on Ready/Busy signal */
  while((HAL_GPIO_ReadPin(NOR_READY_BUSY_GPIO, NOR_READY_BUSY_PIN) != NOR_BUSY_STATE) && (timeout > 0)) 
  {
    timeout--;
  }
  
  timeout = Timeout;
  
  /* Polling on Ready/Busy signal */
  while((HAL_GPIO_ReadPin(NOR_READY_BUSY_GPIO, NOR_READY_BUSY_PIN) != NOR_READY_STATE) && (timeout > 0)) 
  {
    timeout--;
  }  
#else
  /* Wait 1 ms */
//  HAL_Delay(1);
  for(uint32_t i = 100; i != 0; i--);
#endif
}

/**
  * @}
  */  
  
/**
  * @}
  */ 
  
/**
  * @}
  */ 
  
/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

