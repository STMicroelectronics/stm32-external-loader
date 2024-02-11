/* USER CODE BEGIN 0 */

static uint8_t QSPI_WriteEnable(void);
static uint8_t QSPI_AutoPollingMemReady(uint32_t Timeout);
static uint8_t QSPI_Configuration(void);
static uint8_t QSPI_ResetChip(void);

CSP_QSPI_Init_t Flash = {CSP_QSPI_SPI_MODE, CSP_QSPI_3BYTES_SIZE};

/* USER CODE END 0 */
  
/* USER CODE BEGIN 1 */

/* QUADSPI init function */
uint8_t CSP_QUADSPI_Init(void) {
    //prepare QSPI peripheral for ST-Link Utility operations
	hqspi1.Instance = QUADSPI;
    if (HAL_QSPI_DeInit(&hqspi1) != HAL_OK) {
        return HAL_ERROR;
    }

    MX_QUADSPI1_Init();

    if (QSPI_ResetChip() != HAL_OK) {
        return HAL_ERROR;
    }

    if (QSPI_AutoPollingMemReady(HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return HAL_ERROR;
    }

    if (QSPI_Configuration() != HAL_OK) {
        return HAL_ERROR;
    }

    if (QSPI_AutoPollingMemReady(HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return HAL_ERROR;
    }

    return HAL_OK;
}


uint8_t CSP_QSPI_Erase_Chip(void) {
    QSPI_CommandTypeDef sCommand;

    if (QSPI_WriteEnable() != HAL_OK) {
        return HAL_ERROR;
    }

    /* Erasing Sequence --------------------------------- */
    sCommand.Instruction = CHIP_ERASE_CMD;
    sCommand.DummyCycles = 0;
    sCommand.InstructionMode = (Flash.InterfaceMode == CSP_QSPI_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
    sCommand.AddressMode = QSPI_ADDRESS_NONE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode = QSPI_DATA_NONE;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    if (HAL_QSPI_Command(&hqspi1, &sCommand, HAL_QSPI_TIMEOUT_DEFAULT_VALUE)
        != HAL_OK) {
        return HAL_ERROR;
    }

    if (QSPI_AutoPollingMemReady(QUADSPI_MAX_ERASE_TIMEOUT) != HAL_OK) {
        return HAL_ERROR;
    }


    return HAL_OK;
}

static uint8_t QSPI_AutoPollingMemReady(uint32_t Timeout) {

    QSPI_CommandTypeDef sCommand;
    QSPI_AutoPollingTypeDef sConfig;

    /* Configure automatic polling mode to wait for memory ready ------ */
    sCommand.Instruction = READ_STATUS_REG_CMD;
    sCommand.DummyCycles = (Flash.InterfaceMode == CSP_QSPI_QPI_MODE) ? 2 : 0;
    sCommand.InstructionMode = (Flash.InterfaceMode == CSP_QSPI_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
    sCommand.AddressMode = QSPI_ADDRESS_NONE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode = (Flash.InterfaceMode == CSP_QSPI_QPI_MODE) ? QSPI_DATA_4_LINES : QSPI_DATA_1_LINE;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    sConfig.Match = 0x00;
    sConfig.Mask = 0x01;
    sConfig.MatchMode = QSPI_MATCH_MODE_AND;
    sConfig.StatusBytesSize = 1;
    sConfig.Interval = 0x10;
    sConfig.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

    if (HAL_QSPI_AutoPolling(&hqspi1, &sCommand, &sConfig, Timeout) != HAL_OK) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

static uint8_t QSPI_WriteEnable(void) {
    QSPI_CommandTypeDef sCommand;
    QSPI_AutoPollingTypeDef sConfig;

    /* Enable write operations ------------------------------------------ */
    sCommand.Instruction = WRITE_ENABLE_CMD;
    sCommand.DummyCycles = 0;
    sCommand.InstructionMode = (Flash.InterfaceMode == CSP_QSPI_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
    sCommand.AddressMode = QSPI_ADDRESS_NONE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode = QSPI_DATA_NONE;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    if (HAL_QSPI_Command(&hqspi1, &sCommand, HAL_QSPI_TIMEOUT_DEFAULT_VALUE)
        != HAL_OK) {
        return HAL_ERROR;
    }

    /* Configure automatic polling mode to wait for write enabling ---- */
    sConfig.Match = 0x02;
    sConfig.Mask = 0x02;
    sConfig.MatchMode = QSPI_MATCH_MODE_AND;
    sConfig.StatusBytesSize = 1;
    sConfig.Interval = 0x10;
    sConfig.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

    sCommand.Instruction = READ_STATUS_REG_CMD;
    sCommand.DataMode = (Flash.InterfaceMode == CSP_QSPI_QPI_MODE) ? QSPI_DATA_4_LINES : QSPI_DATA_1_LINE;
    if (HAL_QSPI_AutoPolling(&hqspi1, &sCommand, &sConfig,
                             HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

/* Enable QPI mode, enter 4 byte address, and disable wrap length */
static uint8_t QSPI_Configuration(void) {

    QSPI_CommandTypeDef sCommand;
    uint8_t wrapDisable = 0x10;

    /* Enter 4 byte address mode ---------------------------------------- */
    sCommand.Instruction = ENTER_4_BYTE_ADD_CMD;
    sCommand.DummyCycles = 0;
    sCommand.InstructionMode = (Flash.InterfaceMode == CSP_QSPI_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
    sCommand.AddressMode = QSPI_ADDRESS_NONE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode = QSPI_DATA_NONE;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    if (HAL_QSPI_Command(&hqspi1, &sCommand, HAL_QSPI_TIMEOUT_DEFAULT_VALUE)
        != HAL_OK) {
        return HAL_ERROR;
    }
    Flash.AddressSize = CSP_QSPI_4BYTES_SIZE;

    /* Enter QPI mode --------------------------------------------------- */
    sCommand.Instruction = ENTER_QPI_MODE_CMD;

    /* Send the command ------------------------------------------------- */
    if (HAL_QSPI_Command(&hqspi1, &sCommand, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return HAL_ERROR;
    }
    Flash.InterfaceMode = CSP_QSPI_QPI_MODE;

    /* Disable Wrap Length ---------------------------------------------- */
    sCommand.Instruction = SET_BRUST_LENGTH_CMD;
    sCommand.DummyCycles = 6;
    sCommand.InstructionMode = QSPI_INSTRUCTION_4_LINES;
    sCommand.DataMode = QSPI_DATA_4_LINES;
    sCommand.NbData = 1;

    /* Send the command */
    if (HAL_QSPI_Command(&hqspi1, &sCommand, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return HAL_ERROR;
    }

    /* Send the data */
    if (HAL_QSPI_Transmit(&hqspi1, &wrapDisable, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return HAL_ERROR;
    }
    return HAL_OK;
}

uint8_t CSP_QSPI_EraseSector(uint32_t EraseStartAddress, uint32_t EraseEndAddress) {

    QSPI_CommandTypeDef sCommand;

    EraseStartAddress = EraseStartAddress
                        - EraseStartAddress % MEMORY_SECTOR_SIZE;

    /* Erasing Sequence -------------------------------------------------- */
    sCommand.Instruction = (Flash.AddressSize == CSP_QSPI_3BYTES_SIZE) ? SECTOR_ERASE_CMD : SECTOR_ERASE_4B_CMD;
    sCommand.AddressSize = (Flash.AddressSize == CSP_QSPI_3BYTES_SIZE) ? QSPI_ADDRESS_24_BITS : QSPI_ADDRESS_32_BITS;
    sCommand.DummyCycles = 0;
    sCommand.InstructionMode = (Flash.InterfaceMode == CSP_QSPI_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
    sCommand.AddressMode = (Flash.InterfaceMode == CSP_QSPI_QPI_MODE) ? QSPI_ADDRESS_4_LINES : QSPI_ADDRESS_1_LINE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode = QSPI_DATA_NONE;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    while (EraseEndAddress >= EraseStartAddress) {
        sCommand.Address = (EraseStartAddress & 0x0FFFFFFF);

        if (QSPI_WriteEnable() != HAL_OK) {
            return HAL_ERROR;
        }

        if (HAL_QSPI_Command(&hqspi1, &sCommand, HAL_QSPI_TIMEOUT_DEFAULT_VALUE)
            != HAL_OK) {
            return HAL_ERROR;
        }
        EraseStartAddress += MEMORY_SECTOR_SIZE;

        if (QSPI_AutoPollingMemReady(HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
            return HAL_ERROR;
        }
    }

    return HAL_OK;
}

uint8_t CSP_QSPI_WriteMemory(uint8_t* buffer, uint32_t address, uint32_t buffer_size) {

    QSPI_CommandTypeDef sCommand;
    uint32_t end_addr, current_size, current_addr;

    /* Calculation of the size between the write address and the end of the page */
    current_addr = 0;

    while (current_addr <= address) {
        current_addr += MEMORY_PAGE_SIZE;
    }
    current_size = current_addr - address;

    /* Check if the size of the data is less than the remaining place in the page */
    if (current_size > buffer_size) {
        current_size = buffer_size;
    }

    /* Initialize the adress variables */
    current_addr = address;
    end_addr = address + buffer_size;

    sCommand.Instruction = (Flash.AddressSize == CSP_QSPI_3BYTES_SIZE) ? PAGE_PROG_CMD : PAGE_PROG_4B_CMD;
    sCommand.Address = address;
    sCommand.AddressSize = (Flash.AddressSize == CSP_QSPI_3BYTES_SIZE) ? QSPI_ADDRESS_24_BITS : QSPI_ADDRESS_32_BITS;
    sCommand.DummyCycles = 0;
    sCommand.InstructionMode = (Flash.InterfaceMode == CSP_QSPI_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
    sCommand.AddressMode = (Flash.InterfaceMode == CSP_QSPI_QPI_MODE) ? QSPI_ADDRESS_4_LINES : QSPI_ADDRESS_1_LINE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode = (Flash.InterfaceMode == CSP_QSPI_QPI_MODE) ? QSPI_DATA_4_LINES : QSPI_DATA_1_LINE;
    sCommand.NbData = buffer_size;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

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
        if (HAL_QSPI_Command(&hqspi1, &sCommand, HAL_QSPI_TIMEOUT_DEFAULT_VALUE)
            != HAL_OK) {
            return HAL_ERROR;
        }

        /* Transmission of the data */
        if (HAL_QSPI_Transmit(&hqspi1, buffer, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
            return HAL_ERROR;
        }

        /* Configure automatic polling mode to wait for end of program */
        if (QSPI_AutoPollingMemReady(HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
            return HAL_ERROR;
        }

        /* Update the address and size variables for next page programming */
        current_addr += current_size;
        buffer += current_size;
        current_size =
            ((current_addr + MEMORY_PAGE_SIZE) > end_addr) ?
            (end_addr - current_addr) : MEMORY_PAGE_SIZE;
    } while (current_addr <= end_addr);

    return HAL_OK;

}


uint8_t CSP_QSPI_EnableMemoryMappedMode(void) {

    QSPI_CommandTypeDef sCommand;
    QSPI_MemoryMappedTypeDef sMemMappedCfg;

    /* Enable Memory-Mapped mode-------------------------------------------------- */

    switch(Flash.InterfaceMode) {
    case CSP_QSPI_QPI_MODE:
        sCommand.Instruction        = (Flash.AddressSize == CSP_QSPI_3BYTES_SIZE) ? QUAD_FAST_READ_CMD : QUAD_FAST_READ_4B_CMD;
        sCommand.AlternateBytes     = 0x00;
        sCommand.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
        sCommand.DummyCycles        = DEFAULT_DUMMY_CYCLES;
        sCommand.InstructionMode    = QSPI_INSTRUCTION_4_LINES;
        sCommand.AddressMode        = QSPI_ADDRESS_4_LINES;
        sCommand.AlternateByteMode  = QSPI_ALTERNATE_BYTES_4_LINES;
        sCommand.DataMode           = QSPI_DATA_4_LINES;
    	break;
    case CSP_QSPI_SPI_MODE:
    default:
        sCommand.Instruction        = (Flash.AddressSize == CSP_QSPI_3BYTES_SIZE) ? SPI_FAST_READ_CMD : SPI_FAST_READ_4B_CMD;
        sCommand.DummyCycles        = DEFAULT_DUMMY_CYCLES;
        sCommand.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
        sCommand.AddressMode        = QSPI_ADDRESS_1_LINE;
        sCommand.AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE;
        sCommand.DataMode           = QSPI_DATA_1_LINE;

    }
    sCommand.Address = 0;
    sCommand.NbData = 0;
    sCommand.AddressSize = (Flash.AddressSize == CSP_QSPI_3BYTES_SIZE) ? QSPI_ADDRESS_24_BITS : QSPI_ADDRESS_32_BITS;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    sMemMappedCfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;

    if (HAL_QSPI_MemoryMapped(&hqspi1, &sCommand, &sMemMappedCfg) != HAL_OK) {
        return HAL_ERROR;
    }
    return HAL_OK;
}

static uint8_t QSPI_ResetChip() {
    QSPI_CommandTypeDef sCommand;

    /* Reset Chip -------------------------------------------------- */
    sCommand.Instruction = RESET_ENABLE_CMD;
    sCommand.DummyCycles = 0;
    sCommand.InstructionMode = (Flash.InterfaceMode == CSP_QSPI_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
    sCommand.AddressMode = QSPI_ADDRESS_NONE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode = QSPI_DATA_NONE;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    if (HAL_QSPI_Command(&hqspi1, &sCommand, HAL_QSPI_TIMEOUT_DEFAULT_VALUE)
        != HAL_OK) {
        return HAL_ERROR;
    }

    sCommand.Instruction = RESET_EXECUTE_CMD;
    sCommand.DummyCycles = 0;
    sCommand.InstructionMode = (Flash.InterfaceMode == CSP_QSPI_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : QSPI_INSTRUCTION_1_LINE;
    sCommand.AddressMode = QSPI_ADDRESS_NONE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode = QSPI_DATA_NONE;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    if (HAL_QSPI_Command(&hqspi1, &sCommand, HAL_QSPI_TIMEOUT_DEFAULT_VALUE)
        != HAL_OK) {
        return HAL_ERROR;
    }

    HAL_Delay(1);

    Flash.InterfaceMode = CSP_QSPI_SPI_MODE;
    Flash.AddressSize = CSP_QSPI_3BYTES_SIZE;
    return HAL_OK;
}

/* USER CODE END 1 */
