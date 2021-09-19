/**
  ******************************************************************************
  * @file    stm32u5xx_hal_dma.h
  * @author  MCD Application Team
  * @brief   Header file of DMA HAL module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32U5xx_HAL_DMA_H
#define STM32U5xx_HAL_DMA_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal_def.h"

/** @addtogroup STM32U5xxx_HAL_Driver
  * @{
  */

/** @addtogroup DMA
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup DMA_Exported_Types DMA Exported Types
  * @brief    DMA Exported Types
  * @{
  */

/**
  * @brief DMA Configuration Structure definition.
  */
typedef struct
{
  uint32_t Request;              /*!< Specifies the DMA request.
                                      This parameter can be a value of @ref DMA_Request_selection */

  uint32_t Direction;            /*!< Specifies if the data will be transferred from memory to peripheral,
                                      from memory to memory or from peripheral to memory.
                                      This parameter can be a value of @ref DMA_transfer_direction */

  uint32_t SrcInc;               /*!< Specifies whether the source address register should be incremented or not
                                      after a burst beat / single transfer.
                                      This parameter can be a value of @ref DMA_Source_increment_mode */

  uint32_t DestInc;              /*!< Specifies whether the destination address register should be incremented or not
                                      after a burst beat / single transfer.
                                      This parameter can be a value of @ref DMA_Destination_increment_mode */

  uint32_t SrcDataWidth;         /*!< Specifies the Source data width.
                                      This parameter can be a value of @ref DMA_Source_data_width */

  uint32_t DestDataWidth;         /*!< Specifies the Destination data width.
                                       This parameter can be a value of @ref DMA_Destination_data_size */

  uint32_t SrcBurstLength;        /*!< Specifies the Burst Length of the source memory transfers.
                                       It specifies the number of beat within a burst.
                                       This parameter can be a value between 1 and 64 */

  uint32_t DestBurstLength;       /*!< Specifies the Burst Length of the destination memory transfers.
                                       It specifies the number of beat within a burst.
                                       This parameter can be a value between 1 and 64 */

  uint32_t Priority;              /*!< Specifies the software priority for the DMA channel.
                                      This parameter can be a value of @ref DMA_Priority_level */

  uint32_t TriggerMode;           /*!< Specifies the Trigger mode: Each hit triggers:
                                       a single/burst transfer, a block transfer, a repeated block transfer or a linked-list item transfer
                                       This parameter can be a value of @ref DMA_Trigger_Mode */

  uint32_t TriggerPolarity;       /*!< Specifies the DMA Trigger Event Polarity.
                                       This parameter can be a value of @ref DMA_Trigger_Polarity */

  uint32_t TriggerSelection;      /*!< Specifies the DMA Trigger Event Selection.
                                       This parameter can be a value of @ref DMA_Trigger_Selection*/

  uint32_t TransferEventMode;     /*!< Specifies the Transfer Event mode : the complete (and the half) transfer event is generated at the end (resp. half) of
                                       a block transfer, a repeated block transfer, each linked-list item or last linked-list item.
                                       This parameter can be a value of @ref DMA_Transfer_Event_Mode  */

  uint32_t BlockHWRequest;        /*!< Specifies the Block hardware request mode: single / burst level or block level.
                                       This parameter can be a value of @ref DMA_Block_Request */

  uint32_t RepeatCount;           /*!< Specifies the Block Repeat Count: the number of repetitions of the current block.
                                       This parameter can be a value between 1 and 2048 */

  uint32_t DataExchange;          /*!< Specifies the data exchange mode.
                                       This parameter can be a value of @ref DMA_Data_Exchange */

  uint32_t DataAlignment;         /*!< Specifies the source to destination Memory data padding / alignment mode.
                                       This parameter can be a value of @ref DMA_Data_Alignment */

  uint32_t SrcAllocatedPort;      /*!< Specifies the source allocated port.
                                       This parameter can be a value of @ref DMA_Port */

  uint32_t DestAllocatedPort;     /*!< Specifies the destination allocated port.
                                       This parameter can be a value of @ref DMA_Port */

  uint32_t LinkAllocatedPort;     /*!< Specifies the linked-list allocated port.
                                       This parameter can be a value of @ref DMA_Port */

  uint32_t LinkStepMode;          /*!< Specifies the link step mode.
                                       This parameter can be a value of @ref DMA_Link_Step_Mode */

  int32_t SrcAddressOffset;       /*!< This field specifies the source address offset
                                          signed value : if > 0   then  increment the source address by offset from where the last single/burst transfer ends
                                                         if < 0   then  decrement the source address by offset from where the last single/burst transfer ends
                                                         if == 0, the next single/burst source address starts from where the last transfer ends */

  int32_t DestAddressOffset;      /*!< This field specifies the destination address offset
                                          signed value : if > 0   then  increment the destination address by offset from where the last single/burst transfer ends
                                                         if < 0   then  decrement the destination address by offset from where the last single/burst transfer ends
                                                         if == 0, the next single/burst destination address starts from where the last transfer ends */

  int32_t BlockSrcAddressOffset;  /*!< This field specifies the Next block source address offset
                                          signed value : if > 0   then  increment the next block source address by offset from where the last block ends
                                                         if < 0   then  decrement the next block source address by offset from where the last block ends
                                                         if == 0, the next block source address starts from where the last block ends */

  int32_t BlockDestAddressOffset; /*!< this field specifies the Next block destination address offset
                                          signed value : if > 0   then  increment the next block destination address by offset from where the last block ends
                                                         if < 0   then  decrement the next block destination address by offset from where the last block ends
                                                         if == 0, the next block destination address starts from where the last block ends */
} DMA_InitTypeDef;

/**
  * @brief  HAL DMA State structure definition.
  */
typedef enum
{
  HAL_DMA_STATE_RESET               = 0x00U,  /*!< DMA not yet initialized or disabled */
  HAL_DMA_STATE_READY               = 0x01U,  /*!< DMA initialized and ready for use   */
  HAL_DMA_STATE_BUSY                = 0x02U,  /*!< DMA process is ongoing              */
  HAL_DMA_STATE_ERROR               = 0x03U,  /*!< DMA error state                     */
  HAL_DMA_STATE_ABORT               = 0x04U,  /*!< DMA Abort state                     */
} HAL_DMA_StateTypeDef;

/**
  * @brief  HAL DMA Level Complete structure definition.
  */
typedef enum
{
  HAL_DMA_FULL_TRANSFER         = 0x00U,   /*!< Full transfer   */
  HAL_DMA_HALF_TRANSFER         = 0x01U,   /*!< Buffer Transfer */
} HAL_DMA_LevelCompleteTypeDef;

/**
  * @brief  HAL DMA Callbacks IDs structure definition.
  */
typedef enum
{
  HAL_DMA_XFER_CPLT_CB_ID          = 0x00U,    /*!< Full transfer */
  HAL_DMA_XFER_HALFCPLT_CB_ID      = 0x01U,    /*!< Half Transfer */
  HAL_DMA_XFER_ERROR_CB_ID         = 0x02U,    /*!< Error         */
  HAL_DMA_XFER_ABORT_CB_ID         = 0x03U,    /*!< Abort         */
  HAL_DMA_XFER_SUSPEND_CB_ID       = 0x04U,    /*!< Suspend       */
  HAL_DMA_XFER_ALL_CB_ID           = 0x05U     /*!< All           */
} HAL_DMA_CallbackIDTypeDef;

/**
  * @brief  HAL DMA linked-list node structure definition.
  * @note   The linked-list node allows to define a new DMA configuration
  *         (CTR1, CTR2, CBR1, CSAR, CDAR, CTR3, CBR2 and CLLR registers).
  *         When CLLR register is configured to a non NULL value , each time a transfer ends,
  *         a new configuration (linked-list node) is automatically loaded from the address given in CLLR register.
  */
typedef struct DMA_LinkNodeTypeDef
{
  uint32_t LinkRegisters[8];
  uint32_t IndexLLR;
  uint32_t ConfigLLR;
  struct DMA_LinkNodeTypeDef *Next;
} DMA_LinkNodeTypeDef;


/**
  * @brief  HAL DMA linked list node configuration structure definition.
  * @note   Used with HAL_DMA_LinkedList_CreateNode function.
  */
typedef struct
{
  DMA_InitTypeDef  Init;            /*!< Configuration of the specified DMA Linked-List Node      */
  uint32_t         SrcAddress;      /*!< The source memory address for the Linked-List Node       */
  uint32_t         DstAddress;      /*!< The destination memory address for the Linked-List Node  */
  uint32_t         SourceDataSize;  /*!< The source data size of a block in bytes                 */
  uint32_t         ContainCTR1;     /*!< Specifies if the Linked-list node contains CTR1 register */
  uint32_t         ContainCTR2;     /*!< Specifies if the Linked-list node contains CTR2 register */
  uint32_t         ContainCBR1;     /*!< Specifies if the Linked-list node contains CBR1 register */
  uint32_t         ContainCSAR;     /*!< Specifies if the Linked-list node contains CSAR register */
  uint32_t         ContainCDAR;     /*!< Specifies if the Linked-list node contains CDAR register */
  uint32_t         ContainCTR3;     /*!< Specifies if the Linked-list node contains CTR3 register */
  uint32_t         ContainCBR2;     /*!< Specifies if the Linked-list node contains CBR2 register */
  uint32_t         ContainCLLR;     /*!< Specifies if the Linked-list node contains CLLR register */
} DMA_LinkNodeConfTypeDef;

/**
  * @brief  DMA handle Structure definition
  */
typedef struct __DMA_HandleTypeDef
{
  DMA_Channel_TypeDef *Instance;                                                    /*!< Register base address                  */

  DMA_InitTypeDef      Init;                                                        /*!< DMA communication parameters           */

  HAL_LockTypeDef      Lock;                                                        /*!< DMA locking object                     */

  __IO HAL_DMA_StateTypeDef  State;                                                 /*!< DMA transfer state                     */

  void                *Parent;                                                      /*!< Parent object state                    */

  void                 (* XferCpltCallback)(struct __DMA_HandleTypeDef *hdma);      /*!< DMA transfer complete callback         */

  void                 (* XferHalfCpltCallback)(struct __DMA_HandleTypeDef *hdma);  /*!< DMA half transfer complete callback    */

  void                 (* XferErrorCallback)(struct __DMA_HandleTypeDef *hdma);     /*!< DMA transfer error callback            */

  void                 (* XferAbortCallback)(struct __DMA_HandleTypeDef *hdma);     /*!< DMA transfer Abort callback            */

  void                 (* XferSuspendCallback)(struct __DMA_HandleTypeDef *hdma);   /*!< DMA transfer Suspend callback          */

  DMA_LinkNodeTypeDef *FirstLinkedListNodeAddress;                                  /*!< specifies the first node address of the transfer list
                                                                                         (after the initial node defined by the Init struct).
                                                                                         This parameter is used internally by the DMA driver
                                                                                         to construct the linked-list node      */

  DMA_LinkNodeTypeDef *LastLinkedListNodeAddress;                                   /*!< specifies the last node address of the transfer list.
                                                                                         This parameter is used internally by the DMA driver
                                                                                         to construct the linked list node      */

  uint32_t             LinkedListNodeCounter;                                       /*!< Number of nodes in the DMA linked-list */

  __IO uint32_t        ErrorCode;                                                   /*!< DMA Error code                         */

} DMA_HandleTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup DMA_Exported_Constants DMA Exported Constants
  * @brief    DMA Exported constants
  * @{
  */

/** @defgroup DMA_Error_Codes DMA Error Codes
  * @brief    DMA Error Codes
  * @{
  */
#define HAL_DMA_ERROR_NONE        0x00000000U   /*!< No error                                 */
#define HAL_DMA_ERROR_DTE         0x00000001U   /*!< Data transfer error                      */
#define HAL_DMA_ERROR_ULE         0x00000002U   /*!< Update linked-list item error            */
#define HAL_DMA_ERROR_USE         0x00000004U   /*!< User setting error                       */
#define HAL_DMA_ERROR_TIMEOUT     0x00000010U   /*!< Timeout error                            */
#define HAL_DMA_ERROR_NO_XFER     0x00000020U   /*!< Abort requested with no transfer ongoing */
#define HAL_DMA_ERROR_BUSY        0x00000040U   /*!< DeInit requested with transfer ongoing   */
/**
  * @}
  */

/** @defgroup DMA_Request_selection DMA Request selection
  * @brief    DMA_Request_selection
  * @{
  */
/* GPDMA requests */
#define GPDMA_REQUEST_ADC1           0U      /*!< GPDMA HW request is ADC1          */
#define GPDMA_REQUEST_ADC2           1U      /*!< GPDMA HW request is ADC2          */
#define GPDMA_REQUEST_DAC1           2U      /*!< GPDMA HW request is DAC1          */
#define GPDMA_REQUEST_DAC2           3U      /*!< GPDMA HW request is DAC2          */
#define GPDMA_REQUEST_TIM6_UP        4U      /*!< GPDMA HW request is TIM6_UP       */
#define GPDMA_REQUEST_TIM7_UP        5U      /*!< GPDMA HW request is TIM7_UP       */
#define GPDMA_REQUEST_SPI1_RX        6U      /*!< GPDMA HW request is SPI1_RX       */
#define GPDMA_REQUEST_SPI1_TX        7U      /*!< GPDMA HW request is SPI1_TX       */
#define GPDMA_REQUEST_SPI2_RX        8U      /*!< GPDMA HW request is SPI2_RX       */
#define GPDMA_REQUEST_SPI2_TX        9U      /*!< GPDMA HW request is SPI2_TX       */
#define GPDMA_REQUEST_SPI3_RX        10U     /*!< GPDMA HW request is SPI3_RX       */
#define GPDMA_REQUEST_SPI3_TX        11U     /*!< GPDMA HW request is SPI3_TX       */
#define GPDMA_REQUEST_I2C1_RX        12U     /*!< GPDMA HW request is I2C1_RX       */
#define GPDMA_REQUEST_I2C1_TX        13U     /*!< GPDMA HW request is I2C1_TX       */
#define GPDMA_REQUEST_I2C1_EVC       14U     /*!< GPDMA HW request is I2C1_EVC      */
#define GPDMA_REQUEST_I2C2_RX        15U     /*!< GPDMA HW request is I2C2_RX       */
#define GPDMA_REQUEST_I2C2_TX        16U     /*!< GPDMA HW request is I2C2_TX       */
#define GPDMA_REQUEST_I2C2_EVC       17U     /*!< GPDMA HW request is I2C2_EVC      */
#define GPDMA_REQUEST_I2C3_RX        18U     /*!< GPDMA HW request is I2C3_RX       */
#define GPDMA_REQUEST_I2C3_TX        19U     /*!< GPDMA HW request is I2C3_TX       */
#define GPDMA_REQUEST_I2C3_EVC       20U     /*!< GPDMA HW request is I2C3_EVC      */
#define GPDMA_REQUEST_I2C4_RX        21U     /*!< GPDMA HW request is I2C4_RX       */
#define GPDMA_REQUEST_I2C4_TX        22U     /*!< GPDMA HW request is I2C4_TX       */
#define GPDMA_REQUEST_I2C4_EVC       23U     /*!< GPDMA HW request is I2C4_EVC      */
#define GPDMA_REQUEST_USART1_RX      24U     /*!< GPDMA HW request is USART1_RX     */
#define GPDMA_REQUEST_USART1_TX      25U     /*!< GPDMA HW request is USART1_TX     */
#define GPDMA_REQUEST_USART2_RX      26U     /*!< GPDMA HW request is USART2_RX     */
#define GPDMA_REQUEST_USART2_TX      27U     /*!< GPDMA HW request is USART2_TX     */
#define GPDMA_REQUEST_USART3_RX      28U     /*!< GPDMA HW request is USART3_RX     */
#define GPDMA_REQUEST_USART3_TX      29U     /*!< GPDMA HW request is USART3_TX     */
#define GPDMA_REQUEST_UART4_RX       30U     /*!< GPDMA HW request is UART4_RX      */
#define GPDMA_REQUEST_UART4_TX       31U     /*!< GPDMA HW request is UART4_TX      */
#define GPDMA_REQUEST_UART5_RX       32U     /*!< GPDMA HW request is UART5_RX      */
#define GPDMA_REQUEST_UART5_TX       33U     /*!< GPDMA HW request is UART5_TX      */
#define GPDMA_REQUEST_LPUART_RX      34U     /*!< GPDMA HW request is LPUART_RX     */
#define GPDMA_REQUEST_LPUART_TX      35U     /*!< GPDMA HW request is LPUART_TX     */
#define GPDMA_REQUEST_SAI1_A         36U     /*!< GPDMA HW request is SAI1_A        */
#define GPDMA_REQUEST_SAI1_B         37U     /*!< GPDMA HW request is SAI1_B        */
#define GPDMA_REQUEST_SAI2_A         38U     /*!< GPDMA HW request is SAI2_A        */
#define GPDMA_REQUEST_SAI2_B         39U     /*!< GPDMA HW request is SAI2_B        */
#define GPDMA_REQUEST_OCTOSPI1       40U     /*!< GPDMA HW request is OCTOSPI1      */
#define GPDMA_REQUEST_OCTOSPI2       41U     /*!< GPDMA HW request is OCTOSPI2      */
#define GPDMA_REQUEST_TIM1_CH1       42U     /*!< GPDMA HW request is TIM1_CH1      */
#define GPDMA_REQUEST_TIM1_CH2       43U     /*!< GPDMA HW request is TIM1_CH2      */
#define GPDMA_REQUEST_TIM1_CH3       44U     /*!< GPDMA HW request is TIM1_CH3      */
#define GPDMA_REQUEST_TIM1_CH4       45U     /*!< GPDMA HW request is TIM1_CH4      */
#define GPDMA_REQUEST_TIM1_UP        46U     /*!< GPDMA HW request is TIM1_UP       */
#define GPDMA_REQUEST_TIM1_TRIG      47U     /*!< GPDMA HW request is TIM1_TRIG     */
#define GPDMA_REQUEST_TIM1_COM       48U     /*!< GPDMA HW request is TIM1_COM      */
#define GPDMA_REQUEST_TIM8_CH1       49U     /*!< GPDMA HW request is TIM8_CH1      */
#define GPDMA_REQUEST_TIM8_CH2       50U     /*!< GPDMA HW request is TIM8_CH2      */
#define GPDMA_REQUEST_TIM8_CH3       51U     /*!< GPDMA HW request is TIM8_CH3      */
#define GPDMA_REQUEST_TIM8_CH4       52U     /*!< GPDMA HW request is TIM8_CH4      */
#define GPDMA_REQUEST_TIM8_UP        53U     /*!< GPDMA HW request is TIM8_UP       */
#define GPDMA_REQUEST_TIM8_TRIG      54U     /*!< GPDMA HW request is TIM8_TRIG     */
#define GPDMA_REQUEST_TIM8_COM       55U     /*!< GPDMA HW request is TIM8_COM      */
#define GPDMA_REQUEST_TIM2_CH1       56U     /*!< GPDMA HW request is TIM2_CH1      */
#define GPDMA_REQUEST_TIM2_CH2       57U     /*!< GPDMA HW request is TIM2_CH2      */
#define GPDMA_REQUEST_TIM2_CH3       58U     /*!< GPDMA HW request is TIM2_CH3      */
#define GPDMA_REQUEST_TIM2_CH4       59U     /*!< GPDMA HW request is TIM2_CH4      */
#define GPDMA_REQUEST_TIM2_UP        60U     /*!< GPDMA HW request is TIM2_UP       */
#define GPDMA_REQUEST_TIM3_CH1       61U     /*!< GPDMA HW request is TIM3_CH1      */
#define GPDMA_REQUEST_TIM3_CH2       62U     /*!< GPDMA HW request is TIM3_CH2      */
#define GPDMA_REQUEST_TIM3_CH3       63U     /*!< GPDMA HW request is TIM3_CH3      */
#define GPDMA_REQUEST_TIM3_CH4       64U     /*!< GPDMA HW request is TIM3_CH4      */
#define GPDMA_REQUEST_TIM3_UP        65U     /*!< GPDMA HW request is TIM3_UP       */
#define GPDMA_REQUEST_TIM3_TRIG      66U     /*!< GPDMA HW request is TIM3_TRIG     */
#define GPDMA_REQUEST_TIM4_CH1       67U     /*!< GPDMA HW request is TIM4_CH1      */
#define GPDMA_REQUEST_TIM4_CH2       68U     /*!< GPDMA HW request is TIM4_CH2      */
#define GPDMA_REQUEST_TIM4_CH3       69U     /*!< GPDMA HW request is TIM4_CH3      */
#define GPDMA_REQUEST_TIM4_CH4       70U     /*!< GPDMA HW request is TIM4_CH4      */
#define GPDMA_REQUEST_TIM4_UP        71U     /*!< GPDMA HW request is TIM4_UP       */
#define GPDMA_REQUEST_TIM5_CH1       72U     /*!< GPDMA HW request is TIM5_CH1      */
#define GPDMA_REQUEST_TIM5_CH2       73U     /*!< GPDMA HW request is TIM5_CH2      */
#define GPDMA_REQUEST_TIM5_CH3       74U     /*!< GPDMA HW request is TIM5_CH3      */
#define GPDMA_REQUEST_TIM5_CH4       75U     /*!< GPDMA HW request is TIM5_CH4      */
#define GPDMA_REQUEST_TIM5_UP        76U     /*!< GPDMA HW request is TIM5_UP       */
#define GPDMA_REQUEST_TIM5_TRIG      77U     /*!< GPDMA HW request is TIM5_TRIG     */
#define GPDMA_REQUEST_TIM15_CH1      78U     /*!< GPDMA HW request is TIM15_CH1     */
#define GPDMA_REQUEST_TIM15_UP       79U     /*!< GPDMA HW request is TIM15_UP      */
#define GPDMA_REQUEST_TIM15_TRIG     80U     /*!< GPDMA HW request is TIM15_TRIG    */
#define GPDMA_REQUEST_TIM15_COM      81U     /*!< GPDMA HW request is TIM15_COM     */
#define GPDMA_REQUEST_TIM16_CH1      82U     /*!< GPDMA HW request is TIM16_CH1     */
#define GPDMA_REQUEST_TIM16_UP       83U     /*!< GPDMA HW request is TIM16_UP      */
#define GPDMA_REQUEST_TIM17_CH1      84U     /*!< GPDMA HW request is TIM17_CH1     */
#define GPDMA_REQUEST_TIM17_UP       85U     /*!< GPDMA HW request is TIM17_UP      */
#define GPDMA_REQUEST_DCMI           86U     /*!< GPDMA HW request is DCMI          */
#define GPDMA_REQUEST_AES_IN         87U     /*!< GPDMA HW request is AES_IN        */
#define GPDMA_REQUEST_AES_OUT        88U     /*!< GPDMA HW request is AES_OUT       */
#define GPDMA_REQUEST_HASH_IN        89U     /*!< GPDMA HW request is HASH_IN       */
#define GPDMA_REQUEST_USBPD_TX       90U     /*!< GPDMA HW request is USBPD_TX      */
#define GPDMA_REQUEST_USBPD_RX       91U     /*!< GPDMA HW request is USBPD_RX      */
#define GPDMA_REQUEST_MDF1_FLT0      92U     /*!< GPDMA HW request is MDF1_FLT0     */
#define GPDMA_REQUEST_MDF1_FLT1      93U     /*!< GPDMA HW request is MDF1_FLT1     */
#define GPDMA_REQUEST_MDF1_FLT2      94U     /*!< GPDMA HW request is MDF1_FLT2     */
#define GPDMA_REQUEST_MDF1_FLT3      95U     /*!< GPDMA HW request is MDF1_FLT3     */
#define GPDMA_REQUEST_MDF1_FLT4      96U     /*!< GPDMA HW request is MDF1_FLT4     */
#define GPDMA_REQUEST_MDF1_FLT5      97U     /*!< GPDMA HW request is MDF1_FLT5     */
#define GPDMA_REQUEST_MDF2_FLT0      98U     /*!< GPDMA HW request is MDF2_FLT0     */
#define GPDMA_REQUEST_FMAC_READ      99U     /*!< GPDMA HW request is FMAC_READ     */
#define GPDMA_REQUEST_FMAC_WRITE     100U    /*!< GPDMA HW request is FMAC_WRITE    */
#define GPDMA_REQUEST_CORDIC_READ    101U    /*!< GPDMA HW request is CORDIC_READ   */
#define GPDMA_REQUEST_CORDIC_WRITE   102U    /*!< GPDMA HW request is CORDIC_WRITE  */
#define GPDMA_REQUEST_SAES_IN        103U    /*!< GPDMA HW request is SAES_IN       */
#define GPDMA_REQUEST_SAES_OUT       104U    /*!< GPDMA HW request is SAES_OUT      */
#define GPDMA_REQUEST_LPTIM1_IC1     105U    /*!< GPDMA HW request is LPTIM1_IC1    */
#define GPDMA_REQUEST_LPTIM1_IC2     106U    /*!< GPDMA HW request is LPTIM1_IC2    */
#define GPDMA_REQUEST_LPTIM1_UE      107U    /*!< GPDMA HW request is LPTIM1_UE     */
#define GPDMA_REQUEST_LPTIM2_IC1     108U    /*!< GPDMA HW request is LPTIM2_IC1    */
#define GPDMA_REQUEST_LPTIM2_IC2     109U    /*!< GPDMA HW request is LPTIM2_IC2    */
#define GPDMA_REQUEST_LPTIM2_UE      110U    /*!< GPDMA HW request is LPTIM2_UE     */
#define GPDMA_REQUEST_LPTIM3_IC1     111U    /*!< GPDMA HW request is LPTIM3_IC1    */
#define GPDMA_REQUEST_LPTIM3_IC2     112U    /*!< GPDMA HW request is LPTIM3_IC2    */
#define GPDMA_REQUEST_LPTIM3_UE      113U    /*!< GPDMA HW request is LPTIM3_UE     */

/* LPDMA requests */
#define LPDMA_REQUEST_LPUART_RX      0U      /*!< LPDMA HW request is LPUART_RX     */
#define LPDMA_REQUEST_LPUART_TX      1U      /*!< LPDMA HW request is LPUART_TX     */
#define LPDMA_REQUEST_SPI3_RX        2U      /*!< LPDMA HW request is SPI3_RX       */
#define LPDMA_REQUEST_SPI3_TX        3U      /*!< LPDMA HW request is SPI3_TX       */
#define LPDMA_REQUEST_I2C3_RX        4U      /*!< LPDMA HW request is I2C3_RX       */
#define LPDMA_REQUEST_I2C3_TX        5U      /*!< LPDMA HW request is I2C3_TX       */
#define LPDMA_REQUEST_I2C3_EVC       6U      /*!< LPDMA HW request is I2C3_EVC      */
#define LPDMA_REQUEST_ADC2           7U      /*!< LPDMA HW request is ADC2          */
#define LPDMA_REQUEST_DAC1           8U      /*!< LPDMA HW request is DAC1          */
#define LPDMA_REQUEST_DAC2           9U      /*!< LPDMA HW request is DAC2          */
#define LPDMA_REQUEST_MDF2_FLT0      10U     /*!< LPDMA HW request is MDF2_FLT0     */
#define LPDMA_REQUEST_LPTIM1_IC1     11U     /*!< LPDMA HW request is LPTIM1_IC1    */
#define LPDMA_REQUEST_LPTIM1_IC2     12U     /*!< LPDMA HW request is LPTIM1_IC2    */
#define LPDMA_REQUEST_LPTIM1_UE      13U     /*!< LPDMA HW request is LPTIM1_UE     */
#define LPDMA_REQUEST_LPTIM3_IC1     14U     /*!< LPDMA HW request is LPTIM3_IC1    */
#define LPDMA_REQUEST_LPTIM3_IC2     15U     /*!< LPDMA HW request is LPTIM3_IC2    */
#define LPDMA_REQUEST_LPTIM3_UE      16U     /*!< LPDMA HW request is LPTIM3_UE     */

/* Software request */
#define DMA_REQUEST_SW               DMA_CTR2_SWREQ /*!< DMA SW request          */

/**
  * @}
  */

/** @defgroup DMA_transfer_direction DMA transfer direction
  * @brief    DMA transfer direction
  * @{
  */
#define DMA_PERIPH_TO_MEMORY     0x00000000U      /*!< Peripheral to memory direction */
#define DMA_MEMORY_TO_PERIPH     DMA_CTR2_DREQ    /*!< Memory to peripheral direction */
#define DMA_MEMORY_TO_MEMORY     DMA_CTR2_SWREQ   /*!< Memory to memory direction     */
/**
  * @}
  */

/** @defgroup DMA_Source_increment_mode DMA Source increment mode
  * @brief    DMA Source increment mode
  * @{
  */
#define DMA_SINC_FIXED           0x00000000U      /*!< Source fixed       single / burst */
#define DMA_SINC_INCREMENTED     DMA_CTR1_SINC    /*!< Source incremented single / burst */
/**
  * @}
  */

/** @defgroup DMA_Destination_increment_mode DMA Destination increment mode
  * @brief    DMA Destination increment mode
  * @{
  */
#define DMA_DINC_FIXED           0x00000000U      /*!< Destination fixed       single / burst */
#define DMA_DINC_INCREMENTED     DMA_CTR1_DINC    /*!< Destination incremented single / burst */
/**
  * @}
  */

/** @defgroup DMA_Source_data_width DMA Source data width
  * @brief    DMA source data width
  * @{
  */
#define DMA_SRC_DATAWIDTH_BYTE      0x00000000U          /*!< Source data width: Byte     */
#define DMA_SRC_DATAWIDTH_HALFWORD  DMA_CTR1_SDW_LOG2_0  /*!< Source data width: HalfWord */
#define DMA_SRC_DATAWIDTH_WORD      DMA_CTR1_SDW_LOG2_1  /*!< Source data width: Word     */
/**
  * @}
  */

/** @defgroup DMA_Destination_data_width DMA Destination data width
  * @brief    DMA destination data width
  * @{
  */
#define DMA_DEST_DATAWIDTH_BYTE      0x00000000U          /*!< Destination data width: Byte     */
#define DMA_DEST_DATAWIDTH_HALFWORD  DMA_CTR1_DDW_LOG2_0  /*!< Destination data width: HalfWord */
#define DMA_DEST_DATAWIDTH_WORD      DMA_CTR1_DDW_LOG2_1  /*!< Destination data width: Word     */
/**
  * @}
  */

/** @defgroup DMA_Trigger_Polarity DMA trigger polarity
  * @brief    DMA Trigger Polarity
  * @{
  */
#define DMA_TRIG_POLARITY_MASKED    0x00000000U         /*!< No trigger of the selected DMA request. Masked trigger event                                */
#define DMA_TRIG_POLARITY_RISING    DMA_CTR2_TRIGPOL_0  /*!< Trigger of the selected DMA request on the rising edge of the selected trigger event input  */
#define DMA_TRIG_POLARITY_FALLING   DMA_CTR2_TRIGPOL_1  /*!< Trigger of the selected DMA request on the falling edge of the selected trigger event input */
/**
  * @}
  */

/** @defgroup DMA_Trigger_Mode DMA Transfer Trigger Mode
  * @brief    DMA Trigger Mode
  * @{
  */
#define DMA_TRIGM_BLOCK_TRANSFER           0x00000000U        /*!< A block transfer is conditioned by (at least) one hit trigger          */
#define DMA_TRIGM_REPEATED_BLOCK_TRANSFER  DMA_CTR2_TRIGM_0   /*!< A repeated block transfer is conditioned by (at least) one hit trigger */
#define DMA_TRIGM_LLI_LINK_TRANSFER        DMA_CTR2_TRIGM_1   /*!< A LLI link transfer is conditioned by (at least) one hit trigger       */
#define DMA_TRIGM_SINGLE_BURST_TRANSFER    DMA_CTR2_TRIGM     /*!< A single/burst transfer is conditioned by (at least) one hit trigger   */
/**
  * @}
  */

/** @defgroup DMA_Trigger_Selection DMA Transfer Trigger Selection
  * @brief    DMA Trigger Selection
  * @{
  */
/* GPDMA triggers */
#define GPDMA_TRIGGER_EXTI_LINE0       0U       /*!< GPDMA HW Trigger signal is EXTI_LINE0      */
#define GPDMA_TRIGGER_EXTI_LINE1       1U       /*!< GPDMA HW Trigger signal is EXTI_LINE1      */
#define GPDMA_TRIGGER_EXTI_LINE2       2U       /*!< GPDMA HW Trigger signal is EXTI_LINE2      */
#define GPDMA_TRIGGER_EXTI_LINE3       3U       /*!< GPDMA HW Trigger signal is EXTI_LINE3      */
#define GPDMA_TRIGGER_EXTI_LINE4       4U       /*!< GPDMA HW Trigger signal is EXTI_LINE4      */
#define GPDMA_TRIGGER_EXTI_LINE5       5U       /*!< GPDMA HW Trigger signal is EXTI_LINE5      */
#define GPDMA_TRIGGER_EXTI_LINE6       6U       /*!< GPDMA HW Trigger signal is EXTI_LINE6      */
#define GPDMA_TRIGGER_EXTI_LINE7       7U       /*!< GPDMA HW Trigger signal is EXTI_LINE7      */
#define GPDMA_TRIGGER_TAMP_TRG1        8U       /*!< GPDMA HW Trigger signal is TAMP_TRG1       */
#define GPDMA_TRIGGER_TAMP_TRG2        9U       /*!< GPDMA HW Trigger signal is TAMP_TRG2       */
#define GPDMA_TRIGGER_TAMP_TRG3        10U      /*!< GPDMA HW Trigger signal is TAMP_TRG3       */
#define GPDMA_TRIGGER_LPTIM1_CH1       11U      /*!< GPDMA HW Trigger signal is LPTIM1_CH1      */
#define GPDMA_TRIGGER_LPTIM1_CH2       12U      /*!< GPDMA HW Trigger signal is LPTIM1_CH2      */
#define GPDMA_TRIGGER_LPTIM2_CH1       13U      /*!< GPDMA HW Trigger signal is LPTIM2_CH1      */
#define GPDMA_TRIGGER_LPTIM2_CH2       14U      /*!< GPDMA HW Trigger signal is LPTIM2_CH2      */
#define GPDMA_TRIGGER_LPTIM4_OUT       15U      /*!< GPDMA HW Trigger signal is LPTIM4_OUT      */
#define GPDMA_TRIGGER_COMP1_OUT        16U      /*!< GPDMA HW Trigger signal is COMP1_OUT       */
#define GPDMA_TRIGGER_COMP2_OUT        17U      /*!< GPDMA HW Trigger signal is COMP2_OUT       */
#define GPDMA_TRIGGER_RTC_ALRA_TRG     18U      /*!< GPDMA HW Trigger signal is RTC_ALRA_TRG    */
#define GPDMA_TRIGGER_RTC_ALRB_TRG     19U      /*!< GPDMA HW Trigger signal is RTC_ALRB_TRG    */
#define GPDMA_TRIGGER_RTC_WUT_TRG      20U      /*!< GPDMA HW Trigger signal is RTC_WUT_TRG     */
#define GPDMA_TRIGGER_GPDMA_CH0_TCF    22U      /*!< GPDMA HW Trigger signal is GPDMA_CH0_TCF   */
#define GPDMA_TRIGGER_GPDMA_CH1_TCF    23U      /*!< GPDMA HW Trigger signal is GPDMA_CH1_TCF   */
#define GPDMA_TRIGGER_GPDMA_CH2_TCF    24U      /*!< GPDMA HW Trigger signal is GPDMA_CH2_TCF   */
#define GPDMA_TRIGGER_GPDMA_CH3_TCF    25U      /*!< GPDMA HW Trigger signal is GPDMA_CH3_TCF   */
#define GPDMA_TRIGGER_GPDMA_CH4_TCF    26U      /*!< GPDMA HW Trigger signal is GPDMA_CH4_TCF   */
#define GPDMA_TRIGGER_GPDMA_CH5_TCF    27U      /*!< GPDMA HW Trigger signal is GPDMA_CH5_TCF   */
#define GPDMA_TRIGGER_GPDMA_CH6_TCF    28U      /*!< GPDMA HW Trigger signal is GPDMA_CH6_TCF   */
#define GPDMA_TRIGGER_GPDMA_CH7_TCF    29U      /*!< GPDMA HW Trigger signal is GPDMA_CH7_TCF   */
#define GPDMA_TRIGGER_GPDMA_CH8_TCF    30U      /*!< GPDMA HW Trigger signal is GPDMA_CH8_TCF   */
#define GPDMA_TRIGGER_GPDMA_CH9_TCF    31U      /*!< GPDMA HW Trigger signal is GPDMA_CH9_TCF   */
#define GPDMA_TRIGGER_GPDMA_CH10_TCF   32U      /*!< GPDMA HW Trigger signal is GPDMA_CH10_TCF  */
#define GPDMA_TRIGGER_GPDMA_CH11_TCF   33U      /*!< GPDMA HW Trigger signal is GPDMA_CH11_TCF  */
#define GPDMA_TRIGGER_GPDMA_CH12_TCF   34U      /*!< GPDMA HW Trigger signal is GPDMA_CH12_TCF  */
#define GPDMA_TRIGGER_GPDMA_CH13_TCF   35U      /*!< GPDMA HW Trigger signal is GPDMA_CH13_TCF  */
#define GPDMA_TRIGGER_GPDMA_CH14_TCF   36U      /*!< GPDMA HW Trigger signal is GPDMA_CH14_TCF  */
#define GPDMA_TRIGGER_GPDMA_CH15_TCF   37U      /*!< GPDMA HW Trigger signal is GPDMA_CH15_TCF  */
#define GPDMA_TRIGGER_LPDMA_CH0_TCF    38U      /*!< GPDMA HW Trigger signal is LPDMA_CH0_TCF   */
#define GPDMA_TRIGGER_LPDMA_CH1_TCF    39U      /*!< GPDMA HW Trigger signal is LPDMA_CH1_TCF   */
#define GPDMA_TRIGGER_LPDMA_CH2_TCF    40U      /*!< GPDMA HW Trigger signal is LPDMA_CH2_TCF   */
#define GPDMA_TRIGGER_LPDMA_CH3_TCF    41U      /*!< GPDMA HW Trigger signal is LPDMA_CH3_TCF   */
#define GPDMA_TRIGGER_TIM2_TRGO        42U      /*!< GPDMA HW Trigger signal is TIM2_TRGO       */
#define GPDMA_TRIGGER_TIM15_TRGO       43U      /*!< GPDMA HW Trigger signal is TIM15_TRGO      */

/* LPDMA triggers */
#define LPDMA_TRIGGER_EXTI_LINE0        0U      /*!< LPDMA HW Trigger signal is EXTI_LINE0      */
#define LPDMA_TRIGGER_EXTI_LINE1        1U      /*!< LPDMA HW Trigger signal is EXTI_LINE1      */
#define LPDMA_TRIGGER_EXTI_LINE2        2U      /*!< LPDMA HW Trigger signal is EXTI_LINE2      */
#define LPDMA_TRIGGER_EXTI_LINE3        3U      /*!< LPDMA HW Trigger signal is EXTI_LINE3      */
#define LPDMA_TRIGGER_EXTI_LINE4        4U      /*!< LPDMA HW Trigger signal is EXTI_LINE4      */
#define LPDMA_TRIGGER_TAMP_TRG1         5U      /*!< LPDMA HW Trigger signal is TAMP_TRG1       */
#define LPDMA_TRIGGER_TAMP_TRG2         6U      /*!< LPDMA HW Trigger signal is TAMP_TRG2       */
#define LPDMA_TRIGGER_TAMP_TRG3         7U      /*!< LPDMA HW Trigger signal is TAMP_TRG3       */
#define LPDMA_TRIGGER_LPTIM1_CH1        8U      /*!< LPDMA HW Trigger signal is LPTIM1_CH1      */
#define LPDMA_TRIGGER_LPTIM1_CH2        9U      /*!< LPDMA HW Trigger signal is LPTIM1_CH2      */
#define LPDMA_TRIGGER_LPTIM3_CH1        10U     /*!< LPDMA HW Trigger signal is LPTIM3_CH1      */
#define LPDMA_TRIGGER_LPTIM4_OUT        11U     /*!< LPDMA HW Trigger signal is LPTIM4_OUT      */
#define LPDMA_TRIGGER_COMP1_OUT         12U     /*!< LPDMA HW Trigger signal is COMP1_OUT       */
#define LPDMA_TRIGGER_COMP2_OUT         13U     /*!< LPDMA HW Trigger signal is COMP2_OUT       */
#define LPDMA_TRIGGER_RTC_ALRA_TRG      14U     /*!< LPDMA HW Trigger signal is RTC_ALRA_TRG    */
#define LPDMA_TRIGGER_RTC_ALRB_TRG      15U     /*!< LPDMA HW Trigger signal is RTC_ALRB_TRG    */
#define LPDMA_TRIGGER_RTC_WUT_TRG       16U     /*!< LPDMA HW Trigger signal is RTC_WUT_TRG     */
#define LPDMA_TRIGGER_LPDMA_CH0_TCF     18U     /*!< LPDMA HW Trigger signal is LPDMA_CH0_TCF   */
#define LPDMA_TRIGGER_LPDMA_CH1_TCF     19U     /*!< LPDMA HW Trigger signal is LPDMA_CH1_TCF   */
#define LPDMA_TRIGGER_LPDMA_CH2_TCF     20U     /*!< LPDMA HW Trigger signal is LPDMA_CH2_TCF   */
#define LPDMA_TRIGGER_LPDMA_CH3_TCF     21U     /*!< LPDMA HW Trigger signal is LPDMA_CH3_TCF   */
#define LPDMA_TRIGGER_GPDMA_CH0_TCF     22U     /*!< LPDMA HW Trigger signal is GPDMA_CH0_TCF   */
#define LPDMA_TRIGGER_GPDMA_CH1_TCF     23U     /*!< LPDMA HW Trigger signal is GPDMA_CH1_TCF   */
#define LPDMA_TRIGGER_GPDMA_CH4_TCF     24U     /*!< LPDMA HW Trigger signal is GPDMA_CH4_TCF   */
#define LPDMA_TRIGGER_GPDMA_CH5_TCF     25U     /*!< LPDMA HW Trigger signal is GPDMA_CH5_TCF   */
#define LPDMA_TRIGGER_GPDMA_CH6_TCF     26U     /*!< LPDMA HW Trigger signal is GPDMA_CH6_TCF   */
#define LPDMA_TRIGGER_GPDMA_CH7_TCF     27U     /*!< LPDMA HW Trigger signal is GPDMA_CH7_TCF   */
#define LPDMA_TRIGGER_GPDMA_CH12_TCF    28U     /*!< LPDMA HW Trigger signal is GPDMA_CH12_TCF  */
#define LPDMA_TRIGGER_GPDMA_CH13_TCF    29U     /*!< LPDMA HW Trigger signal is GPDMA_CH13_TCF  */
#define LPDMA_TRIGGER_TIM2_TRGO         30U     /*!< LPDMA HW Trigger signal is TIM2_TRGO       */
#define LPDMA_TRIGGER_TIM15_TRGO        31U     /*!< LPDMA HW Trigger signal is TIM15_TRGO      */

/**
  * @}
  */

/** @defgroup DMA_Transfer_Event_Mode DMA Transfer Event Mode
  * @brief    DMA Transfer Event Mode
  * @{
  */
#define DMA_TCEM_BLOCK_TRANSFER           0x00000000U       /*!< The TC (and the HT) event is generated at the (respectively half) end of each block                */
#define DMA_TCEM_REPEATED_BLOCK_TRANSFER  DMA_CTR2_TCEM_0   /*!< The TC (and the HT) event is generated at the (respectively half) end of the repeated block        */
#define DMA_TCEM_EACH_LL_ITEM_TRANSFER    DMA_CTR2_TCEM_1   /*!< The TC (and the HT) event is generated at the (respectively half) end of each linked-list item     */
#define DMA_TCEM_LAST_LL_ITEM_TRANSFER    DMA_CTR2_TCEM     /*!< The TC (and the HT) event is generated at the (respectively half) end of the last linked-list item */
/**
  * @}
  */

/** @defgroup DMA_Data_Alignment DMA Data Alignment
  * @brief    DMA Data Alignment
  * @note     If src data width == dest data width => Data Alignment (PAM[1:0]) is ignored by hardware.
  *
  * @{
  */
#define DMA_DATA_RIGHTALIGN_ZEROPADDED  0x00000000U     /*!< If src data width < dest data width => Right Aligned padded with 0 up to destination data width             */
#define DMA_DATA_RIGHTALIGN_LEFTTRUNC   0x00000000U     /*!< If src data width > dest data width => Right Aligned Left Truncated down to destination data width          */
#define DMA_DATA_RIGHTALIGN_SIGNEXT     DMA_CTR1_PAM_0  /*!< If src data width < dest data width => Right Aligned padded with sign extended up to destination data width */
#define DMA_DATA_LEFTALIGN_RIGHTTRUNC   DMA_CTR1_PAM_0  /*!< If src data width > dest data width => Left Aligned Right Truncated down to the destination data width      */
#define DMA_DATA_PACK                   DMA_CTR1_PAM_1  /*!< If src data width < dest data width => Packed at the destination data width   (Available only for GPDMA)    */
#define DMA_DATA_UNPACK                 DMA_CTR1_PAM_1  /*!< If src data width > dest data width => Unpacked at the destination data width (Available only for GPDMA)    */
/**
  * @}
  */

/** @defgroup DMA_Priority_level DMA Priority level
  * @brief    DMA Priority level
  * @{
  */
#define DMA_LOW_PRIORITY_LOW_WEIGHT   0x00000000U      /*!< Priority level : Low Priority, Low weight  */
#define DMA_LOW_PRIORITY_MID_WEIGHT   DMA_CCR_PRIO_0   /*!< Priority level : Low Priority, Mid weight  */
#define DMA_LOW_PRIORITY_HIGH_WEIGHT  DMA_CCR_PRIO_1   /*!< Priority level : Low Priority, High weight */
#define DMA_HIGH_PRIORITY             DMA_CCR_PRIO     /*!< Priority level : HIGH Priority             */
/**
  * @}
  */

/** @defgroup DMA_Data_Exchange DMA Data Exchange
  * @brief    DMA Data Exchange
  * @{
  */
#define DMA_EXCHANGE_NONE           0x00000000U   /*!< No data exchange                                                          */
#define DMA_EXCHANGE_DEST_BYTE      DMA_CTR1_DBX  /*!< Destination Byte exchange when destination data width is > Byte           */
#define DMA_EXCHANGE_DEST_HALFWORD  DMA_CTR1_DHX  /*!< Destination Half-Word exchange when destination data width is > Half-Word */
#define DMA_EXCHANGE_SRC_BYTE       DMA_CTR1_SBX  /*!< Source Byte endianness exchange when source data width is word            */
/**
  * @}
  */

/** @defgroup DMA_interrupt_enable_definition DMA interrupt enable definition
  * @brief    DMA interrupt enable definition
  * @{
  */
#define DMA_IT_TC   DMA_CCR_TCIE   /*!< Transfer Complete interrupt             */
#define DMA_IT_HT   DMA_CCR_HTIE   /*!< Half Transfer Complete interrupt        */
#define DMA_IT_DTE  DMA_CCR_DTEIE  /*!< Data Transfer Error interrupt           */
#define DMA_IT_ULE  DMA_CCR_ULEIE  /*!< Update Linked-list item Error interrupt */
#define DMA_IT_USE  DMA_CCR_USEIE  /*!< User Setting Error interrupt            */
#define DMA_IT_SUSP DMA_CCR_SUSPIE /*!< Completed Suspension interrupt          */
/**
  * @}
  */

/** @defgroup DMA_flag_definition DMA flag definition
  * @brief    DMA flag definition
  * @{
  */
#define DMA_FLAG_IDLE  DMA_CSR_IDLEF /*!< Idle flag                          */
#define DMA_FLAG_TC    DMA_CSR_TCF   /*!< Transfer Complete flag             */
#define DMA_FLAG_HT    DMA_CSR_HTF   /*!< Half Transfer Complete flag        */
#define DMA_FLAG_DTE   DMA_CSR_DTEF  /*!< Data Transfer Error flag           */
#define DMA_FLAG_ULE   DMA_CSR_ULEF  /*!< Update Linked-list item Error flag */
#define DMA_FLAG_USE   DMA_CSR_USEF  /*!< User Setting Error flag            */
#define DMA_FLAG_SUSP  DMA_CSR_SUSPF /*!< Completed Suspension flag          */
/**
  * @}
  */

/** @defgroup DMA_Port DMA allocated Port
  * @brief    DMA Port
  * @{
  */
#define DMA_PORT0    0x00U   /*!< Port 0 */
#define DMA_PORT1    0x01U   /*!< Port 1 */
/**
  * @}
  */

/** @defgroup DMA_Block_Request DMA Block Request
  * @brief    DMA Block Request
  * @{
  */
#define DMA_BREQ_SINGLE_BURST    0x00000000U     /*!< Hardware request protocol at a single / burst level */
#define DMA_BREQ_BLOCK           DMA_CTR2_BREQ   /*!< Hardware request protocol at a block level          */
/**
  * @}
  */

/** @defgroup DMA_Link_Step_Mode DMA Link Step Mode
  * @brief    DMA Link Step Mode
  * @{
  */
#define DMA_LSM_FULL_EXECUTION    0x00000000U   /*!< Channel is executed for the full linked-list */
#define DMA_LSM_1LINK_EXECUTION   DMA_CCR_LSM   /*!< Channel is executed once for the current LLI */
/**
  * @}
  */

/** @defgroup DMA_Channel_Attributes DMA Channel Attributes
  * @brief DMA channel secure or non-secure and privileged or non-privileged attributes
  * @note Secure and non-secure attributes are only available from secure when the system
  *       implement the security (TZEN=1)
  * @{
  */
#define DMA_CHANNEL_ATTR_PRIV_MASK        0x10U
#define DMA_CHANNEL_ATTR_SEC_MASK         0x20U
#define DMA_CHANNEL_ATTR_SEC_SRC_MASK     0x40U
#define DMA_CHANNEL_ATTR_SEC_DEST_MASK    0x80U

#define DMA_CHANNEL_PRIV          (DMA_CHANNEL_ATTR_PRIV_MASK | 0x01U)     /*!< Channel is privileged             */
#define DMA_CHANNEL_NPRIV         (DMA_CHANNEL_ATTR_PRIV_MASK)             /*!< Channel is unprivileged           */
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
#define DMA_CHANNEL_SEC           (DMA_CHANNEL_ATTR_SEC_MASK | 0x02U)      /*!< Channel is secure                 */
#define DMA_CHANNEL_NSEC          (DMA_CHANNEL_ATTR_SEC_MASK)              /*!< Channel is non-secure             */
#define DMA_CHANNEL_SRC_SEC       (DMA_CHANNEL_ATTR_SEC_SRC_MASK | 0x04U)  /*!< Channel source is secure          */
#define DMA_CHANNEL_SRC_NSEC      (DMA_CHANNEL_ATTR_SEC_SRC_MASK)          /*!< Channel source is non-secure      */
#define DMA_CHANNEL_DEST_SEC      (DMA_CHANNEL_ATTR_SEC_DEST_MASK | 0x08U) /*!< Channel destination is secure     */
#define DMA_CHANNEL_DEST_NSEC     (DMA_CHANNEL_ATTR_SEC_DEST_MASK)         /*!< Channel destination is non-secure */
#endif /* __ARM_FEATURE_CMSE */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/

/** @defgroup DMA_Exported_Macros DMA Exported Macros
  * @brief    DMA Exported Macros
  * @{
  */

/**
  * @brief  Enable the specified DMA Channel.
  * @param  __HANDLE__: DMA handle.
  * @retval None
  */
#define __HAL_DMA_ENABLE(__HANDLE__)  ((__HANDLE__)->Instance->CCR |=  DMA_CCR_EN)

/**
  * @brief  Disable the specified DMA Channel.
  * @param  __HANDLE__: DMA handle.
  * @retval None
  */
#define __HAL_DMA_DISABLE(__HANDLE__)  ((__HANDLE__)->Instance->CCR |= (DMA_CCR_SUSP | DMA_CCR_RESET))

/**
  * @brief  Get the DMA Channel pending flags.
  * @param  __HANDLE__: DMA handle.
  * @param  __FLAG__: Get the specified flag.
  *          This parameter can be any combination of the following values:
  *            @arg DMA_FLAG_TC     : Transfer Complete flag.
  *            @arg DMA_FLAG_HT     : Half Transfer Complete flag.
  *            @arg DMA_FLAG_DTE    : Data Transfer Error flag.
  *            @arg DMA_FLAG_ULE    : Update Linked-list Error flag.
  *            @arg DMA_FLAG_USE    : User Setting Error flag.
  *            @arg DMA_FLAG_SUSP   : Completed Suspension flag.
  *            @arg DMA_FLAG_IDLEF  : Idle flag.
  * @retval The state of FLAG (SET or RESET).
  */
#define __HAL_DMA_GET_FLAG(__HANDLE__, __FLAG__)  ((__HANDLE__)->Instance->CSR & (__FLAG__))

/**
  * @brief  Clear the DMA Channel pending flags.
  * @param  __HANDLE__: DMA handle.
  * @param  __FLAG__: specifies the flag to clear.
  *          This parameter can be any combination of the following values:
  *            @arg DMA_FLAG_TC     : Transfer Complete flag.
  *            @arg DMA_FLAG_HT     : Half Transfer Complete flag.
  *            @arg DMA_FLAG_DTE    : Data Transfer Error flag.
  *            @arg DMA_FLAG_ULE    : Update Linked-list Error flag.
  *            @arg DMA_FLAG_USE    : User Setting Error flag.
  *            @arg DMA_FLAG_SUSP   : Completed Suspension flag.
  * @retval None
  */
#define __HAL_DMA_CLEAR_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->Instance->CFCR = (__FLAG__))

/**
  * @brief  Enable the specified DMA Channel interrupts.
  * @param  __HANDLE__: DMA handle.
  * @param __INTERRUPT__: specifies the DMA interrupt sources to be enabled.
  *          This parameter can be any combination of the following values:
  *            @arg DMA_IT_TC   :  Transfer Complete interrupt.
  *            @arg DMA_IT_HT   :  Half Transfer Complete interrupt.
  *            @arg DMA_IT_DTE  :  Data Transfer Error interrupt.
  *            @arg DMA_IT_ULE  :  Update Linked-list Error interrupt.
  *            @arg DMA_IT_USE  :  User Setting Error interrupt.
  *            @arg DMA_IT_SUSP :  Completed Suspension interrupt.
  * @retval None
  */
#define __HAL_DMA_ENABLE_IT(__HANDLE__, __INTERRUPT__)   ((__HANDLE__)->Instance->CCR |= (__INTERRUPT__))

/**
  * @brief  Disable the specified DMA Channel interrupts.
  * @param  __HANDLE__: DMA handle.
  * @param __INTERRUPT__: specifies the DMA interrupt sources to be disabled.
  *          This parameter can be any combination of the following values:
  *            @arg DMA_IT_TC   :  Transfer Complete interrupt.
  *            @arg DMA_IT_HT   :  Half Transfer Complete interrupt.
  *            @arg DMA_IT_DTE  :  Data Transfer Error interrupt.
  *            @arg DMA_IT_ULE  :  Update Linked-list Error interrupt.
  *            @arg DMA_IT_USE  :  User Setting Error interrupt.
  *            @arg DMA_IT_SUSP :  Completed Suspension interrupt.
  * @retval None
  */
#define __HAL_DMA_DISABLE_IT(__HANDLE__, __INTERRUPT__)  ((__HANDLE__)->Instance->CCR &= ~(__INTERRUPT__))

/**
  * @brief  Checks whether the specified DMA Channel interrupt is enabled or not.
  * @param  __HANDLE__: DMA handle.
  * @param  __INTERRUPT__: specifies the DMA interrupt source to check.
  *            @arg DMA_IT_TC   :  Transfer Complete interrupt.
  *            @arg DMA_IT_HT   :  Half Transfer Complete interrupt.
  *            @arg DMA_IT_DTE  :  Data Transfer Error interrupt.
  *            @arg DMA_IT_ULE  :  Update Linked-list Error interrupt.
  *            @arg DMA_IT_USE  :  User Setting Error interrupt.
  *            @arg DMA_IT_SUSP :  Completed Suspension interrupt.
  * @retval The state of DMA_IT (SET or RESET).
  */
#define __HAL_DMA_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__)  (((__HANDLE__)->Instance->CCR & (__INTERRUPT__)))

/**
  * @brief  Writes the block number of bytes to be transferred from the source on the DMA Channel.
  * @param  __HANDLE__: DMA handle.
  * @param  __COUNTER__: Number of data bytes to be transferred from the source (from 0 to 65535).
  */
#define __HAL_DMA_SET_COUNTER(__HANDLE__, __COUNTER__)  MODIFY_REG((__HANDLE__)->Instance->CBR1, DMA_CBR1_BNDT, (__COUNTER__))

/**
  * @brief  Returns the number of remaining data bytes in the current DMA Channel transfer.
  * @param  __HANDLE__: DMA handle.
  * @retval The number of remaining data units in the current DMA Stream transfer.
  */
#define __HAL_DMA_GET_COUNTER(__HANDLE__) (((__HANDLE__)->Instance->CBR1) & DMA_CBR1_BNDT)

/**
  * @brief  Return the current DMA Channel FIFO filled level.
  * @param  __HANDLE__: DMA handle.
  * @retval The number of available data in the FIFO (in units of the programmed destination data width).
  */
#define __HAL_DMA_GET_FIFOL(__HANDLE__) ((__HANDLE__)->Instance->CSR & DMA_CSR_FIFOL)

/**
  * @}
  */

/* Include DMA HAL Extension module */
#include "stm32u5xx_hal_dma_ex.h"

/* Exported functions --------------------------------------------------------*/
/** @defgroup DMA_Exported_Functions  DMA Exported Functions
  * @brief    DMA Exported Functions
  * @{
  */

/** @defgroup DMA_Exported_Functions_Group1 Initialization and de-initialization functions
  * @brief   Initialization and de-initialization functions
  * @{
  */
HAL_StatusTypeDef HAL_DMA_Init(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMA_DeInit(DMA_HandleTypeDef *hdma);
/**
  * @}
  */

/** @defgroup DMA_Exported_Functions_Group2 I/O operation functions
  * @brief   I/O operation functions
  * @{
  */
HAL_StatusTypeDef HAL_DMA_Start(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t SourceDataSize);
HAL_StatusTypeDef HAL_DMA_Start_IT(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t SourceDataSize);
HAL_StatusTypeDef HAL_DMA_Abort(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMA_Abort_IT(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMA_PollForTransfer(DMA_HandleTypeDef *hdma, HAL_DMA_LevelCompleteTypeDef CompleteLevel, uint32_t Timeout);
void HAL_DMA_IRQHandler(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMA_RegisterCallback(DMA_HandleTypeDef *hdma, HAL_DMA_CallbackIDTypeDef CallbackID, void (* pCallback)(DMA_HandleTypeDef *_hdma));
HAL_StatusTypeDef HAL_DMA_UnRegisterCallback(DMA_HandleTypeDef *hdma, HAL_DMA_CallbackIDTypeDef CallbackID);
/**
  * @}
  */

/** @defgroup DMA_Exported_Functions_Group3 State and Error functions
  * @brief    State and Error functions
  * @{
  */
HAL_DMA_StateTypeDef HAL_DMA_GetState(DMA_HandleTypeDef *hdma);
uint32_t             HAL_DMA_GetError(DMA_HandleTypeDef *hdma);
/**
  * @}
  */

/** @defgroup DMA_Exported_Functions_Group4 DMA Attributes functions
  * @brief    DMA Attributes functions
  * @{
  */
HAL_StatusTypeDef HAL_DMA_ConfigChannelAttributes(DMA_HandleTypeDef *hdma, uint32_t ChannelAttributes);
HAL_StatusTypeDef HAL_DMA_GetConfigChannelAttributes(DMA_HandleTypeDef *hdma, uint32_t *ChannelAttributes);
/**
  * @}
  */

/**
  * @}
  */

/* Private constants ---------------------------------------------------------*/
/** @defgroup DMA_Private_Constants DMA Private Constants
  * @brief    DMA Private Constants
  * @{
  */
#define HAL_TIMEOUT_DMA_ABORT     5U    /* 5 ms */
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup DMA_Private_Macros DMA Private Macros
  * @brief    DMA Private Macros
  * @{
  */
#define IS_DMA_DIRECTION(DIRECTION)    (((DIRECTION) == DMA_PERIPH_TO_MEMORY)  || \
                                        ((DIRECTION) == DMA_MEMORY_TO_PERIPH)  || \
                                        ((DIRECTION) == DMA_MEMORY_TO_MEMORY))

#define IS_DMA_LEVEL_COMPLETE(LEVEL)   (((LEVEL) == HAL_DMA_FULL_TRANSFER)     || \
                                        ((LEVEL) == HAL_DMA_HALF_TRANSFER))

#define IS_DMA_SOURCE_INC(INC)         (((INC) == DMA_SINC_FIXED)              || \
                                        ((INC) == DMA_SINC_INCREMENTED))

#define IS_DMA_DESTINATION_INC(INC)    (((INC) == DMA_DINC_FIXED)              || \
                                        ((INC) == DMA_DINC_INCREMENTED))

#define IS_DMA_SOURCE_DATA_WIDTH(WIDTH) (((WIDTH) == DMA_SRC_DATAWIDTH_BYTE)     || \
                                         ((WIDTH) == DMA_SRC_DATAWIDTH_HALFWORD) || \
                                         ((WIDTH) == DMA_SRC_DATAWIDTH_WORD))

#define IS_DMA_DESTINATION_DATA_WIDTH(WIDTH) (((WIDTH) == DMA_DEST_DATAWIDTH_BYTE)     || \
                                              ((WIDTH) == DMA_DEST_DATAWIDTH_HALFWORD) || \
                                              ((WIDTH) == DMA_DEST_DATAWIDTH_WORD))

#define IS_DMA_SOURCE_BURST_LENGTH(LENGTH)   (((LENGTH) >= 1U) && ((LENGTH) <= 64U))

#define IS_DMA_DESTINATION_BURST_LENGTH(LENGTH) (((LENGTH) >= 1U) && ((LENGTH) <= 64U))

#define IS_DMA_PRIORITY(PRIORITY)      (((PRIORITY) == DMA_LOW_PRIORITY_LOW_WEIGHT)  || \
                                        ((PRIORITY) == DMA_LOW_PRIORITY_MID_WEIGHT)  || \
                                        ((PRIORITY) == DMA_LOW_PRIORITY_HIGH_WEIGHT) || \
                                        ((PRIORITY) == DMA_HIGH_PRIORITY))

#define IS_DMA_DATA_EXCHANGE(EXCHANGE) (((EXCHANGE) & (~(DMA_EXCHANGE_SRC_BYTE | DMA_EXCHANGE_DEST_BYTE | DMA_EXCHANGE_DEST_HALFWORD))) == 0U)

#define IS_DMA_REQUEST(REQUEST)        (((REQUEST) == DMA_REQUEST_SW) || ((REQUEST) <= GPDMA_REQUEST_LPTIM3_UE))

#define IS_DMA_DATA_ALIGNMENT(ALIGNMENT) (((ALIGNMENT) == DMA_DATA_RIGHTALIGN_ZEROPADDED) || \
                                          ((ALIGNMENT) == DMA_DATA_RIGHTALIGN_SIGNEXT)    || \
                                          ((ALIGNMENT) == DMA_DATA_PACK))

#define IS_DMA_TRIGGER_MODE(MODE)      (((MODE) == DMA_TRIGM_BLOCK_TRANSFER)          || \
                                        ((MODE) == DMA_TRIGM_REPEATED_BLOCK_TRANSFER) || \
                                        ((MODE) == DMA_TRIGM_LLI_LINK_TRANSFER)       || \
                                        ((MODE) == DMA_TRIGM_SINGLE_BURST_TRANSFER))

#define IS_DMA_TRIGGER_POLARITY(POLARITY) (((POLARITY) == DMA_TRIG_POLARITY_MASKED)   || \
                                           ((POLARITY) == DMA_TRIG_POLARITY_RISING)   || \
                                           ((POLARITY) == DMA_TRIG_POLARITY_FALLING))

#define IS_DMA_TRIGGER_SELECTION(TRIGGER)  ((TRIGGER) <= GPDMA_TRIGGER_TIM15_TRGO)

#define IS_DMA_EVENT_MODE(MODE)        (((MODE) == DMA_TCEM_BLOCK_TRANSFER)          || \
                                        ((MODE) == DMA_TCEM_REPEATED_BLOCK_TRANSFER) || \
                                        ((MODE) == DMA_TCEM_EACH_LL_ITEM_TRANSFER)   || \
                                        ((MODE) == DMA_TCEM_LAST_LL_ITEM_TRANSFER))

#define IS_DMA_BLOCK_HW_REQUEST(MODE)  (((MODE) == DMA_BREQ_SINGLE_BURST)      || \
                                        ((MODE) == DMA_BREQ_BLOCK))

#define IS_DMA_BLOCK_SIZE(SIZE)        (((SIZE) > 0U) && ((SIZE) <= 65536U))

#define IS_DMA_REPEAT_COUNT(COUNT)      (((COUNT) > 0U) && ((COUNT) <= 2048U))

#define IS_DMA_BURST_ADDR_OFFSET(BURST_ADDR_OFFSET) (((BURST_ADDR_OFFSET) > (-8192)) && ((BURST_ADDR_OFFSET) < 8192))

#define IS_DMA_BLOCK_ADDR_OFFSET(BLOCK_ADDR_OFFSET) (((BLOCK_ADDR_OFFSET) > (-65536)) && ((BLOCK_ADDR_OFFSET) < 65536))

#define IS_DMA_PORT(PORT)              (((PORT) == DMA_PORT0) || ((PORT) == DMA_PORT1))

#define IS_DMA_LINK_STEP_MODE(MODE)    (((MODE) == DMA_LSM_FULL_EXECUTION)     || \
                                        ((MODE) == DMA_LSM_1LINK_EXECUTION))

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
#define IS_DMA_ATTRIBUTES(ATTRIBUTE)    (((((~((ATTRIBUTE) & 0xFFFFFFF0U)) >> 4U) & ((ATTRIBUTE) & 0xFFFFFF0FU)) == 0U) && ((ATTRIBUTE) != 0U))
#else
#define IS_DMA_ATTRIBUTES(ATTRIBUTE)    (((ATTRIBUTE) == DMA_CHANNEL_PRIV)     || \
                                         ((ATTRIBUTE) == DMA_CHANNEL_NPRIV))
#endif /* __ARM_FEATURE_CMSE */
/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/
/** @defgroup DMA_Private_Functions DMA Private Functions
  * @brief    DMA Private Functions
  * @{
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32U5xx_HAL_DMA_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
