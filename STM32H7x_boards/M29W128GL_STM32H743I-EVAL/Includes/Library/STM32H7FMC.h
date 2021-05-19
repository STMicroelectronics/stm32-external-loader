#ifndef __STM32H7QSPI_H
#define __STM32H7QSPI_H
#include "stdint.h"
int Init_fmc (void);
int SectorErase_fmc (uint32_t EraseStartAddress ,uint32_t EraseEndAddress);
int Write_fmc (uint32_t Address, uint32_t Size, uint16_t* Buffer);
int chip_Erase(void );
#endif
