#include "Loader_Src.h"
#include "octospi.h"
#include "main.h"
#include "gpio.h"
#include "Dev_Inf.h"
#include <stdbool.h>


void SystemClock_Config(void);

//#define USE_DEBUG_STRING

#ifdef USE_DEBUG_STRING
void setDebugString(const char* message)
{
	char* ptr = 0x20000000;
	strcpy(ptr, message);
}
#else
#define setDebugString(message)
#endif

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

/**
 * @brief  System initialization.
 * @param  None
 * @retval  LOADER_OK = 1	: Operation succeeded
 * @retval  LOADER_FAIL = 0	: Operation failed
 */
int Init(void)
{
	HAL_StatusTypeDef halRes = HAL_OK;
	int loaderRes = LOADER_OK;

	//trick compiler not to discard unused functions and data with option Discard unused sections (-Wl,--gc-sections)
	volatile bool useUnused = false;
	volatile unsigned long DeviceSize;

	if (useUnused)
	{
		DeviceSize = StorageInfo.DeviceSize;
		UNUSED(DeviceSize);
		SectorErase(0, 0);
		MassErase();
		Write(0,0,NULL);
		CheckSum(0, 0, 0);
		Verify(0,0,0,0);
	}

	__disable_irq();
	HAL_DeInit();
	*(uint32_t*)0xE000EDF0 = 0xA05F0000; //enable interrupts in debug
	SystemInit();
	SCB->VTOR = 0x24000000 | 0x200;
	//SCB_EnableICache(); // enabling cache fails
	halRes = HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_OCTOSPI1_Init();
    __enable_irq();


    halRes = CSP_QUADSPI_Init();
    loaderRes = halRes == HAL_OK ? LOADER_OK : LOADER_FAIL;

	if (loaderRes != LOADER_OK )
	{
		setDebugString("CSP_QUADSPI_Init() LOADER_FAIL");
	}

	if (loaderRes == LOADER_OK )
	{
		halRes = CSP_QSPI_EnableMemoryMappedMode();
		loaderRes = halRes == HAL_OK ? LOADER_OK : LOADER_FAIL;

		if (loaderRes != LOADER_OK )
		{
			setDebugString("CSP_QSPI_EnableMemoryMappedMode() LOADER_FAIL");
		}

	}

#ifdef DEBUG
//	if (loaderRes == LOADER_OK )
//	{
//		loaderRes = SectorErase(0,MEMORY_DUAL_SECTOR_SIZE*4-1);
//	}

	if (loaderRes == LOADER_OK )
	{
		loaderRes = MassErase();
	}



	if (loaderRes == LOADER_OK )
	{
		loaderRes = Write(0 << 13, 15, (const uint8_t *)"Hello sector 0");
		loaderRes = Write(1 << 13, 15, (const uint8_t *)"Hello sector 1");
		loaderRes = Write(2 << 13, 15, (const uint8_t *)"Hello sector 2");
		loaderRes = Write(3 << 13, 15, (const uint8_t *)"Hello sector 3");
	}
#endif

	return loaderRes;
}


/**
 * @brief   Program memory.
 * @param   Address: page address
 * @param   Size   : size of data
 * @param   buffer : pointer to data buffer
 * @retval  LOADER_OK = 1		: Operation succeeded
 * @retval  LOADER_FAIL = 0	: Operation failed
 */
int Write(uint32_t Address, uint32_t Size, const uint8_t* buffer)
{
	HAL_StatusTypeDef res = HAL_OK;

	HAL_ResumeTick();

	//doesn't work?:
	res = HAL_OSPI_Abort(&hospi1);

	if (res != HAL_OK )
	{
		 setDebugString("HAL_OSPI_Abort() LOADER_FAIL");
	}

	res = CSP_QSPI_WriteMemory((uint8_t*) buffer, Address, Size);

	if (res == HAL_OK )
	{
		res = CSP_QSPI_EnableMemoryMappedMode();

		if (res != HAL_OK )
		{
			 setDebugString("CSP_QSPI_EnableMemoryMappedMode() LOADER_FAIL");
		}
	}
	else
	{
		setDebugString("CSP_QSPI_WriteMemory() LOADER_FAIL");
	}

    HAL_SuspendTick();

	return res == HAL_OK ? LOADER_OK : LOADER_FAIL;
}

/**
 * @brief   Sector erase.
 * @param   EraseStartAddress :  erase start address
 * @param   EraseEndAddress   :  erase end address
 * @retval  LOADER_OK = 1		: Operation succeeded
 * @retval  LOADER_FAIL = 0	: Operation failed
 */
int SectorErase(uint32_t EraseStartAddress, uint32_t EraseEndAddress)
{
	HAL_StatusTypeDef res = HAL_OK;

	HAL_ResumeTick();

	//doesn't work?:
	res = HAL_OSPI_Abort(&hospi1);

	if (res != HAL_OK )
	{
		 setDebugString("HAL_OSPI_Abort() LOADER_FAIL");
	}

	res = CSP_QSPI_EraseSector(EraseStartAddress, EraseEndAddress);

	if (res == HAL_OK )
	{
		res = CSP_QSPI_EnableMemoryMappedMode();

		if (res != HAL_OK )
		{
			 setDebugString("CSP_QSPI_EnableMemoryMappedMode() LOADER_FAIL");
		}
	}
	else
	{
		setDebugString("SectorErase() LOADER_FAIL");
	}

    HAL_SuspendTick();


	return res == HAL_OK ? LOADER_OK : LOADER_FAIL;
}

/**
 * Description :
 * Mass erase of external flash area
 * Optional command - delete in case usage of mass erase is not planed
 * Inputs    :
 *      none
 * outputs   :
 *     none
 * Note: Optional for all types of device
 */
int MassErase(void)
{
	HAL_StatusTypeDef res = HAL_OK;

	HAL_ResumeTick();

	//doesn't work?:
	res = HAL_OSPI_Abort(&hospi1);

	if (res != HAL_OK )
	{
		 setDebugString("HAL_OSPI_Abort() LOADER_FAIL");
	}

	res = CSP_QSPI_Erase_Chip();

	if (res == HAL_OK )
	{
		res = CSP_QSPI_EnableMemoryMappedMode();

		if (res != HAL_OK )
		{
			 setDebugString("CSP_QSPI_EnableMemoryMappedMode() LOADER_FAIL");
		}
	}
	else
	{
		setDebugString("CSP_QSPI_Erase_Chip() LOADER_FAIL");
	}

    HAL_SuspendTick();

	return res == HAL_OK ? LOADER_OK : LOADER_FAIL;
}

/**
 * Description :
 * Calculates checksum value of the memory zone
 * Inputs    :
 *      StartAddress  : Flash start address
 *      Size          : Size (in WORD)
 *      InitVal       : Initial CRC value
 * outputs   :
 *     R0             : Checksum value
 * Note: Optional for all types of device
 */
uint32_t CheckSum(uint32_t StartAddress, uint32_t Size, uint32_t InitVal)
{
	uint8_t missalignementAddress = StartAddress % 4;
	uint8_t missalignementSize = Size;
	int cnt;
	uint32_t Val;

	StartAddress -= StartAddress % 4;
	Size += (Size % 4 == 0) ? 0 : 4 - (Size % 4);

	for (cnt = 0; cnt < Size; cnt += 4) {
		Val = *(uint32_t*) StartAddress;
		if (missalignementAddress) {
			switch (missalignementAddress) {
			case 1:
				InitVal += (uint8_t) (Val >> 8 & 0xff);
				InitVal += (uint8_t) (Val >> 16 & 0xff);
				InitVal += (uint8_t) (Val >> 24 & 0xff);
				missalignementAddress -= 1;
				break;
			case 2:
				InitVal += (uint8_t) (Val >> 16 & 0xff);
				InitVal += (uint8_t) (Val >> 24 & 0xff);
				missalignementAddress -= 2;
				break;
			case 3:
				InitVal += (uint8_t) (Val >> 24 & 0xff);
				missalignementAddress -= 3;
				break;
			}
		} else if ((Size - missalignementSize) % 4 && (Size - cnt) <= 4) {
			switch (Size - missalignementSize) {
			case 1:
				InitVal += (uint8_t) Val;
				InitVal += (uint8_t) (Val >> 8 & 0xff);
				InitVal += (uint8_t) (Val >> 16 & 0xff);
				missalignementSize -= 1;
				break;
			case 2:
				InitVal += (uint8_t) Val;
				InitVal += (uint8_t) (Val >> 8 & 0xff);
				missalignementSize -= 2;
				break;
			case 3:
				InitVal += (uint8_t) Val;
				missalignementSize -= 3;
				break;
			}
		} else {
			InitVal += (uint8_t) Val;
			InitVal += (uint8_t) (Val >> 8 & 0xff);
			InitVal += (uint8_t) (Val >> 16 & 0xff);
			InitVal += (uint8_t) (Val >> 24 & 0xff);
		}
		StartAddress += 4;
	}

	return InitVal;
}

/**
 * Description :
 * Verify flash memory with RAM buffer and calculates checksum value of
 * the programmed memory
 * Inputs    :
 *      FlashAddr     : Flash address
 *      RAMBufferAddr : RAM buffer address
 *      Size          : Size (in WORD)
 *      InitVal       : Initial CRC value
 * outputs   :
 *     R0             : Operation failed (address of failure)
 *     R1             : Checksum value
 * Note: Optional for all types of device
 */
uint64_t Verify(uint32_t MemoryAddr, uint32_t RAMBufferAddr, uint32_t Size, uint32_t missalignement)
{
	HAL_ResumeTick();

	uint32_t VerifiedData = 0, InitVal = 0;
	uint64_t checksum;
	Size *= 4;

	if (CSP_QSPI_EnableMemoryMappedMode() != HAL_OK)
	{
		HAL_SuspendTick();
		return LOADER_FAIL;
	}

	checksum = CheckSum((uint32_t) MemoryAddr + (missalignement & 0xf),
			Size - ((missalignement >> 16) & 0xF), InitVal);
	while (Size > VerifiedData) {
		if (*(uint8_t*) MemoryAddr++
				!= *((uint8_t*) RAMBufferAddr + VerifiedData)){
			HAL_SuspendTick();
			return ((checksum << 32) + (MemoryAddr + VerifiedData));
		}
		VerifiedData++;
	}

	HAL_SuspendTick();
	return checksum << 32;
}
