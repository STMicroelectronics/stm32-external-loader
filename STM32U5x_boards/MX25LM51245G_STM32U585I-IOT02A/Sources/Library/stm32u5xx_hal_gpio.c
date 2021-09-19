/**
  ******************************************************************************
  * @file    stm32u5xx_hal_gpio.c
  * @author  MCD Application Team
  * @brief   GPIO HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the General Purpose Input/Output (GPIO) peripheral:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *
  @verbatim
  ==============================================================================
                    ##### GPIO Peripheral features #####
  ==============================================================================
  [..]
    (+) Each port bit of the general-purpose I/O (GPIO) ports can be individually
        configured by software in several modes:
        (++) Input mode
        (++) Analog mode
        (++) Output mode
        (++) Alternate function mode
        (++) External interrupt/event lines

    (+) During and just after reset, the alternate functions and external interrupt
        lines are not active and the I/O ports are configured in input floating mode.

    (+) All GPIO pins have weak internal pull-up and pull-down resistors, which can be
        activated or not.

    (+) In Output or Alternate mode, each IO can be configured on open-drain or push-pull
        type and the IO speed can be selected depending on the VDD value.

    (+) The microcontroller IO pins are connected to onboard peripherals/modules through a
        multiplexer that allows only one peripheral alternate function (AF) connected
       to an IO pin at a time. In this way, there can be no conflict between peripherals
       sharing the same IO pin.

    (+) All ports have external interrupt/event capability. To use external interrupt
        lines, the port must be configured in input mode. All available GPIO pins are
        connected to the 16 external interrupt/event lines from EXTI0 to EXTI15.

    (+) The external interrupt/event controller consists of up to 39 edge detectors
        (16 lines are connected to GPIO) for generating event/interrupt requests (each
        input line can be independently configured to select the type (interrupt or event)
        and the corresponding trigger event (rising or falling or both). Each line can
        also be masked independently.

                     ##### How to use this driver #####
  ==============================================================================
  [..]
    (#) Enable the GPIO AHB clock using the following function: __HAL_RCC_GPIOx_CLK_ENABLE().

    (#) Configure the GPIO pin(s) using HAL_GPIO_Init().
        (++) Configure the IO mode using "Mode" member from GPIO_InitTypeDef structure
        (++) Activate Pull-up, Pull-down resistor using "Pull" member from GPIO_InitTypeDef
             structure.
        (++) In case of Output or alternate function mode selection: the speed is
             configured through "Speed" member from GPIO_InitTypeDef structure.
        (++) In alternate mode is selection, the alternate function connected to the IO
             is configured through "Alternate" member from GPIO_InitTypeDef structure.
        (++) Analog mode is required when a pin is to be used as ADC channel
             or DAC output.
        (++) In case of external interrupt/event selection the "Mode" member from
             GPIO_InitTypeDef structure select the type (interrupt or event) and
             the corresponding trigger event (rising or falling or both).

    (#) In case of external interrupt/event mode selection, configure NVIC IRQ priority
        mapped to the EXTI line using HAL_NVIC_SetPriority() and enable it using
        HAL_NVIC_EnableIRQ().

    (#) To get the level of a pin configured in input mode use HAL_GPIO_ReadPin().

    (#) To set/reset the level of a pin configured in output mode use
        HAL_GPIO_WritePin()/HAL_GPIO_TogglePin().

   (#) To lock pin configuration until next reset use HAL_GPIO_LockPin().

    (#) During and just after reset, the alternate functions are not
        active and the GPIO pins are configured in input floating mode (except JTAG
        pins).

    (#) The LSE oscillator pins OSC32_IN and OSC32_OUT can be used as general purpose
        (PC14 and PC15, respectively) when the LSE oscillator is off. The LSE has
        priority over the GPIO function.

    (#) The HSE oscillator pins OSC_IN/OSC_OUT can be used as
        general purpose PH0 and PH1, respectively, when the HSE oscillator is off.
        The HSE has priority over the GPIO function.

  @endverbatim
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
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"

/** @addtogroup STM32U5xx_HAL_Driver
  * @{
  */

/** @defgroup GPIO GPIO
  * @brief GPIO HAL module driver
  * @{
  */

#ifdef HAL_GPIO_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/** @defgroup GPIO_Private_Defines GPIO Private Defines
  * @{
  */
#define GPIO_MODE             (0x00000003U)
#define EXTI_MODE             (0x10000000U)
#define GPIO_MODE_IT          (0x00010000U)
#define GPIO_MODE_EVT         (0x00020000U)
#define RISING_EDGE           (0x00100000U)
#define FALLING_EDGE          (0x00200000U)
#define GPIO_OUTPUT_TYPE      (0x00000010U)

#define GPIO_NUMBER           (16U)
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @defgroup GPIO_Exported_Functions_Group1 Initialization/de-initialization functions
 *  @brief    Initialization and Configuration functions
 *
@verbatim
 ===============================================================================
              ##### Initialization and de-initialization functions #####
 ===============================================================================

@endverbatim
  * @{
  */
static const LPGPIO_MapTypedef LPGPIO_Map[GPIO_NUMBER]=
{
/* LP Pins   Port    Pin */
/* Pin 0:*/  {GPIOA,   1},
/* Pin 1:*/  {GPIOA,   3},
/* Pin 2:*/  {GPIOA,   6},
/* Pin 3:*/  {GPIOB,   1},
/* Pin 4:*/  {GPIOB,  10},
/* Pin 5:*/  {GPIOC,   2},
/* Pin 6:*/  {GPIOD,  13},
/* Pin 7:*/  {GPIOD,   2},
/* Pin 8:*/  {GPIOC,  10},
/* Pin 9:*/  {GPIOB,   0},
/* Pin 10:*/ {GPIOC,  12},
/* Pin 11:*/ {GPIOB,   3},
/* Pin 12:*/ {GPIOB,   4},
/* Pin 13:*/ {GPIOE,   0},
/* Pin 14:*/ {GPIOE,   2},
/* Pin 15:*/ {GPIOE,   3},
};

/**
  * @brief  Initialize the GPIOx/LPGPIOx peripheral according to the specified parameters in the GPIO_Init.
  * @note   If GPIOx/LPGPIOx peripheral pin is used in EXTI_MODE and the pin is secure in case
  *         the system implements the security (TZEN=1), it is up to the secure application to
  *         insure that the corresponding EXTI line is set secure.
  * @param  GPIOx or LPGPIOx: where x can be (A..I) for the GPIO and (A) for LPGPIO to select the the corresponding peripheral for STM32U5 family
  * @param  GPIO_Init: pointer to a GPIO_InitTypeDef structure that contains
  *         the configuration information for the specified GPIO peripheral.
  * @retval None
  */
void HAL_GPIO_Init(GPIO_TypeDef  *GPIOx, GPIO_InitTypeDef *GPIO_Init)
{
  uint32_t tmp;
  uint32_t iocurrent;
  uint32_t pin_position;
  uint32_t position = 0U;
  GPIO_TypeDef  *GPIO;

  /* Check the parameters */
  assert_param(IS_GPIO_PIN(GPIO_Init->Pin));
  assert_param(IS_GPIO_MODE(GPIO_Init->Mode));
  assert_param(IS_GPIO_PULL(GPIO_Init->Pull));

  /* Save GPIO port address */
  GPIO = GPIOx;

  /* Configure the port pins */
  while (((GPIO_Init->Pin) >> position) != 0U)
  {
    /* Get current io position */
    iocurrent = (GPIO_Init->Pin) & (1UL << position);

    /*Save Pin Position */
    pin_position = position;

    if(iocurrent != 0U)
    {
      /*--------------------- GPIO Mode Configuration ------------------------*/
      /* In case of Alternate function mode selection */
      if(GPIOx == LPGPIOA)
      {
        /* Check the parameters */
        assert_param(IS_LPGPIO_ALL_INSTANCE(GPIOx));
        assert_param(IS_GPIO_AF(GPIO_Init->Alternate));

        tmp = GPIOx->MODER;
        tmp &= ~(LPGPIO_MODER_MOD0 << position);
        tmp |= (((GPIO_Init->Alternate & LPGPIO_MODE_OUTPUT) >> LPGPIO_MODE_POS) << position);
        GPIOx->MODER = tmp;

        /* Save  GPIO Port and pin index */
        GPIO = LPGPIO_Map[position].GPIO_PORT;
        pin_position = LPGPIO_Map[position].Pin_Pos;

        /* Configure Alternate function mapped with the current IO */
        tmp = GPIO->AFR[(pin_position) >> 3U];
        tmp &= ~(0x0FUL << (((pin_position) & 0x07U) * 4U)) ;
        tmp |= ((GPIO_Init->Alternate & 0x0FUL) << (((pin_position) & 0x07U) * 4U));
        GPIO->AFR[(pin_position) >> 3U] = tmp;

        /* Configure IO Direction mode (Alternate) */
        tmp = GPIO->MODER;
        tmp &= ~(GPIO_MODER_MODE0 << (pin_position * 2U));
        tmp |= ((GPIO_Init->Mode & GPIO_MODE) << (pin_position * 2U));
        GPIO->MODER = tmp;

      }
      else if ((GPIO_Init->Mode == GPIO_MODE_AF_PP) || (GPIO_Init->Mode == GPIO_MODE_AF_OD))
      {
        /* Check the parameters */
        assert_param(IS_GPIO_ALL_INSTANCE(GPIOx));
        assert_param(IS_GPIO_AF_INSTANCE(GPIOx));
        assert_param(IS_GPIO_AF(GPIO_Init->Alternate));

        /* Configure Alternate function mapped with the current IO */
        tmp = GPIOx->AFR[position >> 3U];
        tmp &= ~(0x0FUL << ((position & 0x07U) * 4U)) ;
        tmp |= ((GPIO_Init->Alternate & 0x0FUL) << ((position & 0x07U) * 4U));
        GPIOx->AFR[position >> 3U] = tmp;

        /* Configure IO Direction mode (Alternate) */
        tmp = GPIO->MODER;
        tmp &= ~(GPIO_MODER_MODE0 << (pin_position * 2U));
        tmp |= ((GPIO_Init->Mode & GPIO_MODE) << (pin_position * 2U));
        GPIO->MODER = tmp;
      }
      else
      {
        /* Check the parameters */
        assert_param(IS_GPIO_ALL_INSTANCE(GPIOx));

        /* Configure IO Direction mode (Input, Output, Alternate or Analog) */
        tmp = GPIO->MODER;
        tmp &= ~(GPIO_MODER_MODE0 << (pin_position * 2U));
        tmp |= ((GPIO_Init->Mode & GPIO_MODE) << (pin_position * 2U));
        GPIO->MODER = tmp;
      }

      /* In case of Output or Alternate function mode selection */
      if((GPIO_Init->Mode == GPIO_MODE_OUTPUT_PP) || (GPIO_Init->Mode == GPIO_MODE_AF_PP) ||
         (GPIO_Init->Mode == GPIO_MODE_OUTPUT_OD) || (GPIO_Init->Mode == GPIO_MODE_AF_OD))
      {
        /* Check the Speed parameter */
        assert_param(IS_GPIO_SPEED(GPIO_Init->Speed));

        /* Configure the IO Speed */
        tmp = GPIO->OSPEEDR;
        tmp &= ~(GPIO_OSPEEDR_OSPEED0 << (pin_position * 2U));
        tmp |= (GPIO_Init->Speed << (pin_position * 2U));
        GPIO->OSPEEDR = tmp;

        /* Configure the IO Output Type */
        tmp = GPIO->OTYPER;
        tmp &= ~(GPIO_OTYPER_OT0 << pin_position) ;
        tmp |= (((GPIO_Init->Mode & GPIO_OUTPUT_TYPE) >> 4U) << pin_position);
        GPIO->OTYPER = tmp;
      }

      /* Activate the Pull-up or Pull down resistor for the current IO */
      tmp = GPIO->PUPDR;
      tmp &= ~(GPIO_PUPDR_PUPD0 << (pin_position * 2U));
      tmp |= ((GPIO_Init->Pull) << (pin_position * 2U));
      GPIO->PUPDR = tmp;


      /*--------------------- EXTI Mode Configuration ------------------------*/
      /* Configure the External Interrupt or event for the current IO */
      if((GPIO_Init->Mode & EXTI_MODE) == EXTI_MODE)
      {
        tmp = EXTI->EXTICR[position >> 2U];
        tmp &= ~((0x0FUL) << (8U * (position & 0x03U)));
        tmp |= (GPIO_GET_INDEX(GPIOx) << (8U * (position & 0x03U)));
        EXTI->EXTICR[position >> 2U] = tmp;

        /* Clear EXTI line configuration */
        tmp = EXTI->IMR1;
        tmp &= ~((uint32_t)iocurrent);
        if((GPIO_Init->Mode & GPIO_MODE_IT) == GPIO_MODE_IT)
        {
          tmp |= iocurrent;
        }
        EXTI->IMR1 = tmp;

        tmp = EXTI->EMR1;
        tmp &= ~((uint32_t)iocurrent);
        if((GPIO_Init->Mode & GPIO_MODE_EVT) == GPIO_MODE_EVT)
        {
          tmp |= iocurrent;
        }
        EXTI->EMR1 = tmp;

        /* Clear Rising Falling edge configuration */
        tmp = EXTI->RTSR1;
        tmp &= ~((uint32_t)iocurrent);
        if((GPIO_Init->Mode & RISING_EDGE) == RISING_EDGE)
        {
          tmp |= iocurrent;
        }
        EXTI->RTSR1 = tmp;

        tmp = EXTI->FTSR1;
        tmp &= ~((uint32_t)iocurrent);
        if((GPIO_Init->Mode & FALLING_EDGE) == FALLING_EDGE)
        {
          tmp |= iocurrent;
        }
        EXTI->FTSR1 = tmp;
      }
    }
    position++;
  }
}

/**
  * @brief  De-initialize the GPIOx peripheral registers to their default reset values.
  * @param  GPIOx or LPGPIOx: where x can be (A..I) for the GPIO and (A) for LPGPIO to select the the corresponding peripheral for STM32U5 family
  * @param  GPIO_Pin: specifies the port bit to be written.
  *         This parameter can be one of GPIO_PIN_x where x can be (0..15).
  * @retval None
  */
void HAL_GPIO_DeInit(GPIO_TypeDef  *GPIOx, uint32_t GPIO_Pin)
{
  uint32_t tmp;
  uint32_t iocurrent;
  uint32_t pin_position;
  uint32_t position = 0U;
  GPIO_TypeDef  * GPIO;

  /* Check the parameters */
  assert_param(IS_GPIO_PIN(GPIO_Pin));

  /* Save GPIO port address */
  GPIO = GPIOx;

  /* Configure the port pins */
  while ((GPIO_Pin >> position) != 0U)
  {
    /* Get current io position */
    iocurrent = (GPIO_Pin) & (1UL << position);

    /*Save Pin Position */
    pin_position = position;

    if (iocurrent != 0U)
    {
      /* In case of LPGPIO port selected */
      if(GPIOx == LPGPIOA)
      {
        /* Check the parameters */
        assert_param(IS_LPGPIO_ALL_INSTANCE(GPIOx));

        /* Configure LP/IO in Input Mode */
        LPGPIOA->MODER &= ~(1U << pin_position);
        GPIO  = LPGPIO_Map[pin_position].GPIO_PORT;
        pin_position = LPGPIO_Map[position].Pin_Pos;
      }
      else
      {
        /* Check the parameters */
        assert_param(IS_GPIO_ALL_INSTANCE(GPIOx));

        /*------------------------- EXTI Mode Configuration --------------------*/
        /* Clear the External Interrupt or Event for the current IO */
        tmp = EXTI->EXTICR[position >> 2U];
        tmp &= ((0x0FUL) << (8U * (position & 0x03U)));
        if(tmp == (GPIO_GET_INDEX(GPIOx) << (8U * (position & 0x03U))))
        {
          /* Clear EXTI line configuration */
          EXTI->IMR1 &= ~(iocurrent);
          EXTI->EMR1 &= ~(iocurrent);

          /* Clear Rising Falling edge configuration */
          EXTI->RTSR1 &= ~(iocurrent);
          EXTI->FTSR1 &= ~(iocurrent);

          tmp = (0x0FUL) << (8U * (position & 0x03U));
          EXTI->EXTICR[position >> 2U] &= ~tmp;
        }
      }

      /*------------------------- GPIO Mode Configuration --------------------*/
      /* Configure IO in Analog Mode */
      GPIO->MODER |= (GPIO_MODER_MODE0 << (pin_position * 2U));

      /* Configure the default Alternate Function in current IO */
      GPIO->AFR[pin_position >> 3U] &= ~(0x0FUL << ((pin_position & 0x07U) * 4U)) ;

      /* Configure the default value for IO Speed */
      GPIO->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED0 << (pin_position * 2U));

      /* Configure the default value IO Output Type */
      GPIO->OTYPER  &= ~(GPIO_OTYPER_OT0 << pin_position) ;

      /* Deactivate the Pull-up and Pull-down resistor for the current IO */
      GPIO->PUPDR &= ~(GPIO_PUPDR_PUPD0 << (pin_position * 2U));
    }

    position++;
  }
}

/**
  * @}
  */

/** @addtogroup GPIO_Exported_Functions_Group2
 *  @brief GPIO/LPGPIO Read, Write, Toggle, Lock and EXTI management functions.
 *
@verbatim
 ===============================================================================
                       ##### IO operation functions #####
 ===============================================================================

@endverbatim
  * @{
  */

/**
  * @brief  Read the specified input port pin.
  * @param  GPIOx or LPGPIOx: where x can be (A..I) for the GPIO and (A) for LPGPIO to select the the corresponding peripheral for STM32U5 family
  * @param  GPIO_Pin: specifies the port bit to read.
  *         This parameter can be GPIO_PIN_x where x can be (0..15).
  * @retval The input port pin value.
  */
GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
  GPIO_PinState bitstatus;

  /* Check the parameters */
  assert_param(IS_GPIO_PIN(GPIO_Pin));

  /* in LP mode writing in normal IDR has no effect */
  if((GPIOx->IDR & GPIO_Pin) != (uint32_t)GPIO_PIN_RESET)
  {
    bitstatus = GPIO_PIN_SET;
  }
  else
  {
    bitstatus = GPIO_PIN_RESET;
  }

  return bitstatus;
}

/**
  * @brief  Set or clear the selected data port bit.
  * @note   This function uses GPIOx/LPGPIOx_BSRR and GPIOx/LPGPIOx_BRR registers to allow atomic read/modify
  *         accesses. In this way, there is no risk of an IRQ occurring between
  *         the read and the modify access.
  * @param  GPIOx or LPGPIOx: where x can be (A..I) for the GPIO and (A) for LPGPIO to select the the corresponding peripheral for STM32U5 family
  * @param  GPIO_Pin: specifies the port bit to be written.
  *         This parameter can be any combination of GPIO_Pin_x where x can be (0..15).
  * @param  PinState: specifies the value to be written to the selected bit.
  *         This parameter can be one of the GPIO_PinState enum values:
  *            @arg GPIO_PIN_RESET: to clear the port pin
  *            @arg GPIO_PIN_SET: to set the port pin
  * @retval None
  */
void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
{
  /* Check the parameters */
  assert_param(IS_GPIO_PIN(GPIO_Pin));
  assert_param(IS_GPIO_PIN_ACTION(PinState));

  if(PinState != GPIO_PIN_RESET)
  {
    GPIOx->BSRR = (uint32_t)GPIO_Pin;
  }
  else
  {
    GPIOx->BRR  = (uint32_t)GPIO_Pin;
  }
}

/**
  * @brief  Toggle the specified GPIO pin.
  * @param  GPIOx or LPGPIOx: where x can be (A..I) for the GPIO and (A) for LPGPIO to select the the corresponding peripheral for STM32U5 family
  * @param  GPIO_Pin specifies the pin to be toggled.
  *         This parameter can be any combination of GPIO_Pin_x where x can be (0..15).
  * @retval None
  */
void HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
  uint32_t odr;

  /* Check the parameters */
  assert_param(IS_GPIO_PIN(GPIO_Pin));

  /* get current Ouput Data Register value */
  odr = GPIOx->ODR;

  /* Set selected pins that were at low level, and reset ones that were high */
  GPIOx->BSRR = ((odr & GPIO_Pin) << GPIO_NUMBER) | (~odr & GPIO_Pin);
}

/**
* @brief  Lock GPIO Pins configuration registers.
  * @note   The locked registers are GPIOx_MODER, GPIOx_OTYPER, GPIOx_OSPEEDR,
  *         GPIOx_PUPDR, GPIOx_AFRL and GPIOx_AFRH.
  * @note   The configuration of the locked GPIO pins can no longer be modified
  *         until the next reset.
  * @param  GPIOx: where x can be (A..I) to select the GPIO peripheral for STM32U5 family
  * @param  GPIO_Pin: specifies the port bits to be locked.
  *         This parameter can be any combination of GPIO_Pin_x where x can be (0..15).
  * @retval None
  */
HAL_StatusTypeDef HAL_GPIO_LockPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  __IO uint32_t tmp = GPIO_LCKR_LCKK;

  /* Check the parameters */
  assert_param(IS_GPIO_LOCK_INSTANCE(GPIOx));
  assert_param(IS_GPIO_PIN(GPIO_Pin));

  /* Apply lock key write sequence */
  tmp |= GPIO_Pin;
  /* Set LCKx bit(s): LCKK='1' + LCK[15-0] */
  GPIOx->LCKR = tmp;
  /* Reset LCKx bit(s): LCKK='0' + LCK[15-0] */
  GPIOx->LCKR = GPIO_Pin;
  /* Set LCKx bit(s): LCKK='1' + LCK[15-0] */
  GPIOx->LCKR = tmp;
  /* Read LCKK register. This read is mandatory to complete key lock sequence */
  tmp = GPIOx->LCKR;

  /* read again in order to confirm lock is active */
  if ((GPIOx->LCKR & GPIO_LCKR_LCKK) != 0U)
  {
    return HAL_OK;
  }
  else
  {
    return HAL_ERROR;
  }
}

/**
  * @brief  Enable speed optimization for several pin of dedicated port.
  * @note   Not all I/Os support the HSLV mode. Refer to the I/O structure in the corresponding
  *         datasheet for the list of I/Os supporting this feature. Other I/Os HSLV configuration must
  *         be kept at reset value.
  * @note   It must be used only if the I/O supply voltage is below 2.7 V.
  * @param  GPIOx: where x can be (A..I) to select the GPIO peripheral for STM32U5 family
  * @param  GPIO_Pin: specifies the port bit to be written.
  *         This parameter can be any combination of GPIO_Pin_x where x can be (0..15).
  * @retval None
  */
void HAL_GPIO_EnableHighSPeedLowVoltage(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  /* Check the parameters */
  assert_param(IS_GPIO_ALL_INSTANCE(GPIOx));
  assert_param(IS_GPIO_PIN(GPIO_Pin));

  SET_BIT(GPIOx->HSLVR, GPIO_Pin);
}

/**
  * @brief  Disable speed optimization for several pin of dedicated port.
  * @note   Not all I/Os support the HSLV mode. Refer to the I/O structure in the corresponding
  *         datasheet for the list of I/Os supporting this feature. Other I/Os HSLV configuration must
  *         be kept at reset value.
  * @note   It must be used only if the I/O supply voltage is below 2.7 V.
  * @param  GPIOx: where x can be (A..I) to select the GPIO peripheral for STM32U5 family
  * @param  GPIO_Pin: specifies the port bit to be written.
  *         This parameter can be any combination of GPIO_Pin_x where x can be (0..15).
  * @retval None
  */
void HAL_GPIO_DisableHighSPeedLowVoltage(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  /* Check the parameters */
  assert_param(IS_GPIO_ALL_INSTANCE(GPIOx));
  assert_param(IS_GPIO_PIN(GPIO_Pin));

  CLEAR_BIT(GPIOx->HSLVR, GPIO_Pin);
}

/**
  * @brief  Handle EXTI interrupt request.
  * @param  GPIO_Pin: Specifies the port pin connected to corresponding EXTI line.
  * @retval None
  */
void HAL_GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin)
{
  /* EXTI line interrupt detected */
  if(__HAL_GPIO_EXTI_GET_RISING_IT(GPIO_Pin) != 0U)
  {
    __HAL_GPIO_EXTI_CLEAR_RISING_IT(GPIO_Pin);
    HAL_GPIO_EXTI_Rising_Callback(GPIO_Pin);
  }

  if(__HAL_GPIO_EXTI_GET_FALLING_IT(GPIO_Pin) != 0U)
  {
    __HAL_GPIO_EXTI_CLEAR_FALLING_IT(GPIO_Pin);
    HAL_GPIO_EXTI_Falling_Callback(GPIO_Pin);
  }
}

/**
  * @brief  EXTI line rising detection callback.
  * @param  GPIO_Pin: Specifies the port pin connected to corresponding EXTI line.
  * @retval None
  */
__weak void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(GPIO_Pin);

  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_GPIO_EXTI_Rising_Callback could be implemented in the user file
   */
}

/**
  * @brief  EXTI line falling detection callback.
  * @param  GPIO_Pin: Specifies the port pin connected to corresponding EXTI line.
  * @retval None
  */
__weak void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(GPIO_Pin);

  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_GPIO_EXTI_Falling_Callback could be implemented in the user file
   */
}

/**
  * @}
  */

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)

/** @defgroup GPIO_Exported_Functions_Group3 IO attributes management functions
 *  @brief GPIO attributes management functions.
 *
@verbatim
 ===============================================================================
                       ##### IO attributes functions #####
 ===============================================================================

@endverbatim
  * @{
  */

/**
  * @brief  Configure the GPIO pins attributes.
  * @note   Available attributes are to secure GPIO pin(s), so this function is
  *         only available in secure
  * @param  GPIOx: where x can be (A..H) to select the GPIO peripheral for STM32L5 family
  * @param  GPIO_Pin: specifies the pin(s) to configure the secure attribute
  * @param  PinAttributes: specifies the pin(s) to be set in secure mode, other being set non secured.
  * @retval None
  */
void HAL_GPIO_ConfigPinAttributes(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t PinAttributes)
{
  uint32_t tmp;
  uint32_t iocurrent;
  uint32_t position = 0U;

  /* Check the parameters */
  assert_param(IS_GPIO_ALL_INSTANCE(GPIOx));
  assert_param(IS_GPIO_PIN(GPIO_Pin));
  assert_param(IS_GPIO_PIN_ATTRIBUTES(PinAttributes));

  tmp = GPIOx->SECCFGR;

  /* Configure the port pins */
  while ((GPIO_Pin >> position) != 0U)
  {
    /* Get current io position */
    iocurrent = GPIO_Pin & (1UL << position);

    if(iocurrent != 0U)
    {
      /* Configure the IO secure attribute */
      tmp &= ~(GPIO_SECCFGR_SEC0 << position) ;
      tmp |= (PinAttributes << position);
    }
    position++;
  }

  /* Set secure attributes */
  GPIOx->SECCFGR = tmp;
}

/**
  * @brief  Get the GPIO pins attributes.
  * @note   Available attributes are to secure GPIO pin(s), so this function is
  *         only available in secure
  * @param  GPIOx: where x can be (A..H) to select the GPIO peripheral for STM32u5 family
  * @param  GPIO_Pin: specifies the single pin to get the secure attribute from
  * @param  pPinAttributes: pointer to return the pin attributes.
  * @retval HAL Status.
  */
HAL_StatusTypeDef HAL_GPIO_GetConfigPinAttributes(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t *pPinAttributes)
{
  uint32_t iocurrent;
  uint32_t position = 0U;

  /* Check null pointer */
  if(pPinAttributes == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_GPIO_ALL_INSTANCE(GPIOx));
  assert_param(IS_GPIO_PIN(GPIO_Pin) && (GPIO_Pin != GPIO_PIN_All));

  /* Get secure attribute of the port pin */
  while ((GPIO_Pin >> position) != 0U)
  {
    /* Get current io position */
    iocurrent = GPIO_Pin & (1UL << position);

    if(iocurrent != 0U)
    {
      /* Get the IO secure attribute */
      if((GPIOx->SECCFGR & (GPIO_SECCFGR_SEC0 << position)) != 0U)
      {
        *pPinAttributes = GPIO_PIN_SEC;
      }
      else
      {
        *pPinAttributes = GPIO_PIN_NSEC;
      }

      break;
    }
    position++;
  }

  return HAL_OK;
}

/**
  * @}
  */

#endif /* __ARM_FEATURE_CMSE */


/**
  * @}
  */

#endif /* HAL_GPIO_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
