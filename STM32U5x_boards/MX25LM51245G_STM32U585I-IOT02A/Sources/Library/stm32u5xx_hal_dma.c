/**
  ******************************************************************************
  * @file    stm32u5xx_hal_dma.c
  * @author  MCD Application Team
  * @brief  This file provides firmware functions to manage the following
  *         functionalities of the Master Direct Memory Access (DMA) peripheral:
  *           + Initialization/de-initialization functions
  *           + I/O operation functions
  *           + State and errors functions
  @verbatim
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================
  [..]
   (#) Enable and configure the peripheral to be connected to the DMA Channel
       (except for internal SRAM/FLASH memories: no initialization is
       necessary) please refer to Reference manual for connection between peripherals
       and DMA requests.

   (#)
       For a given Channel use HAL_DMA_Init function to program the required configuration through the following parameters:
       transfer request, channel priority, data exchange, Source increment, destination increment,
       source data width, destination data width, data alignment, source Burst length, destination Burst length,
       repeated block count, transfer trigger mode (single/burst transfer, block transfer, repeated block transfer
       or linked-list item transfer), source and destination burst address offset, source and destination block address offset.

       If using the DMA in linked-list mode, then use function HAL_DMAEx_LinkedList_CreateNode to fill a transfer node.
       Note that parameters given to the function HAL_DMA_Init corresponds always to the node zero (located in DMA registers).
       Use function HAL_DMAEx_LinkedList_AddNode to connect the created node to the linked-list at a given position.
       User can make a linked-list circular using function HAL_DMAEx_LinkedList_EnableCircularMode , this function will automatically connect the
       last node of the list to the first one in order to make the list circular.
       In this case the linked-list will loop on node 1 : first node connected after the initial transfer defined by the HAL_DMA_Init().

      -@-   The initial transfer itself (node 0 corresponding to the Init).
            User can disable the circular mode using function HAL_DMA_LinkedList_DisableCircularMode, this function will then remove
            the connection between last node and first one.

       Function HAL_DMAEx_LinkedList_RemoveNode can be used to remove (disconnect) a node from the transfer linked-list.
       When a linked-list is circular (last node connected to first one), if node1 (node where the linked-list loops) is removed,
       the linked-list remains circular and node 2 becomes the first one.
       Note that if the linked-list is made circular, the transfer will loop infinitely (or until aborted by the user).

    [..]
       (+) User can select the transfer trigger mode (parameter TriggerMode) to define the amount of data to be
           transfered upon a trigger hit :
             (++) DMA_TRIGM_SINGLE_BURST_TRANSFER : each hit triggers a single/burst transfer
             (++) DMA_TRIGM_BLOCK_TRANSFER : each hit triggers a block transfer
             (++) DMA_TRIGM_REPEATED_BLOCK_TRANSFER : each hit triggers a repeated block transfer
             (++) DMA_TRIGM_LL_ITEM_TRANSFER : each hit triggers a linked-list item transfer

     *** Polling mode IO operation ***
     =================================
    [..]
          (+) Use HAL_DMA_Start() to start a DMA transfer after the configuration of Source
              address and destination address and the Size of data to be transferred.
          (+) Use HAL_DMA_PollForTransfer() to poll for the end of current transfer or a transfer level
              In this case a fixed Timeout can be configured by User depending on his application.
          (+) Use HAL_DMA_Abort() function to abort the current transfer : blocking method, this API returns
              when the abort ends or generates a timeout (this API should not be called from an interrupt service routine).

     *** Interrupt mode IO operation ***
     ===================================
    [..]
          (+) Configure the DMA interrupt priority using HAL_NVIC_SetPriority()
          (+) Enable the DMA IRQ handler using HAL_NVIC_EnableIRQ()
          (+) Use HAL_DMA_RegisterCallback() function to register user callbacks
              from the following list :
              (++) XferCpltCallback            : transfer complete callback.
              (++) XferHalfCpltCallback        : half transfer complete callback.
              (++) XferErrorCallback           : transfer error callback.
              (++) XferAbortCallback           : transfer abort complete callback.
              (++) XferSuspendCallback         : transfer suspend complete callback.
          (+) Use HAL_DMA_Start_IT() to start the DMA transfer after the configuration of
              Source address and destination address and the Size of data to be transferred. In this
              case the DMA interrupt is configured.
          (+) Use HAL_DMA_IRQHandler() called under DMA_IRQHandler() Interrupt subroutine
          (+) At the end of data transfer HAL_DMA_IRQHandler() function is executed and user can
              add his own function by customization of function pointer XferCpltCallback and
              XferErrorCallback.

          (+) Use HAL_DMA_Abort_IT() function to abort the current transfer : non-blocking method. This API will finish the execution immediately,
              and the callback XferAbortCallback (if specified  by the user) is asserted once the DMA channel has effectively aborted.
              HAL_DMA_Abort_IT() API could be called from an interrupt service routine.

    [..]  Use HAL_DMA_GetState() function to return the DMA state and HAL_DMA_GetError() in case of error detection.

     *** DMA HAL driver macros list ***
     =============================================
     [..]
       Below the list of most used macros in DMA HAL driver.

      (+) __HAL_DMA_ENABLE: Enable the specified DMA Channel.
      (+) __HAL_DMA_DISABLE: Disable the specified DMA Channel.
      (+) __HAL_DMA_GET_FLAG: Get the DMA Channel pending flags.
      (+) __HAL_DMA_CLEAR_FLAG: Clear the DMA Channel pending flags.
      (+) __HAL_DMA_ENABLE_IT: Enable the specified DMA Channel interrupts.
      (+) __HAL_DMA_DISABLE_IT: Disable the specified DMA Channel interrupts.
      (+) __HAL_DMA_GET_IT_SOURCE: Check whether the specified DMA Channel interrupt has occurred or not.

     [..]
      (@) You can refer to the header file of the DMA HAL driver for more useful macros.

    [..]
    @endverbatim
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

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"

/** @addtogroup STM32U5xx_HAL_Driver
  * @{
  */

/** @defgroup DMA DMA
  * @brief DMA HAL module driver
  * @{
  */

#ifdef HAL_DMA_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/** @addtogroup DMA_Private_Functions_Prototypes
  * @{
  */
static void DMA_SetConfig(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t SourceDataSize);
static void DMA_Init     (DMA_HandleTypeDef *hdma);
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/** @addtogroup DMA_Exported_Functions DMA Exported Functions
  * @{
  */

/** @addtogroup DMA_Exported_Functions_Group1
  *
@verbatim
 ===============================================================================
             ##### Initialization and de-initialization functions  #####
 ===============================================================================
    [..]
    This section provides functions allowing to initialize and de-initialize the DMA channel.
    [..]
    The HAL_DMA_Init() function follows the DMA channel configuration procedures as described in
    reference manual.
    The HAL_DMA_DeInit() function allows to deinitialize the DMA channel.

@endverbatim
  * @{
  */

/**
  * @brief  Initialize the DMA according to the specified
  *         parameters in the DMA_InitTypeDef and create the associated handle.
  * @param  hdma: Pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA Channel.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA_Init(DMA_HandleTypeDef *hdma)
{
  uint32_t tickstart = HAL_GetTick();

  /* Check the DMA peripheral handle */
  if (hdma == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_DMA_ALL_INSTANCE             (hdma->Instance));
  assert_param(IS_DMA_REQUEST                  (hdma->Init.Request));
  assert_param(IS_DMA_BLOCK_HW_REQUEST         (hdma->Init.BlkHWRequest));
  assert_param(IS_DMA_DIRECTION                (hdma->Init.Direction));
  assert_param(IS_DMA_SOURCE_INC               (hdma->Init.SrcInc));
  assert_param(IS_DMA_DESTINATION_INC          (hdma->Init.DestInc));
  assert_param(IS_DMA_SOURCE_DATA_WIDTH        (hdma->Init.SrcDataWidth));
  assert_param(IS_DMA_DESTINATION_DATA_WIDTH   (hdma->Init.DestDataWidth));
  assert_param(IS_DMA_PRIORITY                 (hdma->Init.Priority));
  assert_param(IS_DMA_TCEM_EVENT_MODE          (hdma->Init.TransferEventMode));
  assert_param(IS_DMA_MODE                     (hdma->Init.Mode));
  if (IS_GPDMA_INSTANCE(hdma->Instance) != 0U)
  {
    assert_param(IS_DMA_BURST_LENGTH           (hdma->Init.SrcBurstLength));
    assert_param(IS_DMA_BURST_LENGTH           (hdma->Init.DestBurstLength));
    assert_param(IS_DMA_TRANSFER_ALLOCATED_PORT(hdma->Init.TransferAllocatedPort));
  }

  /* Allocate lock resource */
  __HAL_UNLOCK(hdma);

  /* Change DMA peripheral state */
  hdma->State = HAL_DMA_STATE_BUSY;

  /* Disable the DMA channel */
  __HAL_DMA_DISABLE(hdma);

  /* Check if the DMA channel is effectively disabled */
  while ((hdma->Instance->CCR & DMA_CCR_EN) != 0U)
  {
    /* Check for the Timeout */
    if ((HAL_GetTick() - tickstart) > HAL_TIMEOUT_DMA_ABORT)
    {
      /* Update error code */
      hdma->ErrorCode = HAL_DMA_ERROR_TIMEOUT;

      /* Change the DMA state */
      hdma->State = HAL_DMA_STATE_ERROR;

      /* Return error status */
      return HAL_ERROR;
    }
  }

  /* Initialize the DMA channel registers */
  DMA_Init(hdma);

  /* Update DMA channel operation mode */
  hdma->Mode = hdma->Init.Mode;

  /* Initialise the error code */
  hdma->ErrorCode = HAL_DMA_ERROR_NONE;

  /* Initialize the DMA state */
  hdma->State = HAL_DMA_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  DeInitialize the DMA peripheral
  * @param  hdma: Pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA Channel.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA_DeInit(DMA_HandleTypeDef *hdma)
{
  DMA_TypeDef *DMA      = HAL_DMA_INSTANCE(hdma);
  uint32_t    tickstart = HAL_GetTick();


  /* Check the DMA peripheral handle */
  if (hdma == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_DMA_ALL_INSTANCE(hdma->Instance));

  /* Disable the selected DMA Channel */
  __HAL_DMA_DISABLE(hdma);

  /* Check if the DMA channel is effectively disabled */
  while ((hdma->Instance->CCR & DMA_CCR_EN) != 0U)
  {
    /* Check for the Timeout */
    if ((HAL_GetTick() - tickstart) > HAL_TIMEOUT_DMA_ABORT)
    {
      /* Update error code */
      hdma->ErrorCode = HAL_DMA_ERROR_TIMEOUT;

      /* Change the DMA state */
      hdma->State = HAL_DMA_STATE_ERROR;

      /* Return error status */
      return HAL_ERROR;
    }
  }

  /* Reset DMA Channel registers */
  hdma->Instance->CCR   = 0U;
  hdma->Instance->CLBAR = 0U;
  hdma->Instance->CTR1  = 0U;
  hdma->Instance->CTR2  = 0U;
  hdma->Instance->CBR1  = 0U;
  hdma->Instance->CSAR  = 0U;
  hdma->Instance->CDAR  = 0U;
  hdma->Instance->CLLR  = 0U;

  /* Reset 2D Addressing registers */
  if (IS_DMA_2D_ADDRESSING_INSTANCE(hdma->Instance) != 0U)
  {
    hdma->Instance->CTR3 = 0U;
    hdma->Instance->CBR2 = 0U;
  }

  /* Clear privilege attribute */
  CLEAR_BIT(DMA->PRIVCFGR, (1UL << (HAL_DMA_CHANNEL(hdma) & 0x1FU)));

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
  /* Clear secure attribute */
  CLEAR_BIT(DMA->SECCFGR, (1UL << (HAL_DMA_CHANNEL(hdma) & 0x1FU)));
#endif /* __ARM_FEATURE_CMSE */

  /* Clear all flags */
  __HAL_DMA_CLEAR_FLAG(hdma, (DMA_FLAG_TC | DMA_FLAG_HT | DMA_FLAG_DTE | DMA_FLAG_ULE | DMA_FLAG_USE | DMA_FLAG_SUSP | DMA_FLAG_TO));

  /* Clean all callbacks */
  hdma->XferCpltCallback     = NULL;
  hdma->XferHalfCpltCallback = NULL;
  hdma->XferErrorCallback    = NULL;
  hdma->XferAbortCallback    = NULL;
  hdma->XferSuspendCallback  = NULL;

  /* Clean DMA queue */
  hdma->LinkedListQueue = NULL;

  /* Clean DMA parent */
  if (hdma->Parent != NULL)
  {
    hdma->Parent = NULL;
  }

  /* Update DMA channel operation mode */
  hdma->Mode = DMA_NORMAL;

  /* Reset the error code */
  hdma->ErrorCode = HAL_DMA_ERROR_NONE;

  /* Reset the DMA state */
  hdma->State = HAL_DMA_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(hdma);

  return HAL_OK;
}
/**
  * @}
  */


/** @addtogroup DMA_Exported_Functions_Group2
 *
@verbatim
 ===============================================================================
                      #####  IO operation functions  #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Configure the source, destination address and data size and Start DMA transfer
      (+) Configure the source, destination address and data size and
          Start DMA transfer with interrupt
      (+) Abort DMA transfer
      (+) Poll for transfer complete
      (+) Handle DMA interrupt request
      (+) Register and Unregister DMA callbacks

@endverbatim
  * @{
  */

/**
  * @brief  Start the DMA Transfer.
  * @param  hdma            : Pointer to a DMA_HandleTypeDef structure that contains
  *                           the configuration information for the specified DMA Channel.
  * @param  SrcAddress      : The source memory Buffer address.
  * @param  DstAddress      : The destination memory Buffer address.
  * @param  SourceDataSize  : The size of a source block transfer in bytes.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA_Start(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t SourceDataSize)
{
  /* Check the DMA peripheral handle */
  if (hdma == NULL)
  {
    /* Return error status */
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_DMA_BLOCK_SIZE(SourceDataSize));

  /* Process locked */
  __HAL_LOCK(hdma);

  if (hdma->State == HAL_DMA_STATE_READY)
  {
    /* Change DMA peripheral state */
    hdma->State = HAL_DMA_STATE_BUSY;

    /* Initialize the error code */
    hdma->ErrorCode = HAL_DMA_ERROR_NONE;

    /* Configure the source, destination address and the data size */
    DMA_SetConfig(hdma, SrcAddress, DstAddress, SourceDataSize);

    /* Enable the Peripheral */
    __HAL_DMA_ENABLE(hdma);
  }
  else
  {
    /* Change the error code */
    hdma->ErrorCode = HAL_DMA_ERROR_BUSY;

    /* Process unlocked */
    __HAL_UNLOCK(hdma);

    /* Return error status */
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Starts the DMA Transfer with interrupts enabled.
  * @param  hdma            : Pointer to a MMA_HandleTypeDef structure that contains
  *                           the configuration information for the specified DMA Channel.
  * @param  SrcAddress      : The source memory Buffer address.
  * @param  DstAddress      : The destination memory Buffer address.
  * @param  SourceDataSize  : The size of a source block transfer in bytes.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA_Start_IT(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t SourceDataSize)
{
  /* Check the parameters */
  assert_param(IS_DMA_BLOCK_SIZE(SourceDataSize));

  /* Check the DMA peripheral handle */
  if (hdma == NULL)
  {
    /* Return error status */
    return HAL_ERROR;
  }

  /* Process locked */
  __HAL_LOCK(hdma);

  if (hdma->State == HAL_DMA_STATE_READY)
  {
    /* Change DMA peripheral state */
    hdma->State = HAL_DMA_STATE_BUSY;

    /* Initialize the error code */
    hdma->ErrorCode = HAL_DMA_ERROR_NONE;

    /* Configure the source, destination address and the data size */
    DMA_SetConfig(hdma, SrcAddress, DstAddress, SourceDataSize);

    /* Enable common interrupts: Transfer Complete and Transfer Errors ITs */
    __HAL_DMA_ENABLE_IT(hdma, (DMA_IT_TC | DMA_IT_DTE | DMA_IT_ULE | DMA_IT_USE | DMA_IT_TO));

    if (hdma->XferHalfCpltCallback != NULL)
    {
      /* if Half Transfer complete callback is set, enable the corresponding IT */
      __HAL_DMA_ENABLE_IT(hdma, DMA_IT_HT);
    }

    if (hdma->XferSuspendCallback != NULL)
    {
      /* if Transfer suspend callback is set, enable the corresponding IT */
      __HAL_DMA_ENABLE_IT(hdma, DMA_IT_SUSP);
    }

    /* Enable the Peripheral */
    __HAL_DMA_ENABLE(hdma);
  }
  else
  {
    /* Change the error code */
    hdma->ErrorCode = HAL_DMA_ERROR_BUSY;

    /* Process unlocked */
    __HAL_UNLOCK(hdma);

    /* Return error status */
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Abort the DMA Transfer.
  * @param  hdma  : Pointer to a DMA_HandleTypeDef structure that contains
  *                 the configuration information for the specified DMA Channel.
  *
  * @note   After suspending a DMA Channel, a wait until the DMA Channel is
  *         effectively stopped is added. If a Channel is suspended while a data
  *         transfer is on-going, the current data will be transferred and the
  *         Channel will be effectively suspended only after the transfer of
  *         any on-going data is finished.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA_Abort(DMA_HandleTypeDef *hdma)
{
  uint32_t tickstart =  HAL_GetTick();

  /* Check the DMA peripheral handle */
  if (hdma == NULL)
  {
    /* Return error status */
    return HAL_ERROR;
  }

  if (hdma->State != HAL_DMA_STATE_BUSY)
  {
    /* Update error code */
    hdma->ErrorCode = HAL_DMA_ERROR_NO_XFER;

    /* Process Unlocked */
    __HAL_UNLOCK(hdma);

    /* Return error status */
    return HAL_ERROR;
  }
  else
  {
    /* Suspend the channel */
    hdma->Instance->CCR |= DMA_CCR_SUSP;

    /* Change the DMA state*/
    hdma->State = HAL_DMA_STATE_SUSPEND;

    /* Check if the DMA Channel is suspended */
    while ((hdma->Instance->CSR & DMA_CSR_SUSPF) == 0U)
    {
      /* Check for the Timeout */
      if ((HAL_GetTick() - tickstart) > HAL_TIMEOUT_DMA_ABORT)
      {
        /* Update error code */
        hdma->ErrorCode |= HAL_DMA_ERROR_TIMEOUT;

        /* Process Unlocked */
        __HAL_UNLOCK(hdma);

        /* Change the DMA state */
        hdma->State = HAL_DMA_STATE_ERROR;

        /* Change the queue state */
        if ((hdma->Mode & DMA_LINKEDLIST) == DMA_LINKEDLIST)
        {
          hdma->LinkedListQueue->State = HAL_DMA_QUEUE_STATE_READY;
        }

        /* Return error status */
        return HAL_ERROR;
      }
    }

    /* Reset the channel */
    hdma->Instance->CCR |= DMA_CCR_RESET;

    /* Change the DMA state*/
    hdma->State = HAL_DMA_STATE_ABORT;

    /* Clear all status flags */
    __HAL_DMA_CLEAR_FLAG(hdma, (DMA_FLAG_TC | DMA_FLAG_HT | DMA_FLAG_DTE | DMA_FLAG_ULE | DMA_FLAG_USE | DMA_FLAG_SUSP | DMA_FLAG_TO));

    /* Process Unlocked */
    __HAL_UNLOCK(hdma);

    /* Change the DMA state*/
    hdma->State = HAL_DMA_STATE_READY;

    /* Change the queue state */
    if ((hdma->Mode & DMA_LINKEDLIST) == DMA_LINKEDLIST)
    {
      hdma->LinkedListQueue->State = HAL_DMA_QUEUE_STATE_READY;

      /* Clear CBR1 to ensure loading linked-list from memory next start */
      hdma->Instance->CBR1 = 0U;
    }
  }

  return HAL_OK;
}

/**
  * @brief  Abort the DMA Transfer in Interrupt mode.
  * @param  hdma  : Pointer to a DMA_HandleTypeDef structure that contains
  *                 the configuration information for the specified DMA Channel.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA_Abort_IT(DMA_HandleTypeDef *hdma)
{
  /* Check the DMA peripheral handle */
  if (hdma == NULL)
  {
    /* Return error status */
    return HAL_ERROR;
  }

  if (hdma->State != HAL_DMA_STATE_BUSY)
  {
    /* Update error code */
    hdma->ErrorCode = HAL_DMA_ERROR_NO_XFER;

    /* Return error status */
    return HAL_ERROR;
  }
  else
  {
    /* Set Abort State  */
    hdma->State = HAL_DMA_STATE_ABORT;

    /* Suspend the channel and activate suspend interrupt */
    hdma->Instance->CCR |= (DMA_CCR_SUSP | DMA_CCR_SUSPIE);
  }

  return HAL_OK;
}

/**
  * @brief  Polling for transfer complete.
  * @param  hdma:          Pointer to a DMA_HandleTypeDef structure that contains
  *                        the configuration information for the specified DMA Channel.
  * @param  CompleteLevel: Specifies the DMA level complete.
  * @param  Timeout:       Timeout duration.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA_PollForTransfer(DMA_HandleTypeDef *hdma, HAL_DMA_LevelCompleteTypeDef CompleteLevel, uint32_t Timeout)
{
  uint32_t tickstart = HAL_GetTick();
  uint32_t levelFlag, tmp_csr;

  /* Check the parameters */
  assert_param(IS_DMA_LEVEL_COMPLETE(CompleteLevel));

  /* Check the DMA peripheral handle */
  if (hdma == NULL)
  {
    /* Return error status */
    return HAL_ERROR;
  }

  if (hdma->State != HAL_DMA_STATE_BUSY)
  {
    /* No transfer ongoing */
    hdma->ErrorCode = HAL_DMA_ERROR_NO_XFER;

    /* Process Unlocked */
    __HAL_UNLOCK(hdma);

    /* Return error status */
    return HAL_ERROR;
  }

  /* Polling mode is not supported in circular mode */
  if ((hdma->Mode & DMA_LINKEDLIST_CIRCULAR) == DMA_LINKEDLIST_CIRCULAR)
  {
    hdma->ErrorCode = HAL_DMA_ERROR_NOT_SUPPORTED;
    return HAL_ERROR;
  }

  /* Get the level transfer complete flag */
  levelFlag = ((CompleteLevel == HAL_DMA_FULL_TRANSFER) ? DMA_FLAG_IDLE : DMA_FLAG_HT);

  tmp_csr = hdma->Instance->CSR;

  while ((tmp_csr & levelFlag) == 0U)
  {
    /* Check for the Timeout */
    if (Timeout != HAL_MAX_DELAY)
    {
      if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
      {
        /* Update error code */
        hdma->ErrorCode |= HAL_DMA_ERROR_TIMEOUT;

        /*
          If timeout, abort the current transfer.
          Note that the Abort function will
          - Clear all transfer flags.
          - Unlock.
          - Set the State.
        */
        (void)HAL_DMA_Abort(hdma);

        /* Return error status */
        return HAL_ERROR;
      }
    }

    /* Get a newer CSR register value */
    tmp_csr = hdma->Instance->CSR;
  }

  if ((tmp_csr & DMA_FLAG_TO) != 0U)
  {
    /* Update error code */
    hdma->ErrorCode |= HAL_DMA_ERROR_TO;

    /* Clear the error flag */
    __HAL_DMA_CLEAR_FLAG(hdma, DMA_FLAG_TO);
  }

  if ((tmp_csr & (DMA_FLAG_DTE | DMA_FLAG_ULE | DMA_FLAG_USE)) != 0U)
  {
    if ((tmp_csr & DMA_FLAG_DTE) != 0U)
    {
      /* Update error code */
      hdma->ErrorCode |= HAL_DMA_ERROR_DTE;

      /* Clear the error flag */
      __HAL_DMA_CLEAR_FLAG(hdma, DMA_FLAG_DTE);
    }

    if ((tmp_csr & DMA_FLAG_ULE) != 0U)
    {
      /* Update error code */
      hdma->ErrorCode |= HAL_DMA_ERROR_ULE;

      /* Clear the error flag */
      __HAL_DMA_CLEAR_FLAG(hdma, DMA_FLAG_ULE);
    }

    if ((tmp_csr & DMA_FLAG_USE) != 0U)
    {
      /* Update error code */
      hdma->ErrorCode |= HAL_DMA_ERROR_USE;

      /* Clear the error flag */
      __HAL_DMA_CLEAR_FLAG(hdma, DMA_FLAG_USE);
    }

    /* Reset the channel */
    hdma->Instance->CCR |= DMA_CCR_RESET;

    /* Process Unlocked */
    __HAL_UNLOCK(hdma);

    /* Change the DMA state */
    hdma->State = HAL_DMA_STATE_READY;

    /* Change the queue state */
    if ((hdma->Mode & DMA_LINKEDLIST) == DMA_LINKEDLIST)
    {
      hdma->LinkedListQueue->State = HAL_DMA_QUEUE_STATE_READY;
    }

    /* Return error status */
    return HAL_ERROR;
  }

  /* Clear the transfer level flag */
  if (CompleteLevel == HAL_DMA_HALF_TRANSFER)
  {
    /* Clear the Half Transfert flag */
    __HAL_DMA_CLEAR_FLAG(hdma, DMA_FLAG_HT);
  }
  else if (CompleteLevel == HAL_DMA_FULL_TRANSFER)
  {
    /* Clear the Transfert flags */
    __HAL_DMA_CLEAR_FLAG(hdma, (DMA_FLAG_TC | DMA_FLAG_HT));

    /* Process unlocked */
    __HAL_UNLOCK(hdma);

    /* Change the DMA state */
    hdma->State = HAL_DMA_STATE_READY;

    /* Change the queue state */
    if ((hdma->Mode & DMA_LINKEDLIST) == DMA_LINKEDLIST)
    {
      hdma->LinkedListQueue->State = HAL_DMA_QUEUE_STATE_READY;
    }
  }
  else
  {
    /* Return error status */
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Handle DMA interrupt request.
  * @param  hdma: Pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA Channel.
  * @retval None
  */
void HAL_DMA_IRQHandler(DMA_HandleTypeDef *hdma)
{
  DMA_TypeDef *DMA;
  uint32_t globalITFlag;

  /* Global Interrupt Flag management *****************************************/
  DMA = HAL_DMA_INSTANCE(hdma);
  globalITFlag =  1UL << (HAL_DMA_CHANNEL(hdma) & 0x1FU);

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
  if ((DMA->SMISR & globalITFlag) == 0U)
#else
  if ((DMA->MISR & globalITFlag) == 0U)
#endif /* __ARM_FEATURE_CMSE */
  {
    return; /* the global interrupt flag for the current channel is down , nothing to do */
  }

  /* Data Transfer Error Interrupt management *********************************/
  if ((__HAL_DMA_GET_FLAG(hdma, DMA_FLAG_DTE) != 0U))
  {
    if (__HAL_DMA_GET_IT_SOURCE(hdma, DMA_IT_DTE) != 0U)
    {
      /* Clear the transfer error flag */
      __HAL_DMA_CLEAR_FLAG(hdma, DMA_FLAG_DTE);

      /* Update error code : Data Transfer Error */
      hdma->ErrorCode |= HAL_DMA_ERROR_DTE;
    }
  }

  /* Update Linked-list Error Interrupt management ****************************/
  if ((__HAL_DMA_GET_FLAG(hdma, DMA_FLAG_ULE) != 0U))
  {
    if (__HAL_DMA_GET_IT_SOURCE(hdma, DMA_IT_ULE) != 0U)
    {
      /* Clear the update linked-list error flag */
      __HAL_DMA_CLEAR_FLAG(hdma, DMA_FLAG_ULE);

      /* Update error code : Update Linked-list Error */
      hdma->ErrorCode |= HAL_DMA_ERROR_ULE;
    }
  }

  /* User Setting Error Interrupt management **********************************/
  if ((__HAL_DMA_GET_FLAG(hdma, DMA_FLAG_USE) != 0U))
  {
    if (__HAL_DMA_GET_IT_SOURCE(hdma, DMA_IT_USE) != 0U)
    {
      /* Clear the user setting error flag */
      __HAL_DMA_CLEAR_FLAG(hdma, DMA_FLAG_USE);

      /* Update error code : Update Setting Error */
      hdma->ErrorCode |= HAL_DMA_ERROR_USE;
    }
  }

  /* Trigger Overrun Interrupt management *************************************/
  if ((__HAL_DMA_GET_FLAG(hdma, DMA_FLAG_TO) != 0U))
  {
    if (__HAL_DMA_GET_IT_SOURCE(hdma, DMA_IT_TO) != 0U)
    {
      /* Clear the trigger overrun flag */
      __HAL_DMA_CLEAR_FLAG(hdma, DMA_FLAG_TO);

      /* Update error code : Trigger Overrun */
      hdma->ErrorCode |= HAL_DMA_ERROR_TO;
    }
  }

  /* Half Transfer Complete Interrupt management ******************************/
  if ((__HAL_DMA_GET_FLAG(hdma, DMA_FLAG_HT) != 0U))
  {
    if (__HAL_DMA_GET_IT_SOURCE(hdma, DMA_IT_HT) != 0U)
    {
      /* Clear the half transfer flag */
      __HAL_DMA_CLEAR_FLAG(hdma, DMA_FLAG_HT);

      if (hdma->XferHalfCpltCallback != NULL)
      {
        /* Half transfer callback */
        hdma->XferHalfCpltCallback(hdma);
      }
    }
  }

  /* Suspend Transfer Interrupt management ************************************/
  if ((__HAL_DMA_GET_FLAG(hdma, DMA_FLAG_SUSP) != 0U))
  {
    if (__HAL_DMA_GET_IT_SOURCE(hdma, DMA_IT_SUSP) != 0U)
    {
      /* Clear the block transfer complete flag */
      __HAL_DMA_CLEAR_FLAG(hdma, DMA_FLAG_SUSP);

      if (hdma->State == HAL_DMA_STATE_ABORT)
      {
        /* Disable the suspend transfer interrupt */
        __HAL_DMA_DISABLE_IT(hdma, DMA_IT_SUSP);

        /* Reset the channel internal state and reset the FIFO */
        hdma->Instance->CCR |= DMA_CCR_RESET;

        /* Process Unlocked */
        __HAL_UNLOCK(hdma);

        if((hdma->Instance->CCR & DMA_CCR_EN) != 0U)
        {
          /* Change the DMA state to error if DMA disable fails */
          hdma->State = HAL_DMA_STATE_ERROR;
        }
        else
        {
          /* Change the DMA state to Ready if DMA disable success */
          hdma->State = HAL_DMA_STATE_READY;
        }

        /* Change the queue state */
        if ((hdma->Mode & DMA_LINKEDLIST) == DMA_LINKEDLIST)
        {
          hdma->LinkedListQueue->State = HAL_DMA_QUEUE_STATE_READY;
        }

        if (hdma->XferAbortCallback != NULL)
        {
          /* Transfer abort callback */
          hdma->XferAbortCallback(hdma);
        }

        return;
      }
      else
      {
        /* Change the DMA state */
        hdma->State = HAL_DMA_STATE_SUSPEND;

        if (hdma->XferSuspendCallback != NULL)
        {
          /* Transfer suspend callback */
          hdma->XferSuspendCallback(hdma);
        }
      }
    }
  }

  /* Transfer Complete Interrupt management ***********************************/
  if ((__HAL_DMA_GET_FLAG(hdma, DMA_FLAG_TC) != 0U))
  {
    if (__HAL_DMA_GET_IT_SOURCE(hdma, DMA_IT_TC) != 0U)
    {
      if ((hdma->Mode & DMA_LINKEDLIST) == DMA_LINKEDLIST)
      {
        /* If linked-list transfer */
        if (hdma->Instance->CLLR == 0U)
        {
          if (hdma->Instance->CBR1 == 0U)
          {
            /* Change the DMA state to Ready */
            hdma->State = HAL_DMA_STATE_READY;
            /* Change the queue state */
            hdma->LinkedListQueue->State = HAL_DMA_QUEUE_STATE_READY;
          }
        }
      }
      else
      {
        /* If not linked-list transfer */
        if (hdma->Instance->CBR1 == 0U)
        {
          /* Change the DMA state to Ready */
          hdma->State = HAL_DMA_STATE_READY;
        }
      }

      /* Clear TC and HT transfer flags */
      __HAL_DMA_CLEAR_FLAG(hdma, (DMA_FLAG_TC | DMA_FLAG_HT));

      /* Process Unlocked */
      __HAL_UNLOCK(hdma);

      if (hdma->XferCpltCallback != NULL)
      {
        /* Channel Transfer Complete callback */
        hdma->XferCpltCallback(hdma);
      }
    }
  }

  /* Manage error case ********************************************************/
  if (hdma->ErrorCode != HAL_DMA_ERROR_NONE)
  {
    /* Reset the channel internal state and reset the FIFO */
    hdma->Instance->CCR |= DMA_CCR_RESET;

    /* Process Unlocked */
    __HAL_UNLOCK(hdma);

    if((hdma->Instance->CCR & DMA_CCR_EN) != 0U)
    {
      /* Change the DMA state to error if DMA disable fails */
      hdma->State = HAL_DMA_STATE_ERROR;
    }
    else
    {
      /* Change the DMA state to Ready if DMA disable success */
      hdma->State = HAL_DMA_STATE_READY;
    }

    /* Change the queue state */
    if ((hdma->Mode & DMA_LINKEDLIST) == DMA_LINKEDLIST)
    {
      hdma->LinkedListQueue->State = HAL_DMA_QUEUE_STATE_READY;
    }

    if (hdma->XferErrorCallback != NULL)
    {
      /* Transfer error callback */
      hdma->XferErrorCallback(hdma);
    }
  }
}

/**
  * @brief  Register callbacks.
  * @param  hdma:        Pointer to a DMA_HandleTypeDef structure that contains
  *                      the configuration information for the specified DMA Channel.
  * @param  CallbackID:  User Callback identifier which could be a value of
  *                      HAL_DMA_CallbackIDTypeDef enum.
  * @param  pCallback:   Pointer to private callback function.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA_RegisterCallback(DMA_HandleTypeDef *hdma, HAL_DMA_CallbackIDTypeDef CallbackID, void (* pCallback)(DMA_HandleTypeDef *_hdma))
{
  HAL_StatusTypeDef status = HAL_OK;

  if ((hdma == NULL) || (pCallback == NULL))
  {
    /* Update the error code */
    hdma->ErrorCode |= HAL_DMA_ERROR_INVALID_CALLBACK;

    /* Return error status */
    return HAL_ERROR;
  }

  /* Process locked */
  __HAL_LOCK(hdma);

  if (hdma->State == HAL_DMA_STATE_READY)
  {
    switch (CallbackID)
    {
      case  HAL_DMA_XFER_CPLT_CB_ID:
        /* Register transfert complete callback */
        hdma->XferCpltCallback = pCallback;
        break;

      case  HAL_DMA_XFER_HALFCPLT_CB_ID:
        /* Register half transfert callback */
        hdma->XferHalfCpltCallback = pCallback;
        break;

      case  HAL_DMA_XFER_ERROR_CB_ID:
        /* Register transfert error callback */
        hdma->XferErrorCallback = pCallback;
        break;

      case  HAL_DMA_XFER_ABORT_CB_ID:
        /* Register abort callback */
        hdma->XferAbortCallback = pCallback;
        break;

      case  HAL_DMA_XFER_SUSPEND_CB_ID:
        /* Register suspend callback */
        hdma->XferSuspendCallback = pCallback;
        break;

      default:
        /* Update error status */
        status = HAL_ERROR;
        break;
    }
  }
  else
  {
    /* Update error status */
    status =  HAL_ERROR;
  }

  /* Release Lock */
  __HAL_UNLOCK(hdma);

  return status;
}

/**
  * @brief  UnRegister callbacks.
  * @param  hdma:        Pointer to a DMA_HandleTypeDef structure that contains
  *                      the configuration information for the specified DMA Channel.
  * @param  CallbackID:  User Callback identifier which could be a value of
  *                      HAL_DMA_CallbackIDTypeDef enum.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA_UnRegisterCallback(DMA_HandleTypeDef *hdma, HAL_DMA_CallbackIDTypeDef CallbackID)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the DMA peripheral handle */
  if (hdma == NULL)
  {
    /* Return error status */
    return HAL_ERROR;
  }

  /* Process locked */
  __HAL_LOCK(hdma);

  if (hdma->State == HAL_DMA_STATE_READY)
  {
    switch (CallbackID)
    {
      case  HAL_DMA_XFER_CPLT_CB_ID:
        /* UnRegister transfert complete callback */
        hdma->XferCpltCallback = NULL;
        break;

      case  HAL_DMA_XFER_HALFCPLT_CB_ID:
        /* UnRegister half transfert callback */
        hdma->XferHalfCpltCallback = NULL;
        break;

      case  HAL_DMA_XFER_ERROR_CB_ID:
        /* UnRegister transfert error callback */
        hdma->XferErrorCallback = NULL;
        break;

      case  HAL_DMA_XFER_ABORT_CB_ID:
        /* UnRegister abort callback */
        hdma->XferAbortCallback = NULL;
        break;

      case  HAL_DMA_XFER_SUSPEND_CB_ID:
        /* UnRegister suspend callback */
        hdma->XferSuspendCallback = NULL;
        break;

      case   HAL_DMA_XFER_ALL_CB_ID:
        /* UnRegister all available callbacks */
        hdma->XferCpltCallback     = NULL;
        hdma->XferHalfCpltCallback = NULL;
        hdma->XferErrorCallback    = NULL;
        hdma->XferAbortCallback    = NULL;
        hdma->XferSuspendCallback  = NULL;
        break;

      default:
        /* Update error status */
        status = HAL_ERROR;
        break;
    }
  }
  else
  {
    /* Update error status */
    status = HAL_ERROR;
  }

  /* Release Lock */
  __HAL_UNLOCK(hdma);

  return status;
}
/**
  * @}
  */


/** @addtogroup DMA_Exported_Functions_Group3
 *
@verbatim
 ===============================================================================
                    ##### State and Errors functions #####
 ===============================================================================
    [..]
    This subsection provides functions allowing to
      (+) Check the DMA state
      (+) Get error code

@endverbatim
  * @{
  */

/**
  * @brief  Returns the DMA state.
  * @param  hdma: Pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA Channel.
  * @retval DMA state
  */
HAL_DMA_StateTypeDef HAL_DMA_GetState(DMA_HandleTypeDef *hdma)
{
  /* Return the DMA state */
  return hdma->State;
}

/**
  * @brief  Return the DMA error code.
  * @param  hdma : Pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA Channel.
  * @retval DMA Error Code
  */
uint32_t HAL_DMA_GetError(DMA_HandleTypeDef *hdma)
{
  /* Return the DMA error code */
  return hdma->ErrorCode;
}
/**
  * @}
  */


/** @addtogroup DMA_Exported_Functions_Group4
 *
@verbatim
 ===============================================================================
                    ##### DMA Attributes functions #####
 ===============================================================================
    [..]
    This subsection provides functions allowing to
      (+) Configure Secure and Privilege attributes
      (+) Get Secure and Privilege attributes

@endverbatim
  * @{
  */

/**
  * @brief  Configure the DMA channel attribute(s).
  * @note   Available attributes are security and privilege protection.
  * @note   These attributes cannot be modified when the corresponding Lock state is enabled.
  * @param  hdma:              Pointer to a DMA_HandleTypeDef structure that contains
  *                            the configuration information for the specified DMA Channel.
  * @param  ChannelAttributes: Specifies the DMA channel secure/privilege attributes.
  *                            This parameter can be a one or a combination of
  *                            @ref DMA_Protection_Attributes.
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_DMA_ConfigChannelAttributes(DMA_HandleTypeDef *hdma, uint32_t ChannelAttributes)
{
  DMA_TypeDef *DMA;
  uint32_t channelIndex;

  /* Check the DMA peripheral handle */
  if (hdma == NULL)
  {
    /* Return error status */
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_DMA_ATTRIBUTES(ChannelAttributes));

  DMA = HAL_DMA_INSTANCE(hdma);
  channelIndex = 1UL << (HAL_DMA_CHANNEL(hdma) & 0x1FU);

  if (hdma->State == HAL_DMA_STATE_READY)
  {
    /* Channel privilege attribute management */
    if ((ChannelAttributes & DMA_CHANNEL_ATTR_PRIV_MASK) == DMA_CHANNEL_ATTR_PRIV_MASK)
    {
      /* Configure privilege/non-privilege attribute */
      if ((ChannelAttributes & DMA_CHANNEL_PRIV) == DMA_CHANNEL_PRIV)
      {
        DMA->PRIVCFGR |= channelIndex;
      }
      else
      {
        DMA->PRIVCFGR &= (~channelIndex);
      }
    }

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    /* Channel security attribute management */
    if ((ChannelAttributes & DMA_CHANNEL_ATTR_SEC_MASK) == DMA_CHANNEL_ATTR_SEC_MASK)
    {
      /* Configure secure/non-secure attribute */
      if ((ChannelAttributes & DMA_CHANNEL_SEC) == DMA_CHANNEL_SEC)
      {
        DMA->SECCFGR |= channelIndex;
      }
      else
      {
        DMA->SECCFGR &= (~channelIndex);
      }
    }

    /* Channel source security attribute management */
    if ((ChannelAttributes & DMA_CHANNEL_ATTR_SEC_SRC_MASK) == DMA_CHANNEL_ATTR_SEC_SRC_MASK)
    {
      /* Configure Source security attribute */
      if ((ChannelAttributes & DMA_CHANNEL_SRC_SEC) == DMA_CHANNEL_SRC_SEC)
      {
        hdma->Instance->CTR1 |= DMA_CTR1_SSEC;
      }
      else
      {
        hdma->Instance->CTR1 &= (~DMA_CTR1_SSEC);
      }
    }

    /* Channel destination security attribute management */
    if ((ChannelAttributes & DMA_CHANNEL_ATTR_SEC_DEST_MASK) == DMA_CHANNEL_ATTR_SEC_DEST_MASK)
    {
      /* Configure Destination security attribute */
      if ((ChannelAttributes & DMA_CHANNEL_DEST_SEC) == DMA_CHANNEL_DEST_SEC)
      {
        hdma->Instance->CTR1 |= DMA_CTR1_DSEC;
      }
      else
      {
        hdma->Instance->CTR1 &= (~DMA_CTR1_DSEC);
      }
    }
#endif /* __ARM_FEATURE_CMSE */
  }
  else
  {
    /* Change the error code */
    hdma->ErrorCode = HAL_DMA_ERROR_BUSY;

    /* Process unlocked */
    __HAL_UNLOCK(hdma);

    /* Return error status */
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Get the attribute of a DMA channel.
  * @note   Secure and non-secure attributes are only available from secure state
  *         when the system implements the security (TZEN=1).
  * @param  hdma:               Pointer to a DMA_HandleTypeDef structure that contains
  *                             the configuration information for the specified DMA Channel.
  * @param  pChannelAttributes: Pointer to return the attributes.
  * @retval HAL Status.
  */
HAL_StatusTypeDef HAL_DMA_GetConfigChannelAttributes(DMA_HandleTypeDef *hdma, uint32_t *pChannelAttributes)
{
  uint32_t attributes, channelIndex;
  DMA_TypeDef *DMA;

  /* Check the DMA peripheral handle */
  if ((hdma == NULL) || (pChannelAttributes == NULL))
  {
    /* Return error status */
    return HAL_ERROR;
  }

  DMA = HAL_DMA_INSTANCE(hdma);
  channelIndex = 1UL << (HAL_DMA_CHANNEL(hdma) & 0x1FU);

  /* Get channel privilege attribute */
  attributes = ((DMA->PRIVCFGR & channelIndex) == 0U) ? DMA_CHANNEL_NPRIV : DMA_CHANNEL_PRIV;

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
  /* Get channel security attribute */
  attributes |= ((DMA->SECCFGR & channelIndex) == 0U) ? DMA_CHANNEL_NSEC : DMA_CHANNEL_SEC;

  /* Get channel source security attribute */
  attributes |= ((hdma->Instance->CTR1 & DMA_CTR1_SSEC) == 0U) ? DMA_CHANNEL_SRC_NSEC : DMA_CHANNEL_SRC_SEC;

  /* Get channel destination security attribute */
  attributes |= ((hdma->Instance->CTR1 & DMA_CTR1_DSEC) == 0U) ? DMA_CHANNEL_DEST_NSEC : DMA_CHANNEL_DEST_SEC;
#endif /* __ARM_FEATURE_CMSE */

  /* return value */
  *pChannelAttributes = attributes;

  return HAL_OK;
}

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
/**
  * @brief  Lock the DMA channel attribute(s).
  * @note   Available attributes are security and privilege protection.
  * @param  hdma:              Pointer to a DMA_HandleTypeDef structure that contains
  *                            the configuration information for the specified DMA Channel.
  * @retval HAL Status
  */
HAL_StatusTypeDef HAL_DMA_LockChannelAttributes(DMA_HandleTypeDef *hdma)
{
  uint32_t channelIndex;
  DMA_TypeDef *DMA;

  /* Check the DMA peripheral handle */
  if (hdma == NULL)
  {
    /* Return error status */
    return HAL_ERROR;
  }

  DMA = HAL_DMA_INSTANCE(hdma);
  channelIndex = 1UL << (HAL_DMA_CHANNEL(hdma) & 0x1FU);

  /* Lock the DMA channel attribute */
  DMA->RCFGLOCKR |= channelIndex;

  return HAL_OK;
}
#endif /* __ARM_FEATURE_CMSE */

/**
  * @brief  Get the attribute lock state of a DMA channel.
  * @note   Available attributes are security and privilege protection.
  * @param  hdma:       Pointer to a DMA_HandleTypeDef structure that contains
  *                     the configuration information for the specified DMA Channel.
  * @param  pLockState: Pointer to lock state (returned value can be
  *                     DMA_CHANNEL_ATTRIBUTE_UNLOCKED or DMA_CHANNEL_ATTRIBUTE_LOCKED).
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMA_GetLockChannelAttributes(DMA_HandleTypeDef *hdma, uint32_t *pLockState)
{
  uint32_t channelIndex;
  DMA_TypeDef *DMA;

  /* Check the DMA peripheral handle */
  if ((hdma == NULL) || (pLockState == NULL))
  {
    /* Return error status */
    return HAL_ERROR;
  }

  DMA = HAL_DMA_INSTANCE(hdma);
  channelIndex = 1UL << (HAL_DMA_CHANNEL(hdma) & 0x1FU);

  /* Get channel lock attribute state */
  *pLockState = ((DMA->RCFGLOCKR & channelIndex) == 0U) ? DMA_CHANNEL_ATTRIBUTE_UNLOCKED : DMA_CHANNEL_ATTRIBUTE_LOCKED;

  return HAL_OK;
}
/**
  * @}
  */

/**
  * @}
  */


/** @addtogroup DMA_Private_Functions
  * @{
  */

/**
  * @brief  Set the DMA Transfer parameter.
  * @param  hdma:            Pointer to a DMA_HandleTypeDef structure that contains
  *                          the configuration information for the specified DMA Channel.
  * @param  SrcAddress:      The source memory buffer address.
  * @param  DstAddress:      The destination memory buffer address.
  * @param  SourceDataSize:  The size of a source block transfer in bytes.
  * @retval None
  */
static void DMA_SetConfig(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t SourceDataSize)
{
  /* Configure the DMA Channel data size */
  MODIFY_REG(hdma->Instance->CBR1, DMA_CBR1_BNDT, (SourceDataSize & DMA_CBR1_BNDT));

  /* Clear all interrupt flags */
  __HAL_DMA_CLEAR_FLAG(hdma, DMA_FLAG_TC | DMA_FLAG_HT | DMA_FLAG_DTE | DMA_FLAG_ULE | DMA_FLAG_USE | DMA_FLAG_SUSP | DMA_FLAG_TO);

  /* Configure DMA Channel Source address */
  hdma->Instance->CSAR = SrcAddress;

  /* Configure DMA Channel destination address */
  hdma->Instance->CDAR = DstAddress;
}

/**
  * @brief  Initialize the DMA handle according to the specified
  *         parameters in the DMA_InitTypeDef.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA Channel.
  * @retval None
  */
static void DMA_Init(DMA_HandleTypeDef *hdma)
{
  uint32_t tmpreg;

  /* Prepare DMA Channel Control Register (CCR) value *************************/
  tmpreg = hdma->Init.Priority;

  /* Write DMA Channel Control Register (CCR) */
  MODIFY_REG(hdma->Instance->CCR, DMA_CCR_PRIO | DMA_CCR_LAP | DMA_CCR_LSM, tmpreg);


  /* Prepare DMA Channel Transfer Register (CTR1) value ***********************/
  tmpreg = hdma->Init.DestInc | hdma->Init.DestDataWidth | \
           hdma->Init.SrcInc  | hdma->Init.SrcDataWidth;

  /* Add parameters specific to GPDMA */
  if (IS_GPDMA_INSTANCE(hdma->Instance) != 0U)
  {
    tmpreg |= (hdma->Init.TransferAllocatedPort | \
              (((hdma->Init.DestBurstLength - 1U) << DMA_CTR1_DBL_1_Pos) & DMA_CTR1_DBL_1) | \
              (((hdma->Init.SrcBurstLength - 1U) << DMA_CTR1_SBL_1_Pos) & DMA_CTR1_SBL_1));
  }

  /* Write DMA Channel Transfer Register 1 (CTR1) */
  MODIFY_REG(hdma->Instance->CTR1, ~(DMA_CTR1_SSEC | DMA_CTR1_DSEC), tmpreg);


  /* Prepare DMA Channel Transfer Register 2 (CTR2) value *********************/
  tmpreg = hdma->Init.BlkHWRequest                | \
           (hdma->Init.Request & DMA_CTR2_REQSEL) | \
            hdma->Init.TransferEventMode;

  /* Memory to Peripheral Transfer */
  if ((hdma->Init.Direction) == DMA_MEMORY_TO_PERIPH)
  {
    if (IS_GPDMA_INSTANCE(hdma->Instance) != 0U)
    {
      tmpreg |= DMA_CTR2_DREQ;
    }
  }
  /* Memory to Memory Transfer */
  else if ((hdma->Init.Direction) == DMA_MEMORY_TO_MEMORY)
  {
    tmpreg |= DMA_CTR2_SWREQ;
  }
  else
  {
    /* Nothing to do */
  }

  /* Write DMA Channel Transfer Register 2 (CTR2) */
  MODIFY_REG(hdma->Instance->CTR2, (DMA_CTR2_TCEM  | DMA_CTR2_TRIGPOL | DMA_CTR2_TRIGSEL | \
                                    DMA_CTR2_TRIGM | DMA_CTR2_BREQ    | DMA_CTR2_DREQ    | \
                                    DMA_CTR2_SWREQ | DMA_CTR2_REQSEL), tmpreg);


  /* Write DMA Channel Block Register 1 (CBR1) ********************************/
  WRITE_REG(hdma->Instance->CBR1, 0U);


  /* If 2D Addressing is supported by current channel */
  if (IS_DMA_2D_ADDRESSING_INSTANCE(hdma->Instance) != 0U)
  {
    /* Write DMA Channel Transfer Register 3 (CTR3) ***************************/
    WRITE_REG(hdma->Instance->CTR3, 0U);


    /* Write DMA Channel Block Register 2 (CBR2) ******************************/
    WRITE_REG(hdma->Instance->CBR2, 0U);
  }


  /* Write DMA Channel linked-list address register (CLLR) ********************/
  WRITE_REG(hdma->Instance->CLLR, 0U);
}
/**
  * @}
  */

#endif /* HAL_DMA_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
