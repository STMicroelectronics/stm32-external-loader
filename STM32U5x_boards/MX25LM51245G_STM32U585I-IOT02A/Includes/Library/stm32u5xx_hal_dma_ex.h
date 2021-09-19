/**
  ******************************************************************************
  * @file    stm32u5xx_hal_dma_ex.h
  * @author  MCD Application Team
  * @brief   Header file of DMA HAL extension module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2020 STMicroelectronics.
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
#ifndef STM32U5xx_HAL_DMA_EX_H
#define STM32U5xx_HAL_DMA_EX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal_def.h"

/** @addtogroup STM32U5xx_HAL_Driver
  * @{
  */

/** @addtogroup DMAEx
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup DMAEx_Exported_Types DMAEx Exported Types
  * @brief DMAEx Exported types
  * @{
  */

/**
  * @brief DMA data handling configuration structure definition.
  */
typedef struct
{
  uint32_t DataExchange;  /*!< Specifies the data exchange mode.
                               This parameter can be a value of @ref DMA_Data_Exchange                  */

  uint32_t DataAlignment; /*!< Specifies the source to destination Memory data padding / alignment mode.
                               This parameter can be a value of @ref DMA_Data_Alignment                 */

} DMA_DataHandlingConfTypeDef;

/**
  * @brief DMA trigger configuration structure definition.
  */
typedef struct
{
  uint32_t TriggerMode;      /*!< Specifies the trigger mode.
                                  This parameter can be a value of @ref DMA_Trigger_Mode      */

  uint32_t TriggerPolarity;  /*!< Specifies the DMA trigger event polarity.
                                  This parameter can be a value of @ref DMA_Trigger_Polarity  */

  uint32_t TriggerSelection; /*!< Specifies the DMA trigger event selection.
                                  This parameter can be a value of @ref DMA_Trigger_Selection */

} DMA_TriggerConfTypeDef;

/**
  * @brief DMA repeated block configuration structure definition.
  */
typedef struct
{
  uint32_t RepeatCount;      /*!< Specifies the block repeat count : the number of repetitions
                                  of the current block.
                                  This parameter can be a value between 1 and 2048                             */

  int32_t SrcAddrOffset;     /*!< This field specifies the source address offset signed value :
                                  * If > 0   then  increment the source address by offset from
                                    where the last single/burst transfer ends
                                  * If < 0   then  decrement the source address by offset from
                                    where the last single/burst transfer ends
                                  * If == 0, the next single/burst source address starts from
                                    where the last transfer ends                                               */

  int32_t DestAddrOffset;    /*!< This field specifies the destination address offset signed value :
                                  * If > 0   then  increment the destination address by offset from
                                    where the last single/burst transfer ends
                                  * If < 0   then  decrement the destination address by offset from
                                    where the last single/burst transfer ends
                                  * If == 0, the next single/burst destination address starts from
                                    where the last transfer ends                                               */

  int32_t BlkSrcAddrOffset;  /*!< This field specifies the Next block source address offset signed value :
                                  * If > 0   then  increment the next block source address by offset from
                                    where the last block ends
                                  * If < 0   then  decrement the next block source address by offset from
                                    where the last block ends
                                  * If == 0, the next block source address starts from where the last
                                    block ends                                                                 */

  int32_t BlkDestAddrOffset; /*!< This field specifies the Next block destination address offset signed value :
                                  * If > 0   then  increment the next block destination address by offset from
                                    where the last block ends
                                  * If < 0   then  decrement the next block destination address by offset from
                                    where the last block ends
                                  * If == 0, the next block destination address starts from where the last
                                    block ends                                                                 */

} DMA_RepeatBlockConfTypeDef;

/**
  * @brief DMA queue state structure definition.
  */
typedef enum
{
  HAL_DMA_QUEUE_STATE_RESET = 0x00U, /*!< DMA queue empty              */
  HAL_DMA_QUEUE_STATE_READY = 0x01U, /*!< DMA queue ready for use      */
  HAL_DMA_QUEUE_STATE_BUSY  = 0x02U  /*!< DMA queue execution on going */

} HAL_DMA_QStateTypeDef;
/**
  * @}
  */

/**
  * @brief  DMA linked-list node configuration structure definition.
  */
typedef struct
{
  uint32_t                    NodeType;           /*!< Specifies the DMA node type.
                                                       This parameter can be a value of @ref DMA_Node_Type */

  DMA_InitTypeDef             Init;               /*!< Specifies the basic channel configuration           */

  DMA_DataHandlingConfTypeDef DataHandlingConfig; /*!< Specifies the Data handling channel configuration   */

  DMA_TriggerConfTypeDef      TriggerConfig;      /*!< Specifies the Trigger Channel Configuration         */

  DMA_RepeatBlockConfTypeDef  RepeatBlockConfig;  /*!< Specifies the Repeated Block Channel Configuration  */

  uint32_t                    SrcAddress;         /*!< Specifies the source memory address                 */
  uint32_t                    DstAddress;         /*!< Specifies the destination memory address            */
  uint32_t                    DataSize;           /*!< Specifies the source data size in bytes             */

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
  uint32_t                    SrcSecure;          /*!< Specifies the source security attribute             */
  uint32_t                    DestSecure;         /*!< Specifies the destination security attribute        */
#endif /* defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U) */

} DMA_NodeConfTypeDef;

/**
  * @brief  DMA linked-list node structure definition.
  */
typedef struct
{
  uint32_t LinkRegisters[8U]; /*!< Physical Node register description */
  uint32_t NodeInfo;          /*!< Node information                   */

} DMA_NodeTypeDef;

/**
  * @brief DMA linked-list queue structure definition.
  */
typedef struct __DMA_QListTypeDef
{
  DMA_NodeTypeDef                *Head;              /*!< Specifies the queue head node                   */

  DMA_NodeTypeDef                *FirstCircularNode; /*!< Specifies the queue first circular node         */

  uint32_t                       NodeNumber;         /*!< Specifies the queue node number                 */

  __IO HAL_DMA_QStateTypeDef State;                  /*!< Specifies the queue state                       */

  __IO uint32_t                  ErrorCode;          /*!< Specifies the queue error code                  */

  __IO uint32_t                  Type;               /*!< Specifies wether the queue is static or dynamic */

}DMA_QListTypeDef;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup DMAEx_Exported_Constants DMA Exported Constants
  * @brief    DMAEx Exported Constants
  * @{
  */

/** @defgroup Queue_Error_Codes Queue error codes
  * @brief    Queue error codes
  * @{
  */
#define HAL_DMA_QUEUE_ERROR_NONE        0x00U /*!< No error                                                */
#define HAL_DMA_QUEUE_ERROR_BUSY        0x01U /*!< Error busy                                              */
#define HAL_DMA_QUEUE_ERROR_EMPTY       0x02U /*!< Error unallowed operation for empty queue               */
#define HAL_DMA_QUEUE_ERROR_UNSUPPORTED 0x03U /*!< Error unsupported feature                               */
#define HAL_DMA_QUEUE_ERROR_INVALIDTYPE 0x04U /*!< Error incompatible node type or circular intialization
                                                   and queue circular types are incompatible               */
#define HAL_DMA_QUEUE_ERROR_OUTOFRANGE  0x05U /*!< Error incompatible node type                            */
#define HAL_DMA_QUEUE_ERROR_NOTFOUND    0x06U /*!< Error node not found in queue                           */
/**
  * @}
  */

/** @defgroup DMA_LinkedList_Mode DMA linked-list mode
  * @brief    DMA linked-list mode
  * @{
  */
#define DMA_LINKEDLIST_NORMAL   DMA_LINKEDLIST             /*!< Normal linked-list DMA transfer   */
#define DMA_LINKEDLIST_CIRCULAR (DMA_LINKEDLIST | (0x01U)) /*!< Circular linked-list DMA transfer */
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
/* GPDMA1 triggers */
#define GPDMA1_TRIGGER_EXTI_LINE0        0U       /*!< GPDMA1 HW Trigger signal is EXTI_LINE0      */
#define GPDMA1_TRIGGER_EXTI_LINE1        1U       /*!< GPDMA1 HW Trigger signal is EXTI_LINE1      */
#define GPDMA1_TRIGGER_EXTI_LINE2        2U       /*!< GPDMA1 HW Trigger signal is EXTI_LINE2      */
#define GPDMA1_TRIGGER_EXTI_LINE3        3U       /*!< GPDMA1 HW Trigger signal is EXTI_LINE3      */
#define GPDMA1_TRIGGER_EXTI_LINE4        4U       /*!< GPDMA1 HW Trigger signal is EXTI_LINE4      */
#define GPDMA1_TRIGGER_EXTI_LINE5        5U       /*!< GPDMA1 HW Trigger signal is EXTI_LINE5      */
#define GPDMA1_TRIGGER_EXTI_LINE6        6U       /*!< GPDMA1 HW Trigger signal is EXTI_LINE6      */
#define GPDMA1_TRIGGER_EXTI_LINE7        7U       /*!< GPDMA1 HW Trigger signal is EXTI_LINE7      */
#define GPDMA1_TRIGGER_TAMP_TRG1         8U       /*!< GPDMA1 HW Trigger signal is TAMP_TRG1       */
#define GPDMA1_TRIGGER_TAMP_TRG2         9U       /*!< GPDMA1 HW Trigger signal is TAMP_TRG2       */
#define GPDMA1_TRIGGER_TAMP_TRG3         10U      /*!< GPDMA1 HW Trigger signal is TAMP_TRG3       */
#define GPDMA1_TRIGGER_LPTIM1_CH1        11U      /*!< GPDMA1 HW Trigger signal is LPTIM1_CH1      */
#define GPDMA1_TRIGGER_LPTIM1_CH2        12U      /*!< GPDMA1 HW Trigger signal is LPTIM1_CH2      */
#define GPDMA1_TRIGGER_LPTIM2_CH1        13U      /*!< GPDMA1 HW Trigger signal is LPTIM2_CH1      */
#define GPDMA1_TRIGGER_LPTIM2_CH2        14U      /*!< GPDMA1 HW Trigger signal is LPTIM2_CH2      */
#define GPDMA1_TRIGGER_LPTIM4_OUT        15U      /*!< GPDMA1 HW Trigger signal is LPTIM4_OUT      */
#define GPDMA1_TRIGGER_COMP1_OUT         16U      /*!< GPDMA1 HW Trigger signal is COMP1_OUT       */
#define GPDMA1_TRIGGER_COMP2_OUT         17U      /*!< GPDMA1 HW Trigger signal is COMP2_OUT       */
#define GPDMA1_TRIGGER_RTC_ALRA_TRG      18U      /*!< GPDMA1 HW Trigger signal is RTC_ALRA_TRG    */
#define GPDMA1_TRIGGER_RTC_ALRB_TRG      19U      /*!< GPDMA1 HW Trigger signal is RTC_ALRB_TRG    */
#define GPDMA1_TRIGGER_RTC_WUT_TRG       20U      /*!< GPDMA1 HW Trigger signal is RTC_WUT_TRG     */
#define GPDMA1_TRIGGER_GPDMA1_CH0_TCF    22U      /*!< GPDMA1 HW Trigger signal is GPDMA1_CH0_TCF  */
#define GPDMA1_TRIGGER_GPDMA1_CH1_TCF    23U      /*!< GPDMA1 HW Trigger signal is GPDMA1_CH1_TCF  */
#define GPDMA1_TRIGGER_GPDMA1_CH2_TCF    24U      /*!< GPDMA1 HW Trigger signal is GPDMA1_CH2_TCF  */
#define GPDMA1_TRIGGER_GPDMA1_CH3_TCF    25U      /*!< GPDMA1 HW Trigger signal is GPDMA1_CH3_TCF  */
#define GPDMA1_TRIGGER_GPDMA1_CH4_TCF    26U      /*!< GPDMA1 HW Trigger signal is GPDMA1_CH4_TCF  */
#define GPDMA1_TRIGGER_GPDMA1_CH5_TCF    27U      /*!< GPDMA1 HW Trigger signal is GPDMA1_CH5_TCF  */
#define GPDMA1_TRIGGER_GPDMA1_CH6_TCF    28U      /*!< GPDMA1 HW Trigger signal is GPDMA1_CH6_TCF  */
#define GPDMA1_TRIGGER_GPDMA1_CH7_TCF    29U      /*!< GPDMA1 HW Trigger signal is GPDMA1_CH7_TCF  */
#define GPDMA1_TRIGGER_GPDMA1_CH8_TCF    30U      /*!< GPDMA1 HW Trigger signal is GPDMA1_CH8_TCF  */
#define GPDMA1_TRIGGER_GPDMA1_CH9_TCF    31U      /*!< GPDMA1 HW Trigger signal is GPDMA1_CH9_TCF  */
#define GPDMA1_TRIGGER_GPDMA1_CH10_TCF   32U      /*!< GPDMA1 HW Trigger signal is GPDMA1_CH10_TCF */
#define GPDMA1_TRIGGER_GPDMA1_CH11_TCF   33U      /*!< GPDMA1 HW Trigger signal is GPDMA1_CH11_TCF */
#define GPDMA1_TRIGGER_GPDMA1_CH12_TCF   34U      /*!< GPDMA1 HW Trigger signal is GPDMA1_CH12_TCF */
#define GPDMA1_TRIGGER_GPDMA1_CH13_TCF   35U      /*!< GPDMA1 HW Trigger signal is GPDMA1_CH13_TCF */
#define GPDMA1_TRIGGER_GPDMA1_CH14_TCF   36U      /*!< GPDMA1 HW Trigger signal is GPDMA1_CH14_TCF */
#define GPDMA1_TRIGGER_GPDMA1_CH15_TCF   37U      /*!< GPDMA1 HW Trigger signal is GPDMA1_CH15_TCF */
#define GPDMA1_TRIGGER_LPDMA1_CH0_TCF    38U      /*!< GPDMA1 HW Trigger signal is LPDMA1_CH0_TCF  */
#define GPDMA1_TRIGGER_LPDMA1_CH1_TCF    39U      /*!< GPDMA1 HW Trigger signal is LPDMA1_CH1_TCF  */
#define GPDMA1_TRIGGER_LPDMA1_CH2_TCF    40U      /*!< GPDMA1 HW Trigger signal is LPDMA1_CH2_TCF  */
#define GPDMA1_TRIGGER_LPDMA1_CH3_TCF    41U      /*!< GPDMA1 HW Trigger signal is LPDMA1_CH3_TCF  */
#define GPDMA1_TRIGGER_TIM2_TRGO         42U      /*!< GPDMA1 HW Trigger signal is TIM2_TRGO       */
#define GPDMA1_TRIGGER_TIM15_TRGO        43U      /*!< GPDMA1 HW Trigger signal is TIM15_TRGO      */
#define GPDMA1_TRIGGER_ADC4_AWD1         57U      /*!< GPDMA1 HW Trigger signal is ADC4_AWD1       */
#define GPDMA1_TRIGGER_ADC1_AWD1         58U      /*!< GPDMA1 HW Trigger signal is ADC1_AWD1       */

/* LPDMA1 triggers */
#define LPDMA1_TRIGGER_EXTI_LINE0         0U      /*!< LPDMA1 HW Trigger signal is EXTI_LINE0      */
#define LPDMA1_TRIGGER_EXTI_LINE1         1U      /*!< LPDMA1 HW Trigger signal is EXTI_LINE1      */
#define LPDMA1_TRIGGER_EXTI_LINE2         2U      /*!< LPDMA1 HW Trigger signal is EXTI_LINE2      */
#define LPDMA1_TRIGGER_EXTI_LINE3         3U      /*!< LPDMA1 HW Trigger signal is EXTI_LINE3      */
#define LPDMA1_TRIGGER_EXTI_LINE4         4U      /*!< LPDMA1 HW Trigger signal is EXTI_LINE4      */
#define LPDMA1_TRIGGER_TAMP_TRG1          5U      /*!< LPDMA1 HW Trigger signal is TAMP_TRG1       */
#define LPDMA1_TRIGGER_TAMP_TRG2          6U      /*!< LPDMA1 HW Trigger signal is TAMP_TRG2       */
#define LPDMA1_TRIGGER_TAMP_TRG3          7U      /*!< LPDMA1 HW Trigger signal is TAMP_TRG3       */
#define LPDMA1_TRIGGER_LPTIM1_CH1         8U      /*!< LPDMA1 HW Trigger signal is LPTIM1_CH1      */
#define LPDMA1_TRIGGER_LPTIM1_CH2         9U      /*!< LPDMA1 HW Trigger signal is LPTIM1_CH2      */
#define LPDMA1_TRIGGER_LPTIM3_CH1         10U     /*!< LPDMA1 HW Trigger signal is LPTIM3_CH1      */
#define LPDMA1_TRIGGER_LPTIM4_OUT         11U     /*!< LPDMA1 HW Trigger signal is LPTIM4_OUT      */
#define LPDMA1_TRIGGER_COMP1_OUT          12U     /*!< LPDMA1 HW Trigger signal is COMP1_OUT       */
#define LPDMA1_TRIGGER_COMP2_OUT          13U     /*!< LPDMA1 HW Trigger signal is COMP2_OUT       */
#define LPDMA1_TRIGGER_RTC_ALRA_TRG       14U     /*!< LPDMA1 HW Trigger signal is RTC_ALRA_TRG    */
#define LPDMA1_TRIGGER_RTC_ALRB_TRG       15U     /*!< LPDMA1 HW Trigger signal is RTC_ALRB_TRG    */
#define LPDMA1_TRIGGER_RTC_WUT_TRG        16U     /*!< LPDMA1 HW Trigger signal is RTC_WUT_TRG     */
#define LPDMA1_TRIGGER_ADC4_AWD1          17U     /*!< LPDMA1 HW Trigger signal is ADC4_AWD1       */
#define LPDMA1_TRIGGER_LPDMA1_CH0_TCF     18U     /*!< LPDMA1 HW Trigger signal is LPDMA1_CH0_TCF  */
#define LPDMA1_TRIGGER_LPDMA1_CH1_TCF     19U     /*!< LPDMA1 HW Trigger signal is LPDMA1_CH1_TCF  */
#define LPDMA1_TRIGGER_LPDMA1_CH2_TCF     20U     /*!< LPDMA1 HW Trigger signal is LPDMA1_CH2_TCF  */
#define LPDMA1_TRIGGER_LPDMA1_CH3_TCF     21U     /*!< LPDMA1 HW Trigger signal is LPDMA1_CH3_TCF  */
#define LPDMA1_TRIGGER_GPDMA1_CH0_TCF     22U     /*!< LPDMA1 HW Trigger signal is GPDMA1_CH0_TCF  */
#define LPDMA1_TRIGGER_GPDMA1_CH1_TCF     23U     /*!< LPDMA1 HW Trigger signal is GPDMA1_CH1_TCF  */
#define LPDMA1_TRIGGER_GPDMA1_CH4_TCF     24U     /*!< LPDMA1 HW Trigger signal is GPDMA1_CH4_TCF  */
#define LPDMA1_TRIGGER_GPDMA1_CH5_TCF     25U     /*!< LPDMA1 HW Trigger signal is GPDMA1_CH5_TCF  */
#define LPDMA1_TRIGGER_GPDMA1_CH6_TCF     26U     /*!< LPDMA1 HW Trigger signal is GPDMA1_CH6_TCF  */
#define LPDMA1_TRIGGER_GPDMA1_CH7_TCF     27U     /*!< LPDMA1 HW Trigger signal is GPDMA1_CH7_TCF  */
#define LPDMA1_TRIGGER_GPDMA1_CH12_TCF    28U     /*!< LPDMA1 HW Trigger signal is GPDMA1_CH12_TCF */
#define LPDMA1_TRIGGER_GPDMA1_CH13_TCF    29U     /*!< LPDMA1 HW Trigger signal is GPDMA1_CH13_TCF */
#define LPDMA1_TRIGGER_TIM2_TRGO          30U     /*!< LPDMA1 HW Trigger signal is TIM2_TRGO       */
#define LPDMA1_TRIGGER_TIM15_TRGO         31U     /*!< LPDMA1 HW Trigger signal is TIM15_TRGO      */
/**
  * @}
  */

/** @defgroup DMA_Node_Type DMA Node Type
  * @brief    DMA Node Type
  * @{
  */
#define DMA_LPDMA_LINEAR_NODE (DMA_CHANNEL_TYPE_LPDMA | DMA_CHANNEL_TYPE_LINEAR_ADDR) /*!< Defines the LPDMA linear addressing node type      */
#define DMA_GPDMA_LINEAR_NODE (DMA_CHANNEL_TYPE_GPDMA | DMA_CHANNEL_TYPE_LINEAR_ADDR) /*!< Defines the GPDMA linear addressing node type      */
#define DMA_GPDMA_2D_NODE     (DMA_CHANNEL_TYPE_GPDMA | DMA_CHANNEL_TYPE_2D_ADDR)     /*!< Defines the GPDMA 2 dimension addressing node type */
/**
  * @}
  */

/** @defgroup DMA_Link_Allocated_Port DMA Link Allocated Port
  * @brief    DMA Link Allocated Port
  * @{
  */
#define DMA_LINK_ALLOCATED_PORT0  0x00000000U /*!< Link allocated Port 0      */
#define DMA_LINK_ALLOCATED_PORT1  DMA_CCR_LAP /*!< Link allocated Port 1      */
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

/**
  * @}
  */


/* Exported functions --------------------------------------------------------*/

/** @defgroup DMAEx_Exported_Functions DMAEx Exported Functions
  * @brief    DMAEx Exported functions
  * @{
  */

/** @defgroup DMA_Exported_Functions_Group1 Linked-list initialization and de-initialization functions
  * @brief    Linked-list initialization and de-initialization functions
  * @{
  */
HAL_StatusTypeDef HAL_DMAEx_List_Init    (DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMAEx_List_DeInit  (DMA_HandleTypeDef *hdma);
/**
  * @}
  */

/** @defgroup DMA_Exported_Functions_Group2 Linked-list I/O operation functions
  * @brief    Linked-list I/O operation functions
  * @{
  */
HAL_StatusTypeDef HAL_DMAEx_List_Start   (DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMAEx_List_Start_IT(DMA_HandleTypeDef *hdma);
/**
  * @}
  */

/** @defgroup DMA_Exported_Functions_Group3 Linked-List management functions
  * @brief    Linked-List management functions
  * @{
  */
HAL_StatusTypeDef HAL_DMAEx_List_ResetQ               (DMA_QListTypeDef *pQueueLinkList);
HAL_StatusTypeDef HAL_DMAEx_List_BuildNode            (DMA_NodeConfTypeDef *pNodeConfig, DMA_NodeTypeDef *pNode);
HAL_StatusTypeDef HAL_DMAEx_List_GetNodeConfig        (DMA_NodeConfTypeDef *pNodeConfig, DMA_NodeTypeDef *pNode);
HAL_StatusTypeDef HAL_DMAEx_List_InsertNode           (DMA_QListTypeDef *pQueueLinkList, DMA_NodeTypeDef *pPrevNode, DMA_NodeTypeDef *pNewNode);
HAL_StatusTypeDef HAL_DMAEx_List_InsertNode_Head      (DMA_QListTypeDef *pQueueLinkList, DMA_NodeTypeDef *pNewNode);
HAL_StatusTypeDef HAL_DMAEx_List_InsertNode_Tail      (DMA_QListTypeDef *pQueueLinkList, DMA_NodeTypeDef *pNewNode);
HAL_StatusTypeDef HAL_DMAEx_List_RemoveNode           (DMA_QListTypeDef *pQueueLinkList, DMA_NodeTypeDef *pNode);
HAL_StatusTypeDef HAL_DMAEx_List_RemoveNode_Head      (DMA_QListTypeDef *pQueueLinkList);
HAL_StatusTypeDef HAL_DMAEx_List_RemoveNode_Tail      (DMA_QListTypeDef *pQueueLinkList);
HAL_StatusTypeDef HAL_DMAEx_List_ReplaceNode          (DMA_QListTypeDef *pQueueLinkList, DMA_NodeTypeDef *pOldNode, DMA_NodeTypeDef *pNewNode);
HAL_StatusTypeDef HAL_DMAEx_List_ReplaceNode_Head     (DMA_QListTypeDef *pQueueLinkList, DMA_NodeTypeDef *pNewNode);
HAL_StatusTypeDef HAL_DMAEx_List_ReplaceNode_Tail     (DMA_QListTypeDef *pQueueLinkList, DMA_NodeTypeDef *pNewNode);
HAL_StatusTypeDef HAL_DMAEx_List_InsertQ              (DMA_QListTypeDef *pLinkListSrcQueue, DMA_NodeTypeDef *pPrevNode, DMA_QListTypeDef *pLinkListDestQueue);
HAL_StatusTypeDef HAL_DMAEx_List_InsertQ_Head         (DMA_QListTypeDef *pLinkListSrcQueue, DMA_QListTypeDef *pLinkListDestQueue);
HAL_StatusTypeDef HAL_DMAEx_List_InsertQ_Tail         (DMA_QListTypeDef *pLinkListSrcQueue, DMA_QListTypeDef *pLinkListDestQueue);
HAL_StatusTypeDef HAL_DMAEx_List_SetCircularModeConfig(DMA_QListTypeDef *pQueueLinkList, DMA_NodeTypeDef *pFirstCircularNode);
HAL_StatusTypeDef HAL_DMAEx_List_SetCircularMode      (DMA_QListTypeDef *pQueueLinkList);
HAL_StatusTypeDef HAL_DMAEx_List_ClearCircularMode    (DMA_QListTypeDef *pQueueLinkList);
HAL_StatusTypeDef HAL_DMAEx_List_ConvertQToDynamic    (DMA_QListTypeDef *pQueueLinkList);
HAL_StatusTypeDef HAL_DMAEx_List_ConvertQToStatic     (DMA_QListTypeDef *pQueueLinkList);
HAL_StatusTypeDef HAL_DMAEx_List_LinkQ                (DMA_HandleTypeDef *hdma, DMA_QListTypeDef *pQueueLinkList);
HAL_StatusTypeDef HAL_DMAEx_List_UnLinkQ              (DMA_HandleTypeDef *hdma);
/**
  * @}
  */

/** @defgroup DMA_Exported_Functions_Group4 Data handling, repeated block and trigger configuration functions
  * @brief    Data handling, repeated block and trigger configuration functions
  * @{
  */
HAL_StatusTypeDef HAL_DMAEx_ConfigDataHandling(DMA_HandleTypeDef *hdma, DMA_DataHandlingConfTypeDef *sConfigDataHandling);
HAL_StatusTypeDef HAL_DMAEx_ConfigTrigger     (DMA_HandleTypeDef *hdma, DMA_TriggerConfTypeDef *sConfigTrigger);
HAL_StatusTypeDef HAL_DMAEx_ConfigRepeatBlock (DMA_HandleTypeDef *hdma, DMA_RepeatBlockConfTypeDef *sConfigRepeatBlock);
/**
  * @}
  */

/** @defgroup DMA_Exported_Functions_Group5 Suspend and resume operation functions
  * @brief    Suspend and resume operation functions
  * @{
  */
HAL_StatusTypeDef HAL_DMAEx_Suspend   (DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMAEx_Suspend_IT(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMAEx_Resume    (DMA_HandleTypeDef *hdma);
/**
  * @}
  */

/** @defgroup DMA_Exported_Functions_Group6 Fifo status function
  * @brief    Fifo status function
  * @{
  */
uint32_t HAL_DMAEx_GetFifoLevel(DMA_HandleTypeDef *hdma);
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
#define DMA_LINKEDLIST               (0x80U)

#define DMA_CHANNEL_TYPE_LINEAR_ADDR (0x01U)
#define DMA_CHANNEL_TYPE_2D_ADDR     (0x02U)
#define DMA_CHANNEL_TYPE_LPDMA       (0x10U)
#define DMA_CHANNEL_TYPE_GPDMA       (0x20U)

#define NODE_TYPE_MASK   (0x000000FFUL)
#define NODE_CLLR_IDX    (0x00000700UL)
#define NODE_CLLR_IDX_Pos (0x08U)

#define QUEUE_TYPE_STATIC                    (0x00U)
#define QUEUE_TYPE_DYNAMIC                   (0x01U)


#define NODE_CTR1_DEFAULT_OFFSET        (0x00U) /*!< No error                                                                                         */
#define NODE_CTR2_DEFAULT_OFFSET        (0x01U) /*!< Error busy                                                                                       */
#define NODE_CBR1_DEFAULT_OFFSET        (0x02U) /*!< Error out of range node address                                                                  */
#define NODE_CSAR_DEFAULT_OFFSET        (0x03U) /*!< Error 2D addressing is not supported                                                             */
#define NODE_CDAR_DEFAULT_OFFSET        (0x04U) /*!< Error insert source circular queue at the first or the middle of destination queue               */
#define NODE_CTR3_DEFAULT_OFFSET        (0x05U) /*!< Error incompatible node type or circular intialization and queue circular types are incompatible */
#define NODE_CBR2_DEFAULT_OFFSET        (0x06U) /*!< Error incompatible node type                                                                     */
#define NODE_CLLR_2D_DEFAULT_OFFSET     (0x07U) /*!< Error node not found in queue                                                                    */
#define NODE_CLLR_LINEAR_DEFAULT_OFFSET (0x05U) /*!< Error node not found in queue                                                                    */
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/

/** @defgroup DMAEx_Private_Macros DMA Private Macros
  * @brief    DMAEx private macros
  * @{
  */
#define IS_DMA_DATA_ALIGNMENT(ALIGNMENT) (((ALIGNMENT) == DMA_DATA_RIGHTALIGN_ZEROPADDED) || \
                                          ((ALIGNMENT) == DMA_DATA_RIGHTALIGN_SIGNEXT)    || \
                                          ((ALIGNMENT) == DMA_DATA_PACK))

#define IS_DMA_DATA_EXCHANGE(EXCHANGE) (((EXCHANGE) & (~(DMA_EXCHANGE_SRC_BYTE | DMA_EXCHANGE_DEST_BYTE | DMA_EXCHANGE_DEST_HALFWORD))) == 0U)

#define IS_DMA_REPEAT_COUNT(COUNT)      (((COUNT) > 0U) && ((COUNT) <= 2048U))

#define IS_DMA_BURST_ADDR_OFFSET(BURST_ADDR_OFFSET) (((BURST_ADDR_OFFSET) > (-8192)) && ((BURST_ADDR_OFFSET) < 8192))

#define IS_DMA_BLOCK_ADDR_OFFSET(BLOCK_ADDR_OFFSET) (((BLOCK_ADDR_OFFSET) > (-65536)) && ((BLOCK_ADDR_OFFSET) < 65536))

#define IS_DMA_LINK_ALLOCATED_PORT(LINK_ALLOCATED_PORT)     (((LINK_ALLOCATED_PORT) & (~(DMA_CCR_LAP))) == 0U)

#define IS_DMA_LINK_STEP_MODE(MODE)    (((MODE) == DMA_LSM_FULL_EXECUTION)     || \
                                        ((MODE) == DMA_LSM_1LINK_EXECUTION))

#define IS_DMA_TRIGGER_MODE(MODE)           (((MODE) == DMA_TRIGM_BLOCK_TRANSFER)          || \
                                             ((MODE) == DMA_TRIGM_REPEATED_BLOCK_TRANSFER) || \
                                             ((MODE) == DMA_TRIGM_LLI_LINK_TRANSFER)       || \
                                             ((MODE) == DMA_TRIGM_SINGLE_BURST_TRANSFER))

#define IS_DMA_TCEM_LINKEDLIST_EVENT_MODE(MODE) (((MODE) == DMA_TCEM_BLOCK_TRANSFER)          || \
                                                 ((MODE) == DMA_TCEM_REPEATED_BLOCK_TRANSFER) || \
                                                 ((MODE) == DMA_TCEM_EACH_LL_ITEM_TRANSFER)   || \
                                                 ((MODE) == DMA_TCEM_LAST_LL_ITEM_TRANSFER))

#define IS_DMA_LINKEDLIST_MODE(MODE) (((MODE) == DMA_LINKEDLIST_NORMAL) || \
                                      ((MODE) == DMA_LINKEDLIST_CIRCULAR))

#define IS_DMA_TRIGGER_POLARITY(POLARITY)   (((POLARITY) == DMA_TRIG_POLARITY_MASKED)   || \
                                             ((POLARITY) == DMA_TRIG_POLARITY_RISING)   || \
                                             ((POLARITY) == DMA_TRIG_POLARITY_FALLING))

#define IS_DMA_TRIGGER_SELECTION(TRIGGER)   ((TRIGGER) <= GPDMA1_TRIGGER_ADC1_AWD1)

#define IS_DMA_NODE_TYPE(TYPE) (((TYPE) == DMA_LPDMA_LINEAR_NODE) || \
                                ((TYPE) == DMA_GPDMA_LINEAR_NODE) || \
                                ((TYPE) == DMA_GPDMA_2D_NODE))


/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/
/** @defgroup DMAEx_Private_Functions DMAEx Private Functions
  * @brief    DMAEx Private functions
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
#endif /* __cplusplus */

#endif /* STM32U5xx_HAL_DMA_EX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
