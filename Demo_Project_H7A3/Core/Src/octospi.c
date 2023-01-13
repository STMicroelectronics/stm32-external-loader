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
#include <stdbool.h>
#include <string.h> // for memset

//MT25TL256 commands
#define WRITE_ENABLE_CMD 0x06				// WRITE ENABLE
#define READ_STATUS_REG_CMD 0x05			// READ STATUS REGISTER
#define WRITE_STATUS_REG_CMD 0x01			// WRITE STATUS REGISTER
#define SECTOR_ERASE_CMD 0x20 				// 4KiB SUBSECTOR ERASE
#define CHIP_ERASE_CMD 0xC7 				// 128Mib BULK ERASE (for each die, so 256Mib)
#define QUAD_IN_FAST_PROG_CMD 0x38 			// EXTENDED QUAD INPUT FAST PROGRAM
//#define READ_CONFIGURATION_REG_CMD 0x15 	// page 50 table 24?
//#define QUAD_READ_IO_CMD 0xEC 				// ?
#define QUAD_OUT_FAST_READ_CMD 0x6B 		// QUAD OUTPUT FAST READ
#define QPI_ENABLE_CMD 0x35 				// ENTER QUAD INPUT/OUTPUT MODE
#define DISABLE_QIP_MODE 0xf5 				// RESET QUAD INPUT/OUTPUT MODE
#define RESET_ENABLE_CMD 0x66 				// RESET ENABLE
#define RESET_EXECUTE_CMD 0x99 				// RESET MEMORY
#define READ_ID_CMD 0xAF					// MULTIPLE I/O READ ID (supports 1-1-1,2-2-2, 4-4-4

#define READ_VOLATILE_CONFIGURATION_REGISTER_CMD 0x85
#define WRITE_VOLATILE_CONFIGURATION_REGISTER_CMD 0x81
#define READ_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_CMD 0x65
#define WRITE_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_CMD 0x61

//#define DUMMY_CLOCK_CYCLES_READ_QUAD 10  // todo: check which one is this? sometimes 0,4,8 or 10
#define FAST_READ_DUMMY_CYCLES 10

#define AUTO_POLLING_INTERVAL 16 // todo check

#define STATUS_REG_WIP_MASK (1UL<<0)
#define STATUS_REG_WEL_MASK (1UL<<1)


//#define USE_COMMAND_DTR

#define USE_READ_DTR

static uint8_t QSPI_WriteEnable(void);
static uint8_t QSPI_AutoPollingMemReady(void);
static uint8_t QSPI_Configuration(void);
static uint8_t QSPI_ResetChip(void);

static uint8_t QSPI_ReadChipId(void);
static uint8_t QSPI_Command(uint8_t command,
							bool hasAddress,
							uint32_t address,
							const uint8_t* writeData,
							uint8_t* readData,
							size_t dataLength);

static uint8_t QSPI_AutoPollingSetup(void);


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

typedef struct
{
	OspiErrorCodeType latestError;

	bool switchingToQuad;
	bool quadProtocolEnabled; 	// 4-4-4 if true, 1-1-1 if false

	bool switchingToDtr;
	bool dtrProtocolEnabled; 	// dtr for command, address and data

//	uint8_t dummyCycles;
}
DualQuadStateType;



static DualQuadStateType DualQuadState;


static HAL_StatusTypeDef HAL_OSPI_InitOveruled(OSPI_HandleTypeDef *hospi)
{
#ifdef USE_COMMAND_DTR
	//hospi->Init.DelayBlockBypass = HAL_OSPI_DELAY_BLOCK_USED;
#else
	 hospi1.Init.SampleShifting = HAL_OSPI_SAMPLE_SHIFTING_HALFCYCLE;
#endif

	return HAL_OSPI_Init(hospi);
}

#define HAL_OSPI_Init HAL_OSPI_InitOveruled

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
	HAL_StatusTypeDef res = HAL_OK;
	volatile OspiErrorCodeType errorCode = {0};

	//prepare QSPI peripheral for ST-Link Utility operations
	res = HAL_OSPI_DeInit(&hospi1);

	MX_OCTOSPI1_Init();

	if (res == HAL_OK)
	{
		res = QSPI_ResetChip();
	}


	HAL_Delay(1); // todo remove magic number


	if (res == HAL_OK)
	{
		res = QSPI_ReadChipId();
	}

	if (res == HAL_OK)
	{
		res = QSPI_AutoPollingMemReady();
	}

	if (res == HAL_OK)
	{
		res = QSPI_Configuration();
	}

	if (res == HAL_OK)
	{
		res = QSPI_ReadChipId();
	}

	if (res != HAL_OK)
	{
		res = QSPI_ReadChipId();
	}



	if (res != HAL_OK)
	{
		errorCode.errorCode = hospi1.ErrorCode;
	}



	(void)errorCode;

	return res;
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
	sCommand.InstructionMode = DualQuadState.quadProtocolEnabled ? HAL_OSPI_INSTRUCTION_4_LINES : HAL_OSPI_INSTRUCTION_1_LINE;
	sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;
	sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
	sCommand.Address = 0;
	sCommand.AddressMode = HAL_OSPI_ADDRESS_NONE;
	sCommand.AddressSize = HAL_OSPI_ADDRESS_24_BITS;
	sCommand.AddressDtrMode = DualQuadState.dtrProtocolEnabled ? HAL_OSPI_ADDRESS_DTR_ENABLE : HAL_OSPI_ADDRESS_DTR_DISABLE;
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

static uint8_t QSPI_AutoPollingSetup(void)
{
	OSPI_RegularCmdTypeDef sCommand = {0};
	HAL_StatusTypeDef res = HAL_OK;

	sCommand.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG;
	sCommand.FlashId = 0; //only applies if Dualquad is disabled
	sCommand.Instruction = READ_STATUS_REG_CMD;
	sCommand.InstructionMode = DualQuadState.quadProtocolEnabled ? HAL_OSPI_INSTRUCTION_4_LINES : HAL_OSPI_INSTRUCTION_1_LINE;
	sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;
	sCommand.InstructionDtrMode = DualQuadState.dtrProtocolEnabled ? HAL_OSPI_INSTRUCTION_DTR_ENABLE : HAL_OSPI_INSTRUCTION_DTR_DISABLE;

	sCommand.AddressMode = HAL_OSPI_ADDRESS_NONE;
	sCommand.Address = 0;
	sCommand.AddressSize = HAL_OSPI_ADDRESS_24_BITS;
	sCommand.AddressDtrMode = DualQuadState.dtrProtocolEnabled ? HAL_OSPI_ADDRESS_DTR_ENABLE : HAL_OSPI_ADDRESS_DTR_DISABLE;
	sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	sCommand.AlternateBytes = 0;
	sCommand.AlternateBytesSize = 0;
	sCommand.AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE;
	sCommand.DataMode = DualQuadState.quadProtocolEnabled ? HAL_OSPI_DATA_4_LINES : HAL_OSPI_DATA_1_LINE;
	sCommand.NbData = 2; // todo check if this is correct
	sCommand.DataDtrMode = DualQuadState.dtrProtocolEnabled ? HAL_OSPI_DATA_DTR_ENABLE : HAL_OSPI_DATA_DTR_DISABLE;
	sCommand.DummyCycles = 0;
	sCommand.DQSMode = HAL_OSPI_DQS_DISABLE; // no data strobe used
	sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;

	if (res == HAL_OK)
	{
		res = HAL_OSPI_Command(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE);
	}

	return res;
}

static uint8_t QSPI_AutoPollingMemReady(void)
{
	OSPI_AutoPollingTypeDef sConfig = {};
	HAL_StatusTypeDef res = HAL_OK;

	// todo: "avoid the auto-poll operations"

	res = QSPI_AutoPollingSetup();

	if (res == HAL_OK)
	{
		sConfig.Match = 0;
		sConfig.Mask = (STATUS_REG_WIP_MASK<<0) | (STATUS_REG_WIP_MASK<<8);
		sConfig.MatchMode = HAL_OSPI_MATCH_MODE_AND;
		sConfig.AutomaticStop = HAL_OSPI_AUTOMATIC_STOP_ENABLE;
		sConfig.Interval = AUTO_POLLING_INTERVAL;

		res = HAL_OSPI_AutoPolling(&hospi1, &sConfig,	HAL_OSPI_TIMEOUT_DEFAULT_VALUE);
	}

	return res;
}


static uint8_t QSPI_WriteEnable(void)
{
	HAL_StatusTypeDef res = HAL_OK;
	OSPI_AutoPollingTypeDef sConfig = {};

	/* Enable write operations ------------------------------------------ */

	res = QSPI_Command(WRITE_ENABLE_CMD, false, 0, NULL, NULL, 0);

	if (res == HAL_OK)
	{
		res = QSPI_AutoPollingSetup();
	}

	if (res == HAL_OK)
	{
		/* Configure automatic polling mode to wait for write enabling ---- */


		sConfig.Match = STATUS_REG_WEL_MASK | (STATUS_REG_WEL_MASK<<8);
		sConfig.Mask = STATUS_REG_WEL_MASK | (STATUS_REG_WEL_MASK<<8);

//		sConfig.Match = STATUS_REG_WEL_MASK;
//		sConfig.Mask = STATUS_REG_WEL_MASK;

		sConfig.MatchMode = HAL_OSPI_MATCH_MODE_AND;
		sConfig.Interval = AUTO_POLLING_INTERVAL;
		sConfig.AutomaticStop = HAL_OSPI_AUTOMATIC_STOP_ENABLE;

		res = HAL_OSPI_AutoPolling(&hospi1, &sConfig, HAL_OSPI_TIMEOUT_DEFAULT_VALUE);
	}

	return res;
}

/*Enable quad mode and set dummy cycles count*/
static uint8_t QSPI_Configuration(void)
{
	HAL_StatusTypeDef res = HAL_OK;
	uint8_t read_buffer[2] = { 0 };
	uint8_t write_buffer[2] = { 0 };
	/*read status register*/

	// R-M-W dummy cycles:
	res = QSPI_Command(READ_VOLATILE_CONFIGURATION_REGISTER_CMD, false, 0, NULL, read_buffer, 2);

	if (res == HAL_OK)
	{
		res = QSPI_WriteEnable();
	}

	if (res == HAL_OK)
	{
		memcpy(write_buffer,read_buffer,2);

		write_buffer[0] &= ~(((1<<4)-1)<<4);
		write_buffer[0] |= FAST_READ_DUMMY_CYCLES << 4;
		write_buffer[1] &= ~(((1<<4)-1)<<4);
		write_buffer[1] |= FAST_READ_DUMMY_CYCLES << 4;

		res = QSPI_Command(WRITE_VOLATILE_CONFIGURATION_REGISTER_CMD, false, 0, write_buffer, NULL, 2);
	}

	if (res == HAL_OK)
	{
		memset(read_buffer, 0, 2); // clear buffer before reading
		res = QSPI_Command(READ_VOLATILE_CONFIGURATION_REGISTER_CMD, false, 0, NULL, read_buffer, 2);
		if (res == HAL_OK)
		{
			if (memcmp(read_buffer,write_buffer,2) != 0)
			{
				res = HAL_ERROR;
			}
		}
	}

	// R-M-W dummy cycles:

	if (res == HAL_OK)
	{
		memset(read_buffer, 0, 2); // clear buffer before reading
		res = QSPI_Command(READ_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_CMD, false, 0, NULL, read_buffer, 2);
	}

	if (res == HAL_OK)
	{
		res = QSPI_WriteEnable();
	}

	if (res == HAL_OK)
	{
		memcpy(write_buffer,read_buffer,2);

		write_buffer[0] |= ((1<<2)-1)<<6;
		write_buffer[0] &= ~(1 << 7); // enable Quad mode
		write_buffer[1] |= ((1<<2)-1)<<6;
		write_buffer[1] &= ~(1 << 7); // enable Quad mode

#ifdef USE_COMMAND_DTR
		write_buffer[0] &= ~(1 << 5); // enable DTR mode
		write_buffer[1] &= ~(1 << 5); // enable DTR mode
		DualQuadState.switchingToDtr = true;
#endif

		DualQuadState.switchingToQuad = true; // skip auto polling
		res = QSPI_Command(WRITE_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_CMD, false, 0, write_buffer, NULL, 2);
		DualQuadState.switchingToQuad = false;
#ifdef USE_COMMAND_DTR
		DualQuadState.switchingToDtr = false;
#endif
	}

	// TODO: find out why this doesn't work
	if (res == HAL_OK)
	{

		memset(read_buffer, 0, 2); // clear buffer before reading
		res = QSPI_Command(READ_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_CMD, false, 0, NULL, read_buffer, 2);

		if (res == HAL_OK)
		{
			if (memcmp(read_buffer,write_buffer,2) != 0)
			{
				res = HAL_ERROR;
			}
		}
	}


	return res;
}

uint8_t CSP_QSPI_EraseSector(uint32_t EraseStartAddress, uint32_t EraseEndAddress)
{
//	OSPI_RegularCmdTypeDef sCommand = {};
	HAL_StatusTypeDef res = HAL_OK;

	EraseStartAddress &= (MEMORY_DUAL_SECTOR_SIZE - 1);

	/* Erasing Sequence -------------------------------------------------- */


	while (EraseEndAddress >= EraseStartAddress)
	{

		res = QSPI_WriteEnable();
		if (res != HAL_OK)
		{
			break;
		}

		res = QSPI_Command(SECTOR_ERASE_CMD, true, EraseStartAddress, NULL, NULL, 0);

		if (res != HAL_OK)
		{
			break;
		}

		EraseStartAddress += MEMORY_DUAL_SECTOR_SIZE;

		res = QSPI_AutoPollingMemReady();

		if (res != HAL_OK)
		{
			break;
		}
	}

	return res;
}

uint8_t CSP_QSPI_ReadMemory(uint8_t* buffer, uint32_t address, uint32_t buffer_size)
{
	OSPI_RegularCmdTypeDef sCommand = {};
	HAL_StatusTypeDef res = HAL_OK;

	sCommand.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG; //?
	sCommand.FlashId = 0; //only applies if Dualquad is disabled

#ifdef USE_READ_DTR
	sCommand.Instruction = 0x0D; // DTR FAST READ
#else
	sCommand.Instruction = 0x0B; // FAST READ
#endif
	sCommand.InstructionMode = DualQuadState.quadProtocolEnabled ? HAL_OSPI_INSTRUCTION_4_LINES : HAL_OSPI_INSTRUCTION_1_LINE;
	sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;
	sCommand.InstructionDtrMode = DualQuadState.dtrProtocolEnabled ? HAL_OSPI_INSTRUCTION_DTR_ENABLE : HAL_OSPI_INSTRUCTION_DTR_DISABLE;
	sCommand.Address = address;
	sCommand.AddressMode = DualQuadState.quadProtocolEnabled ? HAL_OSPI_ADDRESS_4_LINES : HAL_OSPI_ADDRESS_1_LINE;
	sCommand.AddressSize = HAL_OSPI_ADDRESS_24_BITS;
//	sCommand.AddressDtrMode = DualQuadState.dtrProtocolEnabled ? HAL_OSPI_ADDRESS_DTR_ENABLE : HAL_OSPI_ADDRESS_DTR_DISABLE;
#ifdef USE_READ_DTR
	sCommand.AddressDtrMode = HAL_OSPI_ADDRESS_DTR_ENABLE;
#else
	sCommand.AddressDtrMode = HAL_OSPI_ADDRESS_DTR_DISABLE;
#endif
	sCommand.AlternateBytes = 0;
	sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	sCommand.AlternateBytesSize = 0;
	sCommand.AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE;
	sCommand.DataMode = DualQuadState.quadProtocolEnabled ? HAL_OSPI_DATA_4_LINES : HAL_OSPI_DATA_1_LINE;
	sCommand.NbData = buffer_size;
//	sCommand.DataDtrMode = DualQuadState.dtrProtocolEnabled ? HAL_OSPI_DATA_DTR_ENABLE : HAL_OSPI_DATA_DTR_DISABLE;
#ifdef USE_READ_DTR
	sCommand.DataDtrMode = HAL_OSPI_DATA_DTR_ENABLE;
#else
	sCommand.DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE;
#endif
	sCommand.DummyCycles = FAST_READ_DUMMY_CYCLES;
	sCommand.DQSMode = HAL_OSPI_DQS_DISABLE; // no data strobe used
	sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;


	// Configure the command
	res = HAL_OSPI_Command(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE);

	if (res == HAL_OK)
	{
		// read of the data
		res = HAL_OSPI_Receive(&hospi1, buffer, HAL_OSPI_TIMEOUT_DEFAULT_VALUE);
	}

	return res;
}

uint8_t CSP_QSPI_WriteMemory(const uint8_t* buffer, uint32_t address, uint32_t buffer_size)
{
	OSPI_RegularCmdTypeDef sCommand = {};
	uint32_t current_size; //number of bytes to write in current operation
	uint32_t current_addr; //start address to write to in current operation
	uint32_t bytesRemainingInPage;

	//todo check size in command since half the data goes to one die and half the data goes to the other die

	current_addr = address;

	bytesRemainingInPage = MEMORY_DUAL_PAGE_SIZE - (current_addr % MEMORY_DUAL_PAGE_SIZE);
	current_size = (buffer_size > bytesRemainingInPage) ? bytesRemainingInPage : buffer_size;

	/* Initialize the address variables */


	sCommand.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG; //?
	sCommand.FlashId = 0; //only applies if Dualquad is disabled
	sCommand.Instruction = QUAD_IN_FAST_PROG_CMD;
	sCommand.InstructionMode = DualQuadState.quadProtocolEnabled ? HAL_OSPI_INSTRUCTION_4_LINES : HAL_OSPI_INSTRUCTION_1_LINE;
	sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;
	sCommand.InstructionDtrMode = DualQuadState.dtrProtocolEnabled ? HAL_OSPI_INSTRUCTION_DTR_ENABLE : HAL_OSPI_INSTRUCTION_DTR_DISABLE;
	sCommand.Address = address;
	sCommand.AddressMode = DualQuadState.quadProtocolEnabled ? HAL_OSPI_ADDRESS_4_LINES : HAL_OSPI_ADDRESS_1_LINE;
	sCommand.AddressSize = HAL_OSPI_ADDRESS_24_BITS;
	sCommand.AddressDtrMode = DualQuadState.dtrProtocolEnabled ? HAL_OSPI_ADDRESS_DTR_ENABLE : HAL_OSPI_ADDRESS_DTR_DISABLE;
	sCommand.AlternateBytes = 0;
	sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	sCommand.AlternateBytesSize = 0;
	sCommand.AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE;
	sCommand.DataMode = DualQuadState.quadProtocolEnabled ? HAL_OSPI_DATA_4_LINES : HAL_OSPI_DATA_1_LINE;
	sCommand.NbData = current_size;
	sCommand.DataDtrMode = DualQuadState.dtrProtocolEnabled ? HAL_OSPI_DATA_DTR_ENABLE : HAL_OSPI_DATA_DTR_DISABLE;
	sCommand.DummyCycles = 0;
	sCommand.DQSMode = HAL_OSPI_DQS_DISABLE; // no data strobe used
	sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;

	/* Perform the write page by page */
	do {
		sCommand.Address = current_addr;
		sCommand.NbData = current_size;

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
		buffer_size -= current_size;
		current_addr += current_size;
		buffer += current_size;

		bytesRemainingInPage = MEMORY_DUAL_PAGE_SIZE - (current_addr % MEMORY_DUAL_PAGE_SIZE);
		current_size = (buffer_size > bytesRemainingInPage) ? bytesRemainingInPage : buffer_size;

	} while (current_size);

	return HAL_OK;
}


uint8_t CSP_QSPI_EnableMemoryMappedMode(void)
{
	OSPI_MemoryMappedTypeDef sMemMappedCfg = {};
	HAL_StatusTypeDef res = HAL_OK;
	OSPI_RegularCmdTypeDef sCommand = {};

	/* Initialize memory-mapped mode for read operations */
	sCommand.OperationType = HAL_OSPI_OPTYPE_READ_CFG;
	sCommand.Instruction = 0xEB;// QUAD INPUT/OUTPUT FAST READ;
	sCommand.FlashId = 0;
	sCommand.InstructionMode = DualQuadState.quadProtocolEnabled ? HAL_OSPI_INSTRUCTION_4_LINES : HAL_OSPI_INSTRUCTION_1_LINE;
	sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;
	sCommand.AddressMode = DualQuadState.quadProtocolEnabled ? HAL_OSPI_ADDRESS_4_LINES : HAL_OSPI_ADDRESS_1_LINE;
	sCommand.AddressSize = HAL_OSPI_ADDRESS_24_BITS;
	sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	sCommand.DataMode = DualQuadState.quadProtocolEnabled ? HAL_OSPI_DATA_4_LINES : HAL_OSPI_DATA_1_LINE;
	sCommand.DummyCycles = FAST_READ_DUMMY_CYCLES;
	sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;

	sCommand.InstructionDtrMode = DualQuadState.dtrProtocolEnabled ? HAL_OSPI_INSTRUCTION_DTR_ENABLE : HAL_OSPI_INSTRUCTION_DTR_DISABLE;
	sCommand.AddressDtrMode = DualQuadState.dtrProtocolEnabled ? HAL_OSPI_ADDRESS_DTR_ENABLE : HAL_OSPI_ADDRESS_DTR_DISABLE;
	sCommand.DataDtrMode = DualQuadState.dtrProtocolEnabled ? HAL_OSPI_DATA_DTR_ENABLE : HAL_OSPI_DATA_DTR_DISABLE;

	sCommand.DQSMode = HAL_OSPI_DQS_ENABLE; // why strobing enabled?

	res = HAL_OSPI_Command(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE);


	if (res !=HAL_OK)
	{
		Error_Handler();
	}


	/* Initialize memory-mapped mode for write operations */
	sCommand.OperationType = HAL_OSPI_OPTYPE_WRITE_CFG;
	sCommand.Instruction = 0x32; // QUAD INPUT FAST PROGRAM Command
	sCommand.DummyCycles = 0;
	res = HAL_OSPI_Command(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE);

	if (res !=HAL_OK)
	{
		Error_Handler();
	}

	sMemMappedCfg.TimeOutActivation = HAL_OSPI_TIMEOUT_COUNTER_DISABLE;
	sMemMappedCfg.TimeOutPeriod = 0;

	res = HAL_OSPI_MemoryMapped(&hospi1, &sMemMappedCfg);

	return res;
}

static uint8_t QSPI_Command(uint8_t command,
							bool hasAddress,
							uint32_t address,
							const uint8_t* writeData,
							uint8_t* readData,
							size_t dataLength)
{
	// todo add line width option and dtr option

	OSPI_RegularCmdTypeDef sCommand = {0};
	HAL_StatusTypeDef res = HAL_OK;

	sCommand.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG; //?
	sCommand.FlashId = 0; //only applies if Dualquad is disabled
	sCommand.Instruction = command;

	sCommand.InstructionMode = DualQuadState.quadProtocolEnabled ? HAL_OSPI_INSTRUCTION_4_LINES : HAL_OSPI_INSTRUCTION_1_LINE;
	sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;
	sCommand.InstructionDtrMode = DualQuadState.dtrProtocolEnabled ? HAL_OSPI_INSTRUCTION_DTR_ENABLE : HAL_OSPI_INSTRUCTION_DTR_DISABLE;


	if (hasAddress)
	{
		sCommand.AddressMode = DualQuadState.quadProtocolEnabled ? HAL_OSPI_ADDRESS_4_LINES : HAL_OSPI_ADDRESS_1_LINE;
		sCommand.Address = address; // todo check if this is correct

		if ((command == 0xE2) || (command == 0xE3)) // these commands are the only ones with 4 byte address
		{
			sCommand.AddressSize = HAL_OSPI_ADDRESS_32_BITS;
		}
		else
		{
			sCommand.AddressSize = HAL_OSPI_ADDRESS_24_BITS;
		}
		sCommand.AddressDtrMode = DualQuadState.dtrProtocolEnabled ? HAL_OSPI_ADDRESS_DTR_ENABLE : HAL_OSPI_ADDRESS_DTR_DISABLE;
	}
	else
	{
		sCommand.AddressMode = HAL_OSPI_ADDRESS_NONE;
		sCommand.Address = 0;
		sCommand.AddressSize = HAL_OSPI_ADDRESS_24_BITS;
		sCommand.AddressDtrMode = HAL_OSPI_ADDRESS_DTR_DISABLE;
	}

	sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	sCommand.AlternateBytes = 0;
	sCommand.AlternateBytesSize = 0;
	sCommand.AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE;


	if (dataLength > 0)
	{
		if ((writeData == NULL) != (readData == NULL))
		{
			sCommand.DataMode = DualQuadState.quadProtocolEnabled ? HAL_OSPI_DATA_4_LINES : HAL_OSPI_DATA_1_LINE;
			sCommand.NbData = dataLength;
			sCommand.DataDtrMode = DualQuadState.dtrProtocolEnabled ? HAL_OSPI_DATA_DTR_ENABLE : HAL_OSPI_DATA_DTR_DISABLE;

		}
		else
		{
			res = HAL_ERROR;
		}
	}
	else
	{
		sCommand.DataMode = HAL_OSPI_DATA_NONE;
		sCommand.NbData = 0;
		sCommand.DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE;
	}

	//todo set FAST_READ_DUMMY_CYCLES


	sCommand.DummyCycles = 0;
	sCommand.DQSMode = HAL_OSPI_DQS_DISABLE; // no data strobe used
	sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;

	if (res == HAL_OK)
	{
		res = HAL_OSPI_Command(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE);
	}

	if (res == HAL_OK)
	{
		if (dataLength)
		{
			if (writeData != NULL)
			{
				res = HAL_OSPI_Transmit(&hospi1, (uint8_t*)writeData, HAL_OSPI_TIMEOUT_DEFAULT_VALUE);

				if (res == HAL_OK)
				{
					//write succeeded so if this write was to a config register it should update protocol settings:

					if (DualQuadState.switchingToQuad)
					{
						DualQuadState.quadProtocolEnabled = true;
						DualQuadState.switchingToQuad = false;
					}

					if (DualQuadState.switchingToDtr)
					{
						DualQuadState.dtrProtocolEnabled = true;
						DualQuadState.switchingToDtr = false;
					}

					//Configure automatic polling mode to wait for end of program or write
					res = QSPI_AutoPollingMemReady();
				}
			}
			else
			{
				//memset(readData, 0, dataLength); // clear buffer before reading
				res = HAL_OSPI_Receive(&hospi1, readData, HAL_OSPI_TIMEOUT_DEFAULT_VALUE);
			}
		}
	}

	return res;
}

static uint8_t QSPI_ResetChip()
{
	uint32_t temp = 0;
	HAL_StatusTypeDef res = HAL_OK;
	/* Erasing Sequence -------------------------------------------------- */

	res = QSPI_Command(RESET_ENABLE_CMD, false, 0, NULL, NULL, 0);

	if (res == HAL_OK)
	{
		//delay
		//todo remove magic number
		for (temp = 0; temp < 47; temp++)
		{
			__NOP();
		}

		res = QSPI_Command(RESET_EXECUTE_CMD, false, 0, NULL, NULL, 0);
	}

	if (res == HAL_OK)
	{
		DualQuadState.quadProtocolEnabled = false;
		DualQuadState.dtrProtocolEnabled = false;
	}

	return res;
}



static uint8_t QSPI_ReadChipId(void)
{
	uint8_t test_buffer[3*2] = { 0 };
	HAL_StatusTypeDef res = HAL_OK;

	res = QSPI_Command(READ_ID_CMD, false, 0, NULL, test_buffer, sizeof(test_buffer));

	if (res == HAL_OK)
	{
		//even bytes are first die, odd bytes are second die
		if (    (test_buffer[0*2+0] == test_buffer[0*2+1]) &&
				(test_buffer[1*2+0] == test_buffer[1*2+1]) &&
				(test_buffer[2*2+0] == test_buffer[2*2+1]) &&
				(test_buffer[0*2+0] == 0x20) &&
				(test_buffer[1*2+0] == 0xBA) &&
				(test_buffer[2*2+0] == 0x18))
		{
			// correct id
		}
		else
		{
			res = HAL_ERROR;
		}
	}


	return res;
}


/* USER CODE END 1 */
