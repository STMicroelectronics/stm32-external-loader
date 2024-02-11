/* USER CODE BEGIN Private defines */

uint8_t CSP_QUADSPI_Init(void);
uint8_t CSP_QSPI_EraseSector(uint32_t EraseStartAddress, uint32_t EraseEndAddress);
uint8_t CSP_QSPI_WriteMemory(uint8_t* buffer, uint32_t address, uint32_t buffer_size);
uint8_t CSP_QSPI_EnableMemoryMappedMode(void);
uint8_t CSP_QSPI_Erase_Chip (void);

/* USER CODE END Private defines */

/* USER CODE BEGIN Prototypes */

typedef enum {
  CSP_QSPI_SPI_MODE = 0,                 /*!< 1-1-1 commands, Power on H/W default setting  */
  CSP_QSPI_QPI_MODE                      /*!< 4-4-4 commands                                */
} CSP_QSPI_Interface_t;

typedef enum {
  CSP_QSPI_3BYTES_SIZE = 0,              /*!< 3 Bytes address mode                          */
  CSP_QSPI_4BYTES_SIZE                   /*!< 4 Bytes address mode                          */
} CSP_QSPI_AddressSize_t;

typedef struct
{
  CSP_QSPI_Interface_t  InterfaceMode;      /*!<  Current Flash Interface mode */
  CSP_QSPI_AddressSize_t AddressSize;       /*!<  Current Flash Address mode   */
} CSP_QSPI_Init_t;

/* S25FL064L memory parameters */
#define MEMORY_FLASH_SIZE               0x800000    /* 64 MBits  */
#define MEMORY_SECTOR_SIZE              0x1000      /* 4 kBytes  */
#define MEMORY_PAGE_SIZE                0x100       /* 256 bytes */

/* S25FL064L commands */
#define WRITE_ENABLE_CMD                0x06
#define READ_STATUS_REG_CMD             0x05
#define ENTER_4_BYTE_ADD_CMD            0xB7
#define ENTER_QPI_MODE_CMD              0x38
#define SET_BRUST_LENGTH_CMD            0x77
#define SECTOR_ERASE_CMD                0x20
#define SECTOR_ERASE_4B_CMD             0x21
#define CHIP_ERASE_CMD                  0xC7
#define PAGE_PROG_CMD                   0x02
#define PAGE_PROG_4B_CMD                0x12
#define READ_CONFIGURATION_REG_CMD      0x85
#define QUAD_FAST_READ_CMD              0xEB
#define QUAD_FAST_READ_4B_CMD           0xEC
#define SPI_FAST_READ_CMD               0x0B
#define SPI_FAST_READ_4B_CMD            0x0C
#define RESET_ENABLE_CMD                0x66
#define RESET_EXECUTE_CMD               0x99

/* S25FL064L timeouts */
#define QUADSPI_MAX_ERASE_TIMEOUT 150000    /* 150s max */

/* Facroty default dummy cycles */
#define DEFAULT_DUMMY_CYCLES 8

/* USER CODE END Prototypes */
