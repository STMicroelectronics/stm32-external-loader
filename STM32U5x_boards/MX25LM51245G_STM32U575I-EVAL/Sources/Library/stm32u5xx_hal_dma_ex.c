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
/* Exported functions ---------------------------------------------------------*/

/** @addtogroup DMAEx_Exported_Functions
  * @{
  */


/** @addtogroup DMAEx_Exported_Functions_Group1
  *
@verbatim
 ===============================================================================
                      #####  Linked list operation functions  #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Create a linked-list node
      (+) Add a node to the DMA linked-list
      (+) Remove a node from the DMA linked-list
      (+) Enable/Disable linked-list circular mode
@endverbatim
  * @{
  */
/**
  * @brief  Create a DMA Link Node according to the specified parameters in the
  *         pNodeConfig.
  * @param  pNode:       Pointer to a DMA_LinkNodeTypeDef structure that contains
  *                      Linked-list node registers configurations.
  * @param  pNodeConfig: Pointer to a DMA_LinkNodeConfTypeDef structure that contains
  *                      the configuration information for the specified DMA Linked-List Node.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_LinkedList_CreateNode(DMA_HandleTypeDef *hdma, DMA_LinkNodeTypeDef *pNode, DMA_LinkNodeConfTypeDef *pNodeConfig)
{
  uint32_t index = 0U;
  uint32_t blockoffset;

  /* Check the DMA peripheral state */
  if ((hdma == NULL) || (pNode == NULL) || (pNodeConfig == NULL))
  {
    /* Return error status */
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_DMA_ALL_INSTANCE(hdma->Instance));

  /* Update CTR1 register value ***********************************************/
  if (pNodeConfig->ContainCTR1 != 0U)
  {
    /* Check the parameters */
    assert_param(IS_DMA_SOURCE_INC(pNodeConfig->Init.SrcInc));
    assert_param(IS_DMA_DESTINATION_INC(pNodeConfig->Init.DestInc));
    assert_param(IS_DMA_SOURCE_DATA_WIDTH(pNodeConfig->Init.SrcDataWidth));
    assert_param(IS_DMA_DESTINATION_DATA_WIDTH(pNodeConfig->Init.DestDataWidth));
    assert_param(IS_DMA_DATA_ALIGNMENT(pNodeConfig->Init.DataAlignment));

    /* Prepare DMA Channel Transfer Register (CTR1) value */
    pNode->LinkRegisters[index] = pNodeConfig->Init.DestInc | pNodeConfig->Init.DestDataWidth | pNodeConfig->Init.DataAlignment | \
                                  pNodeConfig->Init.SrcInc  | pNodeConfig->Init.SrcDataWidth;

    /* Add parameters related to GPDMA configuration */
    if (IS_GPDMA_INSTANCE(hdma->Instance) != 0U)
    {
      /* Check the parameters */
      assert_param(IS_DMA_SOURCE_BURST_LENGTH(pNodeConfig->Init.SrcBurstLength));
      assert_param(IS_DMA_DESTINATION_BURST_LENGTH(pNodeConfig->Init.DestBurstLength));
      assert_param(IS_DMA_DATA_EXCHANGE(pNodeConfig->Init.DataExchange));
      assert_param(IS_DMA_PORT(pNodeConfig->Init.SrcAllocatedPort));
      assert_param(IS_DMA_PORT(pNodeConfig->Init.DestAllocatedPort));

      pNode->LinkRegisters[index] |= ((pNodeConfig->Init.DestAllocatedPort << DMA_CTR1_DAP_Pos)                           | \
                                      (((pNodeConfig->Init.DestBurstLength - 1U) << DMA_CTR1_DBL_1_Pos) & DMA_CTR1_DBL_1) | \
                                      (pNodeConfig->Init.SrcAllocatedPort << DMA_CTR1_SAP_Pos)                            | \
                                      pNodeConfig->Init.DataExchange                                                      | \
                                      (((pNodeConfig->Init.SrcBurstLength - 1U) << DMA_CTR1_SBL_1_Pos) & DMA_CTR1_SBL_1));
    }

    /* Increment index to point to next register */
    index++;
  }

  /* Update CTR2 register value ***********************************************/
  if (pNodeConfig->ContainCTR2 != 0U)
  {
    /* Check the parameters */
    assert_param(IS_DMA_REQUEST(pNodeConfig->Init.Request));
    assert_param(IS_DMA_DIRECTION(pNodeConfig->Init.Direction));
    assert_param(IS_DMA_TRIGGER_POLARITY(pNodeConfig->Init.TriggerPolarity));
    assert_param(IS_DMA_EVENT_MODE(pNodeConfig->Init.TransferEventMode));
    assert_param(IS_DMA_BLOCK_HW_REQUEST(pNodeConfig->Init.BlockHWRequest));
    /* Check if trigger feature is active */
    if (pNodeConfig->Init.TriggerPolarity != DMA_TRIG_POLARITY_MASKED)
    {
      assert_param(IS_DMA_TRIGGER_MODE(pNodeConfig->Init.TriggerMode));
      assert_param(IS_DMA_TRIGGER_SELECTION(pNodeConfig->Init.TriggerSelection));
    }

    /* Prepare DMA Channel Transfer Register 2 (CTR2) value */
    pNode->LinkRegisters[index] = pNodeConfig->Init.TransferEventMode | \
                                  pNodeConfig->Init.TriggerPolarity   | \
                                  pNodeConfig->Init.BlockHWRequest    | \
                                  (pNodeConfig->Init.Request & (DMA_CTR2_REQSEL | DMA_CTR2_SWREQ));

    if (pNodeConfig->Init.TriggerPolarity != DMA_TRIG_POLARITY_MASKED)
    {
      pNode->LinkRegisters[index] |= pNodeConfig->Init.TriggerMode | \
                                    ((pNodeConfig->Init.TriggerSelection << DMA_CTR2_TRIGSEL_Pos) & DMA_CTR2_TRIGSEL);
    }

    /* Memory to Peripheral transfer */
    if ((pNodeConfig->Init.Direction) == DMA_MEMORY_TO_PERIPH)
    {
      if (IS_GPDMA_INSTANCE(hdma->Instance) != 0U)
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
      /* Nothing To Do */
    }

    /* Increment index to point to next register */
    index++;
  }

  /* Update CBR1 register value ***********************************************/
  if (pNodeConfig->ContainCBR1 != 0U)
  {
    /* Check the parameters */
    assert_param(IS_DMA_BLOCK_SIZE(pNodeConfig->SourceDataSize));

    /* Set the new CBR1 Register value */
    pNode->LinkRegisters[index] = (pNodeConfig->SourceDataSize & DMA_CBR1_BNDT);

    /* If 2D Addressing is supported by current channel */
    if (IS_DMA_2D_ADDRESSING_INSTANCE(hdma->Instance) != 0U)
    {
      /* Check the parameters */
      assert_param(IS_DMA_REPEAT_COUNT(pNodeConfig->Init.RepeatCount));
      assert_param(IS_DMA_BURST_ADDR_OFFSET(pNodeConfig->Init.SrcAddressOffset));
      assert_param(IS_DMA_BURST_ADDR_OFFSET(pNodeConfig->Init.DestAddressOffset));
      assert_param(IS_DMA_BLOCK_ADDR_OFFSET(pNodeConfig->Init.BlockSrcAddressOffset));
      assert_param(IS_DMA_BLOCK_ADDR_OFFSET(pNodeConfig->Init.BlockDestAddressOffset));

      /* Set the new CBR1 Register value */
      pNode->LinkRegisters[index] |= (((pNodeConfig->Init.RepeatCount - 1U) << DMA_CBR1_BRC_Pos) & DMA_CBR1_BRC);

      /* If the source address offset is negative, set SDEC bit */
      if (pNodeConfig->Init.SrcAddressOffset < 0)
      {
        pNode->LinkRegisters[index] |= DMA_CBR1_SDEC;
      }
      else
      {
        pNode->LinkRegisters[index] &= (~DMA_CBR1_SDEC);
      }

      /* If the destination address offset is negative, set DDEC bit */
      if (pNodeConfig->Init.DestAddressOffset < 0)
      {
        pNode->LinkRegisters[index] |= DMA_CBR1_DDEC;
      }
      else
      {
        pNode->LinkRegisters[index] &= (~DMA_CBR1_DDEC);
      }

      /* If the repeated block source address offset is negative, set BRSEC bit */
      if (pNodeConfig->Init.BlockSrcAddressOffset < 0)
      {
        pNode->LinkRegisters[index] |= DMA_CBR1_BRSDEC;
      }
      else
      {
        pNode->LinkRegisters[index] &= (~DMA_CBR1_BRSDEC);
      }

      /* if the repeated block destination address offset is negative, set BRDEC bit */
      if (pNodeConfig->Init.BlockDestAddressOffset < 0)
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
  }

  /* Update CSAR register value ***********************************************/
  if (pNodeConfig->ContainCSAR != 0U)
  {
    pNode->LinkRegisters[index] = pNodeConfig->SrcAddress;

    /* Increment index to point to next register */
    index++;
  }

  /* Update CDAR register value ***********************************************/
  if (pNodeConfig->ContainCDAR != 0U)
  {
    pNode->LinkRegisters[index] = pNodeConfig->DstAddress;

    /* Increment index to point to next register */
    index++;
  }

  /* Update CTR3 register value ***********************************************/
  if ((pNodeConfig->ContainCTR3 != 0U) && (IS_DMA_2D_ADDRESSING_INSTANCE(hdma->Instance) != 0U))
  {
    /* Check the parameters */
    assert_param(IS_DMA_BURST_ADDR_OFFSET(pNodeConfig->Init.SrcAddressOffset));
    assert_param(IS_DMA_BURST_ADDR_OFFSET(pNodeConfig->Init.DestAddressOffset));

    /* Write new CTR3 Register value : source address offset */
    if (pNodeConfig->Init.SrcAddressOffset < 0)
    {
      blockoffset = (uint32_t)(-pNodeConfig->Init.SrcAddressOffset);
      pNode->LinkRegisters[index] = (blockoffset & DMA_CTR3_SAO);
    }
    else
    {
      pNode->LinkRegisters[index] = (((uint32_t)pNodeConfig->Init.SrcAddressOffset) & DMA_CTR3_SAO);
    }

    /* Write new CTR3 Register value : destination address offset */
    if (pNodeConfig->Init.DestAddressOffset < 0)
    {
      blockoffset = (uint32_t)(-pNodeConfig->Init.DestAddressOffset);
      pNode->LinkRegisters[index] |= ((blockoffset << DMA_CTR3_DAO_Pos) & DMA_CTR3_DAO);
    }
    else
    {
      pNode->LinkRegisters[index] |= ((((uint32_t)pNodeConfig->Init.DestAddressOffset) << DMA_CTR3_DAO_Pos) & DMA_CTR3_DAO);
    }

    /* Increment index to point to next register */
    index++;
  }

  /* Update CBR2 register value ***********************************************/
  if ((pNodeConfig->ContainCBR2 != 0U)  && (IS_DMA_2D_ADDRESSING_INSTANCE(hdma->Instance) != 0U))
  {
    /* Check the parameters */
    assert_param(IS_DMA_BLOCK_ADDR_OFFSET(pNodeConfig->Init.BlockSrcAddressOffset));
    assert_param(IS_DMA_BLOCK_ADDR_OFFSET(pNodeConfig->Init.BlockDestAddressOffset));

    /* Write new CBR2 Register value : repeated block source address offset */
    if (pNodeConfig->Init.BlockSrcAddressOffset < 0)
    {
      blockoffset = (uint32_t)(-pNodeConfig->Init.BlockSrcAddressOffset);
      pNode->LinkRegisters[index] = (blockoffset & DMA_CBR2_BRSAO);
    }
    else
    {
      pNode->LinkRegisters[index] = (((uint32_t)pNodeConfig->Init.BlockSrcAddressOffset) & DMA_CBR2_BRSAO);
    }

    /* Write new CBR2 Register value : repeated block destination address offset */
    if (pNodeConfig->Init.BlockDestAddressOffset < 0)
    {
      blockoffset = (uint32_t)(-pNodeConfig->Init.BlockDestAddressOffset);
      pNode->LinkRegisters[index] |= ((blockoffset & DMA_CBR2_BRSAO) << DMA_CBR2_BRDAO_Pos);
    }
    else
    {
      pNode->LinkRegisters[index] |= ((((uint32_t)pNodeConfig->Init.BlockDestAddressOffset) << DMA_CBR2_BRDAO_Pos) & DMA_CBR2_BRDAO);
    }

    /* Increment index to point to next register */
    index++;
  }

  /* Update CLLR register value ***********************************************/
  if (pNodeConfig->ContainCLLR != 0U)
  {
    /* This node is not yet linked */
    pNode->LinkRegisters[index] = 0U;
  }

  /* Specify the index of LLR register in LinkRegister table */
  pNode->IndexLLR = index;

  /* Configure structure of the node */
  pNode->ConfigLLR = ((pNodeConfig->ContainCTR1 << DMA_CLLR_UT1_Pos) | \
                      (pNodeConfig->ContainCTR2 << DMA_CLLR_UT2_Pos) | \
                      (pNodeConfig->ContainCBR1 << DMA_CLLR_UB1_Pos) | \
                      (pNodeConfig->ContainCSAR << DMA_CLLR_USA_Pos) | \
                      (pNodeConfig->ContainCDAR << DMA_CLLR_UDA_Pos) | \
                      (pNodeConfig->ContainCTR3 << DMA_CLLR_UT3_Pos) | \
                      (pNodeConfig->ContainCBR2 << DMA_CLLR_UB2_Pos) | \
                      (pNodeConfig->ContainCLLR << DMA_CLLR_ULL_Pos));

  /* This node is not yet linked */
  pNode->Next = NULL;

  return HAL_OK;
}

/**
  * @brief  Connect a node to the Linked-list.
  * @param  hdma:      Pointer to a DMA_HandleTypeDef structure that contains
  *                    the configuration information for the specified DMA Channel.
  * @param  pNewNode:  Pointer to a DMA_LinkNodeTypeDef structure that contains
  *                    Linked-list node to be add to the list.
  * @param  pPrevNode: Pointer to the new node position in the linked-list or zero
  *                    to insert the new node at the end of the list.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_LinkedList_AddNode(DMA_HandleTypeDef *hdma, DMA_LinkNodeTypeDef *pNewNode, DMA_LinkNodeTypeDef *pPrevNode)
{
  DMA_LinkNodeTypeDef *pNode;
  uint32_t counter = 0U, nodeInserted = 0U;
  HAL_StatusTypeDef hal_status = HAL_OK;

  /* Check the DMA peripheral handle */
  if ((hdma == NULL) || (pNewNode == NULL))
  {
    /* Return error status */
    return HAL_ERROR;
  }

  if (hdma->State != HAL_DMA_STATE_ERROR)
  {
    /* Process locked */
    __HAL_LOCK(hdma);

    /* Check if this is the first node (after the Inititlization node) */
    if (hdma->FirstLinkedListNodeAddress == NULL)
    {
      if (pPrevNode == NULL)
      {
        /* If this is the first node after the initialization, connect this node
        to the node 0 by updating the DMA channel CLBAR / CLLR registers to this node address */
        hdma->Instance->CLBAR = (uint32_t)pNewNode;
        hdma->Instance->CLLR = (((uint32_t)pNewNode) & DMA_CLLR_LA) | (pNewNode->ConfigLLR);

        /* Set the DMA handle First Linked-List node*/
        hdma->FirstLinkedListNodeAddress = pNewNode;

        /* Update the Handle last node address */
        hdma->LastLinkedListNodeAddress = pNewNode;

        /* Update node counter */
        hdma->LinkedListNodeCounter = 1U;
      }
      else
      {
        /* Update error status */
        hal_status = HAL_ERROR;
      }
    }
    else if (hdma->FirstLinkedListNodeAddress != pNewNode)
    {
      /* Check if the node to insert already exists */
      pNode = hdma->FirstLinkedListNodeAddress;

      while ((counter < hdma->LinkedListNodeCounter) && (hal_status == HAL_OK))
      {
        if (pNode->Next == pNewNode)
        {
          /* Error: this node already exist in the linked-list and it is not first node */
          hal_status = HAL_ERROR;
        }

        pNode = pNode->Next;
        counter++;
      }

      /* The New Node does not exist in the linked-list */
      if (hal_status == HAL_OK)
      {
        /* Check if the previous node is the last one in the current list or zero */
        if ((pPrevNode == hdma->LastLinkedListNodeAddress) || (pPrevNode == NULL))
        {
          /* Insert the new node at the end of the linked-list */
          pNewNode->Next = hdma->LastLinkedListNodeAddress->Next;
          pNewNode->LinkRegisters[pNewNode->IndexLLR] = hdma->LastLinkedListNodeAddress->LinkRegisters[hdma->LastLinkedListNodeAddress->IndexLLR];

          hdma->LastLinkedListNodeAddress->Next = pNewNode;
          hdma->LastLinkedListNodeAddress->LinkRegisters[hdma->LastLinkedListNodeAddress->IndexLLR] = (((uint32_t)pNewNode) & DMA_CLLR_LA) | \
              (pNewNode->ConfigLLR);

          /* Update the Handle last node address */
          hdma->LastLinkedListNodeAddress = pNewNode;

          /* Increment the linked-list node counter */
          hdma->LinkedListNodeCounter++;
        }
        else
        {
          /* Insert the new node after the pPrevNode node */
          pNode = hdma->FirstLinkedListNodeAddress;
          counter = 0;

          while ((counter < hdma->LinkedListNodeCounter) && (nodeInserted == 0U))
          {
            counter++;

            if (pNode == pPrevNode)
            {
              /* Insert the new node after the previous one */
              pNewNode->Next = pNode->Next;
              pNewNode->LinkRegisters[pNewNode->IndexLLR] = pNode->LinkRegisters[pNode->IndexLLR];

              pNode->Next = pNewNode;
              pNode->LinkRegisters[pNode->IndexLLR] = (((uint32_t)pNewNode) & DMA_CLLR_LA) | (pNewNode->ConfigLLR);

              /* Increment the linked-list node counter */
              hdma->LinkedListNodeCounter++;
              nodeInserted = 1;
            }
            else
            {
              pNode = pNode->Next;
            }
          }

          if (nodeInserted == 0U)
          {
            /* Update error status */
            hal_status = HAL_ERROR;
          }
        }
      }
    }
    else
    {
      /* Update error status */
      hal_status = HAL_ERROR;
    }

    /* Process unlocked */
    __HAL_UNLOCK(hdma);

    return hal_status;
  }
  else
  {
    /* Return error status */
    return HAL_ERROR;
  }
}

/**
  * @brief  Disconnect/Remove a node from the transfer linked-list.
  * @param  hdma:   Pointer to a DMA_HandleTypeDef structure that contains
  *                 the configuration information for the specified DMA Channel.
  * @param  pNode:  Pointer to a DMA_LinkNodeTypeDef structure that contains
  *                 Linked-list node to be removed from the list.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_LinkedList_RemoveNode(DMA_HandleTypeDef *hdma, DMA_LinkNodeTypeDef *pNode)
{
  DMA_LinkNodeTypeDef *ptmpNode;
  uint32_t counter = 0, nodeDeleted = 0;
  HAL_StatusTypeDef hal_status = HAL_OK;

  /* Check the DMA peripheral handle */
  if ((hdma == NULL) || (pNode == NULL))
  {
    /* Return error status */
    return HAL_ERROR;
  }

  /* Process locked */
  __HAL_LOCK(hdma);

  if (hdma->State != HAL_DMA_STATE_ERROR)
  {
    /* If first and last node are null (no nodes in the list) : return error */
    if ((hdma->FirstLinkedListNodeAddress == NULL) || (hdma->LastLinkedListNodeAddress == NULL) || (hdma->LinkedListNodeCounter == 0U))
    {
      /* Update error status */
      hal_status = HAL_ERROR;
    }
    else if (hdma->FirstLinkedListNodeAddress == pNode) /* Deleting first node */
    {
      /* Delete 1st node */
      if (hdma->LastLinkedListNodeAddress == pNode)
      {
        /* if the last node is at the same time the first one (1 single node after the init node 0)
        then update the last node too */
        hdma->FirstLinkedListNodeAddress = 0U;
        hdma->LastLinkedListNodeAddress  = 0U;
        hdma->LinkedListNodeCounter      = 0U;

        hdma->Instance->CLLR = 0U;
      }
      else
      {
        if (hdma->FirstLinkedListNodeAddress == hdma->LastLinkedListNodeAddress->Next)
        {
          /* if last node is looping to first (circular list) one update the last node connection */
          hdma->LastLinkedListNodeAddress->Next = pNode->Next;
          hdma->LastLinkedListNodeAddress->LinkRegisters[hdma->LastLinkedListNodeAddress->IndexLLR] = pNode->LinkRegisters[pNode->IndexLLR];
        }

        /* if deleting the first node after the initialization
        connect the next node to the node 0 by updating
        the DMA channel CLLR register to this node address */
        hdma->Instance->CLLR = pNode->LinkRegisters[pNode->IndexLLR];
        hdma->FirstLinkedListNodeAddress = hdma->FirstLinkedListNodeAddress->Next;

        /* Update the Handle node counter */
        hdma->LinkedListNodeCounter--;
      }
    }
    else /* Deleting any other node */
    {
      /* Deleted node is not the first one : find it */
      ptmpNode = hdma->FirstLinkedListNodeAddress;

      while ((counter < hdma->LinkedListNodeCounter) && (nodeDeleted == 0U))
      {
        counter++;

        if (ptmpNode->Next == pNode)
        {
          /* If deleting the last node */
          if (pNode == hdma->LastLinkedListNodeAddress)
          {
            /* Update the linked list last node address in the handle */
            hdma->LastLinkedListNodeAddress = ptmpNode;
          }

          /* Update the next node link after deleting pDMA_LinkedListNode */
          ptmpNode->Next = pNode->Next;
          ptmpNode->LinkRegisters[ptmpNode->IndexLLR] = pNode->LinkRegisters[pNode->IndexLLR];

          nodeDeleted = 1;

          /* Update the Handle node counter */
          hdma->LinkedListNodeCounter--;
        }
        else
        {
          ptmpNode = ptmpNode->Next;
        }
      }

      if (nodeDeleted == 0U)
      {
        /* last node reashed without finding the node to delete : return error */
        hal_status = HAL_ERROR;
      }
    }

    /* Process unlocked */
    __HAL_UNLOCK(hdma);

    return hal_status;
  }
  else
  {
    /* Process unlocked */
    __HAL_UNLOCK(hdma);

    /* Return error status */
    return HAL_ERROR;
  }
}

/**
  * @brief  Make the linked list circular by connecting the last node to the first.
  * @param  hdma:  Pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA Channel.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_LinkedList_EnableCircularMode(DMA_HandleTypeDef *hdma)
{
  HAL_StatusTypeDef hal_status = HAL_OK;

  /* Check the DMA peripheral handle */
  if (hdma == NULL)
  {
    /* Return error status */
    return HAL_ERROR;
  }

  /* Process locked */
  __HAL_LOCK(hdma);

  if (hdma->State != HAL_DMA_STATE_ERROR)
  {
    /* If first and last node are null (no nodes in the list) : return error */
    if ((hdma->FirstLinkedListNodeAddress == NULL) || (hdma->LastLinkedListNodeAddress == NULL) || (hdma->LinkedListNodeCounter == 0U))
    {
      /* Update error status */
      hal_status = HAL_ERROR;
    }
    else
    {
      /* To enable circular mode, the last node should be connected to the first one */
      hdma->LastLinkedListNodeAddress->Next = hdma->FirstLinkedListNodeAddress;
      hdma->LastLinkedListNodeAddress->LinkRegisters[hdma->LastLinkedListNodeAddress->IndexLLR] = hdma->Instance->CLLR;
    }

  }

  /* Process unlocked */
  __HAL_UNLOCK(hdma);

  return hal_status;
}

/**
  * @brief  Disable the linked-list circular mode by setting the last node connection to null
  * @param  hdma:  Pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA Channel.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMAEx_LinkedList_DisableCircularMode(DMA_HandleTypeDef *hdma)
{
  HAL_StatusTypeDef hal_status = HAL_OK;

  /* Check the DMA peripheral handle */
  if (hdma == NULL)
  {
    /* Return error status */
    return HAL_ERROR;
  }

  /* Process locked */
  __HAL_LOCK(hdma);

  if (hdma->State != HAL_DMA_STATE_ERROR)
  {
    /* If first and last node are null (no nodes in the list) : return error*/
    if ((hdma->FirstLinkedListNodeAddress == NULL) || (hdma->LastLinkedListNodeAddress == NULL) || (hdma->LinkedListNodeCounter == 0U))
    {
      /* Update error status */
      hal_status = HAL_ERROR;
    }
    else
    {
      /* To disable circular mode, the last Node should be connected to NULL */
      hdma->LastLinkedListNodeAddress->Next = NULL;
      hdma->LastLinkedListNodeAddress->LinkRegisters[hdma->LastLinkedListNodeAddress->IndexLLR] = 0U;
    }

  }
  /* Process unlocked */
  __HAL_UNLOCK(hdma);

  return hal_status;
}

/**
  * @}
  */

/** @addtogroup DMAEx_Exported_Functions_Group2
  *
@verbatim
 ===============================================================================
                      #####  Suspend and Resume operations functions  #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Suspend a DMA trasnfer
      (+) Resume a DMA transfer
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
    /* Resume the channel */
    hdma->Instance->CCR &= (~DMA_CCR_SUSP);

    /* Clear the Suspend flag */
    hdma->Instance->CFCR |= DMA_CFCR_SUSPF;
  }

  return HAL_OK;
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
