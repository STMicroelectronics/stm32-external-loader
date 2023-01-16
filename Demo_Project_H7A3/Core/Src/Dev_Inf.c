/*
 * Dev_Inf.c
 *
 */
#include "Dev_Inf.h"
#include "octospi.h"

/* This structure contains information used by ST-LINK Utility to program and erase the device */
#if defined (__ICCARM__)
__root struct StorageInfo const StorageInfo  =
{
#else
struct StorageInfo const StorageInfo =
{
#endif
	"STM32H7A3ZIT6Q_MT25TL256", 	     // Device Name + version number
	NOR_FLASH,                  		 // Device Type
	0x90000000,                			 // Device Start Address
	MEMORY_FLASH_SIZE*2,                 // Device Size in Bytes
	MEMORY_PAGE_SIZE*2,                  // Programming Page Size
	0xFF,                                // Initial Content of Erased Memory

	// Specify Size and Address of Sectors (view example below)
	{{(MEMORY_FLASH_SIZE / MEMORY_SECTOR_SIZE),	// number of sectors,
	  (uint32_t) MEMORY_SECTOR_SIZE*2},       	// sector size
	  {0x00000000, 0x00000000}}					// no additional sectors (all sectors are the same size)
};
