/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  * Ref. to https://github.com/STMicroelectronics/stm32-external-loader
  * Ref. to https://www.st.com/content/st_com/en/support/learning/stm32-education/stm32-moocs/external_QSPI_loader.html
  * Ref. to https://github.com/STMicroelectronics/stm32-external-loader/tree/contrib
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include "stm32_spi_flash.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern unsigned char image_test[57600];

uint8_t sector_data[sFLASH_SPI_SECTOR_SIZE];
uint32_t errorCount;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int	my_memcmp(void *dest, void *src, int size)
{
    unsigned char *p = dest;
    unsigned char *q = src;

    while (size > 0)
    {
        if (*p != *q)
        {
            return (*p - *q);
        }
        size--;
        p++;
        q++;
    }
    return 0;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  uint32_t sectorsNeeded = 0;
  uint32_t currentSector = 0;
  uint32_t remainingBytes = 0;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */

  /*
   * In this part the flash management primitives is verified
   */
  /* 1 - Verify that the identifier is correctly read */
  if(sFLASH_MX25L6433F_ID != sFLASH_ReadID())
  {
    Error_Handler();
  }

  /* 2 - determines how many sectors are needed to write the test image */
  sectorsNeeded = sizeof(image_test) / sFLASH_SPI_SECTOR_SIZE;
  remainingBytes = (sizeof(image_test) % sFLASH_SPI_SECTOR_SIZE);

  /* 3 - erases and writes the needed sectors */
  for (currentSector = 0; currentSector < sectorsNeeded; currentSector++)
  {
    /* 3.1 - erases current sector */
  	sFLASH_EraseSector(currentSector * sFLASH_SPI_SECTOR_SIZE);
  	/* 3.2 - writes current sector */
  	sFLASH_WriteBuffer(&image_test[currentSector * sFLASH_SPI_SECTOR_SIZE], currentSector * sFLASH_SPI_SECTOR_SIZE, sFLASH_SPI_SECTOR_SIZE);
  }
  if(remainingBytes != 0)
  {
  	/* 3.3 - erases last sector for the remaining bytes */
  	sFLASH_EraseSector(currentSector * sFLASH_SPI_SECTOR_SIZE);
  	/* 3.4 - writes for the remaining bytes */
  	sFLASH_WriteBuffer(&image_test[currentSector * sFLASH_SPI_SECTOR_SIZE], currentSector * sFLASH_SPI_SECTOR_SIZE, remainingBytes);
  }

    /* 4 - reads and checks the selected sectors */
  for (currentSector = 0; currentSector < sectorsNeeded; currentSector++)
  {
  	/* 4.1 - reads current sector */
  	sFLASH_ReadBuffer(sector_data, currentSector * sFLASH_SPI_SECTOR_SIZE, sFLASH_SPI_SECTOR_SIZE);
  	/* 4.2 - checks current sector */
  	if(my_memcmp((void *)sector_data, (void *)&image_test[currentSector * sFLASH_SPI_SECTOR_SIZE], sFLASH_SPI_SECTOR_SIZE) != 0)
  	//if(memcmp(buffer_read, buffer_test, sFLASH_SPI_SECTOR_SIZE) != 0)
  	{
  		Error_Handler();
  	}
  }
  if(remainingBytes != 0)
  {
    /* 4.3 - reads current sector */
  	sFLASH_ReadBuffer(sector_data, currentSector * sFLASH_SPI_SECTOR_SIZE, remainingBytes);
  	/* 4.4 - checks current sector */
  	if(my_memcmp((void *)sector_data, (void *)&image_test[currentSector * sFLASH_SPI_SECTOR_SIZE], remainingBytes) != 0)
  	{
      Error_Handler();
  	}
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
