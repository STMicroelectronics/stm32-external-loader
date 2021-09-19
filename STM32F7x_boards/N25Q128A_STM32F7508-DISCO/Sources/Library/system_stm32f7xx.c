/**
  ******************************************************************************
  * @file    system_stm32f7xx.c
  * @author  MCD Application Team  
  * @brief   This file contains the system clock configuration for stm32f7xx devices.
  *
  *                   
  *     This file provides two functions and one global variable to be called from 
  *     user application:
  *      - SystemInit(): Setups the system clock (System clock source, PLL Multiplier
  *                      and Divider factors, AHB/APBx prescalers and Flash settings),
  *                      depending on the configuration made in the clock xls tool. 
  *                      This function is called at startup just after reset and 
  *                      before branch to main program. This call is made inside
  *                      the "startup_stm32f7xx.s" file.
  *
  *      - SystemCoreClock variable: Contains the core clock (HCLK), it can be used
  *                                  by the user application to setup the SysTick 
  *                                  timer or configure other parameters.
  *                                     
  *      - SystemCoreClockUpdate(): Updates the variable SystemCoreClock and must
  *                                 be called whenever the core clock is changed
  *                                 during program execution.
  *
  *
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
  ******************************************************************************
  */

/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup stm32f7xx_system
  * @{
  */  
  
/** @addtogroup stm32f7xx_System_Private_Includes
  * @{
  */

#include "stm32f7xx.h"

/**
  * @}
  */

/** @addtogroup stm32f7xx_System_Private_TypesDefinitions
  * @{
  */

/**
  * @}
  */

/** @addtogroup stm32f7xx_System_Private_Defines
  * @{
  */

  /* Select the System clock source then adapt accordingly the clock parameters */
//#define SYSCLK_SOURCE_HSE    /* System clock Frequency 60 Mhz */
#define SYSCLK_SOURCE_PLL  /* System clock Frequency 60 Mhz */
//#define SYSCLK_SOURCE_HSI  /* System clock Frequency 16 Mhz */

/************************* HSE used System clock source ***********************/
#ifdef SYSCLK_SOURCE_HSE
 #define HCLK_PRESCALER_VALUE     RCC_CFGR_HPRE_DIV1
 #define PLCK2_PRESCALER_VALUE    RCC_CFGR_PPRE2_DIV1
 #define PLCK1_PRESCALER_VALUE    RCC_CFGR_PPRE1_DIV2
 #define FLASH_ACR_VALUE          (/*FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN |*/ FLASH_ACR_LATENCY_2WS)
 /* Note: set the right HSE_VALUE in stm32f5xx.h file */
#endif /* SYSCLK_SOURCE_HSE */ 
/******************************************************************************/

/************************* PLL used System clock source ***********************/
#ifdef SYSCLK_SOURCE_PLL
 /* PLL configuration is fixed on FPGA as below:  */
 
 /* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N */
 #define PLL_M      16
 #define PLL_N      360

 /* SYSCLK = PLL_VCO / PLL_P */
 #define PLL_P      2

 /* USB OTG FS, SDIO and RNG Clock =  PLL_VCO / PLLQ */
 #define PLL_Q      5

 #define HCLK_PRESCALER_VALUE     RCC_CFGR_HPRE_DIV1
 #define PLCK2_PRESCALER_VALUE    RCC_CFGR_PPRE2_DIV1
 #define PLCK1_PRESCALER_VALUE    RCC_CFGR_PPRE1_DIV2
 #define FLASH_ACR_VALUE          (/*FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN |*/ FLASH_ACR_LATENCY_2WS)
#endif /* SYSCLK_SOURCE_PLL */ 
/******************************************************************************/

/************************* Miscellaneous Configuration ************************/
/*!< Uncomment the following line if you need to use external SRAM mounted
     on STM324xG_EVAL board as data memory  */
/* #define DATA_IN_ExtSRAM */

/*!< Uncomment the following line if you need to relocate your vector Table in
     Internal SRAM. */
/* #define VECT_TAB_SRAM */
#define VECT_TAB_OFFSET  0x00 /*!< Vector Table base offset field. 
                                   This value must be a multiple of 0x200. */
/******************************************************************************/

/**
  * @}
  */

/** @addtogroup STM32F5xx_System_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32F5xx_System_Private_Variables
  * @{
  */

#if defined(SYSCLK_SOURCE_HSE)
  uint32_t SystemCoreClock = 60000000;
#elif defined (SYSCLK_SOURCE_PLL) 
  uint32_t SystemCoreClock = 80000000;
#else
  uint32_t SystemCoreClock = 16000000;
#endif
  
  __I uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};

/**
  * @}
  */

/** @addtogroup STM32F5xx_System_Private_FunctionPrototypes
  * @{
  */

static void SetSysClock(void);
#ifdef DATA_IN_ExtSRAM
  static void SystemInit_ExtMemCtl(void); 
#endif /* DATA_IN_ExtSRAM */

/**
  * @}
  */

/** @addtogroup STM32F5xx_System_Private_Functions
  * @{
  */

/**
  * @brief  Setup the microcontroller system
  *         Initialize the Embedded Flash Interface, the PLL and update the 
  *         SystemFrequency variable.
  * @param  None
  * @retval None
  */
void SystemInit(void)
{
  /* FPU settings ------------------------------------------------------------*/
  #if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
  #endif

  /* Reset the RCC clock configuration to the default reset state ------------*/
  /* Set HSION bit */
  RCC->CR |= (uint32_t)0x00000001;

  /* Reset CFGR register */
  RCC->CFGR = 0x00000000;

  /* Reset HSEON, CSSON and PLLON bits */
  RCC->CR &= (uint32_t)0xFEF6FFFF;

  /* Reset PLLCFGR register */
  RCC->PLLCFGR = 0x24003010;

  /* Reset HSEBYP bit */
  RCC->CR &= (uint32_t)0xFFFBFFFF;

  /* Disable all interrupts */
  RCC->CIR = 0x00000000;

#ifdef DATA_IN_ExtSRAM
  SystemInit_ExtMemCtl(); 
#endif /* DATA_IN_ExtSRAM */
         
  /* Configure the System clock source, PLL Multiplier and Divider factors, 
     AHB/APBx prescalers and Flash settings ----------------------------------*/
  SetSysClock();

  /* Configure the Vector Table location add offset address ------------------*/
#ifdef VECT_TAB_SRAM
  SCB->VTOR = SRAM1_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM */
#else
  SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH */
#endif
}

/**
   * @brief  Update SystemCoreClock variable according to Clock Register Values.
  *         The SystemCoreClock variable contains the core clock (HCLK), it can
  *         be used by the user application to setup the SysTick timer or configure
  *         other parameters.
  *           
  * @note   Each time the core clock (HCLK) changes, this function must be called
  *         to update SystemCoreClock variable value. Otherwise, any configuration
  *         based on this variable will be incorrect.         
  *     
  * @note   - The system frequency computed by this function is not the real 
  *           frequency in the chip. It is calculated based on the predefined 
  *           constant and the selected clock source:
  *             
  *           - If SYSCLK source is HSI, SystemCoreClock will contain the HSI_VALUE(*)
  *                                              
  *           - If SYSCLK source is HSE, SystemCoreClock will contain the HSE_VALUE(**)
  *                          
  *           - If SYSCLK source is PLL, SystemCoreClock will contain the HSE_VALUE(**) 
  *             or HSI_VALUE(*) multiplied/divided by the PLL factors.
  *         
  *         (*) HSI_VALUE is a constant defined in stm32f5xx.h file (default value
  *             16 MHz) but the real value may vary depending on the variations
  *             in voltage and temperature.   
  *    
  *         (**) HSE_VALUE is a constant defined in stm32f5xx.h file (default value
  *              25 MHz), user has to ensure that HSE_VALUE is same as the real
  *              frequency of the crystal used. Otherwise, this function may
  *              have wrong result.
  *                
  *         - The result of this function could be not correct when using fractional
  *           value for HSE crystal.
  *     
  * @param  None
  * @retval None
  */
void SystemCoreClockUpdate(void)
{
  uint32_t tmp = 0, pllvco = 0, pllp = 2, pllsource = 0, pllm = 2;
  
  /* Get SYSCLK source -------------------------------------------------------*/
  tmp = RCC->CFGR & RCC_CFGR_SWS;

  switch (tmp)
  {
    case 0x00:  /* HSI used as system clock source */
      SystemCoreClock = HSI_VALUE;
      break;
    case 0x04:  /* HSE used as system clock source */
      SystemCoreClock = HSE_VALUE;
      break;
    case 0x08:  /* PLL used as system clock source */

      /* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N
         SYSCLK = PLL_VCO / PLL_P
         */    
      pllsource = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) >> 22;
      pllm = RCC->PLLCFGR & RCC_PLLCFGR_PLLM;
      
      if (pllsource != 0)
      {
        /* HSE used as PLL clock source */
        pllvco = (HSE_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);
      }
      else
      {
        /* HSI used as PLL clock source */
        pllvco = (HSI_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);      
      }

      pllp = (((RCC->PLLCFGR & RCC_PLLCFGR_PLLP) >>16) + 1 ) *2;
      SystemCoreClock = pllvco/pllp;
      break;
    default:
      SystemCoreClock = HSI_VALUE;
      break;
  }
  /* Compute HCLK frequency --------------------------------------------------*/
  /* Get HCLK prescaler */
  tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];
  /* HCLK frequency */
  SystemCoreClock >>= tmp;
}

/**
  * @brief  Configures the System clock source, PLL Multiplier and Divider factors, 
  *         AHB/APBx prescalers and Flash settings
  * @Note   This function should be called only once the RCC clock configuration  
  *         is reset to the default reset state (done in SystemInit() function).   
  * @param  None
  * @retval None
  */
#define FLASH_ACR_ARTEN                      ((uint32_t)0x00000200)
static void SetSysClock(void)
{
	
#ifndef SYSCLK_SOURCE_HSI  
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
	
  /* Enable HSE */
  RCC->CR |= ((uint32_t)RCC_CR_HSEON);
 
  /* Wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CR & RCC_CR_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }

  if (HSEStatus == (uint32_t)0x01)
  {
//    /* Select regulator voltage output Scale 2 mode, System frequency up to 144 MHz */
//    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
//    PWR->CR &= (uint32_t)~(PWR_CR_VOS);

    /* Set HCLK prescaler */
    RCC->CFGR |= HCLK_PRESCALER_VALUE;    

    /* Set PCLK2 prescaler */    
    RCC->CFGR |= PLCK2_PRESCALER_VALUE;

    /* Set PCLK1 prescaler */
    RCC->CFGR |= PLCK1_PRESCALER_VALUE;

    /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
    FLASH->ACR = FLASH_ACR_VALUE;
    
#if defined (SYSCLK_SOURCE_HSE)   
    /* Select the HSE as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= RCC_CFGR_SW_HSE;

    /* Wait till the HSE is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_HSE);
    {
    }
#elif defined (SYSCLK_SOURCE_PLL)
		
		/* Enable flash ART */
		FLASH->ACR |= FLASH_ACR_ARTEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_9WS;
		
		RCC->CR |= (RCC_CR_HSION);
		
    /* Configure the main PLL => NOT NEEDED on FPGA since the PLL configuration
       is fixed and can't be changed */
    RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) |
                   (RCC_PLLCFGR_PLLSRC_HSI) | (PLL_Q << 24);

    /* Enable the main PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait till the main PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0)
    {
    }
   
    /* Select the main PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    /* Wait till the main PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL)
    {
    }

#endif 
    
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock
         configuration. User can add here some code to deal with this error */
  }
#endif /* SYSCLK_SOURCE_HSI */
}

/**
  * @brief  Setup the external memory controller. Called in startup_stm32f5xx.s 
  *          before jump to __main
  * @param  None
  * @retval None
  */ 
#ifdef DATA_IN_ExtSRAM
/**
  * @brief  Setup the external memory controller.
  *         Called in startup_stm32f5xx.s before jump to main.
  *         This function configures the external SRAM mounted on STM324xG_EVAL board
  *         This SRAM will be used as program data memory (including heap and stack).
  * @param  None
  * @retval None
  */
void SystemInit_ExtMemCtl(void)
{
 /* To Do*/ 
}
#endif /* DATA_IN_ExtSRAM */


/**
  * @}
  */

/**
  * @}
  */
  
/**
  * @}
  */    
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
