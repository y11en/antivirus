// loaddyn.c
//

#include "common.h"
#include "savedyn.h"
#include "crc32.h"

#ifndef TREE_STATIC_ONLY

//#define USE_HASHED_IN_DYNAMIC

// ------------------------------------------------------------------------------------------------

#ifndef min 
#define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif

TREE_ERR TreeLoadNode(sRT_CTX* pRTCtx, DNODE* pNode, sIO_CTX* pIOCtx, uint32_t dwLevel);

// ------------------------------------------------------------------------------------------------

TREE_ERR TreeLoadNodeItself(sRT_CTX* pRTCtx, DNODE* pNode, uint8_t* pbBitFlags, sIO_CTX* pIOCtx)
{
	TREE_ERR error = TREE_OK;
	NODEFLAGS nf;
	
	error = _Read(pIOCtx, &nf, sizeof(nf));
	RET_ON_FAIL;

	pNode->N.NodeType.NodeTypeFields.NodeType = NT_Dynamic;
	pNode->N.NodeType.NodeTypeFields.NodeDataType = (enum NodeTypes)nf.bNodeType;

	if (nf.bCondition)
		pNode->pCondition = (DCONDITION*)1;
	if (nf.bNodeDataPtr)
		pNode->NodeData.pNodeDataList = (NODEDATA_P_LIST*)1;
	if (nf.bSignatureID)
		error = _Read(pIOCtx, &pNode->SignatureID, sizeof(pNode->SignatureID));
	RET_ON_FAIL;

	if (pbBitFlags != NULL)
	{
		*pbBitFlags = 0;
		if (nf.bReserved1)
			*pbBitFlags |= (1<<0);
		if (nf.bReserved2)
			*pbBitFlags |= (1<<1);
	}
	
	return error;	
}

// ------------------------------------------------------------------------------------------------

TREE_ERR _INLINE TreeLoadNodeDataList(sRT_CTX* pRTCtx, DNODE* pNode, sIO_CTX* pIOCtx, uint32_t dwLevel)
{
	TREE_ERR error = TREE_OK;
	NODEDATA_P_LIST* pCNode = NULL;

//	_Write(pIOCtx, "<C>>", sizeof(uint32_t));
	if (pNode->NodeData.pNodeDataList != NULL)
	{
		error = _HeapAlloc2(pRTCtx->pHeapCtx, (void**)&pNode->NodeData.pNodeDataList, sizeof(NODEDATA_P_LIST));
		RET_ON_FAIL;
	}		

	pCNode = pNode->NodeData.pNodeDataList;
	while (pCNode!=NULL)
	{
		uint8_t bBitFlags = 0;

		error = _Read(pIOCtx, &pCNode->NodeByte, sizeof(pCNode->NodeByte));
		RET_ON_FAIL;

		error = TreeLoadNodeItself(pRTCtx, &pCNode->Node, &bBitFlags, pIOCtx);
		RET_ON_FAIL;

		if ((bBitFlags & (1<<0)) != 0) // node has child
		{
			error = _HeapAlloc2(pRTCtx->pHeapCtx, (void**)&pCNode->pNextChild, sizeof(NODEDATA_P_LIST));
			RET_ON_FAIL;
		}

		pCNode = pCNode->pNextChild;
	}
	
	pCNode = pNode->NodeData.pNodeDataList;
	while (pCNode!=NULL)
	{
		error = TreeLoadNode(pRTCtx, &(pCNode->Node), pIOCtx, dwLevel+1);
		RET_ON_FAIL;

		pCNode = pCNode->pNextChild;
	}
	

	return error;
}

// ------------------------------------------------------------------------------------------------

TREE_ERR _INLINE TreeLoadNodeDataJoined(sRT_CTX* pRTCtx, DNODE* pNode, sIO_CTX* pIOCtx, uint32_t dwLevel)
{
	TREE_ERR error = TREE_OK;
	uint8_t bBitFlags = 0;
	uint8_t bNumOfBytes;
	uint32_t dwNumOfBytes;
	DNODE sNode;
	uint32_t dwLoadAsJoined;
	uint32_t dwLoadAsHashed;
	
	//	_Write(pIOCtx, "<J>>", sizeof(uint32_t));

	memset(&sNode, 0, sizeof(sNode));

	error = TreeLoadNodeItself(pRTCtx, &sNode, &bBitFlags, pIOCtx);
	RET_ON_FAIL;

	if ((bBitFlags & (1<<0)) != 0)
	{
		// short (uint8_t) data len
		error = _Read(pIOCtx, &bNumOfBytes, sizeof(uint8_t));
		dwNumOfBytes = (uint32_t)bNumOfBytes;
	}
	else
	{
		// long (uint32_t) data len
		error = _Read(pIOCtx, &dwNumOfBytes , sizeof(uint32_t));
	}
	RET_ON_FAIL;



	dwLoadAsJoined = dwNumOfBytes;
	dwLoadAsHashed = 0;
	
#ifdef USE_HASHED_IN_DYNAMIC
	if (pRTCtx->sTreeInfo.dwHashLevel && dwLevel >= pRTCtx->sTreeInfo.dwHashLevel)
	{
		dwLoadAsJoined = 0;
		dwLoadAsHashed = dwNumOfBytes;
	}
	else 
	{
		dwLoadAsJoined = min(dwNumOfBytes, pRTCtx->sTreeInfo.dwHashLevel - dwLevel);
		dwLoadAsHashed = dwNumOfBytes - dwLoadAsJoined;
	}

	if (dwLoadAsJoined < 2 && dwLevel < pRTCtx->sTreeInfo.dwHashLevel)
	{
		if (dwLoadAsHashed > 2)
		{
			dwLoadAsJoined += 2;
			dwLoadAsHashed -= 2;
		}
		else
		{
			dwLoadAsJoined += dwLoadAsHashed;
			dwLoadAsHashed = 0;
		}
	}

	if (dwLoadAsHashed <= sizeof(NODEDATA_P_HASHED) - sizeof(NODEDATA_P_JOINED))
	{
		dwLoadAsJoined += dwLoadAsHashed;
		dwLoadAsHashed = 0;
	}
#endif

	if (dwLoadAsJoined)
	{
		error = _HeapAlloc2(pRTCtx->pHeapCtx, (void**)&pNode->NodeData.pNodeDataJoined, sizeof(NODEDATA_P_JOINED)+dwLoadAsJoined);
		RET_ON_FAIL;
		pNode->NodeData.pNodeDataJoined->dwNumOfBytes = dwLoadAsJoined;
		if (pNode->NodeData.pNodeDataJoined->dwNumOfBytes < 2)
		{
			DBG_STOP;
		}
		error = _Read(pIOCtx, &pNode->NodeData.pNodeDataJoined->NodeBytes, dwLoadAsJoined);
		RET_ON_FAIL;

		if (dwLevel<6 && pNode->NodeData.pNodeDataJoined->dwNumOfBytes>10-dwLevel)
		{
//			error = TreeSplitJoinedNode(pRTCtx, pNode, 8-dwLevel, &pNode);
//			RET_ON_FAIL;
//			if (pNode->NodeType.NodeTypeFields.NodeDataType != NDT_P_List)
//				DBG_STOP;
//			pNode = &pNode->NodeData.pNodeDataList->Node;
			/*
			uint32_t i;
			for (i=dwLevel; i<6; i++)
			{
				error = TreeSplitJoinedNode(pRTCtx, pNode, 0, &pNode);
				RET_ON_FAIL;
				if (pNode->NodeType.NodeTypeFields.NodeDataType != NDT_P_List)
					DBG_STOP;
				pNode = &pNode->NodeData.pNodeDataList->Node;
				if (pNode->NodeType.NodeTypeFields.NodeDataType != NDT_P_Joined)
					break;
			}
			*/
		}

		if (pNode->N.NodeType.NodeTypeFields.NodeDataType == NDT_P_List)
			pNode = &pNode->NodeData.pNodeDataList->Node;
		else
		if (pNode->N.NodeType.NodeTypeFields.NodeDataType == NDT_P_Joined)
			pNode = &pNode->NodeData.pNodeDataJoined->Node;
		else
			DBG_STOP;
	}

	if (dwLoadAsHashed)
	{
		uint8_t* pData;

		pNode->N.NodeType.NodeTypeFields.NodeDataType = NDT_P_Hashed;
		error = _HeapAlloc2(pRTCtx->pHeapCtx, (void**)&pNode->NodeData.pNodeDataHashed, sizeof(NODEDATA_P_HASHED));
		RET_ON_FAIL;
		
		pNode->NodeData.pNodeDataHashed->dwNumOfBytes = dwLoadAsHashed;

		error = _HeapAlloc2(pRTCtx->pHeapCtx, (void**)&pData, dwLoadAsHashed);
		RET_ON_FAIL;
		error = _Read(pIOCtx, pData, dwLoadAsHashed);
		RET_ON_FAIL;
		pNode->NodeData.pNodeDataHashed->dwHashVal = CountCRC32(dwLoadAsHashed, pData, 0);
		_HeapFree(pRTCtx->pHeapCtx, pData);
		pNode = PDNODE (&pNode->NodeData.pNodeDataHashed->Node);
	}

	memcpy(pNode, &sNode, sizeof(NODE));
	
	if ((bBitFlags & (1<<1)) == 0)
	{
		// child node is not tail node
		error = TreeLoadNode(pRTCtx, pNode, pIOCtx, dwLevel+dwNumOfBytes);
	}

	return error;
}

// ------------------------------------------------------------------------------------------------

TREE_ERR _INLINE TreeLoadNodeDataHashed(sRT_CTX* pRTCtx, DNODE* pNode, sIO_CTX* pIOCtx, uint32_t dwLevel)
{
	TREE_ERR error = TREE_OK;
	uint8_t bBitFlags = 0;
	uint8_t bNumOfBytes;
	NODEDATA_P_HASHED* pNDHashed = NULL;

	error = _HeapAlloc2(pRTCtx->pHeapCtx, (void**)&pNode->NodeData.pNodeDataHashed, sizeof(NODEDATA_P_HASHED));
	RET_ON_FAIL;

	pNDHashed = pNode->NodeData.pNodeDataHashed;

	error = TreeLoadNodeItself(pRTCtx, &pNDHashed->Node, &bBitFlags, pIOCtx);
	RET_ON_FAIL;

	if ((bBitFlags & (1<<0)) != 0)
	{
		// short (uint8_t) data len
		error = _Read(pIOCtx, &bNumOfBytes, sizeof(bNumOfBytes));
		pNDHashed->dwNumOfBytes = (uint32_t)bNumOfBytes;
	}
	else
	{
		// long (uint32_t) data len
		error = _Read(pIOCtx, &pNDHashed->dwNumOfBytes, sizeof(pNDHashed->dwNumOfBytes));
	}
	RET_ON_FAIL;

	error = _Read(pIOCtx, &pNDHashed->dwHashVal, sizeof(pNDHashed->dwHashVal));
	RET_ON_FAIL;
	
	if ((bBitFlags & (1<<1)) == 0)
	{
		// child node is not tail node
		error = TreeLoadNode(pRTCtx, &(pNDHashed->Node), pIOCtx, dwLevel+pNDHashed->dwNumOfBytes);
	}
	/*
	else
	{
		// child node is tail node
		pNDHashed->Node.NodeType.NodeTypeFields.NodeDataType = NDT_P_List;
		error = _Read(pIOCtx, &pNDHashed->Node.SignatureID, sizeof(pNDHashed->Node.SignatureID));
	}
	*/
	return error;
}

// ------------------------------------------------------------------------------------------------

TREE_ERR _INLINE TreeLoadNodeDataIndexed(sRT_CTX* pRTCtx, DNODE* pNode, sIO_CTX* pIOCtx, uint32_t dwLevel)
{

	TREE_ERR error = TREE_OK;
	uint32_t i;
	uint32_t dwChildrenCount = 0;
	uint8_t bIndexStoreType = cFull;
	uint32_t dw = 0;
	NODEDATA_P_INDEX32* pIndex = NULL;
		
//	_Write(pIOCtx, "<I>>", sizeof(uint32_t));
	if (pNode->NodeData.pNodeDataIndex!=NULL)
	{
		error = _HeapAlloc2(pRTCtx->pHeapCtx, (void**)&pNode->NodeData.pNodeDataIndex, sizeof(NODEDATA_P_INDEX32));
		RET_ON_FAIL
	}

	pIndex = pNode->NodeData.pNodeDataIndex;

	error = _Read(pIOCtx, &bIndexStoreType, sizeof(bIndexStoreType));
	
	switch(bIndexStoreType)
	{
	case cFull:
		error = _Read(pIOCtx, pIndex, sizeof(NODEDATA_P_INDEX32));
		break;
	case cBitmap:
		for (i=0; i<(sizeof(pIndex->Index)/sizeof(pIndex->Index[0]));i++)
		{
			if (i%32 == 0)
			{
				error = _Read(pIOCtx, &dw, sizeof(uint32_t));
				RET_ON_FAIL;
			}
			if (dw & 0x80000000)
			{
				error = _HeapAlloc2(pRTCtx->pHeapCtx, (void**)&pIndex->Index[i], sizeof(NODE));
				RET_ON_FAIL;
			}
			dw <<= 1;
		}
		break;
	case cArray:
		error = _Read(pIOCtx, (uint8_t*)&dwChildrenCount, sizeof(uint8_t));
		RET_ON_FAIL;
		for (i=0; i<dwChildrenCount; i++)
		{
			uint8_t bNodeIndex;
			error = _Read(pIOCtx, (uint8_t*)&bNodeIndex, sizeof(uint8_t));
			RET_ON_FAIL;
			error = _HeapAlloc2(pRTCtx->pHeapCtx, (void**)&pIndex->Index[bNodeIndex], sizeof(NODE));
			RET_ON_FAIL;
		}
		break;
	}

	for (i=0; i<(sizeof(pIndex->Index)/sizeof(pIndex->Index[0]));i++)
	{
		if (pIndex->Index[i] != NULL)
		{
			error = TreeLoadNodeItself(pRTCtx, pIndex->Index[i], NULL, pIOCtx);
			RET_ON_FAIL;
		}
	}

	for (i=0; i<(sizeof(pIndex->Index)/sizeof(pIndex->Index[0]));i++)
	{
		if (pIndex->Index[i] != NULL)
		{
			error = TreeLoadNode(pRTCtx, pIndex->Index[i], pIOCtx, dwLevel+1);
			RET_ON_FAIL;
		}
	}

	return error;
}

// ------------------------------------------------------------------------------------------------

TREE_ERR _INLINE TreeLoadNodeConditions(sRT_CTX* pRTCtx, DNODE* pNode, sIO_CTX* pIOCtx, uint32_t dwLevel)
{
	TREE_ERR error = TREE_OK;
	DCONDITION* pCondition = NULL;
	uint8_t bBitFlags = 0;

	if (pNode->pCondition != NULL)
	{
		error = _HeapAlloc2(pRTCtx->pHeapCtx, (void**)&pNode->pCondition, sizeof(DCONDITION));
		RET_ON_FAIL;
	}
	
	pCondition = pNode->pCondition;
	while (pCondition!=NULL)
	{
		error = TreeLoadNodeItself(pRTCtx, &pCondition->Node, &bBitFlags, pIOCtx); 
		RET_ON_FAIL;

		error = _Read(pIOCtx, &pCondition->C.ConditionType, sizeof(pCondition->C.ConditionType));
		RET_ON_FAIL;
		
		error = _Read(pIOCtx, &pCondition->C.Data, sizeof(pCondition->C.Data));
		RET_ON_FAIL;

		if ((bBitFlags & (1<<0)) != 0)
		{
			error = _HeapAlloc2(pRTCtx->pHeapCtx, (void**)&pCondition->pNextCondition, sizeof(DCONDITION));
			RET_ON_FAIL;
		}

		pCondition = pCondition->pNextCondition;
	}
	
	pCondition = pNode->pCondition;
	while (pCondition!=NULL)
	{
		error = TreeLoadNode(pRTCtx, &(pCondition->Node), pIOCtx, dwLevel);
		RET_ON_FAIL;

		pCondition = pCondition->pNextCondition;
	}

	return error;
}

// ------------------------------------------------------------------------------------------------

TREE_ERR _INLINE TreeLoadNode(sRT_CTX* pRTCtx, DNODE* pNode, sIO_CTX* pIOCtx, uint32_t dwLevel)
{
	TREE_ERR error = TREE_OK;

//	uint64_t qPos;
//	CALL_SeqIO_Seek(pIOCtx, &qPos, 0, SEEK_CUR);

//	dwLevel++;

	error = TreeLoadNodeConditions(pRTCtx, pNode, pIOCtx, dwLevel);

	switch(pNode->N.NodeType.NodeTypeFields.NodeDataType)
	{
	case NDT_P_List:
		error = TreeLoadNodeDataList(pRTCtx, pNode, pIOCtx, dwLevel);
		break;
	case NDT_P_Index32:
		error = TreeLoadNodeDataIndexed(pRTCtx, pNode, pIOCtx, dwLevel);
		break;
	case NDT_P_Joined:
		error = TreeLoadNodeDataJoined(pRTCtx, pNode, pIOCtx, dwLevel);
		break;
	case NDT_P_JoinedRT:
		ODS(("TreeSaveNode: Run-Time node in tree???"));
		DBG_STOP;
		break;
	case NDT_P_Hashed:
		error = TreeLoadNodeDataHashed(pRTCtx, pNode, pIOCtx, dwLevel);
		break;
	case NDT_P_HashedRT:
		ODS(("TreeSaveNode: Run-Time node in tree???"));
		DBG_STOP;
		break;
	default:
		ODSN(("TreeLoadNode: Unsupported node type"));
		DBG_STOP;
		return TREE_EUNEXPECTED;
	}
	return error;
}

// ------------------------------------------------------------------------------------------------

TREE_ERR _TREE_CALL TreeLoad_Dynamic(sRT_CTX* pRTCtx, sIO_CTX* pIOCtx)
{
	TREE_ERR error = TREE_OK;
	uint64_t qwStartPos;
	TREE_INFO sTreeInfo;

	if (TREE_FAILED(error = TreeUnload_Any(pRTCtx)))
		return error;

	if (TREE_FAILED(error = _Seek(pIOCtx, &qwStartPos, 0, TREE_IO_CTX_SEEK_CUR)))
		return error;
	
	if (TREE_FAILED(error = _Read(pIOCtx, (uint8_t*)&sTreeInfo, sizeof(sTreeInfo))))
		return error;

	// older formats support 
	if (sTreeInfo.dwMagic == MAGIC_TREE_SST_V1 && sTreeInfo.dwFormat==0) 
	{
		sTreeInfo.dwMagic = MAGIC_TREE_DB_V2;
		sTreeInfo.dwFormat = cFORMAT_DYNAMIC;
		if (sTreeInfo.MaxSignatureID)
			sTreeInfo.MinSignatureID = 1;
	}

	if (sTreeInfo.dwMagic != MAGIC_TREE_DB_V2 || sTreeInfo.dwFormat != cFORMAT_DYNAMIC)
		return TREE_ECORRUPT;

	error = TreeCreateRootNode(pRTCtx);
	if (TREE_SUCCEEDED(error))
		error = TreeLoadNodeItself(pRTCtx, PDNODE(pRTCtx->pRoot), NULL, pIOCtx);
	if (TREE_SUCCEEDED(error))
		error = TreeLoadNode(pRTCtx, PDNODE(pRTCtx->pRoot), pIOCtx, 0);

	if (TREE_FAILED(error))
	{
		_Seek(pIOCtx, NULL, qwStartPos, TREE_IO_CTX_SEEK_SET); // restore IO position in case of error
		pRTCtx->pRoot = NULL; // memory leak here, because cannot free memory correctly
	}
	else
		memcpy(&pRTCtx->sTreeInfo, &sTreeInfo, sizeof(sTreeInfo));

	return error;
}

// ------------------------------------------------------------------------------------------------

TREE_ERR _TREE_CALL TreeLoad_Any(sRT_CTX* pRTCtx, sIO_CTX* pIOCtx)
{
	TREE_ERR error;
	uint64_t qwStartPos;
	TREE_INFO sTreeInfo;

	if (TREE_FAILED(error = TreeUnload_Any(pRTCtx)))
		return error;

	if (TREE_FAILED(error = _Seek(pIOCtx, &qwStartPos, 0, TREE_IO_CTX_SEEK_CUR)))
		return error;
	
	if (TREE_FAILED(error = _SeekRead(pIOCtx, qwStartPos, (uint8_t*)&sTreeInfo, sizeof(sTreeInfo))))
		return error;

	// older formats support 
	if (sTreeInfo.dwMagic == MAGIC_TREE_SST_V1 && sTreeInfo.dwFormat==0) 
	{
		sTreeInfo.dwMagic = MAGIC_TREE_DB_V2;
		sTreeInfo.dwFormat = cFORMAT_DYNAMIC;
		if (sTreeInfo.MaxSignatureID)
			sTreeInfo.MinSignatureID = 1;
	}

	if (sTreeInfo.dwMagic != MAGIC_TREE_DB_V2)
		return TREE_ECORRUPT;

	switch (sTreeInfo.dwFormat)
	{
		case cFORMAT_DYNAMIC:
			error = TreeLoad_Dynamic(pRTCtx, pIOCtx);
			break;
		case cFORMAT_STATIC:
			error = TreeLoad_Static(pRTCtx, pIOCtx);
			break;
		default:
			error = TREE_ECORRUPT;
			break;
	}

	return error;
}


#endif // TREE_STATIC_ONLY
