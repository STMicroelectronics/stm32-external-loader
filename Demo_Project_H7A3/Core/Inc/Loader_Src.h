#ifndef INC_LOADER_SRC_H_
#define INC_LOADER_SRC_H_


#include <stdint.h>

int Init(void);
int SectorErase(uint32_t EraseStartAddress, uint32_t EraseEndAddress);
int MassErase(void);
int Write(uint32_t Address, uint32_t Size, const uint8_t* buffer);
uint32_t CheckSum(uint32_t StartAddress, uint32_t Size, uint32_t InitVal);
uint64_t Verify(uint32_t MemoryAddr, uint32_t RAMBufferAddr, uint32_t Size,uint32_t missalignement);


#endif // INC_LOADER_SRC_H_
