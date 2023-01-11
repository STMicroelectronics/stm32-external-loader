/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    octospi.c
  * @brief   This file provides code for the configuration
  *          of the OCTOSPI instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "octospi.h"

/* USER CODE BEGIN 0 */
static uint8_t QSPI_WriteEnable(void);
static uint8_t QSPI_AutoPollingMemReady(void);
static uint8_t QSPI_Configuration(void);
static uint8_t QSPI_ResetChip(void);

static uint8_t QSPI_ReadChipId(void);

#include <stdbool.h>

typedef union
{
	struct{
		bool timeout:1;
		bool transfer:1;
		bool dma:1;
		bool invalidParameter:1;
		bool invalidSequence:1;
	#if defined (USE_HAL_OSPI_REGISTER_CALLBACKS) && (USE_HAL_OSPI_REGISTER_CALLBACKS == 1U)
		bool invalidCallback:1;
	#else
		bool reserved0:1;
	#endif
		uint32_t reserved1:26;
	};
	uint32_t errorCode;
}
OspiErrorCodeType;

//#define USE_DDR

/* USER CODE END 0 */

OSPI_HandleTypeDef hospi1;

/* OCTOSPI1 init function */
void MX_OCTOSPI1_Init(void)
{

  /* USER CODE BEGIN OCTOSPI1_Init 0 */

  /* USER CODE END OCTOSPI1_Init 0 */

  OSPIM_CfgTypeDef sOspiManagerCfg = {0};

  /* USER CODE BEGIN OCTOSPI1_Init 1 */

  /* USER CODE END OCTOSPI1_Init 1 */
  hospi1.Instance = OCTOSPI1;
  hospi1.Init.FifoThreshold = 4;
  hospi1.Init.DualQuad = HAL_OSPI_DUALQUAD_ENABLE;
  hospi1.Init.MemoryType = HAL_OSPI_MEMTYPE_MICRON;
  hospi1.Init.DeviceSize = 32;
  hospi1.Init.ChipSelectHighTime = 1;
  hospi1.Init.FreeRunningClock = HAL_OSPI_FREERUNCLK_DISABLE;
  hospi1.Init.ClockMode = HAL_OSPI_CLOCK_MODE_0;
  hospi1.Init.WrapSize = HAL_OSPI_WRAP_NOT_SUPPORTED;
  hospi1.Init.ClockPrescaler = 54;
  hospi1.Init.SampleShifting = HAL_OSPI_SAMPLE_SHIFTING_NONE;
  hospi1.Init.DelayHoldQuarterCycle = HAL_OSPI_DHQC_DISABLE;
  hospi1.Init.ChipSelectBoundary = 0;
  hospi1.Init.DelayBlockBypass = HAL_OSPI_DELAY_BLOCK_BYPASSED;
  hospi1.Init.MaxTran = 0;
  hospi1.Init.Refresh = 0;
  if (HAL_OSPI_Init(&hospi1) != HAL_OK)
  {
    Error_Handler();
  }
  sOspiManagerCfg.ClkPort = 1;
  sOspiManagerCfg.NCSPort = 1;
  sOspiManagerCfg.IOLowPort = HAL_OSPIM_IOPORT_1_LOW;
  sOspiManagerCfg.IOHighPort = HAL_OSPIM_IOPORT_1_HIGH;
  if (HAL_OSPIM_Config(&hospi1, &sOspiManagerCfg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN OCTOSPI1_Init 2 */

  /* USER CODE END OCTOSPI1_Init 2 */

}

void HAL_OSPI_MspInit(OSPI_HandleTypeDef* ospiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(ospiHandle->Instance==OCTOSPI1)
  {
  /* USER CODE BEGIN OCTOSPI1_MspInit 0 */

  /* USER CODE END OCTOSPI1_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_OSPI;
    PeriphClkInitStruct.OspiClockSelection = RCC_OSPICLKSOURCE_D1HCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* OCTOSPI1 clock enable */
    __HAL_RCC_OCTOSPIM_CLK_ENABLE();
    __HAL_RCC_OSPI1_CLK_ENABLE();

    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**OCTOSPI1 GPIO Configuration
    PE2     ------> OCTOSPIM_P1_IO2
    PF6     ------> OCTOSPIM_P1_IO3
    PF8     ------> OCTOSPIM_P1_IO0
    PF9     ------> OCTOSPIM_P1_IO1
    PF10     ------> OCTOSPIM_P1_CLK
    PC1     ------> OCTOSPIM_P1_IO4
    PC2_C     ------> OCTOSPIM_P1_IO5
    PG6     ------> OCTOSPIM_P1_NCS
    PD6     ------> OCTOSPIM_P1_IO6
    PG14     ------> OCTOSPIM_P1_IO7
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_OCTOSPIM_P1;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPIM_P1;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_OCTOSPIM_P1;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPIM_P1;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_OCTOSPIM_P1;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPIM_P1;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPIM_P1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_OCTOSPIM_P1;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /* USER CODE BEGIN OCTOSPI1_MspInit 1 */

  /* USER CODE END OCTOSPI1_MspInit 1 */
  }
}

void HAL_OSPI_MspDeInit(OSPI_HandleTypeDef* ospiHandle)
{

  if(ospiHandle->Instance==OCTOSPI1)
  {
  /* USER CODE BEGIN OCTOSPI1_MspDeInit 0 */

  /* USER CODE END OCTOSPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_OCTOSPIM_CLK_DISABLE();
    __HAL_RCC_OSPI1_CLK_DISABLE();

    /**OCTOSPI1 GPIO Configuration
    PE2     ------> OCTOSPIM_P1_IO2
    PF6     ------> OCTOSPIM_P1_IO3
    PF8     ------> OCTOSPIM_P1_IO0
    PF9     ------> OCTOSPIM_P1_IO1
    PF10     ------> OCTOSPIM_P1_CLK
    PC1     ------> OCTOSPIM_P1_IO4
    PC2_C     ------> OCTOSPIM_P1_IO5
    PG6     ------> OCTOSPIM_P1_NCS
    PD6     ------> OCTOSPIM_P1_IO6
    PG14     ------> OCTOSPIM_P1_IO7
    */
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_2);

    HAL_GPIO_DeInit(GPIOF, GPIO_PIN_6|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10);

    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1|GPIO_PIN_2);

    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_6|GPIO_PIN_14);

    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_6);

  /* USER CODE BEGIN OCTOSPI1_MspDeInit 1 */

  /* USER CODE END OCTOSPI1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* QUADSPI init function */
uint8_t CSP_QUADSPI_Init(void) {
	//prepare QSPI peripheral for ST-Link Utility operations
	if (HAL_OSPI_DeInit(&hospi1) != HAL_OK) {
		return HAL_ERROR;
	}

	MX_OCTOSPI1_Init();



	if (QSPI_ResetChip() != HAL_OK) {
		return HAL_ERROR;
	}

	HAL_Delay(1); // todo remove magic number

	QSPI_ReadChipId();
//
//	HAL_Delay(1);
//
//	if (QSPI_AutoPollingMemReady() != HAL_OK) {
//		return HAL_ERROR;
//	}
//
//	if (QSPI_WriteEnable() != HAL_OK) {
//
//		return HAL_ERROR;
//	}
//
//	if (QSPI_Configuration() != HAL_OK) {
//		return HAL_ERROR;
//	}

	return HAL_OK;
}


uint8_t CSP_QSPI_Erase_Chip(void)
{
	OSPI_RegularCmdTypeDef sCommand = {};


	if (QSPI_WriteEnable() != HAL_OK) {
		return HAL_ERROR;
	}

	/* Erasing Sequence --------------------------------- */

	sCommand.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG;
	sCommand.FlashId = 0; //only applies if Dualquad is disabled
	sCommand.Instruction = CHIP_ERASE_CMD;
	sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;
	sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;
	sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
	sCommand.Address = 0;
	sCommand.AddressMode = HAL_OSPI_ADDRESS_NONE;
	sCommand.AddressSize = HAL_OSPI_ADDRESS_24_BITS;
	sCommand.AddressDtrMode = HAL_OSPI_ADDRESS_DTR_DISABLE;
	sCommand.AlternateBytes = 0;
	sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	sCommand.AlternateBytesSize = 0;
	sCommand.AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE;
	sCommand.DataMode = HAL_OSPI_DATA_NONE;
	sCommand.NbData = 0; //?
	sCommand.DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE;
	sCommand.DummyCycles = 0;
	sCommand.DQSMode = HAL_OSPI_DQS_DISABLE; // no data strobe used
	sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;


	if (HAL_OSPI_Command(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE)
			!= HAL_OK) {
		return HAL_ERROR;
	}

	if (QSPI_AutoPollingMemReady() != HAL_OK) {
				return HAL_ERROR;
			}

	return HAL_OK;
}

static uint8_t QSPI_AutoPollingMemReady(void)
{
	OSPI_AutoPollingTypeDef sConfig = {};

	// todo: "avoid the auto-poll operations"

	sConfig.Match = 0x00;
	sConfig.Mask = 0x01;
	sConfig.MatchMode = HAL_OSPI_MATCH_MODE_AND;
	sConfig.AutomaticStop = HAL_OSPI_AUTOMATIC_STOP_ENABLE;
	sConfig.Interval = 0x10;

	if (HAL_OSPI_AutoPolling(&hospi1, &sConfig,	HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
		return HAL_ERROR;
	}

	return HAL_OK;

//	return HAL_ERROR;
}


static uint8_t QSPI_WriteEnable(void)
{
	OSPI_RegularCmdTypeDef sCommand = {};
	OSPI_AutoPollingTypeDef sConfig = {};

	/* Enable write operations ------------------------------------------ */
	sCommand.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG;
	sCommand.FlashId = 0; //only applies if Dualquad is disabled
	sCommand.Instruction = WRITE_ENABLE_CMD;
	sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;
	sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;
	sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
	sCommand.Address = 0;
	sCommand.AddressMode = HAL_OSPI_ADDRESS_NONE;
	sCommand.AddressSize = HAL_OSPI_ADDRESS_24_BITS;
	sCommand.AddressDtrMode = HAL_OSPI_ADDRESS_DTR_DISABLE;
	sCommand.AlternateBytes = 0;
	sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	sCommand.AlternateBytesSize = 0;
	sCommand.AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE;
	sCommand.DataMode = HAL_OSPI_DATA_NONE;
	sCommand.NbData = 0; //?
	sCommand.DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE;
	sCommand.DummyCycles = 0;
	sCommand.DQSMode = HAL_OSPI_DQS_DISABLE; // no data strobe used
	sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;

	if (HAL_OSPI_Command(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE)
			!= HAL_OK) {
		return HAL_ERROR;
	}

	/* Configure automatic polling mode to wait for write enabling ---- */
	sConfig.Match = 0x02;
	sConfig.Mask = 0x02;
	sConfig.MatchMode = HAL_OSPI_MATCH_MODE_AND;
	sConfig.Interval = 0x10;
	sConfig.AutomaticStop = HAL_OSPI_AUTOMATIC_STOP_ENABLE;

	if (HAL_OSPI_AutoPolling(&hospi1, &sConfig, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
		return HAL_ERROR;
	}

	return HAL_OK;
}

/*Enable quad mode and set dummy cycles count*/
static uint8_t QSPI_Configuration(void)
{
	OSPI_RegularCmdTypeDef sCommand = {};
	uint8_t test_buffer[4] = { 0 };
	/*read status register*/

	sCommand.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG; //?
	sCommand.FlashId = 0; //only applies if Dualquad is disabled
	sCommand.Instruction = READ_STATUS_REG_CMD;
	sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;
	sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;
	sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
	sCommand.Address = 0;
	sCommand.AddressMode = HAL_OSPI_ADDRESS_NONE;
	sCommand.AddressSize = HAL_OSPI_ADDRESS_24_BITS;
	sCommand.AddressDtrMode = HAL_OSPI_ADDRESS_DTR_DISABLE;
	sCommand.AlternateBytes = 0;
	sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	sCommand.AlternateBytesSize = 0;
	sCommand.AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE;
	sCommand.DataMode = HAL_OSPI_DATA_NONE;
	sCommand.NbData = 1;//why only 1 byte?
	sCommand.DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE;
	sCommand.DummyCycles = 0;
	sCommand.DQSMode = HAL_OSPI_DQS_DISABLE; // no data strobe used
	sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;

	if (HAL_OSPI_Command(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE)	!= HAL_OK) {
		return HAL_ERROR;
	}
	if (HAL_OSPI_Receive(&hospi1, test_buffer, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
		return HAL_ERROR;
	}
	//read 2nd half or other die?
	if (HAL_OSPI_Command(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE)	!= HAL_OK) {
		return HAL_ERROR;
	}
	if (HAL_OSPI_Receive(&hospi1, &(test_buffer[1]),	HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
		return HAL_ERROR;
	}
	/*modify buffer to enable quad mode*/
	test_buffer[0] |= 0x40;

	/*set dummy cycles*/
	test_buffer[1] |= 0xC0;

	sCommand.NbData = 2;

	if (HAL_OSPI_Command(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE)	!= HAL_OK) {
		return HAL_ERROR;
	}

	if (HAL_OSPI_Transmit(&hospi1, test_buffer, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
		Error_Handler();
		return HAL_ERROR;
	}
	return HAL_OK;
}

uint8_t CSP_QSPI_EraseSector(uint32_t EraseStartAddress, uint32_t EraseEndAddress)
{
	OSPI_RegularCmdTypeDef sCommand = {};

	EraseStartAddress &= (MEMORY_SECTOR_SIZE -1);

	/* Erasing Sequence -------------------------------------------------- */

	sCommand.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG; //?
	sCommand.FlashId = 0; //only applies if Dualquad is disabled
	sCommand.Instruction = SECTOR_ERASE_CMD;
	sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE; // todo: check this, quad should be enabled here
	sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;
	sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
	sCommand.Address = 0;
	sCommand.AddressMode = HAL_OSPI_ADDRESS_NONE;
	sCommand.AddressSize = HAL_OSPI_ADDRESS_24_BITS;
	sCommand.AddressDtrMode = HAL_OSPI_ADDRESS_DTR_DISABLE;
	sCommand.AlternateBytes = 0;
	sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	sCommand.AlternateBytesSize = 0;
	sCommand.AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE;
	sCommand.DataMode = HAL_OSPI_DATA_NONE;
	sCommand.NbData = 0;
	sCommand.DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE;
	sCommand.DummyCycles = 0;
	sCommand.DQSMode = HAL_OSPI_DQS_DISABLE; // no data strobe used
	sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;

	while (EraseEndAddress >= EraseStartAddress) {
		sCommand.Address = (EraseStartAddress & 0x0FFFFFFF);

		if (QSPI_WriteEnable() != HAL_OK) {
			return HAL_ERROR;
		}

		if (HAL_OSPI_Command(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE)
				!= HAL_OK) {
			return HAL_ERROR;
		}
		EraseStartAddress += MEMORY_SECTOR_SIZE;

		if (QSPI_AutoPollingMemReady() != HAL_OK) {
			return HAL_ERROR;
		}
	}

	return HAL_OK;
}

uint8_t CSP_QSPI_WriteMemory(const uint8_t* buffer, uint32_t address, uint32_t buffer_size)
{
	OSPI_RegularCmdTypeDef sCommand = {};
	uint32_t end_addr, current_size, current_addr;

	//todo check size in command since half the data goes to one die and half the data goes to the other die

	/* Calculation of the size between the write address and the end of the page */
	// todo improve this algoritm
	current_addr = 0;
	while (current_addr <= address) {
		current_addr += MEMORY_PAGE_SIZE*2;
	}
	current_size = current_addr - address;


	/* Check if the size of the data is less than the remaining place in the page */
	if (current_size > buffer_size) {
		current_size = buffer_size;
	}

	/* Initialize the address variables */
	current_addr = address;
	end_addr = address + buffer_size;

	sCommand.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG; //?
	sCommand.FlashId = 0; //only applies if Dualquad is disabled
	sCommand.Instruction = QUAD_IN_FAST_PROG_CMD;
	sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;
	sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;
	sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
	sCommand.Address = address;
	sCommand.AddressMode = HAL_OSPI_ADDRESS_4_LINES;
	sCommand.AddressSize = HAL_OSPI_ADDRESS_24_BITS;
	sCommand.AddressDtrMode = HAL_OSPI_ADDRESS_DTR_DISABLE;
	sCommand.AlternateBytes = 0;
	sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	sCommand.AlternateBytesSize = 0;
	sCommand.AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE;
	sCommand.DataMode = HAL_OSPI_DATA_4_LINES;
	sCommand.NbData = buffer_size / 2; //todo check size
	sCommand.DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE;
	sCommand.DummyCycles = 0;
	sCommand.DQSMode = HAL_OSPI_DQS_DISABLE; // no data strobe used
	sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;

	/* Perform the write page by page */
	do {
		sCommand.Address = current_addr;
		sCommand.NbData = current_size;

		if (current_size == 0) {
			return HAL_OK;
		}

		/* Enable write operations */
		if (QSPI_WriteEnable() != HAL_OK) {
			return HAL_ERROR;
		}

		/* Configure the command */
		if (HAL_OSPI_Command(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE)
				!= HAL_OK) {

			return HAL_ERROR;
		}

		/* Transmission of the data */
		if (HAL_OSPI_Transmit(&hospi1, (uint8_t*)buffer, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
			return HAL_ERROR;
		}

		/* Configure automatic polling mode to wait for end of program */
		if (QSPI_AutoPollingMemReady() != HAL_OK) {
			return HAL_ERROR;
		}

		/* Update the address and size variables for next page programming */
		current_addr += current_size;
		buffer += current_size;
		current_size =	((current_addr + MEMORY_PAGE_SIZE*2) > end_addr) ?
						(end_addr - current_addr) : MEMORY_PAGE_SIZE*2;
	} while (current_addr <= end_addr);

	return HAL_OK;
}


uint8_t CSP_QSPI_EnableMemoryMappedMode(void)
{
	OSPI_MemoryMappedTypeDef sMemMappedCfg = {};

	/* Enable Memory-Mapped mode-------------------------------------------------- */


	sMemMappedCfg.TimeOutActivation = HAL_OSPI_TIMEOUT_COUNTER_DISABLE;
	sMemMappedCfg.TimeOutPeriod = 0;

	if (HAL_OSPI_MemoryMapped(&hospi1, &sMemMappedCfg) != HAL_OK) {
		return HAL_ERROR;
	}
	return HAL_OK;
}

static uint8_t QSPI_ResetChip()
{
	OSPI_RegularCmdTypeDef sCommand = {};
	uint32_t temp = 0;
	HAL_StatusTypeDef res = HAL_OK;
	OspiErrorCodeType errorCode = {0};
	/* Erasing Sequence -------------------------------------------------- */

	sCommand.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG; //?
	sCommand.FlashId = 0; //only applies if Dualquad is disabled
	sCommand.Instruction = RESET_ENABLE_CMD;
	sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;
	sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;
	sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
	sCommand.Address = 0;
	sCommand.AddressMode = HAL_OSPI_ADDRESS_NONE;
	sCommand.AddressSize = HAL_OSPI_ADDRESS_24_BITS;
	sCommand.AddressDtrMode = HAL_OSPI_ADDRESS_DTR_DISABLE;
	sCommand.AlternateBytes = 0;
	sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	sCommand.AlternateBytesSize = 0;
	sCommand.AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE;
	sCommand.DataMode = HAL_OSPI_DATA_NONE;
	sCommand.NbData = 0;
	sCommand.DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE;
	sCommand.DummyCycles = 0;
	sCommand.DQSMode = HAL_OSPI_DQS_DISABLE; // no data strobe used
	sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;


	res = HAL_OSPI_Command(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE);

	if (res == HAL_OK)
	{
		//delay
		//todo remove magic number
		for (temp = 0; temp < 47; temp++)
		{
			__NOP();
		}

		sCommand.Instruction = RESET_EXECUTE_CMD;
		res = HAL_OSPI_Command(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE);
	}


	// if an error occured: get error code:
	if (res != HAL_OK)
	{
		errorCode.errorCode = hospi1.ErrorCode;
	}


	return res;
}



static uint8_t QSPI_ReadChipId(void)
{
	OSPI_RegularCmdTypeDef sCommand = {};
	uint8_t test_buffer[20*2] = { 0 };
	HAL_StatusTypeDef res = HAL_OK;
	OspiErrorCodeType errorCode = {0};
	int i;

	/*read status register*/

	for(i=1;i<8;++i)
	{
		sCommand.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG; //
		sCommand.FlashId = 0; //only applies if Dualquad is disabled
		sCommand.Instruction = READ_ID_CMD;
		sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;
		sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;
		sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
		sCommand.Address = 0;
		sCommand.AddressMode = HAL_OSPI_ADDRESS_NONE;
		sCommand.AddressSize = HAL_OSPI_ADDRESS_24_BITS;
		sCommand.AddressDtrMode = HAL_OSPI_ADDRESS_DTR_DISABLE;
		sCommand.AlternateBytes = 0;
		sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
		sCommand.AlternateBytesSize = 0;
		sCommand.AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE;
		sCommand.DataMode = HAL_OSPI_DATA_1_LINE;
		sCommand.NbData = i*2; //id needs to be read twice, so total number of bytes is doubled
		sCommand.DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE;
		sCommand.DummyCycles = 0;
		sCommand.DQSMode = HAL_OSPI_DQS_DISABLE; // no data strobe used
		sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;

		res = HAL_OSPI_Command(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE);

		if (res == HAL_OK)
		{
			if(sCommand.NbData > 0) // HAL_OSPI_Receive() crashes when length = 0!
			{
				res = HAL_OSPI_Receive(&hospi1, test_buffer, HAL_OSPI_TIMEOUT_DEFAULT_VALUE);
			}
		}

		if (res != HAL_OK)
		{
			errorCode.errorCode = hospi1.ErrorCode;
			break;
		}
		HAL_Delay(1);
	}

	return res;
}


/* USER CODE END 1 */
