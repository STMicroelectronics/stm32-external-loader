#include "stm32h7xxfmc.h"

static void MPU_Config(void);

HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
	return HAL_OK;
}

int Init_fmc (void)
{
  int result = 0;
  MPU_Config();
  SystemInit(); 
result = NOR_Init();
  if (result == 1)
    return result;
  return 0;  
}

int SectorErase_fmc (uint32_t EraseStartAddress ,uint32_t EraseEndAddress)
{ 
  int result = 0;
  result =Sector_Erase ( EraseStartAddress , EraseEndAddress);
  return result;
	
}

int Write_fmc (uint32_t Address, uint32_t Size, uint16_t* Buffer)
{ 
  int result = 0;
  result =Write_FMC ( Address,  Size, Buffer);
  return result;

}
static void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;

  /* Disable the MPU */
  HAL_MPU_Disable();

  /* Configure the MPU attributes as device for NOR */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x60000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_16MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

