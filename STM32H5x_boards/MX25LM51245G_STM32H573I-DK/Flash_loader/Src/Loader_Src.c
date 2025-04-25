/**
  ******************************************************************************
  * @file    Loader_Src.c
  * @author  ST/GPAM Application Team
  * @brief   This file defines the operations of the external loader for
  *          MX25LM51245G OSPI memory of STM32H573I-DK.
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "Loader_Src.h"

/** @addtogroup Flash_lOADER
  * @ingroup MX25LM51245G_STM32H573I-DK
  * @{
  */

/* External References -------------------------------------------------------*/
extern __NO_RETURN void __PROGRAM_START(void);

/* Private typedefs ----------------------------------------------------------*/
typedef void (*VECTOR_TABLE_Type)(void);

/* Private variables ---------------------------------------------------------*/
/** @defgroup  COM_Private_Variables Exported Variables
  * @{
  */
static UART_HandleTypeDef UartHandle; /*!< Uart Handler*/
/**
  * @}
  */

/* Private defines -----------------------------------------------------------*/
/**
  * @brief debug state machine execution
  */
volatile DEBUG_STATETypedef exec;

/**
  * @brief debug variable to provide RAM buffer address for write operation
  */
uint8_t *Buff_address;

/**
  * @brief debug variable to provide memory address for write/erase operation
  */

uint32_t Address;

/**
  * @brief debug state to provide size for write/erase operation
  */
uint32_t Size;

/**
  * @brief debug variable used to avoid compilation warning
  */
volatile const uint8_t condition = 1;

/* Private macros ------------------------------------------------------------*/
#pragma section = ".bss"
#define DEBUG 0

/**
  * @brief  Debug print macro.
  * @param  fmt: Format string.
  * @param  ...: Additional arguments for the format string.
  * @retval None
  * @note   This macro prints DEBUG messages if the global DEBUG flag is set to 1.
  *         It uses the printf function to output the formatted string.
  */
#define DEBUG_PRINTF(fmt, ...)    \
  do                              \
  {                               \
    if (DEBUG)                    \
      printf(fmt, ##__VA_ARGS__); \
  } while (0)

/**
  * @brief  Debug initialization macro.
  * @param  init_func: Initialization function to be called if DEBUG is enabled.
  * @retval None
  * @note   This macro calls the initialization function if the global DEBUG flag is set to 1.
  */
#define DEBUG_INIT(init_func) \
  do                          \
  {                           \
    if (DEBUG)                \
      init_func();            \
  } while (0)

/* Private function prototypes -----------------------------------------------*/
#if defined(__ICCARM__)
__ATTRIBUTES size_t __write(int, const unsigned char *, size_t);
#endif /* __ICCARM__ */

#if defined(__ICCARM__)
/* New definition from EWARM V9, compatible with EWARM8 */
int iar_fputc(int ch);
#define PUTCHAR_PROTOTYPE int iar_fputc(int ch)
#elif defined(__CC_ARM) || defined(__ARMCC_VERSION)
/* ARM Compiler 5/6*/
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#elif defined(__GNUC__)
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#endif /* __ICCARM__ */

/* Private function ----------------------------------------------------------*/
/** @defgroup Flash_lOADER_Private_Functions Private Functions
  * @{
  */

/**
  * @brief This function configures the source of the time base:
  *        The time source is configured to have 1ms time base with a dedicated
  *        Tick interrupt priority.
  * @note The function is an override of the HAL function to have tick
  *       management functional without interrupt
  * @param TickPriority  Tick interrupt priority.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
    /* Declare and initialize retr */
    HAL_StatusTypeDef retr = HAL_ERROR;

    /* Check uwTickFreq for MisraC 2012 (even if uwTickFreq is a enum type that doesn't take the value zero) */
    if ((uint32_t)uwTickFreq != 0U)
    {
        uint32_t ticks = SystemCoreClock / (1000U / (uint32_t)uwTickFreq);
        SysTick->LOAD = (uint32_t)(ticks - 1UL);     /* Set reload register */
        SysTick->VAL = 0UL;                          /* Load the SysTick Counter Value */
        SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | /* Set processor clock */
                        SysTick_CTRL_ENABLE_Msk;     /* Enable SysTick Timer */
        retr = HAL_OK;
    }

    /* Return status of the HAL operation */
    return retr;
}

/**
  * @brief Provide a tick value in millisecond.
  * @note The function is an override of the HAL function to increment the
  *       tick on a count flag event.
  * @retval tick value
  */
uint32_t HAL_GetTick(void)
{
    /* Check if the SysTick counter flag is set */
    if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == SysTick_CTRL_COUNTFLAG_Msk)
    {
        /* Increment the tick counter */
        uwTick++;
    }

    /* Return the current tick value */
    return uwTick;
}
/**
  * @}
  */

/* Exported variables --------------------------------------------------------*/
/** @addtogroup Flash_lOADER_Exported_Variables
  * @{
  */
BSP_OSPI_NOR_Init_t Flash;

/**
  * @brief status of the mapped mode  MEM_DISABLE or MEM_ENABLE
  */
MEM_MAPSTAT MemoryMappedMode;
/**
  * @}
  */

/**
  * @brief Entry point definition for the vector table.
  */
extern const uint32_t __ICFEDIT_region_RAM_end__;
extern const VECTOR_TABLE_Type __VECTOR_TABLE[];

/**
  * @brief Vector table definition with attributes.
  */
const VECTOR_TABLE_Type __VECTOR_TABLE[] __VECTOR_TABLE_ATTRIBUTE =
{
    (VECTOR_TABLE_Type)(&__ICFEDIT_region_RAM_end__),
    Reset_Handler
};

/**
  * @brief Weak definition of the Reset Handler.
  *        This function is called on reset and starts the program.
  */
__WEAK __NO_RETURN void Reset_Handler(void)
{
    __PROGRAM_START(); /* Enter PreMain (C library entry point) */
}

/**
  * @brief  main function used for debug purpose
  * @retval the function always returns 0
  */
int main(void)
{
    exec = DEBUG_STATE_INIT;
    do
    {
        switch (exec)
        {
        case DEBUG_STATE_WAIT:
        {
            /* nothing to do just loop to wait an exec update */
            break;
        }
        case DEBUG_STATE_INIT:
        {
            Init();
            exec = DEBUG_STATE_WAIT;
            break;
        }
        case DEBUG_STATE_WRITE:
        {
            Write(Address, Size, (uint8_t *)Buff_address);
            exec = DEBUG_STATE_INIT;
            break;
        }
        case DEBUG_STATE_MASSERASE:
        {
            MassErase(0);
            exec = DEBUG_STATE_INIT;
            break;
        }
        case DEBUG_STATE_SECTORERASE:
        {
            SectorErase(Address, Size);
            exec = DEBUG_STATE_INIT;
            break;
        }
        }
    } while (condition);

    return 0;
}

/* Exported functions ---------------------------------------------------------*/
/** @defgroup Flash_lOADER_Exported_Functions
  * @{
  */

/**
  * @brief  System initialization.
  * @param  None
  * @retval Loader status.
  * @retval 1       Operation succeeded.
  * @retval 0       Operation failed.
  */
KEEP_IN_COMPILATION uint32_t Init(void)
{
    /* Declare and initialize the ret variable */
    uint32_t ret = LOADER_STATUS_SUCCESS;

    /* Disable Interrupts */
    __disable_irq();

    /* Initialize .bss section to zero */
    char *start_addr = __section_begin(".bss");
    uint32_t size = __section_size(".bss");
    memset((void *)start_addr, 0, size);

    if ((FLASH->OPTSR_CUR & FLASH_OPTSR_PRODUCT_STATE) == OB_PROD_STATE_OPEN)
    {
        /* Initialize system */
        SystemInit();
        HAL_Init();

        /* Configure the system clock */
        SystemClock_Config();
    }
    else
    {
        /* Deinitialize RCC to allow PLL reconfiguration when configuring system clock */
        HAL_RCC_DeInit();

        /* Configure the system clock */
        SystemClock_Config();
    }

    DEBUG_INIT(COM_Init);
    DEBUG_PRINTF("\r\n\n===================  Start Init ============================\r\n");

    /* Define InterfaceMode and transferRate */
    Flash.InterfaceMode = BSP_OSPI_NOR_OPI_MODE;
    Flash.TransferRate = BSP_OSPI_NOR_STR_TRANSFER;

    DEBUG_PRINTF("\r\n InterfaceMode= BSP_OSPI_NOR_OPI_MODE ");
    DEBUG_PRINTF("\r\n TransferRate= BSP_OSPI_NOR_STR_TRANSFER ");

    /* Deinitialize the OSPI interface */
    if (BSP_OSPI_NOR_DeInit(0) != BSP_ERROR_NONE)
    {
        ret = LOADER_STATUS_FAIL;
    }
    else
    {
        DEBUG_PRINTF("\r\n OSPI interface is Deinitialized");
        /* Initialize OSPI */
        if (BSP_OSPI_NOR_Init(0, &Flash) != BSP_ERROR_NONE)
        {
            ret = LOADER_STATUS_FAIL;
        }
        else
        {
            DEBUG_PRINTF("\r\n OSPI interface is initialized");
            /* Enable the OSPI in memory-mapped mode */
            if (BSP_OSPI_NOR_EnableMemoryMappedMode(0) != BSP_ERROR_NONE)
            {
                ret = LOADER_STATUS_FAIL;
            }
            else
            {
                DEBUG_PRINTF("\r\n Memory-mapped mode is enabled");
                /* Set the return status to success */
                ret = LOADER_STATUS_SUCCESS;
            }
        }
    }
    /* Set the MemoryMappedMode variable to indicate that memory-mapped mode is enabled */
    MemoryMappedMode = MEM_MAPENABLE;

    if (ret != LOADER_STATUS_FAIL)
        DEBUG_PRINTF("\r\n\n=================== Init Pass ============================\r\n");
    else
        DEBUG_PRINTF("\r\n\n=================== Init Failed ============================\r\n");

    /* Return the Loader status */
    return ret;
}

/**
  * @brief   Erases the entire memory.
  * @param   Parallelism The parallelism mode.
  * @retval  Loader status.
  * @retval  1       Operation succeeded.
  * @retval  0       Operation failed.
  */
KEEP_IN_COMPILATION uint32_t MassErase(uint32_t Parallelism)
{
    /* Declare and initialize the ret variable */
    uint32_t ret = LOADER_STATUS_SUCCESS;

    DEBUG_PRINTF("\r\n\n=================== Start MassErase ============================\r\n");

    /* Disable Interrupts */
    __disable_irq();

    /* Exit form memory-mapped mode if enabled */
    if (MemoryMappedMode == MEM_MAPENABLE)
    {
        DEBUG_PRINTF("\r\n Disable memory-mapped mode ");

        if (BSP_OSPI_NOR_DisableMemoryMappedMode(0) != BSP_ERROR_NONE)
        {
            /* Set the return status to failure */
            ret = LOADER_STATUS_FAIL;
        }

        DEBUG_PRINTF("\r\n Memory-mapped mode Disabled");

        /* Set the MemoryMappedMode variable to indicate that memory-mapped mode is disabled */
        MemoryMappedMode = MEM_MAPDISABLE;
    }

    /* Erase the entire OSPI memory */
    if (BSP_OSPI_NOR_Erase_Chip(0) != BSP_ERROR_NONE)
    {
        ret = LOADER_STATUS_FAIL;
    }

    /* Read current status of the OSPI memory */
    while (BSP_OSPI_NOR_GetStatus(0) != BSP_ERROR_NONE);

    if (ret != LOADER_STATUS_FAIL)
        DEBUG_PRINTF("\r\n\n=================== MassErase Pass ============================\r\n");
    else
        DEBUG_PRINTF("\r\n\n=================== MassErase Failed ============================\r\n");

    /* Return the Loader status */
    return ret;
}

/**
  * @brief   Programs memory.
  * @param   Address The page address.
  * @param   Size    The size of data to write.
  * @param   buffer  Pointer to the data buffer.
  * @retval  Loader status.
  * @retval  1       Operation succeeded.
  * @retval  0       Operation failed.
  */
KEEP_IN_COMPILATION uint32_t Write(uint32_t Address, uint32_t Size, uint8_t *buffer)
{
    /* Declare and initialize the ret variable */
    uint32_t ret = LOADER_STATUS_SUCCESS;

    DEBUG_PRINTF("\r\n\n=================== Start Write ============================\r\n");
    DEBUG_PRINTF("\r\n Address = %x ", Address);
    DEBUG_PRINTF("\r\n Size = %x ", Size);
    DEBUG_PRINTF("\r\n buffer = %x ", buffer);

    /* Disable Interrupts */
    __disable_irq();

    Address = Address & 0x0FFFFFFF;

    /* Exit form memory-mapped mode if enabled */
    if (MemoryMappedMode == MEM_MAPENABLE)
    {
        DEBUG_PRINTF("\r\n Disable memory-mapped mode ");

        if (BSP_OSPI_NOR_DisableMemoryMappedMode(0) != BSP_ERROR_NONE)
        {
            /* Set the return status to failure */
            ret = LOADER_STATUS_FAIL;
        }

        DEBUG_PRINTF("\r\n Memory-mapped mode Disabled ");

        /* Set the MemoryMappedMode variable to indicate that memory-mapped mode is disabled */
        MemoryMappedMode = MEM_MAPDISABLE;
    }

    /* Write an amount of data to the OSPI memory */
    if (BSP_OSPI_NOR_Write(0, buffer, Address, Size) != BSP_ERROR_NONE)
    {
        ret = LOADER_STATUS_FAIL;
    }

    if (ret != LOADER_STATUS_FAIL)
        DEBUG_PRINTF("\r\n\n=================== Write Pass ============================\r\n");
    else
        DEBUG_PRINTF("\r\n\n=================== Write Failed ============================\r\n");

    /* Return the Loader status */
    return ret;
}

/**
  * @brief   Erases sectors.
  * @param   EraseStartAddress The erase start address.
  * @param   EraseEndAddress   The erase end address.
  * @retval  Loader status.
  * @retval  1       Operation succeeded.
  * @retval  0       Operation failed.
  */
KEEP_IN_COMPILATION uint32_t SectorErase(uint32_t EraseStartAddress, uint32_t EraseEndAddress)
{
    /* Declare and initialize variable */
    uint32_t ret = LOADER_STATUS_SUCCESS;
    uint32_t current_end_addr;
    uint32_t current_start_addr;

    /* define the Sector Size */
    uint32_t sector_size = 0x10000;

    /* Disable Interrupts */
    __disable_irq();

    DEBUG_PRINTF("\r\n\n=================== Start SectorErase ============================\r\n");
    DEBUG_PRINTF("\r\n EraseStartAddress = %x ", EraseStartAddress);
    DEBUG_PRINTF("\r\n EraseEndAddress = %x ", EraseEndAddress);
    DEBUG_PRINTF("\r\n sector_size = %x ", sector_size);

    /* Exit form memory-mapped mode if enabled */
    if (MemoryMappedMode == MEM_MAPENABLE)
    {
        DEBUG_PRINTF("\r\n Disable memory-mapped mode ");

        if (BSP_OSPI_NOR_DisableMemoryMappedMode(0) != BSP_ERROR_NONE)
        {
            /* Set the return status to failure */
            ret = LOADER_STATUS_FAIL;
        }

        DEBUG_PRINTF("\r\n Memory-mapped mode Disabled ");

        /* Set the MemoryMappedMode variable to indicate that memory-mapped mode is disabled */
        MemoryMappedMode = MEM_MAPDISABLE;
    }

    /* Mask the addresses to 28 bits */
    current_end_addr = EraseEndAddress & 0x0FFFFFFF;
    current_start_addr = EraseStartAddress & 0x0FFFFFFF;

    /* Align the start address to the nearest 64K boundary */
    current_start_addr = current_start_addr - (current_start_addr % sector_size);

    while ((current_end_addr >= current_start_addr) && (ret != LOADER_STATUS_FAIL))
    {
        /* Erase the specified block of the OSPI memory */
        if (BSP_OSPI_NOR_Erase_Block(0, current_start_addr, MX25LM51245G_ERASE_64K) != BSP_ERROR_NONE)
        {
            ret = LOADER_STATUS_FAIL;
        }
        /* Read current status of the OSPI memory */
        while (BSP_OSPI_NOR_GetStatus(0) != BSP_ERROR_NONE);

        current_start_addr += sector_size;
    }

    if (ret != LOADER_STATUS_FAIL)
        DEBUG_PRINTF("\r\n\n=================== SectorErase Pass ============================\r\n");
    else
        DEBUG_PRINTF("\r\n\n=================== SectorErase Failed ============================\r\n");

    /* Return the Loader status */
    return ret;
}

/**
  * @brief  Calculates checksum value of the memory zone.
  * @param  StartAddress Flash start address.
  * @param  Size         Size (in WORD).
  * @param  InitVal      Initial CRC value.
  * @retval Checksum value.
  * @note   Optional for all types of devices.
  */
KEEP_IN_COMPILATION uint32_t CheckSum(uint32_t StartAddress, uint32_t Size, uint32_t InitVal)
{
    /* Calculate misalignment of the start address */
    uint8_t misalignment_address = StartAddress % 4;
    /* Store the original size for later use */
    uint8_t misalignment_size = Size;
    uint32_t cnt;
    uint32_t Val;

    /* Align the start address to the nearest 4-byte boundary */
    StartAddress -= StartAddress % 4;
    /* Adjust the size to be a multiple of 4 bytes */
    Size += (Size % 4 == 0) ? 0 : 4 - (Size % 4);

    /* Iterate over the memory region in 4-byte chunks */
    for (cnt = 0; cnt < Size; cnt += 4)
    {
        /* Read a 4-byte value from the current address */
        Val = *(uint32_t *)StartAddress;

        /* Handle initial misalignment of the start address */
        if (misalignment_address)
        {
            switch (misalignment_address)
            {
            case 1:
                InitVal += (uint8_t)(Val >> 8 & 0xff);
                InitVal += (uint8_t)(Val >> 16 & 0xff);
                InitVal += (uint8_t)(Val >> 24 & 0xff);
                misalignment_address -= 1;
                break;
            case 2:
                InitVal += (uint8_t)(Val >> 16 & 0xff);
                InitVal += (uint8_t)(Val >> 24 & 0xff);
                misalignment_address -= 2;
                break;
            case 3:
                InitVal += (uint8_t)(Val >> 24 & 0xff);
                misalignment_address -= 3;
                break;
            }
        }
        /* Handle final misalignment of the size */
        else if ((Size - misalignment_size) % 4 && (Size - cnt) <= 4)
        {
            switch (Size - misalignment_size)
            {
            case 1:
                InitVal += (uint8_t)Val;
                InitVal += (uint8_t)(Val >> 8 & 0xff);
                InitVal += (uint8_t)(Val >> 16 & 0xff);
                misalignment_size -= 1;
                break;
            case 2:
                InitVal += (uint8_t)Val;
                InitVal += (uint8_t)(Val >> 8 & 0xff);
                misalignment_size -= 2;
                break;
            case 3:
                InitVal += (uint8_t)Val;
                misalignment_size -= 3;
                break;
            }
        }
        /* Process aligned 4-byte chunks */
        else
        {
            InitVal += (uint8_t)Val;
            InitVal += (uint8_t)(Val >> 8 & 0xff);
            InitVal += (uint8_t)(Val >> 16 & 0xff);
            InitVal += (uint8_t)(Val >> 24 & 0xff);
        }
        /* Move to the next 4-byte chunk */
        StartAddress += 4;
    }

    /* Return the calculated checksum value */
    return InitVal;
}

/**
  * @brief  Verifies flash memory with RAM buffer and calculates checksum value of
  *         the programmed memory.
  * @param  MemoryAddr     Flash address.
  * @param  RAMBufferAddr  RAM buffer address.
  * @param  Size           Size (in WORD).
  * @param  misalignment   Misalignment value.
  * @retval Operation failed (address of failure) in R0.
  * @retval Checksum value in R1.
  * @note   Optional for all types of devices.
  */
KEEP_IN_COMPILATION uint64_t Verify(uint32_t MemoryAddr, uint32_t RAMBufferAddr, uint32_t Size, uint32_t misalignment)
{
    uint32_t VerifiedData = 0;
    uint32_t InitVal = 0;
    uint64_t checksum;
    Size *= 4;

    /* Enter memory-mapped mode if disabled */
    if (MemoryMappedMode == MEM_MAPDISABLE)
    {
        /* Configure the OSPI in memory-mapped mode */
        if (BSP_OSPI_NOR_EnableMemoryMappedMode(0) != BSP_ERROR_NONE)
        {
            /* Set the return status to failure */
            return LOADER_STATUS_FAIL;
        }

        /* Set the MemoryMappedMode variable to indicate that memory-mapped mode is enabled */
        MemoryMappedMode = MEM_MAPENABLE;
    }

    /* Calculate checksum of the memory region */
    checksum = CheckSum((uint32_t)MemoryAddr + (misalignment & 0xf), Size - ((misalignment >> 16) & 0xF), InitVal);

    /* Verify the memory region against the RAM buffer */
    while (Size > VerifiedData)
    {
        if (*(uint8_t *)MemoryAddr++ != *((uint8_t *)RAMBufferAddr + VerifiedData))
        {
            /* Return the address of failure and checksum */
            return ((checksum << 32) + (MemoryAddr + VerifiedData));
        }

        VerifiedData++;
    }

    /* Return the checksum value */
    return (checksum << 32);
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 240000000  (CPU Clock)
  *            HCLK(Hz)                       = 240000000  (Bus matrix and AHBs Clock)
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1 (APB1 Clock  240MHz)
  *            APB2 Prescaler                 = 1 (APB2 Clock  240MHz)
  *            APB3 Prescaler                 = 1 (APB3 Clock  240MHz)
  *            HSI Frequency(Hz)              = 64000000
  *            PLL_M                          = 8
  *            PLL_N                          = 60
  *            PLL_P                          = 2
  *            PLL_Q                          = 2
  *            PLL_R                          = 2
  *            VDD(V)                         = 3.3
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */

static void SystemClock_Config(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};

    /* The voltage scaling allows optimizing the power consumption when the device is
    clocked below the maximum system frequency, to update the voltage scaling value
    regarding system frequency refer to product datasheet.
    */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
    {
    }

    /* Use HSE in bypass mode and activate PLL with HSE as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS_DIGITAL;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLL1_SOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 5;
    RCC_OscInitStruct.PLL.PLLN = 100UL;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLP = 2;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    RCC_OscInitStruct.PLL.PLLFRACN = 0;
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1_VCIRANGE_2;
    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1_VCORANGE_WIDE;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        /* Initialization Error */
        Reset_Handler();
    }

    /* Select PLL as system clock source and configure the HCLK, PCLK1, PCLK2 and PCLK3
       clocks dividers */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK
                                   | RCC_CLOCKTYPE_HCLK
                                   | RCC_CLOCKTYPE_PCLK1
                                   | RCC_CLOCKTYPE_PCLK2
                                   | RCC_CLOCKTYPE_PCLK3);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        /* Initialization Error */
        Reset_Handler();
    }
    /** Configure the programming delay */
    __HAL_FLASH_SET_PROGRAM_DELAY(FLASH_PROGRAMMING_DELAY_2);
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART and Loop until the end of transmission */
    HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 0xFFFF);

    return ch;
}

#if defined(__ICCARM__)
size_t __write(int file, unsigned char const *ptr, size_t len)
{
    size_t idx;
    unsigned char const *pdata = ptr;

    for (idx = 0; idx < len; idx++)
    {
        iar_fputc((int)*pdata);
        pdata++;
    }
    return len;
}
#endif /* __ICCARM__ */

/**
  * @brief  Initialize COM module.
  * @param  None.
  * @retval HAL Status.
  */
HAL_StatusTypeDef COM_Init(void)
{
#if defined(__GNUC__)
    setvbuf(stdout, NULL, _IONBF, 0);
#endif /* __GNUC__ */

    /* USART resources configuration (Clock, GPIO pins and USART registers) ----*/
    /* USART configured as follow:
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled
    */
    UartHandle.Instance = USART1;
    UartHandle.Init.BaudRate = 115200U;
    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits = UART_STOPBITS_1;
    UartHandle.Init.Parity = UART_PARITY_NONE;
    UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    UartHandle.Init.Mode = UART_MODE_RX | UART_MODE_TX;
    UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_RXOVERRUNDISABLE_INIT;
    UartHandle.AdvancedInit.OverrunDisable = UART_ADVFEATURE_OVERRUN_DISABLE;
    UartHandle.FifoMode = UART_FIFOMODE_ENABLE;
    return HAL_UART_Init(&UartHandle);
}

/**
  * @brief  UART MSP Init.
  * @param  huart: UART handler pointer.
  * @retval None.
  */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{

    GPIO_InitTypeDef GPIO_InitStruct;
    if (huart->Instance == USART1)
    {
        /* Peripheral Clock Enable */
        __HAL_RCC_USART1_CLK_ENABLE();

        /* GPIO Ports Clock Enable */
        __HAL_RCC_GPIOA_CLK_ENABLE();


        /*Configure GPIO pins : COM_UART_TX_Pin  */
        GPIO_InitStruct.Pin = GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /*Configure GPIO pins : COM_UART_RX_Pin  */
        GPIO_InitStruct.Pin = GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}

/**
  * @brief  UART MSP DeInit.
  * @param  huart: UART handler pointer.
  * @retval None.
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        /* Peripheral clock disable */
        __HAL_RCC_USART1_CLK_DISABLE();

        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9);
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_10);
    }
}

/**
  * @}
  */

/**
  * @}
  */
