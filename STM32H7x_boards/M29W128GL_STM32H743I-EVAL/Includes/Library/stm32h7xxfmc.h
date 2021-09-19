#ifndef __STM32H7XXFMC_H
#define __STM32H7XXFMC_H

#include "stm32h7xx_hal.h"
#include "stm32h743i_eval.h"
#include "stm32h7xx_hal_pwr.h"
#include "stm32h7xx_hal_nor.h"

/* Exported types ------------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Exported constants --------------------------------------------------------*/
#define StartAddresse                   ((uint32_t)0x60000000)
#define NOR_BANK_ADDR                 ((uint32_t)0x60000000)
#define PROGRAM_TIMEOUT               ((uint32_t)0x00004400)  /* NOR program timeout     */
#define ERASE_TIMEOUT                 ((uint32_t)0x00A00000)  /* NOR erase timeout     */

/* #define NOR_MEMORY_WIDTH            FMC_NORSRAM_MEM_BUS_WIDTH_8  */
#define NOR_MEMORY_WIDTH               FMC_NORSRAM_MEM_BUS_WIDTH_16

/* #define NOR_CONTINUOUS_CLOCK    FMC_CONTINUOUS_CLOCK_SYNC_ONLY */
#define NOR_CONTINUOUS_CLOCK    FMC_CONTINUOUS_CLOCK_SYNC_ASYNC

/* NOR Ready/Busy signal GPIO definitions */
#define NOR_READY_BUSY_PIN    GPIO_PIN_6 
#define NOR_READY_BUSY_GPIO   GPIOD
#define NOR_READY_STATE       GPIO_PIN_SET
#define NOR_BUSY_STATE        GPIO_PIN_RESET

int Init_fmc (void);
int NOR_Init(void);
int SystemClock_Config(void);
int SectorErase_fmc (uint32_t EraseStartAddress ,uint32_t EraseEndAddress);
int Sector_Erase (uint32_t EraseStartAddress ,uint32_t EraseEndAddress);
int Write_fmc (uint32_t Address, uint32_t Size, uint16_t* Buffer);
int Write_FMC (uint32_t Address, uint32_t Size, uint16_t* Buffer);
int chip_Erase(void );


#endif
