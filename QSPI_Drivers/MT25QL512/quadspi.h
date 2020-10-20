/* USER CODE BEGIN Private defines */

uint8_t CSP_QUADSPI_Init(void);
uint8_t CSP_QSPI_EraseSector(uint32_t EraseStartAddress, uint32_t EraseEndAddress);
uint8_t CSP_QSPI_WriteMemory(uint8_t* buffer, uint32_t address, uint32_t buffer_size);
uint8_t CSP_QSPI_EnableMemoryMappedMode(void);
uint8_t CSP_QSPI_Erase_Chip (void);

/* USER CODE END Private defines */



/* USER CODE BEGIN Prototypes */

/*MT25QL512 memory parameters*/
#define MEMORY_FLASH_SIZE               0x4000000 /* 512 MBits*/
#define MEMORY_SECTOR_SIZE              0x10000  /* 64kBytes */
#define MEMORY_PAGE_SIZE                0x100   /* 256 bytes */


/*MT25QL512 commands */
#define WRITE_ENABLE_CMD 0x06
#define READ_STATUS_REG_CMD 0x05
#define ENTER_4_BYTE_ADD_CMD 0xB7
#define WRITE_VOL_CFG_REG_CMD 0x81
#define SECTOR_ERASE_CMD 0xD8
#define CHIP_ERASE_CMD 0xC7
#define QUAD_IN_FAST_PROG_CMD 0x32
#define READ_CONFIGURATION_REG_CMD 0x85
#define QUAD_OUT_FAST_READ_CMD 0x6B
#define DUMMY_CLOCK_CYCLES_READ_QUAD 10
#define RESET_ENABLE_CMD 0x66
#define RESET_EXECUTE_CMD 0x99

/*MT25QL512 timeouts*/
#define QUADSPI_MAX_ERASE_TIMEOUT 460000 /* 460s max */

/* USER CODE END Prototypes */