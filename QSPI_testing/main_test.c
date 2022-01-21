/* USER CODE BEGIN 0 */
#include <string.h>
#define SECTORS_COUNT 100
/* USER CODE END 0 */


/* USER CODE BEGIN 2 */

uint8_t buffer_test[MEMORY_SECTOR_SIZE];
uint32_t var = 0;

CSP_QUADSPI_Init();

for (var = 0; var < MEMORY_SECTOR_SIZE; var++) {
    buffer_test[var] = (var & 0xff);
}

for (var = 0; var < SECTORS_COUNT; var++) {

    if (CSP_QSPI_EraseSector(var * MEMORY_SECTOR_SIZE,
                             (var + 1) * MEMORY_SECTOR_SIZE - 1) != HAL_OK) {

        while (1)
            ;  //breakpoint - error detected
    }

    if (CSP_QSPI_WriteMemory(buffer_test, var * MEMORY_SECTOR_SIZE, sizeof(buffer_test)) != HAL_OK) {

        while (1)
            ;  //breakpoint - error detected
    }

}

if (CSP_QSPI_EnableMemoryMappedMode() != HAL_OK) {

    while (1)
        ; //breakpoint - error detected
}

for (var = 0; var < SECTORS_COUNT; var++) {
    if (memcmp(buffer_test,
               (uint8_t*) (0x90000000 + var * MEMORY_SECTOR_SIZE),
               MEMORY_SECTOR_SIZE) != HAL_OK) {
        while (1)
            ;  //breakpoint - error detected - otherwise QSPI works properly
    }
}

/* USER CODE END 2 */