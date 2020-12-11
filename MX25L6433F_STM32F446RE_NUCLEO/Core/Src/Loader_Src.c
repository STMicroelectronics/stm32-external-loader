
#include "main.h"
#include "spi.h"
#include "gpio.h"
#include "stm32_spi_flash.h"

#define LOADER_OK   0x1
#define LOADER_FAIL 0x0
extern void SystemClock_Config(void);

/**
 * @brief  System initialization.
 * @param  None
 * @retval  LOADER_OK = 1   : Operation succeeded
 * @retval  LOADER_FAIL = 0 : Operation failed
 */
int Init(void)
{
    *(uint32_t*)0xE000EDF0 = 0xA05F0000; //enable interrupts in debug


    SystemInit();

    /* ADAPTATION TO THE DEVICE
     *
     * change VTOR setting for H7 device
     * SCB->VTOR = 0x24000000 | 0x200;
     *
     * change VTOR setting for other devices
     * SCB->VTOR = 0x20000000 | 0x200;
     *
     * */

    SCB->VTOR = 0x20000000 | 0x200;

    __set_PRIMASK(0); //enable interrupts

    HAL_Init();

    SystemClock_Config();

    MX_GPIO_Init();

    MX_SPI2_Init();

    __set_PRIMASK(1); //disable interrupts
    return LOADER_OK;
}

/**
  * Description :
  * Read data from the device
  * Inputs    :
  *      Address       : Write location
  *      Size          : Length in bytes
  *      buffer        : Address where to get the data to write
  * outputs   :
  *      R0             : "1" 			: Operation succeeded
  * 			  "0" 			: Operation failure
  * Note: Mandatory for all types except SRAM and PSRAM
  */
int Read (uint32_t Address, uint32_t Size, uint8_t* buffer)
{
	sFLASH_ReadBuffer(buffer, Address, Size);
	return 1;
}

/**
 * @brief   Program memory.
 * @param   Address: page address
 * @param   Size   : size of data
 * @param   buffer : pointer to data buffer
 * @retval  LOADER_OK = 1       : Operation succeeded
 * @retval  LOADER_FAIL = 0 : Operation failed
 */
int Write(uint32_t Address, uint32_t Size, uint8_t* buffer)
{
	sFLASH_WriteBuffer(buffer, Address, Size);
	return LOADER_OK;
}

/**
 * @brief   Sector erase.
 * @param   EraseStartAddress :  erase start address
 * @param   EraseEndAddress   :  erase end address
 * @retval  LOADER_OK = 1       : Operation succeeded
 * @retval  LOADER_FAIL = 0 : Operation failed
 */
int SectorErase(uint32_t EraseStartAddress, uint32_t EraseEndAddress)
{
	EraseStartAddress = EraseStartAddress -  EraseStartAddress % sFLASH_SPI_SECTOR_SIZE;
	while (EraseEndAddress>=EraseStartAddress)
	{
		sFLASH_EraseSector(EraseStartAddress);
		EraseStartAddress += sFLASH_SPI_SECTOR_SIZE;
	}

	return LOADER_OK;
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
	sFLASH_EraseChip();
    return LOADER_OK;
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
	uint8_t missalignementAddress = StartAddress%4;
	uint8_t missalignementSize = Size ;
	int cnt;
	uint32_t Val;
	uint8_t value;

	StartAddress-=StartAddress%4;
	Size += (Size%4==0)?0:4-(Size%4);

	for(cnt=0; cnt<Size ; cnt+=4)
	{
		sFLASH_ReadBuffer(&value, StartAddress ,1);
		Val = value;
		sFLASH_ReadBuffer(&value, StartAddress + 1,1);
		Val+= value<<8;
		sFLASH_ReadBuffer(&value, StartAddress + 2,1);
		Val+= value<<16;
		sFLASH_ReadBuffer(&value, StartAddress + 3,1);
		Val+= value<<24;
		if(missalignementAddress)
		{
			switch (missalignementAddress)
			{
				case 1:
					InitVal += (uint8_t) (Val>>8 & 0xff);
					InitVal += (uint8_t) (Val>>16 & 0xff);
					InitVal += (uint8_t) (Val>>24 & 0xff);
					missalignementAddress-=1;
				break;
				case 2:
					InitVal += (uint8_t) (Val>>16 & 0xff);
					InitVal += (uint8_t) (Val>>24 & 0xff);
					missalignementAddress-=2;
				break;
				case 3:
					InitVal += (uint8_t) (Val>>24 & 0xff);
					missalignementAddress-=3;
				break;
			}
		}
		else if((Size-missalignementSize)%4 && (Size-cnt) <=4)
		{
			switch (Size-missalignementSize)
			{
			case 1:
				InitVal += (uint8_t) Val;
				InitVal += (uint8_t) (Val>>8 & 0xff);
				InitVal += (uint8_t) (Val>>16 & 0xff);
				missalignementSize-=1;
			break;
			case 2:
				InitVal += (uint8_t) Val;
				InitVal += (uint8_t) (Val>>8 & 0xff);
				missalignementSize-=2;
			break;
			case 3:
				InitVal += (uint8_t) Val;
				missalignementSize-=3;
				break;
			}
		}
		else
		{
			InitVal += (uint8_t) Val;
			InitVal += (uint8_t) (Val>>8 & 0xff);
			InitVal += (uint8_t) (Val>>16 & 0xff);
			InitVal += (uint8_t) (Val>>24 & 0xff);
		}
		StartAddress+=4;
	}

	return (InitVal);
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
	uint32_t InitVal = 0;
	uint32_t VerifiedData = 0;
	uint8_t TmpBuffer = 0x00;
	uint64_t checksum;
	Size*=4;

	checksum = CheckSum((uint32_t)MemoryAddr + (missalignement & 0xf), Size - ((missalignement >> 16) & 0xF), InitVal);

	while (Size>VerifiedData)
	{
		sFLASH_ReadBuffer(&TmpBuffer, MemoryAddr+VerifiedData, 1);

		if (TmpBuffer != *((uint8_t*)RAMBufferAddr+VerifiedData))
			return ((checksum<<32) + MemoryAddr+VerifiedData);

		VerifiedData++;
	}

	return (checksum<<32);
}
