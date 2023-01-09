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
  hospi1.Init.ClockPrescaler = 4;
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
    PeriphClkInitStruct.PLL2.PLL2M = 1;
    PeriphClkInitStruct.PLL2.PLL2N = 32;
    PeriphClkInitStruct.PLL2.PLL2P = 45;
    PeriphClkInitStruct.PLL2.PLL2Q = 125;
    PeriphClkInitStruct.PLL2.PLL2R = 6;
    PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_2;
    PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
    PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
    PeriphClkInitStruct.OspiClockSelection = RCC_OSPICLKSOURCE_PLL2;
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

	HAL_Delay(1);

	if (QSPI_AutoPollingMemReady() != HAL_OK) {
		return HAL_ERROR;
	}

	if (QSPI_WriteEnable() != HAL_OK) {

		return HAL_ERROR;
	}

	if (QSPI_Configuration() != HAL_OK) {
		return HAL_ERROR;
	}

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
	sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_4_LINES;
	sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;
	sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_ENABLE; // todo check this
	sCommand.Address = 0;
	sCommand.AddressMode = HAL_OSPI_ADDRESS_NONE;
	sCommand.AddressSize = HAL_OSPI_ADDRESS_24_BITS;
	sCommand.AddressDtrMode = HAL_OSPI_ADDRESS_DTR_ENABLE;
	sCommand.AlternateBytes = 0;
	sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	sCommand.AlternateBytesSize = 0;
	sCommand.AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_ENABLE;
	sCommand.DataMode = HAL_OSPI_DATA_NONE;
	sCommand.NbData = 0; //?
	sCommand.DataDtrMode = HAL_OSPI_DATA_DTR_ENABLE;
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
//	QSPI_CommandTypeDef sCommand;
//	QSPI_AutoPollingTypeDef sConfig;
//
//	/* Enable write operations ------------------------------------------ */
//	sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
//	sCommand.Instruction = WRITE_ENABLE_CMD;
//	sCommand.AddressMode = QSPI_ADDRESS_NONE;
//	sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
//	sCommand.DataMode = QSPI_DATA_NONE;
//	sCommand.DummyCycles = 0;
//	sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
//	sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
//	sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
//
//	if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE)
//			!= HAL_OK) {
//		return HAL_ERROR;
//	}
//
//	/* Configure automatic polling mode to wait for write enabling ---- */
//	sConfig.Match = 0x02;
//	sConfig.Mask = 0x02;
//	sConfig.MatchMode = QSPI_MATCH_MODE_AND;
//	sConfig.StatusBytesSize = 1;
//	sConfig.Interval = 0x10;
//	sConfig.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;
//
//	sCommand.Instruction = READ_STATUS_REG_CMD;
//	sCommand.DataMode = QSPI_DATA_1_LINE;
//	if (HAL_QSPI_AutoPolling(&hqspi, &sCommand, &sConfig,
//	HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
//		return HAL_ERROR;
//	}
//
//	return HAL_OK;
	return HAL_ERROR;
}

/*Enable quad mode and set dummy cycles count*/
static uint8_t QSPI_Configuration(void)
{

//	QSPI_CommandTypeDef sCommand;
//	uint8_t test_buffer[4] = { 0 };
//	/*read status register*/
//	sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
//	sCommand.Instruction = READ_STATUS_REG_CMD;
//	sCommand.AddressMode = QSPI_ADDRESS_NONE;
//	sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
//	sCommand.DataMode = QSPI_DATA_1_LINE;
//	sCommand.DummyCycles = 0;
//	sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
//	sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
//	sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
//	sCommand.NbData = 1;
//
//	if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE)
//			!= HAL_OK) {
//		return HAL_ERROR;
//	}
//	if (HAL_QSPI_Receive(&hqspi, test_buffer,
//	HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
//		return HAL_ERROR;
//	}
//	/*read configuration register*/
//	sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
//	sCommand.Instruction = READ_CONFIGURATION_REG_CMD;
//	sCommand.AddressMode = QSPI_ADDRESS_NONE;
//	sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
//	sCommand.DataMode = QSPI_DATA_1_LINE;
//	sCommand.DummyCycles = 0;
//	sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
//	sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
//	sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
//	sCommand.NbData = 1;
//
//	if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE)
//			!= HAL_OK) {
//		return HAL_ERROR;
//	}
//	if (HAL_QSPI_Receive(&hqspi, &(test_buffer[1]),
//	HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
//		return HAL_ERROR;
//	}
//	/*modify buffer to enable quad mode*/
//	test_buffer[0] |= 0x40;
//
//	/*set dummy cycles*/
//	test_buffer[1] |= 0xC0;
//
//	sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
//	sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
//	sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
//	sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
//	sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
//	sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
//	sCommand.Instruction = WRITE_STATUS_REG_CMD;
//	sCommand.AddressMode = QSPI_ADDRESS_NONE;
//	sCommand.DataMode = QSPI_DATA_1_LINE;
//	sCommand.DummyCycles = 0;
//	sCommand.NbData = 2;
//
//	if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE)
//			!= HAL_OK) {
//		return HAL_ERROR;
//	}
//
//	if (HAL_QSPI_Transmit(&hqspi, test_buffer,
//	HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
//		Error_Handler();
//		return HAL_ERROR;
//	}
//	return HAL_OK;
	return HAL_ERROR;
}

uint8_t CSP_QSPI_EraseSector(uint32_t EraseStartAddress, uint32_t EraseEndAddress)
{
//	QSPI_CommandTypeDef sCommand;
//
//	EraseStartAddress = EraseStartAddress
//			- EraseStartAddress % MEMORY_SECTOR_SIZE;
//
//	/* Erasing Sequence -------------------------------------------------- */
//	sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
//	sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
//	sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
//	sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
//	sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
//	sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
//	sCommand.Instruction = SECTOR_ERASE_CMD;
//	sCommand.AddressMode = QSPI_ADDRESS_1_LINE;
//
//	sCommand.DataMode = QSPI_DATA_NONE;
//	sCommand.DummyCycles = 0;
//
//	while (EraseEndAddress >= EraseStartAddress) {
//		sCommand.Address = (EraseStartAddress & 0x0FFFFFFF);
//
//		if (QSPI_WriteEnable() != HAL_OK) {
//			return HAL_ERROR;
//		}
//
//		if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE)
//				!= HAL_OK) {
//			return HAL_ERROR;
//		}
//		EraseStartAddress += MEMORY_SECTOR_SIZE;
//
//		if (QSPI_AutoPollingMemReady() != HAL_OK) {
//			return HAL_ERROR;
//		}
//	}
//
//	return HAL_OK;
	return HAL_ERROR;
}

uint8_t CSP_QSPI_WriteMemory(const uint8_t* buffer, uint32_t address,uint32_t buffer_size)
{

//	QSPI_CommandTypeDef sCommand;
//	uint32_t end_addr, current_size, current_addr;
//
//	/* Calculation of the size between the write address and the end of the page */
//	current_addr = 0;
//
//
//	//
//	while (current_addr <= address) {
//		current_addr += MEMORY_PAGE_SIZE;
//	}
//	current_size = current_addr - address;
//
//	/* Check if the size of the data is less than the remaining place in the page */
//	if (current_size > buffer_size) {
//		current_size = buffer_size;
//	}
//
//	/* Initialize the adress variables */
//	current_addr = address;
//	end_addr = address + buffer_size;
//
//	sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
//	sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
//	sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
//	sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
//	sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
//	sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
//	sCommand.Instruction = QUAD_IN_FAST_PROG_CMD;
//	sCommand.AddressMode = QSPI_ADDRESS_4_LINES;
//	sCommand.DataMode = QSPI_DATA_4_LINES;
//	sCommand.NbData = buffer_size;
//	sCommand.Address = address;
//	sCommand.DummyCycles = 0;
//
//	/* Perform the write page by page */
//	do {
//		sCommand.Address = current_addr;
//		sCommand.NbData = current_size;
//
//		if (current_size == 0) {
//			return HAL_OK;
//		}
//
//		/* Enable write operations */
//		if (QSPI_WriteEnable() != HAL_OK) {
//			return HAL_ERROR;
//		}
//
//		/* Configure the command */
//		if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE)
//				!= HAL_OK) {
//
//			return HAL_ERROR;
//		}
//
//		/* Transmission of the data */
//		if (HAL_QSPI_Transmit(&hqspi, buffer, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
//
//			return HAL_ERROR;
//		}
//
//		/* Configure automatic polling mode to wait for end of program */
//		if (QSPI_AutoPollingMemReady() != HAL_OK) {
//			return HAL_ERROR;
//		}
//
//		/* Update the address and size variables for next page programming */
//		current_addr += current_size;
//		buffer += current_size;
//		current_size =
//				((current_addr + MEMORY_PAGE_SIZE) > end_addr) ?
//						(end_addr - current_addr) : MEMORY_PAGE_SIZE;
//	} while (current_addr <= end_addr);
//
//	return HAL_OK;

	return HAL_ERROR;
}


uint8_t CSP_QSPI_EnableMemoryMappedMode(void)
{
//
//	QSPI_CommandTypeDef sCommand;
//	QSPI_MemoryMappedTypeDef sMemMappedCfg;
//
//	/* Enable Memory-Mapped mode-------------------------------------------------- */
//
//	sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
//	sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
//	sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
//	sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
//	sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
//	sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
//	sCommand.AddressMode = QSPI_ADDRESS_1_LINE;
//	sCommand.DataMode = QSPI_DATA_4_LINES;
//	sCommand.NbData = 0;
//	sCommand.Address = 0;
//	sCommand.Instruction = QUAD_OUT_FAST_READ_CMD;
//	sCommand.DummyCycles = DUMMY_CLOCK_CYCLES_READ_QUAD;
//
//	sMemMappedCfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;
//
//	if (HAL_QSPI_MemoryMapped(&hqspi, &sCommand, &sMemMappedCfg) != HAL_OK) {
//		return HAL_ERROR;
//	}
//	return HAL_OK;
	return HAL_ERROR;
}

static uint8_t QSPI_ResetChip()
{
//	QSPI_CommandTypeDef sCommand;
//	uint32_t temp = 0;
//	/* Erasing Sequence -------------------------------------------------- */
//	sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
//	sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
//	sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
//	sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
//	sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
//	sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
//	sCommand.Instruction = RESET_ENABLE_CMD;
//	sCommand.AddressMode = QSPI_ADDRESS_NONE;
//	sCommand.Address = 0;
//	sCommand.DataMode = QSPI_DATA_NONE;
//	sCommand.DummyCycles = 0;
//
//	if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE)
//			!= HAL_OK) {
//		return HAL_ERROR;
//	}
//	for (temp = 0; temp < 0x2f; temp++) {
//		__NOP();
//	}
//
//	sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
//	sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
//	sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
//	sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
//	sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
//	sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
//	sCommand.Instruction = RESET_EXECUTE_CMD;
//	sCommand.AddressMode = QSPI_ADDRESS_NONE;
//	sCommand.Address = 0;
//	sCommand.DataMode = QSPI_DATA_NONE;
//	sCommand.DummyCycles = 0;
//
//	if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE)
//			!= HAL_OK) {
//		return HAL_ERROR;
//	}
//	return HAL_OK;
	return HAL_ERROR;
}


/* USER CODE END 1 */
