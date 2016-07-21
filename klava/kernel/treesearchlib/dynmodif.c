// dynmodif.c
//

#include "common.h"
#include "crc32.h"

#ifndef TREE_STATIC_ONLY

// ------------------------------------------------------------------------------------------------

void TreeCopyNode (DNODE* pNodeDst, DNODE* pNodeSrc)
{
	pNodeDst->N.NodeType        = pNodeSrc->N.NodeType;
	pNodeDst->N.NodeType.NodeTypeFields.NodeDataType    = pNodeSrc->N.NodeType.NodeTypeFields.NodeDataType;
//	pNodeDst->bHasCondition   = pNodeSrc->bHasCondition;
//	pNodeDst->bHasSignatureID = pNodeSrc->bHasSignatureID;
//	pNodeDst->N.NodeType.NodeTypeFields.NodeDataType    = pNodeSrc->N.NodeType.NodeTypeFields.NodeDataType;
	pNodeDst->SignatureID     = pNodeSrc->SignatureID;	
	pNodeDst->NodeData.pNodeDataList   = pNodeSrc->NodeData.pNodeDataList;
//	pNodeDst->ulNumOfChilds   = pNodeSrc->ulNumOfChilds;
	pNodeDst->pCondition      = pNodeSrc->pCondition;
}

// ------------------------------------------------------------------------------------------

TREE_ERR TreeAddRawSignature(sRT_CTX* pRTCtx, DNODE** ppNode, const uint8_t* pData, uint32_t dwDataSize, DNODE** ppTailNode)
{
	TREE_ERR error = TREE_OK;
	DNODE* pNode;
	NODEDATA_P_JOINED* pNDJoined;
	uint32_t i,j;
	
	pNode = *ppNode;
	for (i=0; i<dwDataSize; i++)
	{
		if (pNode->N.NodeType.NodeTypeFields.NodeDataType == NDT_P_List && pNode->NodeData.pNodeDataList == NULL && pNode->pCondition == NULL && (dwDataSize - i > 1)) // last node in branch
		{
			error = _HeapAlloc2(pRTCtx->pHeapCtx, (void**)&pNDJoined, sizeof(NODEDATA_P_JOINED)+ dwDataSize - i);
			if (TREE_FAILED(error))
				return error;

			pNDJoined->dwNumOfBytes = dwDataSize - i;
			if (pNDJoined->dwNumOfBytes<=1) {	DBG_STOP;	};
			// TreeCopyNode(&pNDJoined->Node, pNode);
			TreeInitNode(&pNDJoined->Node);
			for (j=0; j<pNDJoined->dwNumOfBytes; j++)
				pNDJoined->NodeBytes[j] = pData[i+j];
			pNode->N.NodeType.NodeTypeFields.NodeType = NT_Dynamic;
			pNode->N.NodeType.NodeTypeFields.NodeDataType = NDT_P_Joined;
//			pNode->ulNumOfChilds = 1;
			pNode->NodeData.pNodeDataJoined = pNDJoined;
			*ppTailNode = &pNDJoined->Node;
			return TREE_OK;
		}
		error = TreeAddNode(pRTCtx, &pNode, pData[i], &pNode);
		if (TREE_FAILED(error))
			return error;
	}

	if (pNode->N.NodeType.NodeTypeFields.NodeDataType == NDT_P_JoinedRT)
		error = TreeSplitNode(pRTCtx, pNode, &pNode);
	if (ppTailNode != NULL)
		*ppTailNode = pNode;
	return error;
}

TREE_ERR TreeAddNodeWithCondition(sRT_CTX* pRTCtx, DNODE** ppNode, uint8_t nConditionType, uint8_t Byte1, uint8_t Byte2, DNODE** ppNewNode)
{
	DCONDITION** ppCondition;
	TREE_ERR error = TREE_OK;
	
	error = TreeSplitNode(pRTCtx, *ppNode, ppNode);
	if (TREE_FAILED(error))
		return error;
	
	ppCondition = &((*ppNode)->pCondition);
	while (*ppCondition != NULL)
	{
		if ((*ppCondition)->C.ConditionType.nType == nConditionType &&
			(*ppCondition)->C.Data.Byte1 == Byte1 &&
			(*ppCondition)->C.Data.Byte2 == Byte2 )
		{
			*ppNewNode = &((*ppCondition)->Node);
			return error;
		}
		ppCondition = &((*ppCondition)->pNextCondition);
	}
	error = _HeapAlloc2(pRTCtx->pHeapCtx, (void**)ppCondition, sizeof(DCONDITION));
	if (*ppCondition == NULL)
		return TREE_ENOMEM;
	(*ppCondition)->C.ConditionType.nType = nConditionType;
	(*ppCondition)->C.Data.Byte1 = Byte1;
	(*ppCondition)->C.Data.Byte2 = Byte2;
	TreeInitNode(&((*ppCondition)->Node));
	*ppNewNode = &((*ppCondition)->Node);
	return error;
}

TREE_ERR _TREE_CALL TreeAddSignature(sRT_CTX* pRTCtx, const uint8_t* pSignature, uint32_t dwSignatureSize, tSIGNATURE_ID SignatureID, tSIGNATURE_ID* pResultSignatureID)
{
	TREE_ERR error = TREE_OK;
	tSIGNATURE_ID ret_val = 0;
	uint8_t* pRawData;
	uint32_t dwPosInSig = 0;
	uint32_t dwRawDataLen = 0;
	DNODE* pNode;
	uint32_t dwRawDataTotal = 0;
	

	if (pSignature==NULL || dwSignatureSize==0 || SignatureID==0)
		return TREE_EINVAL;

	if (pRTCtx->sTreeInfo.dwMagic == 0) // no data
	{
		if (TREE_FAILED(error = TreeCreateRootNode(pRTCtx)))
			return error;
	}

	if (pRTCtx->sTreeInfo.dwFormat != cFORMAT_DYNAMIC)
		return TREE_ECORRUPT;
	
	pNode = PDNODE(pRTCtx->pRoot);
	pRawData = (uint8_t*)pSignature;
	while ((dwPosInSig < dwSignatureSize) && (TREE_SUCCEEDED(error)))
	{
		if (pSignature[dwPosInSig] == SIGNATURE_ESC_CHAR)
		{
			dwPosInSig++;
			if (dwPosInSig >= dwSignatureSize)
			{
				//PR_TRACE(( NULL, prtERROR, "Signature format error (no data after escape character) @%d", pSignature[dwPosInSig], dwPosInSig));
				error = TREE_EINVAL;
				break;
			}
			switch (pSignature[dwPosInSig])
			{
			case SIGNATURE_ESC_CHAR:
				// skip double ESC char
				if (dwRawDataLen > 0)
					error = TreeAddRawSignature(pRTCtx, &pNode, pRawData, dwRawDataLen, &pNode); 
				dwRawDataTotal += dwRawDataLen;
				dwRawDataLen = 0;
				pRawData = (uint8_t*)pSignature + dwPosInSig;
				break;
			case cCT_BitMaskAnd:
			case cCT_BitMaskOr:
//			case cCT_BitMaskXor:
			case cCT_Range:
			case cCT_Aster:
				if (dwPosInSig + 2 >= dwSignatureSize)
				{
					//PR_TRACE(( NULL, prtERROR, "Signature format error (not enought data after escape character) @%d", pSignature[dwPosInSig], dwPosInSig));
					error = TREE_EINVAL;
					break;
				}
				if (dwRawDataLen > 0)
					error = TreeAddRawSignature(pRTCtx, &pNode, pRawData, dwRawDataLen, &pNode);
				dwRawDataTotal += dwRawDataLen;
//				if (dwRawDataLen < 3)
//				{	
//					//PR_TRACE(( NULL, prtERROR, "Signature format error (condition must be after 3'rd byte) @%d", pSignature[dwPosInSig], dwPosInSig));
//					error = TREE_EINVAL;
//					break;
//				}
				if (TREE_SUCCEEDED (error))
				{
					uint32_t op = pSignature[dwPosInSig];
					uint32_t _mask = pSignature[dwPosInSig+1];
					uint32_t c = pSignature[dwPosInSig+2];
					// verify condition
					switch (op)
					{
					case cCT_BitMaskAnd:
						if (c & (~_mask))
						{
							ODS(("Signature format error (invalid parameters for [&]) @%d", dwPosInSig));
							error = TREE_EINVAL;
						}
						break;
					case cCT_BitMaskOr:
						if ((c & _mask) != _mask)
						{
							ODS(("Signature format error (invalid parameters for [|]) @%d", dwPosInSig));
							error = TREE_EINVAL;
						}
						break;
//					case cCT_BitMaskXor:
//						{
//							tBOOL bOk = cFALSE;
//							tINT i;
//							for (i=0; i<256; i++)
//							{
//								if((i & _mask) == c)
//								{
//									bOk = cTRUE;
//									break;
//								}
//							}
//							if (!bOk)
//							{
//								ODS(("Signature format error (invalid parameters for [^]) @%d", dwPosInSig));
//								error = TREE_EINVAL;
//							}
//						}
//						break;
					case cCT_Range:
						if (_mask > c)
						{
							ODS(("Signature format error (invalid parameters for [R,c1>c2]) @%d", dwPosInSig));
							error = TREE_EINVAL;
						}
						if (_mask == c)
						{
							ODS(("Signature format error (invalid parameters for [R,c1==c2]) @%d", dwPosInSig));
							error = TREE_EINVAL;
						}
					case cCT_Aster:
						break;
					}
				}

				if (TREE_SUCCEEDED(error))
					error = TreeAddNodeWithCondition(pRTCtx, &pNode, pSignature[dwPosInSig], pSignature[dwPosInSig+1], pSignature[dwPosInSig+2], &pNode);
				dwPosInSig+=3;
				dwRawDataLen = 0;
				pRawData = (uint8_t*)pSignature + dwPosInSig;
				continue;
			default:
				//PR_TRACE(( NULL, prtERROR, "Signature format error (invalid escape character %02X) @%d", pSignature[dwPosInSig], dwPosInSig));
				error = TREE_EINVAL;
				dwPosInSig--;
				break;
				
			}
		}
		//pRawData[dwRawDataLen++] = pSignature[dwPosInSig++];
		dwPosInSig++;
		dwRawDataLen++;
	}
	if (TREE_SUCCEEDED(error))
	{
		if (dwRawDataLen > 0)
			error = TreeAddRawSignature(pRTCtx, &pNode, pRawData, dwRawDataLen, &pNode);
		dwRawDataLen = 0;
		pRawData = (uint8_t*)pSignature + dwPosInSig;
	}

	if (TREE_SUCCEEDED(error))
	{
		if (pNode->SignatureID)
		{
			ret_val = pNode->SignatureID;
			error = TREE_EDUPLICATE;
		}
		else
		{
			ret_val = pNode->SignatureID = SignatureID;
			if (pRTCtx->sTreeInfo.MaxSignatureID < SignatureID)
				pRTCtx->sTreeInfo.MaxSignatureID = SignatureID;
			if (pRTCtx->sTreeInfo.MinSignatureID > SignatureID || pRTCtx->sTreeInfo.MinSignatureID==0)
				pRTCtx->sTreeInfo.MinSignatureID = SignatureID;
		}
	}

	if ( pResultSignatureID )
		*pResultSignatureID = ret_val;

	//PR_TRACE(( NULL, prtNOTIFY, "Leave \"TreeAddSignature\" method, %terr", error));
	return error;
}

// ------------------------------------------------------------------------------------------------

TREE_ERR TreeSplitNode(sRT_CTX* pRTCtx, DNODE* pNode, DNODE** ppNewNode)
{
	TREE_ERR error = TREE_OK;
	switch(pNode->N.NodeType.NodeTypeFields.NodeDataType)
	{
	case NDT_P_Index32:
		break;
	case NDT_P_List:
		break;
	case NDT_P_Joined:
		error = TreeSplitJoinedNode(pRTCtx, pNode, 0, ppNewNode);
		if (TREE_FAILED(error))
			return error;
		break;
	case NDT_P_JoinedRT:
		{
			NODEDATA_P_JOINED_RT* pTNode = pNode->NodeData.pNodeDataJoinedRT;
			DNODE* pJoinedRTNode = pNode;
			//_HeapFree(pRTCtx->pHeapCtx, pNode);
			error = TreeSplitJoinedNode(pRTCtx, pTNode->pParent.Node, pTNode->dwCurrentByte + 1, ppNewNode);
			if (TREE_FAILED(error))
				return error;
			FreeJoinedRTNode(pRTCtx, pJoinedRTNode);
			//_HeapFree(pRTCtx->pHeapCtx, pTNode);
		}
		break;
	case NDT_P_Hashed:
	case NDT_P_HashedRT:
		DBG_STOP;
		break;
	default:
		ODSN(("TreeSplitNode: Unsupported node type"));
		DBG_STOP;
		return TREE_EUNEXPECTED;
	}
	return error;
}

/*
TREE_ERR TreeAddNodeBitMask(sRT_CTX* pRTCtx, DNODE** ppNode, uint8_t Mask, uint8_t MaskedByte, DNODE** ppNewNode)
{
	DCONDITION** ppCondition;
	TREE_ERR error = TREE_OK;

	error = TreeSplitNode(pRTCtx, *ppNode, ppNode);
	if (TREE_FAILED(error))
		return error;
	
	ppCondition = &((*ppNode)->pCondition);
	while (*ppCondition != NULL)
	{
		if ((*ppCondition)->nConditionType == BitMask &&
			(*ppCondition)->C.Data.Byte1 == BitMask &&
			(*ppCondition)->C.Data.Byte2 == MaskedByte)
		{
			*ppNewNode = &((*ppCondition)->Node);
			return error;
		}
		ppCondition = &((*ppCondition)->pNextCondition);
	}
	error = _HeapAlloc2(pRTCtx->pHeapCtx, (void**)ppCondition, sizeof(DCONDITION));
	if (*ppCondition == NULL)
		return TREE_ENOMEM;
	(*ppCondition)->nConditionType = BitMask;
	(*ppCondition)->C.Data.Byte2 = MaskedByte;
	(*ppCondition)->C.Data.Byte1 = BitMask;
	TreeInitNode(&((*ppCondition)->Node));
	*ppNewNode = &((*ppCondition)->Node);
	return error;
}
*/
/*
TREE_ERR TreeAddNodeRange(sRT_CTX* pRTCtx, DNODE** ppNode, uint8_t Min, uint8_t Max, DNODE** ppNewNode)
{
	TREE_ERR error = TREE_OK;
	DCONDITION** ppCondition;
	
	error = TreeSplitNode(pRTCtx, *ppNode, ppNode);
	if (TREE_FAILED(error))
		return error;
	
	ppCondition = &((*ppNode)->pCondition);
	while (*ppCondition != NULL)
	{
		if ((*ppCondition)->nConditionType == Range &&
			(*ppCondition)->C.Data.Byte2 == Max &&
			(*ppCondition)->Range.Min == Min)
		{
			*ppNewNode = &((*ppCondition)->Node);
			return error;
		}
		ppCondition = &((*ppCondition)->pNextCondition);
	}
	
	error = _HeapAlloc2(pRTCtx->pHeapCtx, (void**)ppCondition, sizeof(DCONDITION));
	if (*ppCondition == NULL)
		return TREE_ENOMEM;

	(*ppCondition)->nConditionType = Range;
	(*ppCondition)->C.Data.Byte2 = Max;
	(*ppCondition)->Range.Min = Min;
	TreeInitNode(&((*ppCondition)->Node));
	*ppNewNode = &((*ppCondition)->Node);
	return error;
}
*/

// ------------------------------------------------------------------------------------------------

TREE_ERR TreeAddNode(sRT_CTX* pRTCtx, DNODE** ppNode, uint8_t NodeByte, DNODE** ppNewNode)
{
	TREE_ERR error = TREE_OK;
	DNODE* pNewNode = NULL;
	
	if (NODE_DATA_TYPE(*ppNode) == NDT_P_List && TreeGetNodeChildrenCount(*ppNode) > 10)
		error = TreeConvertNodeList2Indexed(pRTCtx, *ppNode);
	if (TREE_FAILED(error))
		return error;
	
	switch((*ppNode)->N.NodeType.NodeTypeFields.NodeDataType)
	{
	case NDT_P_Index32:
		error = TreeAddNodeIndexed(pRTCtx, ppNode, NodeByte, &pNewNode);
		break;
	case NDT_P_List:
		error = TreeAddNodeList(pRTCtx, ppNode, NodeByte, &pNewNode);
		break;
	case NDT_P_Joined:
		error = TreeAddNodeJoined(pRTCtx, ppNode, NodeByte, &pNewNode);
		break;
	case NDT_P_JoinedRT:
		error = TreeAddNodeJoinedRT(pRTCtx, ppNode, NodeByte, &pNewNode);
		break;
	case NDT_P_Hashed:
		error = TreeAddNodeHashed(pRTCtx, ppNode, NodeByte, &pNewNode);
		break;
	case NDT_P_HashedRT:
		error = TreeAddNodeHashedRT(pRTCtx, ppNode, NodeByte, &pNewNode);
		break;
	default:
		ODSN(("TreeAddNode: Unsupported node type"));
		DBG_STOP;
		return TREE_EUNEXPECTED;
	}
//	if (error == TREE_OK)
//		(*ppNode)->ulNumOfChilds++;
	if (error == TREE_EDUPLICATE)
		error = TREE_OK;
	*ppNewNode = pNewNode;
	return error;
}

TREE_ERR TreeAddNodeList(sRT_CTX* pRTCtx, DNODE** ppNode, uint8_t NodeByte, DNODE** ppNewNode)
{
	NODEDATA_P_LIST* pChild;
	NODEDATA_P_LIST** ppChild;
	DNODE* pNode = *ppNode;
	TREE_ERR error = TREE_OK;
	
	ppChild = &pNode->NodeData.pNodeDataList; 
	
	do {
		pChild = *ppChild;
		
		if (pChild == NULL || pChild->NodeByte > NodeByte)
		{
			// Create new node
			NODEDATA_P_LIST* pNewChild;
			error = _HeapAlloc2(pRTCtx->pHeapCtx, (void**)&pNewChild, sizeof(NODEDATA_P_LIST));
			if (pNewChild == NULL)
				return TREE_ENOMEM;
			pNewChild->NodeByte = NodeByte;
			*ppChild = pNewChild;
			pNewChild->pNextChild = pChild;
			
			TreeInitNode(&pNewChild->Node);
			*ppNewNode = &(pNewChild->Node);
			
			return error;
		}
		else if (pChild->NodeByte == NodeByte)
		{
			// Node already exist
			*ppNewNode = &(pChild->Node);
			return TREE_EDUPLICATE;
		}
		else
		{
			ppChild = &pChild->pNextChild;
		}
	} while (ppChild != NULL);
	return error;
}

TREE_ERR TreeAddNodeIndexed(sRT_CTX* pRTCtx, DNODE** ppNode, uint8_t NodeByte, DNODE** ppNewNode)
{
	//	int nResult = -1;
	NODEDATA_P_INDEX32* pIndex;
	DNODE* pNode = *ppNode;
	TREE_ERR error = TREE_OK;

	pIndex = pNode->NodeData.pNodeDataIndex; 

	if (pIndex->Index[NodeByte] == NULL)
	{
		// create node
		DNODE* pNewNode;
		error = _HeapAlloc2(pRTCtx->pHeapCtx, (void**)&pNewNode, sizeof(DNODE));
		if (pNewNode == NULL)
			return TREE_ENOMEM;
		TreeInitNode(pNewNode);
		pIndex->Index[NodeByte] = pNewNode;
	}
	else
	{
		// same node already exists
		error = TREE_EDUPLICATE;
	}

	*ppNewNode = pIndex->Index[NodeByte];
	return error;
}

// ------------------------------------------------------------------------------------------------

TREE_ERR TreeSplitJoinedNode(sRT_CTX* pRTCtx, DNODE* pNode, uint32_t nPos, DNODE** ppNewNode)
{
	TREE_ERR error = TREE_OK;
	NODEDATA_P_JOINED* pJNode;
	NODEDATA_P_LIST* pCNode;
	uint32_t i;
	//		ODSN(("Memory used by tree:	%d bytes\nProcess heap size:	%d bytes\n", _getallocatedmemory(), _getheapsize());
	if (pNode->N.NodeType.NodeTypeFields.NodeDataType != NDT_P_Joined) 
		return TREE_EUNEXPECTED;

//	if (nPos == 0)
//		DBG_STOP;

	pJNode = pNode->NodeData.pNodeDataJoined;
	if (nPos >= pJNode->dwNumOfBytes) 
		return TREE_EINVAL;

	pCNode = NULL;
	error = _HeapAlloc2(pRTCtx->pHeapCtx, (void**)&pCNode, sizeof(NODEDATA_P_LIST));
	pCNode->NodeByte = pJNode->NodeBytes[nPos];

	if (pJNode->dwNumOfBytes - nPos > 2) // tail is two or more bytes of size
	{
		NODEDATA_P_JOINED* pEJNode = NULL;
		error = _HeapAlloc2(pRTCtx->pHeapCtx, (void**)&pEJNode, sizeof(NODEDATA_P_JOINED) + pJNode->dwNumOfBytes - nPos - 1);
		if (pEJNode == NULL)
			return TREE_ENOMEM;
		pEJNode->dwNumOfBytes = pJNode->dwNumOfBytes - nPos - 1;
		TreeCopyNode(&(pEJNode->Node), &(pJNode->Node)); 
		for (i=0; i<pEJNode->dwNumOfBytes; i++)
			pEJNode->NodeBytes[i] = pJNode->NodeBytes[nPos+i+1];
		pCNode->Node.N.NodeType.NodeTypeFields.NodeType = NT_Dynamic;
		pCNode->Node.N.NodeType.NodeTypeFields.NodeDataType = NDT_P_Joined;
		pCNode->Node.NodeData.pNodeDataJoined = pEJNode;
	}
	else
	{
		if (pJNode->dwNumOfBytes - nPos == 2) // tail is one byte of size 
		{
			NODEDATA_P_LIST* pECNode = NULL;
			error = _HeapAlloc2(pRTCtx->pHeapCtx, (void**)&pECNode, sizeof(NODEDATA_P_LIST));
			if (pECNode == NULL)
				return TREE_ENOMEM;
			pECNode->NodeByte = pJNode->NodeBytes[nPos+1];
			TreeCopyNode(&(pECNode->Node), &(pJNode->Node));
			pCNode->Node.N.NodeType.NodeTypeFields.NodeType = NT_Dynamic;
			pCNode->Node.N.NodeType.NodeTypeFields.NodeDataType = NDT_P_List;
			pCNode->Node.NodeData.pNodeDataList = pECNode;
		}
		else // tail is empty
		{
			TreeCopyNode(&(pCNode->Node), &(pJNode->Node));
		}
	}

	ZeroMemory(&(pJNode->Node), sizeof(DNODE));

	if (nPos > 1) // head is more then two bytes of size
	{
		error = _HeapReAlloc2(pRTCtx->pHeapCtx, (void**)&pNode->NodeData.pNodeDataJoined, pNode->NodeData.pNodeDataJoined, sizeof(NODEDATA_P_JOINED) + nPos);
		if (pNode->NodeData.pNodeDataJoined  == NULL)
			return TREE_ENOMEM;
		pJNode = pNode->NodeData.pNodeDataJoined;
		pJNode->dwNumOfBytes = nPos;
		pJNode->Node.N.NodeType.NodeTypeFields.NodeType = NT_Dynamic;
		pJNode->Node.N.NodeType.NodeTypeFields.NodeDataType = NDT_P_List;
		pJNode->Node.NodeData.pNodeDataList = pCNode;
		pJNode->Node.SignatureID = 0;
		*ppNewNode = &(pJNode->Node);
	}
	else
	{
		if (nPos == 1) // head is one bytes of size
		{
			NODEDATA_P_LIST* pHCNode = NULL; 
			error = _HeapAlloc2(pRTCtx->pHeapCtx, (void**)&pHCNode, sizeof(NODEDATA_P_LIST));
			if (pHCNode == NULL)
				return TREE_ENOMEM;
			pHCNode->NodeByte = pJNode->NodeBytes[0];
			pHCNode->Node.N.NodeType.NodeTypeFields.NodeType = NT_Dynamic;
			pHCNode->Node.N.NodeType.NodeTypeFields.NodeDataType = NDT_P_List;
			pHCNode->Node.NodeData.pNodeDataList = pCNode;
			pNode->N.NodeType.NodeTypeFields.NodeType = NT_Dynamic;
			pNode->N.NodeType.NodeTypeFields.NodeDataType = NDT_P_List;
			pNode->NodeData.pNodeDataList = pHCNode;
			*ppNewNode = &(pHCNode->Node);
			_HeapFree(pRTCtx->pHeapCtx, pJNode);
		}
		else // head is empty
		{
			pNode->N.NodeType.NodeTypeFields.NodeType = NT_Dynamic;
			pNode->N.NodeType.NodeTypeFields.NodeDataType = NDT_P_List;
			pNode->NodeData.pNodeDataList = pCNode;
//			pNode->ulNumOfChilds = 1;
			*ppNewNode = pNode;
			_HeapFree(pRTCtx->pHeapCtx, pJNode);
		}
		//		ODSN(("Memory used by tree:	%d bytes\nProcess heap size:	%d bytes\n", _getallocatedmemory(), _getheapsize());
	}			
	return error;
}


// ------------------------------------------------------------------------------------------------

TREE_ERR TreeAddNodeJoined(sRT_CTX* pRTCtx, DNODE** ppNode, uint8_t NodeByte, DNODE** ppNewNode)
{
	TREE_ERR error = TREE_OK;
	NODEDATA_P_JOINED* pJNode;
	pJNode = (*ppNode)->NodeData.pNodeDataJoined;
	if (NodeByte == pJNode->NodeBytes[0])
	{
		// same node already exists
		*ppNewNode = PDNODE (TreeGetNextNodeJoined (PNODE(*ppNode), NodeByte, pRTCtx));
		return TREE_EDUPLICATE;
	}
	error = TreeSplitJoinedNode(pRTCtx, *ppNode, 0, ppNode);
	if (TREE_FAILED(error))
		return error;
	return TreeAddNode(pRTCtx, ppNode, NodeByte, ppNewNode);
}

// ------------------------------------------------------------------------------------------------

TREE_ERR TreeAddNodeJoinedRT(sRT_CTX* pRTCtx, DNODE** ppNode, uint8_t NodeByte, DNODE** ppNewNode)
{
	TREE_ERR error = TREE_OK;
	NODEDATA_P_JOINED_RT* pTNode;
	NODEDATA_P_JOINED* pJNode;
	DNODE* pJoinedRTNode;
	pJoinedRTNode = *ppNode;
	// RT nodes: node data follows DNODE structure
	pTNode = (NODEDATA_P_JOINED_RT *)((*ppNode) + 1);
	pJNode = pTNode->pParent.Node->NodeData.pNodeDataJoined;
	
#ifdef _TREE_DEBUG
	if (DNODE_TYPE(pTNode->pParent.Node) != NDT_P_Joined)
	{
		ODSN(("Oops: DNODE_TYPE(pTNode->pParent.Node) != NDT_P_Joined"));
	}
	if (pTNode->dwCurrentByte + 1 >= pJNode->dwNumOfBytes)
	{
		ODSN(("Oops: pTNode->dwCurrentByte + 1 >= pTNode->pNDJoined->dwNumOfBytes"));
	}
#endif
	
	if (pJNode->NodeBytes[pTNode->dwCurrentByte + 1] == NodeByte)
	{
		// same node already exists
		*ppNewNode = PDNODE (TreeGetNextNodeJoinedRT(PNODE (*ppNode), NodeByte, pRTCtx));
		return TREE_EDUPLICATE;
	}
			
	//_HeapFree(pRTCtx->pHeapCtx, *ppNode);
	error = TreeSplitJoinedNode(pRTCtx, pTNode->pParent.Node, pTNode->dwCurrentByte + 1, ppNode);
	if (TREE_FAILED(error))
		return error;
	FreeJoinedRTNode(pRTCtx, pJoinedRTNode);
	//_HeapFree(pRTCtx->pHeapCtx, pTNode);
	return TreeAddNode(pRTCtx, ppNode, NodeByte, ppNewNode);
}

// ------------------------------------------------------------------------------------------------

TREE_ERR TreeAddNodeHashed(sRT_CTX* pRTCtx, DNODE** ppNode, uint8_t NodeByte, DNODE** ppNewNode)
{
	TREE_ERR error = TREE_EUNEXPECTED;
	ODS(("TreeAddNodeHashed: Cannot add node to hashed data"));
	return error;
}

TREE_ERR TreeAddNodeHashedRT(sRT_CTX* pRTCtx, DNODE** ppNode, uint8_t NodeByte, DNODE** ppNewNode)
{
	TREE_ERR error = TREE_EUNEXPECTED;
	ODS(("TreeAddNodeHashed: Cannot add node to hashed data"));
	return error;
}

// ------------------------------------------------------------------------------------------------

TREE_ERR TreeConvertNodeList2Indexed(sRT_CTX* pRTCtx, DNODE* pNode)
{
	NODEDATA_P_INDEX32* pIndex;
	NODEDATA_P_LIST* pChild;
	NODEDATA_P_LIST* pNextChild;
	TREE_ERR error = TREE_OK;
	int i;
	
	if (pNode->N.NodeType.NodeTypeFields.NodeDataType != NDT_P_List)
		return TREE_EUNEXPECTED;
	
	pChild = pNode->NodeData.pNodeDataList; 
	error = _HeapAlloc2(pRTCtx->pHeapCtx, (void**)&pIndex, sizeof(NODEDATA_P_INDEX32)); 
	if (pIndex == NULL)
		return TREE_ENOMEM;

	for (i=0; i<(sizeof(pIndex->Index)/sizeof(pIndex->Index[0]));i++)
		pIndex->Index[i] = NULL;
	
	while (pChild != NULL)
	{
		DNODE* pNewNode;
		error = _HeapAlloc2(pRTCtx->pHeapCtx, (void**)&pNewNode, sizeof(DNODE));
		if(TREE_FAILED(error))
			return error;
		pIndex->Index[pChild->NodeByte] = pNewNode;
		memcpy(pNewNode, &(pChild->Node), sizeof(DNODE));
		
		pNextChild = pChild->pNextChild;
		_HeapFree(pRTCtx->pHeapCtx, pChild);
		pChild = pNextChild;
		
	};
	
	pNode->N.NodeType.NodeTypeFields.NodeType = NT_Dynamic;
	pNode->N.NodeType.NodeTypeFields.NodeDataType = NDT_P_Index32;
	pNode->NodeData.pNodeDataIndex = pIndex;
	return TREE_OK;
}		

// ------------------------------------------------------------------------------------------------
//
//TREE_ERR TreeOptimizeNodes(sRT_CTX* pRTCtx, DNODE* pRoot, uint32_t* pdwNodesConverted)
//{
//	TREE_ERR error = TREE_OK;
//	DNODE* pChild = NULL;
//	uint32_t dwNodesJoined = 0;
//	tLONG level=0;
//	
//	if (level==0)
//	{
//		*pdwNodesConverted=0;
//		dwNodesJoined=0;
//	}
//	
//	error = TreeJoinNodes2(pRTCtx, pRoot, &dwNodesJoined);
//	
//	error = TreeGetNextNodeChild(pRTCtx, pRoot, &pChild);
//	if (TREE_FAILED(error))
//		return error;
//
//	while (pChild != NULL)
//	{
//		level++;
//		TreeOptimizeNodes(pRTCtx, pChild, pdwNodesConverted);
//		level--;
//		error = TreeGetNextNodeChild(pRTCtx, pRoot, &pChild);
//		if (TREE_FAILED(error))
//		{
//			level = 0;
//			return error;
//		}
//	};
//	
//	if (TreeGetNodeChildrenCount(pRoot) > 10)
//	{
//		error = TreeConvertNodeList2Indexed(pRTCtx, pRoot);
//		if (TREE_SUCCEEDED(error))
//			*pdwNodesConverted++;
//	}
//	
//#ifdef _TREE_DEBUG
//	if (level == 0)
//	{
//		ODSN(("Nodes dwNodesJoined: %dn", dwNodesJoined));
//	}
//#endif
//	return TREE_OK;
//}	
//
// ------------------------------------------------------------------------------------------------

//int TreeJoinNodes(sRT_CTX* pRTCtx, DNODE* pNode)
//{
//	TREE_ERR error = TREE_OK;
//	NODEDATA_P_LIST* pChild;
//	DNODE* pNextNode;
//	NODEDATA_P_LIST* pNextChildList;
//	NODEDATA_P_JOINED* pNextChildJoined;
//	NODEDATA_P_JOINED* pNDJoined;
//	
//	int i;
//	int TailSize;
//	
//	if (pNode->N.NodeType.NodeTypeFields.NodeDataType != NDT_P_List || TreeGetNodeChildrenCount(pNode) != 1 || pNode->pCondition != NULL)
//		return 0;
//	
//	pChild = pNode->NodeData.pNodeDataList; 
//	pNextNode = &(pChild->Node);
//	
//	if ((pNextNode->N.NodeType.NodeTypeFields.NodeDataType != NDT_P_Joined && pNextNode->N.NodeType.NodeTypeFields.NodeDataType != NDT_P_List) || TreeGetNodeChildrenCount(pNextNode) != 1 || pNextNode->SignatureID != NULL || pNextNode->pCondition != NULL)
//		return 0;	
//	
//	pNextChildList = pNextNode->NodeData.pNodeDataList;
//	pNextChildJoined = pNextNode->NodeData.pNodeDataJoined;
//	/*
//	if (pNextNode->N.NodeType.NodeTypeFields.NodeDataType == NDT_P_List)
//	{
//	TailSize = 2;
//	error = _HeapAlloc2(pRTCtx->pHeapCtx, (void**)&pNDJoined, sizeof(NODEDATA_P_JOINED)+TailSize);
//	pNDJoined->dwNumOfBytes = TailSize;
//	pNDJoined->NodeBytes[0] = pChild->NodeByte;
//	pNDJoined->NodeBytes[1] = pNextChildList->NodeByte;
//	TreeCopyNode(&(pNDJoined->Node), &(pNextChildList->Node));
//	_HeapFree(pRTCtx->pHeapCtx, pNextNode->pNodeChilds);
//	}
//	else
//	{
//	pNDJoined = (NODEDATA_P_JOINED*)_realloc(pNextChildJoined, sizeof(NODEDATA_P_JOINED)+pNextChildJoined->dwNumOfBytes+1);
//	memmove(pNDJoined->NodeBytes+1, pNDJoined->NodeBytes, pNDJoined->dwNumOfBytes);
//	pNDJoined->dwNumOfBytes++;
//	pNDJoined->NodeBytes[0] = pChild->NodeByte;
//	}
//	*/
//	// --
//	if (pNextNode->N.NodeType.NodeTypeFields.NodeDataType == NDT_P_List)
//		TailSize = 1;
//	else
//		TailSize = pNextChildJoined->dwNumOfBytes;
//	TailSize++;
//	
//	error = _HeapAlloc2(pRTCtx->pHeapCtx, (void**)&pNDJoined, sizeof(NODEDATA_P_JOINED)+TailSize);
//	pNDJoined->dwNumOfBytes = TailSize;
//	
//	pNDJoined->NodeBytes[0] = pChild->NodeByte;
//	if (pNextNode->N.NodeType.NodeTypeFields.NodeDataType == NDT_P_List)
//	{
//		pNDJoined->NodeBytes[1] = pNextChildList->NodeByte;
//		TreeCopyNode(&(pNDJoined->Node), &(pNextChildList->Node));
//	}
//	else
//	{
//		for (i=1; i<TailSize; i++)
//		{
//			pNDJoined->NodeBytes[i] = pNextChildJoined->NodeBytes[i-1];
//		}
//		TreeCopyNode(&(pNDJoined->Node), &(pNextChildJoined->Node));
//	}
//	
//	_HeapFree(pRTCtx->pHeapCtx, pNextNode->NodeData.pNodeDataJoinedRT);
//	// -- 
//	_HeapFree(pRTCtx->pHeapCtx, pChild);
//	
//	pNode->N.NodeType.NodeTypeFields.NodeDataType = NDT_P_Joined;
//	pNode->NodeData.pNodeDataJoined = pNDJoined;
//	return 1;
//}		
//
// ------------------------------------------------------------------------------------------------
/*
TREE_ERR TreeJoinNodes2(sRT_CTX* pRTCtx, DNODE* pNode, uint32_t* pdwNodesJoined)
{
	TREE_ERR error = TREE_OK;
	DNODE* pCurrNode;
	NODEDATA_P_LIST* pNDList;
	NODEDATA_P_JOINED* pNDJoined;
	
	uint8_t NodeBytes[DBG_MAX_SIGNATURE_LEN]; 
	
	int i;
	int NodeBytesCount;
	
	if (pNode->N.NodeType.NodeTypeFields.NodeDataType != NDT_P_List || TreeGetNodeChildrenCount(pNode) != 1)
		return errNOT_OK;
	
	pNDList = pNode->NodeData.pNodeDataList;
	NodeBytes[0] = pNDList->NodeByte;
	NodeBytesCount = 1;
	
	pCurrNode = &(pNDList->Node);
	
	while (pCurrNode->N.NodeType.NodeTypeFields.NodeDataType==NDT_P_List && TreeGetNodeChildrenCount(pCurrNode)
		==1 && pCurrNode->SignatureID==0)
	{
		pNDList = pCurrNode->NodeData.pNodeDataList; 
		
		NodeBytes[NodeBytesCount] = pNDList->NodeByte;
		NodeBytesCount++;
		pCurrNode = &(pNDList->Node);
	}
	
	if (NodeBytesCount < 2)
		return errNOT_OK;
	
	error = _HeapAlloc2(pRTCtx->pHeapCtx, (void**)&pNDJoined, sizeof(NODEDATA_P_JOINED)+NodeBytesCount);
	if (TREE_FAILED(error))
		return error;
	pNDJoined->dwNumOfBytes = NodeBytesCount;
	for (i=0; i<NodeBytesCount; i++)
		pNDJoined->NodeBytes[i] = NodeBytes[i];
	TreeCopyNode(&(pNDJoined->Node), pCurrNode);
	
	// free chained blocks
	pNDList = pNode->NodeData.pNodeDataList;
	for (i=0; i<NodeBytesCount; i++)
	{
		NODEDATA_P_LIST* pNextChild;
		pNextChild = pNDList->Node.NodeData.pNodeDataList;
		_HeapFree(pRTCtx->pHeapCtx, pNDList);
		pNDList = pNextChild;
	}
	
	pNode->N.NodeType.NodeTypeFields.NodeDataType = NDT_P_Joined;
	pNode->NodeData.pNodeDataJoined = pNDJoined;
	*pdwNodesJoined += NodeBytesCount-1;
	return TREE_OK;
}		
*/
// ------------------------------------------------------------------------------------------------

TREE_ERR iTreeDeleteSignature(sRT_CTX* pRTCtx, DNODE* pNode, const uint8_t* pSignature, uint32_t dwSignatureSize, bool_t* pDeleteParent)
{
	TREE_ERR error;
	uint8_t  bSigByte;
	bool_t bDeleteParent;

	*pDeleteParent = 0;

	if (dwSignatureSize == 0)
	{
		if (pNode->SignatureID == 0)
			return TREE_ENOTFOUND;
//		ODS(("Deleting signature ID=%08X", pNode->SignatureID));
		pNode->SignatureID = 0;
		if ((pNode->pCondition == NULL) && (TreeGetNodeChildrenCount(pNode) == 0))
			*pDeleteParent = 1;
		return TREE_OK;
	}
	
	bSigByte = pSignature[0];
	if (bSigByte == SIGNATURE_ESC_CHAR)
	{
		if (pSignature[1] != SIGNATURE_ESC_CHAR)
		{
			DCONDITION* pCondition = pNode->pCondition;
			error = TREE_ENOTFOUND;
			while (pCondition != NULL)
			{
                          if (pCondition->C.ConditionType.nType == pSignature[1]
					&& pCondition->C.Data.Byte1 == pSignature[2]
					&& pCondition->C.Data.Byte2 == pSignature[3])
				{
					error = iTreeDeleteSignature(pRTCtx, &pCondition->Node, pSignature+4, dwSignatureSize-4, &bDeleteParent);
					if (TREE_SUCCEEDED(error))
					{
						if (bDeleteParent)
						{
							DCONDITION** ppCondition = &pNode->pCondition;
							while (*ppCondition)
							{
								if (*ppCondition == pCondition)
								{
									*ppCondition = pCondition->pNextCondition;
									_HeapFree(pRTCtx->pHeapCtx, pCondition);
									break;
								}
								ppCondition = &((*ppCondition)->pNextCondition);
							}
							*pDeleteParent = ((pNode->pCondition == NULL) && (TreeGetNodeChildrenCount(pNode) == 0) && pNode->SignatureID == 0);
						}
						return error; // Signature found 
					}
					break;
				}
				pCondition = pCondition->pNextCondition;
			}
			return error;
		}
	}


	switch (pNode->N.NodeType.NodeTypeFields.NodeDataType)
	{
	case NDT_P_List:
		{
			NODEDATA_P_LIST** ppListChild = &pNode->NodeData.pNodeDataList;
			while (*ppListChild != NULL) 
			{
				if ((*ppListChild)->NodeByte == bSigByte)
				{
					error = iTreeDeleteSignature(pRTCtx, &((*ppListChild)->Node), pSignature+1, dwSignatureSize - 1, &bDeleteParent);
					if (bDeleteParent)
					{
						NODEDATA_P_LIST* pListChildNext = (*ppListChild)->pNextChild;
						_HeapFree(pRTCtx->pHeapCtx, *ppListChild);
						*ppListChild = pListChildNext;
						if (pNode->pCondition == NULL && pNode->SignatureID == 0 && pNode->NodeData.pNodeDataList == NULL)
							*pDeleteParent = 1;
					}
					return error;
				}
				ppListChild = &((*ppListChild)->pNextChild);
			}
		}
		break;
	case NDT_P_Index32:
		{
			NODEDATA_P_INDEX32* pIndex;
			pIndex = pNode->NodeData.pNodeDataIndex; 
			if (pIndex->Index[bSigByte]!=NULL)
			{
				error = iTreeDeleteSignature(pRTCtx, pIndex->Index[bSigByte], pSignature+1, dwSignatureSize - 1, &bDeleteParent);
				if (bDeleteParent)
				{
					_HeapFree(pRTCtx->pHeapCtx, pIndex->Index[bSigByte]);
					pIndex->Index[bSigByte] = NULL;
					if (TreeGetNodeChildrenCount(pNode) == 0)
					{
						_HeapFree(pRTCtx->pHeapCtx, pNode->NodeData.pNodeDataIndex);
						pNode->NodeData.pNodeDataIndex = NULL;
						pNode->N.NodeType.NodeTypeFields.NodeType = NT_Dynamic;
						pNode->N.NodeType.NodeTypeFields.NodeDataType = NDT_P_List;
						if (pNode->pCondition == NULL && pNode->SignatureID == 0)
							*pDeleteParent = 1;
					}
				}
				return error;
			}
		}
		break;
	case NDT_P_Joined:
		if (pNode->NodeData.pNodeDataJoined->dwNumOfBytes <= dwSignatureSize)
		{
			uint32_t i;
			bool_t bExact = 1;

			for (i=0; i<pNode->NodeData.pNodeDataJoined->dwNumOfBytes; i++)
			{
				if (pNode->NodeData.pNodeDataJoined->NodeBytes[i] != pSignature[i])
				{
					bExact = 0;
					break;
				}
			}

			if (bExact)
			{
				error = iTreeDeleteSignature(pRTCtx, &pNode->NodeData.pNodeDataJoined->Node, pSignature+pNode->NodeData.pNodeDataJoined->dwNumOfBytes, dwSignatureSize - pNode->NodeData.pNodeDataJoined->dwNumOfBytes, &bDeleteParent);
				if (bDeleteParent)
				{
					_HeapFree(pRTCtx->pHeapCtx, pNode->NodeData.pNodeDataJoined);
					pNode->NodeData.pNodeDataJoined = NULL;
					pNode->N.NodeType.NodeTypeFields.NodeType = NT_Dynamic;
					pNode->N.NodeType.NodeTypeFields.NodeDataType = NDT_P_List;
					if (pNode->pCondition == NULL && pNode->SignatureID == 0)
						*pDeleteParent = 1;
				}
				return error;
			}
		}
		break;
	case NDT_P_JoinedRT:
		ODS(("iTreeDeleteSignature: Run-Time node in tree???"));
		DBG_STOP;
	case NDT_P_Hashed:
		if (pNode->NodeData.pNodeDataHashed->dwNumOfBytes <= dwSignatureSize)
		{
			uint32_t dwHashVal;
			
			dwHashVal = CountCRC32(pNode->NodeData.pNodeDataHashed->dwNumOfBytes, (uint8_t*) pSignature, 0);
			
			if (dwHashVal == pNode->NodeData.pNodeDataHashed->dwHashVal)
			{
				error = iTreeDeleteSignature(pRTCtx, &pNode->NodeData.pNodeDataHashed->Node, pSignature+pNode->NodeData.pNodeDataHashed->dwNumOfBytes, dwSignatureSize - pNode->NodeData.pNodeDataHashed->dwNumOfBytes, &bDeleteParent);
				if (bDeleteParent)
				{
					_HeapFree(pRTCtx->pHeapCtx, pNode->NodeData.pNodeDataHashed);
					pNode->N.NodeType.NodeTypeFields.NodeType = NT_Dynamic;
					pNode->N.NodeType.NodeTypeFields.NodeDataType = NDT_P_List;
					pNode->NodeData.pNodeDataList = NULL;
					if (pNode->pCondition == NULL && pNode->SignatureID == 0)
						*pDeleteParent = 1;
				}
				return error;
			}
		}
		break;
	case NDT_P_HashedRT:
		ODS(("iTreeDeleteSignature: Run-Time node in tree???"));
		DBG_STOP;
	default:
		DBG_STOP;
	}
	
	return TREE_ENOTFOUND;
}

TREE_ERR _TREE_CALL TreeDeleteSignature(sRT_CTX* pRTCtx, const uint8_t* pSignature, uint32_t dwSignatureSize)
{
	bool_t bDummy;

	if (pRTCtx->sTreeInfo.dwMagic == 0) // no data
		return TREE_ENOTFOUND;
	if (pRTCtx->sTreeInfo.dwFormat != cFORMAT_DYNAMIC)
		return TREE_ECORRUPT;
	if (pRTCtx->pRoot == NULL)
		return TREE_ENOTFOUND;

	return iTreeDeleteSignature(pRTCtx, PDNODE(pRTCtx->pRoot), pSignature, dwSignatureSize, &bDummy);
}

// ------------------------------------------------------------------------------------------------

TREE_ERR TreeGetNextNodeChild(sRT_CTX* pRTCtx, DNODE* pNode, DNODE** ppNextChild)
{
	TREE_ERR error = TREE_OK;
	
	DCONDITION* pCondition = pNode->pCondition;
	while (pCondition != NULL)
	{
		if (*ppNextChild == NULL)
		{
			*ppNextChild = &(pCondition->Node);
			return error;
		}
		else if (&(pCondition->Node) == *ppNextChild)
		{
			*ppNextChild = NULL;
		}
		pCondition = pCondition->pNextCondition;
	};
	
	switch(pNode->N.NodeType.NodeTypeFields.NodeDataType)
	{
	case NDT_P_List:
		error = TreeGetNextNodeChildList(pNode, ppNextChild);
		break;
	case NDT_P_Index32:
		error = TreeGetNextNodeChildIndexed(pNode, ppNextChild);
		break;
	case NDT_P_Joined:
		error = TreeGetNextNodeChildJoined(pRTCtx, pNode, ppNextChild);
		break;
	case NDT_P_JoinedRT:
		error = TreeGetNextNodeChildJoinedRT(pRTCtx, pNode, ppNextChild);
		break;
	case NDT_P_Hashed:
		error = TreeGetNextNodeChildHashed(pRTCtx, pNode, ppNextChild);
		break;
	case NDT_P_HashedRT:
		error = TreeGetNextNodeChildHashedRT(pRTCtx, pNode, ppNextChild);
		break;
	default:
		ODSN(("TreeGetNextNodeChild: Unsupported node type"));
		DBG_STOP;
		return TREE_EUNEXPECTED;
	}
	
	return error;
}

// ------------------------------------------------------------------------------------------------

TREE_ERR TreeGetNextNodeChildList(DNODE* pNode, DNODE** ppNextChild)
{
	NODEDATA_P_LIST* pChild;
	pChild = pNode->NodeData.pNodeDataList; 
	
	while (pChild != NULL)
	{
		if (*ppNextChild == NULL)
		{
			// found
			*ppNextChild = &(pChild->Node);
			return TREE_OK;
		}
		else if (*ppNextChild == &(pChild->Node))
		{
			// previous
			*ppNextChild = NULL;
		}
		pChild = pChild->pNextChild;
	};
	// not found
	*ppNextChild = NULL;
	return TREE_ENOTFOUND;
}

// ------------------------------------------------------------------------------------------------

TREE_ERR TreeGetNextNodeChildIndexed(DNODE* pNode, DNODE** ppNextChild)
{
	int i;
	NODEDATA_P_INDEX32* pIndex;
	pIndex = pNode->NodeData.pNodeDataIndex; 
	
	for (i=0; i<(sizeof(pIndex->Index)/sizeof(pIndex->Index[0])); i++)
	{
		if (pIndex->Index[i] != NULL)
		{
			if (*ppNextChild == NULL)
			{
				// found
				*ppNextChild = pIndex->Index[i];
				return TREE_OK;
			}
			else if (*ppNextChild == pIndex->Index[i])
			{
				// previous
				*ppNextChild = NULL;
			}
		}
	};
	// not found
	*ppNextChild = NULL;
	return TREE_ENOTFOUND;
}

// ------------------------------------------------------------------------------------------------

TREE_ERR TreeGetNextNodeChildJoined(sRT_CTX* pRTCtx, DNODE* pNode, DNODE** ppNewNode)
{
	DNODE* pJoinedRTNode;
	NODEDATA_P_JOINED_RT* pNDJoinedRT;
	NODEDATA_P_JOINED* pNDJoined;
	
	pNDJoined = pNode->NodeData.pNodeDataJoined;
	pJoinedRTNode = *ppNewNode;
	
	if (pJoinedRTNode == NULL)
	{
		if (TREE_FAILED(AllocJoinedRTNode(pRTCtx, &pJoinedRTNode, &pNDJoinedRT)))
			return TREE_ENOMEM;

		pJoinedRTNode->N.NodeType.NodeTypeFields.NodeType = NT_Dynamic;
		pJoinedRTNode->N.NodeType.NodeTypeFields.NodeDataType = NDT_P_JoinedRT;
		pJoinedRTNode->SignatureID = 0;
//		pJoinedRTNode->NodeData.pNodeDataJoinedRT = pNDJoinedRT;
		pNDJoinedRT->dwCurrentByte = 0;
		pNDJoinedRT->pParent.Node = pNode;
		*ppNewNode = pJoinedRTNode;
		return TREE_OK;
	}

	FreeJoinedRTNode(pRTCtx, pJoinedRTNode);
	*ppNewNode = NULL;
	return TREE_ENOTFOUND;
}


// ------------------------------------------------------------------------------------------------

TREE_ERR TreeGetNextNodeChildJoinedRT(sRT_CTX* pRTCtx, DNODE* pNode, DNODE** ppNextChild)
{
	NODEDATA_P_JOINED_RT* pNDJoinedRT;
	NODEDATA_P_JOINED* pNDJoined;
	uint32_t dwCurrByte;
	
	// RT nodes: node data follows DNODE structure
	pNDJoinedRT = (NODEDATA_P_JOINED_RT *)(pNode + 1);
	pNDJoined = pNDJoinedRT->pParent.Node->NodeData.pNodeDataJoined;
	dwCurrByte = pNDJoinedRT->dwCurrentByte + 1;
	
	if (pNDJoinedRT->pParent.Node->N.NodeType.NodeTypeFields.NodeDataType != NDT_P_Joined)
	{
		ODSN(("Oops: pTNode->pParent.Node->N.NodeType.NodeTypeFields.NodeDataType != NDT_P_Joined"));
		DBG_STOP
	}
	if (dwCurrByte >= pNDJoined->dwNumOfBytes)
	{
		ODSN(("Oops: (pNDJoined->dwNumOfBytes <= pNDJoinedRT->dwCurrentByte)"));
		DBG_STOP
	}
	
	if (*ppNextChild == NULL)
	{
		DNODE* pNewTempNode;
		NODEDATA_P_JOINED_RT* pNewTempData;

		if (pNDJoined->dwNumOfBytes == dwCurrByte + 1) // last byte in array
		{
			*ppNextChild = &(pNDJoined->Node);
			return TREE_OK;
		}
		
		if (TREE_FAILED(AllocJoinedRTNode(pRTCtx, &pNewTempNode, &pNewTempData)))
			return TREE_ENOMEM;
		
		pNewTempNode->N.NodeType.NodeTypeFields.NodeType = NT_Dynamic;
		pNewTempNode->N.NodeType.NodeTypeFields.NodeDataType = NDT_P_JoinedRT;
		pNewTempNode->SignatureID = 0;
//		pNewTempNode->NodeData.pNodeDataJoinedRT = pNewTempData;
		pNewTempData->dwCurrentByte = dwCurrByte;
		pNewTempData->pParent.Node = pNDJoinedRT->pParent.Node;
		*ppNextChild = pNewTempNode;
		return TREE_OK;
	}
	
	if ((*ppNextChild)->N.NodeType.NodeTypeFields.NodeDataType == NDT_P_JoinedRT)
	{
		FreeJoinedRTNode(pRTCtx, *ppNextChild);
	}
	
	*ppNextChild = NULL;
	return TREE_ENOTFOUND;
}

// ------------------------------------------------------------------------------------------------

TREE_ERR TreeGetNextNodeChildHashed(sRT_CTX* pRTCtx, DNODE* pNode, DNODE** ppNextChild)
{
	DNODE* pHashedRTNode;
	NODEDATA_P_HASHED_RT* pNDHashedRT;
	NODEDATA_P_HASHED* pNDHashed;
	
	pNDHashed = pNode->NodeData.pNodeDataHashed;
	pHashedRTNode = *ppNextChild;
	
	if (pHashedRTNode == NULL)
	{
		if (TREE_FAILED(AllocHashedRTNode(pRTCtx, &pHashedRTNode, &pNDHashedRT)))
			return TREE_ENOMEM;
		
		pHashedRTNode->N.NodeType.NodeTypeFields.NodeType = NT_Dynamic;
		pHashedRTNode->N.NodeType.NodeTypeFields.NodeDataType = NDT_P_HashedRT;
		pHashedRTNode->SignatureID = 0;
//		pHashedRTNode->NodeData.pNodeDataHashedRT = pNDHashedRT;
		pNDHashedRT->dwCurrentByte = 0;
		pNDHashedRT->pParent.Node = pNode;
		*ppNextChild = pHashedRTNode;
		return TREE_OK;
	}
	FreeHashedRTNode(pRTCtx, pHashedRTNode);
	*ppNextChild = NULL;
	return TREE_ENOTFOUND;
}

// ------------------------------------------------------------------------------------------------

TREE_ERR TreeGetNextNodeChildHashedRT(sRT_CTX* pRTCtx, DNODE* pNode, DNODE** ppNextChild)
{
	NODEDATA_P_HASHED_RT* pNDHashedRT;
	NODEDATA_P_HASHED* pNDHashed;
	
	// RT nodes: node data follows DNODE structure
	pNDHashedRT = (NODEDATA_P_HASHED_RT *)(pNode + 1);
	pNDHashed = pNDHashedRT->pParent.Node->NodeData.pNodeDataHashed;
	
	if (pNDHashedRT->pParent.Node->N.NodeType.NodeTypeFields.NodeDataType != NDT_P_Hashed)
	{
		ODSN(("Oops: pNDHashedRT->pParent.Node->N.NodeType.NodeTypeFields.NodeDataType != NDT_P_Hashed"));
	}
	
	if (*ppNextChild == NULL)
	{
		*ppNextChild = &(pNDHashed->Node);
		return TREE_OK;
	}
	
	if ((*ppNextChild)->N.NodeType.NodeTypeFields.NodeDataType == NDT_P_HashedRT)
	{
		FreeHashedRTNode(pRTCtx, *ppNextChild);
	}
	
	*ppNextChild = NULL;
	return TREE_ENOTFOUND;
}
// ------------------------------------------------------------------------------------------------

TREE_ERR iTreeIsSignature(sRT_CTX* pRTCtx, DNODE* pNode, const uint8_t* pSignature, uint32_t dwSignatureSize, tSIGNATURE_ID* result) 
{
	TREE_ERR error = TREE_OK;
	uint32_t dwPosInSig = 0;
	uint8_t  bSigByte;
	
	while (dwPosInSig < dwSignatureSize)
	{
		bSigByte = pSignature[dwPosInSig];
		if (bSigByte == SIGNATURE_ESC_CHAR)
		{
			dwPosInSig++;
			if (pSignature[dwPosInSig] != SIGNATURE_ESC_CHAR)
			{
				DCONDITION* pCondition = pNode->pCondition;
				while (pCondition != NULL)
				{
					if (pCondition->C.ConditionType.nType == pSignature[dwPosInSig]
						&& pCondition->C.Data.Byte1 == pSignature[dwPosInSig+1]
						&& pCondition->C.Data.Byte2 == pSignature[dwPosInSig+2])
					{
						//error = iTreeIsSignature(pRTCtx, &pCondition->Node, pSignature+(dwPosInSig+3), dwSignatureSize-(dwPosInSig+3), result);
						//if (TREE_SUCCEEDED(error))
						//	return error; // Signature found
						//break;
						pNode = &pCondition->Node;
						dwPosInSig+=3;
						break;
					}
					pCondition = pCondition->pNextCondition;
				}
				if (pCondition) // right condition found
					continue;
				error = TREE_ENOTFOUND;
				break;
			}
		}
		pNode = PDNODE (TreeGetNextNode(PNODE(pNode), bSigByte, pRTCtx));
		dwPosInSig++;
		if (pNode == NULL)
		{
			error = TREE_ENOTFOUND;
			break;
		}
	}
	
	if (TREE_SUCCEEDED(error))
	{
		if (pNode != NULL && result != NULL)
			*result = pNode->SignatureID;
	}

	FreeRTNode(pRTCtx, pNode);

	return error;
}

TREE_ERR _TREE_CALL TreeIsSignature(sRT_CTX* pRTCtx, const uint8_t* pSignature, uint32_t dwSignatureSize, tSIGNATURE_ID* result) 
{
	if (pRTCtx->sTreeInfo.dwMagic == 0) // no data
		return TREE_ENOTFOUND;
	if (pRTCtx->sTreeInfo.dwFormat != cFORMAT_DYNAMIC)
		return TREE_ECORRUPT;
	if (pRTCtx->pRoot == NULL)
		return TREE_ENOTFOUND;

	return iTreeIsSignature(pRTCtx, PDNODE(pRTCtx->pRoot), pSignature, dwSignatureSize, result);
}

// ------------------------------------------------------------------------------------------------

#endif // TREE_STATIC_ONLY

