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

//settings:
//#define USE_COMMAND_DTR // if enabled dtr for command phase + address + data will be used, doesn't work
#define USE_READ_DTR // if enabled DTR_FAST_READ_CMD will be used

//MT25TL256 commands:
#define WRITE_ENABLE_CMD 0x06				// WRITE ENABLE
#define READ_STATUS_REG_CMD 0x05			// READ STATUS REGISTER
#define SECTOR_ERASE_CMD 0x20 				// 4KiB SUBSECTOR ERASE
#define PAGE_PROGRAM_CMD 0x02				// PAGE PROGRAM, program 1-256 bytes (cannot cross page boundary)
#define CHIP_ERASE_CMD 0xC7 				// 128Mib BULK ERASE (for each die, so 256Mib total)
#define FAST_READ_CMD 0x0B // FAST READ
#define RESET_ENABLE_CMD 0x66 				// RESET ENABLE
#define RESET_EXECUTE_CMD 0x99 				// RESET MEMORY
#define READ_ID_CMD 0xAF					// MULTIPLE I/O READ ID (supports 1-1-1,2-2-2, 4-4-4
//DTR commands (commands with command in STR and with address and data in DTR when device is not in DTR mode)
#define DTR_FAST_READ_CMD 0x0D 				// DTR FAST READ
//configuration register commands:
#define READ_VOLATILE_CONFIGURATION_REGISTER_CMD 0x85
#define WRITE_VOLATILE_CONFIGURATION_REGISTER_CMD 0x81
#define READ_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_CMD 0x65
#define WRITE_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_CMD 0x61

//bit fields:
#define ENHANCED_VOLATILE_CONFIGURATION_REGISTER_QUAD_DISABLE 	(1<<7)
#define ENHANCED_VOLATILE_CONFIGURATION_REGISTER_DTR_DISABLE 	(1<<5)

#define STATUS_REG_WIP_MASK (1UL<<0)
#define STATUS_REG_WEL_MASK (1UL<<1)

//timing:
#define MT25TL256_MAX_CLK_STR (133*1000*1000)
#define MT25TL256_MAX_CLK_DTR (90*1000*1000)

#if defined(USE_COMMAND_DTR) || defined(USE_READ_DTR)
#define MT25TL256_MAX_CLK MT25TL256_MAX_CLK_DTR
#else
#define MT25TL256_MAX_CLK MT25TL256_MAX_CLK_STR
#endif

#define FAST_READ_DUMMY_CYCLES_MASK (((1<<4)-1)<<4)
#define FAST_READ_DUMMY_CYCLES 11 	// (11 is needed in STR and 9 in DTR, less needed in case of lower clocks)
#define AUTO_POLLING_INTERVAL 16 	// value from example
#define CHIP_ERASE_TIMEOUT_MS ((114+1)*1000)	// 128Mb die erase time max 114s (BULK ERASE)

// these commands are the only ones with 4 byte address:
#define READ_NONVOLATILE_LOCK_BITS_CMD 0xE2
#define	WRITE_NONVOLATILE_LOCK_BITS_CMD 0xE3


#define EMPTY() // force multiple passes for macro expansion(http://jhnet.co.uk/articles/cpp_magic)
#define HAL_OSPI_Init(x) HAL_OSPI_InitOveruled(x)
#define HAL_OSPI_Init_Original(x) HAL_OSPI_Init EMPTY() (x)


static uint8_t QSPI_WriteEnable(void);
static uint8_t QSPI_AutoPollingMemReady(uint32_t timeoutMs);
static uint8_t QSPI_Configuration(void);
static uint8_t QSPI_ResetChip(void);
static uint8_t QSPI_ReadChipId(void);
static uint8_t QSPI_AutoPollingSetup(void);

static uint8_t QSPI_InitCommandStruct(	OSPI_RegularCmdTypeDef* sCommandPtr,
										uint8_t command,
										bool hasAddress,
										uint32_t address,
										size_t dataLength,
										bool write
										);

static uint8_t QSPI_Command(uint8_t command,
							bool hasAddress,
							uint32_t address,
							const uint8_t* writeData,
							uint8_t* readData,
							size_t dataLength);

static HAL_StatusTypeDef HAL_OSPI_InitOveruled(OSPI_HandleTypeDef *hospiPtr);


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
    // this is needed or else communication may fail
    __HAL_RCC_OSPI1_FORCE_RESET();
    __HAL_RCC_OSPI1_RELEASE_RESET();
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

static HAL_StatusTypeDef HAL_OSPI_InitOveruled(OSPI_HandleTypeDef *hospiPtr)
{
	hospiPtr->Init.ClockPrescaler = (SystemCoreClock+MT25TL256_MAX_CLK-1) / MT25TL256_MAX_CLK;

#if defined(USE_COMMAND_DTR) || defined(USE_READ_DTR)
//	hospiPtr->Init.DelayBlockBypass = HAL_OSPI_DELAY_BLOCK_USED; // doesn't work
	hospi1.Init.DelayHoldQuarterCycle = HAL_OSPI_DHQC_ENABLE;
#else
	hospi1.Init.SampleShifting = HAL_OSPI_SAMPLE_SHIFTING_HALFCYCLE;
#endif


	if (hospi1.Init.DelayHoldQuarterCycle == HAL_OSPI_DHQC_ENABLE)
	{
		// delay quarter cycle requires at least 4 cycles
		if (hospiPtr->Init.ClockPrescaler < 4)
		{
			hospiPtr->Init.ClockPrescaler = 4;
		}
	}

	return HAL_OSPI_Init_Original(hospiPtr);
}

uint8_t CSP_QUADSPI_Init(void) {
	HAL_StatusTypeDef res = HAL_OK;

	memset(&DualQuadState, 0, sizeof(DualQuadState));

	HAL_OSPI_DeInit(&hospi1);
	MX_OCTOSPI1_Init();

	if (res == HAL_OK)
	{
		DualQuadState.quadProtocolEnabled = true;
		res = QSPI_ResetChip();
	}

	if (res == HAL_OK)
	{
		res = QSPI_ResetChip();
	}

//	HAL_Delay(1); // TODO: use correct delay value from datasheet

	if (res == HAL_OK)
	{
		res = QSPI_ReadChipId();
	}


	if (res == HAL_OK)
	{
		res = QSPI_AutoPollingMemReady(HAL_OSPI_TIMEOUT_DEFAULT_VALUE);
	}

	if (res == HAL_OK)
	{
		res = QSPI_Configuration();
	}


	return res;
}

uint8_t CSP_QSPI_Erase_Chip(void)
{
	HAL_StatusTypeDef res = HAL_OK;

	res = QSPI_WriteEnable();

	if (res == HAL_OK)
	{
		res = QSPI_Command(CHIP_ERASE_CMD, false, 0, NULL, NULL, 0);
	}

	return res;
}

uint8_t CSP_QSPI_EraseSector(uint32_t EraseStartAddress, uint32_t EraseEndAddress)
{
	HAL_StatusTypeDef res = HAL_OK;

	EraseStartAddress &= ~(MEMORY_DUAL_SECTOR_SIZE - 1); // mask lower bits of address, only an entire sector can be erased

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
	}

	return res;
}

uint8_t CSP_QSPI_ReadMemory(uint8_t* buffer, uint32_t address, uint32_t buffer_size)
{
	HAL_StatusTypeDef res = HAL_OK;

#ifdef USE_READ_DTR
	res = QSPI_Command(DTR_FAST_READ_CMD, true, address, NULL, buffer, buffer_size);
#else
	res = QSPI_Command(FAST_READ_CMD, true, address, NULL, buffer, buffer_size);
#endif

	return res;
}

uint8_t CSP_QSPI_WriteMemory(const uint8_t* buffer, uint32_t address, uint32_t buffer_size)
{
	uint32_t current_size; //number of bytes to write in current operation
	uint32_t current_addr; //start address to write to in current operation
	uint32_t bytesRemainingInPage;
	HAL_StatusTypeDef res = HAL_OK;

	current_addr = address;

	bytesRemainingInPage = MEMORY_DUAL_PAGE_SIZE - (current_addr % MEMORY_DUAL_PAGE_SIZE);
	current_size = (buffer_size > bytesRemainingInPage) ? bytesRemainingInPage : buffer_size;

	// Perform the write page by page
	do
	{
		res = QSPI_WriteEnable();
		if (res != HAL_OK)
		{
			break;
		}

		res = QSPI_Command(PAGE_PROGRAM_CMD, true, current_addr, buffer, NULL, current_size);
		if (res != HAL_OK)
		{
			break;
		}


		// Update the address and size variables for next page programming
		buffer_size -= current_size;
		current_addr += current_size;
		buffer += current_size;

		current_size = (buffer_size > MEMORY_DUAL_PAGE_SIZE) ? MEMORY_DUAL_PAGE_SIZE : buffer_size;
	}
	while (current_size);

	return res;
}

uint8_t CSP_QSPI_EnableMemoryMappedMode(void)
{
	OSPI_MemoryMappedTypeDef sMemMappedCfg = {};
	HAL_StatusTypeDef res = HAL_OK;
	OSPI_RegularCmdTypeDef sCommand = {};

#ifdef	USE_READ_DTR
	QSPI_InitCommandStruct(&sCommand, DTR_FAST_READ_CMD, true, 0, 1, false);
#else
	QSPI_InitCommandStruct(&sCommand, FAST_READ_CMD, true, 0, 1, false);
#endif
	sCommand.OperationType = HAL_OSPI_OPTYPE_READ_CFG; // memory map instruction, called by peripheral, not with function

	res = HAL_OSPI_Command(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE);
	if (res !=HAL_OK)
	{
		Error_Handler();
	}

	// Initialize memory-mapped mode for write operations
	sCommand.OperationType = HAL_OSPI_OPTYPE_WRITE_CFG;
	sCommand.Instruction = 0;// TODO: check if this is the correct way to disable memory-mapped write?
	sCommand.DummyCycles = 0;//write has no dummy cycles
	sCommand.DQSMode = HAL_OSPI_DQS_ENABLE; // why strobing enabled?

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

// static functions:

static uint8_t QSPI_AutoPollingSetup(void)
{
	OSPI_RegularCmdTypeDef sCommand = {0};
	HAL_StatusTypeDef res = HAL_OK;

	// 2 bytes so both dies are checked
	res = QSPI_InitCommandStruct(&sCommand, READ_STATUS_REG_CMD, false, 0, 2, false);

	if (res == HAL_OK)
	{
		res = HAL_OSPI_Command(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE);
	}

	return res;
}

static uint8_t QSPI_AutoPollingMemReady(uint32_t timeoutMs)
{
	OSPI_AutoPollingTypeDef sConfig = {};
	HAL_StatusTypeDef res = HAL_OK;

	res = QSPI_AutoPollingSetup();

	if (res == HAL_OK)
	{
		sConfig.Match = 0;
		sConfig.Mask = (STATUS_REG_WIP_MASK<<0) | (STATUS_REG_WIP_MASK<<8);
		sConfig.MatchMode = HAL_OSPI_MATCH_MODE_AND;
		sConfig.AutomaticStop = HAL_OSPI_AUTOMATIC_STOP_ENABLE;
		sConfig.Interval = AUTO_POLLING_INTERVAL;

		res = HAL_OSPI_AutoPolling(&hospi1, &sConfig, timeoutMs);
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

		//check for both dies so shift mask
		sConfig.Match = STATUS_REG_WEL_MASK | (STATUS_REG_WEL_MASK<<8);
		sConfig.Mask = STATUS_REG_WEL_MASK | (STATUS_REG_WEL_MASK<<8);

		sConfig.MatchMode = HAL_OSPI_MATCH_MODE_AND;
		sConfig.Interval = AUTO_POLLING_INTERVAL;
		sConfig.AutomaticStop = HAL_OSPI_AUTOMATIC_STOP_ENABLE;

		res = HAL_OSPI_AutoPolling(&hospi1, &sConfig, HAL_OSPI_TIMEOUT_DEFAULT_VALUE);
	}

	return res;
}

static uint8_t QSPI_Configuration(void)
{
	HAL_StatusTypeDef res = HAL_OK;
	uint8_t read_buffer[2] = { 0 };
	uint8_t write_buffer[2] = { 0 };

	// R-M-W dummy cycles:
	res = QSPI_Command(READ_VOLATILE_CONFIGURATION_REGISTER_CMD, false, 0, NULL, read_buffer, 2);

	if (res == HAL_OK)
	{
		res = QSPI_WriteEnable();
	}

	if (res == HAL_OK)
	{
		memcpy(write_buffer,read_buffer,2);

		write_buffer[0] &= ~FAST_READ_DUMMY_CYCLES_MASK;
		write_buffer[0] |= FAST_READ_DUMMY_CYCLES << 4;
		write_buffer[1] &= ~FAST_READ_DUMMY_CYCLES_MASK;
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

		write_buffer[0] &= ~ENHANCED_VOLATILE_CONFIGURATION_REGISTER_QUAD_DISABLE; // enable Quad mode
		write_buffer[1] &= ~ENHANCED_VOLATILE_CONFIGURATION_REGISTER_QUAD_DISABLE; // enable Quad mode
		DualQuadState.switchingToQuad = true; // auto polling will be done in quad mode

#ifdef USE_COMMAND_DTR
		write_buffer[0] &= ~ENHANCED_VOLATILE_CONFIGURATION_REGISTER_DTR_DISABLE; // enable DTR mode
		write_buffer[1] &= ~ENHANCED_VOLATILE_CONFIGURATION_REGISTER_DTR_DISABLE; // enable DTR mode
		DualQuadState.switchingToDtr = true;
#endif


		res = QSPI_Command(WRITE_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_CMD, false, 0, write_buffer, NULL, 2);
		DualQuadState.switchingToQuad = false;
#ifdef USE_COMMAND_DTR
		DualQuadState.switchingToDtr = false;
#endif
	}

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

static uint8_t QSPI_ResetChip()
{
	HAL_StatusTypeDef res = HAL_OK;
	/* Erasing Sequence -------------------------------------------------- */

	res = QSPI_Command(RESET_ENABLE_CMD, false, 0, NULL, NULL, 0);

	if (res == HAL_OK)
	{
		HAL_Delay(1); // TODO: use correct delay value from datasheet

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
	uint8_t i;

	res = QSPI_Command(READ_ID_CMD, false, 0, NULL, test_buffer, sizeof(test_buffer));

	for(i=0;i<2;++i)
	{
		//even bytes are first die, odd bytes are second die
		if ((test_buffer[0*2+i] != 0x20) ||
			(test_buffer[1*2+i] != 0xBA) ||
			(test_buffer[2*2+i] != 0x18))
		{
			//incorrect id
			res = HAL_ERROR;
			break;
		}
	}

	return res;
}

static uint8_t QSPI_InitCommandStruct(	OSPI_RegularCmdTypeDef* sCommandPtr,
										uint8_t command,
										bool hasAddress,
										uint32_t address,
										size_t dataLength,
										bool write
										)
{
	HAL_StatusTypeDef res = HAL_OK;

	if (sCommandPtr == NULL)
	{
		res = HAL_ERROR;
		return res;
	}


	sCommandPtr->OperationType = HAL_OSPI_OPTYPE_COMMON_CFG; // called by a function call instead of by peripheral in memory mapped mode
	sCommandPtr->FlashId = 0; //only applies if Dualquad is disabled

	sCommandPtr->Instruction = command;
	sCommandPtr->InstructionMode = DualQuadState.quadProtocolEnabled ? HAL_OSPI_INSTRUCTION_4_LINES : HAL_OSPI_INSTRUCTION_1_LINE;
	sCommandPtr->InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;
	sCommandPtr->InstructionDtrMode = DualQuadState.dtrProtocolEnabled ? HAL_OSPI_INSTRUCTION_DTR_ENABLE : HAL_OSPI_INSTRUCTION_DTR_DISABLE;

	if (hasAddress)
	{
		sCommandPtr->AddressMode = DualQuadState.quadProtocolEnabled ? HAL_OSPI_ADDRESS_4_LINES : HAL_OSPI_ADDRESS_1_LINE;
		sCommandPtr->Address = address; // peripheral will automatically convert this to correct address for each die

		if ((command == READ_NONVOLATILE_LOCK_BITS_CMD) ||
			(command == WRITE_NONVOLATILE_LOCK_BITS_CMD))
		{
			// these commands are the only ones with 4 byte address
			sCommandPtr->AddressSize = HAL_OSPI_ADDRESS_32_BITS;
		}
		else
		{
			sCommandPtr->AddressSize = HAL_OSPI_ADDRESS_24_BITS;
		}
		if (command == DTR_FAST_READ_CMD)
		{
			sCommandPtr->AddressDtrMode = HAL_OSPI_ADDRESS_DTR_ENABLE;
		}
		else
		{
			sCommandPtr->AddressDtrMode = DualQuadState.dtrProtocolEnabled ? HAL_OSPI_ADDRESS_DTR_ENABLE : HAL_OSPI_ADDRESS_DTR_DISABLE;
		}
	}
	else
	{
		sCommandPtr->AddressMode = HAL_OSPI_ADDRESS_NONE;
		sCommandPtr->Address = 0;
		sCommandPtr->AddressSize = HAL_OSPI_ADDRESS_24_BITS;
		sCommandPtr->AddressDtrMode = HAL_OSPI_ADDRESS_DTR_DISABLE;
	}

	sCommandPtr->AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	sCommandPtr->AlternateBytes = 0;
	sCommandPtr->AlternateBytesSize = 0;
	sCommandPtr->AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE;


	if (dataLength > 0)
	{
		sCommandPtr->DataMode = DualQuadState.quadProtocolEnabled ? HAL_OSPI_DATA_4_LINES : HAL_OSPI_DATA_1_LINE;
		sCommandPtr->NbData = dataLength;

		if (command == DTR_FAST_READ_CMD)
		{
			sCommandPtr->DataDtrMode = HAL_OSPI_DATA_DTR_ENABLE;
		}
		else
		{
			sCommandPtr->DataDtrMode = DualQuadState.dtrProtocolEnabled ? HAL_OSPI_DATA_DTR_ENABLE : HAL_OSPI_DATA_DTR_DISABLE;
		}
	}
	else
	{
		sCommandPtr->DataMode = HAL_OSPI_DATA_NONE;
		sCommandPtr->NbData = 0;
		sCommandPtr->DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE;
	}

	if ((sCommandPtr->Instruction == FAST_READ_CMD) || (sCommandPtr->Instruction == DTR_FAST_READ_CMD))
	{
		sCommandPtr->DummyCycles = FAST_READ_DUMMY_CYCLES;
	}
	else
	{
		sCommandPtr->DummyCycles = 0;
	}

	sCommandPtr->DQSMode = HAL_OSPI_DQS_DISABLE; // no data strobe used
	sCommandPtr->SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;

	return res;
}

static uint8_t QSPI_Command(uint8_t command,
							bool hasAddress,
							uint32_t address,
							const uint8_t* writeData,
							uint8_t* readData,
							size_t dataLength)
{
	OSPI_RegularCmdTypeDef sCommand = {0};
	HAL_StatusTypeDef res = HAL_OK;
	uint32_t autoPollingTimeoutMs;

	if (dataLength > 0)
	{
		if ((writeData == NULL) != (readData == NULL))
		{
			res = QSPI_InitCommandStruct(&sCommand, command, hasAddress, address, dataLength, writeData != NULL);
		}
		else
		{
			// number of non-zero pointers not equal to 1
			res = HAL_ERROR;
		}
	}
	else
	{
		res = QSPI_InitCommandStruct(&sCommand, command, hasAddress, address, 0, false);
	}


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
					res = QSPI_AutoPollingMemReady(HAL_OSPI_TIMEOUT_DEFAULT_VALUE);
				}
			}
			else // writeData!= NULL
			{
				res = HAL_OSPI_Receive(&hospi1, readData, HAL_OSPI_TIMEOUT_DEFAULT_VALUE);
			}
		}
		else
		{
			if ((command == SECTOR_ERASE_CMD) ||
				(command == CHIP_ERASE_CMD))
			{
				if (command == SECTOR_ERASE_CMD)
				{
					autoPollingTimeoutMs = HAL_OSPI_TIMEOUT_DEFAULT_VALUE;
				}
				else
				{
					autoPollingTimeoutMs = CHIP_ERASE_TIMEOUT_MS;
				}

				res = QSPI_AutoPollingMemReady(autoPollingTimeoutMs);
			}
		}
	}

	if (res != HAL_OK)
	{
		DualQuadState.latestError.errorCode = hospi1.ErrorCode;

		if (DualQuadState.latestError.timeout)
		{
			res = HAL_TIMEOUT;
		}
	}


	return res;
}

/* USER CODE END 1 */
