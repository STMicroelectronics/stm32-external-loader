/**
  ******************************************************************************
  * @file    stm32u5xx_hal_dma_ex.c
  * @author  MCD Application Team
  * @brief   DMA Extension HAL module driver
  *          This file provides firmware functions to manage the following
  *          functionalities of the DMA Extension peripheral:
  *           + Extended features functions
  *
  @verbatim
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================
  [..]
  The DMA Extension HAL driver can be used as follows:
   (+) Create a DMA Link Node using the HAL_DMAEx_LinkedList_CreateNode() function
       according to the specified parameters in the pNodeConfig parameter.

   (+) Connect a node to the Linked-list using HAL_DMAEx_LinkedList_AddNode() function.
       The new node could be added at the tail of the linked-list, or inserted between
       two existing nodes.

   (+) Disconnect/Remove a node from the linked-list using the HAL_DMAEx_LinkedList_RemoveNode()
       function. This API allows to remove the last node of the linked-list, or any other node.

   (+) Make the linked-list circular by connecting the last node to the first one, using
       the HAL_DMAEx_LinkedList_EnableCircularMode() function. The reverse operation
       is also possible through the call to HAL_DMAEx_LinkedList_DisableCircularMode()
       function.

   (+) Suspend an on-going DMA transfer using HAL_DMAEx_Suspend() function in polling mode,
       and HAL_DMAEx_Suspend_IT() function in interrupt mode.
       (++) In interrput mode, use HAL_DMA_IRQHandler() called under DMA_IRQHandler() interrupt subroutine.
       (++) At the end of transfer suspension, HAL_DMA_IRQHandler() function is executed and user can
            add his own function by customization of function pointer XferSuspendCallback
           (i.e a member of DMA handle structure).

   (+) Resume a suspended DMA transfer using HAL_DMAEx_Resume() function.

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

/** @defgroup DMAEx DMAEx
  * @brief DMA Extended HAL module driver
  * @{
  */

#ifdef HAL_DMA_MODULE_ENABLED

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private Constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void DMA_LinkedList_Init(DMA_HandleTypeDef *hdma);

/* Exported functions ---------------------------------------------------------*/

/** @addtogroup DMAEx_Exported_Functions
  * @{
  */


/** @addtogroup DMAEx_Exported_Functions_Group1
  *
@verbatim
 ===============================================================================
     #####  Linked-list initialization and de-initialization functions  #####
 ===============================================================================
    [..]
    This section provides functions allowing to initialize and de-initialize the
    DMA linked-list.

@endverbatim
  * @{
  */

/**
  * @brief  Initialize the DMA linked-list according to the specified
  *         parameters in the DMA_InitLinkedListTypeDef.
  * @param  hdma: Pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA Channel.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_List_Init(DMA_HandleTypeDef *hdma)
{
  uint32_t tickstart = HAL_GetTick();

  /* Check the DMA peripheral handle */
  if (hdma == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_DMA_ALL_INSTANCE              (hdma->Instance));
  assert_param(IS_DMA_PRIORITY                  (hdma->InitLinkedList.Priority));
  assert_param(IS_DMA_LINK_STEP_MODE            (hdma->InitLinkedList.LinkStepMode));
  assert_param(IS_DMA_TCEM_LINKEDLIST_EVENT_MODE(hdma->InitLinkedList.TransferEventMode));
  assert_param(IS_DMA_LINKEDLIST_MODE           (hdma->InitLinkedList.LinkedListMode));

  if (IS_GPDMA_INSTANCE(hdma->Instance) != 0U)
  {
    assert_param(IS_DMA_LINK_ALLOCATED_PORT(hdma->InitLinkedList.LinkAllocatedPort));
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
  DMA_LinkedList_Init(hdma);

  /* Update DMA channel operation mode */
  hdma->Mode = hdma->InitLinkedList.LinkedListMode;

  /* Initialise the error code */
  hdma->ErrorCode = HAL_DMA_ERROR_NONE;

  /* Initialize the DMA state */
  hdma->State = HAL_DMA_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  DeInitialize the DMA peripheral when initialized to operate with
  *         linked-list.
  * @param  hdma: Pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA Channel.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_List_DeInit(DMA_HandleTypeDef *hdma)
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
  __HAL_DMA_CLEAR_FLAG(hdma, (DMA_FLAG_TC | DMA_FLAG_HT | DMA_FLAG_DTE | DMA_FLAG_ULE | DMA_FLAG_USE | DMA_FLAG_SUSP));

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


/** @addtogroup DMAEx_Exported_Functions_Group2
  *
@verbatim
 ===============================================================================
                 #####  Linked-list I/O operation functions  #####
 ===============================================================================
    [..]
    This section provides functions allowing to start linked-list execution in
    polling and interrupt modes.

@endverbatim
  * @{
  */

/**
  * @brief  Start the DMA linked-list transfer.
  * @param  hdma: Pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA Channel.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_List_Start(DMA_HandleTypeDef *hdma)
{
  uint32_t cllr_mask;

  /* Check the DMA peripheral handle */
  if ((hdma == NULL) || (hdma->LinkedListQueue == NULL))
  {
    /* Return error status */
    return HAL_ERROR;
  }

  /* Process locked */
  __HAL_LOCK(hdma);

  if (hdma->State == HAL_DMA_STATE_READY)
  {
    /* Update states */
    hdma->State                  = HAL_DMA_STATE_BUSY;
    hdma->LinkedListQueue->State = HAL_DMA_QUEUE_STATE_BUSY;

    /* Initialize error codes */
    hdma->ErrorCode                  = HAL_DMA_ERROR_NONE;
    hdma->LinkedListQueue->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

    /* Calculate CLLR mask */
    if ((hdma->LinkedListQueue->Head->NodeInfo & DMA_CHANNEL_TYPE_2D_ADDR) == DMA_CHANNEL_TYPE_2D_ADDR)
    {
      cllr_mask = DMA_CLLR_UT1 | DMA_CLLR_UT2 | DMA_CLLR_UB1 | DMA_CLLR_USA | \
                  DMA_CLLR_UDA | DMA_CLLR_UT3 | DMA_CLLR_UB2 | DMA_CLLR_ULL;
    }
    else
    {
      cllr_mask = DMA_CLLR_UT1 | DMA_CLLR_UT2 | DMA_CLLR_UB1 | DMA_CLLR_USA | \
                  DMA_CLLR_UDA | DMA_CLLR_ULL;
    }

    /* Update DMA registers for linked-list */
    hdma->Instance->CLBAR = ((uint32_t)hdma->LinkedListQueue->Head & DMA_CLBAR_LBA);
    hdma->Instance->CLLR  = ((uint32_t)hdma->LinkedListQueue->Head & DMA_CLLR_LA) | cllr_mask;

    /* Enable DMA channel */
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
  * @brief  Start the DMA linked-list transfer in interrupt mode.
  * @param  hdma: Pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA Channel.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_List_Start_IT(DMA_HandleTypeDef *hdma)
{
  uint32_t cllr_mask;

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
    /* Update states */
    hdma->State                  = HAL_DMA_STATE_BUSY;
    hdma->LinkedListQueue->State = HAL_DMA_QUEUE_STATE_BUSY;

    /* Initialize error codes */
    hdma->ErrorCode                  = HAL_DMA_ERROR_NONE;
    hdma->LinkedListQueue->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;

    /* Enable common interrupts: Transfer Complete and Transfer Errors ITs */
    __HAL_DMA_ENABLE_IT(hdma, (DMA_IT_TC | DMA_IT_DTE | DMA_IT_ULE | DMA_IT_USE));

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

    /* Calculate CLLR mask */
    if ((hdma->LinkedListQueue->Head->NodeInfo & DMA_CHANNEL_TYPE_2D_ADDR) == DMA_CHANNEL_TYPE_2D_ADDR)
    {
      cllr_mask = DMA_CLLR_UT1 | DMA_CLLR_UT2 | DMA_CLLR_UB1 | DMA_CLLR_USA | \
                  DMA_CLLR_UDA | DMA_CLLR_UT3 | DMA_CLLR_UB2 | DMA_CLLR_ULL;
    }
    else
    {
      cllr_mask = DMA_CLLR_UT1 | DMA_CLLR_UT2 | DMA_CLLR_UB1 | DMA_CLLR_USA | \
                  DMA_CLLR_UDA | DMA_CLLR_ULL;
    }

    /* Update DMA registers for linked-list */
    hdma->Instance->CLBAR = ((uint32_t)hdma->LinkedListQueue->Head & DMA_CLBAR_LBA);
    hdma->Instance->CLLR  = ((uint32_t)hdma->LinkedListQueue->Head & DMA_CLLR_LA) | cllr_mask;

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
  * @}
  */


/** @addtogroup DMA_Exported_Functions_Group3
  *
@verbatim
 ===============================================================================
                 #####  Linked-List management functions  #####
 ===============================================================================
    [..]
    This section provides functions allowing to manage linked-list.

@endverbatim
  * @{
  */

/**
  * @brief  Reset the linked-list queue.
  * @param  pQueueLinkList: Pointer to a DMA_QListTypeDef structure that
  *         contains queue informations.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_List_ResetQ(DMA_QListTypeDef *pQueueLinkList)
{
  uint32_t next_addr = 0U, current_addr, regnode_number, count = pQueueLinkList->NodeNumber;

  /* Check if no valid tail is selected and no new valid node is selected */
  if (pQueueLinkList == NULL)
  {
    return HAL_ERROR;
  }

  /* Check queue state */
  if (pQueueLinkList->State == HAL_DMA_QUEUE_STATE_BUSY)
  {
    pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_BUSY;
    return HAL_ERROR;
  }
  else
  {
    /* Set queue state */
    pQueueLinkList->State = HAL_DMA_QUEUE_STATE_BUSY;

    if (pQueueLinkList->Head != 0U)
    {
      /* Update CLLR and register number for 2D addressing node */
      if ((pQueueLinkList->Head->NodeInfo & DMA_CHANNEL_TYPE_2D_ADDR) == DMA_CHANNEL_TYPE_2D_ADDR)
      {
        regnode_number = 8U;
      }
      /* Update CLLR and register number for linear addressing node */
      else
      {
        regnode_number = 6U;
      }

      /* Check that node to be removed is linked to the selected Queue */
      while (count != 0U)
      {
        if (count == pQueueLinkList->NodeNumber)
        {
          current_addr = (uint32_t)pQueueLinkList->Head & DMA_CLLR_LA;
          next_addr = (pQueueLinkList->Head->LinkRegisters[regnode_number - 1U] & DMA_CLLR_LA);
        }
        else
        {
          current_addr = next_addr;
          next_addr = (((DMA_NodeTypeDef *)(current_addr + ((uint32_t)pQueueLinkList->Head & DMA_CLBAR_LBA)))->LinkRegisters[regnode_number - 1U] & DMA_CLLR_LA);
        }

        ((DMA_NodeTypeDef *)(current_addr + ((uint32_t)pQueueLinkList->Head & DMA_CLBAR_LBA)))->LinkRegisters[regnode_number - 1U] = 0U;

        /* Decrement counter */
        count--;
      }
    }

    /* Reset head node address */
    pQueueLinkList->Head = NULL;

    /* Reser node number */
    pQueueLinkList->NodeNumber = 0U;

    /* Reset first circular node */
    pQueueLinkList->FirstCircularNode = NULL;
  }

  /* Update queue state and error code */
  pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;
  pQueueLinkList->State     = HAL_DMA_QUEUE_STATE_RESET;

  return HAL_OK;
}

/**
  * @brief  Build a DMA Node according to the specified parameters in the
  *         DMA_NodeConfTypeDef.
  * @param  pNodeConfig: Pointer to a DMA_NodeConfTypeDef structure that contains
  *                      the configuration information for the specified DMA
  *                      Linked-List Node.
  * @param  pNode:       Pointer to a DMA_NodeTypeDef structure that contains
  *                      Linked-list node registers configurations.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_List_BuildNode(DMA_NodeConfTypeDef *pNodeConfig, DMA_NodeTypeDef *pNode)
{
  uint32_t index = 0U;
  int32_t blockoffset;

  /* Check the parameters */
  if ((pNodeConfig == NULL) || (pNode == NULL))
  {
    return HAL_ERROR;
  }

  /* Add node type assert */
  assert_param(IS_DMA_NODE_TYPE(pNodeConfig->NodeType));


  /* Update CTR1 register value ***********************************************/
  /* Check the parameters */
  assert_param(IS_DMA_SOURCE_INC            (pNodeConfig->Init.SrcInc));
  assert_param(IS_DMA_DESTINATION_INC       (pNodeConfig->Init.DestInc));
  assert_param(IS_DMA_SOURCE_DATA_WIDTH     (pNodeConfig->Init.SrcDataWidth));
  assert_param(IS_DMA_DESTINATION_DATA_WIDTH(pNodeConfig->Init.DestDataWidth));
  assert_param(IS_DMA_DATA_ALIGNMENT        (pNodeConfig->DataHandlingConfig.DataAlignment));

  /* Prepare DMA Channel Transfer Register (CTR1) value */
  pNode->LinkRegisters[index] = pNodeConfig->Init.DestInc                     | pNodeConfig->Init.DestDataWidth | \
                                pNodeConfig->DataHandlingConfig.DataAlignment | pNodeConfig->Init.SrcInc        | \
                                pNodeConfig->Init.SrcDataWidth;

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
  if (pNodeConfig->SrcSecure == DMA_CHANNEL_SRC_SEC)
  {
    pNode->LinkRegisters[index] |= DMA_CTR1_SSEC;
  }

  if (pNodeConfig->DestSecure == DMA_CHANNEL_DEST_SEC)
  {
    pNode->LinkRegisters[index] |= DMA_CTR1_DSEC;
  }
#endif /* defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U) */

  /* Add parameters related to GPDMA configuration */
  if ((pNodeConfig->NodeType & DMA_CHANNEL_TYPE_GPDMA) == DMA_CHANNEL_TYPE_GPDMA)
  {
    /* Check the parameters */
    assert_param(IS_DMA_BURST_LENGTH           (pNodeConfig->Init.SrcBurstLength));
    assert_param(IS_DMA_BURST_LENGTH           (pNodeConfig->Init.DestBurstLength));
    assert_param(IS_DMA_DATA_EXCHANGE          (pNodeConfig->DataHandlingConfig.DataExchange));
    assert_param(IS_DMA_TRANSFER_ALLOCATED_PORT(pNodeConfig->Init.TransferAllocatedPort));

    pNode->LinkRegisters[index] |= (pNodeConfig->Init.TransferAllocatedPort                                            | \
                                   pNodeConfig->DataHandlingConfig.DataExchange                                        | \
                                   (((pNodeConfig->Init.DestBurstLength - 1U) << DMA_CTR1_DBL_1_Pos) & DMA_CTR1_DBL_1) | \
                                   (((pNodeConfig->Init.SrcBurstLength - 1U) << DMA_CTR1_SBL_1_Pos) & DMA_CTR1_SBL_1));
  }

  /* Increment index to point to next register */
  index++;


  /* Update CTR2 register value ***********************************************/
  /* Check the parameters */
  assert_param(IS_DMA_REQUEST         (pNodeConfig->Init.Request));
  assert_param(IS_DMA_DIRECTION       (pNodeConfig->Init.Direction));
  assert_param(IS_DMA_TCEM_EVENT_MODE (pNodeConfig->Init.TransferEventMode));
  assert_param(IS_DMA_BLOCK_HW_REQUEST(pNodeConfig->Init.BlkHWRequest));

  /* Prepare DMA Channel Transfer Register 2 (CTR2) value */
  pNode->LinkRegisters[index] = pNodeConfig->Init.TransferEventMode | \
                                (pNodeConfig->Init.Request & (DMA_CTR2_REQSEL | DMA_CTR2_SWREQ));

  /* Memory to Peripheral transfer */
  if ((pNodeConfig->Init.Direction) == DMA_MEMORY_TO_PERIPH)
  {
    if ((pNodeConfig->NodeType & DMA_CHANNEL_TYPE_GPDMA) == DMA_CHANNEL_TYPE_GPDMA)
    {
      pNode->LinkRegisters[index] |= DMA_CTR2_DREQ;
    }
  }
  /* Memory to Memory transfer */
  else if ((pNodeConfig->Init.Direction) == DMA_MEMORY_TO_MEMORY)
  {
    pNode->LinkRegisters[index] |= DMA_CTR2_SWREQ;
  }
  else
  {
    /* Nothing to do */
  }

  /* Check if trigger feature is active */
  if (pNodeConfig->TriggerConfig.TriggerPolarity != DMA_TRIG_POLARITY_MASKED)
  {
    assert_param(IS_DMA_TRIGGER_POLARITY (pNodeConfig->TriggerConfig.TriggerPolarity));
    assert_param(IS_DMA_TRIGGER_MODE     (pNodeConfig->TriggerConfig.TriggerMode));
    assert_param(IS_DMA_TRIGGER_SELECTION(pNodeConfig->TriggerConfig.TriggerSelection));

    pNode->LinkRegisters[index] |= pNodeConfig->TriggerConfig.TriggerMode     | \
                                   pNodeConfig->TriggerConfig.TriggerPolarity | \
                                  ((pNodeConfig->TriggerConfig.TriggerSelection << DMA_CTR2_TRIGSEL_Pos) & DMA_CTR2_TRIGSEL);
  }

  /* Increment index to point to next register */
  index++;


  /* Update CBR1 register value ***********************************************/
  /* Set the new CBR1 Register value */
  pNode->LinkRegisters[index] = (pNodeConfig->DataSize & DMA_CBR1_BNDT);

  /* If 2D Addressing is supported by current channel */
  if ((pNodeConfig->NodeType & DMA_CHANNEL_TYPE_2D_ADDR) == DMA_CHANNEL_TYPE_2D_ADDR)
  {
    /* Check the parameters */
    assert_param(IS_DMA_REPEAT_COUNT     (pNodeConfig->RepeatBlockConfig.RepeatCount));
    assert_param(IS_DMA_BURST_ADDR_OFFSET(pNodeConfig->RepeatBlockConfig.SrcAddrOffset));
    assert_param(IS_DMA_BURST_ADDR_OFFSET(pNodeConfig->RepeatBlockConfig.DestAddrOffset));
    assert_param(IS_DMA_BLOCK_ADDR_OFFSET(pNodeConfig->RepeatBlockConfig.BlkSrcAddrOffset));
    assert_param(IS_DMA_BLOCK_ADDR_OFFSET(pNodeConfig->RepeatBlockConfig.BlkDestAddrOffset));

    /* Set the new CBR1 Register value */
    pNode->LinkRegisters[index] |= (((pNodeConfig->RepeatBlockConfig.RepeatCount - 1U) << DMA_CBR1_BRC_Pos) & DMA_CBR1_BRC);

    /* If the source address offset is negative, set SDEC bit */
    if (pNodeConfig->RepeatBlockConfig.SrcAddrOffset < 0)
    {
      pNode->LinkRegisters[index] |= DMA_CBR1_SDEC;
    }
    else
    {
      pNode->LinkRegisters[index] &= (~DMA_CBR1_SDEC);
    }

    /* If the destination address offset is negative, set DDEC bit */
    if (pNodeConfig->RepeatBlockConfig.DestAddrOffset < 0)
    {
      pNode->LinkRegisters[index] |= DMA_CBR1_DDEC;
    }
    else
    {
      pNode->LinkRegisters[index] &= (~DMA_CBR1_DDEC);
    }

    /* If the repeated block source address offset is negative, set BRSEC bit */
    if (pNodeConfig->RepeatBlockConfig.BlkSrcAddrOffset < 0)
    {
      pNode->LinkRegisters[index] |= DMA_CBR1_BRSDEC;
    }
    else
    {
      pNode->LinkRegisters[index] &= (~DMA_CBR1_BRSDEC);
    }

    /* if the repeated block destination address offset is negative, set BRDEC bit */
    if (pNodeConfig->RepeatBlockConfig.BlkDestAddrOffset < 0)
    {
      pNode->LinkRegisters[index] |= DMA_CBR1_BRDDEC;
    }
    else
    {
      pNode->LinkRegisters[index] &= (~DMA_CBR1_BRDDEC);
    }
  }

  /* Increment index to point to next register */
  index++;


  /* Update CSAR register value ***********************************************/
  pNode->LinkRegisters[index] = pNodeConfig->SrcAddress;

  /* Increment index to point to next register */
  index++;


  /* Update CDAR register value ***********************************************/
  pNode->LinkRegisters[index] = pNodeConfig->DstAddress;

  /* Increment index to point to next register */
  index++;


  /* Check if the selected channel is 2D addressing */
  if ((pNodeConfig->NodeType & DMA_CHANNEL_TYPE_2D_ADDR) == DMA_CHANNEL_TYPE_2D_ADDR)
  {
    /* Update CTR3 register value ***********************************************/
    /* Check the parameters */
    assert_param(IS_DMA_BURST_ADDR_OFFSET(pNodeConfig->RepeatBlockConfig.SrcAddrOffset));
    assert_param(IS_DMA_BURST_ADDR_OFFSET(pNodeConfig->RepeatBlockConfig.DestAddrOffset));

    /* Write new CTR3 Register value : source address offset */
    if (pNodeConfig->RepeatBlockConfig.SrcAddrOffset < 0)
    {
      blockoffset = (- pNodeConfig->RepeatBlockConfig.SrcAddrOffset);
      pNode->LinkRegisters[index] = ((uint32_t)blockoffset & DMA_CTR3_SAO);
    }
    else
    {
      pNode->LinkRegisters[index] = ((uint32_t)pNodeConfig->RepeatBlockConfig.SrcAddrOffset & DMA_CTR3_SAO);
    }

    /* Write new CTR3 Register value : destination address offset */
    if (pNodeConfig->RepeatBlockConfig.DestAddrOffset < 0)
    {
      blockoffset = (- pNodeConfig->RepeatBlockConfig.DestAddrOffset);
      pNode->LinkRegisters[index] |= (((uint32_t)blockoffset << DMA_CTR3_DAO_Pos) & DMA_CTR3_DAO);
    }
    else
    {
      pNode->LinkRegisters[index] |= (((uint32_t)pNodeConfig->RepeatBlockConfig.DestAddrOffset << DMA_CTR3_DAO_Pos) & DMA_CTR3_DAO);
    }

    /* Increment index to point to next register */
    index++;


    /* Update CBR2 register value ***********************************************/
    /* Check the parameters */
    assert_param(IS_DMA_BLOCK_ADDR_OFFSET(pNodeConfig->RepeatBlockConfig.BlkSrcAddrOffset));
    assert_param(IS_DMA_BLOCK_ADDR_OFFSET(pNodeConfig->RepeatBlockConfig.BlkDestAddrOffset));

    /* Write new CBR2 Register value : repeated block source address offset */
    if (pNodeConfig->RepeatBlockConfig.BlkSrcAddrOffset < 0)
    {
      blockoffset = (- pNodeConfig->RepeatBlockConfig.BlkSrcAddrOffset);
      pNode->LinkRegisters[index] = ((uint32_t)blockoffset & DMA_CBR2_BRSAO);
    }
    else
    {
      pNode->LinkRegisters[index] = ((uint32_t)pNodeConfig->RepeatBlockConfig.BlkSrcAddrOffset & DMA_CBR2_BRSAO);
    }

    /* Write new CBR2 Register value : repeated block destination address offset */
    if (pNodeConfig->RepeatBlockConfig.BlkDestAddrOffset < 0)
    {
      blockoffset = (- pNodeConfig->RepeatBlockConfig.BlkDestAddrOffset);
      pNode->LinkRegisters[index] |= (((uint32_t)blockoffset & DMA_CBR2_BRSAO) << DMA_CBR2_BRDAO_Pos);
    }
    else
    {
      pNode->LinkRegisters[index] |= (((uint32_t)pNodeConfig->RepeatBlockConfig.BlkDestAddrOffset << DMA_CBR2_BRDAO_Pos) & DMA_CBR2_BRDAO);
    }

    /* Increment index to point to next register */
    index++;
  }

  /* Set node information */
  pNode->NodeInfo = pNodeConfig->NodeType;
  if ((pNodeConfig->NodeType & DMA_CHANNEL_TYPE_2D_ADDR) == DMA_CHANNEL_TYPE_2D_ADDR)
  {
    pNode->NodeInfo |= (7U << NODE_CLLR_IDX_Pos);
  }
  else
  {
    pNode->NodeInfo |= (5U << NODE_CLLR_IDX_Pos);
  }

  return HAL_OK;
}

/**
  * @brief  Get node configuration.
  * @param  pNodeConfig: Pointer to a DMA_NodeConfTypeDef structure that contains
  *                      the configuration information for the specified DMA
  *                      Linked-List Node.
  * @param  pNode:       Pointer to a DMA_NodeTypeDef structure that contains
  *                      Linked-list node registers configurations.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_List_GetNodeConfig(DMA_NodeConfTypeDef *pNodeConfig, DMA_NodeTypeDef *pNode)
{
  uint16_t offset;

  /* Check the parameters */
  if ((pNodeConfig == NULL) || (pNode == NULL))
  {
    return HAL_ERROR;
  }

  /* Get node information *****************************************************/
  pNodeConfig->NodeType = (pNode->NodeInfo & NODE_TYPE_MASK);


  /* Get CTR1 fields **********************************************************/
  pNodeConfig->Init.SrcInc                         = pNode->LinkRegisters[0] & DMA_CTR1_SINC;
  pNodeConfig->Init.DestInc                        = pNode->LinkRegisters[0] & DMA_CTR1_DINC;
  pNodeConfig->Init.SrcDataWidth                   = pNode->LinkRegisters[0] & DMA_CTR1_SDW_LOG2;
  pNodeConfig->Init.DestDataWidth                  = pNode->LinkRegisters[0] & DMA_CTR1_DDW_LOG2;
  pNodeConfig->Init.SrcBurstLength                 = ((pNode->LinkRegisters[0] & DMA_CTR1_SBL_1) >> DMA_CTR1_SBL_1_Pos) + 1U;
  pNodeConfig->Init.DestBurstLength                = ((pNode->LinkRegisters[0] & DMA_CTR1_DBL_1) >> DMA_CTR1_DBL_1_Pos) + 1U;
  pNodeConfig->Init.TransferAllocatedPort          = pNode->LinkRegisters[0] & (DMA_CTR1_SAP | DMA_CTR1_DAP);
  pNodeConfig->DataHandlingConfig.DataExchange     = pNode->LinkRegisters[0] & (DMA_CTR1_SBX | DMA_CTR1_DBX | DMA_CTR1_DHX);
  pNodeConfig->DataHandlingConfig.DataAlignment    = pNode->LinkRegisters[0] & DMA_CTR1_PAM;
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
  /* Set node attribute parameters */
  pNodeConfig->SrcSecure                           = pNode->LinkRegisters[0] & DMA_CTR1_SSEC;
  pNodeConfig->DestSecure                          = pNode->LinkRegisters[0] & DMA_CTR1_DSEC;
#endif /* defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U) */


  /* Get CTR2 fields **********************************************************/
  /* Get direction and request */
  if ((pNode->LinkRegisters[1] & DMA_CTR2_SWREQ) != 0U)
  {
    pNodeConfig->Init.Request   = DMA_REQUEST_SW;
    pNodeConfig->Init.Direction = DMA_MEMORY_TO_MEMORY;
  }
  else
  {
    pNodeConfig->Init.Request   = DMA_CTR2_REQSEL;

    if ((pNode->LinkRegisters[1] & DMA_CTR2_DREQ) != 0U)
    {
      pNodeConfig->Init.Direction = DMA_MEMORY_TO_PERIPH;
    }
    else
    {
      pNodeConfig->Init.Direction = DMA_PERIPH_TO_MEMORY;
    }
  }

  /* Get block hardware request */
  pNodeConfig->Init.BlkHWRequest              = (pNode->LinkRegisters[1] & DMA_CTR2_BREQ);
  pNodeConfig->TriggerConfig.TriggerMode      = pNode->LinkRegisters[1] & DMA_CTR2_TRIGM;
  pNodeConfig->TriggerConfig.TriggerPolarity  = pNode->LinkRegisters[1] & DMA_CTR2_TRIGPOL;
  pNodeConfig->TriggerConfig.TriggerSelection = (pNode->LinkRegisters[1] & DMA_CTR2_TRIGSEL) >> DMA_CTR2_TRIGSEL_Pos;
  pNodeConfig->Init.TransferEventMode         = pNode->LinkRegisters[1] & DMA_CTR2_TCEM;

  /* Get CBR1 fields **********************************************************/
  pNodeConfig->DataSize = pNode->LinkRegisters[2] & DMA_CBR1_BNDT;

  if (pNodeConfig->NodeType == DMA_GPDMA_2D_NODE)
  {
    pNodeConfig->RepeatBlockConfig.RepeatCount = ((pNode->LinkRegisters[2] & DMA_CBR1_BRC) >> DMA_CBR1_BRC_Pos) + 1U;
  }
  else
  {
    pNodeConfig->RepeatBlockConfig.RepeatCount = 1U;
  }

  /* Get CSAR field ***********************************************************/
  pNodeConfig->SrcAddress = pNode->LinkRegisters[3];


  /* Get CDAR field ***********************************************************/
  pNodeConfig->DstAddress = pNode->LinkRegisters[4];


  if (pNodeConfig->NodeType == DMA_GPDMA_2D_NODE)
  {
    /* Get CTR3 field *********************************************************/
    offset = (uint16_t)(pNode->LinkRegisters[5] & DMA_CTR3_SAO);
    pNodeConfig->RepeatBlockConfig.SrcAddrOffset  = (int32_t)offset;

    offset = (uint16_t)((pNode->LinkRegisters[5] & DMA_CTR3_DAO) >> DMA_CTR3_DAO_Pos);
    pNodeConfig->RepeatBlockConfig.DestAddrOffset = (int32_t)offset;

    if ((pNode->LinkRegisters[2] & DMA_CBR1_SDEC) != 0U)
    {
      pNodeConfig->RepeatBlockConfig.SrcAddrOffset *= (-1);
    }

    if ((pNode->LinkRegisters[2] & DMA_CBR1_DDEC) != 0U)
    {
      pNodeConfig->RepeatBlockConfig.DestAddrOffset *= (-1);
    }

    /* Get CBR2 fields ********************************************************/
    offset = (uint16_t)(pNode->LinkRegisters[6] & DMA_CBR2_BRSAO);
    pNodeConfig->RepeatBlockConfig.BlkSrcAddrOffset = (int32_t)offset;

    offset = (uint16_t)((pNode->LinkRegisters[6] & DMA_CBR2_BRDAO) >> DMA_CBR2_BRDAO_Pos);
    pNodeConfig->RepeatBlockConfig.BlkDestAddrOffset = (int32_t)offset;

    if ((pNode->LinkRegisters[2] & DMA_CBR1_BRSDEC) != 0U)
    {
      pNodeConfig->RepeatBlockConfig.BlkSrcAddrOffset *= (-1);
    }

    if ((pNode->LinkRegisters[2] & DMA_CBR1_BRDDEC) != 0U)
    {
      pNodeConfig->RepeatBlockConfig.BlkDestAddrOffset *= (-1);
    }
  }
  else
  {
    /* Get CTR3 field *********************************************************/
    pNodeConfig->RepeatBlockConfig.SrcAddrOffset     = 0;
    pNodeConfig->RepeatBlockConfig.DestAddrOffset    = 0;

    /* Get CBR2 fields ********************************************************/
    pNodeConfig->RepeatBlockConfig.BlkSrcAddrOffset  = 0;
    pNodeConfig->RepeatBlockConfig.BlkDestAddrOffset = 0;
  }

  return HAL_OK;
}

/**
  * @brief  Insert new node to linked-list queue.
  * @param  pQueueLinkList: Pointer to a DMA_QListTypeDef structure that
  *                         contains queue informations.
  * @param  pPrevNode:      Pointer to a DMA_NodeTypeDef structure that contains
  *                         linked-list node registers configurations.
  * @param  pNewNode:       Pointer to a DMA_NodeTypeDef structure that contains
  *                         linked-list node registers configurations.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_List_InsertNode(DMA_QListTypeDef *pQueueLinkList, DMA_NodeTypeDef *pPrevNode, DMA_NodeTypeDef *pNewNode)
{
  uint32_t regnode_number, cllr_mask, temp, count = pQueueLinkList->NodeNumber, Address = 0U;

  /* Check if no valid tail is selected and no new valid node is selected */
  if ((pQueueLinkList == NULL) || (pNewNode == NULL))
  {
    return HAL_ERROR;
  }

  /* Check queue state */
  if (pQueueLinkList->State == HAL_DMA_QUEUE_STATE_BUSY)
  {
    pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_BUSY;
    return HAL_ERROR;
  }
  else
  {
    /* Set queue state */
    pQueueLinkList->State = HAL_DMA_QUEUE_STATE_BUSY;

    /* Check if nodes are in the same memory base address */
    temp = (((uint32_t)pQueueLinkList->Head | (uint32_t)pPrevNode | (uint32_t)pNewNode) & DMA_CLBAR_LBA);
    if (temp != ((uint32_t)pNewNode & DMA_CLBAR_LBA))
    {
      pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_OUTOFRANGE;
      return HAL_ERROR;
    }

    /* Check if nodes are compatible with queue type */
    if (pQueueLinkList->Head != NULL)
    {
      if (pPrevNode != NULL)
      {
        temp = ((uint32_t)pQueueLinkList->Head->NodeInfo & NODE_TYPE_MASK)| (pPrevNode->NodeInfo & NODE_TYPE_MASK) | (pNewNode->NodeInfo & NODE_TYPE_MASK);
      }
      else
      {
        temp = ((uint32_t)pQueueLinkList->Head->NodeInfo & NODE_TYPE_MASK) | (pNewNode->NodeInfo & NODE_TYPE_MASK);
      }

      if (temp != (pNewNode->NodeInfo & NODE_TYPE_MASK))
      {
        pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;
        return HAL_ERROR;
      }
    }

    /* Update CLLR and register number for 2D addressing node */
    if ((pNewNode->NodeInfo & DMA_CHANNEL_TYPE_2D_ADDR) == DMA_CHANNEL_TYPE_2D_ADDR)
    {
      cllr_mask = DMA_CLLR_UT1 | DMA_CLLR_UT2 | DMA_CLLR_UB1 | DMA_CLLR_USA | \
                  DMA_CLLR_UDA | DMA_CLLR_UT3 | DMA_CLLR_UB2 | DMA_CLLR_ULL;
      regnode_number = 8U;
    }
    /* Update CLLR and register number for linear addressing node */
    else
    {
      cllr_mask = DMA_CLLR_UT1 | DMA_CLLR_UT2 | DMA_CLLR_UB1 | DMA_CLLR_USA | \
                  DMA_CLLR_UDA | DMA_CLLR_ULL;
      regnode_number = 6U;
    }

    /* Empty Queue ************************************************************/
    if (pQueueLinkList->Head == NULL)
    {
      if (pPrevNode == NULL)
      {
        pQueueLinkList->Head                      = pNewNode;
        pQueueLinkList->NodeNumber                = 1U;
      }
      else
      {
        pQueueLinkList->Head                          = pPrevNode;
        pPrevNode->LinkRegisters[regnode_number - 1U] = ((uint32_t)pNewNode & DMA_CLLR_LA) | cllr_mask;
        pQueueLinkList->NodeNumber                    = 2U;
      }
    }
    /* Not Empty Queue **********************************************************/
    else
    {
      /* Check if previous node is empty */
      if (pPrevNode == NULL)
      {
        pNewNode->LinkRegisters[regnode_number - 1U] = ((uint32_t)pQueueLinkList->Head & DMA_CLLR_LA) | cllr_mask;
        pQueueLinkList->Head                         = pNewNode;
      }
      /* Check if previous node is not empty */
      else
      {
        /* Check that previous node is linked to the selected Queue */
        while ((count != 0U) && (Address != ((uint32_t)pPrevNode & DMA_CLLR_LA)))
        {
          if (count == pQueueLinkList->NodeNumber)
          {
            Address = (uint32_t)pQueueLinkList->Head & DMA_CLLR_LA;
          }
          else
          {
            Address = ((DMA_NodeTypeDef *)(Address + ((uint32_t)pQueueLinkList->Head & DMA_CLBAR_LBA)))->LinkRegisters[regnode_number - 1U] & DMA_CLLR_LA;
          }

          /* Decrement counter */
          count--;
        }

        if (Address == ((uint32_t)pPrevNode & DMA_CLLR_LA))
        {
          if (count == 0U)
          {
            pPrevNode->LinkRegisters[regnode_number - 1U] = ((uint32_t)pNewNode & DMA_CLLR_LA) | cllr_mask;
          }
          else
          {
            pNewNode->LinkRegisters[regnode_number - 1U] = pPrevNode->LinkRegisters[regnode_number - 1U];

            pPrevNode->LinkRegisters[regnode_number - 1U] = ((uint32_t)pNewNode & DMA_CLLR_LA) | cllr_mask;
          }
        }
        else
        {
          pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_NOTFOUND;
          return HAL_ERROR;
        }
      }

      pQueueLinkList->NodeNumber++;
    }
  }

  /* Update queue state and error code */
  pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;
  pQueueLinkList->State     = HAL_DMA_QUEUE_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  Insert new node at the head of linked-list queue.
  * @param  pQueueLinkList: Pointer to a DMA_QListTypeDef structure that
  *                         contains queue informations.
  * @param  pNewNode:       Pointer to a DMA_NodeTypeDef structure that contains
  *                         linked-list node registers configurations.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_List_InsertNode_Head(DMA_QListTypeDef *pQueueLinkList, DMA_NodeTypeDef *pNewNode)
{
  uint32_t regnode_number, cllr_mask;

  /* Check if no valid tail is selected and no new valid node is selected */
  if ((pQueueLinkList == NULL) || (pNewNode == NULL))
  {
    return HAL_ERROR;
  }

  /* Check queue state */
  if (pQueueLinkList->State == HAL_DMA_QUEUE_STATE_BUSY)
  {
    pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_BUSY;
    return HAL_ERROR;
  }
  else
  {
    /* Set queue state */
    pQueueLinkList->State = HAL_DMA_QUEUE_STATE_BUSY;

    if (pQueueLinkList->Head != NULL)
    {
      /* Check if nodes are in the same memory base address */
      if (((uint32_t)pQueueLinkList->Head & DMA_CLBAR_LBA) != ((uint32_t)pNewNode & DMA_CLBAR_LBA))
      {
        pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_OUTOFRANGE;
        return HAL_ERROR;
      }

      /* Check if nodes are compatible with queue type */
      if ((pQueueLinkList->Head->NodeInfo & NODE_TYPE_MASK) != (pNewNode->NodeInfo & NODE_TYPE_MASK))
      {
        pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;
        return HAL_ERROR;
      }
    }

    /* Empty Queue ************************************************************/
    if (pQueueLinkList->Head == NULL)
    {
      pQueueLinkList->Head       = pNewNode;
    }
    /* Not Empty Queue **********************************************************/
    else
    {
      /* Update CLLR and register number for 2D addressing node */
      if ((pNewNode->NodeInfo & DMA_CHANNEL_TYPE_2D_ADDR) == DMA_CHANNEL_TYPE_2D_ADDR)
      {
        cllr_mask = DMA_CLLR_UT1 | DMA_CLLR_UT2 | DMA_CLLR_UB1 | DMA_CLLR_USA | \
                    DMA_CLLR_UDA | DMA_CLLR_UT3 | DMA_CLLR_UB2 | DMA_CLLR_ULL;
        regnode_number = 8U;
      }
      /* Update CLLR and register number for linear addressing node */
      else
      {
        cllr_mask = DMA_CLLR_UT1 | DMA_CLLR_UT2 | DMA_CLLR_UB1 | DMA_CLLR_USA | \
                    DMA_CLLR_UDA | DMA_CLLR_ULL;
        regnode_number = 6U;
      }

      pNewNode->LinkRegisters[regnode_number - 1U] = ((uint32_t)pQueueLinkList->Head & DMA_CLLR_LA) | cllr_mask;
      pQueueLinkList->Head                         = pNewNode;
    }

    pQueueLinkList->NodeNumber++;
  }

  /* Update queue state and error code */
  pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;
  pQueueLinkList->State     = HAL_DMA_QUEUE_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  Insert new node at the tail linked-list queue.
  * @param  pQueueLinkList: Pointer to a DMA_QListTypeDef structure that
  *                         contains queue informations.
  * @param  pNewNode:       Pointer to a DMA_NodeTypeDef structure that contains
  *                         linked-list node registers configurations.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_List_InsertNode_Tail(DMA_QListTypeDef *pQueueLinkList, DMA_NodeTypeDef *pNewNode)
{
  uint32_t regnode_number, cllr_mask, count = pQueueLinkList->NodeNumber, Address = 0U;

  /* Check if no valid tail is selected and no new valid node is selected */
  if ((pQueueLinkList == NULL) || (pNewNode == NULL))
  {
    return HAL_ERROR;
  }

  /* Check queue state */
  if (pQueueLinkList->State == HAL_DMA_QUEUE_STATE_BUSY)
  {
    pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_BUSY;
    return HAL_ERROR;
  }
  else
  {
    /* Set queue state */
    pQueueLinkList->State = HAL_DMA_QUEUE_STATE_BUSY;

    if (pQueueLinkList->Head != NULL)
    {
      /* Check if nodes are in the same memory base address */
      if (((uint32_t)pQueueLinkList->Head & DMA_CLBAR_LBA) != ((uint32_t)pNewNode & DMA_CLBAR_LBA))
      {
        pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_OUTOFRANGE;
        return HAL_ERROR;
      }

      /* Check if nodes are compatible with queue type */
      if ((pQueueLinkList->Head->NodeInfo & NODE_TYPE_MASK) != (pNewNode->NodeInfo & NODE_TYPE_MASK))
      {
        pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;
        return HAL_ERROR;
      }
    }

    /* Empty Queue ************************************************************/
    if (pQueueLinkList->Head == NULL)
    {
      pQueueLinkList->Head = pNewNode;
    }
    /* Not Empty Queue **********************************************************/
    else
    {
      /* Update CLLR and register number for 2D addressing node */
      if ((pNewNode->NodeInfo & DMA_CHANNEL_TYPE_2D_ADDR) == DMA_CHANNEL_TYPE_2D_ADDR)
      {
        cllr_mask = DMA_CLLR_UT1 | DMA_CLLR_UT2 | DMA_CLLR_UB1 | DMA_CLLR_USA | \
                    DMA_CLLR_UDA | DMA_CLLR_UT3 | DMA_CLLR_UB2 | DMA_CLLR_ULL;
        regnode_number = 8U;
      }
      /* Update CLLR and register number for linear addressing node */
      else
      {
        cllr_mask = DMA_CLLR_UT1 | DMA_CLLR_UT2 | DMA_CLLR_UB1 | DMA_CLLR_USA | \
                    DMA_CLLR_UDA | DMA_CLLR_ULL;
        regnode_number = 6U;
      }

      /* Get last node address */
      while (count != 0U)
      {
        if (count == pQueueLinkList->NodeNumber)
        {
          Address = (uint32_t)pQueueLinkList->Head & DMA_CLLR_LA;
        }
        else
        {
          Address = ((DMA_NodeTypeDef *)(Address + ((uint32_t)pQueueLinkList->Head & DMA_CLBAR_LBA)))->LinkRegisters[regnode_number - 1U] & DMA_CLLR_LA;
        }

        /* Decrement counter */
        count--;
      }

      ((DMA_NodeTypeDef *)(Address + ((uint32_t)pQueueLinkList->Head & DMA_CLBAR_LBA)))->LinkRegisters[regnode_number - 1U] = ((uint32_t)pNewNode & DMA_CLLR_LA) | cllr_mask;
    }

    pQueueLinkList->NodeNumber++;
  }

  /* Update queue state and error code */
  pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;
  pQueueLinkList->State     = HAL_DMA_QUEUE_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  Remove a node from linked-list queue.
  * @param  pQueueLinkList: Pointer to a DMA_QListTypeDef structure that
  *                         contains queue informations.
  * @param  pNode:          Pointer to a DMA_NodeTypeDef structure that contains
  *                         linked-list node registers configurations.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_List_RemoveNode(DMA_QListTypeDef *pQueueLinkList, DMA_NodeTypeDef *pNode)
{
  uint32_t prev_addr = 0U, current_addr = 0U, regnode_number, count = pQueueLinkList->NodeNumber;

  /* Check if no valid tail is selected and no new valid node is selected */
  if ((pQueueLinkList == NULL) || (pNode == NULL))
  {
    return HAL_ERROR;
  }

  /* Check the Queue */
  if (pQueueLinkList->Head == NULL)
  {
    pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_EMPTY;
    return HAL_ERROR;
  }

  /* Check queue state */
  if (pQueueLinkList->State == HAL_DMA_QUEUE_STATE_BUSY)
  {
    pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_BUSY;
    return HAL_ERROR;
  }
  else
  {
    /* Set queue state */
    pQueueLinkList->State = HAL_DMA_QUEUE_STATE_BUSY;

    /* Update register number */
    if ((pNode->NodeInfo & DMA_CHANNEL_TYPE_2D_ADDR) == DMA_CHANNEL_TYPE_2D_ADDR)
    {
      regnode_number = 8U; /* 2D addressing node contains 8 registers */
    }
    else
    {
      regnode_number = 6U; /* Linear addressing node contains 6 registers */
    }

    /* Check that node to be removed is linked to the selected Queue */
    while ((count != 0U) && (current_addr != ((uint32_t)pNode & DMA_CLLR_LA)))
    {
      if (count == pQueueLinkList->NodeNumber)
      {
        current_addr = (uint32_t)pQueueLinkList->Head & DMA_CLLR_LA;
      }
      else
      {
        prev_addr = current_addr;
        current_addr = ((DMA_NodeTypeDef *)(current_addr + ((uint32_t)pQueueLinkList->Head & DMA_CLBAR_LBA)))->LinkRegisters[regnode_number - 1U] & DMA_CLLR_LA;
      }

      /* Decrement counter */
      count--;
    }

    /* Remove selected node */
    if (current_addr == ((uint32_t)pNode & DMA_CLLR_LA))
    {
      /* Removed node is the head node */
      if (count == (pQueueLinkList->NodeNumber - 1U))
      {
        pQueueLinkList->Head = (DMA_NodeTypeDef *)(((uint32_t)pQueueLinkList->Head & DMA_CLBAR_LBA) + (pNode->LinkRegisters[regnode_number - 1U] & DMA_CLLR_LA));
        pNode->LinkRegisters[regnode_number - 1U] = 0U;
      }
      /* Removed node is in the middle */
      else if (count > 0U)
      {
        ((DMA_NodeTypeDef *)(((uint32_t)pQueueLinkList->Head & DMA_CLBAR_LBA) + prev_addr))->LinkRegisters[regnode_number - 1U] = pNode->LinkRegisters[regnode_number - 1U];
        pNode->LinkRegisters[regnode_number - 1U] = 0U;
      }
      /* Removed node is the last */
      else
      {
        ((DMA_NodeTypeDef *)(((uint32_t)pQueueLinkList->Head & DMA_CLBAR_LBA) + prev_addr))->LinkRegisters[regnode_number - 1U] = 0U;
      }

      /* Decrement node number */
      pQueueLinkList->NodeNumber--;
    }
    else
    {
      pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_NOTFOUND;
      return HAL_ERROR;
    }
  }

  /* Update queue state and error code */
  pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;
  pQueueLinkList->State     = HAL_DMA_QUEUE_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  Remove the head node from linked-list queue.
  * @param  pQueueLinkList: Pointer to a DMA_QListTypeDef structure that
  *                         contains queue informations.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_List_RemoveNode_Head(DMA_QListTypeDef *pQueueLinkList)
{
  uint32_t Address, regnode_number;

  /* Check if no valid tail is selected and no new valid node is selected */
  if (pQueueLinkList == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the Queue */
  if (pQueueLinkList->Head == NULL)
  {
    pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_EMPTY;
    return HAL_ERROR;
  }

  /* Check queue state */
  if (pQueueLinkList->State == HAL_DMA_QUEUE_STATE_BUSY)
  {
    pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_BUSY;
    return HAL_ERROR;
  }
  else
  {
    /* Set queue state */
    pQueueLinkList->State = HAL_DMA_QUEUE_STATE_BUSY;

    /* Update register number */
    if ((pQueueLinkList->Head->NodeInfo & DMA_CHANNEL_TYPE_2D_ADDR) == DMA_CHANNEL_TYPE_2D_ADDR)
    {
      regnode_number = 8U; /* 2D addressing node contains 8 registers */
    }
    else
    {
      regnode_number = 6U; /* Linear addressing node contains 6 registers */
    }

    /* Remove head node */
    if (pQueueLinkList->NodeNumber == 1U)
    {
      pQueueLinkList->Head->LinkRegisters[regnode_number - 1U] = 0U;
      pQueueLinkList->FirstCircularNode = 0U;
      pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;
      pQueueLinkList->NodeNumber = 0U;
    }
    else
    {
      Address = pQueueLinkList->Head->LinkRegisters[regnode_number - 1U] & DMA_CLLR_LA;
      pQueueLinkList->Head->LinkRegisters[regnode_number - 1U] = 0U;
      pQueueLinkList->Head = ((DMA_NodeTypeDef *)(Address + ((uint32_t)pQueueLinkList->Head & DMA_CLBAR_LBA)));
    }

    /* Decrement node number */
    pQueueLinkList->NodeNumber--;
  }

  /* Update queue state and error code */
  pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;
  pQueueLinkList->State     = HAL_DMA_QUEUE_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  Remove the tail node from linked-list queue.
  * @param  pQueueLinkList: Pointer to a DMA_QListTypeDef structure that
  *                         contains queue informations.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_List_RemoveNode_Tail(DMA_QListTypeDef *pQueueLinkList)
{
  uint32_t prev_addr = 0U, current_addr = 0U, regnode_number, count = pQueueLinkList->NodeNumber;

  /* Check if no valid tail is selected and no new valid node is selected */
  if (pQueueLinkList == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the Queue */
  if (pQueueLinkList->Head == NULL)
  {
    pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_EMPTY;
    return HAL_ERROR;
  }

  /* Check queue state */
  if (pQueueLinkList->State == HAL_DMA_QUEUE_STATE_BUSY)
  {
    pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_BUSY;
    return HAL_ERROR;
  }
  else
  {
    /* Set queue state */
    pQueueLinkList->State = HAL_DMA_QUEUE_STATE_BUSY;

    /* Update register number */
    if ((pQueueLinkList->Head->NodeInfo & DMA_CHANNEL_TYPE_2D_ADDR) == DMA_CHANNEL_TYPE_2D_ADDR)
    {
      regnode_number = 8U; /* 2D addressing node contains 8 registers */
    }
    else
    {
      regnode_number = 6U; /* Linear addressing node contains 6 registers */
    }

    if (pQueueLinkList->NodeNumber == 1U)
    {
      pQueueLinkList->Head->LinkRegisters[regnode_number - 1U] = 0U;
      pQueueLinkList->FirstCircularNode = 0U;
      pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;
      pQueueLinkList->NodeNumber = 0U;
    }
    else
    {
      /* Check that node to be removed is linked to the selected Queue */
      while (count != 0U)
      {
        if (count == pQueueLinkList->NodeNumber)
        {
          current_addr = (uint32_t)pQueueLinkList->Head & DMA_CLLR_LA;
        }
        else
        {
          prev_addr = current_addr;
          current_addr = ((DMA_NodeTypeDef *)(current_addr + ((uint32_t)pQueueLinkList->Head & DMA_CLBAR_LBA)))->LinkRegisters[regnode_number - 1U] & DMA_CLLR_LA;
        }

        /* Decrement counter */
        count--;
      }

      ((DMA_NodeTypeDef *)(((uint32_t)pQueueLinkList->Head & DMA_CLBAR_LBA) + prev_addr))->LinkRegisters[regnode_number - 1U] = 0U;
    }

    /* Decrement node number */
    pQueueLinkList->NodeNumber--;
  }

  /* Update queue state and error code */
  pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;
  pQueueLinkList->State     = HAL_DMA_QUEUE_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  Replace node in linked-list queue.
  * @param  pQueueLinkList: Pointer to a DMA_QListTypeDef structure that
  *                         contains queue informations.
  * @param  pOldNode:       Pointer to a DMA_NodeTypeDef structure that contains
  *                         linked-list node registers configurations.
  * @param  pNewNode:       Pointer to a DMA_NodeTypeDef structure that contains
  *                         linked-list node registers configurations.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_List_ReplaceNode(DMA_QListTypeDef *pQueueLinkList, DMA_NodeTypeDef *pOldNode, DMA_NodeTypeDef *pNewNode)
{
  uint32_t cllr_mask, prev_addr = 0U, current_addr = 0U, regnode_number, count = pQueueLinkList->NodeNumber;

  /* Check if no valid tail is selected and no new valid node is selected */
  if ((pQueueLinkList == NULL) || (pOldNode == NULL) || (pNewNode == NULL))
  {
    return HAL_ERROR;
  }

  /* Check the Queue */
  if (pQueueLinkList->Head == NULL)
  {
    pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_EMPTY;
    return HAL_ERROR;
  }

  /* Check queue state */
  if (pQueueLinkList->State == HAL_DMA_QUEUE_STATE_BUSY)
  {
    pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_BUSY;
    return HAL_ERROR;
  }
  else
  {
    /* Set queue state */
    pQueueLinkList->State = HAL_DMA_QUEUE_STATE_BUSY;

    /* Check if queues are circular */
    if (pQueueLinkList->FirstCircularNode != NULL)
    {
      pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;
      return HAL_ERROR;
    }

    /* Check if new node is compatible with queue type */
    if (((uint32_t)pQueueLinkList->Head->NodeInfo & NODE_TYPE_MASK) != (pNewNode->NodeInfo & NODE_TYPE_MASK))
    {
      pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;
      return HAL_ERROR;
    }

    /* Update CLLR and register number for 2D addressing node */
    if ((pNewNode->NodeInfo & DMA_CHANNEL_TYPE_2D_ADDR) == DMA_CHANNEL_TYPE_2D_ADDR)
    {
      cllr_mask = DMA_CLLR_UT1 | DMA_CLLR_UT2 | DMA_CLLR_UB1 | DMA_CLLR_USA | \
                  DMA_CLLR_UDA | DMA_CLLR_UT3 | DMA_CLLR_UB2 | DMA_CLLR_ULL;
      regnode_number = 8U;
    }
    /* Update CLLR and register number for linear addressing node */
    else
    {
      cllr_mask = DMA_CLLR_UT1 | DMA_CLLR_UT2 | DMA_CLLR_UB1 | DMA_CLLR_USA | \
                  DMA_CLLR_UDA | DMA_CLLR_ULL;
      regnode_number = 6U;
    }

    /* Check that node to be removed is linked to the selected Queue */
    while ((count != 0U) && (current_addr != ((uint32_t)pOldNode & DMA_CLLR_LA)))
    {
      if (count == pQueueLinkList->NodeNumber)
      {
        current_addr = (uint32_t)pQueueLinkList->Head & DMA_CLLR_LA;
      }
      else
      {
        prev_addr = current_addr;
        current_addr = ((DMA_NodeTypeDef *)(current_addr + ((uint32_t)pQueueLinkList->Head & DMA_CLBAR_LBA)))->LinkRegisters[regnode_number - 1U] & DMA_CLLR_LA;
      }

      /* Decrement counter */
      count--;
    }

    /* Replace selected node selected node */
    if (current_addr == ((uint32_t)pOldNode & DMA_CLLR_LA))
    {
      /* Replaced node is the head node */
      if (count == (pQueueLinkList->NodeNumber - 1U))
      {
        pNewNode->LinkRegisters[regnode_number - 1U] = ((DMA_NodeTypeDef *)(current_addr + ((uint32_t)pQueueLinkList->Head & DMA_CLBAR_LBA)))->LinkRegisters[regnode_number - 1U];
        pQueueLinkList->Head = pNewNode;
        ((DMA_NodeTypeDef *)(current_addr + ((uint32_t)pQueueLinkList->Head & DMA_CLBAR_LBA)))->LinkRegisters[regnode_number - 1U] = 0U;
      }
      /* Replaced node is in the middle */
      else if (count > 0U)
      {
        ((DMA_NodeTypeDef *)(prev_addr + ((uint32_t)pQueueLinkList->Head & DMA_CLBAR_LBA)))->LinkRegisters[regnode_number - 1U]    = ((uint32_t)pNewNode & DMA_CLLR_LA) | cllr_mask;
        pNewNode->LinkRegisters[regnode_number - 1U] = ((DMA_NodeTypeDef *)(current_addr + ((uint32_t)pQueueLinkList->Head & DMA_CLBAR_LBA)))->LinkRegisters[regnode_number - 1U];
        ((DMA_NodeTypeDef *)(current_addr + ((uint32_t)pQueueLinkList->Head & DMA_CLBAR_LBA)))->LinkRegisters[regnode_number - 1U] = 0U;
      }
      /* Replaced node is the last */
      else
      {
        ((DMA_NodeTypeDef *)(prev_addr + ((uint32_t)pQueueLinkList->Head & DMA_CLBAR_LBA)))->LinkRegisters[regnode_number - 1U] = ((uint32_t)pNewNode & DMA_CLLR_LA) | cllr_mask;
      }
    }
    else
    {
      pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_NOTFOUND;
      return HAL_ERROR;
    }
  }

  /* Update queue state and error code */
  pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;
  pQueueLinkList->State     = HAL_DMA_QUEUE_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  Replace the head node of linked-list queue.
  * @param  pQueueLinkList: Pointer to a DMA_QListTypeDef structure that
  *                         contains queue informations.
  * @param  pNewNode:       Pointer to a DMA_NodeTypeDef structure that contains
  *                         linked-list node registers configurations.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_List_ReplaceNode_Head(DMA_QListTypeDef *pQueueLinkList, DMA_NodeTypeDef *pNewNode)
{
  uint32_t regnode_number;

  /* Check if no valid tail is selected and no new valid node is selected */
  if ((pQueueLinkList == NULL) || (pNewNode == NULL))
  {
    return HAL_ERROR;
  }

  /* Check the Queue */
  if (pQueueLinkList->Head == NULL)
  {
    pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_EMPTY;
    return HAL_ERROR;
  }

  /* Check queue state */
  if (pQueueLinkList->State == HAL_DMA_QUEUE_STATE_BUSY)
  {
    pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_BUSY;
    return HAL_ERROR;
  }
  else
  {
    /* Set queue state */
    pQueueLinkList->State = HAL_DMA_QUEUE_STATE_BUSY;

    /* Check if queues are circular */
    if (pQueueLinkList->FirstCircularNode != NULL)
    {
      pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;
      return HAL_ERROR;
    }

    /* Check if new node is compatible with queue type */
    if (((uint32_t)pQueueLinkList->Head->NodeInfo & NODE_TYPE_MASK) != (pNewNode->NodeInfo & NODE_TYPE_MASK))
    {
      pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;
      return HAL_ERROR;
    }

    /* Update CLLR and register number for 2D addressing node */
    if ((pNewNode->NodeInfo & DMA_CHANNEL_TYPE_2D_ADDR) == DMA_CHANNEL_TYPE_2D_ADDR)
    {
      regnode_number = 8U;
    }
    /* Update CLLR and register number for linear addressing node */
    else
    {
      regnode_number = 6U;
    }

    pNewNode->LinkRegisters[regnode_number - 1U] = pQueueLinkList->Head->LinkRegisters[regnode_number - 1U];
    pQueueLinkList->Head->LinkRegisters[regnode_number - 1U] = 0U;
    pQueueLinkList->Head = pNewNode;
  }

  /* Update queue state and error code */
  pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;
  pQueueLinkList->State     = HAL_DMA_QUEUE_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  Replace the tail node of linked-list queue.
  * @param  pQueueLinkList: Pointer to a DMA_QListTypeDef structure that
  *                         contains queue informations.
  * @param  pNewNode:       Pointer to a DMA_NodeTypeDef structure that contains
  *                         linked-list node registers configurations.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_List_ReplaceNode_Tail(DMA_QListTypeDef *pQueueLinkList, DMA_NodeTypeDef *pNewNode)
{
  uint32_t cllr_mask, prev_addr = 0U, current_addr = 0U, regnode_number, count = pQueueLinkList->NodeNumber;

  /* Check if no valid tail is selected and no new valid node is selected */
  if ((pQueueLinkList == NULL) || (pNewNode == NULL))
  {
    return HAL_ERROR;
  }

  /* Check the Queue */
  if (pQueueLinkList->Head == NULL)
  {
    pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_EMPTY;
    return HAL_ERROR;
  }

  /* Check queue state */
  if (pQueueLinkList->State == HAL_DMA_QUEUE_STATE_BUSY)
  {
    pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_BUSY;
    return HAL_ERROR;
  }
  else
  {
    /* Set queue state */
    pQueueLinkList->State = HAL_DMA_QUEUE_STATE_BUSY;

    /* Check if queues are circular */
    if (pQueueLinkList->FirstCircularNode != NULL)
    {
      pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;
      return HAL_ERROR;
    }

    /* Check if new node is compatible with queue type */
    if (((uint32_t)pQueueLinkList->Head->NodeInfo & NODE_TYPE_MASK) != (pNewNode->NodeInfo & NODE_TYPE_MASK))
    {
      pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;
      return HAL_ERROR;
    }

    /* Update CLLR and register number for 2D addressing node */
    if ((pNewNode->NodeInfo & DMA_CHANNEL_TYPE_2D_ADDR) == DMA_CHANNEL_TYPE_2D_ADDR)
    {
      cllr_mask = DMA_CLLR_UT1 | DMA_CLLR_UT2 | DMA_CLLR_UB1 | DMA_CLLR_USA | \
                  DMA_CLLR_UDA | DMA_CLLR_UT3 | DMA_CLLR_UB2 | DMA_CLLR_ULL;
      regnode_number = 8U;
    }
    /* Update CLLR and register number for linear addressing node */
    else
    {
      cllr_mask = DMA_CLLR_UT1 | DMA_CLLR_UT2 | DMA_CLLR_UB1 | DMA_CLLR_USA | \
                  DMA_CLLR_UDA | DMA_CLLR_ULL;
      regnode_number = 6U;
    }

    /* Check that node to be removed is linked to the selected Queue */
    while (count != 0U)
    {
      if (count == pQueueLinkList->NodeNumber)
      {
        current_addr = (uint32_t)pQueueLinkList->Head & DMA_CLLR_LA;
      }
      else
      {
        prev_addr = current_addr;
        current_addr = ((DMA_NodeTypeDef *)(current_addr + ((uint32_t)pQueueLinkList->Head & DMA_CLBAR_LBA)))->LinkRegisters[regnode_number - 1U] & DMA_CLLR_LA;
      }

      /* Decrement counter */
      count--;
    }

    ((DMA_NodeTypeDef *)(prev_addr + ((uint32_t)pQueueLinkList->Head & DMA_CLBAR_LBA)))->LinkRegisters[regnode_number - 1U] = ((uint32_t)pNewNode & DMA_CLLR_LA) | cllr_mask;
  }

  /* Update queue state and error code */
  pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;
  pQueueLinkList->State     = HAL_DMA_QUEUE_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  Insert a source linked-list queue to a destination linked-list queue.
  * @param  pLinkListSrcQueue:  Pointer to a DMA_QListTypeDef structure
  *                             that contains queue informations.
  * @param  pLinkListDestQueue: Pointer to a DMA_QListTypeDef structure
  *                             that contains queue informations.
  * @param  pPrevNode:          Pointer to a DMA_NodeTypeDef structure that contains
  *                             linked-list node registers configurations.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_List_InsertQ(DMA_QListTypeDef *pLinkListSrcQueue, DMA_NodeTypeDef *pPrevNode, DMA_QListTypeDef *pLinkListDestQueue)
{
  uint32_t regnode_number, cllr_mask, count, Address = 0U;

  /* Check if no valid tails are selected and no new valid node is selected */
  if ((pLinkListSrcQueue == NULL) || (pLinkListDestQueue == NULL) || (pLinkListDestQueue == NULL))
  {
    return HAL_ERROR;
  }

  /* Check the Queue */
  if (pLinkListSrcQueue->Head == NULL)
  {
    pLinkListSrcQueue->ErrorCode = HAL_DMA_QUEUE_ERROR_EMPTY;
    return HAL_ERROR;
  }

  /* Check source queue state */
  if (pLinkListSrcQueue->State == HAL_DMA_QUEUE_STATE_BUSY)
  {
    pLinkListSrcQueue->ErrorCode  = HAL_DMA_QUEUE_ERROR_BUSY;
    return HAL_ERROR;
  }

  /* Check destination queue state */
  if (pLinkListDestQueue->State == HAL_DMA_QUEUE_STATE_BUSY)
  {
    pLinkListDestQueue->ErrorCode  = HAL_DMA_QUEUE_ERROR_BUSY;
    return HAL_ERROR;
  }

  /* Set queue state */
  pLinkListSrcQueue->State  = HAL_DMA_QUEUE_STATE_BUSY;
  pLinkListDestQueue->State = HAL_DMA_QUEUE_STATE_BUSY;

  /* Check if queues are in the same memory base address */
  if (((uint32_t)pLinkListSrcQueue->Head & DMA_CLBAR_LBA) != ((uint32_t)pLinkListDestQueue->Head & DMA_CLBAR_LBA))
  {
    pLinkListSrcQueue->ErrorCode = HAL_DMA_QUEUE_ERROR_OUTOFRANGE;
    return HAL_ERROR;
  }

  /* Check that queue types are compatible */
  if ((pLinkListSrcQueue->Head->NodeInfo & NODE_TYPE_MASK) != (pLinkListDestQueue->Head->NodeInfo & NODE_TYPE_MASK))
  {
    pLinkListSrcQueue->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;
    return HAL_ERROR;
  }

  /* Check if queues are circular */
  if ((pLinkListSrcQueue->FirstCircularNode != NULL) || (pLinkListDestQueue->FirstCircularNode != NULL))
  {
    pLinkListSrcQueue->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;
    return HAL_ERROR;
  }

  /* Update CLLR and register number for 2D addressing node */
  if ((pLinkListSrcQueue->Head->NodeInfo & DMA_CHANNEL_TYPE_2D_ADDR) == DMA_CHANNEL_TYPE_2D_ADDR)
  {
    cllr_mask = DMA_CLLR_UT1 | DMA_CLLR_UT2 | DMA_CLLR_UB1 | DMA_CLLR_USA | \
      DMA_CLLR_UDA | DMA_CLLR_UT3 | DMA_CLLR_UB2 | DMA_CLLR_ULL;
    regnode_number = 8U;
  }
  /* Update CLLR and register number for linear addressing node */
  else
  {
    cllr_mask = DMA_CLLR_UT1 | DMA_CLLR_UT2 | DMA_CLLR_UB1 | DMA_CLLR_USA | \
      DMA_CLLR_UDA | DMA_CLLR_ULL;
    regnode_number = 6U;
  }

  /* Empty destination Queue ************************************************/
  if (pLinkListDestQueue->Head == NULL)
  {
    pLinkListDestQueue->Head              = pLinkListSrcQueue->Head;
    pLinkListDestQueue->NodeNumber        = pLinkListSrcQueue->NodeNumber;
  }
  /* Not Empty Queue ********************************************************/
  else
  {
    /* Check if previous node is empty */
    if (pPrevNode == NULL)
    {
      /* Update counter */
      count = pLinkListSrcQueue->NodeNumber;

      /* Check that previous node is linked to the selected Queue */
      while (count != 0U)
      {
        if (count == pLinkListSrcQueue->NodeNumber)
        {
          Address = (uint32_t)pLinkListSrcQueue->Head & DMA_CLLR_LA;
        }
        else
        {
          Address = ((DMA_NodeTypeDef *)(Address + ((uint32_t)pLinkListSrcQueue->Head & DMA_CLBAR_LBA)))->LinkRegisters[regnode_number - 1U] & DMA_CLLR_LA;
        }

        /* Decrement counter */
        count--;
      }

      /* Update source queue last node CLLR to link it with destination first node */
      ((DMA_NodeTypeDef *)(Address + ((uint32_t)pLinkListSrcQueue->Head & DMA_CLBAR_LBA)))->LinkRegisters[regnode_number - 1U] = ((uint32_t)pLinkListDestQueue->Head & DMA_CLLR_LA) | cllr_mask;

      pLinkListDestQueue->Head        = pLinkListSrcQueue->Head;
      pLinkListDestQueue->NodeNumber += pLinkListSrcQueue->NodeNumber;
      pLinkListSrcQueue->NodeNumber   = pLinkListDestQueue->NodeNumber;
    }
    else
    {
      /* Update counter */
      count = pLinkListDestQueue->NodeNumber;

      /* Check that previous node is linked to the selected Queue */
      while ((count != 0U) && (Address != ((uint32_t)pPrevNode & DMA_CLLR_LA)))
      {
        if (count == pLinkListDestQueue->NodeNumber)
        {
          Address = (uint32_t)pLinkListDestQueue->Head & DMA_CLLR_LA;
        }
        else
        {
          Address = ((DMA_NodeTypeDef *)(Address + ((uint32_t)pLinkListDestQueue->Head & DMA_CLBAR_LBA)))->LinkRegisters[regnode_number - 1U] & DMA_CLLR_LA;
        }

        /* Decrement counter */
        count--;
      }

      /* If previous node is not last destination queue node */
      if (Address == ((uint32_t)pPrevNode & DMA_CLLR_LA))
      {
        if (count == 0U)
        {
          /* Update source queue last node CLLR to link it with destination first node */
          ((DMA_NodeTypeDef *)(Address + ((uint32_t)pLinkListDestQueue->Head & DMA_CLBAR_LBA)))->LinkRegisters[regnode_number - 1U] = ((uint32_t)pLinkListSrcQueue->Head & DMA_CLLR_LA) | cllr_mask;
          pLinkListDestQueue->NodeNumber += pLinkListSrcQueue->NodeNumber;
        }
        else
        {
          /* Get next node address for destination queue and count */
          uint32_t next_address = (pPrevNode->LinkRegisters[regnode_number - 1U] & DMA_CLLR_LA);
          uint32_t remain_node  = count;
          /* Update destination queue previous node CLLR to link it with source queue first node */
          ((DMA_NodeTypeDef *)(Address + ((uint32_t)pLinkListDestQueue->Head & DMA_CLBAR_LBA)))->LinkRegisters[regnode_number - 1U] = ((uint32_t)pLinkListSrcQueue->Head & DMA_CLLR_LA) | cllr_mask;

          /* Update counter */
          count = pLinkListSrcQueue->NodeNumber;

          /* Check that previous node is linked to the selected Queue */
          while (count != 0U)
          {
            if (count == pLinkListSrcQueue->NodeNumber)
            {
              Address = (uint32_t)pLinkListSrcQueue->Head & DMA_CLLR_LA;
            }
            else
            {
              Address = ((DMA_NodeTypeDef *)(Address + ((uint32_t)pLinkListSrcQueue->Head & DMA_CLBAR_LBA)))->LinkRegisters[regnode_number - 1U] & DMA_CLLR_LA;
            }

            /* Decrement counter */
            count--;
          }

          ((DMA_NodeTypeDef *)(Address + ((uint32_t)pLinkListSrcQueue->Head & DMA_CLBAR_LBA)))->LinkRegisters[regnode_number - 1U] = next_address | cllr_mask;

          /* Update queues counter */
          pLinkListDestQueue->NodeNumber += pLinkListSrcQueue->NodeNumber;
          pLinkListSrcQueue->NodeNumber += remain_node;
        }
      }
      else
      {
        pLinkListDestQueue->ErrorCode = HAL_DMA_QUEUE_ERROR_NOTFOUND;
        return HAL_ERROR;
      }
    }
  }

  /* Update queue state and error code */
  pLinkListSrcQueue->ErrorCode  = HAL_DMA_QUEUE_ERROR_NONE;
  pLinkListSrcQueue->State      = HAL_DMA_QUEUE_STATE_READY;
  pLinkListDestQueue->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;
  pLinkListDestQueue->State     = HAL_DMA_QUEUE_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  Insert a source linked-list queue at the head of destination queue.
  * @param  pLinkListSrcQueue:  Pointer to a DMA_QListTypeDef structure
  *                             that contains queue informations.
  * @param  pLinkListDestQueue: Pointer to a DMA_QListTypeDef structure
  *                             that contains queue informations.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_List_InsertQ_Head(DMA_QListTypeDef *pLinkListSrcQueue, DMA_QListTypeDef *pLinkListDestQueue)
{
  uint32_t regnode_number, cllr_mask, count, Address = 0U;

  /* Check if no valid tails are selected and no new valid node is selected */
  if ((pLinkListSrcQueue == NULL) || (pLinkListDestQueue == NULL) || (pLinkListDestQueue == NULL))
  {
    return HAL_ERROR;
  }

  /* Check the Queue */
  if (pLinkListSrcQueue->Head == NULL)
  {
    pLinkListSrcQueue->ErrorCode = HAL_DMA_QUEUE_ERROR_EMPTY;
    return HAL_ERROR;
  }

  /* Check source queue state */
  if (pLinkListSrcQueue->State == HAL_DMA_QUEUE_STATE_BUSY)
  {
    pLinkListSrcQueue->ErrorCode  = HAL_DMA_QUEUE_ERROR_BUSY;
    return HAL_ERROR;
  }

  /* Check destination queue state */
  if (pLinkListDestQueue->State == HAL_DMA_QUEUE_STATE_BUSY)
  {
    pLinkListDestQueue->ErrorCode  = HAL_DMA_QUEUE_ERROR_BUSY;
    return HAL_ERROR;
  }

  /* Set queue state */
  pLinkListSrcQueue->State  = HAL_DMA_QUEUE_STATE_BUSY;
  pLinkListDestQueue->State = HAL_DMA_QUEUE_STATE_BUSY;

  /* Check if queues are in the same memory base address */
  if (((uint32_t)pLinkListSrcQueue->Head & DMA_CLBAR_LBA) != ((uint32_t)pLinkListDestQueue->Head & DMA_CLBAR_LBA))
  {
    pLinkListSrcQueue->ErrorCode = HAL_DMA_QUEUE_ERROR_OUTOFRANGE;
    return HAL_ERROR;
  }

  /* Check that queue types are compatible */
  if ((pLinkListSrcQueue->Head->NodeInfo & NODE_TYPE_MASK) != (pLinkListDestQueue->Head->NodeInfo & NODE_TYPE_MASK))
  {
    pLinkListSrcQueue->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;
    return HAL_ERROR;
  }

  /* Check if queues are circular */
  if ((pLinkListSrcQueue->FirstCircularNode != NULL) || (pLinkListDestQueue->FirstCircularNode != NULL))
  {
    pLinkListSrcQueue->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;
    return HAL_ERROR;
  }

  /* Update CLLR and register number for 2D addressing node */
  if ((pLinkListSrcQueue->Head->NodeInfo & DMA_CHANNEL_TYPE_2D_ADDR) == DMA_CHANNEL_TYPE_2D_ADDR)
  {
    cllr_mask = DMA_CLLR_UT1 | DMA_CLLR_UT2 | DMA_CLLR_UB1 | DMA_CLLR_USA | \
      DMA_CLLR_UDA | DMA_CLLR_UT3 | DMA_CLLR_UB2 | DMA_CLLR_ULL;
    regnode_number = 8U;
  }
  /* Update CLLR and register number for linear addressing node */
  else
  {
    cllr_mask = DMA_CLLR_UT1 | DMA_CLLR_UT2 | DMA_CLLR_UB1 | DMA_CLLR_USA | \
      DMA_CLLR_UDA | DMA_CLLR_ULL;
    regnode_number = 6U;
  }

  /* Empty destination Queue ************************************************/
  if (pLinkListDestQueue->Head == NULL)
  {
    pLinkListDestQueue->Head              = pLinkListSrcQueue->Head;
    pLinkListDestQueue->NodeNumber        = pLinkListSrcQueue->NodeNumber;
  }
  /* Not Empty Queue ********************************************************/
  else
  {
    /* Update counter */
    count = pLinkListSrcQueue->NodeNumber;

    /* Check that previous node is linked to the selected Queue */
    while (count != 0U)
    {
      if (count == pLinkListSrcQueue->NodeNumber)
      {
        Address = (uint32_t)pLinkListSrcQueue->Head & DMA_CLLR_LA;
      }
      else
      {
        Address = ((DMA_NodeTypeDef *)(Address + ((uint32_t)pLinkListSrcQueue->Head & DMA_CLBAR_LBA)))->LinkRegisters[regnode_number - 1U] & DMA_CLLR_LA;
      }

      /* Decrement counter */
      count--;
    }

    /* Update source queue last node CLLR to link it with destination first node */
    ((DMA_NodeTypeDef *)(Address + ((uint32_t)pLinkListSrcQueue->Head & DMA_CLBAR_LBA)))->LinkRegisters[regnode_number - 1U] = ((uint32_t)pLinkListDestQueue->Head & DMA_CLLR_LA) | cllr_mask;

    pLinkListDestQueue->Head        = pLinkListSrcQueue->Head;
    pLinkListDestQueue->NodeNumber += pLinkListSrcQueue->NodeNumber;
    pLinkListSrcQueue->NodeNumber   = pLinkListDestQueue->NodeNumber;
  }

  /* Update queue state and error code */
  pLinkListSrcQueue->ErrorCode  = HAL_DMA_QUEUE_ERROR_NONE;
  pLinkListSrcQueue->State      = HAL_DMA_QUEUE_STATE_READY;
  pLinkListDestQueue->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;
  pLinkListDestQueue->State     = HAL_DMA_QUEUE_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  Insert a source linked-list queue at the tail of destination queue.
  * @param  pLinkListSrcQueue:  Pointer to a DMA_QListTypeDef structure
  *                             that contains queue informations.
  * @param  pLinkListDestQueue: Pointer to a DMA_QListTypeDef structure
  *                             that contains queue informations.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_List_InsertQ_Tail(DMA_QListTypeDef *pLinkListSrcQueue, DMA_QListTypeDef *pLinkListDestQueue)
{
  uint32_t regnode_number, cllr_mask, count, Address = 0U;

  /* Check if no valid tails are selected and no new valid node is selected */
  if ((pLinkListSrcQueue == NULL) || (pLinkListDestQueue == NULL) || (pLinkListDestQueue == NULL))
  {
    return HAL_ERROR;
  }

  /* Check the Queue */
  if (pLinkListSrcQueue->Head == NULL)
  {
    pLinkListSrcQueue->ErrorCode = HAL_DMA_QUEUE_ERROR_EMPTY;
    return HAL_ERROR;
  }

  /* Check source queue state */
  if (pLinkListSrcQueue->State == HAL_DMA_QUEUE_STATE_BUSY)
  {
    pLinkListSrcQueue->ErrorCode  = HAL_DMA_QUEUE_ERROR_BUSY;
    return HAL_ERROR;
  }

  /* Check destination queue state */
  if (pLinkListDestQueue->State == HAL_DMA_QUEUE_STATE_BUSY)
  {
    pLinkListDestQueue->ErrorCode  = HAL_DMA_QUEUE_ERROR_BUSY;
    return HAL_ERROR;
  }

  /* Set queue state */
  pLinkListSrcQueue->State  = HAL_DMA_QUEUE_STATE_BUSY;
  pLinkListDestQueue->State = HAL_DMA_QUEUE_STATE_BUSY;

  /* Check if queues are in the same memory base address */
  if (((uint32_t)pLinkListSrcQueue->Head & DMA_CLBAR_LBA) != ((uint32_t)pLinkListDestQueue->Head & DMA_CLBAR_LBA))
  {
    pLinkListSrcQueue->ErrorCode = HAL_DMA_QUEUE_ERROR_OUTOFRANGE;
    return HAL_ERROR;
  }

  /* Check that queue types are compatible */
  if ((pLinkListSrcQueue->Head->NodeInfo & NODE_TYPE_MASK) != (pLinkListDestQueue->Head->NodeInfo & NODE_TYPE_MASK))
  {
    pLinkListSrcQueue->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;
    return HAL_ERROR;
  }

  /* Check if queues are circular */
  if ((pLinkListSrcQueue->FirstCircularNode != NULL) || (pLinkListDestQueue->FirstCircularNode != NULL))
  {
    pLinkListSrcQueue->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;
    return HAL_ERROR;
  }

  /* Update CLLR and register number for 2D addressing node */
  if ((pLinkListSrcQueue->Head->NodeInfo & DMA_CHANNEL_TYPE_2D_ADDR) == DMA_CHANNEL_TYPE_2D_ADDR)
  {
    cllr_mask = DMA_CLLR_UT1 | DMA_CLLR_UT2 | DMA_CLLR_UB1 | DMA_CLLR_USA | \
      DMA_CLLR_UDA | DMA_CLLR_UT3 | DMA_CLLR_UB2 | DMA_CLLR_ULL;
    regnode_number = 8U;
  }
  /* Update CLLR and register number for linear addressing node */
  else
  {
    cllr_mask = DMA_CLLR_UT1 | DMA_CLLR_UT2 | DMA_CLLR_UB1 | DMA_CLLR_USA | \
      DMA_CLLR_UDA | DMA_CLLR_ULL;
    regnode_number = 6U;
  }

  /* Empty source Queue *****************************************************/
  if (pLinkListDestQueue->Head == NULL)
  {
    pLinkListDestQueue->Head              = pLinkListSrcQueue->Head;
    pLinkListDestQueue->NodeNumber        = pLinkListSrcQueue->NodeNumber;
  }
  /* Not Empty Queue ********************************************************/
  else
  {
    /* Update counter */
    count = pLinkListDestQueue->NodeNumber;

    /* Check that previous node is linked to the selected Queue */
    while (count != 0U)
    {
      if (count == pLinkListDestQueue->NodeNumber)
      {
        Address = (uint32_t)pLinkListDestQueue->Head & DMA_CLLR_LA;
      }
      else
      {
        Address = ((DMA_NodeTypeDef *)(Address + ((uint32_t)pLinkListDestQueue->Head & DMA_CLBAR_LBA)))->LinkRegisters[regnode_number - 1U] & DMA_CLLR_LA;
      }

      /* Decrement counter */
      count--;
    }

    /* Update source queue last node CLLR to link it with destination first node */
    ((DMA_NodeTypeDef *)(Address + ((uint32_t)pLinkListDestQueue->Head & DMA_CLBAR_LBA)))->LinkRegisters[regnode_number - 1U] = ((uint32_t)pLinkListSrcQueue->Head & DMA_CLLR_LA) | cllr_mask;
    pLinkListDestQueue->NodeNumber += pLinkListSrcQueue->NodeNumber;
  }

  /* Update queue state and error code */
  pLinkListSrcQueue->ErrorCode  = HAL_DMA_QUEUE_ERROR_NONE;
  pLinkListSrcQueue->State      = HAL_DMA_QUEUE_STATE_READY;
  pLinkListDestQueue->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;
  pLinkListDestQueue->State     = HAL_DMA_QUEUE_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  Set circular mode configuration for linked-list queue.
  * @param  pQueueLinkList:     Pointer to a DMA_QListTypeDef structure
  *                             that contains queue informations.
  * @param  pFirstCircularNode: Pointer to a DMA_NodeTypeDef structure that contains
  *                             linked-list node registers configurations.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_List_SetCircularModeConfig(DMA_QListTypeDef *pQueueLinkList, DMA_NodeTypeDef *pFirstCircularNode)
{
  uint32_t circular_addr = 0U, current_addr = 0U, regnode_number, count = pQueueLinkList->NodeNumber, cllr_mask;

  /* Check if no valid tail is selected and no new valid node is selected */
  if ((pQueueLinkList == NULL) || (pFirstCircularNode == NULL))
  {
    return HAL_ERROR;
  }

  /* Check the Queue */
  if (pQueueLinkList->Head == NULL)
  {
    pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_EMPTY;
    return HAL_ERROR;
  }

  /* Check queue state */
  if (pQueueLinkList->State == HAL_DMA_QUEUE_STATE_BUSY)
  {
    pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_BUSY;
    return HAL_ERROR;
  }
  else
  {
    /* Set queue state */
    pQueueLinkList->State = HAL_DMA_QUEUE_STATE_BUSY;

    /* Check if the Queue is circular */
    if (pQueueLinkList->FirstCircularNode != NULL)
    {
      if (pQueueLinkList->FirstCircularNode == pFirstCircularNode)
      {
        return HAL_OK;
      }
      else
      {
        return HAL_ERROR;
      }
    }

    /* Update register number */
    if ((pFirstCircularNode->NodeInfo & DMA_CHANNEL_TYPE_2D_ADDR) == DMA_CHANNEL_TYPE_2D_ADDR)
    {
      cllr_mask = DMA_CLLR_UT1 | DMA_CLLR_UT2 | DMA_CLLR_UB1 | DMA_CLLR_USA | \
        DMA_CLLR_UDA | DMA_CLLR_UT3 | DMA_CLLR_UB2 | DMA_CLLR_ULL;

      regnode_number = 8U; /* 2D addressing node contains 8 registers */
    }
    else
    {
      cllr_mask = DMA_CLLR_UT1 | DMA_CLLR_UT2 | DMA_CLLR_UB1 | DMA_CLLR_USA | \
        DMA_CLLR_UDA | DMA_CLLR_ULL;

      regnode_number = 6U; /* Linear addressing node contains 6 registers */
    }

    /* Check that previous node is linked to the selected Queue */
    while (count != 0U)
    {
      if (count == pQueueLinkList->NodeNumber)
      {
        current_addr = (uint32_t)pQueueLinkList->Head & DMA_CLLR_LA;

        /* Store first circular node address */
        if (current_addr == ((uint32_t)pFirstCircularNode & DMA_CLLR_LA))
        {
          circular_addr = current_addr;
        }
      }
      else
      {
        current_addr = ((DMA_NodeTypeDef *)(current_addr + ((uint32_t)pQueueLinkList->Head & DMA_CLBAR_LBA)))->LinkRegisters[regnode_number - 1U] & DMA_CLLR_LA;

        /* Store first circular node address */
        if (current_addr == ((uint32_t)pFirstCircularNode & DMA_CLLR_LA))
        {
          circular_addr = current_addr;
        }
      }

      /* Decrement counter */
      count--;
    }

    /* First circular node doesn't exist in selected Queue */
    if (circular_addr == 0U)
    {
      pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_NOTFOUND;
      return HAL_ERROR;
    }

    /* Set circular mode */
    ((DMA_NodeTypeDef *)(current_addr + ((uint32_t)pQueueLinkList->Head & DMA_CLBAR_LBA)))->LinkRegisters[regnode_number - 1U] = \
     (circular_addr + ((uint32_t)pQueueLinkList->Head & DMA_CLBAR_LBA)) | cllr_mask;

    /* Update linked-list circular state */
    pQueueLinkList->FirstCircularNode = ((DMA_NodeTypeDef *)(circular_addr + ((uint32_t)pQueueLinkList->Head & DMA_CLBAR_LBA)));
  }

  /* Update queue state and error code */
  pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;
  pQueueLinkList->State     = HAL_DMA_QUEUE_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  Set circular mode for linked-list queue.
  * @param  pQueueLinkList:     Pointer to a DMA_QListTypeDef structure
  *                             that contains queue informations.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_List_SetCircularMode(DMA_QListTypeDef *pQueueLinkList)
{
  uint32_t current_addr = 0U, regnode_number, cllr_mask, count = pQueueLinkList->NodeNumber;

  /* Check if no valid tail is selected and no new valid node is selected */
  if (pQueueLinkList == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the Queue */
  if (pQueueLinkList->Head == NULL)
  {
    pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_EMPTY;
    return HAL_ERROR;
  }

  /* Check queue state */
  if (pQueueLinkList->State == HAL_DMA_QUEUE_STATE_BUSY)
  {
    pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_BUSY;
    return HAL_ERROR;
  }
  else
  {
    /* Set queue state */
    pQueueLinkList->State = HAL_DMA_QUEUE_STATE_BUSY;

    /* Check if the Queue is circular */
    if (pQueueLinkList->FirstCircularNode != NULL)
    {
      if (pQueueLinkList->FirstCircularNode == pQueueLinkList->Head)
      {
        return HAL_OK;
      }
      else
      {
        return HAL_ERROR;
      }
    }

    /* Update CLLR and register number for 2D addressing node */
    if ((pQueueLinkList->Head->NodeInfo & DMA_CHANNEL_TYPE_2D_ADDR) == DMA_CHANNEL_TYPE_2D_ADDR)
    {
      cllr_mask = DMA_CLLR_UT1 | DMA_CLLR_UT2 | DMA_CLLR_UB1 | DMA_CLLR_USA | \
                  DMA_CLLR_UDA | DMA_CLLR_UT3 | DMA_CLLR_UB2 | DMA_CLLR_ULL;
      regnode_number = 8U;
    }
    /* Update CLLR and register number for linear addressing node */
    else
    {
      cllr_mask = DMA_CLLR_UT1 | DMA_CLLR_UT2 | DMA_CLLR_UB1 | DMA_CLLR_USA | \
                  DMA_CLLR_UDA | DMA_CLLR_ULL;
      regnode_number = 6U;
    }

    /* Check that previous node is linked to the selected Queue */
    while (count != 0U)
    {
      if (count == pQueueLinkList->NodeNumber)
      {
        current_addr = (uint32_t)pQueueLinkList->Head & DMA_CLLR_LA;
      }
      else
      {
        current_addr = ((DMA_NodeTypeDef *)(current_addr + ((uint32_t)pQueueLinkList->Head & DMA_CLBAR_LBA)))->LinkRegisters[regnode_number - 1U] & DMA_CLLR_LA;
      }

      /* Decrement counter */
      count--;
    }

    /* Set circular mode */
    ((DMA_NodeTypeDef *)(current_addr + ((uint32_t)pQueueLinkList->Head & DMA_CLBAR_LBA)))->LinkRegisters[regnode_number - 1U] = \
      ((uint32_t)pQueueLinkList->Head & DMA_CLLR_LA) | cllr_mask;

    /* Update linked-list circular state */
    pQueueLinkList->FirstCircularNode = pQueueLinkList->Head;
  }

  /* Update queue state and error code */
  pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;
  pQueueLinkList->State     = HAL_DMA_QUEUE_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  Clear circular mode for linked-list queue.
  * @param  pQueueLinkList:     Pointer to a DMA_QListTypeDef structure
  *                             that contains queue informations.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_List_ClearCircularMode(DMA_QListTypeDef *pQueueLinkList)
{
  uint32_t current_addr = 0U, regnode_number, count = pQueueLinkList->NodeNumber;

  /* Check if no valid tail is selected and no new valid node is selected */
  if (pQueueLinkList == NULL)
  {
    return HAL_ERROR;
  }

  /* Check if queue is empty */
  if (pQueueLinkList->Head == NULL)
  {
    pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_EMPTY;
    return HAL_ERROR;
  }

  /* Check linked-list circular state */
  if (pQueueLinkList->FirstCircularNode == NULL)
  {
    return HAL_OK;
  }

  /* Check queue state */
  if (pQueueLinkList->State == HAL_DMA_QUEUE_STATE_BUSY)
  {
    pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_BUSY;
    return HAL_ERROR;
  }
  else
  {
    /* Set queue state */
    pQueueLinkList->State = HAL_DMA_QUEUE_STATE_BUSY;

    /* Update register number */
    if ((pQueueLinkList->Head->NodeInfo & DMA_CHANNEL_TYPE_2D_ADDR) == DMA_CHANNEL_TYPE_2D_ADDR)
    {
      regnode_number = 8U; /* 2D addressing node contains 8 registers */
    }
    else
    {
      regnode_number = 6U; /* Linear addressing node contains 6 registers */
    }

    /* Check that previous node is linked to the selected Queue */
    while (count != 0U)
    {
      if (count == pQueueLinkList->NodeNumber)
      {
        current_addr = (uint32_t)pQueueLinkList->Head & DMA_CLLR_LA;
      }
      else
      {
        current_addr = ((DMA_NodeTypeDef *)(current_addr + ((uint32_t)pQueueLinkList->Head & DMA_CLBAR_LBA)))->LinkRegisters[regnode_number - 1U] & DMA_CLLR_LA;
      }

      /* Decrement counter */
      count--;
    }

    /* Clear circular mode */
    ((DMA_NodeTypeDef *)(current_addr + ((uint32_t)pQueueLinkList->Head & DMA_CLBAR_LBA)))->LinkRegisters[regnode_number - 1U] = 0U;

    /* Update linked-list circular state */
    pQueueLinkList->FirstCircularNode = NULL;
  }

  /* Update queue state and error code */
  pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_NONE;
  pQueueLinkList->State     = HAL_DMA_QUEUE_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  Link linked-list queue to a DMA channel.
  * @param  pQueueLinkList: Pointer to a DMA_QListTypeDef structure
  *                         that contains queue informations.
  * @param  hdma:           Pointer to a DMA_HandleTypeDef structure that contains
  *                         the configuration information for the specified DMA Channel.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_List_LinkQ(DMA_HandleTypeDef *hdma, DMA_QListTypeDef *pQueueLinkList)
{
  /* Check if no valid tail is selected and no new valid node is selected */
  if ((hdma == NULL) || (pQueueLinkList == NULL))
  {
    return HAL_ERROR;
  }

  /* Check queue state */
  if (pQueueLinkList->State == HAL_DMA_QUEUE_STATE_BUSY)
  {
    pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_BUSY;
    return HAL_ERROR;
  }

  /* Check DMA state */
  if (hdma->State == HAL_DMA_STATE_READY)
  {
    /* Check linearity compatibility */
    if ((IS_DMA_2D_ADDRESSING_INSTANCE(hdma->Instance) == 0U) && ((pQueueLinkList->Head->NodeInfo & DMA_CHANNEL_TYPE_2D_ADDR) == DMA_CHANNEL_TYPE_2D_ADDR))
    {
      pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_UNSUPPORTED;
      return HAL_ERROR;
    }

    /* Check circular compatibility */
    if (hdma->Mode == DMA_LINKEDLIST_CIRCULAR)
    {
      if (pQueueLinkList->FirstCircularNode == NULL)
      {
        pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;
        return HAL_ERROR;
      }
    }
    else
    {
      if (pQueueLinkList->FirstCircularNode != NULL)
      {
        pQueueLinkList->ErrorCode = HAL_DMA_QUEUE_ERROR_INVALIDTYPE;
        return HAL_ERROR;
      }
    }

    /* Register Queue to DMA handle */
    hdma->LinkedListQueue = pQueueLinkList;
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
  * @brief  Unlink linked-list queue from a DMA channel.
  * @param  hdma:           Pointer to a DMA_HandleTypeDef structure that contains
  *                         the configuration information for the specified DMA Channel.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_List_UnLinkQ(DMA_HandleTypeDef *hdma)
{
  /* Check if no valid tail is selected and no new valid node is selected */
  if (hdma == NULL)
  {
    return HAL_ERROR;
  }

  /* Check DMA state */
  if (hdma->State == HAL_DMA_STATE_READY)
  {
    /* Register Queue to DMA handle */
    hdma->LinkedListQueue = NULL;
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
  * @brief  Convert a linked-list queue to dynamic.
  * @param  pQueueLinkList: Pointer to a DMA_QListTypeDef structure
  *                         that contains queue informations.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_List_ConvertQToDynamic(DMA_QListTypeDef *pQueueLinkList)
{
  uint32_t node_count = (pQueueLinkList->NodeNumber - 1U), current_reg_counter = 0U, next_reg_counter, regnode_number;
  uint32_t current_node_addr = (uint32_t)pQueueLinkList->Head, next_node_addr;
  DMA_NodeTypeDef dummy_node = {0};
  uint32_t update_link[8] = {DMA_CLLR_UT1, DMA_CLLR_UT2, DMA_CLLR_UB1, DMA_CLLR_USA, DMA_CLLR_UDA, DMA_CLLR_UT3, DMA_CLLR_UB2, DMA_CLLR_ULL};

  /* Check if a valid Queue */
  if (pQueueLinkList == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the Queue parameter */
  if (pQueueLinkList->Head == NULL)
  {
    return HAL_ERROR;
  }

  /* Check if Queue is dynamic */
  if (pQueueLinkList->Type == QUEUE_TYPE_DYNAMIC)
  {
    return HAL_OK;
  }

  /* Update register number */
  if ((pQueueLinkList->Head->NodeInfo & DMA_CHANNEL_TYPE_2D_ADDR) == DMA_CHANNEL_TYPE_2D_ADDR)
  {
    regnode_number = 8U; /* 2D addressing node contains 8 registers */
  }
  else
  {
    regnode_number = 6U; /* Linear addressing node contains 6 registers */
    update_link[5] = update_link[7];
  }

    /* Store register value */
    while (current_reg_counter != regnode_number)
    {
      dummy_node.LinkRegisters[current_reg_counter] = ((DMA_NodeTypeDef *)current_node_addr)->LinkRegisters[current_reg_counter];
      current_reg_counter++;
    }

    /* Get next node */
    next_node_addr = ((uint32_t)current_node_addr & DMA_CLBAR_LBA) | (((DMA_NodeTypeDef *)current_node_addr)->LinkRegisters[regnode_number - 1U] & DMA_CLLR_LA);

    /* Repeat for all Queue nodes */
    while (node_count != 0U)
    {
      current_reg_counter = 0U;
      next_reg_counter    = 0U;

      /* Repeat for all node regiters */
      while (current_reg_counter != regnode_number)
      {
        /* Check if register number is CSAR or CDAR */
        if ((current_reg_counter == 3U) || (current_reg_counter == 4U))
        {
          dummy_node.LinkRegisters[current_reg_counter] = ((DMA_NodeTypeDef *)next_node_addr)->LinkRegisters[current_reg_counter];
          next_reg_counter++;
        }
        else
        {
          /* Compare node registers */
          if (dummy_node.LinkRegisters[current_reg_counter] != ((DMA_NodeTypeDef *)next_node_addr)->LinkRegisters[current_reg_counter])
          {
            dummy_node.LinkRegisters[current_reg_counter] = ((DMA_NodeTypeDef *)next_node_addr)->LinkRegisters[current_reg_counter];
            next_reg_counter++;
          }
          else
          {
            /* Update CLLR update fields */
            ((DMA_NodeTypeDef *)current_node_addr)->LinkRegisters[((((DMA_NodeTypeDef *)current_node_addr)->NodeInfo & NODE_CLLR_IDX) >> 8U)] &= ~update_link[current_reg_counter];
          }
        }

        /* Check for last node register to avoid data overlaps */
        if (current_reg_counter != (regnode_number - 1U))
        {
          /* Update node register */
          ((DMA_NodeTypeDef *)next_node_addr)->LinkRegisters[next_reg_counter] = ((DMA_NodeTypeDef *)next_node_addr)->LinkRegisters[current_reg_counter + 1U];
        }

        /* Increment register counter */
        current_reg_counter++;
      }

      /* Update CLLR index */
      ((DMA_NodeTypeDef *)next_node_addr)->NodeInfo &= ~NODE_CLLR_IDX;
      ((DMA_NodeTypeDef *)next_node_addr)->NodeInfo |= ((next_reg_counter - 1U) << 8U);

      /* Clear unused regiter for next node */
      while (next_reg_counter != regnode_number)
      {
        ((DMA_NodeTypeDef *)next_node_addr)->LinkRegisters[next_reg_counter] = 0U;
        next_reg_counter++;
      }

      /* Update current node, next node and node counter for next iteration */
      current_node_addr = next_node_addr;
      next_node_addr = (next_node_addr & DMA_CLBAR_LBA) | (dummy_node.LinkRegisters[regnode_number - 1U] & DMA_CLLR_LA);
      node_count--;
    }

    /* Set queue type */
    pQueueLinkList->Type = QUEUE_TYPE_DYNAMIC;

  return HAL_OK;
}

/**
  * @brief  Convert a linked-list queue to static.
  * @param  pQueueLinkList: Pointer to a DMA_QListTypeDef structure
  *                         that contains queue informations.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_List_ConvertQToStatic (DMA_QListTypeDef *pQueueLinkList)
{
  uint32_t node_count = (pQueueLinkList->NodeNumber - 1U), current_reg_counter = 0U, next_reg_counter, regnode_number;
  uint32_t current_node_addr = (uint32_t)pQueueLinkList->Head, next_node_addr;
  DMA_NodeTypeDef dummy_node = {0};
  uint32_t update_link[8] = {DMA_CLLR_UT1, DMA_CLLR_UT2, DMA_CLLR_UB1, DMA_CLLR_USA, DMA_CLLR_UDA, DMA_CLLR_UT3, DMA_CLLR_UB2, DMA_CLLR_ULL};

  /* Check if a valid Queue */
  if (pQueueLinkList == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the Queue parameter */
  if (pQueueLinkList->Head == NULL)
  {
    return HAL_ERROR;
  }

  /* Check if Queue is dynamic */
  if (pQueueLinkList->Type == QUEUE_TYPE_STATIC)
  {
    return HAL_OK;
  }

  /* Update register number */
  if ((pQueueLinkList->Head->NodeInfo & DMA_CHANNEL_TYPE_2D_ADDR) == DMA_CHANNEL_TYPE_2D_ADDR)
  {
    regnode_number = 8U; /* 2D addressing node contains 8 registers */
  }
  else
  {
    regnode_number = 6U; /* Linear addressing node contains 6 registers */
    update_link[5] = update_link[7];
  }

  /* Store register value */
  while (current_reg_counter != regnode_number)
  {
    dummy_node.LinkRegisters[current_reg_counter] = ((DMA_NodeTypeDef *)current_node_addr)->LinkRegisters[current_reg_counter];
    current_reg_counter++;
  }

  /* Get next node */
  next_node_addr = ((uint32_t)current_node_addr & DMA_CLBAR_LBA) | (((DMA_NodeTypeDef *)current_node_addr)->LinkRegisters[regnode_number - 1U] & DMA_CLLR_LA);

  /* Repeat for all Queue nodes */
  while (node_count != 0U)
  {
    current_reg_counter = 0U;
    next_reg_counter    = 0U;

    /* Repeat for all node regiters */
    while (current_reg_counter != regnode_number)
    {
      /* Check if register is updated on next node */
      if ((((DMA_NodeTypeDef *)current_node_addr)->LinkRegisters[regnode_number - 1U] & update_link[current_reg_counter]) == update_link[current_reg_counter])
      {
        dummy_node.LinkRegisters[current_reg_counter] = ((DMA_NodeTypeDef *)next_node_addr)->LinkRegisters[next_reg_counter];
        next_reg_counter++;
      }

      current_reg_counter++;
    }

    /* Update CLLR register for current node */
    ((DMA_NodeTypeDef *)current_node_addr)->LinkRegisters[current_reg_counter - 1U] |= (DMA_CLLR_UT1 | DMA_CLLR_UT2 | DMA_CLLR_UB1 | DMA_CLLR_USA | DMA_CLLR_UDA | DMA_CLLR_UT3 | DMA_CLLR_UB2 | DMA_CLLR_ULL);

    /* Rebuild next node */
    current_reg_counter = 0U;
    while (current_reg_counter != regnode_number)
    {
      ((DMA_NodeTypeDef *)next_node_addr)->LinkRegisters[current_reg_counter] = dummy_node.LinkRegisters[current_reg_counter];
      current_reg_counter++;
    }

    /* Update next node and node counter for next iteration */
    current_node_addr = next_node_addr;
    next_node_addr = (next_node_addr & DMA_CLBAR_LBA) | (dummy_node.LinkRegisters[regnode_number - 1U] & DMA_CLLR_LA);
    node_count--;
  }

  /* Set queue type */
  pQueueLinkList->Type = QUEUE_TYPE_STATIC;

  return HAL_OK;
}
/**
  * @}
  */


/** @addtogroup DMAEx_Exported_Functions_Group4
  *
@verbatim
 ===============================================================================
  #####  Data handling, repeated block and trigger configuration functions ####
 ===============================================================================
    [..]
    This section provides functions allowing to configure advanced DMA channel
    features.

@endverbatim
  * @{
  */
/**
  * @brief  DMA channel data handling configuration.
  * @param  hdma:                Pointer to a DMA_HandleTypeDef structure that contains
  *                              the configuration information for the specified DMA Channel.
  * @param  sConfigDataHandling: Pointer to a DMA_DataHandlingConfTypeDef structure that contains
  *                              the data handling configuration.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_ConfigDataHandling(DMA_HandleTypeDef *hdma, DMA_DataHandlingConfTypeDef *sConfigDataHandling)
{
  /* Check parameters */
  if ((hdma == NULL) || (sConfigDataHandling == NULL))
  {
    return HAL_ERROR;
  }

  /* Check parameters */
  assert_param(IS_DMA_DATA_ALIGNMENT(sConfigDataHandling->DataAlignment));
  assert_param(IS_DMA_DATA_EXCHANGE (sConfigDataHandling->DataExchange));

  if (hdma->State == HAL_DMA_STATE_READY)
  {
    MODIFY_REG(hdma->Instance->CTR1,(DMA_CTR1_DHX | DMA_CTR1_DBX | DMA_CTR1_SBX | DMA_CTR1_PAM),
               (sConfigDataHandling->DataAlignment | sConfigDataHandling->DataExchange));
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
  * @brief  DMA channel trigger configuration.
  * @param  hdma:           Pointer to a DMA_HandleTypeDef structure that contains
  *                         the configuration information for the specified DMA Channel.
  * @param  sConfigTrigger: Pointer to a DMA_TriggerConfTypeDef structure that contains
  *                         the trigger configuration.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_ConfigTrigger(DMA_HandleTypeDef *hdma, DMA_TriggerConfTypeDef *sConfigTrigger)
{
  /* Check parameters */
  if ((hdma == NULL) || (sConfigTrigger == NULL))
  {
    return HAL_ERROR;
  }

  /* Check parameters */
  assert_param(IS_DMA_ALL_INSTANCE     (hdma->Instance));
  assert_param(IS_DMA_TRIGGER_POLARITY (sConfigTrigger->TriggerPolarity));
  assert_param(IS_DMA_TRIGGER_MODE     (sConfigTrigger->TriggerMode));
  assert_param(IS_DMA_TRIGGER_SELECTION(sConfigTrigger->TriggerSelection));

  if (hdma->State == HAL_DMA_STATE_READY)
  {
    MODIFY_REG(hdma->Instance->CTR2,(DMA_CTR2_TRIGPOL | DMA_CTR2_TRIGSEL | DMA_CTR2_TRIGM),
               (sConfigTrigger->TriggerPolarity | sConfigTrigger->TriggerMode | \
               (sConfigTrigger->TriggerSelection << DMA_CTR2_TRIGSEL_Pos)));
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
  * @brief  DMA channel repeated block configuration.
  * @param  hdma:               Pointer to a DMA_HandleTypeDef structure that contains
  *                             the configuration information for the specified DMA Channel.
  * @param  sConfigRepeatBlock: Pointer to a DMA_RepeatBlockConfTypeDef structure that contains
  *                             the repeated block configuration.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_ConfigRepeatBlock(DMA_HandleTypeDef *hdma, DMA_RepeatBlockConfTypeDef *sConfigRepeatBlock)
{
  uint32_t tmpreg1, tmpreg2;

  /* Check parameters */
  if ((hdma == NULL) || (sConfigRepeatBlock == NULL))
  {
    return HAL_ERROR;
  }

  /* Check parameters */
  assert_param(IS_DMA_2D_ADDRESSING_INSTANCE (hdma->Instance));
  assert_param(IS_DMA_REPEAT_COUNT           (sConfigRepeatBlock->RepeatCount));
  assert_param(IS_DMA_REPEAT_COUNT           (sConfigRepeatBlock->RepeatCount));
  assert_param(IS_DMA_BURST_ADDR_OFFSET      (sConfigRepeatBlock->SrcAddrOffset));
  assert_param(IS_DMA_BURST_ADDR_OFFSET      (sConfigRepeatBlock->DestAddrOffset));
  assert_param(IS_DMA_BLOCK_ADDR_OFFSET      (sConfigRepeatBlock->BlkSrcAddrOffset));
  assert_param(IS_DMA_BLOCK_ADDR_OFFSET      (sConfigRepeatBlock->BlkDestAddrOffset));

  if (hdma->State == HAL_DMA_STATE_READY)
  {
    tmpreg1 = ((sConfigRepeatBlock->RepeatCount - 1U) << DMA_CBR1_BRC_Pos);

    if (sConfigRepeatBlock->DestAddrOffset < 0)
    {
      tmpreg1 |= DMA_CBR1_DDEC;
      tmpreg2 = (uint32_t)(- sConfigRepeatBlock->DestAddrOffset);
      tmpreg2 = tmpreg2 << DMA_CTR3_DAO_Pos;
    }
    else
    {
      tmpreg2 = ((uint32_t)sConfigRepeatBlock->DestAddrOffset << DMA_CTR3_DAO_Pos);
    }

    if (sConfigRepeatBlock->SrcAddrOffset < 0)
    {
      tmpreg1 |= DMA_CBR1_SDEC;
      tmpreg2 |= (uint32_t)(- sConfigRepeatBlock->SrcAddrOffset);
    }
    else
    {
      tmpreg2 |= (uint32_t)sConfigRepeatBlock->SrcAddrOffset;
    }

    /* Write DMA Channel Transfer Register 3 (CTR3) */
    WRITE_REG(hdma->Instance->CTR3, tmpreg2);

    if (sConfigRepeatBlock->BlkDestAddrOffset < 0)
    {
      tmpreg1 |= DMA_CBR1_BRDDEC;
      tmpreg2 = (uint32_t)(- sConfigRepeatBlock->BlkDestAddrOffset);
      tmpreg2 = tmpreg2 << DMA_CBR2_BRDAO_Pos;
    }
    else
    {
      tmpreg2 = ((uint32_t)sConfigRepeatBlock->BlkDestAddrOffset << DMA_CBR2_BRDAO_Pos);
    }

    if (sConfigRepeatBlock->BlkSrcAddrOffset < 0)
    {
      tmpreg1 |= DMA_CBR1_BRSDEC;
      tmpreg2 |= (uint32_t)(- sConfigRepeatBlock->BlkSrcAddrOffset);
    }
    else
    {
      tmpreg2 |= (uint32_t)sConfigRepeatBlock->BlkSrcAddrOffset;
    }

    /* Write DMA Channel block register 2 (CBR2) */
    WRITE_REG(hdma->Instance->CBR2, tmpreg2);

    /* Write DMA Channel block register 1 (CBR1) */
    WRITE_REG(hdma->Instance->CBR1, tmpreg1);
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
  * @}
  */


/** @addtogroup DMAEx_Exported_Functions_Group5
  *
@verbatim
 ===============================================================================
               #####  Suspend and resume operation functions #####
 ===============================================================================
    [..]
    This section provides functions allowing to supend and resume DMA channel
    operation in polling and interrupt modes.

@endverbatim
  * @{
  */

/**
  * @brief  Suspend the DMA Transfer.
  * @param  hdma: Pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA Channel.
  *
  * @note   After suspending a DMA Channel, a check for wait until the DMA Channel is
  *         effectively suspended is added. If a Channel is suspended
  *         while a data transfer is ongoing, the current data will be transferred
  *         and the Channel will be effectively suspended only after the transfer of
  *         this single data is finished.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_Suspend(DMA_HandleTypeDef *hdma)
{
  uint32_t tickstart = HAL_GetTick();

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

    /* Check if the DMA Channel is in IDLE state */
    while ((hdma->Instance->CSR & DMA_CSR_SUSPF) != 0U)
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

        /* Return error status */
        return HAL_ERROR;
      }

      /* Change DMA peripheral state */
      hdma->State = HAL_DMA_STATE_SUSPEND;
    }
  }

  return HAL_OK;
}

/**
  * @brief  Suspend the DMA Transfer in Interrupt mode.
  * @param  hdma: Pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA Channel.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_Suspend_IT(DMA_HandleTypeDef *hdma)
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

    /* Process Unlocked */
    __HAL_UNLOCK(hdma);

    /* Return error status */
    return HAL_ERROR;
  }
  else
  {
    /* Suspend the channel and activate suspend interrupt */
    hdma->Instance->CCR |= (DMA_CCR_SUSP | DMA_CCR_SUSPIE);
  }

  return HAL_OK;
}

/**
  * @brief  Resume the DMA Transfer.
  * @param  hdma: Pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA Channel.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_Resume(DMA_HandleTypeDef *hdma)
{
  /* Check the DMA peripheral handle */
  if (hdma == NULL)
  {
    /* Return error status */
    return HAL_ERROR;
  }

  if (hdma->State != HAL_DMA_STATE_SUSPEND)
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
    /* Resume the channel */
    hdma->Instance->CCR &= (~DMA_CCR_SUSP);

    /* Clear the Suspend flag */
    hdma->Instance->CFCR |= DMA_CFCR_SUSPF;

    /* Change DMA peripheral state */
    hdma->State = HAL_DMA_STATE_BUSY;
  }

  return HAL_OK;
}
/**
  * @}
  */

/** @addtogroup DMAEx_Exported_Functions_Group5
  *
@verbatim
 ===============================================================================
                        #####  Fifo status function #####
 ===============================================================================
    [..]
    This section provides function allowing to get DMA channel fifo level.

@endverbatim
  * @{
  */

/**
  * @brief  Get the FIFO level.
  * @param  hdma: Pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA Channel.
  * @retval HAL status.
  */
uint32_t HAL_DMAEx_GetFifoLevel(DMA_HandleTypeDef *hdma)
{
  return ((hdma->Instance->CSR & DMA_CSR_FIFOL) >> DMA_CSR_FIFOL_Pos);
}
/**
  * @}
  */

/**
  * @}
  */


/** @addtogroup DMAEx_Private_Functions
  * @{
  */

/**
  * @brief  Initialize the DMA handle according to the specified
  *         parameters in the DMA_InitTypeDef.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA Channel.
  * @retval None
  */
static void DMA_LinkedList_Init(DMA_HandleTypeDef *hdma)
{
  uint32_t tmpreg;

  /* Prepare DMA Channel Control Register (CCR) value *************************/
  tmpreg = hdma->InitLinkedList.Priority | hdma->InitLinkedList.LinkStepMode | hdma->InitLinkedList.LinkAllocatedPort;

  /* Write DMA Channel Control Register (CCR) */
  MODIFY_REG(hdma->Instance->CCR, DMA_CCR_PRIO | DMA_CCR_LAP | DMA_CCR_LSM, tmpreg);

  /* Write DMA Channel Control Register (CTR1) */
  WRITE_REG(hdma->Instance->CTR1, 0U);

  /* Write DMA Channel Control Register (CTR2) */
  WRITE_REG(hdma->Instance->CTR2, hdma->InitLinkedList.TransferEventMode);

  /* Write DMA Channel Control Register (CBR1) */
  WRITE_REG(hdma->Instance->CBR1, 0U);

  /* Write DMA Channel Control Register (CSAR) */
  WRITE_REG(hdma->Instance->CSAR, 0U);

  /* Write DMA Channel Control Register (CDAR) */
  WRITE_REG(hdma->Instance->CDAR, 0U);

  /* If 2D Addressing is supported by current channel */
  if (IS_DMA_2D_ADDRESSING_INSTANCE(hdma->Instance) != 0U)
  {
    /* Write DMA Channel Control Register (CTR3) */
    WRITE_REG(hdma->Instance->CTR3, 0U);

    /* Write DMA Channel Control Register (CBR2) */
    WRITE_REG(hdma->Instance->CBR2, 0U);
  }

  /* Write DMA Channel linked-list address register (CLLR) */
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
