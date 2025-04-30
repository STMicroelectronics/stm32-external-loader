/* USER CODE BEGIN Private defines */

uint8_t CSP_QUADSPI_Init(void);
uint8_t CSP_QSPI_EraseSector(uint32_t EraseStartAddress, uint32_t EraseEndAddress);
uint8_t CSP_QSPI_WriteMemory(uint8_t* buffer, uint32_t address, uint32_t buffer_size);
uint8_t CSP_QSPI_EnableMemoryMappedMode(void);
uint8_t CSP_QSPI_Erase_Chip (void);

/* USER CODE END Private defines */



/* USER CODE BEGIN Prototypes */

/*AT25SF128A memory parameters*/
#define MEMORY_FLASH_SIZE	0x1000000	/// 16 MBytes, 128 MBits
#define MEMORY_BLOCK_SIZE	0x10000	/// 64 KBytes, 256 blocks
#define MEMORY_SECTOR_SIZE	0x1000	/// 4 kBytes, 4096 sectors
#define MEMORY_PAGE_SIZE	0x100	/// 256 bytes, 65536 pages

/*AT25SF128A commands */
#define WRITE_ENABLE_CMD 0x06
#define RESET_ENABLE_CMD 0x66
#define RESET_EXECUTE_CMD 0x99

#define READ_STATUS_REG_1_CMD 0x05
#define READ_STATUS_REG_2_CMD 0x35
#define READ_STATUS_REG_3_CMD 0x15
#define WRITE_STATUS_REG_1_CMD 0x01
#define WRITE_STATUS_REG_2_CMD 0x31
#define WRITE_STATUS_REG_3_CMD 0x11

#define SUBSECTOR_ERASE_CMD 0x20
#define SECTOR_ERASE_CMD 0xD8
#define CHIP_ERASE_CMD 0xC7

#define QUAD_OUT_FAST_READ_CMD 0x6B
#define QUAD_INOUT_FAST_READ_CMD 0xEB
#define QUAD_PROG_CMD 0x32

#define DUMMY_CLOCK_CYCLES_READ_QUAD 8
#define DUMMY_CLOCK_CYCLES_MEMORY_MAPPED_MODE 6

/* USER CODE END Prototypes */